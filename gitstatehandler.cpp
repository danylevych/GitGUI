#include "gitstatehandler.h"
#include "qdebug.h"

#include <QProcess>
#include <QRegularExpression>

////////////////////////////////////////////////////////////////////////
QString GitStateHandler::errorMsg = QString();

GitStateHandler::GitStateHandler(const QString& dir)
    : next(nullptr)
    , dir(dir)
{   }

void GitStateHandler::SetNext(std::shared_ptr<GitStateHandler> next)
{
    this->next = next;
}

void GitStateHandler::SetDir(const QString &dir)
{
    this->dir = dir;
}

QString GitStateHandler::GetErrorMsg() const
{
    return errorMsg;
}

bool GitStateHandler::RunGitCommand(const QString &command)
{
    QProcess process;
    process.setWorkingDirectory(dir);

    qDebug() << dir;
    qDebug() << "Running command: " << command;

    // Regular expression to split the comand for arguments.
    QRegularExpression reg("(\"[^\"]*\"|[^\\s\"]+)");

    QStringList gitArguments;
    auto it = reg.globalMatch(command);
    while (it.hasNext())
    {
        auto match = it.next();
        gitArguments << match.captured(1);
    }

    process.start("git", gitArguments);
    process.waitForFinished();

    qDebug() << "Command output: " << process.readAllStandardOutput();
    qDebug() << "Command errors: " << process.readAllStandardError();

    return process.exitCode() == 0;
}

////////////////////////////////////////////////////////////////////////

GitInitStateHandler::GitInitStateHandler(const QString &dir)
    : GitStateHandler(dir)
{   }

bool GitInitStateHandler::Handle(GitStates& state)
{
    if (state == GitStates::None)
    {
        if (RunGitCommand("init"))
        {
            state = GitStates::Init;
            errorMsg = "";
            return true;
        }
    }
    else
    {
        if (next)
        {
            return next->Handle(state);
        }
    }

    state = GitStates::None;
    errorMsg = "Could not execute 'git init'";
    return false;
}

////////////////////////////////////////////////////////////////////////

GitAddStateHandler::GitAddStateHandler(const QString &dir)
    : GitStateHandler(dir)
{   }


bool GitAddStateHandler::Handle(GitStates& state)
{
    if (state == GitStates::Init)
    {
        if (RunGitCommand("add ."))
        {
            state = GitStates::Add;
            errorMsg = "";
            return true;
        }
    }
    else
    {
        if (next)
        {
            return next->Handle(state);
        }
    }

    errorMsg = "Could not execute 'git add'";
    return false;
}

////////////////////////////////////////////////////////////////////////

GitCommitStateHandler::GitCommitStateHandler(const QString &dir)
    : GitStateHandler(dir)
    , commitMsg()
{   }

bool GitCommitStateHandler::Handle(GitStates &state)
{
    if (state == GitStates::Add)
    {
        if (RunGitCommand("commit -m \"" + commitMsg + "\""))
        {
            state = GitStates::Commit;
            errorMsg = "";
            return true;
        }
    }
    else
    {
        if (next)
        {
            return next->Handle(state);
        }
    }

    errorMsg = "Could not execute 'git commit'";
    return false;
}

void GitCommitStateHandler::SetCommitMsg(const QString &commitMsg)
{
    this->commitMsg = commitMsg;
}

////////////////////////////////////////////////////////////////////////

GitRemoteStateHandler::GitRemoteStateHandler(const QString &dir)
    : GitStateHandler(dir)
    , remoteBranch()
{   }

bool GitRemoteStateHandler::Handle(GitStates &state)
{
    if (state == GitStates::Commit && IsRemoteBranch()) // We have remote branch.
    {
        state = static_cast<GitStates>(state | GitStates::Remote);
    }

    if (state == GitStates::Commit)
    {
        if (RunGitCommand("remote add origin " + remoteBranch))
        {
            state = static_cast<GitStates>(state | GitStates::Remote);
            errorMsg = "";
            return true;
        }
    }
    else
    {
        if (next)
        {
            return next->Handle(state);
        }
    }

    errorMsg = "Could not execute 'git remote'";
    return false;
}

void GitRemoteStateHandler::SetRemoteBranch(const QString &remoteBranch)
{
    this->remoteBranch = remoteBranch;
}

bool GitRemoteStateHandler::IsRemoteBranch()
{
    QProcess process;
    process.setWorkingDirectory(dir);

    process.start("git", QStringList() << "remote");
    process.waitForFinished();

    QByteArray output = process.readAllStandardOutput();
    return QString::fromUtf8(output).trimmed() == "origin";
}

////////////////////////////////////////////////////////////////////////

GitPushStateHandler::GitPushStateHandler(const QString &dir)
    : GitStateHandler(dir)
    , branch()
{   }

bool GitPushStateHandler::Handle(GitStates &state)
{
    // Only if we have commited changes and remote branch we can make a push.
    if (state & (GitStates::Commit | GitStates::Remote))
    {
        auto name = GetBranchName();

        if (RunGitCommand("push -u origin " + name))
        {
            state = static_cast<GitStates>(state | GitStates::Push);
            errorMsg = "";
            return true;
        }
    }
    else
    {
        if (next)
        {
            return next->Handle(state);
        }
    }

    errorMsg = "Could not execute 'git push'";
    return false;
}

QString GitPushStateHandler::GetBranchName()
{
    QProcess process;
    process.setWorkingDirectory(dir);

    process.start("git", QStringList() << "rev-parse" << "--abbrev-ref" << "HEAD");
    process.waitForFinished();

    QByteArray output = process.readAllStandardOutput();
    return QString::fromUtf8(output).trimmed();
}

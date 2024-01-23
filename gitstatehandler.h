#ifndef GITSTATEHANDLER_H
#define GITSTATEHANDLER_H

#include <memory>
#include <QString>

#include "GitStates.h"

//////////////////////////////////////////////////////////////////////

class GitStateHandler
{
protected:
    std::shared_ptr<GitStateHandler> next;
    QString dir;

    // Static - because only one error msg can be at the time for all handlers.
    static QString errorMsg;

public:
    GitStateHandler(const QString& dir);
    virtual ~GitStateHandler() = default;

public:
    virtual void SetNext(std::shared_ptr<GitStateHandler> next);
    virtual bool Handle(GitStates& state) = 0;

public:
    void SetDir(const QString& dir);
    QString GetErrorMsg() const;

protected:
    bool RunGitCommand(const QString& command);
};

//////////////////////////////////////////////////////////////////////

class GitInitStateHandler : public GitStateHandler
{
public:
    GitInitStateHandler(const QString& dir);
    virtual ~GitInitStateHandler() override = default;

public:
    virtual bool Handle(GitStates& state) override;
};

//////////////////////////////////////////////////////////////////////

class GitAddStateHandler : public GitStateHandler
{
public:
    GitAddStateHandler(const QString& dir);
    virtual ~GitAddStateHandler() override = default;

public:
    virtual bool Handle(GitStates& state) override;
};

//////////////////////////////////////////////////////////////////////

class GitCommitStateHandler : public GitStateHandler
{
private:
    QString commitMsg;

public:
    GitCommitStateHandler(const QString& dir);
    virtual ~GitCommitStateHandler() override = default;

public:
    virtual bool Handle(GitStates& state) override;
    void SetCommitMsg(const QString& commitMsg);
};

//////////////////////////////////////////////////////////////////////

class GitRemoteStateHandler : public GitStateHandler
{
private:
    QString remoteBranch;

public:
    GitRemoteStateHandler(const QString& dir);
    virtual ~GitRemoteStateHandler() override = default;

public:
    virtual bool Handle(GitStates& state) override;
    void SetRemoteBranch(const QString& remoteBranch);

private:
    bool IsRemoteBranch();
};

//////////////////////////////////////////////////////////////////////

class GitPushStateHandler : public GitStateHandler
{
private:
    QString branch;

public:
    GitPushStateHandler(const QString& dir);
    virtual ~GitPushStateHandler() override = default;

public:
    virtual bool Handle(GitStates& state) override;

private:
    QString GetBranchName();
};

//////////////////////////////////////////////////////////////////////
#endif // GITSTATEHANDLER_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>

#include <QFile>
#include <QBitArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "Tools.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , workPath()
    , state(GitStates::None)
{
    ui->setupUi(this);

    InitApplicationData();
    InitHandlers();

    if (workPath.isEmpty())
    {
        HideAllComponents();
    }
    else
    {
        ShowAllComponents();
    }
}

MainWindow::~MainWindow()
{
    if (!workPath.isEmpty()) // Write down the working directory path.
    {
        // Remove the file, other words clear it.
        QFile::remove(PATH_TO_APP_DATA);

        // Write down new information.
        QFile file(PATH_TO_APP_DATA);

        if (file.open(QFile::WriteOnly | QFile::Text))
        {
            QString jsonData = "{"
                               "\"workPath\" : \"" + workPath + "\","
                               "\"state\" : " + QString::number(state) +
                               "}";

            QTextStream out(&file);
            out << jsonData;
            file.close();
        }
    }

    delete ui;
}


#pragma region INIT REGION
void MainWindow::InitHandlers()
{
    // Init handlers.
    auto initHandler   = std::make_shared<GitInitStateHandler>(workPath);
    auto addHandler    = std::make_shared<GitAddStateHandler>(workPath);
    auto commitHandler = std::make_shared<GitCommitStateHandler>(workPath);
    auto remoteHandler = std::make_shared<GitRemoteStateHandler>(workPath);
    auto pushHandler   = std::make_shared<GitPushStateHandler>(workPath);

    initHandler->SetNext(addHandler);
    addHandler->SetNext(commitHandler);
    commitHandler->SetNext(remoteHandler);
    remoteHandler->SetNext(pushHandler);

    // Add pointers to the map.
    handlers[GitStates::Init]   = initHandler;
    handlers[GitStates::Add]    = addHandler;
    handlers[GitStates::Commit] = commitHandler;
    handlers[GitStates::Remote] = remoteHandler;
    handlers[GitStates::Push]   = pushHandler;
}

void MainWindow::InitApplicationData()
{
    QFile file(PATH_TO_APP_DATA);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "We cannot open the file, maybe it doesn't exist.";
        return;
    }

    QByteArray jsonData = file.readAll();
    if (file.error() != QFile::NoError)
    {
        qDebug() << "Failed to read the file.";
        return;
    }

    file.close();

    if (jsonData.isEmpty())
    {
        qDebug() << "The file with storing data was empty.";
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(jsonData);

    if (!document.isObject()) // We store project data only in json object.
    {
        qDebug() << "The documet is not Json Object.";
        return;
    }

    // Init all needed data.
    try
    {
        QJsonObject object = document.object();

        workPath = object.value("workPath").toString();
        state = static_cast<GitStates>(object.value("state").toInt());
    }
    catch(...)
    {
        qDebug() << "Something hapend, but it is not scary.";
    }
}
#pragma endregion INIT REGION


void MainWindow::on_choosePath_clicked()
{
    workPath = QFileDialog::getExistingDirectory(this, "Choose the working directory");

    if (!workPath.isEmpty())
    {
        workPath += "/";
        QMessageBox::information(this, "Chosen directory", "You've chosen: " + workPath);
        ui->pathLabel->setText(workPath);
        ShowAllComponents();
        SetPathEachHandler();
    }
    else
    {
        HideAllComponents();
    }
}

void MainWindow::ShowAllComponents()
{
    ui->pathLabel->setHidden(false);
    ui->actionsGroup->setHidden(false);

    // Check if in our directory we have .git folder.
    if (HasFolder(workPath, ".git"))
    {
        ui->initRepos->setEnabled(false);
        state = GitStates::Init;
    }
    else
    {
        ui->initRepos->setEnabled(true);
        state = GitStates::None;
    }
}

void MainWindow::HideAllComponents()
{
    ui->pathLabel->setHidden(true);
    ui->actionsGroup->setHidden(true);
}

void MainWindow::SetPathEachHandler()
{
    for (auto& handler : handlers)
    {
        handler.second->SetDir(workPath);
    }
}


void MainWindow::on_initRepos_clicked()
{
    if (handlers[GitStates::Init]->Handle(state))
    {
        QMessageBox::information(this, "Information", "The init was seccessful.");
        ui->initRepos->setEnabled(false);
    }
    else
    {
        QMessageBox::warning(this, "Warning", handlers[GitStates::Init]->GetErrorMsg());
    }
}

void MainWindow::on_addChanges_clicked()
{
    if (handlers[GitStates::Add]->Handle(state))
    {
        QMessageBox::information(this, "Information", "The add was seccessful.");
        ui->initRepos->setEnabled(false);
    }
    else
    {
        QMessageBox::warning(this, "Warning", handlers[GitStates::Add]->GetErrorMsg());
    }
}

void MainWindow::on_commitChanges_clicked()
{
    if (state & GitStates::Add)
    {
        QString commitMsg = QInputDialog::getText(this, "Commit", "Enter message:");

        if (commitMsg.isEmpty())
        {
            QMessageBox::warning(this, "Warning", "You can't commit changes without text.");
            return;
        }

        dynamic_cast<GitCommitStateHandler*>(handlers[GitStates::Commit].get())->SetCommitMsg(commitMsg);

        if (handlers[GitStates::Commit]->Handle(state))
        {
            QMessageBox::information(this, "Information", "The add was seccessful.");
            ui->initRepos->setEnabled(false);
        }
        else
        {
            QMessageBox::warning(this, "Warning", handlers[GitStates::Commit]->GetErrorMsg());
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning", "You can't commit changes.");
    }
}

void MainWindow::on_pushButton_clicked()
{
    while (handlers[GitStates::Remote]->Handle(state))
    {
        QString remoteBranchUrl = QInputDialog::getText(this, "Remote", "Enter url to your repos: :");

        if (remoteBranchUrl.isEmpty())
        {
            QMessageBox::warning(this, "Warning", "Something wrong with url.");
            return;
        }

        dynamic_cast<GitRemoteStateHandler*>(handlers[GitStates::Remote].get())->SetRemoteBranch(remoteBranchUrl);
    }

    if (handlers[GitStates::Push]->Handle(state))
    {
        QMessageBox::information(this, "Information", "The push was seccessful.");
        ui->initRepos->setEnabled(false);
    }
    else
    {
        QMessageBox::warning(this, "Warning", handlers[GitStates::Push]->GetErrorMsg());
    }
}

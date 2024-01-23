#include "choosepathwindow.h"
#include "ui_choosepathwindow.h"
#include "mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>


ChoosePathWindow::ChoosePathWindow(QWidget *parent, MainWindow* mainWindow)
    : QMainWindow(parent)
    , ui(new Ui::ChoosePathWindow)
    , mainWindow(mainWindow)
{
    ui->setupUi(this);
}

ChoosePathWindow::~ChoosePathWindow()
{
    delete ui;
}


void ChoosePathWindow::SetMainWindow(MainWindow* mainWindowPtr)
{
    mainWindow = mainWindowPtr;
}

void ChoosePathWindow::on_pushButton_clicked()
{
    QString currentPath = QFileDialog::getExistingDirectory(this, "Choose the directory");

    if (!currentPath.isEmpty())
    {
        QMessageBox::information(this, "Chosen directory", "You have chosen: " + currentPath);
        mainWindow->SetWorkDir(currentPath);

        this->hide();
        mainWindow->show();

        mainWindow;
    }
}


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include "gitstatehandler.h"

#include "GitStates.h"
#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QString workPath;
    GitStates state;
    std::map<GitStates, std::shared_ptr<GitStateHandler>> handlers;

private: // Constants.
    ////////////////////////////////////////////////
    ///  The json object, that store information
    ///  about the app look like that:
    ///
    ///  {
    ///     "workPath" : "C:/some dir/another dir/",
    ///     "state"    : 0
    ///  }
    ///
    ////////////////////////////////////////////////
    const QString PATH_TO_APP_DATA = "AppData.json";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private: // Init Section.
    void InitHandlers();
    void InitApplicationData();

private slots:
    void on_choosePath_clicked();
    void on_initRepos_clicked();
    void on_addChanges_clicked();
    void on_commitChanges_clicked();
    void on_pushButton_clicked();

private: // Helper Funktions;
    void HideAllComponents();
    void ShowAllComponents();
    void SetPathEachHandler();
};
#endif // MAINWINDOW_H

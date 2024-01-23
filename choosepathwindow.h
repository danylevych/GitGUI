#ifndef CHOOSEPATHWINDOW_H
#define CHOOSEPATHWINDOW_H

#include <QMainWindow>

class MainWindow;

namespace Ui {
class ChoosePathWindow;
}

class ChoosePathWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::ChoosePathWindow *ui;
    MainWindow* mainWindow;

public:
    explicit ChoosePathWindow(QWidget *parent = nullptr, MainWindow* mainWindow = nullptr);
    ~ChoosePathWindow();

public:
    void SetMainWindow(MainWindow* mainWindowPtr);

private slots:
    void on_pushButton_clicked();
};

#endif // CHOOSEPATHWINDOW_H

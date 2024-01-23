#include <QApplication>
#include <QFileInfo>
#include <QString>

#include "mainwindow.h"
#include <QProcess>

// TODO: Save the state to the file maybe .json.
// TODO: Change the return value, probably to bool.

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow window;
    window.show();

    return a.exec();
}

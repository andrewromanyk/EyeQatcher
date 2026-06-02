#include "mainwindow.h"

#include <QApplication>
#include <QStatusBar>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("andrewromanyk");
    QCoreApplication::setApplicationName("EyeCatcher");
    // TODO: add tray icon
    MainWindow w;
    w.show();
    return a.exec();
}

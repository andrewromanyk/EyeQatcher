#include "mainwindow.h"

#include <QApplication>
#include <QStatusBar>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("andrewromanyk");
    QCoreApplication::setApplicationName("EyeCatcher");
    a.setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.setWindowFlags(w.windowFlags() & ~Qt::WindowMaximizeButtonHint);
    w.show();

    return a.exec();
}

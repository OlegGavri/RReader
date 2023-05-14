#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("RReader");
    QCoreApplication::setOrganizationName("reffum");
    QCoreApplication::setOrganizationDomain("reffum.com");

    MainWindow w;
    w.show();
    return a.exec();
}

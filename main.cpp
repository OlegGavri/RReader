#include "mainwindow.h"
#include "settings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("RReader");
    QCoreApplication::setOrganizationName("reffum");
    QCoreApplication::setOrganizationDomain("reffum.com");

    Settings::Init();

    MainWindow w;
    w.show();
    return a.exec();
}

#include "mainwindow.h"
#include "settings.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("RReader");
    QCoreApplication::setOrganizationName("reffum");
    QCoreApplication::setOrganizationDomain("reffum.com");
    QCoreApplication::setApplicationVersion(VERSION_STRING);

    QCommandLineParser cmdLineParser;
    cmdLineParser.setApplicationDescription("PDF/DJVU reader");
    cmdLineParser.addHelpOption();
    cmdLineParser.addVersionOption();
    cmdLineParser.addPositionalArgument("docs", "Open documents", "[docs...]");

    cmdLineParser.process(app);


    QStringList cmdLineDocuments = cmdLineParser.positionalArguments();

    Settings::Init();

    MainWindow w(cmdLineDocuments);
    w.show();
    return app.exec();
}

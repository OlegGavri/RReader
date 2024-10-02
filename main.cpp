#include <syslog.h>

#include "mainwindow.h"
#include "settings.h"
#include "documents.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //TODO: Use qInfo(), qWarning etc instead of syslog.
    openlog(QApplication::applicationName().toStdString().c_str(), 0, LOG_USER | LOG_INFO);
    syslog(LOG_INFO, "Application started");

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
    Documents::Init(cmdLineDocuments);

    MainWindow w;
    w.show();

    int exitCode = app.exec();

    Documents::Deinit();

    return exitCode;
}

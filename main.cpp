#include <syslog.h>
#include <iostream>

#include "mainwindow.h"
#include "settings.h"
#include "documents.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QtDebug>

using namespace std;

// Write messages from qInfo(), qCritical() etc to syslog
void syslogMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch(type)
    {
    case QtDebugMsg:
        syslog(LOG_DEBUG, "%s", localMsg.constData());
        cout << localMsg.constData() << endl;
        break;
    case QtInfoMsg:
        syslog(LOG_INFO, "%s", localMsg.constData());
        cout << localMsg.constData() << endl;
        break;
    case QtWarningMsg:
        syslog(LOG_WARNING, "%s", localMsg.constData());
        cerr << localMsg.constData() << endl;
        break;
    case QtCriticalMsg:
        syslog(LOG_CRIT, "%s", localMsg.constData());
        cerr << localMsg.constData() << endl;
        break;
    case QtFatalMsg:
        syslog(LOG_ERR, "%s", localMsg.constData());
        cerr << localMsg.constData() << endl;
        break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(syslogMessageHandler);
    QApplication app(argc, argv);
    openlog(QApplication::applicationName().toStdString().c_str(), 0, LOG_USER | LOG_INFO);
    qInfo() << "Application started";

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

#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include "messagesender.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt(QDateTime::currentDateTime().toString("[dd/MM/yy hh:mm:ss] "));
    switch (type) {
    case QtInfoMsg:
        txt += QString("Info: %1").arg(msg);
        break;
    case QtDebugMsg:
        txt += QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt += QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("Fatal: %1").arg(msg);
        break;
    }
    MessageSender::send(txt);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("MWPCReadout");
    QApplication::setOrganizationName("JINR");
    QApplication::setApplicationVersion("0.9");
    qInstallMessageHandler(myMessageHandler);
    MainWindow w;
    w.show();
    return a.exec();
}

#include <iostream>
#include <QCoreApplication>
#include <QApplication>

#include "managetoolserver.h"
#include "dbmanager.h"
#include <QThread>
#include "processthreadsapi.h"
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include "Base/PathTool.h"
#include <QDebug>

using namespace std;
#define MAXSIZE 10*1024*1024

void logOutFile(QString log)
{
    static int count = 0;

    QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd.log");
    if(count){
        fileName = QDateTime::currentDateTime().toString("yyyyMMdd_%1.log").arg(count);
    }
    QFile file(PathTool::logPath(fileName));    
    while(file.size() > MAXSIZE){
        count++;
        fileName = QDateTime::currentDateTime().toString("yyyyMMdd_%1.log").arg(count);
        file.setFileName(PathTool::logPath());
    }
    static QMutex mutex;
    QTextStream stream(&file);
    mutex.lock();

    file.open(QFileDevice::WriteOnly | QFileDevice::Append);
    stream<<log<<"\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}
void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QByteArray localMsg = msg.toLocal8Bit();
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString typetext;
    switch (type) {
    case QtDebugMsg:
        typetext = "Debug:";
        break;
    case QtInfoMsg:
        typetext = "Info:";
        break;
    case QtWarningMsg:
        typetext = "Warning:";
        break;
    case QtCriticalMsg:
        typetext = "Critical:";
        break;
    case QtFatalMsg:
        typetext = "Fatal:";
        break;
    }
    QString log = QString("%1 %2 %3%4").arg(QString::number(GetCurrentThreadId()),datetime,typetext,localMsg);
    logOutFile(log);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QCoreApplication a(argc, argv);

    DBManager::instance().initDatabase();

#ifdef QT_NO_DEBUG
    qInstallMessageHandler(messageOutput);
#endif

    ManageToolServer mtServer;

    a.exec();

    return 0;
}



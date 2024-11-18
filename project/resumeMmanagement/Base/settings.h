#ifndef SETTINGS_H
#define SETTINGS_H
#include <QSettings>
#include <QString>
#include <QVariant>
//#include <QHostAddress>


#ifdef QT_NO_DEBUG
    #define SERVER_IP   "192.168.10.122"
#else
    #define SERVER_IP   "127.0.0.1"
#endif

class Settings
{
public:
    Settings();
    static void setAccount(const QString& account){
        setValue("Account", account);
    }
    static QString getAccount(){
        return getValue("Account").toString();
    }
    static void setServer(const QString& account){
        setValue("ServerIp", account);
    }
    static QString getServer(){
        return getValue("ServerIp",(SERVER_IP)).toString();
    }
    static void setValue(QString key, QVariant value);
    static QVariant getValue(QString key,QVariant defaluteV=QVariant());
};

#endif // SETTINGS_H

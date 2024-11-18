#include "absdbmanager.h"
#include <QtDebug>

QSqlDatabase AbsDBManager::db_MTTool;

AbsDBManager::AbsDBManager()
{
   //git yyds

}

bool AbsDBManager::insert(QString table, QVariantMap binding)
{
    QSqlQuery q(db_MTTool);
    QString cmd;
    if(binding.isEmpty())
        return false;

    QStringList keys = binding.keys();
    if(binding.count() == 1)
    {
        cmd = QString("INSERT INTO %1 (%2) VALUES(:%3)")
                .arg(table,binding.firstKey(),binding.firstKey());
    }
    else
    {
        cmd = QString("INSERT INTO %1 (%2) VALUES(:%3)")
                .arg(table,keys.join(","),keys.join(",:"));
    }

    q.prepare(cmd);

    foreach(QString key, keys)
    {
        qDebug()<<QString(":%1").arg(key)<<binding[key];
        q.bindValue(QString(":%1").arg(key), binding[key]);
    }

    return queryExec(q, "INSERT");
}

bool AbsDBManager::replace(QString table, QVariantMap binding)
{
    QSqlQuery q(db_MTTool);

    QStringList keys = binding.keys();
    QString cmd = QString("REPLACE INTO %1 (%2) VALUES(:%3)")
                   .arg(table,keys.join(","),keys.join(",:"));

    q.prepare(cmd);

    foreach(QString key, keys)
    {
        q.bindValue(QString(":%1").arg(key), binding[key]);
    }

    return queryExec(q, "REPLACE");
}

bool AbsDBManager::deleteFrom(QString table, QString conditions)
{
    QSqlQuery q(db_MTTool);

    QString cmd = QString("DELETE FROM %1 WHERE %2")
            .arg(table, conditions);

    return queryExec(q, "DELETE_FROM", cmd);
}

bool AbsDBManager::update(QString table, QVariantMap binding, QString conditions)
{
    QSqlQuery q(db_MTTool);

    QString settings;
    QStringList bindingList;
    QStringList keys = binding.keys();
    foreach(QString key, keys)
    {
        bindingList.append(QString("%1='%2'").arg(key, binding[key].toString()));
    }
    if(bindingList.count() == 0)
        return false;
    if(bindingList.count() == 1)
        settings = bindingList.first();
    else
        settings = bindingList.join(",");

    QString cmd = QString("UPDATE %1 SET %2 WHERE %3").arg(table, settings, conditions);
    return queryExec(q, "UPDATE", cmd);
}

QSqlQuery AbsDBManager::query(QString cmd, QString msg)
{
    QSqlQuery q(db_MTTool);
    queryExec(q, msg, cmd);
    return q;
}

bool AbsDBManager::queryExec(QSqlQuery &q, QString msg, QString cmd)
{
    if(db_MTTool.isOpen() == false){
        qDebug() << "[queryExec] database is invalid";
        return false;
    }

    if(cmd.isEmpty())
    {
        if(!q.exec())
        {
            if(!msg.isEmpty())
            {
                qDebug()<<"[AbsDBManager]"<<msg<<"failed"<<q.lastError();
                qDebug()<<"SQL CMD:"<<q.lastQuery();
            }
            return false;
        }
    }
    else
    {
        if(!q.exec(cmd))
        {
            if(!msg.isEmpty())
            {
                qDebug()<<"[AbsDBManager]"<<msg<<"failed"<<q.lastError();
                qDebug()<<"SQL CMD:"<<q.lastQuery();
            }
            return false;
        }
    }
//    qDebug()<<msg<<q.lastQuery();
    return true;
}

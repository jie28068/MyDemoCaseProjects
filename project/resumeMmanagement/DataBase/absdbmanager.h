#ifndef ABSDBMANAGER_H
#define ABSDBMANAGER_H
#include <QString>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class AbsDBManager
{
public:
    AbsDBManager();
    //初始化数据库
    virtual void initDB(QString db)=0;
    /**
     * @brief insert 向表中插入数据：
     * INSERT INTO TABLE_NAME [(column1, column2, column3,...columnN)]
     *  VALUES (value1, value2, value3,...valueN);
     * @param table 插入的目标表名
     * @param binding   插入数据的[key-value]map
     * @return 操作是否成功
     */
    virtual bool insert(QString table, QVariantMap binding);
    //replace
    virtual bool replace(QString table, QVariantMap binding);
    //delete
    virtual bool deleteFrom(QString table, QString conditions);
    //update
    virtual bool update(QString table, QVariantMap binding, QString conditions);
    //query
    virtual QSqlQuery query(QString cmd, QString msg=QString());

protected:
    //创建各个表
    virtual bool createTable() = 0;
    static bool queryExec(QSqlQuery &q, QString msg=QString(), QString cmd=QString());

    //定义俩个数据
    static QSqlDatabase db_MTTool;


};

#endif // ABSDBMANAGER_H

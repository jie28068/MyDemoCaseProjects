#include "sqlitemanager.h"


SqliteManager::SqliteManager()
{

}

/*初始化数据库*/
void SqliteManager::initDB(QString db)
{
    qDebug()<<"db drivers:"<<db_MTTool.drivers();
    if(QSqlDatabase::contains(db))
     {
         db_MTTool = QSqlDatabase::database(db);
     }
     else
     {
         db_MTTool = QSqlDatabase::addDatabase("QSQLITE", db);
         db_MTTool.setDatabaseName(db);
     }
     if(!db_MTTool.open())
     {
         qDebug()<<"database open failed";
     }
     else
     {
         qDebug()<<"database open success";
     }
     createTable();
}

/*创建表*/
bool SqliteManager::createTable()
{

    QStringList tables = db_MTTool.tables();
    QSqlQuery q(db_MTTool);
    QString cmd;
    if(tables.indexOf("ArchiveInfo") == -1)
    {
        /*档案信息表*/
        cmd = QString(R"(
                    CREATE TABLE "ArchiveInfo" (
                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                      "name" nchar(8) DEFAULT '***',
                      "birthday" date(10) DEFAULT '********',
                      "tel_no" char(14) DEFAULT '***********',
                      "post" char(8) DEFAULT '',
                      "level" char(32) DEFAULT '',
                      "format" char(5) NOT NULL,
                      "type" char(4) DEFAULT '',
                      "src" integer(8) NOT NULL,
                      "mark" char(8) DEFAULT '',
                      "sign" char(32) NOT NULL  UNIQUE,
                      "timestamp" timestamp NOT NULL,
                      "opt_id" INTEGER NOT NULL,
                      "experience" char(255)  DEFAULT ''
                    )
               )");
        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"ArchiveInfo"<<"success";
        }
        else
        {
           qDebug()<<"[create table]"<<"ArchiveInfo"<<"error";
        }
    }

    if(tables.indexOf("PlanInfo") == -1)
    {
        /*计划信息表*/
        cmd = QString(R"(
                    CREATE TABLE "PlanInfo" (
                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                      "type" char(16) DEFAULT custom,
                      "create_time" datetime(20) NOT NULL DEFAULT (datetime('now', 'localtime')),
                      "node_time" datetime(20) NOT NULL,
                      "finish_time" datetime(20) DEFAULT '',
                      "related_id" INTEGER(8),
                      "detail" TEXT,
                      "note" TEXT,
                      "role_create" integer(8) NOT NULL,
                      "role_target" integer(8),
                      "role_group" integer(8),
                      "role_complete" integer(8) DEFAULT 0
                    );
               )");

        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"PlanInfo"<<"success";
        }
        else
        {
           qDebug()<<"[create table]"<<"PlanInfo"<<"error";
        }
    }

    if(tables.indexOf("UserPlan") == -1)
    {
        /*用户信息表*/
        cmd = QString(R"(
                        CREATE TABLE "UserPlan" (
                          "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                          "plan_id" INTEGER(8) NOT NULL,
                          "user_id" INTEGER(8) NOT NULL,
                          "status" nchar(8) DEFAULT '待执行',
                          "opt_time" datetime(20)
                        );
                    )");

        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"UserPlan"<<"success";
        }
        else
        {
           qDebug()<<"[create table]"<<"UserPlan"<<"error";
        }
        cmd = QString(R"(
                    CREATE UNIQUE INDEX user_plan_index
                        ON UserPlan (
                        user_id,
                        plan_id
                        );
                )");
        if(queryExec(q ,"[create unique index]", cmd))
        {
            qDebug()<<"[create unique index]"<<"user_plan_index"<<"success";

        }
        else
        {
           qDebug()<<"[create unique index]"<<"user_plan_index"<<"error";
        }


    }

    if(tables.indexOf("UserInfo") == -1)
    {
        /*用户信息表*/
        cmd = QString(R"(
                    CREATE TABLE "UserInfo" (
                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                      "name" nchar(8) NOT NULL,
                      "email" char(24) NOT NULL UNIQUE,
                      "tel_no" char(14) NOT NULL UNIQUE,
                      "passwd" char(32) NOT NULL,
                      "pow" integer(2) DEFAULT 40,
                      "department" nchar(8) NOT NULL,
                      "type" char(8) DEFAULT 'internal'
                    );
               )");

        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"UserInfo"<<"success";
        }
        else
        {
           qDebug()<<"[create table]"<<"UserInfo"<<"error";
        }
    }

    if(tables.indexOf("GroupInfo") == -1)
    {
        /*用户信息组表*/
        cmd = QString(R"(
                    CREATE TABLE "GroupInfo" (
                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                      "name" nchar(8) NOT NULL UNIQUE
                    );
               )");

        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"GroupInfo"<<"success";
        }
        else
        {
           qDebug()<<"[create table]"<<"GroupInfo"<<"error";
        }
    }

    if(tables.indexOf("UserGroup") == -1)
    {
        /*用户分组*/
        cmd = QString(R"(
                    CREATE TABLE "UserGroup" (
                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                      "user_id" INTEGER(8) NOT NULL,
                      "group_id" INTEGER(8) NOT NULL
                    );
               )");

        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"UserGroup"<<"success";
        }
        else
        {
           qDebug()<<"[create table]"<<"UserGroup"<<"error";
        }
        cmd = QString(R"(
                    CREATE UNIQUE INDEX user_group_index
                        ON UserGroup (
                        user_id,
                        group_id
                        );
                )");
        if(queryExec(q ,"[create unique index]", cmd))
        {
            qDebug()<<"[create unique index]"<<"user_group_index"<<"success";

        }
        else
        {
           qDebug()<<"[create unique index]"<<"user_group_index"<<"error";
        }
    }

//    if(tables.indexOf("ResumeInfo") == -1)
//    {
//        /*档案文本表*/
//        cmd = QString(R"(
//                    CREATE TABLE "ResumeInfo" (
//                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
//                      "sign" char(32) NOT NULL  UNIQUE,
//                      "text_entirety" text DEFAULT '',
//                      "text_simplify" text DEFAULT '',
//                      "name" nchar(8) DEFAULT '***',
//                      "birthday" date(10) DEFAULT '********',
//                      "experience" char(255)  DEFAULT ''
//                    );
//               )");
//        if(q.exec(cmd))
//        {
//            qDebug()<<"[create table]"<<"ResumeInfo"<<"success";

//        }
//        else
//        {
//           qDebug()<<"[create table]"<<"ResumeInfo"<<"error";
//        }
//    }

    if(tables.indexOf("WorkLog") == -1)
    {
        /*工作日志表*/
        cmd = QString(R"(
                        CREATE TABLE "WorkLog" (
                        "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                        "user_id" INTEGER(8) NOT NULL,
                        "operation" TEXT NOT NULL,
                        "time" datetime(20) NOT NULL
                        );
               )");
        if(q.exec(cmd))
        {
            qDebug()<<"[create table]"<<"WorkLog"<<"success";

        }
        else
        {
           qDebug()<<"[create table]"<<"WorkLog"<<"error";
        }
    }
    return true;   
}


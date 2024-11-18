#include "dbmanager.h"
#include <QFile>
#include "DataBase/sqlitemanager.h"
#include "Base/FormatTranslator.h"
#include "Base/PathTool.h"
#include "Base/SignTool.h"


#include "sessions/sessionbaselogin.h"
//DBManager::DBManager()
//{

//}

void DBManager::initDatabase()
{
    db = new SqliteManager;
    db->initDB(PathTool::databasePath()+"/"+"KLMT.sqlite3");
}

bool DBManager::archiveSignCheck(QString sign)
{
    QString cmd = QString("SELECT * FROM ArchiveInfo WHERE sign='%1'").arg(sign);
    QSqlQuery q = db->query(cmd, "[archiveSignCheck]");

    if(q.next())//存在相同签名的档案
        return false;

    return true;
}

QVariantMap DBManager::getArchiveInfo(QString sign)
{

    QString format;
    quint64 size = 0;
    QString cmd = QString("SELECT format FROM ArchiveInfo WHERE sign='%1'").arg(sign);
    QSqlQuery q = db->query(cmd, "[DBManager::getArchiveInfo]");
    if(!q.next())
        format = QString();
    else
        format = q.value(0).toString();

    QString filepath = PathTool::archivePath()+"/"+sign+"."+format;
    qDebug()<<"filepath"<<filepath;

    QFile f(filepath);
    if(!f.open(QFile::ReadOnly)){
        size = 0;
    }
    else{
        size = f.size();
    }

    QVariantMap archiveMap;
    archiveMap["sign"] = sign;
    archiveMap["size"] = size;
    archiveMap["format"] = format;
    return archiveMap;
}


bool DBManager::newArchiveInfo(QVariantMap &archiveMap)
{
    return db->insert("ArchiveInfo", archiveMap);
}

bool DBManager::newArchiveText(QVariantMap &archiveMap)
{
    return db->insert("ResumeInfo", archiveMap);
}

bool DBManager::updateArchiveInfo(QVariantMap &archiveMap)
{
    QString id = archiveMap["id"].toString();
    QString condition = QString("id='%1'").arg(id);
    archiveMap.remove("id");
    archiveMap.remove("date");
    return db->update("ArchiveInfo", archiveMap, condition);
}

/*
    cmd = QString(R"(
                    CREATE TABLE "UserInfo" (
                      "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                      "name" nchar(8) NOT NULL,
                      "email" char(24) NOT NULL,
                      "tel_no" char(14) NOT NULL,
                      "passwd" char(32) NOT NULL,
                      "pow" integer(2) DEFAULT 40,
                      "department" nchar(8) NOT NULL,
                      "type" char(8) DEFAULT 'internal'
                    );
*/
QVariantMap DBManager::BaseLoginHandle(const QVariantMap &srcParams,int &LoginResult)
{
    //提取用户ID、密码
    QString UserId=srcParams["user_id"].toString();
    QString Passwd=srcParams["passwd"].toString();
    qDebug()<<"login id:"<<UserId;
    qDebug()<<"login passwd:"<<Passwd;
    QString cmd=QString("select * from UserInfo where email = '%1' or tel_no = '%1' ").arg(UserId);
    QSqlQuery q=db->query(cmd,"QUERY");

    QVariantList mapList=FormatTransLator::QueryToMap(q);
    if(mapList.count()==0)
    {
        //找不到用户email 或者 tel_no
        QVariantMap map;
        qDebug()<<"Error,User information could not be found.";
        LoginResult=NoUserMessage;
        return map;
    }
    for(int i=0;i<mapList.count();i++)
    {
        QVariantMap map;
        map=mapList.value(i).toMap();
        if(QString::compare(Passwd,map.value("passwd").toString())==0)
        {
            qDebug()<<"login successfully.";
            map.remove("passwd");
            LoginResult=LoginSucess;
            return map;      //密码正确 返回信息
        }
    }
    // 密码错误
    qDebug()<<"Error,User password error.";
    {
        QVariantMap map;
        LoginResult=PasswordError;
        return map;
    }
}

bool DBManager::newUserInfo(const QVariantMap &userParams)
{
    return db->insert("UserInfo", userParams);
}

int DBManager::queryUserID(QString contiditon)
{
    QString cmd = QString("SELECT id FROM UserInfo WHERE %1 ").arg(contiditon);
    QSqlQuery q = db->query(cmd, "[queryUserID]");
    if(!q.next())
        return 0;
    else
        return q.value(0).toInt();
}

bool DBManager::newUserGroup(QVariantMap groupMap)
{
    return db->insert("GroupInfo", groupMap);
}

QVariantList DBManager::queryGroupUserID(int gID)
{
    QString cmd = QString("SELECT user_id FROM UserGroup WHERE group_id =%1").arg(gID);
    QSqlQuery q = db->query(cmd,"[queryGroupUserID]");
    QVariantList l = FormatTransLator::QueryToMap(q);
//    qDebug()<<l;
    QList<QVariant> retList = FormatTransLator::QVariantListTranslate<QVariant>(l, "user_id");
//    qDebug()<<retList;
    return retList;
}

QVariantList DBManager::HrmFilteResumeHandle(const QVariantMap &srcParams)
{
    QString conditions = srcParams["Conditions"].toString();
    QString cmd = QString(R"(
SELECT
       [a].[id],
       [a].[name],
       [a].[birthday],
       [a].[tel_no],
       CASE [a].[level] WHEN 0 THEN '一般人才' WHEN 10 THEN '优质人才' ELSE '未定义人才' END AS [levelname],
       [a].[post],
       [a].[level],
       [a].[format],
       [a].[type],
       [a].[src] AS [oldsrcid],
       [u].[name] AS [src],
       [a].[mark],
       [a].[sign],
       DATETIME ([a].[timestamp] / 1000, 'unixepoch', 'localtime') AS [date],
       [a].[timestamp],
       [a].[opt_id]
FROM   [ArchiveInfo] AS [a]
       LEFT JOIN [UserInfo] AS [u] ON [oldsrcid] = [u].[id];
                        )");

    if(!conditions.isEmpty())
    {
        cmd.append(QString(" WHERE %1").arg(conditions));
    }

//    qDebug()<<cmd;
    QSqlQuery q=db->query(cmd,"HrmFilteResumeHandle");

    QVariantList mapList=FormatTransLator::QueryToMap(q);

    if(mapList.count()==0)
    {
        //没有符合条件
        qDebug()<<"Did not meet the conditions";
    }

    return mapList;
}

QVariantList DBManager::QueryResumeTextList()
{
    QString cmd="select * from ArchiveInfo";
    QSqlQuery q=db->query(cmd,"QUERY");
    QVariantList mapList=FormatTransLator::QueryToMap(q);
    return mapList;
}

bool DBManager::UpdateSession(const QVariantMap &srcParams)
{
    QString id = srcParams["id"].toString();
    QString cmd = QString("select note from PlanInfo where id = %1").arg(id);
    QSqlQuery q=db->query(cmd,"QUERY");

    QString note1;
    if(q.next())
        note1 = q.value(0).toString();

    QString note2 = srcParams["new_note"].toString();
    if(note1.contains(note2))
    {
        return false;
    }

    note1.append(note2);
    note1.append("\n");

    QVariantMap m;
    m.insert("note",note1);
    QString n = QString("id = %2").arg(id);

    return db->update("PlanInfo",m , n);
}

QVariantList DBManager::SessionPlanQuery(const QVariantMap &srcParams)
{
    QString type = srcParams["type"].toString();
    QString finished = srcParams["finished"].toString();
    QString conditions;
    if(type.isEmpty() && finished.isEmpty())
        conditions = QString();
    else
    {
        conditions = QString("WHERE PI.type='%1' AND finished='%2'").arg(type).arg(finished);
    }

    int user_id = srcParams["id"].toInt();

    QString cmd = QString(R"(
                            SELECT
                                PI.id,
                                PI.type,
                                PI.create_time,
                                PI.node_time,
                                PI.finish_time,
                                CASE

                                    WHEN finish_time = '' THEN
                                    'false' ELSE 'true'
                                END AS finished,
                                PI.related_id,
                                PI.detail,
                                PI.note,
                                PI.role_create,
                            CASE

                                    WHEN %1 IN ( SELECT user_id FROM UserGroup WHERE group_id = PI.role_group ) OR %1=role_target THEN
                                    'true' ELSE 'false'
                                END AS is_related,
                                PI.role_group,
                                PI.role_complete,
                                AI.name,
                                AI.sign,
                                AI.format,
                                UI.name AS name_create,
                                IFNULL(UP.status, '无关') AS status
                            FROM
                                PlanInfo AS PI
                                LEFT JOIN ArchiveInfo AS AI ON PI.related_id=AI.id
                                LEFT JOIN UserInfo AS UI ON UI.id=PI.role_create
                                LEFT JOIN UserPlan AS UP ON UP.plan_id=PI.id AND UP.user_id=%1
                            %2
                        )").arg(user_id).arg(conditions);
    QSqlQuery q=db->query(cmd,"QUERY");
    QVariantList mapList=FormatTransLator::QueryToMap(q);
    if(mapList.count()==0)
    {
        qDebug()<<"Did not SessionPlanQuery";
    }
    return mapList;
}

QVariantList DBManager::SessionBaseQueryUsrInfo(const QVariantMap &srcParams)
{
    //[filter] 查询条件 [isAll] 是否查询全部
    QString strFilter = srcParams["filter"].toString();
    bool bisAll = srcParams["isAll"].toBool();
    QString cmd;
    if(bisAll){
        cmd = QString(R"(
                                  SELECT *
                                  FROM   [UserInfo]
                                  WHERE  %1 ;
                                )").arg(strFilter);
    }else{
        cmd = QString(R"(
                                  SELECT
                                         [id],
                                         [department] ,
                                         [name] ,
                                         [type]
                                  FROM   [UserInfo]
                                  WHERE  %1 ;
                                )").arg(strFilter);
    }

    QSqlQuery q=db->query(cmd,"QueryUsrInfo");
    QVariantList mapList=FormatTransLator::QueryToMap(q);
    if(mapList.count()==0)
    {
        qDebug()<<"Did not SessionBaseQueryUsrInfo";
    }
    return mapList;
}

QVariantList DBManager::SessionPlanStatis(const QVariantMap &srcParams)
{
    Q_UNUSED(srcParams)
    QString cmd = QString(R"(
                        SELECT
                               [type],
                               COUNT (*) AS [amount],
                               CASE [finish_time] WHEN '' THEN '未完成' ELSE '已完成' END AS [status]
                        FROM   [PlanInfo]
                        GROUP  BY [status], [type];
                        )");
    QSqlQuery q=db->query(cmd,"Statis");
    QVariantList mapList=FormatTransLator::QueryToMap(q);
    if(mapList.count()==0)
    {
        qDebug()<<"Did not SessionPlanStatis";
    }
    return mapList;
}

QVariantList DBManager::SessionQueryWorkLog(const QVariantMap &srcParams)
{
    QString id = srcParams["userId"].toString();
    QString startDate = srcParams["startDate"].toString();
    if(startDate.isEmpty()){
        startDate = QDateTime::currentDateTime().addDays(-30).toString("yyyy-MM-dd");
    }
    QString cmd = QString(R"(
                        SELECT
                               [u].[department],
                               [u].[name],
                               [w].[operation],
                               [w].[time]
                        FROM   [WorkLog] AS [w]
                               LEFT JOIN [UserInfo] AS [u] ON [w].[user_id] = [u].[id]
                        WHERE  [w].[user_id] = %1 AND [w].[time] > '%2'
                        ORDER  BY [w].[time];
                        )").arg(id,startDate);
    QSqlQuery q=db->query(cmd,"WorkLog");
    QVariantList mapList=FormatTransLator::QueryToMap(q);
    if(mapList.count()==0)
    {
        qDebug()<<"Did not SessionQueryWorkLog";
    }
    return mapList;
}

bool DBManager::SessionPlanFinish(const QVariantMap &srcParams)
{
    QString r = srcParams["id"].toString();
    QVariantMap map;
    QString rc = srcParams["role_complete"].toString();
    QString ft = srcParams["finish_time"].toString();
    map.insert("role_complete",rc);
    map.insert("finish_time",ft);
    QString n = QString("id = %1").arg(r);
    return db->update("PlanInfo",map , n);
}

bool DBManager::SessionBaseUserGroupUpdate(QVariantMap &srcParams)
{
    int user_id = srcParams["user_id"].toInt();
    int group_id = srcParams["group_id"].toInt();
    QString opt = srcParams["opt"].toString();
    srcParams.remove("opt");

    if(opt == "insert")
    {
        //插入
        QString cmd = QString("INSERT INTO UserGroup(user_id,group_id) VALUE(%1,%2)").arg(user_id).arg(group_id);
        return db->insert("UserGroup", srcParams);
    }
    else if(opt == "delete")
    {
        //删除
        QString condition = QString("user_id=%1 AND group_id=%2").arg(user_id).arg(group_id);
        return db->deleteFrom("UserGroup", condition);
    }
    return false;
}

/**
 * @brief DBManager::queryGroupUsers
 * @param id    组id，若为0则无视id查询所有用户
 * @return
 */
QVariantList DBManager::queryGroupUsers(int id)
{
    QString cmd;
    if(id)
    {

    }
    else
    {
        cmd = QString("SELECT id,name,email,tel_no,pow,department,type FROM UserInfo where type='internal' ");
    }

    QSqlQuery q = db->query(cmd, "[queryGroupUsers]");
    return FormatTransLator::QueryToMap(q);
}

QVariantMap DBManager::queryGroupInfo()
{
    QString cmd = QString(R"(
                            SELECT
                                UG.group_id,
                                UG.user_id,
                                GI.name AS group_name,
                                UI.name AS user_name,
                                UI.department,
                                UI.email,
                                UI.pow,
                                UI.tel_no,
                                UI.type
                            FROM
                                UserGroup AS UG
                                LEFT JOIN UserInfo AS UI ON UG.user_id = UI.id
                                LEFT JOIN GroupInfo AS GI ON UG.group_id = GI.id
                        )");
    QSqlQuery q = db->query(cmd,"[queryGroupInfo]");
    QVariantList list_group_info = FormatTransLator::QueryToMap(q);

    cmd = QString("SELECT id,name FROM GroupInfo");
    q = db->query(cmd,"[queryGroupList]");
    QVariantList list_group = FormatTransLator::QueryToMap(q);
//    while (q.next()) {
//        list_group<<q.value(0);
//    }

    QVariantMap map_group;
    map_group["list_group"] = list_group;
    map_group["list_group_info"] = list_group_info;

    return map_group;
}

int DBManager::SessionPlanCreate(QVariantMap groupMap)
{
    QVariantList related_id_list = groupMap["related_id_list"].toList();
    QVariantList target_list = groupMap["target_list"].toList();

    groupMap.remove("related_id_list");
    groupMap.remove("target_list");


    foreach(QVariant related_id, related_id_list)
    {
        QVariantMap m;
        QString ty = "初筛中";
        m.insert("type",ty);
        QString n = QString("id = %1").arg(related_id.toString());
        db->update("ArchiveInfo", m, n);
    }

    if(related_id_list.isEmpty())//单个创建
    {
        return db->insert("PlanInfo", groupMap);
    }
    else//批量创建
    {
        int ret = 0;
        foreach(QVariant related_id, related_id_list)
        {
            groupMap["related_id"] = related_id;
            qDebug()<<groupMap;
            if(db->insert("PlanInfo", groupMap))
            {
                foreach(QVariant target, target_list)
                {
                    QVariantMap user_plan;
                    user_plan["plan_id"] = related_id;
                    user_plan["user_id"] = target;

                    QString cmd = QString("INSERT INTO UserPlan (plan_id, user_id) VALUES((SELECT id FROM PlanInfo WHERE related_id=%1 AND create_time='%2'), %3)")
                            .arg(related_id.toString())
                            .arg(groupMap["create_time"].toString())
                            .arg(target.toString());

                    db->query(cmd,"[SessionPlanCreate] insert UserPlan");
                }
                ret++;
            }
        }
        return ret;
    }
}

bool DBManager::insertOneWorkLog(QString user, QString log)
{
    QVariantMap logmap;
    logmap["user_id"]=user;
    logmap["operation"]=log;
    logmap["time"]=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return db->insert("WorkLog", logmap);
}


#ifndef DBMANAGER_H
#define DBMANAGER_H
#include "Base/singleton.h"
#include "DataBase/absdbmanager.h"
#include "DataBase/sqlitemanager.h"


enum{
     LoginSucess = 0 ,    //登录成功
     NoUserMessage = 1,   //无用户信息
     PasswordError = 2,   //密码错误
};     //用户登录时 返回结果;


class DBManager:public Singleton<DBManager>
{
    //单例实现相关
    friend class Singleton<DBManager>;
public:
    DBManager(const DBManager&)=delete;
    DBManager& operator =(const DBManager&)= delete;

public:
    void initDatabase();

    //业务相关方法，功能目标：实现各接口对数据库的操作
    /**
     * @brief DBManager::archiveSignCheck   档案签名校验
     * @param sign  待校验的签名
     * @return  签名是否合法[已存在：false][不存在：true]
     */
    bool archiveSignCheck(QString sign);

    /**
     * @brief getArchiveSize    获取档案签名对应文件大小
     * @param sign  档案签名
     * @return  文件大小
     */
    QVariantMap getArchiveInfo(QString sign);//获取档案签名对应文件大小

    /**
     * @brief newArchiveInfo    档案信息入库
     * @param archiveMap    档案信息
     * @return  入库是否成功
     */
    bool newArchiveInfo(QVariantMap &archiveMap);//插入新的档案信息
    bool newArchiveText(QVariantMap &archiveMap);//档案入库后，将档案文本、sign、format存入ArchiveText
    bool updateArchiveInfo(QVariantMap &archiveMap);//更新档案信息

    /**
     * @brief 用户登录会话处理
     * @param 输入: srcParams  账号[user_id]：用户的邮箱或者手机号and密码[passwd]：密码    输出:LoginResult  返回登录结果
     * @return  用户信息:key：通信合法标识 name：姓名 email：邮箱  tel_no：电话号码 ~~passwd：密码  pow：权限 department：部门  type：用户类型[内部用户：internal][外部用户：external]
     */
    QVariantMap BaseLoginHandle(const QVariantMap &srcParams,int &LoginResult);    //用户登录会话处理
    bool newUserInfo(const QVariantMap &userParams);//用户注册
    int queryUserID(QString contiditon);//自定条件 查询用户ID
    bool newUserGroup(QVariantMap groupMap);//创建新的用户组
    QVariantList queryGroupUserID(int gID);//根据用户组ID查询用户组下的所有用户ID

    /**
     * @brief 查询简历列表会话处理
     * @param [Conditions]：过滤条件
     * @return  符合条件的列表
     */
    QVariantList HrmFilteResumeHandle(const QVariantMap &srcParams);  //根据过滤条件查询简历列表会话处理
    QVariantList QueryResumeTextList();                                   //查询简历档案中文本数据(用来简历对比)

    /**
     * @brief 计划更新会话
     * @param 添加的信息：new_note(往note末尾添加)
     * @return null
     */
    bool UpdateSession(const QVariantMap &srcParams);

    /**
     * @brief DBManager::计划查询会话
     * @param srcParams
     * @return  计划列表 计划信息
     */
    QVariantList SessionPlanQuery(const QVariantMap &srcParams);

    /**
     * @brief SessionBaseQueryUsrInfo 查询用户信息
     * @param srcParams [filter] 查询条件 [isAll] 是否查询全部
     * @return
     */
    QVariantList SessionBaseQueryUsrInfo(const QVariantMap &srcParams);

    /**
     * @brief SessionPlanStatis 计划数量总览查询
     * @param srcParams
     * @return 各状态计划及其数量
     */
    QVariantList SessionPlanStatis(const QVariantMap &srcParams);

    /**
     * @brief SessionQueryWrokLog 工作日志查询
     * @param srcParams
     * @return 当前用户的工作日志
     */
    QVariantList SessionQueryWorkLog(const QVariantMap &srcParams);

    /**
     * @brief 计划完成会话
     * @param srcParams
     * @return null
     */
    bool SessionPlanFinish(const QVariantMap &srcParams);

    /**
     * @brief 用户组成员变更
     * @param srcParams
     * @return null
     */
    bool SessionBaseUserGroupUpdate(QVariantMap &srcParams);
    QVariantList queryGroupUsers(int id);//查询组id下的所有用户，id为0则无视分组查询所有用户
    QVariantMap queryGroupInfo();//查询所有用户组
    /**
     * @brief 计划创建
     * @param srcParams
     * @return null
     */
    int SessionPlanCreate(QVariantMap groupMap);
    /**
     * @brief insertOneWorkLog 插入一条工作日志
     * @param log 日志内容 需包含
     *                   + user_id：用户id
     *                   + operation：操作
     *                   + time：时间
     * @return
     */
    bool insertOneWorkLog(QString user,QString log);

private:
    DBManager()=default;
    AbsDBManager* db;
};

#endif // DBMANAGER_H

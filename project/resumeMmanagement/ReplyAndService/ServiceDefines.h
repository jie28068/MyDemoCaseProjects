#ifndef SERVICEDEFINES_H
#define SERVICEDEFINES_H

#define SERVICE_GROUP_BASE  "Base"

#define SERVICE_BASE_LOGIN  "Login"  //登入
#define SERVICE_BASE_REGIST  "Regist"   //注册
#define SERVICE_BASE_USERGROUPCREATE "UserGroupCreate" //创建
#define SERVICE_BASE_USERGROUPUPDATE "UserGroupUpdate" //变更
#define SERVICE_BASE_QUERYGROUPINFO "QueryGroupInfo" //用户组查询
#define SERVICE_BASE_QUERYGROUPUSERS "QueryGroupUsers" //用户组用户查询
#define SERVICE_BASE_QUERYWROKLOG "QueryWorkLog" //工作日志查询
#define SERVICE_BASE_QUERYUSRINFO "QueryUsrInfo" //用户信息查询 ,传过滤条件


#define SERVICE_GROUP_HRM   "Hrm"
#define SERVICE_HRM_UPLOADRESUME  "UploadResume"    //简历上传请求
#define SERVICE_HRM_FILTERRESUME  "FilterResume"    //查询简历列表
#define SERVICE_HRM_QUERYRESUME  "QueryResume"  //查询简历详情
#define SERVICE_HRM_UPLOADARCHIVE  "UploadArchive"  //档案入库
#define SERVICE_HRM_UPDATEARCHIVE  "UpdateArchive"    //档案替换

#define SERVICE_GROUP_PLAN "Plan"
#define SERVICE_PLAN_NOTIFY "Notify" //通知
#define SERVICE_PLAN_CREATE "Create" //创建
#define SERVICE_PLAN_UPDATE "Update" //更新
#define SERVICE_PLAN_FINISH "Finish" //完成
#define SERVICE_PLAN_QUERY "Query" //查询
#define SERVICE_PLAN_STATIS "Statis" //获取计划数量总览

#endif // SERVICEDEFINES_H

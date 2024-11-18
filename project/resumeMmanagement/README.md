[TOC]
# KLMT
Keliang manage tools 科梁管理工具集
## 1 服务端
### 1.1 ManageToolServer
接口定义
通知：服务端主动发起
请求：客户端主动发起，并需要服务端回应
#### 1.1.1 请求接口
接口请求要素：
1. 操作人[Key]：区别不同的客户端对应的不同用户
2. 接口组[ServiceGroup]：接口按不同功能进行分组
3. 接口[Service]：接口唯一对应的接口名
4. 接口参数表[Params]：接口附带的参数
5. 时间戳[Timestamp]：接口调用的时间戳（毫秒级）

接口返回要素：
1. 结果[Success]：成功或失败
2. 结果备注[Result]：成功提示或失败原因
3. 请求来源信息[Request]：包含接口调用时的操作人，接口组，接口，时间戳四个要素信息（原封不动）
4. 返回内容[Params]：调用者需要的数据内容
5. 时间戳[Timestamp]：响应时间戳

#### 1.1.2 简历上传请求
服务组：Hrm  
服务：UploadResume
+ [sign]：文件签名
+ [size]：文件大小  

返回：无附带参数
独占连接，传输文件流

#### 1.1.3 简历上传数据流
先不做

#### 1.1.4 注册
服务组：Base  
服务：Regist  
+ [name]：姓名
+ [email]：邮箱
+ [tel_no]：电话号码
+ [passwd]：密码
+ [department]：部门

返回：
无附带参数

#### 1.1.5 登录
服务组：Base  
服务：Login  
+ 账号[user_id]：用户的邮箱或者手机号  
+ 密码[passwd]：密码 

返回：
+ key：通信合法标识
+ name：姓名
+ email：邮箱
+ tel_no：电话号码
+ ~~passwd：密码~~
+ pow：权限[超级管理员10][部门管理员20][小组管理员30][员工40]
+ department：部门
+ type：用户类型[内部用户：internal][外部用户：external]

#### 1.1.6 查询简历列表
服务组：Hrm  
服务：FilterResume  
+ [Conditions]：过滤条件

返回：
>resume_list
>> + name：姓名 默认**
>> + birthday：生日 默认********
>> + tel_no：电话号码  默认***********
>> + post：岗位[QT][C++][产品经理]
>> + level：级别[未定：0][初级：10][中级：20][高级：30]
>> + type：档案类型
>> + src：档案来源（用户ID）
>> + mark：档案标记 默认无标记
>> + sign：档案签名，对应的简历文件MD5
>> + timestamp：变更时间
>> + opt_id：操作人


#### 1.1.7 查询简历详情
服务组：Hrm  
服务：QueryResume  
+ [ID]：简历ID

返回：
+ sign：档案签名
+ size：档案大小  

独占连接  
[文件流传输]

#### 1.1.8 档案入库
服务组：Hrm  
服务：UploadArchive  
+ ame：姓名 默认**
+ birthday：生日 默认********
+ tel_no：电话号码  默认***********
+ post：岗位[QT][C++][产品经理]
+ level：级别[未定：0][初级：10][中级：20][高级：30]
+ type：档案类型
+ src：档案来源（用户ID）
+ mark：档案标记 默认无标记
+ sign：档案签名，对应的简历文件MD5
+ timestamp：变更时间
+ opt_iD：操作人

返回：无附带参数

#### 1.1.9 档案替换
服务组：Hrm  
服务：ReplaceArchive
+ old_sign：旧档案签名
+ new_sign：新档案签名

返回：无附带参数

## 2 客户端
### 2.1 独立功能控件
#### 2.1.1 简历预览
简历预览控件  
输入：数据流，或者文件名。  
目标：
+ 可以预览简历文件。
+ 可以缩放，拖拽。

#### 2.1.2 简历上传
简历上传控件  
功能要素：
1. 上传简历  
输入：拖拽或者浏览获取文件路径  
输出：上传[选中的文件路径]  
2. 显示简历上传状态：正在上传、正在比对  
输入：简历状态
3. 显示简历上传结果：  
输入：[未重复|可能重复]
4. 简历信息补全
调用简历信息补全控件
5. 简历信息比对
调用简历信息比对控件

#### 2.1.3 简历信息补全
1. 补全信息  
输入：操作的简历ID
目标：必要的输入控件进行信息补全
输出：补全的信息

#### 2.1.4 简历信息比对
1. 比对简历信息  
输入：相似简历ID列表[简历ID]，正在比对的简历[文件名]  
目标：可以单独拖动每个预览窗的拉条，也可以同步拖动2个预览窗的拉条  
输出：用户选择结果[简历重复][简历不重复]  

### 2.2 业务窗体层次
#### 2.2.1 主界面
要素：
1. 数据总览
2. 功能入口
功能：
1. 简历上传，调用简历上传控件
2. 简历浏览，跳转简历浏览窗体
## 3 通信模块
目前使用ZMQ方案，后期完善通信模块应抽象到方案无关
### 3.1 通信模型
1. 请求回应（Request-Reply）：用于从服务器请求数据，或者更新信息到服务器
2. 管道模型：用于文件上传下载

### 3.2 服务端
1. 回应请求
2. 接收和发送文件

### 3.3 客户端
1. 请求或者更新数据
2. 上传或者下载文件

## 4 数据库
### 4.1 信息管理
#### 4.1.1 档案信息：ArchiveInfo
+ id：自增id
+ name：姓名 默认**
+ birthday：生日 默认********
+ tel_no：电话号码  默认***********
+ post：岗位[QT][C++][产品经理]
+ level：级别[未定：0][初级：10][中级：20][高级：30]
+ type：档案类型
+ src：档案来源（用户ID）
+ mark：档案标记 默认无标记
+ sign：档案签名，对应的简历文件MD5
+ timestamp：变更时间
+ opt_id：操作人
```SQL
CREATE TABLE "ArchiveInfo" (
  "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "name" nchar(8) DEFAULT '***',
  "birthday" date(10) DEFAULT '********',
  "tel_no" char(14) DEFAULT '***********',
  "post" char(8) DEFAULT '',
  "level" integer(4) DEFAULT 0,
  "type" char(4) DEFAULT '',
  "src" integer(8) NOT NULL ON CONFLICT FAIL,
  "mark" char(8) DEFAULT '',
  "sign" char(32) NOT NULL ON CONFLICT FAIL DEFAULT '',
  "timestamp" timestamp NOT NULL,
  "opt_id" INTEGER NOT NULL
)
```

> 一个想法：计划和记录：一系列按条件筛选的计划组成一份记录

#### 4.1.2 计划信息：PlanInfo
+ id：自增id，计划id
+ type：计划类型[filter：筛选计划][invite：邀约计划][interview：面试计划][entry：入职计划][custom：自定义计划]
+ create_time：计划创建时间，何时创建计划
+ node_time：计划节点时间，何时需要执行或完成计划
+ finish_time：计划完成时间
+ related_id：关联id
+ detail：计划详情
+ note：计划便笺，用户根据需要手动记录信息
+ role_create：计划创建人
+ role_targets：计划接收人 默认为创建人
+ role_complete：计划完成人
```SQL
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
  "role_targets" integer(8) NOT NULL,
  "role_complete" integer(8) DEFAULT 0
);
```

#### 4.1.3 用户信息：UserInfo
+ id：自增id
+ name：姓名
+ email：邮箱
+ tel_no：电话号码
+ passwd：密码
+ pow：权限[超级管理员10][部门管理员20][小组管理员30][员工40]
+ department：部门
+ type：用户类型[内部用户：internal][外部用户：external][外部渠道：external_channel][内部渠道：internal_channel]
```SQL
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
```
#### 4.1.4 用户组信息：GroupInfo
+ id：自增id
+ name：用户组名
```SQL
CREATE TABLE "GroupInfo" (
  "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "name" nchar(8) NOT NULL UNIQUE
);
```
#### 4.1.5 用户分组 UserGroup
+ id：自增id
+ user_id：用户id
+ group_id：用户组id
```SQL
CREATE TABLE "main"."Untitled" (
  "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "user_id" INTEGER(8) NOT NULL,
  "group_id" INTEGER(8) NOT NULL
);
```
#### 4.1.5 工作日志 WorkLog
+ id：自增id
+ user_id：用户id
+ operation：操作
+ time：时间
```SQL
CREATE TABLE "WorkLog" (
  "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "user_id" INTEGER(8) NOT NULL,
  "operation" TEXT NOT NULL,
  "time" datetime(20) NOT NULL
);
```

# 5.通信过程
## 5.1 客户端通信过程
代码参考KLWidget  
### 5.1.1 实现通信的前提：  
+ 参与通信的窗体继承自ServiceWidget，见KLWidget
+ 接收返回的方法用Q_INVOKABLE修饰，见KLWidget::BaseLogin

发起请求：  
```C++
//见KLWidget::userLogin  
ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_LOGIN, params, this);
```
接收请求：  
```C++
//见KLWidget::BaseLogin
Q_INVOKABLE void BaseLogin(QVariantMap responseParams);
```
回调函数规则，以KLWidget::userLogin登录流程为例：  
+ 服务组：SERVICE_GROUP_BASE->Base
+ 服务：SERVICE_BASE_LOGIN->Login
+ 回调函数命名：[服务组][服务]->BaseLogin
+ 回调函数参数：固定为一个QVariantMap类型的参数

### 5.1.2 接口请求规则
+ 独立功能控件不建议参与接口调用通信
+ 建议在各业务窗体继承ServiceWidget进行接口调用和接口响应处理，业务只耦合到业务窗体一层
+ 有更好的想法可以尝试改进

## 5.2 服务端通信过程
代码参考ManageToolServer,这一层未进行逻辑抽象了，目前用的socket通信框架，如果需要替换其他通信框架，需要实现对应的消息句柄类，继承自AbsMsgHandle
### 5.2.1 初始化
初始化对应的网络服务，端口监听
```
initSocketServer
```
### 5.2.2 新的连接
创建新的消息句柄，连接销毁请求信号
```C++
ManageToolServer::newSocketConnect()
...C++
connect(msgHandle, &SocketHandle::destroyRequire, this, &ManageToolServer::deleteSocketConnect);
```
### 5.2.3 释放连接
```
void ManageToolServer::deleteSocketConnect()
```
### 5.2.4 消息句柄
socket通信框架对应的消息句柄类就是 SocketHandle：
1. 构造函数连接了2个信号：QTcpSocket::stateChanged和QTcpSocket::readyRead，用于维护连接状态和触发接收数据
2. 实现sendMsg方法，用于发送数据
3. 接收数据后调用抽象方法recvMsg，此方法AbsMsgHandle中有默认实现  

AbsMsgHandle：
1. 构造函数：添加十秒延时用于连接合法性检查
2. 合法性检查：超过十秒未获取到key，就发起销毁请求
```C++
AbsMsgHandle::handleCheck()
```
3. 接收消息：解析获取servicePath（[serviceGroup]+[service]）  
```C++
AbsMsgHandle::recvMsg
```
4. 查找servicePath对应的消息句柄，没有则创建  
```C++
if(!mapSession.contains(servicePath)){//会话不存在
        QObject* sessionObj = SessionFactory::createSession(serviceGroup, service, this);
        if(!sessionObj){
            qDebug()<<"[AbsMsgHandle::recvMsg] create session failed"<<serviceGroup<<service;
            return;
        }
        mapSession[servicePath] = qobject_cast<AbsSession*>(sessionObj);
}
```
5. 调用AbsMsgHandle::sessionStart开始会话  
6. AbsMsgHandle::sessionStart，单次的接口交互流程具有默认实现（即请求可以立即返回结果结束会话的）
### 5.2.5 会话  
#### 5.2.5.1 默认实现:  
```C++
void AbsSession::sessionStart(QVariantMap& sessionData)
{
    srcData = sessionData;
    msgParse();//解析固定字段
    sessionRun();//实施业务流程：数据库操作，响应数据的生成
    sessionResponse();//发送响应数据
}
```
>
> 复杂过程实现：待补充
>
采用默认实现流程的话，一个新的接口只需要继承AbsMsgHandle，然后实现SessionBaseLogin::sessionRun方法，可以参考：
```C++
/**
 * @brief SessionBaseLogin::sessionRun
 * 登录接口逻辑：验证密码，返回登录结果
 * session的最简实现流程就是在sessionRun中使用createResponse生成返回数据responseParams，
 * sessionStart会自动完成数据解析，数据回应过程
 */
void SessionBaseLogin::sessionRun()
{
    // qDebug()<<"[SessionBaseLogin::sessionRun]"<<srcData;

    // Do something
    // 数据库增删改查操作
    // 准备好响应参数map：responseParams
    //.....

    //返回响应数据
    createResponse(true, "登录成功");//使用固定模板生成响应数据，responseParams为各个接口返回数据中的差异部分
}
```
#### 5.2.5.2 接口和会话的对应关系
会话类的命名规则：[Session][服务组][服务]
>例：登录接口，服务组：Base，服务：Login，则登录接口类命名为SessionBaseLogin
>
会话类注册：
> 见会话工厂类：
>
```C++
void SessionFactory::registeSessions()
{
    registeSession(SessionBaseLogin::staticMetaObject);
    //registeSession(SessionXXXXXX::staticMetaObject);
    //......
}
```


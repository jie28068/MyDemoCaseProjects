#include "userbrowser.h"
#include "ui_userbrowser.h"
#include <QTimer>
#include "Base/FormatTranslator.h"

UserBrowser::UserBrowser(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::UserBrowser)
{
    ui->setupUi(this);
    model_group = new MTTableModel(this);
    model_user = new MTTableModel(this);

    filter_group = new QSortFilterProxyModel(this);
    filter_group->setSourceModel(model_group);
    ui->table_group->setModel(filter_group);
    ui->table_group->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    filter_user = new QSortFilterProxyModel(this);
    filter_user->setSourceModel(model_user);
    ui->table_user->setModel(filter_user);
    ui->table_user->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void UserBrowser::initUserModel(QList<QVariantMap> userMap)
{
    QStringList headerList;
    headerList<<"id"<<"name"<<"opt";
    model_user->setHeaderList(headerList);

    QMap<QString, QString> headerMap = {{"id","ID"},{"name","姓名"},{"opt","操作"}};
    model_user->setHeaderMap(headerMap);
    model_user->setDefaultData(2, "添加到组");
    model_user->setData(userMap);

    ui->table_user->setColumnHidden(0, true);
}

void UserBrowser::initGroupModel(QList<QVariantMap> groupMap)
{
    QStringList headerList;
    headerList<<"group_name"<<"user_id"<<"group_id"<<"user_name"<<"opt";
    model_group->setHeaderList(headerList);

    QMap<QString, QString> headerMap = {{"group_name","group_name"},{"user_id","ID"},{"group_id","GID"},{"user_name","姓名"},{"opt","操作"}};
    model_group->setHeaderMap(headerMap);
    model_group->setDefaultData(4, "移除");
    model_group->setData(groupMap);

    ui->table_group->setColumnHidden(0, true);
    ui->table_group->setColumnHidden(1, true);
    ui->table_group->setColumnHidden(2, true);
}

UserBrowser::~UserBrowser()
{
    delete ui;
}

void UserBrowser::on_table_user_clicked(const QModelIndex &index)
{
    qDebug()<<index.row()<<index.column();
    QModelIndex sourceIndex = filter_user->mapToSource(index);
    if(index.column() == 2)//所选用户添加到组
    {
        QVariantMap curUserData = model_user->getCurTableData(sourceIndex.row());
        int gID = curGroupID();
        if(gID == 0)
        {
            showMsg(false, "用户无法加入空的用户组");
            return;
        }

        groupUserUpdate(curGroupID(), curUserData["id"].toInt(), "insert");

        qDebug()<<"add to group"<<curUserData["name"]<<curUserData["id"].toInt()<<curGroupID();
    }
    else//选择用户
    {
        QVariantMap curUserData = model_user->getCurTableData(sourceIndex.row());
        updateTargetRole(curUserData["id"].toInt(), curUserData["name"].toString());
    }
}

void UserBrowser::on_table_group_clicked(const QModelIndex &index)
{
    qDebug()<<index.row()<<index.column();
    QModelIndex sourceIndex = filter_group->mapToSource(index);
    if(sourceIndex.column() == 4)//移除所选用户
    {
        QVariantMap curGroupData = model_group->getCurTableData(sourceIndex.row());
        groupUserUpdate(curGroupData["group_id"].toInt(), curGroupData["user_id"].toInt(), "delete");
        qDebug()<<"del from group"<<curGroupData["user_name"].toString()<<curGroupData["user_id"].toInt()<<curGroupData["group_id"].toInt();
    }
    else
    {

    }
}

int UserBrowser::curGroupID()
{
    return groupMap.value(curGroupName(), 0);
}

QString UserBrowser::curGroupName()
{
    QString comStr = ui->com_group->currentText();

    if(groupMap.contains(comStr))
        return comStr;

    return QString();
}

void UserBrowser::leaveEvent(QEvent *event)
{
    if(!flag_com)
        this->hide();
    QWidget::leaveEvent(event);
}

void UserBrowser::mouseMoveEvent(QMouseEvent *event)
{
    flag_com = ui->com_group->geometry().contains(event->pos());
    QWidget::mouseMoveEvent(event);
}

void UserBrowser::showEvent(QShowEvent *event)
{
    queryGroupUsers(0);
    QWidget::showEvent(event);
}

void UserBrowser::on_btn_create_group_clicked()
{
    createUserGroup(ui->com_group->currentText());
}


void UserBrowser::on_com_group_currentTextChanged(const QString &arg1)
{
    //选择用户组
}

void UserBrowser::showMsg(bool success, QString msg)
{
    ui->lab_msg->setProperty("success",success);
    ui->lab_msg->setText(msg);
    style()->unpolish(ui->lab_msg);
    style()->polish(ui->lab_msg);

    QTimer::singleShot(2000, ui->lab_msg, &QLabel::clear);
}

void UserBrowser::createUserGroup(QString groupName)
{
    QVariantMap params;
    params["name"] = groupName;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_USERGROUPCREATE, params,this);
}

void UserBrowser::BaseUserGroupCreate(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);
}

void UserBrowser::queryGroupInfo()
{
    QVariantMap params;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_QUERYGROUPINFO, params, this);
}

void UserBrowser::BaseQueryGroupInfo(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);

    QVariantMap params = responseMap["Params"].toMap();
    QVariantMap group_info = params["group_info"].toMap();

    QVariantList groupList = group_info["list_group"].toList();
    QVariantList groupInfoList = group_info["list_group_info"].toList();
    groupMap = FormatTransLator::QVariantListToQMap<QString, int>(groupList, "name", "id");
    qDebug()<<groupMap;

    QStringList groupNames = groupMap.keys();
    updateGroupList(groupNames);

    QList<QVariantMap> group_list_map = FormatTransLator::QVariantToMapList(groupInfoList);
    initGroupModel(group_list_map);
//    ui->com_group->setCurrentIndex(0);
}

void UserBrowser::updateGroupList(QStringList list)
{
    if(list.isEmpty())
        return;

    int index = ui->com_group->currentIndex();
    ui->com_group->clear();
    ui->com_group->addItems(list);
    if(index < ui->com_group->count())
        ui->com_group->setCurrentIndex(index);
}


void UserBrowser::queryGroupUsers(int groupID)
{
    QVariantMap params;
    params["id"] = groupID;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_QUERYGROUPUSERS, params, this);
}

void UserBrowser::BaseQueryGroupUsers(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);

    QVariantMap params = responseMap["Params"].toMap();
    QVariantList user_list = params["user_list"].toList();//用户列表
    QList<QVariantMap> user_list_map = FormatTransLator::QVariantToMapList(user_list);
    int group_id = params["group_id"].toInt();//组ID
    if(group_id == 0){
        initUserModel(user_list_map);
        queryGroupInfo();//查询用户组信息
    }
}

void UserBrowser::groupUserUpdate(int groupID, int userID, QString opt)
{
    QVariantMap params;
    params["opt"] = opt;
    params["group_id"] = groupID;
    params["user_id"] = userID;

    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_USERGROUPUPDATE, params, this);
}

void UserBrowser::BaseUserGroupUpdate(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);
    if(success)
    {
        queryGroupUsers(0);
    }
}

void UserBrowser::on_com_group_activated(int index)
{
    QString curGroup = ui->com_group->itemText(index);
    if(curGroup.isEmpty())
        return;

    filter_group->setFilterKeyColumn(0);
    filter_group->setFilterRegExp(QString("^%1$").arg(curGroup));

    int gID = curGroupID();
    if(gID == 0)
    {
        showMsg(false, "无法选择空的用户组创建计划");
        return;
    }
    QString gName = curGroupName();
    emit updateGroupRole(gID, gName);
}


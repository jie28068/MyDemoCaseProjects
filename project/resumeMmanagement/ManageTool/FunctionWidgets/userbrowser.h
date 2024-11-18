#ifndef USERBROWSER_H
#define USERBROWSER_H

#include <QWidget>
#include <QVariantMap>
#include <QStringList>
#include <QSortFilterProxyModel>
#include <QMouseEvent>
#include "Model/mttablemodel.h"
#include "serviceobject.h"
#include "managetoolclient.h"
#include "ReplyAndService/ServiceDefines.h"

namespace Ui {
class UserBrowser;
}

class UserBrowser : public QWidget, ServiceObject
{
    Q_OBJECT

public:
    explicit UserBrowser(QWidget *parent = nullptr);
    void initUserModel(QList<QVariantMap> userMap);
    void initGroupModel(QList<QVariantMap> groupMap);
    ~UserBrowser();

    QList<int> selList;

public slots:
    Q_INVOKABLE void BaseUserGroupCreate(QVariantMap responseMap);
    Q_INVOKABLE void BaseQueryGroupInfo(QVariantMap responseMap);
    Q_INVOKABLE void BaseQueryGroupUsers(QVariantMap responseMap);
    Q_INVOKABLE void BaseUserGroupUpdate(QVariantMap responseMap);
signals:
    void updateGroupRole(int id, QString name);
    void updateTargetRole(int id, QString name);


private slots:
    void on_table_user_clicked(const QModelIndex &index);
    void on_table_group_clicked(const QModelIndex &index);

    void on_btn_create_group_clicked();

    void on_com_group_currentTextChanged(const QString &arg1);

    void on_com_group_activated(int index);

private:
    Ui::UserBrowser *ui;
    MTTableModel* model_group;
    MTTableModel* model_user;
    QMap<QString, int> groupMap;

    bool flag_com;

    QSortFilterProxyModel* filter_group;
    QSortFilterProxyModel* filter_user;

    void groupUserUpdate(int groupID, int userID, QString opt);

    //server interface
    void createUserGroup(QString groupName);
    void queryGroupInfo();
    void queryGroupUsers(int groupID);

    // QWidget interface
    void showMsg(bool success, QString msg);
    int curGroupID();
    QString curGroupName();
    void updateGroupList(QStringList list);
protected:
    void leaveEvent(QEvent *event) override;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // USERBROWSER_H

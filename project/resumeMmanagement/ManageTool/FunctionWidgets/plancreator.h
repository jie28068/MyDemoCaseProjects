#ifndef PLANCREATOR_H
#define PLANCREATOR_H

#include <QWidget>
#include <QVariantMap>
#include <QSortFilterProxyModel>
#include "Model/mttablemodel.h"
#include "userbrowser.h"
#include "serviceobject.h"
#include "HRMTool/resumepreviewer.h"

namespace Ui {
class PlanCreate;
}

class PlanCreator : public QWidget, ServiceObject
{
    Q_OBJECT

public:
    explicit PlanCreator(QWidget *parent = nullptr);
    ~PlanCreator();

    enum PlanType{
        PLAN_FILTER,//简历过滤
        PLAN_INVITE,//邀约
    };
    Q_ENUM(PlanType)

    void planCreateStart(PlanType planType , QStringList relatedList);

public slots:
    Q_INVOKABLE void HrmFilterResume(QVariantMap responseMap);
    Q_INVOKABLE void PlanCreate(QVariantMap responseParams);
signals:
    void createPlanInfo(QVariantMap info);

private slots:
    void updateGroupRole(int gID, QString gName);
    void updateTargetRole(int id, QString name);
    void on_btn_sel_receiver_clicked();
    void on_btn_create_plan_clicked();
    void updateRelatedResume(QStringList relatedList);//鏇存柊鍏宠仈绠€鍘
    void on_table_archive_clicked(const QModelIndex &index);

    void on_table_attached_clicked(const QModelIndex &index);

private:
    Ui::PlanCreate *ui;
    QVariantMap params;
    QMap<PlanType ,QString> mapPlanType;
    QMap<QString ,QString> mapPlanTranslate;
    QMap<QString ,ResumePreviewer*> mapPreviewer;
    UserBrowser* frm_user_browser;
    QStringList relatedIDList;
    MTTableModel* model;
    QSortFilterProxyModel* filter_archive;
    QSortFilterProxyModel* filter_attached;

    void initModels();
    void initModelData(QList<QVariantMap> &dataMap);
    void filterResume(QString conditions);
    void create(QVariantMap params);
    void showMsg(bool success, QString msg);
    void addPreviewer(QString sign, QString name);
    void delPreviewer(QString sign);

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // PLANCREATOR_H

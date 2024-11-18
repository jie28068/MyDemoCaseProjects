#ifndef PLANBROWSER_H
#define PLANBROWSER_H

#include <QWidget>
#include "serviceobject.h"
#include <QListWidgetItem>
#include <QSortFilterProxyModel>
#include "Model/mttablemodel.h"

namespace Ui {
class PlanBrowser;
}

class PlanBrowser : public QWidget,public ServiceObject
{
    Q_OBJECT

public:
    explicit PlanBrowser(QWidget *parent = nullptr);
    ~PlanBrowser();
    void showEvent(QShowEvent *event);
public slots:
    Q_INVOKABLE void PlanUpdate(QVariantMap responseMap);
    Q_INVOKABLE void PlanFinish(QVariantMap responseMap);
    Q_INVOKABLE void HrmUpdateArchive(QVariantMap responseMap);
private slots:
    void on_comboBox_currentIndexChanged(int index);


//    void on_listWidget_itemActivated(QListWidgetItem *item);
    void on_tableView_clicked(const QModelIndex &index);
    void on_btn_no_pass_clicked();
    void on_btn_pass_clicked();
    void on_check_me_toggled(bool checked);

private:
    Ui::PlanBrowser *ui;
    MTTableModel *tabelModel;
    QSortFilterProxyModel* filter_plan;
    int currow;


    void setBtnPassState(bool enable);
    void planUpdate(QString note);
    void archiveUpdate(QString);
    void initUi();
    void setUpList(QVariantList);
    /**
     * @brief PlanBrowser::getPlanInfo 调用接口获取计划列表
     * @param type 计划类型
     * @param role 角色(未使用)
     */
    void getPlanInfo(QString type, bool finished="");
    /**
     * @brief PlanQuery 接口回调函数
     * @param responseParams 返回参数
     */
    Q_INVOKABLE void PlanQuery(QVariantMap responseParams);
    void showMsg(bool success, QString msg);
    int curPlanID();
    QString curArchiveID();
    void planComplete();
    void initModels();
    void updateCurPlan(int row);
};

#endif // PLANBROWSER_H

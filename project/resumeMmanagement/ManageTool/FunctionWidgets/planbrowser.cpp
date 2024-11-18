#include "planbrowser.h"
#include "ui_planbrowser.h"
#include "planitemwidget.h"
#include "Base/PathTool.h"
#include <QTimer>

PlanBrowser::PlanBrowser(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::PlanBrowser)
{
    ui->setupUi(this);
    initUi();
    initModels();
    ui->frm_plan_execute->hide();
    currow = -1;
}

PlanBrowser::~PlanBrowser()
{
    delete ui;
    delete tabelModel;
}

void PlanBrowser::showEvent(QShowEvent *event)
{
    getPlanInfo("filter",false);
    return QWidget::showEvent(event);
}

void PlanBrowser::initModels()
{
    tabelModel = new MTTableModel();
    QStringList headList({"name","create_time","name_create","is_related","status","finished"});
    QMap<QString,QString>headmap;
    headmap.insert("name","名字");
    headmap.insert("create_time","创建时间");
    headmap.insert("name_create","创建人");
    headmap.insert("is_related","相关");
    headmap.insert("status","状态");
    headmap.insert("finished","是否结束");
    tabelModel->setHeaderList(headList);
    tabelModel->setHeaderMap(headmap);

    filter_plan = new QSortFilterProxyModel(this);
    filter_plan->setSourceModel(tabelModel);
    filter_plan->setFilterKeyColumn(5);
    filter_plan->setFilterFixedString("false");

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setModel(filter_plan);
    ui->tableView->setColumnHidden(3,true);
    ui->tableView->setColumnHidden(4, true);
    ui->tableView->setColumnHidden(5, true);
}

void PlanBrowser::updateCurPlan(int row)
{
    if(row < 0)
        return;
    if(row >= tabelModel->rowCount())
        return;

    QVariantMap map = tabelModel->getTableData().at(row);
    qDebug()<<"PlanBrowser::on_tableView_clicked"<<row<<map.value("name").toString();
    QString filePath = PathTool::clientCache() + QString("/%1.%2").arg(map.value("sign").toString(), map.value("format").toString());
    ui->frm_resume_preview->previewLocalFile(filePath);
    ui->lab_note->setText(map.value("note").toString());
}

void PlanBrowser::initUi()
{
     //添加计划类型[filter：筛选计划][invite：邀约计划][interview：面试计划][entry：入职计划][custom：自定义计划]
//    ui->comboBox->addItem("筛选计划","filter");
//    ui->comboBox->addItem("邀约计划","invite");
//    ui->comboBox->addItem("面试计划","interview");
//    ui->comboBox->addItem("入职计划","entry");
//    ui->comboBox->addItem("自定义计划","custom");

}

void PlanBrowser::setUpList(QVariantList planList)
{
//    ui->listWidget->clear();
//    foreach(QVariant plan,planList){
//        QVariantMap map = plan.toMap();

//        QListWidgetItem *item = new QListWidgetItem();

//        item->setSizeHint(QSize(ui->listWidget->width(),80));
//        PlanItemWidget * itemwidget = new PlanItemWidget(map);
//        ui->listWidget->addItem(item);
//        ui->listWidget->setItemWidget(item,itemwidget);
//    }
    QList<QVariantMap> mapList;
    foreach(QVariant plan,planList){
        QVariantMap map = plan.toMap();
        mapList.append(map);
    }

    qDebug()<<mapList;
    tabelModel->setData(mapList);
    if(ui->check_me->isChecked()){
        filter_plan->setFilterKeyColumn(3);
        filter_plan->setFilterFixedString("true");
    }
    else{
        filter_plan->setFilterKeyColumn(3);
        filter_plan->setFilterRegExp(".*");
    }

    updateCurPlan(currow);
}
/**
 * @brief PlanBrowser::getPlanInfo 调用接口获取计划列表
 * @param type 计划类型
 * @param role 角色(未使用)
 */
void PlanBrowser::getPlanInfo(QString type, bool finished)
{
    QVariantMap params;
    params["type"] = type;
    params["finished"] = finished;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_PLAN, SERVICE_PLAN_QUERY, params, this);
//    ManageToolClient::instance().serviceReply(SERVICE_GROUP_PLAN, SERVICE_PLAN_STATIS, params, this);
}
/**
 * @brief PlanQuery 接口回调函数
 * @param responseParams 返回参数
 */
void PlanBrowser::PlanQuery(QVariantMap responseParams)
{
    qDebug()<<"PlanQuery";
    QVariantMap parmas = responseParams.value("Params").toMap();
    QVariantList planlist = parmas.value("plan_list").toList();
    setUpList(planlist);
}

void PlanBrowser::on_comboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
//    if(!ManageToolClient::instance().getUserInfo()->login())
//        return;

//    qDebug()<<"PlanBrowser::on_comboBox_currentIndexChanged"<<index;
//    getPlanInfo(ui->comboBox->currentData().toString());

}

/**
 * @brief PlanBrowser::on_btn_pass_clicked  筛选通过，完成计划
 */
void PlanBrowser::on_btn_pass_clicked()
{
    qDebug()<<"PlanBrowser::on_btn_pass_clicked()";
    if(!ui->tableView->currentIndex().isValid())return;
    planComplete();
}

/**
 * @brief PlanBrowser::on_btn_no_pass_clicked   筛选不通过，更新note
 */
void PlanBrowser::on_btn_no_pass_clicked()
{
    qDebug()<<"PlanBrowser::on_btn_no_pass_clicked()";
    if(!ui->tableView->currentIndex().isValid())return;
    planUpdate(ui->edit_note->toPlainText());    
}

void PlanBrowser::setBtnPassState(bool enable)
{
    ui->btn_no_pass->setEnabled(enable);
    ui->btn_pass->setEnabled(enable);
}

void PlanBrowser::showMsg(bool success, QString msg)
{
    ui->lab_msg->setProperty("success",success);
    ui->lab_msg->setText(msg);
    style()->unpolish(ui->lab_msg);
    style()->polish(ui->lab_msg);

    QTimer::singleShot(2000, ui->lab_msg, &QLabel::clear);
}

void PlanBrowser::planUpdate(QString note)
{
    if(note.isEmpty()){
        note = "不通过";
    }

    QVariantMap params;
    params["id"] = curPlanID();
    params["new_note"] = note;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_PLAN, SERVICE_PLAN_UPDATE, params, this);

//    archiveUpdate(curSign(),"初筛不通过");
}

void PlanBrowser::archiveUpdate(QString type)
{
    QVariantMap params;
    params["id"] = curArchiveID();
    params["type"] = type;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_UPDATEARCHIVE, params, this);
}

void PlanBrowser::HrmUpdateArchive(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);
    if(success)
        getPlanInfo("filter",false);
}

void PlanBrowser::PlanUpdate(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);
    if(success)
    {
        getPlanInfo("filter",false);
    }
}

void PlanBrowser::planComplete()
{
    QVariantMap params;
    params["id"] = curPlanID();
    params["result"] = "筛选通过";
    params["note"] = ui->edit_note->toPlainText();
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_PLAN, SERVICE_PLAN_FINISH, params, this);
}

void PlanBrowser::PlanFinish(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();
    showMsg(success, msg);
    if(success)
        archiveUpdate("初筛通过");
}


int PlanBrowser::curPlanID()
{
    return tabelModel->getCurTableData(ui->tableView->currentIndex().row()).value("id").toInt();
}

QString PlanBrowser::curArchiveID()
{
    return tabelModel->getCurTableData(ui->tableView->currentIndex().row()).value("related_id").toString();
}

//void PlanBrowser::on_listWidget_itemActivated(QListWidgetItem *item)
//{
//    PlanItemWidget * itemwidget = (PlanItemWidget*)(ui->listWidget->itemWidget(item));
//    if(itemwidget){
//        QVariantMap datamap = itemwidget->getDatamap();
//        ui->textEdit->setText(datamap.value("note").toString());
//    }
//}


void PlanBrowser::on_tableView_clicked(const QModelIndex &index)
{
    QModelIndex sourceIndex = filter_plan->mapToSource(index);

    if(currow==sourceIndex.row())return;

    currow = sourceIndex.row();
    updateCurPlan(currow);

//    QVariantMap map = tabelModel->getTableData().at(sourceIndex.row());
//    qDebug()<<"PlanBrowser::on_tableView_clicked"<<sourceIndex.row()<<map.value("name").toString();
//    QString filePath = PathTool::clientCache() + QString("/%1.%2").arg(map.value("sign").toString(), map.value("format").toString());
//    ui->frm_resume_preview->previewLocalFile(filePath);
//    ui->lab_note->setText(map.value("note").toString());
}


void PlanBrowser::on_check_me_toggled(bool checked)
{
    if(checked){
        filter_plan->setFilterKeyColumn(3);
        filter_plan->setFilterFixedString("true");
    }
    else{
        filter_plan->setFilterKeyColumn(3);
        filter_plan->setFilterRegExp(".*");
    }
}


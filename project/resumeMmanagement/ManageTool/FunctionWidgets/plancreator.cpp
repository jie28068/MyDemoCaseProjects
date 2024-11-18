#include "plancreator.h"
#include "ui_plancreator.h"
#include "ReplyAndService/ServiceDefines.h"
#include <QStyle>
#include <QTimer>
#include "Base/FormatTranslator.h"

PlanCreator::PlanCreator(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::PlanCreate)
{
    ui->setupUi(this);
    mapPlanType = {{PLAN_FILTER,"简历筛选"},
                   {PLAN_INVITE,"面试邀约"}};

    mapPlanTranslate = {{"filter","简历筛选"},
                        {"invite","面试邀约"}};

    frm_user_browser = new UserBrowser(this);
    connect(frm_user_browser, &UserBrowser::updateGroupRole, this, &PlanCreator::updateGroupRole);
    connect(frm_user_browser, &UserBrowser::updateTargetRole, this, &PlanCreator::updateTargetRole);
    frm_user_browser->hide();

    initModels();
}

PlanCreator::~PlanCreator()
{
    delete ui;
}

void PlanCreator::planCreateStart(PlanType planType, QStringList relatedList)
{
    ui->date_node_time->setDateTime(QDateTime::currentDateTime());
    ui->edit_detail->setText("筛选计划：请查看简历并完成筛选");
    ui->lab_plan_type->setText(mapPlanType[planType]);
    if(relatedIDList == relatedList)
        return;

    relatedIDList = relatedList;

    switch (planType) {
    case PLAN_FILTER:
        updateRelatedResume(relatedIDList);
        break;

    case PLAN_INVITE:
        break;

    default:
        break;
    }
}

void PlanCreator::updateGroupRole(int gID, QString gName)
{
    params["role_target"]=0;
    params["role_group"] = gID;
    ui->btn_sel_receiver->setText(gName);
}

void PlanCreator::updateTargetRole(int id, QString name)
{
    params["role_target"]=id;
    params["role_group"] = 0;
    ui->btn_sel_receiver->setText(name);
}

//选择接收人
void PlanCreator::on_btn_sel_receiver_clicked()
{
    QPoint pos = ui->btn_sel_receiver->geometry().bottomLeft();
    QPoint showPos = ui->frame_6->mapTo(this, pos);
    frm_user_browser->move(showPos);
    frm_user_browser->setMaximumWidth(ui->btn_sel_receiver->geometry().width());
    frm_user_browser->show();
}


void PlanCreator::on_btn_create_plan_clicked()
{
    if((!params.contains("role_target")) && (!params.contains("role_group")))
    {
        showMsg(false, "请选择计划接收人或接收用户组");
        return;
    }

    relatedIDList = model->getColumn("id", filter_attached);
    if(relatedIDList.isEmpty())
    {
        showMsg(false, "创建筛选计划失败，请选择简历附件");
        return;
    }

    params["type"] = mapPlanTranslate.key(ui->lab_plan_type->text());
    params["node_time"] = ui->date_node_time->text();
    params["detail"] = ui->edit_detail->toPlainText();
    params["related_id_list"] = relatedIDList;

    create(params);
}

void PlanCreator::updateRelatedResume(QStringList relatedList)
{
    Q_UNUSED(relatedList);
}

void PlanCreator::initModels()
{
    model = new MTTableModel(this);
    QStringList headerList;
    headerList<<"selected"<<"name"<<"date";
    QMap<QString, QString>headerMap = {
                                        {"selected","selected"},
                                        {"name","姓名"},
                                        {"date","入库时间"}
                                      };
    model->setHeaderList(headerList);
    model->setHeaderMap(headerMap);
    model->setDefaultData(0, false);

    filter_archive = new QSortFilterProxyModel(this);
    filter_archive->setSourceModel(model);
    ui->table_archive->setModel(filter_archive);
    ui->table_archive->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_archive->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->table_archive->setColumnHidden(0,true);

    filter_attached = new QSortFilterProxyModel(this);
    filter_attached->setSourceModel(model);
    filter_attached->setFilterKeyColumn(0);
    filter_attached->setFilterFixedString("true");
    ui->table_attached->setModel(filter_attached);
    ui->table_attached->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_attached->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->table_attached->setColumnHidden(0,true);
}

void PlanCreator::initModelData(QList<QVariantMap>& dataMap)
{
    model->setData(dataMap);
}

void PlanCreator::create(QVariantMap params)
{
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_PLAN, SERVICE_PLAN_CREATE, params, this);
}

void PlanCreator::PlanCreate(QVariantMap responseParams)
{
    qDebug()<<"PlanCreate";
    bool success = responseParams["Success"].toBool();
    QString msg = responseParams["Result"].toString();

    showMsg(success, msg);
}

void PlanCreator::filterResume(QString conditions)
{
    QVariantMap params;
    params["Conditions"] = conditions;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_FILTERRESUME, params, this);
}

void PlanCreator::HrmFilterResume(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString msg = responseMap["Result"].toString();

    showMsg(success, msg);
    if(success)
    {
        QVariantMap params = responseMap["Params"].toMap();
        QVariantList resume_list = params["resume_list"].toList();
        QList<QVariantMap> resume_map = FormatTransLator::QVariantToMapList(resume_list);
        initModelData(resume_map);
//        foreach(QVariant resume, resume_list)
//        {
//            QVariantMap resumeMap = resume.toMap();
//            QString sign = resumeMap["sign"].toString();
//            QString name = resumeMap["name"].toString();

//            ResumePreviewer* frm_preview = new ResumePreviewer;
//            frm_preview->previewResume(sign);
//            ui->frm_attached->addTab(frm_preview, name);
//        }

    }
}

void PlanCreator::showMsg(bool success, QString msg)
{
    ui->lab_msg->setProperty("success",success);
    ui->lab_msg->setText(msg);
    style()->unpolish(ui->lab_msg);
    style()->polish(ui->lab_msg);

    QTimer::singleShot(2000, ui->lab_msg, &QLabel::clear);
}

void PlanCreator::addPreviewer(QString sign, QString name)
{
    if(mapPreviewer.contains(sign))
        return;

    ResumePreviewer* frm_previewer = new ResumePreviewer;
    frm_previewer->previewResume(sign);
    mapPreviewer.insert(sign, frm_previewer);
    ui->frm_attached->addTab(frm_previewer, name);
}

void PlanCreator::delPreviewer(QString sign)
{
    if(!mapPreviewer.contains(sign))
        return;

    ResumePreviewer* frm_previewer = mapPreviewer[sign];
    mapPreviewer.remove(sign);

    int index = ui->frm_attached->indexOf(frm_previewer);
    ui->frm_attached->removeTab(index);

    frm_previewer->deleteLater();
}

void PlanCreator::showEvent(QShowEvent *event)
{
    filterResume("a.type='初次入库'");
    QWidget::showEvent(event);
}

void PlanCreator::on_table_archive_clicked(const QModelIndex &index)
{
    QModelIndex source_index = filter_archive->mapToSource(index);
    model->setTableData(source_index.row(), "selected", true);
    QVariantMap curRowMap = model->getCurTableData(source_index.row());

    addPreviewer(curRowMap["sign"].toString(), curRowMap["name"].toString());
}


void PlanCreator::on_table_attached_clicked(const QModelIndex &index)
{
    QModelIndex source_index = filter_attached->mapToSource(index);
    model->setTableData(source_index.row(), "selected", false);
    QVariantMap curRowMap = model->getCurTableData(source_index.row());

    delPreviewer(curRowMap["sign"].toString());
}


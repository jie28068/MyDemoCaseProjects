#include "archivebrowser.h"
#include "ui_archivebrowser.h"
#include <QtDebug>
#include <QStringList>
#include <QTimer>


ArchiveBrowser::ArchiveBrowser(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::ArchiveBrowser)
{
    ui->setupUi(this);
    ui->frm_archive_info->setOptFlag(ArchiveInfo::OPT_UPDATE);
    ui->table_archive->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList headerList;
    headerList<<QString("name")
           <<QString("post")
          <<QString("levelname")
         <<QString("type");
    model.setHeaderList(headerList);


    QMap<QString, QString> headerMap = {{"name","姓名"},
                                        {"post","岗位"},
                                        {"levelname","级别"},
                                        {"type","类型"}};

    model.setHeaderMap(headerMap);

//    connect(ui->frm_archive_info, &ArchiveInfo::archiveInfoUpdate, this, &ArchiveBrowser::archiveInfoUpdate);
//    connect(&ManageToolClient::instance(), &ManageToolClient::streamRst, this, &ArchiveBrowser::resumeReceiveRst);
    connect(ui->btn_archive_update, &QPushButton::clicked, this, &ArchiveBrowser::archiveUpdate);
}

ArchiveBrowser::~ArchiveBrowser()
{
    delete ui;
}

QStringList ArchiveBrowser::getPlanList()
{
    return planIDList;
}

void ArchiveBrowser::clearPlanList()
{
    planIDList.clear();
}

void ArchiveBrowser::queryArchiveList(QString condition)
{
    QVariantMap params;
    //add conditions...
    Q_UNUSED(condition);

    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_FILTERRESUME, params, this);
}

void ArchiveBrowser::resumePreview(QString sign, QString format)
{
    QString filePath = PathTool::clientCache()+QString("/%1.%2").arg(sign,format);
    ui->frm_resume_preview->previewLocalFile(filePath);
//    ui->frm_resume_preview->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
//    ui->frm_resume_preview->setProperty("DisplayAlerts", false);
//    ui->frm_resume_preview->setControl(filePath);
}

void ArchiveBrowser::resumePreview(QString filePath)
{
    ui->frm_resume_preview->previewLocalFile(filePath);
//    ui->frm_resume_preview->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
//    ui->frm_resume_preview->setProperty("DisplayAlerts", false);
//    auto rect =ui->frm_resume_preview->geometry();
//    ui->frm_resume_preview-> setGeometry(rect);
//    ui->frm_resume_preview->setControl(filePath);
}

void ArchiveBrowser::recvResult(bool success, QString msg)
{
    ui->lab_msg->setProperty("success",success);
    style()->unpolish(ui->lab_msg);
    style()->polish(ui->lab_msg);
    ui->lab_msg->setText(msg);
    QTimer::singleShot(3000, ui->lab_msg, &QLabel::clear);
}

void ArchiveBrowser::archiveInfoUpdate(QVariantMap info)
{
//    qDebug()<<"archiveInfoUpdate"<<info;
    QVariantMap params = info;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_UPDATEARCHIVE, params, this);
}

void ArchiveBrowser::HrmUploadArchive(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString result = responseMap["Result"].toString();
    recvResult(success, result);
}

void ArchiveBrowser::HrmUpdateArchive(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString result = responseMap["Result"].toString();
    recvResult(success, result);
}


void ArchiveBrowser::HrmFilterResume(QVariantMap responseMap)
{
    qDebug()<<"HrmFilterResume";
    QVariantMap params = responseMap["Params"].toMap();
    QVariantList resume_list = params["resume_list"].toList();
    QList<QVariantMap> list;
    foreach(QVariant resume, resume_list)
    {
        list<<resume.toMap();
    }
    ui->frm_archive_info->setData(list);

    model.setData(list);
    filter_archive = new SortFilterProxyModel(this);
    filter_archive->setSourceModel(&model);
    ui->table_archive->setModel(filter_archive);
    //ui->table_archive->setModel(&model);
}

void ArchiveBrowser::on_btn_filterResume_clicked()
{
    queryArchiveList();
}

void ArchiveBrowser::on_table_archive_clicked(const QModelIndex &index)
{
    ui->frm_archive_info->setCurIndex(index.row());
    QVariantMap map = ui->frm_archive_info->getArchiveInfo();
    QString sign = map["sign"].toString();
    ui->frm_resume_preview->previewResume(sign);

//    QString archiveFile = PathTool::findArchiveFile(sign);
//    if(archiveFile.isEmpty())
//        queryResume(sign);
//    else
//        resumePreview(archiveFile);
}


void ArchiveBrowser::archiveUpdate()
{
    QVariantMap archiveInfo = ui->frm_archive_info->getArchiveInfo();
    if(archiveInfo.isEmpty())
        return;

    archiveInfoUpdate(archiveInfo);
    queryArchiveList();
}

//void ArchiveBrowser::on_btn_add_plan_clicked()
//{
//    QModelIndex index = ui->table_archive->currentIndex();
//    QVariantMap curArchiveInfo = model.getCurTableData(index.row());
//    QString id = curArchiveInfo["id"].toString();
//    QString name = curArchiveInfo["name"].toString();
//    if(planIDList.contains(id))
//        recvResult(false, QString("添加失败，%1已经添加到筛选列表").arg(name));
//    else
//    {
//        planIDList<<id;
//        recvResult(true, QString("%1已添加到筛选列表，当前选了%2份简历").arg(name).arg(planIDList.count()));
//    }
//}



void ArchiveBrowser::on_comboBox_1_currentTextChanged(const QString &arg1)
{
    if(arg1=="默认")
    {
        filter_archive->setRxCol1("");
    }
    else
    {
        filter_archive->setRxCol1(arg1);
    }
    filter_archive->setSourceModel(&model);

    filter_archive->setFilterRegExp(arg1);
}


void ArchiveBrowser::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    if(arg1=="默认")
    {
        filter_archive->setRxCol2("");
    }
    else
    {
        filter_archive->setRxCol2(arg1);
    }
    filter_archive->setSourceModel(&model);

    filter_archive->setFilterRegExp(arg1);
}


void ArchiveBrowser::on_comboBox_3_currentTextChanged(const QString &arg1)
{
    if(arg1=="默认")
    {
        filter_archive->setRxCol3("");
    }
    else
    {
        filter_archive->setRxCol3(arg1);
    }
    filter_archive->setSourceModel(&model);

    filter_archive->setFilterRegExp(arg1);
}


void ArchiveBrowser::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        filter_archive->setRxCol0("");
    }
    else
    {
        filter_archive->setRxCol0(arg1);
    }
    filter_archive->setSourceModel(&model);

    filter_archive->setFilterRegExp(arg1);
}


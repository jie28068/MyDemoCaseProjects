#include "uploadresume.h"
#include "ui_uploadresume.h"
#include "Base/SignTool.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QUrl>
#include <QStyle>
#include <QTimer>
#include <QTime>
#include "Base/PathTool.h"

#define RETURN_FAIL   "-1"
#define RETURN_SUCCEED   "0"

UploadResume::UploadResume(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::UploadResume)
{
    ui->setupUi(this);
//    ui->textEdit->setAcceptDrops(false);
    ui->frm_archive_info->setOptFlag(ArchiveInfo::OPT_COMMIT);
    setAcceptDrops(true);
    contextMenu = new QMenu;
    delAct = new QAction("清空信息", this);
    contextMenu->addAction(delAct);
    connect(contextMenu, &QMenu::triggered, this, &UploadResume::clearProcessInf);


//    connect(ui->frm_archive_info,&ArchiveInfo::archiveInfoCommit, this, &UploadResume::confirmArchiveInfo);
    connect(ui->page_compare, &ArchiveCompare::compareBack, this, &UploadResume::compareOptCancel);
    connect(ui->page_compare, &ArchiveCompare::compareReplace, this, &UploadResume::compareOptReplace);
//    connect(&ManageToolClient::instance(), &ManageToolClient::streamRst, this, &UploadResume::resumeReceiveRst);
    connect(this,&UploadResume::switchPreviewDisplay, ui->frm_preview, &ResumePreviewer::switchPreviewWidget);

}

UploadResume::~UploadResume()
{
    delete ui;
}

bool UploadResume::previewFile(QString path)
{
    qDebug()<<"previewFile"<<path;
    ui->frm_preview->previewLocalFile(path);
    return true;
}

QString UploadResume::GetResumeFormat(const QString &strFilePath)
{
    QFileInfo fileInfo(strFilePath);
    QString format = fileInfo.suffix();

    return format;
}

QString UploadResume::Text = NULL;
QString UploadResume::birthday = NULL;

void UploadResume::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->acceptProposedAction();
        emit switchPreviewDisplay(0);
    }else{
        event->ignore();
    }
}

void UploadResume::dropEvent(QDropEvent *event)
{

    const QMimeData* mimeData = event->mimeData();
    if(mimeData->hasUrls()){
        QList<QUrl>urlList = mimeData->urls();
        strFilePath = urlList.at(0).toLocalFile();
        previewFile(strFilePath);
        uploadReply();
    }
}

void UploadResume::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint i = ui->list_textMsg->mapFromGlobal(event->globalPos());
    QRect rect(QPoint(0, 0), ui->list_textMsg->size());
    if(rect.contains(i)){
        contextMenu->exec(event->globalPos());
    }
}

void UploadResume::uploadReply()
{
    QFile localFile(strFilePath);
    if (!localFile.open(QIODevice::ReadOnly)){
        qDebug() << "file open error.";
        return;
    }
    fileSign = SignTool::SignForStream(localFile.readAll());
    QVariantMap params;

    params["format"]=GetResumeFormat(strFilePath);
    params["sign"] = fileSign;
    params["size"] = localFile.size();

    Text=GetaText::GetTextfromPdforWord(strFilePath,GetResumeFormat(strFilePath));

    //提取文本中 姓名、生日、经历段等数据
    RegularExpression *Regular=new RegularExpression;
    QStringList list = Regular->GetRegularTimeList(Text);
    birthday = Regular->GetTextBirthday(list);
    experience = Regular->GetTextExperience(list);

    params["birthday"] = birthday;
    params["experience"] = experience;
    localFile.close();

    ui->frm_archive_info->propertyInit(fileSign);

    ui->frm_archive_info->setDate(birthday);

//    ui->frm_archive_info->setAge(birthday);

    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_UPLOADRESUME, params, this);
}

void UploadResume::confirmArchiveInfo(QVariantMap info)//确认简历信息
{
    QVariantMap params = info;
    qDebug()<<strFilePath;

    params.insert("format",GetResumeFormat(strFilePath));
    params.insert("experience",experience);

    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_UPLOADARCHIVE, params, this);
}

void UploadResume::HrmUploadResume(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString result = responseMap["Result"].toString();
    QVariantMap params = responseMap["Params"].toMap();
    recvResult(success, result);


    if(success)
    {
        if(result == "简历上传请求成功,正在上传文件...")
            ManageToolClient::instance().streamUpload(strFilePath);
    }
    else
    {
        if(result == "简历可能重复"){
            ui->page_compare->compareStart(params["similarity_list"].toList(), fileSign, strFilePath);
            ui->stackedWidget->setCurrentWidget(ui->page_compare);
        }
    }
}

void UploadResume::HrmUploadArchive(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString result = responseMap["Result"].toString();
    recvResult(success, result);
}

void UploadResume::compareOptCancel()
{
    ui->stackedWidget->setCurrentWidget(ui->page_upload);
}

void UploadResume::compareOptReplace(QString oldSign, QString newSign)
{
    qDebug()<<"replace"<<oldSign<<"->"<<newSign;
}

void UploadResume::recvResult(bool success, QString msg)
{
    ui->lab_msg->setProperty("success",success);
    style()->unpolish(ui->lab_msg);
    style()->polish(ui->lab_msg);
    ui->lab_msg->setText(msg);
    QTimer::singleShot(3000, ui->lab_msg, &QLabel::clear);

    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ");
    textMsg = msg + "\n";
    ui->list_textMsg->addItem(dateTime + textMsg);
    ui->list_textMsg->setCurrentRow(ui->list_textMsg->count() - 1);

}

void UploadResume::clearProcessInf(QAction *act)
{
    if("清空信息" == act->text()){
        ui->list_textMsg->clear();
    }
}

void UploadResume::on_btn_resume_browse_clicked()
{
    qDebug()<<QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QUrl url = QFileDialog::getOpenFileUrl(this
                                           , "请选择要入库的简历文件"
                                           , QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                                           , "*.*");

    if(url.isEmpty())
        return;

    strFilePath = url.toLocalFile();
    previewFile(strFilePath);
    uploadReply();
}


void UploadResume::on_btn_archive_upload_clicked()
{
    QVariantMap archiveInfo = ui->frm_archive_info->getArchiveInfo();
    confirmArchiveInfo(archiveInfo);
}


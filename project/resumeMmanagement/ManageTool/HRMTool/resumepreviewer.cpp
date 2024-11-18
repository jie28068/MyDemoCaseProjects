#include "resumepreviewer.h"
#include "ui_resumepreviewer.h"
#include "Base/PathTool.h"

#include <QTextDocument>
#include <QStackedWidget>
#include <QTimer>
#include <QLabel>
ResumePreviewer::ResumePreviewer(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::ResumePreviewer)
{
    ui->setupUi(this);
    ui->frm_preview->setParent(ui->page_word);
    ui->page_word->installEventFilter(this);
    connect(&ManageToolClient::instance(), &ManageToolClient::streamRst, this, &ResumePreviewer::resumeReceiveRst);
}

ResumePreviewer::~ResumePreviewer()
{
    delete ui;
}

void ResumePreviewer::previewResume(QString sign)
{
    QString archiveFile = PathTool::findArchiveFile(sign);
    if(archiveFile.isEmpty())
        queryResume(sign);
    else
        resumePreview(archiveFile);
}

bool ResumePreviewer::previewLocalFile(QString filename)
{
    return resumePreview(filename);
}

void ResumePreviewer::queryResume(QString sign)
{
    QVariantMap params;
    params["sign"] = sign;

    ManageToolClient::instance().serviceReply(SERVICE_GROUP_HRM, SERVICE_HRM_QUERYRESUME, params, this);
}

void ResumePreviewer::HrmQueryResume(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    QString result = responseMap["Result"].toString();
    QVariantMap params = responseMap["Params"].toMap();

    if(success)
    {
        qDebug()<<"[ArchiveBrowser::HrmQueryResume]load resume success";
        QString sign = params["sign"].toString();
        QString format = params["format"].toString();
        quint64 size = params["size"].toULongLong();

        ManageToolClient::instance().waitForStream(sign, format, size);
    }
    else
    {
        qDebug()<<"[ArchiveBrowser::HrmQueryResume]"<<result;
    }
}

bool ResumePreviewer::resumeReceiveRst(bool success, QString msg)
{
    if(success)
    {
        //预览文件
        return resumePreview(msg);
    }
    else
    {
        qDebug()<<"[ArchiveBrowser::resumeReceiveRst]"<<msg;
        return false;
    }
}

void ResumePreviewer::switchPreviewWidget(int switchFlag)
{
    qDebug() << __func__ ;
    qDebug()<<"switchFlag=="<<switchFlag;
    if(switchFlag==2){
        ui->stackedWidget->setCurrentIndex(2);
    }else if(switchFlag==1){
        ui->stackedWidget->setCurrentIndex(1);
    }else{
        ui->stackedWidget->setCurrentIndex(0);
    }

}

bool ResumePreviewer::resumePreview(QString filePath)
{
    ui->frm_preview->clear();
    ui->frm_preview->repaint();

    if(filePath.endsWith("pdf"))
    {
//        ui->frm_preview->hide();
        ui->stackedWidget->setCurrentWidget(ui->page_pdf);
        /*********************************************************************************
         * 思路：
         * 将打开的pdf每页提取成图片，将每张图片放入一个label控件中，
         * 设置一个垂直布局，将多个label控件垂直起来放入widget
         * 然后将widget放入scrollArea控件中
         * *******************************************************************************/
        if(filePath.length()==0) return false;

        Poppler::Document * document = Poppler::Document::load(filePath); //将pdf文件加载进Document
        if (!document || document->isLocked())
        {

            // ... error message ....

            delete document;
            return false;
        }

        QWidget * widget = new QWidget;
        QVBoxLayout *vboxlayout=new QVBoxLayout;
        for(int i=0;i<document->numPages();i++)                        //获取pdf页数进行循环
        {
            QLabel *label=new QLabel;
            QImage image=document->page(i)->renderToImage(125,120);   //将pdf每页截成Image
            label->setPixmap(QPixmap::fromImage(image));               //将图片存入label中
            vboxlayout->addWidget(label);
            vboxlayout->setAlignment(widget,Qt::AlignCenter);
        }
        widget->setLayout(vboxlayout);                                 //

        ui->scrollArea->setWidget(widget);                             //将widget加入至ScrollArea滑动区域

        delete document;
        return true;
    }
    else if(filePath.endsWith("doc")||filePath.endsWith("docx"))
    {
        ui->stackedWidget->setCurrentWidget(ui->page_word);
//        ui->frm_preview->show();
        ui->frm_preview->dynamicCall("SetVisible(bool Visible)","false");//不显示窗体
        ui->frm_preview->setProperty("DisplayAlerts", false);
//        QRect rect = QRect(ui->page_word->geometry());
//        rect.setWidth(rect.width()-20);
//        ui->frm_preview->setGeometry(rect);
        return ui->frm_preview->setControl(filePath);
    }
    return false;
}

bool ResumePreviewer::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->page_word)
    {
        if(event->type() == QEvent::Resize)
        {
            bool ret = QWidget::eventFilter(watched, event);
            ui->frm_preview->setGeometry(ui->page_word->geometry());
            return ret;
        }
    }

    return QWidget::eventFilter(watched, event);
}

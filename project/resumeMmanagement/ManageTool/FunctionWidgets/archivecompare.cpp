#include "archivecompare.h"
#include "ui_archivecompare.h"
#include <QFileInfo>
#include "Base/PathTool.h"
#include "Base/FormatTranslator.h"

ArchiveCompare::ArchiveCompare(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArchiveCompare)
{
    ui->setupUi(this);
    model = new MTTableModel(this);
    QStringList headerList;
    headerList<<QString("name")
            <<QString("Reason");

    model->setHeaderList(headerList);


    QMap<QString, QString> headerMap = {{"name","姓名"},
                                        {"Reason","原因"}};


    model->setHeaderMap(headerMap);

    ui->table_similar_list->resizeRowsToContents();
    ui->table_similar_list->verticalHeader()->hide();
    ui->table_similar_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->table_similar_list->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->table_similar_list->setModel(model);
}

ArchiveCompare::~ArchiveCompare()
{
    delete ui;
}

/**
 * @brief ArchiveCompare::compareStart  开始查看比对
 * @param compareList   重复档案清单
 * @param sign  新简历签名
 * @param curFile   新简历文件名
 */
bool ArchiveCompare::compareStart(QVariantList compareList, QString sign, QString curFile)
{
    if(compareList.isEmpty())
        return false;

    qDebug()<<"compareStart";
    newSign = sign;
    ui->frm_ax_new->previewLocalFile(curFile);
//    previewArchiveFile(curFile, ui->frm_ax_new);
    QList<QVariantMap> mapList = FormatTransLator::QVariantToMapList(compareList);
    model->setData(mapList);
    ui->frm_archive_info->setData(mapList);
    ui->table_similar_list->setModel(model);
    compareIndex(0);
    return true;
}

void ArchiveCompare::on_btn_back_clicked()
{
    emit compareBack();
}


void ArchiveCompare::on_btn_replace_clicked()
{
    //    emit compareReplace()
}

void ArchiveCompare::previewArchive(QString sign, QAxWidget *frmPreview)
{
    QString filePath = PathTool::findArchiveFile(sign);
    previewArchiveFile(filePath, frmPreview);
}

void ArchiveCompare::previewArchiveFile(QString filePath, QAxWidget *frmPreview)
{
    frmPreview->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
    frmPreview->setProperty("DisplayAlerts", false);
    frmPreview->setControl(filePath);
}

void ArchiveCompare::on_table_similar_list_clicked(const QModelIndex &index)
{
    compareIndex(index.row());
    ui->frm_archive_info->setCurIndex(index.row());
}

void ArchiveCompare::compareIndex(int index)
{
    QVariantMap map = model->getCurTableData(index);
    curSign = map["sign"].toString();
    ui->frm_ax_old->previewResume(curSign);
}


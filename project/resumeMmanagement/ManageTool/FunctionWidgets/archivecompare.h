#ifndef ARCHIVECOMPARE_H
#define ARCHIVECOMPARE_H

#include <QWidget>
#include "QAxWidget"
#include "Model/mttablemodel.h"

namespace Ui {
class ArchiveCompare;
}

class ArchiveCompare : public QWidget
{
    Q_OBJECT

public:
    explicit ArchiveCompare(QWidget *parent = nullptr);
    ~ArchiveCompare();
    bool compareStart(QVariantList compareList, QString sign, QString curFile);
    void updateOldResume(QString resumeFile);

signals:
    void compareBack();//返回
    void compareReplace(QString oldSign, QString newSign);//档案简历替换

private slots:
    void on_btn_back_clicked();
    void on_btn_replace_clicked();

    void on_table_similar_list_clicked(const QModelIndex &index);

private:
    Ui::ArchiveCompare *ui;
    QString newSign;
    QString curSign;
    MTTableModel* model;

    void previewArchive(QString sign, QAxWidget* frmPreview);
    void previewArchiveFile(QString filePath, QAxWidget *frmPreview);
    void compareIndex(int index);
};

#endif // ARCHIVECOMPARE_H

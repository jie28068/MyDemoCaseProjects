#ifndef ARCHIVEBROWSER_H
#define ARCHIVEBROWSER_H

#include <QWidget>
#include "serviceobject.h"
#include "ReplyAndService/ServiceDefines.h"
#include "managetoolclient.h"
#include "Model/mttablemodel.h"
#include "resumepreviewer.h"
#include "Base/PathTool.h"
#include "Base/sortfilterproxymodel.h"
namespace Ui {
class ArchiveBrowser;
}

class ArchiveBrowser :public QWidget, public ServiceObject
{
    Q_OBJECT

public:
    explicit ArchiveBrowser(QWidget *parent = nullptr);
    ~ArchiveBrowser();
    QStringList getPlanList();
    void clearPlanList();

private:
    Ui::ArchiveBrowser *ui;
    MTTableModel model;
    QStringList planIDList;
    SortFilterProxyModel* filter_archive;
    void queryArchiveList(QString condition = QString());
    void resumePreview(QString sign, QString format);
    void resumePreview(QString filePath);

public slots:
    void archiveInfoUpdate(QVariantMap info);
    Q_INVOKABLE void HrmFilterResume(QVariantMap responseMap);
    Q_INVOKABLE void HrmUploadArchive(QVariantMap responseMap);
    Q_INVOKABLE void HrmUpdateArchive(QVariantMap responseMap);
    void recvResult(bool success, QString msg);
private slots:
    void on_btn_filterResume_clicked();
    void on_table_archive_clicked(const QModelIndex &index);
    void archiveUpdate();
//    void on_btn_add_plan_clicked();
    void on_comboBox_1_currentTextChanged(const QString &arg1);
    void on_comboBox_2_currentTextChanged(const QString &arg1);
    void on_comboBox_3_currentTextChanged(const QString &arg1);
    void on_lineEdit_textChanged(const QString &arg1);
};

#endif // ARCHIVEBROWSER_H

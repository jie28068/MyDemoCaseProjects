#ifndef UPLOADRESUME_H
#define UPLOADRESUME_H

#include <QWidget>
#include <QMenu>
#include <QVariantMap>
#include "serviceobject.h"
#include "FunctionWidgets/archiveinfo.h"
#include "managetoolclient.h"
#include <Base/GetTextfrompdforword.h>
#include "Base/regularexpression.h"
namespace Ui {
class UploadResume;
}

class UploadResume : public QWidget, public ServiceObject
{
    Q_OBJECT

public:
    explicit UploadResume(QWidget *parent = nullptr);
    ~UploadResume();

    bool previewFile(QString path);

    QString GetResumeFormat(const QString &strFilePath);

    QString GetText();

    static QString Text;
    static QString birthday;

private:
    void dragEnterEvent(QDragEnterEvent* event);//拖动进入事件

    void dropEvent(QDropEvent* event);//放下事件

    void contextMenuEvent(QContextMenuEvent *event);

    void uploadReply();//上传请求

    void confirmArchiveInfo(QVariantMap info);//确认简历信息

signals:
    void switchPreviewDisplay(int);

public slots:
    Q_INVOKABLE void HrmUploadResume(QVariantMap responseMap);
    Q_INVOKABLE void HrmUploadArchive(QVariantMap responseMap);
    void compareOptCancel();//取消比对操作
    void compareOptReplace(QString oldSign, QString newSign);//替换档案签名
    void recvResult(bool success, QString msg);
    void clearProcessInf(QAction *act);

private slots:

    void on_btn_resume_browse_clicked();

    void on_btn_archive_upload_clicked();

private:
    Ui::UploadResume *ui;
    QMenu *contextMenu;
    QAction *delAct;
    QString strFilePath;
    QString fileSign;
    QString textMsg;
    QStringList experience;
};

#endif // UPLOADRESUME_H

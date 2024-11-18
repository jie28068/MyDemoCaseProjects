#ifndef RESUMEPREVIEWER_H
#define RESUMEPREVIEWER_H

#include <QWidget>
#include <QResizeEvent>
#include "serviceobject.h"
#include "managetoolclient.h"
#include "ReplyAndService/ServiceDefines.h"

#include "../poppler/poppler-qt5.h"
namespace Ui {
class ResumePreviewer;
}

//简历预览器
class ResumePreviewer : public QWidget, public ServiceObject
{
    Q_OBJECT

public:
    explicit ResumePreviewer(QWidget *parent = nullptr);
    ~ResumePreviewer();
    void previewResume(QString sign);
    bool previewLocalFile(QString filename);


public slots:
    Q_INVOKABLE void HrmQueryResume(QVariantMap responseMap);
    bool resumeReceiveRst(bool success, QString msg);

    void switchPreviewWidget(int);

private:
    Ui::ResumePreviewer *ui;
    void queryResume(QString sign);
    bool resumePreview(QString filePath);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // RESUMEPREVIEWER_H

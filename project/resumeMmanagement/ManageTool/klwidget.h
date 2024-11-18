#ifndef KLWIDGET_H
#define KLWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QPainter>
#include <QGradient>
#include "userlogin.h"
#include "HRMTool/hrmtool.h"
#include "managetoolclient.h"
#include "serviceobject.h"
#include "mtreply.h"

#include <QKeyEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class KLWidget; }
QT_END_NAMESPACE

class KLWidget : public QWidget, public ServiceObject
{
    Q_OBJECT

public:
    KLWidget(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    ~KLWidget();

public slots:
    void userLogin(QString userID, QString passwd);
    void userSignup(QStringList userInfo);
    void showMainPage();
    // Q_INVOKABLE void BaseLogin(QVariantMap responseMap);
    void newNotify(QString topic, QString msg);
    void msgClicked();
    void reLogin();

signals:
    void serviceReply(QString serviceGroup, QString service, QVariantMap& params);
    void login_failed();

private:
    void initSysTray();
    void showLog(QString path);
    void showMessage(const QString &title, const QString &msg,QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 10000);
    Q_INVOKABLE void BaseLogin(QVariantMap responseParams);
    Q_INVOKABLE void BaseRegist(QVariantMap responseParams);
    Q_INVOKABLE void BaseQueryWorkLog(QVariantMap responseParams);

private slots:
    void slot_loginSuccess();
    void on_btn_HRMTool_clicked();
    void widgetQuit();
    void trayIconActive(QSystemTrayIcon::ActivationReason reason);

    void on_btn_plan_selection_clicked();

    void on_btn_WorkLog_clicked();

private:
    Ui::KLWidget *ui;
    UserLogin* winUserLogin;
    QSystemTrayIcon* systray;
    QString curTopic;
    QString curUserID;
    QString curPasswd;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif // KLWIDGET_H

#include "klwidget.h"
#include "ui_klwidget.h"
#include "Base/objectfactory.h"
#include "Base/SignTool.h"
#include "ReplyAndService/sessionfactory.h"
#include <QtDebug>
#include <QButtonGroup>
#include <QMenu>
#include "Model/usermodel.h"
#include "Base/PathTool.h"
#include <QProcess>
#include "Base/timeanalyzer.h"

KLWidget::KLWidget(QWidget *parent):
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::KLWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    curUserID = QString();
    curPasswd = QString();

    initSysTray();

    QWidget::connect(&(ManageToolClient::instance()), &ManageToolClient::newNotify, this, &KLWidget::newNotify);
    QWidget::connect(&(ManageToolClient::instance()), &ManageToolClient::connectRecover, this, &KLWidget::reLogin);
    QWidget::connect(ui->page_hrm, &HRMTool::return_mainPage, this, &KLWidget::showMainPage);

    winUserLogin = new UserLogin;
//    QWidget::connect(winUserLogin, &UserLogin::login_quit, this, &KLWidget::close);
    QWidget::connect(winUserLogin, &UserLogin::userLogin, this, &KLWidget::userLogin);
    QWidget::connect(winUserLogin, &UserLogin::userSignup, this, &KLWidget::userSignup);
    QWidget::connect(this, &KLWidget::login_failed, winUserLogin, &UserLogin::userLoginFailed);
    winUserLogin->show();
}

void KLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return||event->key() == Qt::Key_Enter)
    {
      on_btn_HRMTool_clicked();
    }
}

KLWidget::~KLWidget()
{
    winUserLogin->deleteLater();
    delete systray;
    delete ui;
}

/**
 * @brief ManageToolClient::userLogin
 * 参数表：
 * 账号[UserID]：用户的邮箱或者手机号
 * 密码[Passwd]：密码
 * @param userID
 * @param passwd
 */
void KLWidget::userLogin(QString userID, QString passwd)
{
    curUserID = userID;
    curPasswd = passwd;
    QVariantMap params;
    params["user_id"] = userID;
    params["passwd"] = SignTool::SignForPasswd(passwd);
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_LOGIN, params, this);
}

/**
 * @brief ManageToolClient::userSignup
 * 参数表：
 * 注册信息[userInfo]：包括用户注册时填入的所有信息
 * @param userInfo
 */
void KLWidget::userSignup(QStringList userInfo)
{
    QVariantMap params;
    params["name"] = userInfo.at(0);
    params["email"] = userInfo.at(3);
    params["tel_no"] = userInfo.at(2);
    params["passwd"] = SignTool::SignForPasswd(userInfo.at(4));
    params["department"] = userInfo.at(1);
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_REGIST, params, this);
}

void KLWidget::newNotify(QString topic, QString msg)//新的通知
{
    showMessage(topic, msg, QSystemTrayIcon::Information, 5000);
}

void KLWidget::showMainPage()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void KLWidget::initSysTray()
{
    systray = new QSystemTrayIcon(this);
    systray->setIcon(QIcon(":/images/icon.svg"));

    connect(systray, &QSystemTrayIcon::activated, this, &KLWidget::trayIconActive);
    connect(systray, &QSystemTrayIcon::messageClicked, this, &KLWidget::msgClicked);

    QMenu* menu = new QMenu;

    QAction *actionClose = new QAction("退出", menu);
    menu->addAction(actionClose);
    connect(actionClose, &QAction::triggered, this, &KLWidget::widgetQuit);

    systray->setContextMenu(menu);
    systray->show();
}

void KLWidget::msgClicked()
{
    qDebug()<<"[KLWidget::msgClicked]cur topic:"<<curTopic;
    if(curTopic == "新计划通知")
    {
        this->show();
        on_btn_HRMTool_clicked();
    }
    else if(curTopic == "ManageTool")
    {
//        trayIconActive();
    }
}

void KLWidget::reLogin()
{
    if(curUserID.isEmpty())
        return;

    userLogin(curUserID, curPasswd);
}

void KLWidget::showLog(QString path)
{
    TimeAnalyzer ta(__func__);
    QProcess::execute("notepad.exe",{path});
}

void KLWidget::BaseLogin(QVariantMap responseParams)
{
    qDebug()<<"BaseLogin";
    bool success = responseParams.value("Success").toBool();
    QString msg = responseParams.value("Result").toString();

    if(true == success){
        QVariantMap params = responseParams["Params"].toMap();
        ManageToolClient::instance().setUserInfo(params);
        ManageToolClient::instance().getUserInfo()->setLogin(success);
        slot_loginSuccess();
    }else{
        winUserLogin->loginMsg(success, msg);
    }
}

void KLWidget::BaseRegist(QVariantMap responseParams)
{
    qDebug()<<"BaseRegist";
    bool success = responseParams.value("Success").toBool();
    QString msg = responseParams.value("Result").toString();

    winUserLogin->registMsg(success, msg);
}

void KLWidget::BaseQueryWorkLog(QVariantMap responseParams)
{
    qDebug()<<"BaseQueryWorkLog";
    bool success = responseParams.value("Success").toBool();

    if(success){
        QVariantMap params = responseParams.value("Params").toMap();
        QVariantList logList = params.value("work_log").toList();
        QString userId = params.value("user_id").toString();
        QString path = PathTool::cachePath() + "/worklog_" + userId + ".txt";
        QFile file(path);
        file.open(QFile::WriteOnly);
        foreach(QVariant log,logList){
            QVariantMap logmap = log.toMap();
            QString logline = QString("%1   %2      %3      %4\n").arg(logmap.value("department").toString(),
                                                                  logmap.value("name").toString(),
                                                                  logmap.value("time").toString(),
                                                                  logmap.value("operation").toString());
            file.write(logline.toLocal8Bit());
        }
        file.close();
        showLog(path);
    }
}

void KLWidget::slot_loginSuccess()
{
    UserModel* model = ManageToolClient::instance().getUserInfo();
    ui->lab_hello->setText(QString("你好！%1").arg(model->name()));
    ui->lab_department->setText(model->department());

    if(winUserLogin != nullptr){
        winUserLogin->deleteLater();
        winUserLogin = nullptr;
    }
    this->showMaximized();
}


void KLWidget::on_btn_HRMTool_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void KLWidget::widgetQuit()
{
    systray->hide();
    systray->deleteLater();
    exit(0);
//    QApplication::exit();
}

void KLWidget::trayIconActive(QSystemTrayIcon::ActivationReason reason)
{
    if(!ManageToolClient::instance().getUserInfo()->login())
        return;

    switch (reason) {
    case QSystemTrayIcon::Trigger:
        show();
        break;
    default:break;
    }
}

void KLWidget::closeEvent(QCloseEvent *event)
{
    if(systray->isVisible())
    {
        hide();
        showMessage("ManageTool", "程序已最小化，单击再次打开", QSystemTrayIcon::Information, 5000);
        event->ignore();
    }
    else
        QWidget::closeEvent(event);
}

void KLWidget::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);
//    painter.save();
//    painter.fillRect(geometry(), QGradient::HappyAcid);
//    painter.restore();
    QWidget::paintEvent(event);
}

void KLWidget::showMessage(const QString &title, const QString &msg,QSystemTrayIcon::MessageIcon icon, int msecs)
{
    curTopic = title;
    systray->showMessage(title, msg, icon, msecs);
}


void KLWidget::on_btn_plan_selection_clicked()
{
    on_btn_HRMTool_clicked();
}


void KLWidget::on_btn_WorkLog_clicked()
{
    QVariantMap params;
    params["userId"] = ManageToolClient::instance().getUserInfo()->id();

    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_QUERYWROKLOG, params, this);
}


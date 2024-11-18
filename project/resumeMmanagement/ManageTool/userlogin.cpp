#include "userlogin.h"
#include "ui_userlogin.h"

#include <QDebug>
#include "Base/settings.h"

UserLogin::UserLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
    ui->line_user_id->setText(Settings::getAccount());
//    this->setStyleSheet(":/styles/styles/UserLogin.qss");
    ui->stackedWidget->setCurrentIndex(0);

    installEventFilter(this);
}

void UserLogin::keyPressEvent(QKeyEvent *event)
{

     if (event->key() == Qt::Key_Return||event->key() == Qt::Key_Enter)
     {
       on_btn_login_clicked();
     }
}

bool UserLogin::eventFilter(QObject *watched, QEvent *event)
{
    if(watched && watched==this){
        static int pressX,pressY;
        static bool isPress = false;
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        {
            pressX = ((QMouseEvent*)event)->x();
            pressY = ((QMouseEvent*)event)->y();
            isPress = true;
            break;
        }
        case QEvent::MouseMove:
        {
            if(isPress){
                int moveX = ((QMouseEvent*)event)->x()-pressX;
                int moveY = ((QMouseEvent*)event)->y()-pressY;
                this->move(this->x()+moveX,this->y()+moveY);
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            isPress = false;
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched,event);
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::on_btn_close_clicked()
{
    QApplication::exit();
    //emit login_quit();
}


void UserLogin::on_btn_login_clicked()
{
    Settings::setAccount(ui->line_user_id->text());
    emit userLogin(ui->line_user_id->text(), ui->line_user_passwd->text());
}

void UserLogin::userLoginFailed()
{
    qDebug() << __func__ ;
}

void UserLogin::registMsg(bool success, QString msg)
{
    ui->lab_msg->setText(msg);
    ui->lab_msg->setProperty("success",success);
    style()->unpolish(ui->lab_msg);
    style()->polish(ui->lab_msg);
}

void UserLogin::loginMsg(bool success, QString msg)
{
    ui->msg->setText(msg);
    ui->msg->setProperty("success", success);
    style()->unpolish(ui->msg);
    style()->polish(ui->msg);
}


void UserLogin::on_btn_userLogin_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void UserLogin::on_btn_userSignup_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void UserLogin::on_btn_signup_clicked()
{
    QStringList userInfo;
    userInfo << ui->line_name->text()
             << ui->comboBox_section->currentText()
             << ui->line_tel->text()
             << ui->line_mail->text()
             << ui->line_password->text();

    ui->line_user_id->setText(ui->line_tel->text());
    ui->line_user_passwd->setText(ui->line_password->text());

    emit userSignup(userInfo);
}








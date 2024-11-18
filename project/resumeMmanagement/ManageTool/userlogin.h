#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QWidget>
#include <Base/filehelper.h>

#include <Base/filehelper.h>
namespace Ui {
class UserLogin;
}
#include <QKeyEvent>
class UserLogin : public QWidget
{
    Q_OBJECT
    QString username;
    QString passwd;
public:
    explicit UserLogin(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    ~UserLogin();

signals:
    void login_quit();
    void login_success();
    void userLogin(QString userId, QString passwd);
    void userSignup(QStringList userInfo);

public slots:
    void userLoginFailed();
    void registMsg(bool success, QString msg);
    void loginMsg(bool success, QString msg);

private slots:
    void on_btn_close_clicked();

    void on_btn_login_clicked();

    void on_btn_userLogin_clicked();

    void on_btn_userSignup_clicked();

    void on_btn_signup_clicked();


private:
    Ui::UserLogin *ui;
};

#endif // USERLOGIN_H

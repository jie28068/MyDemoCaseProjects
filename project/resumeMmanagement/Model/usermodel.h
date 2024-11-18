#ifndef USERMODEL_H
#define USERMODEL_H

#include <QVariantMap>
#include <QString>
#include "absmtmodel.h"
#include "datamap.h"

class UserModel : public AbsMTModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(QString tel_no READ tel_no WRITE setTel_no)
    Q_PROPERTY(QString passwd READ passwd WRITE setPasswd)
    Q_PROPERTY(int pow READ pow WRITE setPow)
    Q_PROPERTY(QString department READ department WRITE setDepartment)
    Q_PROPERTY(QString type READ type WRITE setType)
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(bool login READ login WRITE setLogin NOTIFY loginChanged)
private:
    QString m_name;

    QString m_email;

    QString m_tel_no;

    QString m_passwd;

    int m_pow;

    QString m_department;

    QString m_type;

    bool m_login;

    int m_id;

public:
    UserModel();

    const QString &name() const;
    void setName(const QString &newName);
    const QString &email() const;
    void setEmail(const QString &newEmail);
    const QString &tel_no() const;
    void setTel_no(const QString &newTel_no);
    const QString &passwd() const;
    void setPasswd(const QString &newPasswd);
    int pow() const;
    void setPow(int newPow);
    const QString &department() const;
    void setDepartment(const QString &newDepartment);
    const QString &type() const;
    void setType(const QString &newType);
    bool login() const;
    void setLogin(bool newLogin);
    int id() const;
    void setId(int newId);

signals:
    void loginChanged();
};

#endif // USERMODEL_H

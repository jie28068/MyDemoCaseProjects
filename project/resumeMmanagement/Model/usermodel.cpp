#include "usermodel.h"

UserModel::UserModel()
{
    m_login=false;
}

const QString &UserModel::name() const
{
    return m_name;
}

void UserModel::setName(const QString &newName)
{
    m_name = newName;
}

const QString &UserModel::email() const
{
    return m_email;
}

void UserModel::setEmail(const QString &newEmail)
{
    m_email = newEmail;
}

const QString &UserModel::tel_no() const
{
    return m_tel_no;
}

void UserModel::setTel_no(const QString &newTel_no)
{
    m_tel_no = newTel_no;
}

const QString &UserModel::passwd() const
{
    return m_passwd;
}

void UserModel::setPasswd(const QString &newPasswd)
{
    m_passwd = newPasswd;
}

int UserModel::pow() const
{
    return m_pow;
}

void UserModel::setPow(int newPow)
{
    m_pow = newPow;
}

const QString &UserModel::department() const
{
    return m_department;
}

void UserModel::setDepartment(const QString &newDepartment)
{
    m_department = newDepartment;
}

const QString &UserModel::type() const
{
    return m_type;
}

void UserModel::setType(const QString &newType)
{
    m_type = newType;
}

 bool UserModel::login() const
{
    return m_login;
}

void UserModel::setLogin(bool newLogin)
{
    if (m_login == newLogin)
        return;
    m_login = newLogin;
    emit loginChanged();
}

int UserModel::id() const
{
    return m_id;
}

void UserModel::setId(int newId)
{
    m_id = newId;
}

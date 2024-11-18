#include "usergropmodel.h"

UserGropModel::UserGropModel()
{

}

int UserGropModel::user_id() const
{
    return m_user_id;
}

void UserGropModel::setUser_id(int newUser_id)
{
    m_user_id = newUser_id;
}

int UserGropModel::group_id() const
{
    return m_group_id;
}

void UserGropModel::setGroup_id(int newGroup_id)
{
    m_group_id = newGroup_id;
}

#ifndef USERGROPMODEL_H
#define USERGROPMODEL_H

#include "absmtmodel.h"

class UserGropModel : public AbsMTModel
{
    Q_OBJECT
    Q_PROPERTY(int user_id READ user_id WRITE setUser_id)
    Q_PROPERTY(int group_id READ group_id WRITE setGroup_id)
private:

    int m_user_id;

    int m_group_id;

public:
    UserGropModel();
    int user_id() const;
    void setUser_id(int newUser_id);
    int group_id() const;
    void setGroup_id(int newGroup_id);
};

#endif // USERGROPMODEL_H

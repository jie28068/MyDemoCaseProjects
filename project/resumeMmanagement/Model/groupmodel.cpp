#include "groupmodel.h"

GroupModel::GroupModel()
{

}

const QString &GroupModel::name() const
{
    return m_name;
}

void GroupModel::setName(const QString &newName)
{
    m_name = newName;
}

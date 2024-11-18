#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "absmtmodel.h"

class GroupModel : public AbsMTModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

private:

    QString m_name;

public:
    GroupModel();
    const QString &name() const;
    void setName(const QString &newName);
};

#endif // GROUPMODEL_H

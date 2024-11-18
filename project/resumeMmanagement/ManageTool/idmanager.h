#ifndef IDMANAGER_H
#define IDMANAGER_H

#include <QObject>
#include <serviceobject.h>

class IDManager : public QObject
{
    Q_OBJECT
public:
    explicit IDManager(QObject *parent = nullptr);

signals:

};

#endif // IDMANAGER_H

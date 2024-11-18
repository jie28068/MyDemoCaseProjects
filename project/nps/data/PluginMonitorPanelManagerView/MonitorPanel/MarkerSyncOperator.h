#ifndef MARKERSYNCOPERATOR_H
#define MARKERSYNCOPERATOR_H

#include "def.h"
#include <QObject>

class MarkerSyncOperator : public QObject
{
    Q_OBJECT

public:
    MarkerSyncOperator(QObject *parent);
    ~MarkerSyncOperator();

    // void add()

private:
};

#endif // MARKERSYNCOPERATOR_H

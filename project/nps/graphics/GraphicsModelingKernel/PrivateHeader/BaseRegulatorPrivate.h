#ifndef BASEREGULATORPRIVATE
#define BASEREGULATORPRIVATE

#include <QPolygonF>

// #include "BaseRegulator.h"

class BaseRegulatorPrivate
{
public:
    BaseRegulatorPrivate();
    QPointF centerPos;
    QPolygonF leftArrow;
    QPolygonF rightArrow;
    QPolygonF topArrow;
    QPolygonF bottomArrow;
    BaseRegulator::Direction diretion;
    QPolygonF allPoints;
    unsigned int index;
    unsigned int regulatorSize;
    QPointF lastMousePressPos;
    int movedXCount;
    int movedYCount;
    bool isLocked;
    bool isInRange; // 是否为范围内调节，如母线
};

BaseRegulatorPrivate::BaseRegulatorPrivate()
{
    centerPos = QPointF();
    leftArrow = QPolygonF();
    rightArrow = QPolygonF();
    topArrow = QPolygonF();
    bottomArrow = QPolygonF();
    diretion = BaseRegulator::kDirectionLeftRight;
    allPoints = QPolygonF();
    index = 0;
    regulatorSize = 10;
    lastMousePressPos = QPointF();
    movedXCount = 0;
    movedYCount = 0;
    isLocked = false;
    isInRange = false;
}

#endif
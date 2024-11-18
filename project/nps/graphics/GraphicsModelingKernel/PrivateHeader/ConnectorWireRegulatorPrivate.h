#pragma once

#include <QPolygonF>

class ConnectorWireRegulatorPrivate
{
public:
    ConnectorWireRegulatorPrivate();
    QPointF centerPos;
    QPolygonF leftArrow;
    QPolygonF rightArrow;
    QPolygonF topArrow;
    QPolygonF bottomArrow;
    ConnectorWireRegulator::Direction diretion;
    QPolygonF allPoints;
    unsigned int index;
    unsigned int regulatorSize;
    QPointF lastMousePressPos;
    int movedXCount;
    int movedYCount;
    bool isLocked;
};

ConnectorWireRegulatorPrivate::ConnectorWireRegulatorPrivate()
{
    centerPos = QPointF();
    leftArrow = QPolygonF();
    rightArrow = QPolygonF();
    topArrow = QPolygonF();
    bottomArrow = QPolygonF();
    diretion = ConnectorWireRegulator::kDirectionLeftRight;
    allPoints = QPolygonF();
    index = 0;
    regulatorSize = 10;
    lastMousePressPos = QPointF();
    movedXCount = 0;
    movedYCount = 0;
    isLocked = false;
}
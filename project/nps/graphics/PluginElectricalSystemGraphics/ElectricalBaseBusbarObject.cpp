#include "ElectricalBaseBusbarObject.h"

#include <QPainter>

ElectricalBaseBusbarObject::ElectricalBaseBusbarObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent), m_bShowPowerFlow(false), m_showColor(QColor())
{ 
}

ElectricalBaseBusbarObject::~ElectricalBaseBusbarObject() { }

void ElectricalBaseBusbarObject::setShowPowerFlow(bool isShow) 
{ 
    m_bShowPowerFlow = isShow;
}

void ElectricalBaseBusbarObject::setShowColor(QColor &color) 
{ 
    m_showColor = color;
}

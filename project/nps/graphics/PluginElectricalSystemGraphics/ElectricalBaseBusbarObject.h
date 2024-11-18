#pragma once

#include "defines.h"

class ElectricalBaseBusbarObject : public GraphicsLayer
{
    Q_OBJECT

public:
    ElectricalBaseBusbarObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ElectricalBaseBusbarObject();

    void setShowPowerFlow(bool isShow);
    void setShowColor(QColor& color);
protected:
    bool m_bShowPowerFlow;
    QColor m_showColor;
};
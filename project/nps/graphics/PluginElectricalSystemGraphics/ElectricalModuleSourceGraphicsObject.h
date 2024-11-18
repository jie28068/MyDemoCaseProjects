#pragma once

#include "BusBarvirtualPortGraphicsObject.h"
#include "ElectrialDataTextGraphicsObject.h"
#include "defines.h"

class ElectricalBaseBusbarObject;
class ElectricalModuleSourceGraphicsObject : public SourceGraphicsObject
{
    Q_OBJECT

public:
    ElectricalModuleSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                         QGraphicsItem *parent = 0);
    ~ElectricalModuleSourceGraphicsObject();

    virtual void createInternalGraphics();

    virtual void updatePosition();

    virtual void setSourceBoundingRect(QRectF rect = QRectF()); // 设置图元边界

    virtual QSizeF getMinimumSize() const { return QSize(10, 10); };

    bool isNeedFilp(QString prototypeName);

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual void userMenu(QSharedPointer<QMenu> menu) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

public slots:
    void onReceiveShowPowerFlowChange(bool flag);
    void onItemSelectChange(bool flag);
    void onWireHasConnected();

    void onSourceChanged(QString key, QVariant value);

    virtual void onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue);

protected:
    ElectricalBaseBusbarObject *m_pBusBarObject;
    ElectrialDataTextGraphicsObject *dataTextGraphics;
    BusBarvirtualPortGraphicsObject *virtualTopPort;
    BusBarvirtualPortGraphicsObject *virtualBottomPort;
    SourceProtoTypeTextGraphicsObject *protoTypeText;
    QPointF pressPointF;
    QGraphicsTextItem *textOverLapItem;
};

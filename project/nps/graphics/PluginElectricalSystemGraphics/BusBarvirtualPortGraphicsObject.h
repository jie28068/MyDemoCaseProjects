#pragma once

#include "defines.h"
#include "ElectricalModulePortGraphicsObject.h"

/// @brief 电气虚拟端口，用于从母线开始连线
class BusBarvirtualPortGraphicsObject : public ElectricalModulePortGraphicsObject
{
    Q_OBJECT

public:
    BusBarvirtualPortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                                       QSharedPointer<PortContext> context, QGraphicsItem *parent = 0);
    ~BusBarvirtualPortGraphicsObject();
/*     enum virtualPortAnchor {
        defaultAnchor,
        topAnchor,  //上侧
        bottomAnchor //下侧
    };
    void setVirtualPortAnchor(virtualPortAnchor type){
        portAnchor = type;
    } */

    virtual void createInternalGraphics() override;

    void setPosition(QPointF clickPos);

    virtual void unLinkConnectorWire(ConnectorWireGraphicsObject *connectorWire);

    virtual int type() const override;

    virtual Qt::AnchorPoint getAnchorPoint() override;

    virtual QPointF getExternalPos() override;

    bool isDrawingConnecterWire();

signals:
    void wireHasConnected();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;


    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};
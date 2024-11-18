#pragma once

#include "GraphicsLayer.h"

class BadgeGraphicsObject : public GraphicsLayer
{
    Q_OBJECT
public:
    BadgeGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~BadgeGraphicsObject();

    virtual void updatePosition();

    void setMsgTip(QString msg);

    virtual void userShow(bool visible);

public slots:
    void onShowModelRunningSortChanged(bool show);

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QString msgTip;
    QFont font;
};

#ifndef IRREGULARANCHORPORTGRAPHICS_H
#define IRREGULARANCHORPORTGRAPHICS_H

/*
锚点方向不规则的端口图元，比如圆形加法器上的端口，端口外联点连线角度不是0,90,180,270度

*/

#include "ControlModulePortGraphicsObject.h"
#include "IrRegularAnchorPortTextGraphics.h"
#include "defines.h"

class IrRegularAnchorPortGraphics : public PortGraphicsObject
{
    Q_OBJECT

public:
    IrRegularAnchorPortGraphics(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                                QSharedPointer<PortContext> context, QGraphicsItem *parent = 0);
    ~IrRegularAnchorPortGraphics();

    /// @brief 创建内部图元
    virtual void createInternalGraphics() override;

    virtual Qt::AnchorPoint getAnchorPoint();

    virtual void updatePosition() override;

    virtual QRectF boundingRect() const;

    virtual QPainterPath shape() const override;

    virtual QPointF getLinkPos();

    virtual QPointF getExternalPos();

    /// @brief 端口是否可以移动
    /// @return
    virtual bool canMove();

    QPointF getPortNamePos();

    void setDataTextShow(const QString &text);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private slots:
    void onPortHideOrShowChange(bool value);

private:
    qreal radius;
    QPolygonF arrowPoints;
    int currentAngle;
    IrRegularPortNameTextGraphicsObject *portNameGraphics;
    ControlDataTextGraphicsObject *dataTextGraphics;
};

#endif // IRREGULARANCHORPORTGRAPHICS_H

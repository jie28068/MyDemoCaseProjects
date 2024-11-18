#ifndef SIZECONTROLGRAPHICSOBJECT_H
#define SIZECONTROLGRAPHICSOBJECT_H

#include "GraphicsLayer.h"

#include <QObject>
#include <QScopedPointer>

class SizeControlGraphicsObjectPrivate;
class SizeControlGraphicsObject : public GraphicsLayer
{
    Q_OBJECT
public:
    enum Direction {
        kDirectionInvalid = 0,
        kDirectionLeftTop,
        kDirectionTopCenter,
        kDirectionRightTop,
        kDirectionRightCenter,
        kDirectionRightBottom,
        kDirectionBottomCenter,
        kDirectionLeftBottom,
        kDirectionLeftCenter,
    };

public:
    virtual QString id() override;

    /// @brief 获取大小控制块方向
    /// @return
    SizeControlGraphicsObject::Direction getDirection();

    /// @brief 获取控制块对应的鼠标形状
    /// @return
    Qt::CursorShape getCursorShape();

    virtual void userShow(bool visible) override;

    /// @brief 生成目前支持的大小控制块列表
    /// @param canvasScene
    /// @return
    static QList<SizeControlGraphicsObject *> createAllSizeControlGraphics(ICanvasScene *canvasScene);

protected:
    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:

    void readyChange();

    void sizeChanged(QPointF offset);

    void finishChange();

private:
    SizeControlGraphicsObject(ICanvasScene *canvasScene, SizeControlGraphicsObject::Direction direction,
                              QGraphicsItem *parent = 0);
    ~SizeControlGraphicsObject();

private:
    QScopedPointer<SizeControlGraphicsObjectPrivate> dataPtr;
};

#endif // SIZECONTROLGRAPHICSOBJECT_H

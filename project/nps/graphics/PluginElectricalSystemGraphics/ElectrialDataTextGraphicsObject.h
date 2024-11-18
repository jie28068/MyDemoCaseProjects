#ifndef ELECTRICALDATATEXTGRAPHICSOBJECT_H
#define ELECTRICALDATATEXTGRAPHICSOBJECT_H

#include "defines.h"

class ElectrialDataTextGraphicsObject : public TextGraphicsObject<QGraphicsTextItem>
{
    Q_OBJECT

public:
    ElectrialDataTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ElectrialDataTextGraphicsObject();

    enum ElectrialDataType {
        InvalidData,           // 默认
        BusBarLoadFlowData,    // 母线的潮流计算数据数据
        ComponentLoadFlowData, // 电气元件的潮流计算数据 一般显示母线端口旁边
    };

    void setElectrialDataType(ElectrialDataType type);

    ElectrialDataType getElectrialDataType();

    virtual void updatePosition();

    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void setShowData(QList<showDataStruct> &dataList, SourceGraphicsObject *source);

private:
    ElectrialDataType dataType;
    QPointF clickPos;
};
#endif // ELECTRICALDATATEXTGRAPHICSOBJECT_H
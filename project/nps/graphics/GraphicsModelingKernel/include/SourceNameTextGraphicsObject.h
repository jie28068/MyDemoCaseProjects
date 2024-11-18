#ifndef SOURCENAMETEXTGRAPHICSOBJECT_H
#define SOURCENAMETEXTGRAPHICSOBJECT_H

#include "EditableGraphicsTextItem.h"
#include "Global.h"
#include "TextGraphicsObject.h"

#include <QFont>

class SourceNameTextGraphicsObjectPrivate;

class GRAPHICSMODELINGKERNEL_EXPORT SourceNameTextGraphicsObject : public TextGraphicsObject<EditableGraphicsTextItem>
{
    Q_OBJECT

public:
    SourceNameTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~SourceNameTextGraphicsObject();

    virtual void updatePosition();

    virtual void setAngleOffset(QMap<int, QPointF> nameOffset);

    virtual void movePositionBy(int angle, const QPointF &pointDelta);

    virtual QRectF boundingRect() const override;

    virtual void userShow(bool visible);

    QRectF mapToParentBoundingRect();

    bool isMousePressing();
    /// @brief 撤销重做，设置文本
    /// @param name
    /// @param isChanged 是否为撤销重做设置的文本
    void seTextNameChanged(QString name, bool isChanged);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void processPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    QPointF handlePositionChanged(const QPointF &pointPosition);

    Qt::AnchorPoint calculateDirection(const QVariantList &listAnchor, int angle);

    void updateProxyAngleSet();

    QPointF getDefaultPoint(int angle);

    QPointF calcRelPosition(const QPointF &point, const QRectF &recf);

    QFont getSourceNameFont();

public slots:
    void onSourceChange(QString key, QVariant val); // @key:属性名称 @val:属性值
    void onTextContentsChanged();
    void onOriginalTextContentsChanged();

    void onSourcePropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

private:
    QScopedPointer<SourceNameTextGraphicsObjectPrivate> dataPtr;
};

#endif // SOURCENAMETEXTGRAPHICSOBJECT_H

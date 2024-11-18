#ifndef COPYRIGHTGRAPHICSOBJECT_H
#define COPYRIGHTGRAPHICSOBJECT_H

#include "ActionManager.h"
#include "BorderTextGraphicsObject.h"
#include "CanvasContext.h"
#include "EditableGraphicsTextItem.h"
#include "GraphicsLayer.h"
#include "SourceGraphicsObject.h"
#include <QList>
#include <qstyleoption.h>

class CopyRightGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

public:
    CopyRightGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~CopyRightGraphicsObject();

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;
    virtual int type() const override;

protected:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

private slots:
    void onContextChanged(QString key, QVariant value);
    void onContextPropertyChanged(QString key, QVariant oldValue, QVariant value);
    void onEditClicked();

private:
    void initTableTitle();
    void initItem();
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void rightGraphicsMenu(QSharedPointer<QMenu> rgMenu);
    void setText(const QString &text, BorderTextGraphicsObject<QGraphicsTextItem> *Item);
    QFont getFont();

private:
    BorderTextGraphicsObject<QGraphicsTextItem> *softVersionItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *canvasTypeItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *canvasNameItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *descriptionItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *dateItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *authorItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *ver_titleItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *type_titleItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *name_titleItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *desc_titleItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *date_titleItem;
    BorderTextGraphicsObject<QGraphicsTextItem> *author_titleItem;
    QList<BorderTextGraphicsObject<QGraphicsTextItem> *> itemList;

private:
    QSharedPointer<QMenu> rgMenu;
    BusinessHooksServer *hookserver;
    QSharedPointer<CanvasContext> canvasContext;
};

#endif // COPYRIGHTGRAPHICSOBJECT_H

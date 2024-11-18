#pragma once
#include "TextGraphicsObject.h"

template<class T>
class BorderTextGraphicsObject : public GraphicsLayer
{
public:
    BorderTextGraphicsObject(ICanvasScene *canvasScene, QString str, Qt::AlignmentFlag align, QGraphicsItem *parent)
        : GraphicsLayer(canvasScene, parent)
    {
        m_text = new TextGraphicsObject<T>(canvasScene, this);
        m_text->setPlainText(str);
        m_alignment = align;
    }

    void setBoundingRect(const QRectF &rc)
    {
        m_boundingRect = rc;
        QRectF rcText = m_text->boundingRect();
        QFontMetrics fontMetrics(m_text->font());
        qreal textHeight = fontMetrics.ascent() + 4;
        qreal textWidth = fontMetrics.width(m_text->getPlainText());
        if (m_alignment == Qt::AlignLeft) {
            m_text->setPos(0, (m_boundingRect.height() - textHeight) / 2);
        } else if (m_alignment == Qt::AlignCenter) {
            QPointF pos((m_boundingRect.width() - textWidth) / 2, (m_boundingRect.height() - textHeight) / 2);
            m_text->setPos(pos);
        }
    }

    virtual QRectF boundingRect() const override { return m_boundingRect; }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override
    {
        painter->save();
        painter->drawRect(m_boundingRect);
        painter->restore();
    }

    TextGraphicsObject<T> *getTextItem() { return m_text; }

    virtual QFont font() { return m_text->font(); }
    virtual void setfont(const QFont &font) { m_text->setfont(font); }

private:
    QRectF m_boundingRect;
    Qt::AlignmentFlag m_alignment;
    TextGraphicsObject<T> *m_text;
};

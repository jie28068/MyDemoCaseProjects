#pragma once

#include "CanvasContext.h"
#include "GraphicsKernelDefinition.h"
#include "GraphicsLayer.h"
#include "ICanvasScene.h"
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QTextDocument>
/// @brief 文字图层

template<class T = QGraphicsTextItem>
class TextGraphicsObject : public GraphicsLayer
{
public:
    TextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0) : GraphicsLayer(canvasScene, parent)
    {
        textItem = new T(this);
        textFont = GKD::DEFAULT_FONT;
        textItem->setFont(GKD::DEFAULT_FONT);
        QTextDocument *textDocument = textItem->document();
        textDocument->setDocumentMargin(1);
        graphicsType = kTextGraphics;
        setLayerFlag(GraphicsLayer::kTextGraphicsLayer);
        enableMove = true;
    }

    /// @brief 设置图元类型
    /// @param type 类型
    void setType(int type) { graphicsType = type; }

    /// @brief 可编辑开关
    /// @param enable
    void enableEditable(bool enable)
    {
        enableMove = enable;
        if (enable) {
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        } else {
            textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        }
    }

    QTextDocument *document() { return textItem->document(); }

    virtual int type() const override { return graphicsType; };

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        GraphicsLayer::paint(painter, option, widget);
        painter->save();
        if (textItem->font() != textFont) {
            textItem->setFont(textFont);
            textItem->update();
        }
        painter->restore();
    }

    virtual QRectF boundingRect() const
    {
        QRectF rcText = textItem->boundingRect();
        // return rcText;
        int adjustSize = 2;
        return rcText.adjusted(-adjustSize, -adjustSize, adjustSize, adjustSize);
    }

    void setPlainText(QString text) { textItem->setPlainText(text); }

    QString getPlainText() const { return textItem->document()->toPlainText(); }

    virtual QFont font() const { return textItem->font(); }

    virtual void setfont(const QFont &font)
    {
        textFont = font;
        textItem->setFont(font);
    }

    virtual QRectF rectSize() const { return textItem->boundingRect(); }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (!enableMove) {
            event->ignore();
        } else {
            GraphicsLayer::mouseMoveEvent(event);
        }
    }

public:
    T *textItem;
    int graphicsType;
    QFont textFont;
    bool enableMove;
};

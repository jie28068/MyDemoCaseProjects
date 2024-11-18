#include "ButtonGraphics.h"

#include <QFont>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class ButtonGraphicsPrivate
{
public:
    ButtonGraphicsPrivate()
    {
        size = QSize(20, 20);
        backgroundColor = QColor(Qt::white);
        hoverBackgroundColor = QColor(Qt::white);
        isHover = false;
    }

    QSize size;
    QColor backgroundColor;
    QColor hoverBackgroundColor;
    bool isHover;
    int eventID;
    QString toolTip;
    QPixmap icon;
    QPixmap hoverIcon;
};

ButtonGraphics::ButtonGraphics(int eventID, QGraphicsItem *parent) : QGraphicsObject(parent)
{
    setAcceptHoverEvents(true);
    dataPtr.reset(new ButtonGraphicsPrivate());
    dataPtr->eventID = eventID;
}

ButtonGraphics::~ButtonGraphics() { }

void ButtonGraphics::setSize(const QSize &size)
{
    dataPtr->size = size;
}

void ButtonGraphics::setBackgroundColor(const QColor &color)
{
    dataPtr->backgroundColor = color;
}

void ButtonGraphics::setHoverBackgroundColor(const QColor &color)
{
    dataPtr->hoverBackgroundColor = color;
}

QRectF ButtonGraphics::boundingRect() const
{
    return QRectF(QPointF(0, 0), dataPtr->size);
}

void ButtonGraphics::setToolTip(const QString &toolTip)
{
    dataPtr->toolTip = toolTip;
}

void ButtonGraphics::setIcon(const QPixmap &pixmap)
{
    dataPtr->icon = pixmap;
}

void ButtonGraphics::setHoverIcon(const QPixmap &pixmap)
{
    dataPtr->hoverIcon = pixmap;
}

void ButtonGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    if (dataPtr->isHover) {
        painter->fillRect(boundingRect(), dataPtr->hoverBackgroundColor);
        drawIcon(painter, dataPtr->hoverIcon);
    } else {
        painter->fillRect(boundingRect(), dataPtr->backgroundColor);
        drawIcon(painter, dataPtr->icon);
    }

    if (!dataPtr->toolTip.isEmpty() && dataPtr->isHover) {
        QFont font = painter->font();
        font.setPixelSize(10);
        QFontMetrics fontMetrics(font);
        int width = fontMetrics.width(dataPtr->toolTip) + 10;
        int height = fontMetrics.height() + 5;

        qreal xPos = boundingRect().center().x() - width / 2;
        qreal yPos = boundingRect().bottom() + 5;
        QRectF toolTipRect = QRectF(QPointF(xPos, yPos), QSize(width, height));

        QPainterPath path;
        path.addRoundedRect(toolTipRect, 5, 5, Qt::AbsoluteSize);
        painter->fillPath(path, dataPtr->hoverBackgroundColor);

        QPen pen = painter->pen();
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->setFont(font);

        painter->drawText(toolTipRect, Qt::AlignCenter, dataPtr->toolTip);
    }

    painter->restore();
}

void ButtonGraphics::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverEnterEvent(event);
    dataPtr->isHover = true;
}

void ButtonGraphics::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverLeaveEvent(event);
    dataPtr->isHover = false;
}

void ButtonGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mousePressEvent(event);
    emit clicked(dataPtr->eventID);
}

void ButtonGraphics::drawIcon(QPainter *painter, const QPixmap &imagePixmap)
{
    if (!imagePixmap.isNull()) {
        QRect rc = boundingRect().toRect();

        if (rc.width() > imagePixmap.width() && rc.height() > imagePixmap.height()) {
            // 当前大小足以容纳图片，把图片居中显示
            int xOffset = (rc.width() - imagePixmap.width()) / 2;
            int yOffset = (rc.height() - imagePixmap.height()) / 2;
            QPoint topLeft = rc.topLeft() + QPoint(xOffset, yOffset);
            QRect dstRect = QRect(topLeft, imagePixmap.size());
            painter->drawPixmap(dstRect, imagePixmap);
        } else {
            qreal pixmapRatio = imagePixmap.width() * 1.0 / imagePixmap.height();
            qreal graphicsRatio = rc.width() * 1.0 / rc.height();
            if (pixmapRatio > graphicsRatio) {
                qreal graphicsHeight = rc.width() / pixmapRatio;
                QRectF dstRect = QRectF(QPoint(0, 0), QSize(rc.width(), graphicsHeight));
                dstRect.moveCenter(rc.center());
                painter->drawPixmap(dstRect.toRect(), imagePixmap);
            } else {
                qreal graphicsWidth = rc.height() * pixmapRatio;
                QRectF dstRect = QRectF(QPoint(0, 0), QSize(graphicsWidth, rc.height()));
                dstRect.moveCenter(rc.center());
                painter->drawPixmap(dstRect.toRect(), imagePixmap);
            }
        }
    }
}

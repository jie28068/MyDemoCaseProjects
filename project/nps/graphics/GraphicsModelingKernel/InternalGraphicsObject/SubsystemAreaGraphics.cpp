#include "SubsystemAreaGraphics.h"
#include "BusinessHooksServer.h"
#include "ButtonGraphics.h"
#include "CanvasSceneDefaultImpl.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "SubSystemCommand.h"
#include "TransformProxyGraphicsObject.h"
#include "graphicsmodelingkernel.h"

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QRectF>

class SubsystemAreaGraphicsPrivate
{
public:
    SubsystemAreaGraphicsPrivate()
    {
        boundingRect = QRectF();
        isHover = false;
        buttonGraphics = nullptr;
    }

    QRectF boundingRect;

    bool isHover;

    ButtonGraphics *buttonGraphics;
};

SubsystemAreaGraphics::SubsystemAreaGraphics(QGraphicsItem *parent) : QGraphicsObject(parent)
{
    setAcceptHoverEvents(true);
    dataPtr.reset(new SubsystemAreaGraphicsPrivate());
    dataPtr->buttonGraphics = new ButtonGraphics(0, this);
    dataPtr->buttonGraphics->setBackgroundColor(QColor(217, 239, 255));
    dataPtr->buttonGraphics->setHoverBackgroundColor(QColor(77, 179, 255));
    dataPtr->buttonGraphics->setToolTip(tr("Create SubSystem"));
    QPixmap icon;
    bool isload = icon.load(":/GraphicsModelingKernel/dCreteaSubSystem");
    dataPtr->buttonGraphics->setIcon(icon);
    QPixmap hoverIcon;
    isload = hoverIcon.load(":/GraphicsModelingKernel/hCreteaSubSystem");
    dataPtr->buttonGraphics->setHoverIcon(hoverIcon);
    connect(dataPtr->buttonGraphics, &ButtonGraphics::clicked, this, &SubsystemAreaGraphics::onClicked);
}

SubsystemAreaGraphics::~SubsystemAreaGraphics() { }

void SubsystemAreaGraphics::setBoundingRect(const QRectF &rc)
{
    dataPtr->boundingRect = rc;
    if (dataPtr->buttonGraphics) {
        dataPtr->buttonGraphics->setPos(rc.bottomRight() + QPointF(-20, 5));
    }
}

QRectF SubsystemAreaGraphics::boundingRect() const
{
    return dataPtr->boundingRect;
}

void SubsystemAreaGraphics::onClicked(int eventID)
{
    createSubSystem();
    setVisible(false);
}

void SubsystemAreaGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QColor color(217, 239, 255);

    if (dataPtr->isHover) {
        color = QColor(77, 179, 255);
    }

    QPen pen = painter->pen();
    pen.setColor(color);
    pen.setWidthF(4.0);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(pen);

    painter->drawRect(dataPtr->boundingRect);

    QRectF textRect = QRectF(dataPtr->boundingRect.bottomLeft(), dataPtr->boundingRect.bottomRight() + QPointF(0, 20));
    painter->drawText(textRect, Qt::AlignCenter, tr("SubSystem"));

    painter->restore();
}

void SubsystemAreaGraphics::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverEnterEvent(event);
    dataPtr->isHover = true;
    setCursor(Qt::PointingHandCursor);
}

void SubsystemAreaGraphics::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverLeaveEvent(event);
    dataPtr->isHover = false;
    unsetCursor();
}

void SubsystemAreaGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(scene());
    if (defaultScene) {
        auto items = defaultScene->selectedItems();
        if (items.isEmpty()) {
            event->accept();
            setVisible(false);
            createSubSystem();
        } else {
            setVisible(false);
        }
    }
}

void SubsystemAreaGraphics::createSubSystem()
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(scene());
    if (defaultScene) {
        auto items = defaultScene->selectedItems();
        foreach (auto item, items) {
            TransformProxyGraphicsObject *transformGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (transformGraphics) {
                auto sourceProxy = transformGraphics->getSourceProxy();
                if (sourceProxy && sourceProxy->prototypeName() == "ElectricalInterface") {
                    // 包含电气接口模块不允许创建子系统
                    blog(tr("Subsystem can not contain electricalInterface module"), -2);
                    return;
                }
            }
        }
        SubSystemCommand *command = new SubSystemCommand(defaultScene, nullptr);
        command->setSelectedRect(dataPtr->boundingRect);
        defaultScene->getUndoStack()->push(command);
    }
}

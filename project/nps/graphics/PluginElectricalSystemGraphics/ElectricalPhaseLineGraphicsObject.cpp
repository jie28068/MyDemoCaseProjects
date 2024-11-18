#include "ElectricalPhaseLineGraphicsObject.h"

#include <QPainter>

ElectricalPhaseLineGraphicsObject::ElectricalPhaseLineGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent), gridSpace(10), currentAngle(0), phaseNumber(3)
{
    ICanvasScene *scene = getCanvasScene();
    if (scene) {
        gridSpace = scene->getGridSpace();
        PCanvasContext canvasCtx = scene->getCanvasContext();
        TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
        if (canvasCtx && proxyGraphics) {
            BusinessHooksServer *businessHooks = canvasCtx->getBusinessHooksServer();
            PSourceProxy sourceProxy = proxyGraphics->getSourceProxy();
            // 资源模块变更的时候，重新获取相数值
            connect(sourceProxy.data(), SIGNAL(sourceChange(QString, QVariant)), this,
                    SLOT(onSourceChange(QString, QVariant)));
            if (businessHooks) {
                phaseNumber = businessHooks->getElectricalPhaseNumber(sourceProxy);
            }
        }
    }
    setLayerFlag(GraphicsLayer::kBusbarPhaseGraphicsLayer);
}

ElectricalPhaseLineGraphicsObject::~ElectricalPhaseLineGraphicsObject() { }

QRectF ElectricalPhaseLineGraphicsObject::boundingRect() const
{
    return QRectF(1, 1, gridSpace - 2, gridSpace - 2);
}

void ElectricalPhaseLineGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                              QWidget *widget)
{
    auto parent = parentItem();
    if (parent && !parent->isVisible()) {
        return;
    }
    QRectF rc = boundingRect();
    qreal width = rc.width();
    qreal height = rc.height();
    painter->save();
    QColor color = QColor(0, 0, 0);
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        color = proxyGraphics->getSourceProxy()->getStateColor();
        GraphicsLayerProperty &layerPorperty = proxyGraphics->getLayerProperty();
        if (layerPorperty.getHightLight()) {
            auto canvasContext = getCanvasContext();
            if (canvasContext) {
                color = canvasContext->getCanvasProperty().getHighLightColor();
            } else {
                color = QColor("#0079c2");
            }
        }
    }
    painter->setPen(QPen(color, 1, Qt::SolidLine));

    QLineF line1 = QLineF(rc.bottomLeft(), rc.topLeft() + QPointF(width / 3, 0));
    QLineF line2 = QLineF(rc.bottomLeft() + QPointF(width / 3, 0), rc.topLeft() + QPointF(width * 2 / 3, 0));
    QLineF line3 = QLineF(rc.bottomLeft() + QPointF(width / 3 * 2, 0), rc.topRight());

    if (phaseNumber == -1) {
        BusinessHooksServer *businessHooks = getCanvasScene()->getCanvasContext()->getBusinessHooksServer();
        if (businessHooks) {
            phaseNumber = businessHooks->getElectricalPhaseNumber(proxyGraphics->getSourceProxy());
        }
    }
    if (phaseNumber == 3) {
        painter->drawLine(line1);
        painter->drawLine(line2);
        painter->drawLine(line3);
    } else if (phaseNumber == 1) {
        painter->drawLine(line2);
    }
    painter->restore();
}

void ElectricalPhaseLineGraphicsObject::onSourceChange(QString key, QVariant val)
{
    if (GKD::SOURCE_OTHER != key)
        return;

    ICanvasScene *scene = getCanvasScene();
    if (scene) {
        PCanvasContext canvasCtx = scene->getCanvasContext();
        TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
        if (canvasCtx && proxyGraphics) {
            BusinessHooksServer *businessHooks = canvasCtx->getBusinessHooksServer();
            PSourceProxy sourceProxy = proxyGraphics->getSourceProxy();
            phaseNumber = businessHooks->getElectricalPhaseNumber(sourceProxy);
        }
    }
}

void ElectricalPhaseLineGraphicsObject::updatePosition()
{
    QRectF proxyRc = getTransformProxyGraphicsObject()->boundingRect();

    int angle = getTransformProxyGraphicsObject()->getAngle();
    if (angle == 0 || angle == 90) {
        setPos(proxyRc.topLeft());
    } else if (angle == 180) {
        setPos(proxyRc.topRight() - QPointF(boundingRect().width(), 0));
    } else if (angle == 270) {
        setPos(proxyRc.bottomLeft() - QPointF(0, boundingRect().height()));
    }
    if (currentAngle != angle) {
        resetTransform();
        Utility::rotateOnCenter(this, angle);
        currentAngle = angle;
    }
}

void ElectricalPhaseLineGraphicsObject::userShow(bool visible)
{
    // 当图层需要显示时，必须要图层属性的Visible未被设置或者为true时才显示
    bool layerVisible = getLayerProperty().getVisible();
    auto canvasScene = getCanvasScene();
    if (canvasScene && canvasScene->getCanvasContext()) {
        bool isBaseLayerVisible = canvasScene->getCanvasContext()->getCanvasProperty().isBaseLayerVisible();
        if (!isBaseLayerVisible) {
            layerVisible = false;
        }
    }
    if (visible && layerVisible) {
        setVisible(true);
    } else {
        setVisible(false);
    }
}

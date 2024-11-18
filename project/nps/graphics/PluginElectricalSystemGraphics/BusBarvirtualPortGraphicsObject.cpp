#include "BusBarvirtualPortGraphicsObject.h"
#include <QGraphicsItem>
#include <QPainter>

BusBarvirtualPortGraphicsObject::BusBarvirtualPortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics, 
                                                                QSharedPointer<PortContext> context, QGraphicsItem *parent)
    : ElectricalModulePortGraphicsObject(canvasScene, sourceGraphics, context, parent)                                                              
{
    setFlag(ItemIsSelectable,false);
    setLayerFlag(GraphicsLayer::kVirtutalportLayer);
}

BusBarvirtualPortGraphicsObject::~BusBarvirtualPortGraphicsObject() { }

void BusBarvirtualPortGraphicsObject::createInternalGraphics() { }

void BusBarvirtualPortGraphicsObject::setPosition(QPointF clickPos)
{ 
    clickPos = QPointF(qRound(clickPos.rx() / 10.0) * 10.0, qRound(clickPos.ry() / 10.0) * 10.0);
    TransformProxyGraphicsObject* trans = getTransformProxyGraphicsObject();
    if(!trans)
        return;
    QPointF centerPos = trans->boundingRect().center();
    Qt::AnchorPoint anchor = getAnchorPoint();
    switch (anchor) {
    case Qt::AnchorTop:
        setPos(trans->mapFromScene(clickPos).x(), centerPos.y() - 10 );
        break;
    case Qt::AnchorRight:
        setPos(centerPos.x() + 10, trans->mapFromScene(clickPos).y());
        break;
    case Qt::AnchorBottom:
        setPos(trans->mapFromScene(clickPos).x(), centerPos.y() + 10 );
        break;
    case Qt::AnchorLeft:
        setPos(centerPos.x() - 10, trans->mapFromScene(clickPos).y());
        break;
    }
}

void BusBarvirtualPortGraphicsObject::unLinkConnectorWire(ConnectorWireGraphicsObject *connectorWire) 
{ 
    if (nullptr == connectorWire) {
        return;
    }

    int index = dataPtr->linkedConnectorWireGraphicsObject.indexOf(connectorWire);
    if (index != -1) {
        dataPtr->linkedConnectorWireGraphicsObject.removeAt(index);
    }
    // 连接线断开时，当前端口时输出端口，则取消所连接的连接线的高亮显示状态
    if (dataPtr->context->type() == PortOutputType) {
        for each (auto wire in dataPtr->linkedConnectorWireGraphicsObject) {
            if (wire) {
                wire->enableHighLightHomologous(false);
            }
        }
    }

    linkChanged();
}

int BusBarvirtualPortGraphicsObject::type() const
{
    return kVirtualPortGraphics;;
}

Qt::AnchorPoint BusBarvirtualPortGraphicsObject::getAnchorPoint()
{
    return PortGraphicsObject::getAnchorPoint();
}

QPointF BusBarvirtualPortGraphicsObject::getExternalPos()
{
    QRectF portSceneRect = mapToScene(boundingRect()).boundingRect();
    QPointF centerPos = getLinkPos();
    int gridSpace = 10;
    ICanvasScene *canvasScene = dynamic_cast<ICanvasScene *>(scene());
    if (canvasScene) {
        gridSpace = canvasScene->getGridSpace();
    }
    int counts = 2;
    Qt::AnchorPoint anchor = getAnchorPoint();
    switch (anchor) {
    case Qt::AnchorTop:
        return QPointF(centerPos.rx(), centerPos.ry() - counts * gridSpace);
    case Qt::AnchorRight:
        return QPointF(centerPos.rx() + counts * gridSpace, centerPos.ry());
    case Qt::AnchorBottom:
        return QPointF(centerPos.rx(), centerPos.ry() + counts * gridSpace);
    case Qt::AnchorLeft:
        return QPointF(centerPos.rx() - counts * gridSpace, centerPos.ry());
    }
    return PortGraphicsObject::getExternalPos();
}

bool BusBarvirtualPortGraphicsObject::isDrawingConnecterWire()
{
    return dataPtr->currentDrawingConnectorWire ? true : false;
}

void BusBarvirtualPortGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    // 母线端口颜色应该根据连接线所连接的元件状态确定
    if(getSourceProxy())
    {
        QColor color = getSourceProxy()->getStateColor();
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawEllipse(boundingRect()); 
    }
    painter->restore();
}

void BusBarvirtualPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event) 
{ 
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }

    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }

    if (!canvasContext->canModify()) {
        return;
    }
    if (!canLink()) {
        GraphicsLayer::mousePressEvent(event);
        return;
    }

    // 在已经按住鼠标左键或者右键的情况下
    if (dataPtr->currentDrawingConnectorWire) {
        event->accept();
        return;
    }

    dataPtr->currentDrawingConnectorWire = new ConnectorWireGraphicsObject(getCanvasScene(), nullptr);

    // 新创建的连接线和当前端口连接起来
    dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(this);
    // 把当前新建的连接线添加到场景
    canvasScene->addConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
    event->accept();
}

void BusBarvirtualPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) 
{ 
    ElectricalModulePortGraphicsObject::mouseMoveEvent(event);
}

void BusBarvirtualPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) 
{ 
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene)
        return;
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }

    if (!canvasContext->canModify()) {
        return;
    }

    TransformProxyGraphicsObject* trans = getTransformProxyGraphicsObject();
    if(!trans)
        return;
    SourceGraphicsObject *sourceGraphics = trans->getSourceGraphicsObject();
    if(!sourceGraphics)
        return;

    if (dataPtr->currentDrawingConnectorWire) {
        if (!dataPtr->currentDrawingConnectorWire->isConnectedWithPort()) {
            dataPtr->currentDrawingConnectorWire->clearLinks();
            // 鼠标松开时，连接线还没有分别连接2个端口，则把连接线从场景中移除
            canvasScene->deleteConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
            dataPtr->currentDrawingConnectorWire = nullptr;

        } else {
            QPointF sencePos = trans->mapToScene(pos());
            QPointF centerPos = trans->boundingRect().center();
            int angle = trans->getAngle();
            if (angle % 180 == 0) {
                sencePos.setY(trans->mapToScene(centerPos).y());
            } else {
                sencePos.setX(trans->mapToScene(centerPos).x());
            }
            PortGraphicsObject* outPutPort = dataPtr->currentDrawingConnectorWire->getOutputTypePortGraphics();
            PortGraphicsObject* inPutPort = sourceGraphics->addPortGraphicsObject(sencePos);
            if(outPutPort && inPutPort)
            {
                dataPtr->currentDrawingConnectorWire->unlinkPortGraphicsObject(dataPtr->currentDrawingConnectorWire->getInputTypePortGraphics());
                dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(inPutPort);
                canvasContext->addConnectWireContext(dataPtr->currentDrawingConnectorWire->getConnectorWireContext());
                // 添加到AddConnectorWireCommand
                canvasScene->addConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire, true);
                emit wireHasConnected();
            }
            else
            {
                dataPtr->currentDrawingConnectorWire->clearLinks();
                // 鼠标松开时，连接线还没有分别连接2个端口，则把连接线从场景中移除
                canvasScene->deleteConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
                dataPtr->currentDrawingConnectorWire = nullptr;
            }

            dataPtr->currentDrawingConnectorWire = nullptr;
        }
        event->accept();
    }

    //鼠标松开的时候还按着别的按键，那就判断sourceGrahics是否被选中，没选中就设置端点为隐藏
    if(event->buttons() & Qt::LeftButton  || event->buttons() & Qt::RightButton){
        if(!sourceGraphics->isSelected()){
            emit wireHasConnected();
        }
    }
}

#include "GraphicsLayer.h"
#include "CanvasContext.h"
#include "CanvasSceneDefaultImpl.h"
#include "ConnectorWireSwitch.h"
#include "GraphicsKernelDefinition.h"
#include "GraphicsLayerManager.h"
#include "GraphicsLayerPrivate.h"
#include "ICanvasScene.h"
#include "PortGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include "graphicsmodelingkernel.h"

#include <QPainter>
#include <QStyleOption>

GraphicsLayer::GraphicsLayer(ICanvasScene *canvasScene, QGraphicsItem *parent) : QGraphicsObject(parent)
{
    layerData.reset(new GraphicsLayerPrivate());
    layerData->canvasScene = canvasScene;

    connect(&layerData->properties, &NPSPropertyManager::propertyChanged, this, &GraphicsLayer::onPropertyChanged);
}

GraphicsLayer::~GraphicsLayer() { }

void GraphicsLayer::addToGroup(TransformItemGroup *group)
{
    if (this->group()) {
        return;
    }
    if (group) {
        if (type() == kConnectorWireGraphics) {
            ConnectorWireGraphicsObject *connector = dynamic_cast<ConnectorWireGraphicsObject *>(this);
            if (connector) {
                if (connector->isConnectedWithPort()) {
                    bool flag = false;
                    if (connector->isSourceBothSelected()) {
                        flag = true;
                    }
                    auto inputPortGraphics = connector->getInputTypePortGraphics();
                    auto outputPortGraphics = connector->getOutputTypePortGraphics();
                    if (!inputPortGraphics || !outputPortGraphics) {
                        return;
                    }
                    if (dynamic_cast<QGraphicsItemGroup *>(inputPortGraphics->topLevelItem())
                        && dynamic_cast<QGraphicsItemGroup *>(outputPortGraphics->topLevelItem())) {
                        flag = true;
                    }
                    if (flag) {
                        layerData->parentItem = this->parentItem();
                        group->addToGroup(this);
                        auto connectorSwitch = connector->getConnectorWireSwitch();
                        if (connectorSwitch) {
                            group->addToGroup(connectorSwitch);
                        }
                    }
                } else {
                    layerData->parentItem = this->parentItem();
                    group->addToGroup(this);
                    auto connectorSwitch = connector->getConnectorWireSwitch();
                    if (connectorSwitch) {
                        group->addToGroup(connectorSwitch);
                    }
                }
            }
        } else if (type() == kTransformProxyGraphics) {
            layerData->parentItem = this->parentItem();
            group->addToGroup(this);
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(this);
            if (proxyGraphics) {
                proxyGraphics->refreshSizeControlGraphics(false);
            }
        }
    }
}

void GraphicsLayer::removeFromGroup(TransformItemGroup *group)
{
    if (group) {
        group->removeFromGroup(this);
        this->setParentItem(layerData->parentItem);
        if (type() == kTransformProxyGraphics) {
            TransformProxyGraphicsObject *transformGraphics = dynamic_cast<TransformProxyGraphicsObject *>(this);
            if (transformGraphics) {
                auto sourceProxy = transformGraphics->getSourceProxy();
                if (sourceProxy) {
                    sourceProxy->setPos(transformGraphics->pos());
                }
            }
        } else if (type() == kConnectorWireGraphics) {
            ConnectorWireGraphicsObject *connector = dynamic_cast<ConnectorWireGraphicsObject *>(this);
            if (connector) {
                auto connectorSwitch = connector->getConnectorWireSwitch();
                if (connectorSwitch) {
                    group->removeFromGroup(connectorSwitch);
                }
            }
        }
    }
}

QString GraphicsLayer::id()
{
    return layerData->uuid;
}

void GraphicsLayer::createInternalGraphics() { }

void GraphicsLayer::updatePosition()
{
    QList<QGraphicsItem *> childs = childItems();
    QListIterator<QGraphicsItem *> childIter(childs);
    while (childIter.hasNext()) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(childIter.next());
        if (layer) {
            layer->updatePosition();
        }
    }
}

GraphicsLayer::GraphicsLayerFlag GraphicsLayer::getLayerFlag()
{
    return layerData->layerFlag;
}

void GraphicsLayer::setLayerFlag(GraphicsLayer::GraphicsLayerFlag flag)
{
    if (layerData->layerFlag == flag) {
        return;
    }
    ICanvasScene *defaultScene = layerData->canvasScene;
    if (!defaultScene) {
        return;
    }
    QSharedPointer<CanvasContext> canvasCtx = defaultScene->getCanvasContext();
    if (!canvasCtx) {
        return;
    }

    QSharedPointer<GraphicsLayerManager> graphicsLayerManager = defaultScene->getGraphicsLayerManager();
    if (graphicsLayerManager) {
        QList<int> nowIndexs = Utility::getBinaryOneFlag(flag);
        if (layerData->layerFlag == kInvalidGraphicsLayer) {
            graphicsLayerManager->addLayer(nowIndexs, this);
        } else {
            // 这里计算索引处理，是为了支持一个图层对象可以属于多个图层的机制
            GraphicsLayer::GraphicsLayerFlag lastFlag = layerData->layerFlag;
            QList<int> lastIndexs = Utility::getBinaryOneFlag(lastFlag);
            QSet<int> lastSets = lastIndexs.toSet();
            QSet<int> nowSets = nowIndexs.toSet();
            // 计算差集，确定需要被移除的数组索引
            QList<int> removeIndexs = lastSets.subtract(nowSets).toList();
            graphicsLayerManager->removeLayer(removeIndexs, this);
            // 此时lastSets已经变化了，重置一下
            lastSets = lastIndexs.toSet();
            nowSets = nowIndexs.toSet();
            QList<int> addedIndexs = nowSets.subtract(lastSets).toList();
            graphicsLayerManager->addLayer(addedIndexs, this);
        }
    }

    layerData->layerFlag = flag;

    // 同步属性
    if (canvasCtx) {
        CanvasProperty &canvasProperty = canvasCtx->getCanvasProperty();
        bool isBaseLayerVisible = canvasProperty.isBaseLayerVisible();
        if (!isBaseLayerVisible) {
            if (layerData->layerFlag & kSourceGraphicsLayer) {
                layerData->properties.setVisible(false);
            } else if (layerData->layerFlag & kPortGraphicsLayer) {
                layerData->properties.setVisible(false);
            }
        }

        bool isSourceNameVisible = canvasProperty.isSourceNameVisible();
        if (!isSourceNameVisible) {
            if (layerData->layerFlag & kSourceNameGraphicsLayer) {
                layerData->properties.setVisible(false);
            }
        }

        bool isConnectorWireVisible = canvasProperty.isConnectWireVisible();
        if (!isConnectorWireVisible) {
            if (layerData->layerFlag & kConnectorWireGraphicsLayer) {
                layerData->properties.setVisible(false);
            }
        }

        bool isBusPhapseVisible = canvasProperty.isBusbarPhapseVisible();
        if (!isBusPhapseVisible) {
            if (layerData->layerFlag & kBusbarPhaseGraphicsLayer) {
                layerData->properties.setVisible(false);
            }
        }

        bool isLegendVisible = canvasProperty.isLegendVisible();
        if (!isLegendVisible) {
            if (layerData->layerFlag & kLegendGraphicsLayer) {
                layerData->properties.setVisible(false);
            }
        }
    }
}

ICanvasScene *GraphicsLayer::getCanvasScene()
{
    return layerData->canvasScene;
}

QSharedPointer<CanvasContext> GraphicsLayer::getCanvasContext()
{
    if (!layerData->canvasScene) {
        return nullptr;
    }
    return layerData->canvasScene->getCanvasContext();
}

void GraphicsLayer::userShow(bool visible)
{
    // 当图层需要显示时，必须要图层属性的Visible未被设置或者为true时才显示
    bool layerVisible = getLayerProperty().getVisible();
    if (visible && layerVisible) {
        setVisible(true);
    } else {
        setVisible(false);
    }
}

QRectF GraphicsLayer::boundingRect() const
{
    return QRectF();
}

void GraphicsLayer::onPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue)
{
    processPropertyChanged(name, oldValue, newValue);
}

TransformProxyGraphicsObject *GraphicsLayer::getTransformProxyGraphicsObject()
{
    if (type() == kTransformProxyGraphics) {
        return dynamic_cast<TransformProxyGraphicsObject *>(this);
    }
    QGraphicsItem *item = parentItem();
    if (item) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
        if (layer) {
            return layer->getTransformProxyGraphicsObject();
        }
    }
    return nullptr;
}

QSharedPointer<SourceProxy> GraphicsLayer::getSourceProxy()
{
    auto proxy = getTransformProxyGraphicsObject();
    if (proxy) {
        return proxy->getSourceProxy();
    }
    return nullptr;
}

QRectF GraphicsLayer::unitedChildsBoundingRect()
{
    QRectF rc = boundingRect();
    QRectF sceneRc = mapToScene(rc).boundingRect();
    QList<QGraphicsItem *> childs = childItems();
    QListIterator<QGraphicsItem *> childIter(childs);
    while (childIter.hasNext()) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(childIter.next());
        if (layer && layer->isVisible()) {
            sceneRc = sceneRc.united(layer->mapToScene(layer->boundingRect()).boundingRect());
        }
    }
    rc = mapFromScene(sceneRc).boundingRect();
    return rc;
}

BusinessHooksServer *GraphicsLayer::getBusinessHook()
{
    BusinessHooksServer *hook = nullptr;
    if (layerData->canvasScene) {
        auto canvasContext = layerData->canvasScene->getCanvasContext();
        if (!canvasContext) {
            return nullptr;
        }
        hook = canvasContext->getBusinessHooksServer();
    }
    return hook;
}

GraphicsLayerProperty &GraphicsLayer::getLayerProperty()
{
    return layerData->properties;
}

void GraphicsLayer::cleanLayerInfo()
{
    if (!layerData) {
        return;
    }
    // 把当前的图层信息清理
    QList<int> indexs = Utility::getBinaryOneFlag(layerData->layerFlag);
    if (layerData->canvasScene && !layerData->canvasScene->isIgnoreLayerManager()) {
        QSharedPointer<GraphicsLayerManager> graphicsLayerManager = layerData->canvasScene->getGraphicsLayerManager();
        if (!graphicsLayerManager.isNull()) {
            graphicsLayerManager->removeLayer(indexs, this);
        }
    }

    // 遍历子图元同步清理掉
    QList<QGraphicsItem *> childs = childItems();
    QListIterator<QGraphicsItem *> childIter(childs);
    while (childIter.hasNext()) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(childIter.next());
        if (layer) {
            layer->cleanLayerInfo();
        }
    }
}

void GraphicsLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!layerData->canvasScene) {
        return QGraphicsObject::mouseMoveEvent(event);
    }
    if (!layerData->isMovingCleanBranchPoints) {
        auto selectedItems = layerData->canvasScene->selectedItems();
        for each (auto item in selectedItems) {
            if (item->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (proxyGraphics) {
                    QList<PortGraphicsObject *> ports = proxyGraphics->getPortGraphicsObjectList();
                    for each (PortGraphicsObject *portGraphics in ports) {
                        if (portGraphics) {
                            portGraphics->cleanConnectorWireBranchPoints();
                        }
                    }
                }
            }
        }
        layerData->isMovingCleanBranchPoints = true;
    }
    QGraphicsObject::mouseMoveEvent(event);
}

void GraphicsLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!layerData->canvasScene) {
        return QGraphicsObject::mouseReleaseEvent(event);
    }
    layerData->isMovingCleanBranchPoints = false;
    auto selectedItems = layerData->canvasScene->selectedItems();
    for each (auto item in selectedItems) {
        if (item->type() == kTransformProxyGraphics) {
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                QList<PortGraphicsObject *> ports = proxyGraphics->getPortGraphicsObjectList();
                for each (PortGraphicsObject *portGraphics in ports) {
                    if (portGraphics) {
                        portGraphics->calcuteConnectorWireBranchPoints();
                    }
                }
            }
        }
    }
    QGraphicsObject::mouseReleaseEvent(event);
}

void GraphicsLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) { }

void GraphicsLayer::processPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue)
{
    if (name == LAYER_VISIBLE_PROPERTY_KEY) {
        userShow(newValue.toBool());
    }
}

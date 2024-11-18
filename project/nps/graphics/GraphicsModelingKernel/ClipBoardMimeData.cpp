#include "ClipBoardMimeData.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "GraphicsKernelDefinition.h"
#include "PortGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"

ClipBoardMimeData::ClipBoardMimeData(CanvasContext::Type type) : QMimeData()
{
    canvasType = type;
}

void ClipBoardMimeData::addGraphicsLayer(GraphicsLayer *layer)
{
    if (nullptr == layer) {
        return;
    }
    int graphicsType = layer->type();
    if (graphicsType == kTransformProxyGraphics) {
        TransformProxyGraphicsObject *graphics = dynamic_cast<TransformProxyGraphicsObject *>(layer);
        if (graphics) {
            PSourceProxy originalSourceProxy = graphics->getSourceProxy();
            qDebug() << originalSourceProxy->getSourceProperty().toJson();
            PSourceProxy copySourceProxy = PSourceProxy(new SourceProxy(*originalSourceProxy.data()));
            addSourceProxy(copySourceProxy);
            qDebug() << copySourceProxy->getSourceProperty().toJson();

            // 删除资源的时候，需要把和该资源相关联的连接线也删除
            // 获取该资源的端口列表
            QList<PortGraphicsObject *> portGraphicsList = graphics->getPortGraphicsObjectList();
            // 遍历端口,获取端口的连接线
            QListIterator<PortGraphicsObject *> iterPort(portGraphicsList);
            while (iterPort.hasNext()) {
                PortGraphicsObject *portGraphics = iterPort.next();
                if (nullptr != portGraphics) {
                    // 端口连接线列表
                    QList<ConnectorWireGraphicsObject *> connectorWireList = portGraphics->getLinkedConnectorWireList();
                    QListIterator<ConnectorWireGraphicsObject *> iterConnector(connectorWireList);
                    while (iterConnector.hasNext()) {
                        ConnectorWireGraphicsObject *connectorWireGraphics = iterConnector.next();
                        if (connectorWireGraphics) {
                            PConnectorWireContext connectorCtx = connectorWireGraphics->getConnectorWireContext();
                            addConnectorWireContext(connectorCtx);
                        }
                    }
                }
            }
        }
    } else if (graphicsType == kConnectorWireGraphics) {
        ConnectorWireGraphicsObject *graphics = dynamic_cast<ConnectorWireGraphicsObject *>(layer);
        if (graphics) {
            PConnectorWireContext connectorCtx = graphics->getConnectorWireContext();
            addConnectorWireContext(connectorCtx);
        }
    }
}

CanvasContext::Type ClipBoardMimeData::getCanvasType()
{
    return canvasType;
}

QMap<QString, QSharedPointer<SourceProxy>> ClipBoardMimeData::getSourcesMap()
{
    return sourcesMap;
}

QMap<QString, QSharedPointer<ConnectorWireContext>> ClipBoardMimeData::getConnectorsMap()
{
    return connectorsMap;
}

bool ClipBoardMimeData::IsContainSource(QString sourceID)
{
    foreach (QSharedPointer<SourceProxy> source, sourcesMap.values()) {
        if (source->uuid() == sourceID)
            return true;
    }
    return false;
}

void ClipBoardMimeData::addSourceProxy(QSharedPointer<SourceProxy> source)
{
    if (source.isNull()) {
        return;
    }
    QString uuid = source->uuid();
    if (sourcesMap.contains(uuid)) {
        return;
    }
    sourcesMap[uuid] = source;
}

void ClipBoardMimeData::addConnectorWireContext(QSharedPointer<ConnectorWireContext> connectorCtx)
{
    if (connectorCtx.isNull()) {
        return;
    }
    QString uuid = connectorCtx->uuid();
    if (connectorsMap.contains(uuid)) {
        return;
    }
    connectorsMap[uuid] = connectorCtx;
}

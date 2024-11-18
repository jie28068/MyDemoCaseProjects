#include "TransformProxyGraphicsObject.h"
#include "AnnotationSourceGraphicsObject.h"
#include "AutoConnectionPromptGraphics.h"
#include "BuriedData.h"
#include "CanvasContext.h"
#include "CanvasSceneDefaultImpl.h"
#include "Commands/SourceProxyCommand.h"
#include "ControlCopyCommand.h"
#include "GeometricGraphicsAnnotation.h"
#include "GraphicsConfig.h"
#include "GraphicsFactoryManager.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "ICanvasScene.h"
#include "IConnectorWireAlgorithm.h"
#include "ImageGraphicsObject.h"
#include "ManhattaConnectorWireAlgorithm.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"
#include "PositionCommand.h"
#include "SizeControlGraphicsObject.h"
#include "SourceNameTextGraphicsObject.h"
#include "SourceProxy.h"
#include "TextGraphicsAnnotation.h"
#include "TransformCommand.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObjectPrivate.h"
#include "Utility.h"
#include "graphicsmodelingkernel.h"
#include "qwt_mml_document.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsRotation>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtCore>

static const qreal ALIGN_SHIFT = 5;

TransformProxyGraphicsObject::TransformProxyGraphicsObject(ICanvasScene *weakCanvasScene,
                                                           QSharedPointer<SourceProxy> sourceProxy,
                                                           QGraphicsItem *parent)
    : GraphicsLayer(weakCanvasScene, parent)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    dataPtr.reset(new TransformProxyGraphicsObjectPrivate());
    dataPtr->sourceProxy = sourceProxy;

    connect(sourceProxy.data(), SIGNAL(sourceChange(QString, QVariant)), this, SLOT(onSourceChange(QString, QVariant)));

    createInternalGraphics();
    ICanvasScene *canvasScene = weakCanvasScene;

    connect(this, SIGNAL(sourceNamePositionChanged(QPointF)), canvasScene, SLOT(onSourceNamePositionChanged(QPointF)));
    connect(dataPtr->sourceProxy.data(), SIGNAL(sigResetPortList(int)), this, SLOT(onResetPortList(int)));
    connect(dataPtr->sourceProxy.data(), &SourceProxy::badgeMsgChanged, this,
            &TransformProxyGraphicsObject::onSourceBadgeMsgChanged);

    int layerFlag = kTransformProxyGraphicsLayer | kSelectedGraphicsLayer | kMoveableGraphicsLayer;
    setLayerFlag((GraphicsLayer::GraphicsLayerFlag)layerFlag);

    onSourceStateChange(dataPtr->sourceProxy->state());
    onSourceCommentStateChange(dataPtr->sourceProxy->getCommentState());
}

TransformProxyGraphicsObject::~TransformProxyGraphicsObject()
{
    clearAlignLine();
}

int TransformProxyGraphicsObject::type() const
{
    return kTransformProxyGraphics;
}

QString TransformProxyGraphicsObject::id()
{
    if (!dataPtr->sourceProxy.isNull()) {
        QString sourceUUID = dataPtr->sourceProxy->uuid();
        return ProxyIDPrefix + sourceUUID;
    }
    return "";
}

void TransformProxyGraphicsObject::updatePosition()
{
    GraphicsLayer::updatePosition();

    QList<QSharedPointer<WireParameters>> wireParametersList;
    auto ports = getPortGraphicsObjectList();
    for each (auto port in ports) {
        if (port) {
            auto wires = port->getLinkedConnectorWireList();
            for each (auto wire in wires) {
                auto parameterPtr = wire->refreshWireParameters();
                if (parameterPtr) {
                    wireParametersList.append(parameterPtr);
                }
            }
        }
    }
    if (!wireParametersList.isEmpty()) {
        auto canvasScene = getCanvasScene();
        if (canvasScene) {
            canvasScene->refreshConnectorWires(wireParametersList);
        }
    }
}

SourceGraphicsObject *TransformProxyGraphicsObject::getSourceGraphicsObject()
{
    return dataPtr->sourceGraphics;
}

SourceNameTextGraphicsObject *TransformProxyGraphicsObject::getSourceNameTextGraphicsObject()
{
    return dataPtr->nameGraphics;
}

QSharedPointer<SourceProxy> TransformProxyGraphicsObject::getSourceProxy()
{
    return dataPtr->sourceProxy;
}

QRectF TransformProxyGraphicsObject::boundingRect() const
{
    if (dataPtr->sourceGraphics && (dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer)) {
        AnnotationSourceGraphicsObject *annotationGraphics =
                dynamic_cast<AnnotationSourceGraphicsObject *>(dataPtr->sourceGraphics);
        if (annotationGraphics && !annotationGraphics->isInterfactionFinished()) {
            return QRectF(0, 0, 0, 0);
        }
        return shape().controlPointRect();
    } else {
        return shape().boundingRect();
    }
    return dataPtr->boundingRect;
}

QPainterPath TransformProxyGraphicsObject::shape() const
{
    if (dataPtr->sourceGraphics && (dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer)) {
        return dataPtr->sourceGraphics->transform().map(dataPtr->sourceGraphics->shape());
    }
    QPainterPath path;
    path.addRect(dataPtr->boundingRect);
    path.closeSubpath();
    return path;
}

PortGraphicsObject *TransformProxyGraphicsObject::getPortGraphicsObject(QString id)
{
    QList<QGraphicsItem *> childs = childItems();
    for (int i = 0; i < childs.size(); i++) {
        if (childs[i]->type() == kPortGraphics) {
            PortGraphicsObject *port = dynamic_cast<PortGraphicsObject *>(childs[i]);
            QString portId = port->id();
            if (portId == id) {
                return port;
            }
        }
    }
    return nullptr;
}

QList<PortGraphicsObject *> TransformProxyGraphicsObject::getPortGraphicsObjectList()
{
    QList<PortGraphicsObject *> ports;
    QList<QGraphicsItem *> childs = childItems();
    for (int i = 0; i < childs.size(); i++) {
        if (childs[i]->type() == kPortGraphics) {
            PortGraphicsObject *port = dynamic_cast<PortGraphicsObject *>(childs[i]);
            if (nullptr != port) {
                ports.append(port);
            }
        }
    }
    return ports;
}

bool TransformProxyGraphicsObject::checkNameValid(QString nameString)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return true;
    }
    auto businessHook = canvasContext->getBusinessHooksServer();
    if (!businessHook) {
        return true;
    }
    return businessHook->checkNameValid(nameString);
}

void TransformProxyGraphicsObject::setBoundingRect(const QRectF &boundingRect)
{
    if (dataPtr->boundingRect == boundingRect) {
        return;
    }
    prepareGeometryChange();
    dataPtr->boundingRect = boundingRect;
    refreshSizeControlGraphics(isSelected());

    updatePosition();
}

void TransformProxyGraphicsObject::onProcessSizeChanged(QPointF pos)
{
    // 如果偏移量都为0，不处理
    if (Utility::isEqualZero(pos.x()) && Utility::isEqualZero(pos.y())) {
        return;
    }
    if (!dataPtr->sourceProxy) {
        return;
    }
    ICanvasScene *canvasScene = getCanvasScene();
    SizeControlGraphicsObject *sizeControlGraphics = dynamic_cast<SizeControlGraphicsObject *>(sender());
    if (!canvasScene || !sizeControlGraphics) {
        return;
    }
    int gridSpace = canvasScene->getGridSpace();

    // 部分模块有突出部分 必须和端口对齐所以一次只能缩放偶数个单位
    QString prototypeName = dataPtr->sourceProxy->prototypeName();
    if (gridSpace > 0 && (prototypeName == "Goto" || prototypeName == "From" || prototypeName == "Gain")) {
        if (getAngle() % 180 == 0) {
            int yGridPos = 2 * gridSpace;
            qreal ypos = qRound(pos.ry() * 1.0 / yGridPos) * yGridPos;
            pos.setY(ypos);
        } else {
            int XGridPos = 2 * gridSpace;
            qreal xpos = qRound(pos.rx() * 1.0 / XGridPos) * XGridPos;
            pos.setX(xpos);
        }
    }

    pos = Utility::pointAlignmentToGrid(pos, gridSpace);

    QPointF offset = pos - dataPtr->lastPos;

    QRectF changingRect = boundingRect();
    QRectF tempRect = changingRect;

    SizeControlGraphicsObject::Direction direction = sizeControlGraphics->getDirection();
    QPointF pointHandle = calcChangingRect(changingRect, offset, direction);

    QRectF changingSceneRect = mapRectToScene(changingRect);
    // 不允许缩放到场景外
    if (!canvasScene->sceneRect().contains(changingSceneRect)) {
        return;
    }

    // 拖动的时候，根据输入输出端口的最大数量校验资源允许缩放的最小尺寸
    // modify by liwenyu 2023.07.24 修改最小大小为一个网格，参考simulink
    // int nInputPortCount = 0;
    // int nOutputPortCount = 0;
    // QSizeF sourceMinimumSize = dataPtr->sourceGraphics->getMinimumSize();

    // if (getAngle() % 180 != 0) {
    //     sourceMinimumSize = QSizeF(sourceMinimumSize.height(), sourceMinimumSize.width());
    // }
    // modify end 2023.07.24

    QSizeF sourceMinimumSize = QSizeF(10, 10);

    // 调整偏移量，只调整满足最小尺寸的分量
    bool adjustOffset = false;
    if (changingSceneRect.width() < sourceMinimumSize.width()) {
        offset.setX(0);
        adjustOffset = true;
    }
    if (changingRect.height() < sourceMinimumSize.height()) {
        offset.setY(0);
        adjustOffset = true;
    }
    if (adjustOffset) {
        changingRect = tempRect;
        pointHandle = calcChangingRect(changingRect, offset, direction);
        QRectF changingSceneRect = mapRectToScene(changingRect);
    }

    QRectF adjustChangingSceneRect =
            changingSceneRect.adjusted(gridSpace / 2, gridSpace / 2, -(gridSpace / 2), -(gridSpace / 2));
    QList<PortGraphicsObject *> portList = getPortGraphicsObjectList();
    for each (PortGraphicsObject *portGraphics in portList) {
        PPortContext portCtx = portGraphics->getPortContext();
        if (portCtx->isAbsolutePos()) {
            QPointF portScenePos = mapToScene(portGraphics->pos());
            if (!changingSceneRect.contains(portScenePos)) {
                return;
            }
        }
    }

    dataPtr->alignmentOffset = QPointF(0, 0);

    dataPtr->lastPos = pos;

    QRectF oldRect = dataPtr->sourceProxy->getCombineTransform().mapRect(boundingRect());
    QRectF newRect = dataPtr->sourceProxy->getCombineTransform().mapRect(changingRect);
    QTransform offsetScaleTransform =
            QTransform::fromScale(newRect.width() / oldRect.width(), newRect.height() / oldRect.height());

    dataPtr->sourceProxy->setScaleTransform(dataPtr->sourceProxy->scaleTransform() * offsetScaleTransform);

    // 若拖动的handle对应的矩形角跟item的原点在同一X或同一Y，则需要进行位移
    QPointF originPoint = dataPtr->sourceProxy->getCombineTransform().map(QPointF(0, 0));
    QPointF pointDelta = offset;
    if (Utility::isEqualZero(originPoint.x() - pointHandle.x())
        || Utility::isEqualZero(originPoint.y() - pointHandle.y())) {
        if (Utility::isEqualZero(originPoint.x() - pointHandle.x())
            && Utility::isEqualZero(originPoint.y() - pointHandle.y())) {
            pointDelta = offset;
        } else if (Utility::isEqualZero(originPoint.x() - pointHandle.x())) {
            pointDelta.setY(0);
        } else if (Utility::isEqualZero(originPoint.y() - pointHandle.y())) {
            pointDelta.setX(0);
        }
        QTransform offsetTranslateTransform = QTransform::fromTranslate(pointDelta.x(), pointDelta.y());
        dataPtr->sourceProxy->setTranslateTransform(dataPtr->sourceProxy->translateTransform()
                                                    * offsetTranslateTransform);
    }

    applyTransformChanged();
}

void TransformProxyGraphicsObject::onReadySizeChange()
{
    // 图元变更大小时，清理连接线分支点
    for each (PortGraphicsObject *portGraphics in getPortGraphicsObjectList()) {
        if (portGraphics) {
            portGraphics->cleanConnectorWireBranchPoints();
        }
    }

    dataPtr->lastScaleTransform = dataPtr->sourceProxy->scaleTransform();
    dataPtr->lastRotateTransform = dataPtr->sourceProxy->rotateTransform();
    dataPtr->lastTranslateTransform = dataPtr->sourceProxy->translateTransform();
    dataPtr->lastPos = QPointF(0, 0);
    dataPtr->isOnSizeChange = true;
}

void TransformProxyGraphicsObject::onFinishSizeChange()
{
    if (!dataPtr->sourceProxy) {
        return;
    }
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    // 图元变更大小结束，重新计算分支点
    for each (PortGraphicsObject *portGraphics in getPortGraphicsObjectList()) {
        if (portGraphics) {
            portGraphics->calcuteConnectorWireBranchPoints();
        }
    }

    // 说明触发了缩放
    if (dataPtr->lastPos != QPointF(0, 0)) {
        TransformCommand *command = new TransformCommand(getCanvasScene());
        command->setOldTransforms(id(), dataPtr->lastScaleTransform, dataPtr->lastRotateTransform,
                                  dataPtr->lastTranslateTransform, dataPtr->sourceProxy->xAxisFlipTransform(),
                                  dataPtr->sourceProxy->yAxisFlipTransform());
        command->setNewTransforms(id(), dataPtr->sourceProxy->scaleTransform(), dataPtr->sourceProxy->rotateTransform(),
                                  dataPtr->sourceProxy->translateTransform(),
                                  dataPtr->sourceProxy->xAxisFlipTransform(),
                                  dataPtr->sourceProxy->yAxisFlipTransform());
        QSharedPointer<QUndoStack> undoStack = canvasScene->getUndoStack();
        if (undoStack && !command->isEmpty()) {
            undoStack->push(command);
        } else {
            delete command;
        }
        dataPtr->lastPos = QPointF(0, 0);
    }
    dataPtr->isOnSizeChange = false;
}

void TransformProxyGraphicsObject::applyTransformChanged()
{
    QMap<PortGraphicsObject *, QPointF> oldPortScenePosMap;
    QList<PortGraphicsObject *> portList = getPortGraphicsObjectList();
    // 计算保存变换之前的端口场景坐标
    for each (PortGraphicsObject *portGraphics in portList) {
        PPortContext portCtx = portGraphics->getPortContext();
        if (portCtx->isAbsolutePos()) {
            QPointF portScenePos = mapToScene(portGraphics->pos());
            oldPortScenePosMap[portGraphics] = portScenePos;
        }
    }
    int oldAngle = getAngle();
    dataPtr->sourceGraphics->applyTransform();
    setBoundingRect(dataPtr->sourceGraphics->getTransformBoundingRect());
    int newAngle = getAngle();

    // 母线缩放时的需求，端口位置不变所以端口坐标是绝对值的，保持端口相对不变
    // 但是当母线旋转的时候就不需要这样处理了
    if (oldAngle == newAngle) {
        for each (PortGraphicsObject *portGraphics in portList) {
            PPortContext portCtx = portGraphics->getPortContext();
            if (portCtx->isAbsolutePos()) {
                QPointF portScenePos = oldPortScenePosMap[portGraphics];
                QPointF itemPos = dataPtr->sourceGraphics->mapFromScene(portScenePos);
                portCtx->setPos(itemPos);
            }
        }
    }

    emit transformChanged(boundingRect(), getAngle());
    updatePosition();
}

void TransformProxyGraphicsObject::onSourceChange(QString key, QVariant val)
{
    if (GKD::SOURCE_STATE == key) {
        onSourceStateChange(val.toString());
    } else if (GKD::SOURCE_NAME == key) {
        auto str = dataPtr->sourceProxy->prototypeName_CHS();
        if (!str.isEmpty()) {
            setToolTip(str);
        } else {
            setToolTip(dataPtr->sourceProxy->name());
        }
    } else if (GKD::SOURCE_POS == key) {
        QPointF pos = val.toPointF();
        if (pos != this->pos()) {
            setPos(pos);
        }
    } else if (GKD::SOURCE_COMMENTSTATE == key) {
        onSourceCommentStateChange(val.toString());
    }
}

void TransformProxyGraphicsObject::onSourceStateChange(QString state)
{
    if (state == "normal") {
        auto str = dataPtr->sourceProxy->prototypeName_CHS();
        if (!str.isEmpty()) {
            setToolTip(str);
        } else {
            setToolTip(dataPtr->sourceProxy->name());
        }
        // 禁用和直通状态下，排序内容重置、隐藏
        dataPtr->sourceProxy->setBadgeMsg("");
    } else {
        QString strTip = dataPtr->sourceProxy->statetips();
        setToolTip(strTip);
    }
}

void TransformProxyGraphicsObject::onSourceCommentStateChange(QString state)
{
    if (state == "normal") {
        setOpacity(1.0);
        if (dataPtr->backgroundImageGraphics) {
            dataPtr->backgroundImageGraphics->setOpacity(1.0);
        }
    } else if (state == "disable" || state == "through") {
        setOpacity(0.6);
        if (dataPtr->backgroundImageGraphics) {
            dataPtr->backgroundImageGraphics->setOpacity(0.6);
        }
        // 禁用和直通状态下，排序内容重置、隐藏
        dataPtr->sourceProxy->setBadgeMsg("");
    }
}

void TransformProxyGraphicsObject::onResetPortList(int sizeOffset)
{
    auto canvasContext = getCanvasContext();
    if (!dataPtr->sourceGraphics || !canvasContext || !dataPtr->sourceProxy) {
        return;
    }
    dataPtr->sourceGraphics->applyTransform();
    setBoundingRect(dataPtr->sourceGraphics->getTransformBoundingRect());

    ICanvasScene *canvasScene = getCanvasScene();
    QList<QSharedPointer<ConnectorWireContext>> connectWireContextList;
    // 添加新端口之前的端口列表
    auto portListGraphics = getPortGraphicsObjectList();

    // 根据端口信息创建端口图元
    QMap<QString, PortGraphicsObject *> newPortMap;
    QString moduleType = dataPtr->sourceProxy->moduleType();
    QSharedPointer<ICanvasGraphicsObjectFactory> factory = GraphicsFactoryManager::getInstance().getFactory(moduleType);
    if (factory) {
        QList<QSharedPointer<PortContext>> portList = dataPtr->sourceProxy->portList();
        QListIterator<QSharedPointer<PortContext>> portIter(portList);
        while (portIter.hasNext()) {
            QSharedPointer<PortContext> portContext = portIter.next();
            if (portContext) {
                PortGraphicsObject *portGraphics =
                        factory->createPortGraphicsObject(canvasScene, dataPtr->sourceGraphics, portContext);
                if (portGraphics) {
                    portGraphics->setParentItem(this);
                    portGraphics->createInternalGraphics();
                    newPortMap.insert(portContext->uuid(), portGraphics);
                }
            }
        }
    }

    // 删除旧端口和未连接端口的连接线
    for each (PortGraphicsObject *port in portListGraphics) {
        auto wireGraphics = port->getLinkedConnectorWireList();
        for each (ConnectorWireGraphicsObject *wire in wireGraphics) {
            if (wire) {
                auto wireCtx = wire->getConnectorWireContext();
                if (wireCtx) {
                    QString srcPortUUID = wireCtx->srcPortID();
                    QString dstPortUUID = wireCtx->dstPortID();
                    bool isMatched = false;
                    if (wireCtx->srcSource() == dataPtr->sourceProxy) {
                        if (newPortMap.contains(srcPortUUID)) {
                            auto matchPort = newPortMap[srcPortUUID];
                            isMatched = true;
                            wire->linkPortGraphicsObject(matchPort);
                        }
                    } else if (wireCtx->dstSource() == dataPtr->sourceProxy) {
                        if (newPortMap.contains(dstPortUUID)) {
                            auto matchPort = newPortMap[dstPortUUID];
                            isMatched = true;
                            wire->linkPortGraphicsObject(matchPort);
                        }
                    }

                    if (!isMatched || !wire->isConnectedWithPort()) {
                        canvasScene->deleteConnectorWireGraphicsObject(wire);
                    }
                }
            }
        }
        port->deletePortGraphicsObjectSelf();
    }

    for (auto port : newPortMap) { // 更新分叉点显示
        port->calcuteConnectorWireBranchPoints();
    }

    updatePosition();
    adjustSceneRect();
}

MathMLGraphicsObject *TransformProxyGraphicsObject::getMathMLGraphicsObject()
{
    return dataPtr->mathMLGraphics;
}

void TransformProxyGraphicsObject::createInternalGraphics()
{
    GraphicsConfig &config = GraphicsConfig::getInstance();
    auto canvasContext = getCanvasContext();
    if (!canvasContext || !dataPtr->sourceProxy) {
        return;
    }
    auto canvasScene = getCanvasScene();

    QString moduleType = dataPtr->sourceProxy->moduleType();
    QSharedPointer<ICanvasGraphicsObjectFactory> factory = GraphicsFactoryManager::getInstance().getFactory(moduleType);
    if (factory.isNull()) {
        return;
    }
    dataPtr->backgroundImageGraphics = new ImageGraphicsObject(canvasScene, this);
    dataPtr->backgroundImageGraphics->setImageType(ImageGraphicsObject::kBackground);
    SourceProperty &sourceProperty = dataPtr->sourceProxy->getSourceProperty();
    QByteArray backgroundImageData = sourceProperty.getBackgroundImage();
    if (!backgroundImageData.isNull()) {
        dataPtr->backgroundImageGraphics->loadFromData(backgroundImageData);
    }

    QColor backgroundColor = sourceProperty.getBackgroundColor();
    QStringList transparentModules = config.getTransparentBackgroundModules();
    if (moduleType == GKD::SOURCE_MODULETYPE_ELECTRICAL || moduleType == GKD::SOURCE_MODULETYPE_ANNOTATION
        || transparentModules.contains(dataPtr->sourceProxy->prototypeName())) {
        sourceProperty.setShowOutSideBorder(false);
        if (backgroundColor == QColor(Qt::white)) {
            dataPtr->backgroundImageGraphics->setFillColor(QColor());
        } else {
            dataPtr->backgroundImageGraphics->setFillColor(backgroundColor);
        }
        QString prototypeName = dataPtr->sourceProxy->prototypeName();
        if (prototypeName != "Busbar" && prototypeName != "DotBusbar"
            && dataPtr->sourceProxy->svgImageDatas().isEmpty()) {
            // 电气构造型
            dataPtr->backgroundImageGraphics->setFillColor(backgroundColor);
            sourceProperty.setShowOutSideBorder(true);
        }

    } else {
        dataPtr->backgroundImageGraphics->setFillColor(backgroundColor);
    }
    dataPtr->backgroundImageGraphics->setZValue(-100);

    // 创建资源对应的图元
    if (nullptr == dataPtr->sourceGraphics) {
        dataPtr->sourceGraphics = factory->createSourceGraphicsObject(canvasScene, dataPtr->sourceProxy);
        if (dataPtr->sourceGraphics) {
            dataPtr->sourceGraphics->setParentItem(this);
            dataPtr->sourceGraphics->createInternalGraphics();
            AnnotationSourceGraphicsObject *annotationSource =
                    dynamic_cast<AnnotationSourceGraphicsObject *>(dataPtr->sourceGraphics);
            if (annotationSource) {
                bool isAnnotationVisible = canvasContext->getCanvasProperty().isAnnotationVisible();
                if (!isAnnotationVisible) {
                    annotationSource->getLayerProperty().setVisible(false);
                } else {
                    annotationSource->getLayerProperty().setVisible(true);
                }
            }
        } else {
            return;
        }
    }

    if (dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer) {
        // 注解模块不需要网格对齐
        setZValue(10000);
        setPos(dataPtr->sourceProxy->pos());
    } else {
        setPos(Utility::calculateAlignmentPos(dataPtr->sourceGraphics, dataPtr->sourceProxy->pos()));
    }

    applyTransforms(dataPtr->sourceProxy->scaleTransform(), dataPtr->sourceProxy->rotateTransform(),
                    dataPtr->sourceProxy->translateTransform(), dataPtr->sourceProxy->xAxisFlipTransform(),
                    dataPtr->sourceProxy->yAxisFlipTransform());
    // 资源创建之后才设置代理图元的boundingrect
    setBoundingRect(dataPtr->sourceGraphics->getTransformBoundingRect());

    // 创建数学表达式图元
    dataPtr->mathMLGraphics = new MathMLGraphicsObject(canvasScene, this);
    dataPtr->mathMLGraphics->setMathMLName(dataPtr->sourceProxy->prototypeName());

    dataPtr->foregroundImageGraphics = new ImageGraphicsObject(canvasScene, this);
    dataPtr->foregroundImageGraphics->setImageType(ImageGraphicsObject::kForeground);
    QByteArray foregroundImageData = sourceProperty.getForegroundImage();
    if (!foregroundImageData.isNull()) {
        dataPtr->foregroundImageGraphics->loadFromData(foregroundImageData);
    }

    // 创建大小控制块图元
    dataPtr->sizeControlGraphics = SizeControlGraphicsObject::createAllSizeControlGraphics(canvasScene);
    QListIterator<SizeControlGraphicsObject *> sizeControlIter(dataPtr->sizeControlGraphics);
    while (sizeControlIter.hasNext()) {
        SizeControlGraphicsObject *sizeControlGraphics = sizeControlIter.next();
        if (sizeControlGraphics) {
            sizeControlGraphics->setParentItem(this);
            connect(sizeControlGraphics, SIGNAL(sizeChanged(QPointF)), this, SLOT(onProcessSizeChanged(QPointF)));
            sizeControlGraphics->setZValue(sizeControlGraphics->zValue() + 1);
            connect(sizeControlGraphics, SIGNAL(readyChange()), this, SLOT(onReadySizeChange()));
            connect(sizeControlGraphics, SIGNAL(finishChange()), this, SLOT(onFinishSizeChange()));
        }
    }

    // 根据端口信息创建端口图元
    QList<QSharedPointer<PortContext>> portList = dataPtr->sourceProxy->portList();
    QListIterator<QSharedPointer<PortContext>> portIter(portList);
    while (portIter.hasNext()) {
        QSharedPointer<PortContext> portContext = portIter.next();
        if (dataPtr->sourceGraphics && portContext) {
            PortGraphicsObject *portGraphics =
                    factory->createPortGraphicsObject(canvasScene, dataPtr->sourceGraphics, portContext);
            if (portGraphics) {
                portGraphics->setParentItem(this);
                portGraphics->createInternalGraphics();
            }
        }
    }

    // 创建资源文字图元
    if (!dataPtr->sourceProxy->name().isEmpty()) {
        dataPtr->nameGraphics = new SourceNameTextGraphicsObject(canvasScene, this);
        dataPtr->nameGraphics->enableEditable(true);
        dataPtr->nameGraphics->setPlainText(dataPtr->sourceProxy->name());
        dataPtr->nameGraphics->setAngleOffset(dataPtr->sourceProxy->nameAngleOffset());
        dataPtr->nameGraphics->updatePosition();
        connect(dataPtr->sourceProxy.data(), SIGNAL(sourceChange(QString, QVariant)), dataPtr->nameGraphics,
                SLOT(onSourceChange(QString, QVariant)));
    }

    // 创建外边框图元
    dataPtr->outlineGraphics = new TransformProxyOutlineGraphicsObject(canvasScene, this);
    connect(dataPtr->sourceProxy.data(), &SourceProxy::sourceChange, dataPtr->outlineGraphics,
            &TransformProxyOutlineGraphicsObject::onSourceSizeChanged);

    dataPtr->badgeGraphics = new BadgeGraphicsObject(canvasScene, this);
    dataPtr->badgeGraphics->setMsgTip(dataPtr->sourceProxy->getBadgeMsg());
    dataPtr->badgeGraphics->userShow(canvasContext->isShowModelRunningSort());

    adjustSize();
    updatePosition();
    refreshSizeControlGraphics(isSelected());
    adjustPos();
    bool isLoading = canvasScene->isLoadingCanvas();
    if (!isLoading) {
        setSelected(true);
    } else {
        bool nameIsShow = true;
        if (dataPtr->nameGraphics) {
            dataPtr->nameGraphics->userShow(nameIsShow);
        }
    }

    if (dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer) {
        bool isAnnotationVisible = canvasContext->getCanvasProperty().isAnnotationVisible();
        if (isAnnotationVisible) {
            dataPtr->sourceGraphics->getLayerProperty().setVisible(true);
            dataPtr->sourceGraphics->userShow(true);
            userShow(true);
        }
    }
}

void TransformProxyGraphicsObject::rotateOnCenter(int angle, Qt::Axis axis /* = Qt::ZAxis */)
{
    QSharedPointer<SourceProxy> sourceProxy = dataPtr->sourceProxy;
    SourceGraphicsObject *sourceGraphics = dataPtr->sourceGraphics;
    if (sourceProxy.isNull() || nullptr == sourceGraphics) {
        return;
    }

    // 旋转的时候，先清理该图元端口的连接线的分支点
    for each (PortGraphicsObject *portGraphics in getPortGraphicsObjectList()) {
        if (portGraphics) {
            portGraphics->cleanConnectorWireBranchPoints();
        }
    }

    QRectF lastRect = sourceProxy->getCombineTransform().mapRect(sourceGraphics->boundingRect());
    QPointF lastCenter = lastRect.center();

    QTransform rotateTrans;
    rotateTrans.rotate(angle, axis);
    switch (axis) {
    case Qt::XAxis:
        if (angle % 180 != 0) {
            return;
        }
        sourceProxy->setxAxisFlipTransform(sourceProxy->xAxisFlipTransform() * rotateTrans);
        break;
    case Qt::YAxis:
        if (angle % 180 != 0) {
            return;
        }
        sourceProxy->setyAxisFlipTransform(sourceProxy->yAxisFlipTransform() * rotateTrans);
        break;
    case Qt::ZAxis:

        sourceProxy->setRotateTransform(sourceProxy->rotateTransform() * rotateTrans);
        break;
    }

    QRectF currentRect = sourceProxy->getCombineTransform().mapRect(sourceGraphics->boundingRect());
    QPointF currentCenter = currentRect.center();

    QPointF offset = (lastCenter - currentCenter);

    // 处理上次旋转之后为了保持资源对齐方式(比如网格对齐)所添加的平移偏移量
    // 上次添加了一个平移,那么这次就把之前的平移量取消
    offset = offset - dataPtr->alignmentOffset;

    sourceProxy->setTranslateTransform(sourceProxy->translateTransform()
                                       * QTransform::fromTranslate(offset.x(), offset.y()));

    // 资源图元应用Transform变换矩阵
    dataPtr->sourceGraphics->applyTransform();

    // 旋转之后的资源坐标位置
    QPointF sourcePos = dataPtr->sourceGraphics->mapToScene(dataPtr->sourceGraphics->pos());
    // 根据资源对齐方式,计算之后的坐标
    QPointF alignSourcePos = Utility::calculateAlignmentPos(sourceGraphics, sourcePos);
    // 计算对齐的平移偏移量
    dataPtr->alignmentOffset = alignSourcePos - sourcePos;
    // 如果对齐的平移偏移量不为0，那么需要把对齐偏移量应用到资源图元
    if (dataPtr->alignmentOffset != QPointF(0, 0)) {
        sourceProxy->setTranslateTransform(
                sourceProxy->translateTransform()
                * QTransform::fromTranslate(dataPtr->alignmentOffset.x(), dataPtr->alignmentOffset.y()));
        dataPtr->sourceGraphics->applyTransform();
    }
    // 根据资源图元的transform变换矩阵，计算代理图元的boundingrect
    setBoundingRect(dataPtr->sourceGraphics->getTransformBoundingRect());

    if (axis == Qt::ZAxis) {
        // 发射信号，通知代理图层发生了旋转
        emit transformChanged(boundingRect(), getAngle());
    }

    updatePosition();
    // 旋转之后，重新计算分支点
    for each (PortGraphicsObject *portGraphics in getPortGraphicsObjectList()) {
        if (portGraphics) {
            portGraphics->calcuteConnectorWireBranchPoints();
        }
    }

    // 旋转完成之后，重新设置连接线的是否可移动状态
    /*     for each (auto item in selectedWireItems) {
            if (item) {
                item->setFlag(ItemIsMovable, true);
            }
        } */
}

void TransformProxyGraphicsObject::applyTransforms(QTransform scale, QTransform rotate, QTransform translate,
                                                   QTransform xFlipTransform, QTransform yFlipTransform)
{
    QSharedPointer<SourceProxy> sourceProxy = dataPtr->sourceProxy;
    if (sourceProxy.isNull() || !dataPtr->sourceGraphics) {
        return;
    }
    SourceGraphicsObject *sourceGraphics = dataPtr->sourceGraphics;
    QTransform oldTransform = sourceProxy->getCombineTransform();

    QRectF lastRect = oldTransform.mapRect(dataPtr->sourceGraphics->boundingRect());
    QPointF lastCenter = lastRect.center();

    sourceProxy->setScaleTransform(scale);
    sourceProxy->setRotateTransform(rotate);
    sourceProxy->setTranslateTransform(translate);
    sourceProxy->setxAxisFlipTransform(xFlipTransform);
    sourceProxy->setyAxisFlipTransform(yFlipTransform);

    QTransform newTransform = sourceProxy->getCombineTransform();

    QRectF currentRect = newTransform.mapRect(dataPtr->sourceGraphics->boundingRect());
    QPointF currentCenter = currentRect.center();

    QPointF offset = (lastCenter - currentCenter);

    // 处理上次旋转之后为了保持资源对齐方式(比如网格对齐)所添加的平移偏移量
    // 上次添加了一个平移,那么这次就把之前的平移量取消
    offset = offset - dataPtr->alignmentOffset;

    // 资源图元应用Transform变换矩阵
    dataPtr->sourceGraphics->applyTransform();
    QRectF rc = dataPtr->sourceGraphics->getTransformBoundingRect();

    // 旋转之后的资源坐标位置
    QPointF sourcePos = dataPtr->sourceGraphics->mapToScene(dataPtr->sourceGraphics->pos());
    // 根据资源对齐方式,计算之后的坐标
    QPointF alignSourcePos = Utility::calculateAlignmentPos(sourceGraphics, sourcePos);

    // 计算对齐的平移偏移量
    dataPtr->alignmentOffset = alignSourcePos - sourcePos;
    // 如果对齐的平移偏移量不为0，那么需要把对齐偏移量应用到资源图元
    if (dataPtr->alignmentOffset != QPointF(0, 0)) {
        sourceProxy->setTranslateTransform(
                sourceProxy->translateTransform()
                * QTransform::fromTranslate(dataPtr->alignmentOffset.x(), dataPtr->alignmentOffset.y()));
        dataPtr->sourceGraphics->applyTransform();
    }
    // 根据资源图元的transform变换矩阵，计算代理图元的boundingrect
    setBoundingRect(dataPtr->sourceGraphics->getTransformBoundingRect());

    // 发射信号，通知代理图层发生了旋转
    emit transformChanged(boundingRect(), getAngle());
    updatePosition();
}

int TransformProxyGraphicsObject::getAngle() const
{
    if (dataPtr->sourceProxy.isNull()) {
        return 0;
    }
    return Utility::getTransformAngle(dataPtr->sourceProxy->getSRTTransform());
}

void TransformProxyGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    int nXOffset_1 = 20;
    int nXOffset_2 = 15;
    int nYOffset = 10;
    if (60 > boundingRect().width()) {
        nXOffset_1 -= nYOffset;
        nXOffset_2 -= nYOffset;
    }

    painter->save();
    QColor color = sourceProxy->getStateColor();
    QPen pen(color, 1, Qt::SolidLine);
    painter->setPen(pen);

    QRectF rc = boundingRect();
    bool isBaseLayerVisible = getCanvasContext()->getCanvasProperty().isBaseLayerVisible();
    if (sourceProxy->isHadUpperLimitation() && isBaseLayerVisible) {
        painter->drawLine(QPointF(rc.left() + boundingRect().width() - nXOffset_1, rc.top()),
                          QPointF(rc.left() + boundingRect().width() - nXOffset_2, rc.top() - nYOffset));
        painter->drawLine(QPointF(rc.left() + boundingRect().width() - nXOffset_2, rc.top() - nYOffset),
                          QPointF(rc.left() + boundingRect().width() + nYOffset, rc.top() - nYOffset));
    }

    if (sourceProxy->isHadLowerLimitation() && isBaseLayerVisible) {
        painter->drawLine(QPointF(rc.left() + nXOffset_1, rc.bottom()),
                          QPointF(rc.left() + nXOffset_2, rc.bottom() + nYOffset));
        painter->drawLine(QPointF(rc.left() + nXOffset_2, rc.bottom() + nYOffset),
                          QPointF(rc.left() - nYOffset, rc.bottom() + nYOffset));
    }

    if (sourceProxy->getCommentState() == "through") {
        QPolygonF inputPorPointtList;
        QPolygonF outPorPointtList;
        for each (auto port in getPortGraphicsObjectList()) {
            if (port->isOutputPort()) {
                outPorPointtList.push_back(port->pos());
            } else {
                inputPorPointtList.push_back(port->pos());
            }
        }
        Qt::PenStyle style = (Qt::PenStyle)getCanvasContext()->lineStyle();
        QColor lineColor = getCanvasContext()->lineColor();
        QPen pen(lineColor, 1, style);
        painter->setPen(pen);
        for (int i = 0; i < inputPorPointtList.size() && inputPorPointtList.size() == outPorPointtList.size(); ++i) {
            painter->drawLine(inputPorPointtList[i], outPorPointtList[i]);
        }
    }
    painter->restore();
}

void TransformProxyGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext || !dataPtr->sourceProxy) {
        return QGraphicsObject::mouseDoubleClickEvent(event);
    }
    auto businessHook = canvasContext->getBusinessHooksServer();
    if (!businessHook) {
        return QGraphicsObject::mouseDoubleClickEvent(event);
    }

    bool isexist = businessHook->isExistBlock(dataPtr->sourceProxy);
    bool stateIsValid = businessHook->checkBlockState(dataPtr->sourceProxy);
    // 控制模块被删除原型的模块无法双击 控制模块被修改后状态为error标红的不能双击打开
    if (!isexist || !stateIsValid) {
        blog(QString(tr(
                "The module prototype has been removed or modified and cannot be edited"))); // 模块原型已被删除或修改，无法编辑
        return;
    }
    bool canvasCanModify = canvasContext->canModify();
    if (event->button() == Qt::LeftButton) {
        // add by liwenyu 2023.08.25 https://zt.xtkfpt.online/bug-view-9943.html
        if (!canvasCanModify) {
            // 画板锁定状态下不能修改注解图元
            if (dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer) {
                event->accept();
                return QGraphicsObject::mouseDoubleClickEvent(event);
            }
        }
        // end
        if (!dataPtr->sourceProxy.isNull()) {
            // 双击，触发属性显示信号
            dataPtr->sourceProxy->triggerMouseDoubleClick();
        }
        event->accept();
    }
    QGraphicsObject::mouseDoubleClickEvent(event);
}

void TransformProxyGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return GraphicsLayer::mouseMoveEvent(event);
    }
    // 标记场景有代理图元在移动中
    canvasScene->setTransformProxyChanging(true);
    if (dataPtr->isCopy) {
        auto canvasCtx = canvasScene->getCanvasContext();
        if (canvasCtx && !canvasCtx->canModify()) {
            return GraphicsLayer::mouseMoveEvent(event);
        }
        QPointF nowScenePos = event->scenePos();
        if (!dataPtr->mousePressedPoint.isNull()) {
            if ((nowScenePos - dataPtr->mousePressedPoint).manhattanLength() >= 10) {
                event->ignore();
                dataPtr->mousePressedPoint = QPointF();
                this->clearFocus();
                onTriggerCopy();
            }
            return;
        }
    }
    GraphicsLayer::mouseMoveEvent(event);
}

void TransformProxyGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto canvasScene = getCanvasScene();
    canvasScene->setTransformProxyChanging(false);

    auto selectedItems = canvasScene->selectedItems();
    dataPtr->isMousePressed = false;
    dataPtr->isMovingCleanBranchPoints = false;
    QPointF nowPos = pos();

    // 清除对齐线
    clearAlignLine();
    dataPtr->readyCheckPorts.clear();
    dataPtr->readyMatchPorts.clear();

    if (nowPos != dataPtr->lastPos && !selectedItems.isEmpty()) {
        PositionCommand *command = new PositionCommand(canvasScene);
        for each (auto item in selectedItems) {
            if (item->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (proxyGraphics) {
                    auto sourceProxy = proxyGraphics->getSourceProxy();
                    if (sourceProxy) {
                        sourceProxy->setPos(proxyGraphics->pos());
                    }
                    command->addPositionOffset(proxyGraphics->id(), nowPos - dataPtr->lastPos);
                }
            } else if (item->type() == kConnectorWireGraphics) {
                ConnectorWireGraphicsObject *wire = dynamic_cast<ConnectorWireGraphicsObject *>(item);
                if (wire && wire->isSourceBothSelected()) {
                    // 如果连接线两端的资源都是选择状态，那么设置连接线可移动
                    QPointF wirePos = wire->pos();
                    QPolygonF points = wire->getPoints();
                    int pointsSize = points.size();
                    for (int i = 0; i < pointsSize; i++) {
                        points[i] = points[i] + wirePos;
                    }
                    wire->setPoints(points);
                    wire->cachePoints();
                    wire->setPos(QPointF(0, 0));
                    wire->setFlag(ItemIsMovable, false);
                }
            }
        }
        QMap<QString, PConnectorWireContext> nowWiresContext;
        QMapIterator<QString, PConnectorWireContext> wireCtxIter(dataPtr->lastConnectorWireCtx);
        while (wireCtxIter.hasNext()) {
            wireCtxIter.next();
            QString wireUUID = wireCtxIter.key();
            PConnectorWireContext nowWireCtx = canvasContext->getConnectorWireContext(wireUUID);
            if (nowWireCtx) {
                nowWiresContext[wireUUID] = PConnectorWireContext(new ConnectorWireContext(*nowWireCtx));
            }
        }
        command->setConnectorWireContext(dataPtr->lastConnectorWireCtx, nowWiresContext);
        auto undoStack = canvasScene->getUndoStack();
        if (undoStack) {
            undoStack->push(command);
        }
        dataPtr->lastConnectorWireCtx.clear();

        QRectF rc = unitedChildsBoundingRect();
        QRectF changingRect = QRectF(pos() + rc.topLeft(), QSizeF(rc.width(), rc.height()));
        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
        if (defaultScene) {
            QRectF rect = defaultScene->getAllItemUnitedRect();
            if (rect.left() > 0) {
                rect.setLeft(0);
            }
            if (rect.top() > 0) {
                rect.setTop(0);
            }
            if (rect.right() < 2239) {
                rect.setRight(2239);
            }
            if (rect.bottom() < 1583) {
                rect.setBottom(1583);
            }
            defaultScene->setSceneRect(rect);
        }
    }
    if (dataPtr->isCopy) {
        // 触发了复制的操作
        if (!dataPtr->copySourceUUID.isEmpty()) {
            TransformProxyGraphicsObject *copyProxy =
                    canvasScene->getTransformProxyGraphicsByID(dataPtr->copySourceUUID);
            if (copyProxy) {
                PSourceProxy sourceProxy = copyProxy->getSourceProxy();
                QRectF rcBound = copyProxy->boundingRect();
                QRectF rcBound1 = QRectF(0, 0, rcBound.width(), rcBound.height());
                QPointF offset = rcBound.center() - rcBound1.center();
                ControlCopyCommand *command = new ControlCopyCommand(getCanvasScene());
                QPointF itemPos = copyProxy->pos();
                //+ offset; // copyProxy->mapToScene(copyProxy->boundingRect()).boundingRect().center();
                sourceProxy->setPos(itemPos);
                command->addSourceProxy(sourceProxy);
                auto undoStack = canvasScene->getUndoStack();
                if (undoStack) {
                    undoStack->push(command);
                }
            }
            dataPtr->copySourceUUID = "";
            setSelected(false);
        } else {
            if (canvasContext->canModify()) {
                // 没有触发复制操作,那么执行模块间自动连线
                onTriggerAutoConnect();
            }
        }
    } else {
        QString prototypeName = dataPtr->sourceProxy->prototypeName();
        if ((prototypeName == "Busbar" || prototypeName == "DotBusbar") && getSourceGraphicsObject()) {
            getSourceGraphicsObject()->mouseReleaseEvent(event);
        }
    }

    dataPtr->isCopy = false;
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
    if (defaultScene) {
        defaultScene->refreshLastClickedProxyGraphics(this);
    }

    GraphicsLayer::mouseReleaseEvent(event);
}

void TransformProxyGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }

    canvasScene->resetSubsystemGraphics();

    // 保存鼠标点击该图元时的位置坐标
    dataPtr->lastPos = pos();
    dataPtr->lastConnectorWireCtx.clear();
    dataPtr->isMousePressed = true;
    dataPtr->mousePressedPoint = event->scenePos();

    dataPtr->readyCheckPorts.clear();
    dataPtr->readyMatchPorts.clear();

    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        dataPtr->isCopy = true;
        GraphicsLayer::mousePressEvent(event);
        return;
    }

    auto allSelectedItems = canvasScene->selectedItems();
    for each (auto item in allSelectedItems) {
        if (item->type() == kConnectorWireGraphics) {
            ConnectorWireGraphicsObject *wire = dynamic_cast<ConnectorWireGraphicsObject *>(item);
            if (wire && wire->isSourceBothSelected()) {
                // 如果连接线两端的资源都是选择状态，那么设置连接线可移动
                wire->setFlag(ItemIsMovable, true);
            }
        } else if (item->type() == kTransformProxyGraphics) {
            // 保存当前关联的连接线的状态
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                auto portGraphics = proxyGraphics->getPortGraphicsObjectList();
                for each (auto port in portGraphics) {
                    if (port) {
                        auto wires = port->getLinkedConnectorWireList();
                        for each (auto wire in wires) {
                            PConnectorWireContext wireCtx = wire->getConnectorWireContext();
                            if (!dataPtr->lastConnectorWireCtx.contains(wireCtx->uuid())) {
                                PConnectorWireContext cacheCtx =
                                        QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(*wireCtx));
                                dataPtr->lastConnectorWireCtx[wireCtx->uuid()] = cacheCtx;
                            }
                        }
                    }
                }
            }
        }
    }

    // 获取当前代理图元的连接线信息
    auto portGraphics = getPortGraphicsObjectList();
    for each (auto port in portGraphics) {
        if (port) {
            auto wires = port->getLinkedConnectorWireList();
            for each (auto wire in wires) {
                PConnectorWireContext wireCtx = wire->getConnectorWireContext();
                if (!dataPtr->lastConnectorWireCtx.contains(wireCtx->uuid())) {
                    PConnectorWireContext cacheCtx =
                            QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(*wireCtx));
                    dataPtr->lastConnectorWireCtx[wireCtx->uuid()] = cacheCtx;
                }
            }
        }
    }

    GraphicsLayer::mousePressEvent(event);
}

void TransformProxyGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    dataPtr->isHover = true;
    GraphicsLayer::hoverEnterEvent(event);
}

void TransformProxyGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    dataPtr->isHover = false;
    GraphicsLayer::hoverLeaveEvent(event);
}

void TransformProxyGraphicsObject::keyPressEvent(QKeyEvent *event)
{

    GraphicsLayer::keyPressEvent(event);
}

void TransformProxyGraphicsObject::refreshSizeControlGraphics(bool isSelected)
{
    if (dataPtr->sizeControlGraphics.size() <= 0) {
        return;
    }
    if (dataPtr->sourceGraphics && dataPtr->sourceGraphics->getScaleDirection() == SourceGraphicsObject::kScaleNone) {
        for each (SizeControlGraphicsObject *sizeControl in dataPtr->sizeControlGraphics) {
            if (sizeControl) {
                sizeControl->getLayerProperty().setVisible(false);
                sizeControl->userShow(false);
            }
        }
        return;
    } else { // 恢复SizeControl图层隐藏属性
        for each (SizeControlGraphicsObject *sizeControl in dataPtr->sizeControlGraphics) {
            if (sizeControl) {
                sizeControl->getLayerProperty().setVisible(true);
                sizeControl->userShow(true);
            }
        }
    }
    SourceGraphicsObject::ScaleDirection scaleDir = dataPtr->sourceGraphics->getScaleDirection();
    int angle = getAngle();
    if (angle % 180 != 0) {
        if (scaleDir == SourceGraphicsObject::kScaleVertical) {
            scaleDir = SourceGraphicsObject::kScaleHorizontal;
        } else if (scaleDir == SourceGraphicsObject::kScaleHorizontal) {
            scaleDir = SourceGraphicsObject::kScaleVertical;
        }
    }
    // initSizeControlVisibleProperty(scaleDir);

    QRectF rc = boundingRect();
    QListIterator<SizeControlGraphicsObject *> sizeControlIter(dataPtr->sizeControlGraphics);
    while (sizeControlIter.hasNext()) {
        SizeControlGraphicsObject *graphics = sizeControlIter.next();
        if (graphics) {
            graphics->userShow(isSelected);
            switch (graphics->getDirection()) {
            case SizeControlGraphicsObject::kDirectionLeftTop:
                graphics->setPos(rc.topLeft());
                if (scaleDir != SourceGraphicsObject::kScaleAll) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionTopCenter:
                graphics->setPos(rc.center().x(), rc.top());
                if (scaleDir == SourceGraphicsObject::kScaleHorizontal) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionRightTop:
                graphics->setPos(rc.topRight());
                if (scaleDir != SourceGraphicsObject::kScaleAll) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionRightCenter:
                graphics->setPos(rc.right(), rc.center().y());
                if (scaleDir == SourceGraphicsObject::kScaleVertical) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionRightBottom:
                graphics->setPos(rc.bottomRight());
                if (scaleDir != SourceGraphicsObject::kScaleAll) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionBottomCenter:
                graphics->setPos(rc.center().x(), rc.bottom());
                if (scaleDir == SourceGraphicsObject::kScaleHorizontal) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionLeftBottom:
                graphics->setPos(rc.bottomLeft());
                if (scaleDir != SourceGraphicsObject::kScaleAll) {
                    graphics->userShow(false);
                }
                break;
            case SizeControlGraphicsObject::kDirectionLeftCenter:
                graphics->setPos(rc.left(), rc.center().y());
                if (scaleDir == SourceGraphicsObject::kScaleVertical) {
                    graphics->userShow(false);
                }
                break;

            default:
                break;
            }
        }
    }
}

bool TransformProxyGraphicsObject::isCopying()
{
    return dataPtr->isCopy;
}

void TransformProxyGraphicsObject::adjustSize()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene || !dataPtr->sourceGraphics || !dataPtr->sourceProxy) {
        return;
    }
    bool isBlocked = canvasScene->isLoadingCanvas();
    if (isBlocked) {
        return;
    }
    if (dataPtr->sourceProxy->moduleType() != GKD::SOURCE_MODULETYPE_CONTROL) {
        return;
    }

    auto portList = getPortGraphicsObjectList();
    int maxInput = 0;
    int maxOutPut = 0;
    bool isHorizontal = true;
    for each (auto controlPort in portList) {
        if (controlPort) {
            auto controlProtContext = controlPort->getPortContext();
            if (!controlProtContext) {
                continue;
            }
            QRectF rc = controlPort->portNameRect();
            if (controlProtContext->type() == PortInputType) {
                if (controlPort->getAnchorPoint() == Qt::AnchorLeft
                    || controlPort->getAnchorPoint() == Qt::AnchorRight) {
                    if (maxInput < rc.width()) {
                        maxInput = rc.width();
                    }
                } else {
                    if (maxInput < rc.height()) {
                        maxInput = rc.height();
                        isHorizontal = false;
                    }
                }

            } else if (controlProtContext->type() == PortOutputType) {
                if (controlPort->getAnchorPoint() == Qt::AnchorLeft
                    || controlPort->getAnchorPoint() == Qt::AnchorRight) {
                    if (maxOutPut < rc.width()) {
                        maxOutPut = rc.width();
                    }
                } else {
                    if (maxOutPut < rc.height()) {
                        maxOutPut = rc.height();
                        isHorizontal = false;
                    }
                }
            }
        }
    }
    int maxValue = qMax(maxInput, maxOutPut);
    QSizeF size = dataPtr->sourceProxy->size();
    QSizeF expandSize;
    if (isHorizontal) {
        expandSize = QSizeF(maxValue * 2, 0);
    } else {
        expandSize = QSizeF(0, maxValue * 2);
    }
    expandSize = Utility::sizeAlignmentToGrid(expandSize, 10);
    dataPtr->sourceGraphics->adjustBoundingRect(QRectF(QPointF(0, 0), size + expandSize));
    applyTransformChanged();
}

void TransformProxyGraphicsObject::adjustPos()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene || !dataPtr->sourceProxy) {
        return;
    }
    // 如果画板在加载中，就不调整位置了
    bool isBlocked = canvasScene->isLoadingCanvas();
    if (isBlocked) {
        return;
    }
    QRectF sceneRc = mapToScene(boundingRect()).boundingRect();
    QPointF centerPos = sceneRc.center();
    QPointF offset = dataPtr->sourceProxy->pos() - centerPos;
    Utility::pointAlignmentToGrid(offset, 10);
    moveBy(offset.x(), offset.y());
    sceneRc = mapToScene(boundingRect()).boundingRect();
    dataPtr->sourceProxy->setSize(sceneRc.size());
    dataPtr->sourceProxy->setPos(pos());
}

QPointF TransformProxyGraphicsObject::calcChangingRect(QRectF &changingRect, QPointF offset, int direction)
{
    QPointF pointHandle;
    switch (direction) {
    case SizeControlGraphicsObject::kDirectionRightCenter:
    case SizeControlGraphicsObject::kDirectionRightBottom:
    case SizeControlGraphicsObject::kDirectionBottomCenter: {
        pointHandle = changingRect.bottomRight();
        changingRect.setBottomRight(changingRect.bottomRight() + offset);
    } break;
    case SizeControlGraphicsObject::kDirectionLeftCenter:
    case SizeControlGraphicsObject::kDirectionLeftTop:
    case SizeControlGraphicsObject::kDirectionTopCenter: {
        pointHandle = changingRect.topLeft();
        changingRect.setTopLeft(changingRect.topLeft() + offset);
    } break;
    case SizeControlGraphicsObject::kDirectionLeftBottom: {
        pointHandle = changingRect.bottomLeft();
        changingRect.setBottomLeft(changingRect.bottomLeft() + offset);
    } break;
    case SizeControlGraphicsObject::kDirectionRightTop: {
        pointHandle = changingRect.topRight();
        changingRect.setTopRight(changingRect.topRight() + offset);
    } break;
    default: {
    }
    }
    return pointHandle;
}

void TransformProxyGraphicsObject::onTriggerCopy()
{
    auto canvasContext = getCanvasContext();
    if (!dataPtr->sourceProxy || !canvasContext) {
        return;
    }
    auto canvasScene = getCanvasScene();
    auto clipBoard = QApplication::clipboard();
    if (clipBoard) {
        clipBoard->clear();
    }

    // 把新创建的资源的名称给业务层接口处理,然后改变资源的uuid
    PSourceProxy newSource = PSourceProxy(new SourceProxy(*dataPtr->sourceProxy));
    if (!newSource) {
        return;
    }

    SourceProperty &newSourceProperty = newSource->getSourceProperty();

    // 注释模块只有图片注释有名称 其他都不需要名称
    if (dataPtr->sourceProxy->moduleType() != GKD::SOURCE_MODULETYPE_ANNOTATION
        || dataPtr->sourceProxy->prototypeName() == ImageAnnotationStr) {
        BusinessHooksServer *businessHooks = canvasContext->getBusinessHooksServer();
        if (businessHooks) {
            businessHooks->setNewSourceProxy(dataPtr->sourceProxy, newSource); // 重命名
        }
    }
    newSource->setUUID(Utility::createUUID()); // 更新UUID
    newSource->setOriginalUUID(
            dataPtr->sourceProxy->uuid()); // 设置源UUID，表明是从哪一个souce拷贝构造出来的 // 设置坐标
    // 重新设置输入输出序列
    if (newSource->prototypeName() == "In" || newSource->prototypeName() == "Out") {
        newSourceProperty.setInputOutputIndex(-1);
        newSourceProperty.setInputOutputDisplayIndex(-1);
    }
    if (newSource->prototypeName() == "Busbar" || newSource->prototypeName() == "DotBusbar") {
        newSource->resetPortList(QList<QSharedPointer<PortContext>>());
    }
    newSource->setSourceCreateType(SourceProxy::kCreateCopy);
    canvasScene->loadFromSourceProxy(newSource);
    GraphicsLayer *proxyGraphics = canvasScene->getTransformProxyGraphicsBySource(newSource);
    canvasScene->clearSelection();
    if (proxyGraphics) {
        proxyGraphics->setSelected(true);
        proxyGraphics->setFocus(Qt::MouseFocusReason);
        // 数据埋点，统计模块使用次数
        BuriedData &data = BuriedData::getInstance();
        QString typeString = "Control";

        if (canvasContext->type() == CanvasContext::kElectricalType
            || canvasContext->type() == CanvasContext::kElecUserDefinedType) {
            typeString = "Electrical";
        }
        data.increaseSourceUsage(typeString.toStdString(), newSource->prototypeName().toStdString());
    }
    setSelected(false);
    dataPtr->copySourceUUID = newSource->uuid();
}

void TransformProxyGraphicsObject::onTriggerAutoConnect()
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
    if (defaultScene) {
        TransformProxyGraphicsObject *otherProxy = defaultScene->getLastClickedPorxyGraphics();
        if (otherProxy && otherProxy != this) {
            // 上次点击的图元，未连接的输出端口列表
            QList<PortGraphicsObject *> nolinkOutputPorts;
            for each (auto port in otherProxy->getPortGraphicsObjectList()) {
                if (port && port->getLinkedConnectorWireList().isEmpty() && port->isOutputPort()) {
                    nolinkOutputPorts.push_back(port);
                }
            }

            // 本次点击的图元，未连接的输入端口列表
            QList<PortGraphicsObject *> nolinkInputPorts;
            for each (auto port in getPortGraphicsObjectList()) {
                if (port && port->getLinkedConnectorWireList().isEmpty() && !port->isOutputPort()) {
                    nolinkInputPorts.push_back(port);
                }
            }

            int outputSize = nolinkOutputPorts.size();
            int inputSize = nolinkInputPorts.size();
            int minSize = outputSize < inputSize ? outputSize : inputSize;

            for (int i = 0; i < minSize; i++) {
                PortGraphicsObject *outputPort = nolinkOutputPorts[i];
                PortGraphicsObject *inputPort = nolinkInputPorts[i];
                defaultScene->onNewConnectWireShouldBeCreate(outputPort, inputPort);
            }
            otherProxy->setSelected(false);
        }
    }
}

void TransformProxyGraphicsObject::initSizeControlVisibleProperty(SourceGraphicsObject::ScaleDirection scaleDir)
{
    if (dataPtr->sizeControlGraphics.size() <= 0) {
        return;
    }
    if (dataPtr->sourceGraphics) {
        QVariantMap visibleProperties;
        visibleProperties[LayerPropertyVisible] = true;
        QVariantMap unVisibleProperties;
        unVisibleProperties[LayerPropertyVisible] = false;
        // SourceGraphicsObject::ScaleDirection scaleDir = dataPtr->sourceGraphics->getScaleDirection();
        switch (scaleDir) {
        case SourceGraphicsObject::kScaleNone: {
            for each (SizeControlGraphicsObject *sizeControl in dataPtr->sizeControlGraphics) {
                if (sizeControl) {
                    sizeControl->getLayerProperty().setVisible(false);
                    sizeControl->userShow(false);
                }
            }
        } break;
        case SourceGraphicsObject::kScaleHorizontal: {
            for each (SizeControlGraphicsObject *sizeControl in dataPtr->sizeControlGraphics) {
                if (sizeControl->getDirection() == SizeControlGraphicsObject::kDirectionLeftCenter
                    || sizeControl->getDirection() == SizeControlGraphicsObject::kDirectionRightCenter) {
                    sizeControl->getLayerProperty().setVisible(true);
                } else {
                    sizeControl->getLayerProperty().setVisible(false);
                }
                sizeControl->userShow(false);
            }
        } break;
        case SourceGraphicsObject::kScaleVertical: {
            for each (SizeControlGraphicsObject *sizeControl in dataPtr->sizeControlGraphics) {
                if (sizeControl->getDirection() == SizeControlGraphicsObject::kDirectionTopCenter
                    || sizeControl->getDirection() == SizeControlGraphicsObject::kDirectionBottomCenter) {
                    sizeControl->getLayerProperty().setVisible(true);
                } else {
                    sizeControl->getLayerProperty().setVisible(false);
                }
                sizeControl->userShow(false);
            }
        } break;
        case SourceGraphicsObject::kScaleAll: {
            for each (SizeControlGraphicsObject *sizeControl in dataPtr->sizeControlGraphics) {
                if (sizeControl) {
                    sizeControl->getLayerProperty().setVisible(true);
                    sizeControl->userShow(false);
                }
            }
        } break;
        default: {
        }
        }
    }
}

QPointF TransformProxyGraphicsObject::calcuteOverlapPos(QPointF pos, QSizeF size)
{
    auto scene = getCanvasScene();
    if (!scene || scene->allowOverlap()) {
        return pos;
    }
    QRectF detechRect = QRectF(pos, size);
    QList<QGraphicsItem *> detectItems = scene->items(detechRect);
    if (detectItems.isEmpty()) {
        return pos;
    }
    for each (QGraphicsItem *item in detectItems) {
        // 如果检测区域内有代理图元，说明重叠了，
        if (item->type() == kTransformProxyGraphics && item != this) {
            int gridSpace = scene->getGridSpace();
            // 将坐标向下移动一个网格
            return calcuteOverlapPos(pos + QPointF(0, gridSpace), size);
        }
    }
    return pos;
}

QVariant TransformProxyGraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return QGraphicsObject::itemChange(change, value);
    }
    auto canvasScene = getCanvasScene();
    QPointF movingPos, alignMovingPos, movingOffset;
    QRectF detechRect;
    switch (change) {
    case QGraphicsItem::ItemSelectedHasChanged: {
        bool isSelectedStatus = isSelected();
        emit selectedChanged(isSelectedStatus);
        if (isSelectedStatus) {
            TransformItemGroup *transformGroup = canvasScene->getTransformItemGroup();
            if (transformGroup && !transformGroup->hasLayer(this->id()) && !group()) {
                transformGroup->addToGroup(this);
                refreshSizeControlGraphics(false);
                transformGroup->resetBoundingRect();
                if (dataPtr->sourceGraphics) {
                    dataPtr->sourceGraphics->setHoverStatus(false);
                }
                return QGraphicsObject::itemChange(change, value);
            }
        }
        if (!isSelectedStatus) {
            clearAlignLine();
        }
        refreshSizeControlGraphics(isSelectedStatus);
        GeometricGraphicsAnnotation *geoAnnotationGraphics =
                dynamic_cast<GeometricGraphicsAnnotation *>(dataPtr->sourceGraphics);
        if (geoAnnotationGraphics) {
            geoAnnotationGraphics->refreshAnnotationPoint(isSelectedStatus);
        }
        TextGraphicsAnnotation *textAnnotationGraphics =
                dynamic_cast<TextGraphicsAnnotation *>(dataPtr->sourceGraphics);
        if (textAnnotationGraphics) {
            textAnnotationGraphics->refreshSizeControlGraphics(isSelectedStatus);
        }
    } break;
    case QGraphicsItem::ItemPositionHasChanged: {
        // 触发位置变化信号
        QPointF currentPos = pos();
        if (dataPtr->lastScenePos.isNull()) {
            dataPtr->lastScenePos = currentPos;
        } else {
            TransformItemGroup *transformGroup = canvasScene->getTransformItemGroup();
            // 场景中不存在图元组的情况下，才同步和当前模块关联的悬空连接线的位置
            if (!transformGroup) {
                QPointF offsetPos = currentPos - dataPtr->lastScenePos;
                auto allWires = getLinkedConnectorWires();
                for (auto wire : allWires) {
                    if (wire && !wire->isConnectedWithPort()) {
                        auto wireCtx = wire->getConnectorWireContext();
                        auto points = wireCtx->points();
                        for (auto &point : points) {
                            point = point + offsetPos;
                        }
                        wireCtx->setPoints(points);
                    }
                }
            }

            dataPtr->lastScenePos = currentPos;
        }

        emit positionChanged(currentPos);
        updatePosition();
        if (dataPtr->isMousePressed) {
            createConnectionPrompt();
        }
        createAlignLine();

    } break;
    case QGraphicsItem::ItemTransformHasChanged: {
        updatePosition();
    } break;
    case QGraphicsItem::ItemTransformChange: {

    } break;
    case QGraphicsItem::ItemPositionChange: {
        bool itemCanMove = true;
        if (!canvasScene->isLoadingCanvas()) {
            itemCanMove = canvasContext->canModify();
            if (itemCanMove) {
                auto interactioMode = canvasContext->getInteractionMode();
                // 区域放大模式下也不允许拖动
                if (interactioMode == kAreaAmplification) {
                    itemCanMove = false;
                }
            }
        }
        if (dataPtr->isCopy || !itemCanMove) {
            return pos();
        }

        dataPtr->alignmentOffset = QPointF(0, 0);
        movingPos = value.toPointF();
        alignMovingPos = movingPos;

        if (dataPtr->sourceGraphics
            && !(dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer)) {
            alignMovingPos = Utility::pointAlignmentToGrid(movingPos, 10);
        }

        if (boundingRect().isEmpty() || getCanvasScene()->getTransformItemGroup()) {
            return alignMovingPos;
        }
        QRectF rc = unitedChildsBoundingRect();

        QRectF changingRect = QRectF(movingPos + rc.topLeft(), QSizeF(rc.width(), rc.height()));
        // 如果移动到场景外
        QRectF sceneRect = canvasScene->sceneRect();

        // add by liwenyu 2023.07.05 研发需求:https://zt.xtkfpt.online/story-view-143.html
        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
        if (defaultScene) {
            QRectF copyRightRect = defaultScene->getCopyRightRect();
            if (getCanvasContext()->getCanvasProperty().isLegendVisible() && changingRect.intersects(copyRightRect)) {
                return pos();
            }
        }
        // end

        // 增加 dataPtr->outlineGraphics 条件是为了保证不是在代理的createInternalGraphics里面设置坐标时进行边界判断
        // 解决https://zt.xtkfpt.online/bug-view-6155.html
        /* if (!sceneRect.contains(changingRect) && dataPtr->outlineGraphics) {
            return Utility::adjustItemChangePos(sceneRect, changingRect, alignMovingPos, pos());
        } */
        if (changingRect.left() < sceneRect.left()) {
            sceneRect.setLeft(changingRect.left());
            canvasScene->setSceneRect(sceneRect);
        } else if (changingRect.right() > sceneRect.right()) {
            sceneRect.setRight(changingRect.right());
            canvasScene->setSceneRect(sceneRect);
        }

        if (changingRect.top() < sceneRect.top()) {
            sceneRect.setTop(changingRect.top());
            canvasScene->setSceneRect(sceneRect);
        } else if (changingRect.bottom() > sceneRect.bottom()) {
            sceneRect.setBottom(changingRect.bottom());

            canvasScene->setSceneRect(sceneRect);
        }
        return alignMovingPos;
    } break;
    default: {
    }
    }
    return QGraphicsObject::itemChange(change, value);
}

QList<ConnectorWireGraphicsObject *> TransformProxyGraphicsObject::getLinkedConnectorWires()
{
    QList<ConnectorWireGraphicsObject *> linkWires;
    auto ports = getPortGraphicsObjectList();
    for each (auto port in ports) {
        if (port) {
            linkWires.append(port->getLinkedConnectorWireList());
        }
    }
    return linkWires;
}

void TransformProxyGraphicsObject::emitSourceNameHasChanged(QPointF pointDelta)
{
    emit sourceNamePositionChanged(pointDelta);
}

QRectF TransformProxyGraphicsObject::getTransformSceneRect()
{
    return mapRectToScene(boundingRect());
}

QRectF TransformProxyGraphicsObject::getSourceNameBounding()
{
    if (dataPtr->nameGraphics) {
        return dataPtr->nameGraphics->mapToParentBoundingRect();
    } else {
        return QRectF(0, 0, 0, 0);
    }
}

bool TransformProxyGraphicsObject::isOnSizeChange()
{
    return dataPtr->isOnSizeChange;
}

void TransformProxyGraphicsObject::createAlignLine()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    clearAlignLine();

    if (dataPtr->isCopy || !isSelected())
        return;

    // 水平线
    alignHorizontalLine();
    // 垂直线
    alignVerticalLine();
    // 中心线
    alignCenterLine();

    QColor penColor(0x2E9FE6);
    penColor.setAlpha(128);
    QPen pen(penColor, 1.0, Qt::SolidLine);
    pen.setJoinStyle(Qt::MiterJoin);
    for each (auto line in dataPtr->alignLine) {
        QGraphicsLineItem *item = canvasScene->addLine(line, pen);
        if (item) {
            dataPtr->alignLineItems.push_back(item);
        }
    }
}

void TransformProxyGraphicsObject::alignHorizontalLine()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    if (!dataPtr->sourceGraphics || !(dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kSourceGraphicsLayer)) {
        return;
    }
    QLineF lineLeft;
    QLineF lineRight;
    // 上方的水平对齐线
    QRectF topArea(QPointF(canvasScene->sceneRect().left(), getTransformSceneRect().top() - 0.5),
                   QSizeF(canvasScene->width(), 1));
    auto containsTop = canvasScene->transProxyAtRectF(topArea);
    for each (auto item in containsTop) {
        TransformProxyGraphicsObject *proxyGraphics = item;
        if (!checkAlignRule(proxyGraphics)) {
            continue;
        }
        // 去除不是对齐边界的情况
        QRectF senceRectF = getTransformSceneRect();
        QRectF otherSenceRectF = proxyGraphics->getTransformSceneRect();
        if (senceRectF.top() > otherSenceRectF.top() - 0.5
            && senceRectF.top() < otherSenceRectF.top() + 0.5) // 在边界内
        {
            // 往左边的对齐线
            QLineF left(otherSenceRectF.left() - ALIGN_SHIFT, senceRectF.top(), senceRectF.right() + ALIGN_SHIFT,
                        senceRectF.top());
            // 往右边的对齐线
            QLineF right(otherSenceRectF.right() + ALIGN_SHIFT, senceRectF.top(), senceRectF.left() - ALIGN_SHIFT,
                         senceRectF.top());
            // 往左往右都只选最短的一条
            if (otherSenceRectF.left() < senceRectF.left()
                && (left.length() < lineLeft.length() || lineLeft.isNull())) {
                lineLeft = left;
            } else if (otherSenceRectF.right() > senceRectF.right()
                       && (right.length() < lineRight.length() || lineRight.isNull())) {
                lineRight = right;
            }
        }
    }

    if (!lineLeft.isNull()) {
        dataPtr->alignLine.push_back(lineLeft);
        QLineF line1(lineLeft.x1() + ALIGN_SHIFT, lineLeft.y1() + ALIGN_SHIFT, lineLeft.x1() + ALIGN_SHIFT,
                     lineLeft.y1() - ALIGN_SHIFT);
        QLineF line2(lineLeft.x2() - ALIGN_SHIFT, lineLeft.y2() + ALIGN_SHIFT, lineLeft.x2() - ALIGN_SHIFT,
                     lineLeft.y2() - ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
        lineLeft = QLineF();
    }
    if (!lineRight.isNull()) {
        dataPtr->alignLine.push_back(lineRight);
        QLineF line1(lineRight.x1() - ALIGN_SHIFT, lineRight.y1() + ALIGN_SHIFT, lineRight.x1() - ALIGN_SHIFT,
                     lineRight.y1() - ALIGN_SHIFT);
        QLineF line2(lineRight.x2() + ALIGN_SHIFT, lineRight.y2() + ALIGN_SHIFT, lineRight.x2() + ALIGN_SHIFT,
                     lineRight.y2() - ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
        lineRight = QLineF();
    }

    // 下方的水平对齐线
    QRectF buttonArea(QPointF(canvasScene->sceneRect().left(), getTransformSceneRect().bottom() - 0.5),
                      QSizeF(canvasScene->width(), 1));
    auto containsButton = canvasScene->transProxyAtRectF(buttonArea);
    for each (auto item in containsButton) {
        TransformProxyGraphicsObject *proxyGraphics = item;
        if (!checkAlignRule(proxyGraphics)) {
            continue;
        }

        // 去除不是对齐边界的情况
        QRectF senceRectF = getTransformSceneRect();
        QRectF otherSenceRectF = proxyGraphics->getTransformSceneRect();
        if (senceRectF.bottom() > otherSenceRectF.bottom() - 0.5
            && senceRectF.bottom() < otherSenceRectF.bottom() + 0.5) // 在边界内
        {
            // 往左边的对齐线
            QLineF left(otherSenceRectF.left() - ALIGN_SHIFT, senceRectF.bottom(), senceRectF.right() + ALIGN_SHIFT,
                        senceRectF.bottom());
            // 往右边的对齐线
            QLineF right(otherSenceRectF.right() + ALIGN_SHIFT, senceRectF.bottom(), senceRectF.left() - ALIGN_SHIFT,
                         senceRectF.bottom());
            // 往左往右都只选最短的一条
            if (otherSenceRectF.left() < senceRectF.left()
                && (left.length() < lineLeft.length() || lineLeft.isNull())) {
                lineLeft = left;
            } else if (otherSenceRectF.right() > senceRectF.right()
                       && (right.length() < lineRight.length() || lineRight.isNull())) {
                lineRight = right;
            }
        }
    }

    if (!lineLeft.isNull()) {
        dataPtr->alignLine.push_back(lineLeft);
        QLineF line1(lineLeft.x1() + ALIGN_SHIFT, lineLeft.y1() + ALIGN_SHIFT, lineLeft.x1() + ALIGN_SHIFT,
                     lineLeft.y1() - ALIGN_SHIFT);
        QLineF line2(lineLeft.x2() - ALIGN_SHIFT, lineLeft.y2() + ALIGN_SHIFT, lineLeft.x2() - ALIGN_SHIFT,
                     lineLeft.y2() - ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
    }
    if (!lineRight.isNull()) {
        dataPtr->alignLine.push_back(lineRight);
        QLineF line1(lineRight.x1() - ALIGN_SHIFT, lineRight.y1() + ALIGN_SHIFT, lineRight.x1() - ALIGN_SHIFT,
                     lineRight.y1() - ALIGN_SHIFT);
        QLineF line2(lineRight.x2() + ALIGN_SHIFT, lineRight.y2() + ALIGN_SHIFT, lineRight.x2() + ALIGN_SHIFT,
                     lineRight.y2() - ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
    }
}

void TransformProxyGraphicsObject::alignVerticalLine()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    if (!dataPtr->sourceGraphics || !(dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kSourceGraphicsLayer)) {
        return;
    }
    QLineF lineTop;
    QLineF lineButton;
    // 左边的垂直对齐线
    QRectF leftArea(QPointF(getTransformSceneRect().left() - 0.5, canvasScene->sceneRect().top()),
                    QSizeF(1, canvasScene->height()));
    auto transList = canvasScene->transProxyAtRectF(leftArea);
    for each (auto item in transList) {
        TransformProxyGraphicsObject *proxyGraphics = item;
        if (!checkAlignRule(proxyGraphics)) {
            continue;
        }

        // 去除不是对齐边界的情况
        QRectF senceRectF = getTransformSceneRect();
        QRectF otherSenceRectF = proxyGraphics->getTransformSceneRect();
        if (senceRectF.left() > otherSenceRectF.left() - 0.5
            && senceRectF.left() < otherSenceRectF.left() + 0.5) // 在边界内
        {
            // 往上边的对齐线
            QLineF top(senceRectF.left(), otherSenceRectF.top() - ALIGN_SHIFT, senceRectF.left(),
                       senceRectF.bottom() + ALIGN_SHIFT);
            // 往下边的对齐线
            QLineF button(senceRectF.left(), otherSenceRectF.bottom() + ALIGN_SHIFT, senceRectF.left(),
                          senceRectF.top() - ALIGN_SHIFT);
            // 往上往下都只选最短的一条
            if (otherSenceRectF.top() < senceRectF.top() && (top.length() < lineTop.length() || lineTop.isNull())) {
                lineTop = top;
            } else if (otherSenceRectF.bottom() > senceRectF.bottom()
                       && (button.length() < lineButton.length() || lineButton.isNull())) {
                lineButton = button;
            }
        }
    }

    if (!lineTop.isNull()) {
        dataPtr->alignLine.push_back(lineTop);
        QLineF line1(lineTop.x1() + ALIGN_SHIFT, lineTop.y1() + ALIGN_SHIFT, lineTop.x1() - ALIGN_SHIFT,
                     lineTop.y1() + ALIGN_SHIFT);
        QLineF line2(lineTop.x2() + ALIGN_SHIFT, lineTop.y2() - ALIGN_SHIFT, lineTop.x2() - ALIGN_SHIFT,
                     lineTop.y2() - ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
        lineTop = QLineF();
    }
    if (!lineButton.isNull()) {
        dataPtr->alignLine.push_back(lineButton);
        QLineF line1(lineButton.x1() + ALIGN_SHIFT, lineButton.y1() - ALIGN_SHIFT, lineButton.x1() - ALIGN_SHIFT,
                     lineButton.y1() - ALIGN_SHIFT);
        QLineF line2(lineButton.x2() + ALIGN_SHIFT, lineButton.y2() + ALIGN_SHIFT, lineButton.x2() - ALIGN_SHIFT,
                     lineButton.y2() + ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
        lineButton = QLineF();
    }

    // 右边的垂直对齐线
    QRectF rightArea(QPointF(getTransformSceneRect().right() - 0.5, canvasScene->sceneRect().top()),
                     QSizeF(1, canvasScene->height()));
    auto containsButton = canvasScene->transProxyAtRectF(rightArea);
    for each (auto item in containsButton) {
        TransformProxyGraphicsObject *proxyGraphics = item;
        if (!checkAlignRule(proxyGraphics)) {
            continue;
        }

        // 去除不是对齐边界的情况
        QRectF senceRectF = getTransformSceneRect();
        QRectF otherSenceRectF = proxyGraphics->getTransformSceneRect();
        if (senceRectF.right() > otherSenceRectF.right() - 0.5
            && senceRectF.right() < otherSenceRectF.right() + 0.5) // 在边界内
        {
            // 往上边的对齐线
            QLineF top(senceRectF.right(), otherSenceRectF.top() - ALIGN_SHIFT, senceRectF.right(),
                       senceRectF.bottom() + ALIGN_SHIFT);
            // 往下边的对齐线
            QLineF button(senceRectF.right(), otherSenceRectF.bottom() + ALIGN_SHIFT, senceRectF.right(),
                          senceRectF.top() - ALIGN_SHIFT);
            // 往上往下都只选最短的一条
            if (otherSenceRectF.top() < senceRectF.top() && (top.length() < lineTop.length() || lineTop.isNull())) {
                lineTop = top;
            } else if (otherSenceRectF.bottom() > senceRectF.bottom()
                       && (button.length() < lineButton.length() || lineButton.isNull())) {
                lineButton = button;
            }
        }
    }
    if (!lineTop.isNull()) {
        dataPtr->alignLine.push_back(lineTop);
        QLineF line1(lineTop.x1() + ALIGN_SHIFT, lineTop.y1() + ALIGN_SHIFT, lineTop.x1() - ALIGN_SHIFT,
                     lineTop.y1() + ALIGN_SHIFT);
        QLineF line2(lineTop.x2() + ALIGN_SHIFT, lineTop.y2() - ALIGN_SHIFT, lineTop.x2() - ALIGN_SHIFT,
                     lineTop.y2() - ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
    }
    if (!lineButton.isNull()) {
        dataPtr->alignLine.push_back(lineButton);
        QLineF line1(lineButton.x1() + ALIGN_SHIFT, lineButton.y1() - ALIGN_SHIFT, lineButton.x1() - ALIGN_SHIFT,
                     lineButton.y1() - ALIGN_SHIFT);
        QLineF line2(lineButton.x2() + ALIGN_SHIFT, lineButton.y2() + ALIGN_SHIFT, lineButton.x2() - ALIGN_SHIFT,
                     lineButton.y2() + ALIGN_SHIFT);
        dataPtr->alignLine.push_back(line1);
        dataPtr->alignLine.push_back(line2);
    }
}

void TransformProxyGraphicsObject::alignCenterLine()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    if (!dataPtr->sourceGraphics || !(dataPtr->sourceGraphics->getLayerFlag() & GraphicsLayer::kSourceGraphicsLayer)) {
        return;
    }
    QLineF lineTop;
    QLineF lineButton;
    // 中心对齐线
    QRectF centerArea(QPointF(getTransformSceneRect().center().x() - 0.5, canvasScene->sceneRect().top()),
                      QSizeF(1, canvasScene->height()));
    auto containsTop = canvasScene->transProxyAtRectF(centerArea);
    for each (auto item in containsTop) {
        TransformProxyGraphicsObject *proxyGraphics = item;
        if (!checkAlignRule(proxyGraphics)) {
            continue;
        }

        // 去除不是中心对齐的情况
        QPointF center = getTransformSceneRect().center();
        QPointF otherCenter = proxyGraphics->getTransformSceneRect().center();
        if (center.x() > otherCenter.x() - 0.5 && center.x() < otherCenter.x() + 0.5) // 在边界内
        {
            QLineF line(center.x(), otherCenter.y(), center.x(), center.y());
            // 往上往下都只选最短的一条
            if (center.y() > otherCenter.y() && (line.length() < lineTop.length() || lineTop.isNull())) {
                lineTop = line;
            } else if (center.y() < otherCenter.y() && (line.length() < lineButton.length() || lineButton.isNull())) {
                lineButton = line;
            }
        }
    }

    if (!lineTop.isNull()) {
        dataPtr->alignLine.push_back(lineTop);
    }
    if (!lineButton.isNull()) {
        dataPtr->alignLine.push_back(lineButton);
    }
}

bool TransformProxyGraphicsObject::checkAlignRule(TransformProxyGraphicsObject *transProxy)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene || !transProxy) {
        return false;
    }
    // 只有当图元为资源实体图元 并且不是自身的时候才画对齐线
    // 增加筛选掉以组拖动时 组里面其他图元也要去除
    // 俩图元互相包含时也不参与对齐
    TransformItemGroup *group = canvasScene->getTransformItemGroup();
    auto sourceGraphics = transProxy->getSourceGraphicsObject();
    if (!transProxy || transProxy == this || (group && group->hasLayer(this->id()) && group->hasLayer(transProxy->id()))
        || !(sourceGraphics && sourceGraphics->getLayerFlag() & GraphicsLayer::kSourceGraphicsLayer)) {
        return false;
    }
    // 俩图元互相包含时也不参与对齐
    if (getTransformSceneRect().contains(transProxy->getTransformSceneRect())
        || transProxy->getTransformSceneRect().contains(getTransformSceneRect())) {
        return false;
    }

    return true;
}

QList<PortGraphicsObject *> TransformProxyGraphicsObject::getBeCheckedAutoMatchPorts()
{
    QList<PortGraphicsObject *> result;
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return result;
    }
    auto allProxyGraphics = canvasScene->getTransformProxyGraphicsList();
    for each (auto proxy in allProxyGraphics) {
        if (!proxy || proxy == this) {
            continue;
        }
        auto ports = proxy->getPortGraphicsObjectList();
        for each (auto port in ports) {
            if (port->getLinkedConnectorWireList().isEmpty()) {
                result.push_back(port);
            }
        }
    }

    return result;
}

QList<PortGraphicsObject *> TransformProxyGraphicsObject::getReadyCheckPorts()
{
    // 当前图元的端口列表
    QList<PortGraphicsObject *> ports = getPortGraphicsObjectList();
    // 当前未连接的端口列表
    QList<PortGraphicsObject *> readyCheckPorts;
    for each (auto port in ports) {
        if (port->getLinkedConnectorWireList().isEmpty()) {
            readyCheckPorts.push_back(port);
        }
    }
    return readyCheckPorts;
}

TransformProxyOutlineGraphicsObject *TransformProxyGraphicsObject::getOutlineGraphicsObject()
{
    return dataPtr->outlineGraphics;
}

void TransformProxyGraphicsObject::clearAlignLine()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    dataPtr->alignLine.clear();
    for each (auto item in dataPtr->alignLineItems) {
        canvasScene->removeItem(item);
    }
    dataPtr->alignLineItems.clear();
}

void TransformProxyGraphicsObject::createConnectionPrompt()
{

    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
    if (!defaultScene) {
        return;
    }
    auto autoConnectionPrompt = defaultScene->getAutoConnectionPromptGraphics();
    if (nullptr == autoConnectionPrompt) {
        autoConnectionPrompt = defaultScene->createAutoConnectionPromptGraphics();
    }

    if (dataPtr->readyCheckPorts.isEmpty()) {
        dataPtr->readyCheckPorts = getReadyCheckPorts();
    }

    if (!dataPtr->readyCheckPorts.isEmpty() && autoConnectionPrompt) {
        autoConnectionPrompt->clean();
        if (dataPtr->readyMatchPorts.isEmpty()) {
            dataPtr->readyMatchPorts = getBeCheckedAutoMatchPorts();
        }
        // 如果端口数量过多，实时匹配会卡顿
        if (dataPtr->readyCheckPorts.size() > 150 || dataPtr->readyMatchPorts.size() > 150) {
            return;
        }
        if (!dataPtr->readyMatchPorts.isEmpty()) {
            for each (auto port in dataPtr->readyCheckPorts) {
                int maxDistance = 0xffffff;
                PortGraphicsObject *matchPort = nullptr;
                QPoint portLinkPos = port->getLinkPos().toPoint();
                for each (auto otherPort in dataPtr->readyMatchPorts) {
                    if (!port || !otherPort) {
                        continue;
                    }
                    if (port->portIsMatchAutoConnect(otherPort)) {
                        int distance = Utility::lineDistance(portLinkPos, otherPort->getPropmtPos().toPoint());
                        if (distance < maxDistance) {
                            matchPort = otherPort;
                            maxDistance = distance;
                        }
                    }
                }
                if (matchPort) {
                    QPoint matchLinkPos = matchPort->getPropmtPos().toPoint();
                    QPainterPath path = defaultScene->getTransformProxyGraphicsPainterPath();
                    bool isCollision = Utility::checkCollision(portLinkPos, matchLinkPos, path);
                    if (!isCollision) {
                        autoConnectionPrompt->addPortPair(port, matchPort);
                    }
                }
            }
        }
    }
}

void TransformProxyGraphicsObject::emitDeleteSource()
{
    if (!dataPtr->sourceProxy) {
        return;
    }
    emit deleteSource(dataPtr->sourceProxy->uuid());
}

void TransformProxyGraphicsObject::onSourceBadgeMsgChanged(const QString &msg)
{
    if (dataPtr->badgeGraphics) {
        dataPtr->badgeGraphics->setMsgTip(msg);
    }
}

void TransformProxyGraphicsObject::adjustSceneRect()
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
    if (!defaultScene)
        return;
    QRectF transRect = mapToScene(unitedChildsBoundingRect()).boundingRect();
    QRectF sceneRect = defaultScene->sceneRect();
    if (transRect.left() < sceneRect.left()) {
        sceneRect.setLeft(transRect.left());
        defaultScene->setSceneRect(sceneRect);
    } else if (transRect.right() > sceneRect.right()) {
        sceneRect.setRight(transRect.right());
        defaultScene->setSceneRect(sceneRect);
    } else if (transRect.top() < sceneRect.top()) {
        sceneRect.setTop(transRect.top());
        defaultScene->setSceneRect(sceneRect);
    } else if (transRect.bottom() > sceneRect.bottom()) {
        sceneRect.setBottom(transRect.bottom());
        defaultScene->setSceneRect(sceneRect);
    } else {
        QRectF rect = defaultScene->getAllItemUnitedRect();
        if (rect.left() > 0) {
            rect.setLeft(0);
        }
        if (rect.top() > 0) {
            rect.setTop(0);
        }
        if (rect.right() < 2239) {
            rect.setRight(2239);
        }
        if (rect.bottom() < 1583) {
            rect.setBottom(1583);
        }
        defaultScene->setSceneRect(rect);
    }
}

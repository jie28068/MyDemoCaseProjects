#include "CanvasContext.h"
#include "BuriedData.h"
#include "CanvasContextPrivate.h"
#include "CanvasViewManager.h"
#include "ConnectorWireContext.h"
#include "PortContext.h"
#include "SourceProxy.h"

#include "ActionManager.h"
#include "CanvasContext.pb.h"
#include "ConnectorWireContext.pb.h"
#include "PortContext.pb.h"
#include "SourceProxy.pb.h"
#include "Utility.h"
#include "graphicsmodelingkernel.h"

#include <QDebug>
#include <QFile>
#include <QTextCodec>

CanvasContext::CanvasContext(QString uuid, QObject *parent) : QObject(parent)
{
    dataPtr.reset(new CanvasContextPrivate());
    dataPtr->uuid = uuid;
    connect(this, &CanvasContext::canvasInteractionModeChanged, &ActionManager::getInstance(),
            &ActionManager::onCanvasInteractionModeChanged);
}

CanvasContext::~CanvasContext() { }

CanvasContext::CanvasContext(const CanvasContext &other)
{
    dataPtr.reset(new CanvasContextPrivate(*other.dataPtr.data()));
}

CanvasContext &CanvasContext::operator=(const CanvasContext &other)
{
    if (this == &other) {
        return *this;
    }

    *dataPtr = *(other.dataPtr.data());

    return *this;
}

QString CanvasContext::name() const
{
    return dataPtr->name;
}

void CanvasContext::setName(QString value)
{
    if (dataPtr->name == value)
        return;
    dataPtr->name = value;

    emit canvasContextChanged(GKD::CANVAS_NAME, value);
}

QString CanvasContext::uuid() const
{
    return dataPtr->uuid;
}

void CanvasContext::setUUID(QString uuid)
{
    dataPtr->uuid = uuid;
}

QSize CanvasContext::size() const
{
    return dataPtr->size;
}

void CanvasContext::setSize(QSize size)
{
    if (dataPtr->size == size)
        return;
    dataPtr->size = size;

    emit canvasContextChanged(GKD::CANVAS_SIZE, size);
}

QBoxLayout::Direction CanvasContext::direction() const
{
    return dataPtr->direction;
}

void CanvasContext::setDirection(QBoxLayout::Direction value)
{
    if (dataPtr->direction == value)
        return;
    dataPtr->direction = value;

    emit canvasContextChanged(GKD::CANVAS_DIRECTION, value);
}

QColor CanvasContext::backgroundColor() const
{
    return dataPtr->backgroundColor;
}

void CanvasContext::setBackgroundColor(QColor color)
{
    if (dataPtr->backgroundColor == color)
        return;
    dataPtr->backgroundColor = color;

    emit canvasContextChanged(GKD::CANVAS_BACKGROUND_COLOR, color);
}

qreal CanvasContext::scale() const
{
    return dataPtr->scale;
}

void CanvasContext::setScale(qreal scale)
{
    if (scale > 5.0) {
        scale = 5.0;
    } else if (scale < 0.2) {
        scale = 0.2;
    }
    if (dataPtr->scale == scale)
        return;

    dataPtr->scale = scale;

    emit canvasContextChanged(GKD::CANVAS_SCALE, scale);
}

CanvasContext::Type CanvasContext::type() const
{
    return dataPtr->type;
}

void CanvasContext::setType(CanvasContext::Type type)
{
    if (dataPtr->type == type)
        return;

    dataPtr->type = type;
}

QString CanvasContext::description() const
{
    return dataPtr->description;
}

void CanvasContext::setDescription(QString value)
{
    if (dataPtr->description == value)
        return;
    dataPtr->description = value;

    emit canvasContextChanged(GKD::CANVAS_DESCRIPTION, value);
}

QString CanvasContext::author() const
{
    return dataPtr->author;
}

void CanvasContext::setAuthor(QString value)
{
    if (dataPtr->author == value)
        return;
    dataPtr->author = value;

    emit canvasContextChanged(GKD::CANVAS_AUTHOR, value);
}

QPointF CanvasContext::centerPos() const
{
    return dataPtr->centerPoint;
}

void CanvasContext::setCenterPos(QPointF pos)
{
    if (dataPtr->centerPoint == pos)
        return;
    dataPtr->centerPoint = pos;

    emit canvasContextChanged(GKD::CANVAS_CENTER_POS, pos);
}

QString CanvasContext::version() const
{
    return dataPtr->version;
}

void CanvasContext::setVersion(QString version)
{
    if (dataPtr->version == version)
        return;
    dataPtr->version = version;

    emit canvasContextChanged(GKD::CANVAS_VERSION, version);
}

int CanvasContext::gridSpace() const
{
    return dataPtr->gridSpace <= 0 ? 10 : dataPtr->gridSpace;
}

void CanvasContext::setGridSpace(int value)
{
    if (dataPtr->gridSpace == value)
        return;
    dataPtr->gridSpace = value;

    emit canvasContextChanged(GKD::CANVAS_GRID_SPACE, value);
}

CanvasContext::GridFlag CanvasContext::gridFlag() const
{
    return dataPtr->gridFlag;
}

void CanvasContext::setGridFlag(CanvasContext::GridFlag flag)
{
    if (dataPtr->gridFlag == flag)
        return;
    int oldGridFlag = dataPtr->gridFlag;
    dataPtr->gridFlag = flag;
    emit gridFlagChanged(oldGridFlag, flag);
}

CanvasContext::LayerFlag CanvasContext::layerFlag() const
{
    return dataPtr->layerFlag;
}

void CanvasContext::setLayerFlag(CanvasContext::LayerFlag flag)
{
    if (dataPtr->layerFlag == flag)
        return;
    dataPtr->layerFlag = flag;

    emit canvasContextChanged(GKD::CANVAS_LAYER_FLAG, flag);
}

QString CanvasContext::connectorWireAlgorithmName() const
{
    return dataPtr->connectorWireAlgorithmName;
}

void CanvasContext::setConnectorWireAlgorithmName(QString algorithmName)
{
    if (dataPtr->connectorWireAlgorithmName == algorithmName)
        return;
    dataPtr->connectorWireAlgorithmName = algorithmName;

    emit canvasContextChanged(GKD::CANVAS_WIRE_ALG, algorithmName);
}

bool CanvasContext::showArrow() const
{
    return dataPtr->showArrow;
}

void CanvasContext::setShowArrow(bool value)
{
    if (dataPtr->showArrow == value)
        return;
    dataPtr->showArrow = value;
}

bool CanvasContext::showConnectorWireSwitch() const
{
    return dataPtr->showConnectorWireSwitch;
}

void CanvasContext::setShowConnectorWireSwitch(bool value)
{
    if (dataPtr->showConnectorWireSwitch == value)
        return;
    dataPtr->showConnectorWireSwitch = value;
}

void CanvasContext::addSourceProxy(QSharedPointer<SourceProxy> source)
{
    if (source.isNull()) {
        return;
    }
    // 修复 https://zt.xtkfpt.online/bug-view-10895.html
    // 因为子系统模块的canvas uuid和子系统模块的uuid一致，在撤销重做时，如果保持子系统的uuid不变
    // 那么如果子系统被双击打开过，就会存在与缓存中，当操作新创建的画布时，会导致实际操作的是缓存的画布对象，和实际操作的不一致
    if (dataPtr->businessHooksServer) {
        QString tempID = dataPtr->businessHooksServer->isContainCanvasContext(source);
        if (!tempID.isEmpty()) {
            CanvasViewManager::getInstance().removeCanvaView(tempID);
        }
    }

    ICanvasView *canvasView = CanvasViewManager::getInstance().getCanvasView(dataPtr->uuid);
    if (canvasView) {
        auto canvasScene = canvasView->getCanvasScene();
        if (canvasScene) {
            auto proxyGraphics = canvasScene->getTransformProxyGraphicsByID(source->uuid());
            if (!proxyGraphics) {
                canvasView->addSource(source, true);

                // 数据埋点，统计模块使用次数
                BuriedData &data = BuriedData::getInstance();
                QString typeString = "Control";
                if (dataPtr->type == kElectricalType || dataPtr->type == kElecUserDefinedType) {
                    typeString = "Electrical";
                }
                data.increaseSourceUsage(typeString.toStdString(), source->prototypeName().toStdString());
                return;
            }
        }
    }

    QString sourceUUID = source->uuid();
    if (dataPtr->sourceMap.contains(source->uuid())) {
        return;
    }
    dataPtr->sourceMap.insert(sourceUUID, source);

    source->attachToCanvasContext(this);

    addInputAndOutputSource(source);

    bool isModelAlreadyExist = false;
    if (dataPtr->businessHooksServer) {
        isModelAlreadyExist = dataPtr->businessHooksServer->isModelDataAlreadyExist(source);
    }

    if (!isModelAlreadyExist) {
        emit canvasAddSourceProxy(source);
        source->setSourceCreateType(SourceProxy::kCreateNormal);
    }
}

bool CanvasContext::deleteSourceProxy(QString uuid)
{
    PSourceProxy sourceProxy = dataPtr->sourceMap.value(uuid, PSourceProxy());
    if (!sourceProxy) {
        return true;
    }

    // 修复 https://zt.xtkfpt.online/bug-view-10895.html
    // 因为子系统模块的canvas uuid和子系统模块的uuid一致，在撤销重做时，如果保持子系统的uuid不变
    // 那么如果子系统被双击打开过，就会存在与缓存中，当操作新创建的画布时，会导致实际操作的是缓存的画布对象，和实际操作的不一致
    // 删除子系统的时候，需要把对应的画板和actionManager关联取消
    if (dataPtr->businessHooksServer) {
        QString tempID = dataPtr->businessHooksServer->isContainCanvasContext(sourceProxy);
        if (!tempID.isEmpty()) {
            ActionManager::getInstance().detachCanvasView(tempID);
        }
    }

    ICanvasView *canvasView = CanvasViewManager::getInstance().getCanvasView(dataPtr->uuid);
    if (canvasView) {
        auto canvasScene = canvasView->getCanvasScene();
        if (canvasScene) {
            auto proxyGraphics = canvasScene->getTransformProxyGraphicsByID(uuid);
            if (proxyGraphics) {
                canvasView->deleteSource(uuid);
                return true;
            }
        }
    }

    sourceProxy->attachToCanvasContext(nullptr);
    bool ret = (dataPtr->sourceMap.remove(uuid) != 0);

    deleteInputAndOutputSource(sourceProxy);

    emit canvasDeleteSourceProxy(sourceProxy);

    return ret;
}

QMap<QString, QSharedPointer<SourceProxy>> CanvasContext::getAllSource()
{
    return dataPtr->sourceMap;
}

QSharedPointer<SourceProxy> CanvasContext::getSource(QString uuid)
{
    return dataPtr->sourceMap.value(uuid, QSharedPointer<SourceProxy>());
}

void CanvasContext::addConnectWireContext(PConnectorWireContext ctx)
{
    if (ctx.isNull()) {
        return;
    }
    if (dataPtr->connectorWireMap.contains(ctx->uuid())) {
        return;
    }

    ICanvasView *canvasView = CanvasViewManager::getInstance().getCanvasView(dataPtr->uuid);
    if (canvasView) {
        auto canvasScene = canvasView->getCanvasScene();
        if (canvasScene) {
            auto wire = canvasScene->getConnectorWireGraphicsByID(ctx->uuid());
            if (!wire) {
                canvasView->addWire(ctx);
                dataPtr->connectorWireMap[ctx->uuid()] = ctx;
                return;
            }
        }
    }

    dataPtr->connectorWireMap[ctx->uuid()] = ctx;

    emit canvasAddConnectorWire(ctx);
}

bool CanvasContext::deleteConnectWireContext(QString uuid)
{
    ICanvasView *canvasView = CanvasViewManager::getInstance().getCanvasView(dataPtr->uuid);
    if (canvasView) {
        auto canvasScene = canvasView->getCanvasScene();
        if (canvasScene) {
            auto wire = canvasScene->getConnectorWireGraphicsByID(uuid);
            if (wire) {
                canvasView->deleteWire(uuid);
                return true;
            }
        }
    }

    PConnectorWireContext ctx = dataPtr->connectorWireMap.value(uuid, QSharedPointer<ConnectorWireContext>());
    if (!ctx.isNull()) {
        dataPtr->connectorWireMap.remove(uuid);
        emit canvasDeleteConnectorWire(ctx);
        return true;
    } else {
        return false;
    }
}

QMap<QString, QSharedPointer<ConnectorWireContext>> CanvasContext::getAllConnectorWireContext()
{
    return dataPtr->connectorWireMap;
}

PConnectorWireContext CanvasContext::getConnectorWireContext(QString uuid)
{
    return dataPtr->connectorWireMap.value(uuid, QSharedPointer<ConnectorWireContext>());
}

void CanvasContext::setBusinessHooksServer(BusinessHooksServer *server)
{
    if (nullptr == server) {
        return;
    }
    dataPtr->businessHooksServer = server;
}

BusinessHooksServer *CanvasContext::getBusinessHooksServer()
{
    return dataPtr->businessHooksServer;
}

bool CanvasContext::showConnectorWireBranchPoint()
{
    return dataPtr->showConnectorWireBranchPoint;
}

void CanvasContext::enableConnectorWireBranchPoint(bool enable)
{
    dataPtr->showConnectorWireBranchPoint = enable;
}

bool CanvasContext::getAllowOverlap()
{
    return dataPtr->allowOverlap;
}

void CanvasContext::setAllowOverlap(bool enable)
{
    dataPtr->allowOverlap = enable;
}

bool CanvasContext::highlightHomologousConnectorWire()
{
    return dataPtr->highlightHomologousConnectorWire;
}

void CanvasContext::setHighlightHomologousConnectorWire(bool show)
{
    dataPtr->highlightHomologousConnectorWire = show;
}

bool CanvasContext::getShowNavigationPreview()
{
    return dataPtr->showNavigationPreview;
}

void CanvasContext::setShowNavigationPreview(bool value)
{
    dataPtr->showNavigationPreview = value;
    emit canvasNavigationPreviewVisible(dataPtr->showNavigationPreview);
}

bool CanvasContext::getMagnify()
{
    return dataPtr->magnify;
}

void CanvasContext::setMagnify(bool value)
{
    if (dataPtr->magnify == value)
        return;
    dataPtr->magnify = value;
}

bool CanvasContext::getScreenDrag()
{
    return dataPtr->screenDrag;
}

void CanvasContext::setScreenDrag(bool value)
{
    if (dataPtr->screenDrag == value)
        return;
    dataPtr->screenDrag = value;
}

bool CanvasContext::getLocked()
{
    return dataPtr->locked;
}
void CanvasContext::setLocked(bool value)
{
    if (dataPtr->locked == value)
        return;
    dataPtr->locked = value;
    setStatusFlag(kLockedStatus, value);
}

QString CanvasContext::getLastModifyTime()
{
    return dataPtr->lastModifyTime;
}

void CanvasContext::setLastModifyTime(QString strTime)
{
    if (dataPtr->lastModifyTime == strTime) {
        return;
    }
    dataPtr->lastModifyTime = strTime;

    emit canvasContextChanged(GKD::CANVAS_LastModify_Time, strTime);
}

int CanvasContext::lineStyle()
{
    return dataPtr->lineStyle != 0 ? dataPtr->lineStyle : 1;
}
void CanvasContext::setLineStyle(int value)
{
    if (dataPtr->lineStyle == value) {
        return;
    }
    dataPtr->lineStyle = value;
    emit refreshCanvas();
}

QColor CanvasContext::lineColor()
{
    return dataPtr->lineColor;
}
void CanvasContext::setLineColor(QColor color)
{
    if (dataPtr->lineColor == color) {
        return;
    }
    dataPtr->lineColor = color;
    emit refreshCanvas();
}

QColor CanvasContext::lineSelectColor()
{
    return dataPtr->lineSelectColor;
}
void CanvasContext::setLineSelectColor(QColor color)
{
    if (dataPtr->lineSelectColor == color) {
        return;
    }
    dataPtr->lineSelectColor = color;
    emit refreshCanvas();
}

bool CanvasContext::isShowPowerFlowData()
{
    return dataPtr->isShowPowerFlowData;
}

void CanvasContext::setIsShowPowerFlowData(bool value)
{
    if (value == true || value != dataPtr->isShowPowerFlowData) {
        dataPtr->isShowPowerFlowData = value;
        emit canvasShowPowerFlowFlagChanged(value);
    }
}

bool CanvasContext::isShowAlignLine()
{
    return dataPtr->isShowAlignLine;
}

void CanvasContext::setIsShowAlignLine(bool value)
{
    if (value != dataPtr->isShowAlignLine) {
        dataPtr->isShowAlignLine = value;
        // emit canvasShowPowerFlowFlagChanged(value);
    }
}

void CanvasContext::setInteractionMode(InteractionMode mode)
{
    if (dataPtr->interactionMode != mode) {
        int oldMode = dataPtr->interactionMode;
        dataPtr->interactionMode = mode;
        emit canvasInteractionModeChanged(dataPtr->uuid, mode, oldMode);
    }
}

InteractionMode CanvasContext::getInteractionMode()
{
    return dataPtr->interactionMode;
}

void CanvasContext::canvasGroupPositionChanged()
{
    emit canvasGroupPositionChange();
}

CanvasContext::StatusFlags CanvasContext::getStatusFlags()
{
    return dataPtr->statusFlag;
}

bool CanvasContext::canModify()
{
    bool isLocked = dataPtr->statusFlag.testFlag(kLockedStatus);
    bool isProjectActivate = dataPtr->statusFlag.testFlag(kProjectActivate);
    bool isSimulationRunning = dataPtr->statusFlag.testFlag(kSimulationRunningStatus);
    bool isNextConstructiveBorad = dataPtr->statusFlag.testFlag(CanvasContext::kNextConstructiveBorad);
    return !isLocked && isProjectActivate && !isSimulationRunning && !isNextConstructiveBorad;
}

QMap<int, QSharedPointer<SourceProxy>> CanvasContext::getInputSourceMap()
{
    return dataPtr->inSourceMap;
}

QMap<int, QSharedPointer<SourceProxy>> CanvasContext::getOutputSourceMap()
{
    return dataPtr->outSourceMap;
}

CanvasProperty &CanvasContext::getCanvasProperty()
{
    return dataPtr->canvasProperty;
}

void CanvasContext::renameSourceUUID(QString oldUUID, QString newUUID)
{
    if (oldUUID == newUUID || newUUID.isEmpty() || oldUUID.isEmpty()) {
        return;
    }
    auto sourceProxy = dataPtr->sourceMap.value(oldUUID, nullptr);
    if (sourceProxy) {
        sourceProxy->setUUID(newUUID);
        dataPtr->sourceMap[newUUID] = sourceProxy;
        dataPtr->sourceMap.remove(oldUUID);
    }
}

bool CanvasContext::isShowModelRunningSort()
{
    return dataPtr->showModelRunningSort;
}

void CanvasContext::enableShowModelRunningSort(bool enable)
{
    if (dataPtr->showModelRunningSort == enable) {
        return;
    }
    dataPtr->showModelRunningSort = enable;
    emit showModelRunningSortChanged(enable);
}

void CanvasContext::setStatusFlag(StatusFlag flag, bool on)
{
    bool testFlag = dataPtr->statusFlag.testFlag(flag);
    if (testFlag != on || flag == kActiveWindow) {
        auto flags = dataPtr->statusFlag.setFlag(flag, on);
        dataPtr->statusFlag = flags;
        emit statusFlagsChanged(flag);
    }
}

void CanvasContext::addInputAndOutputSource(PSourceProxy source)
{
    if (!source)
        return;
    SourceProperty &sourceProperty = source->getSourceProperty();
    int inputOutputIndex = sourceProperty.getInputOutputIndex();
    if (source->prototypeName() == "In") {
        if (inputOutputIndex == -1) {
            int index = dataPtr->extraInfoMap[GKD::CANVAS_INPUT_INDEX].toInt();
            sourceProperty.setInputOutputIndex(index);
            sourceProperty.setInputOutputDisplayIndex(index);
            dataPtr->extraInfoMap[GKD::CANVAS_INPUT_INDEX] = index + 1;
        }
        dataPtr->inSourceMap[sourceProperty.getInputOutputIndex()] = source;
    } else if (source->prototypeName() == "Out") {
        if (inputOutputIndex == -1) {
            int index = dataPtr->extraInfoMap[GKD::CANVAS_OUTPUT_INDEX].toInt();
            sourceProperty.setInputOutputIndex(index);
            sourceProperty.setInputOutputDisplayIndex(index);
            dataPtr->extraInfoMap[GKD::CANVAS_OUTPUT_INDEX] = index + 1;
        }
        dataPtr->outSourceMap[sourceProperty.getInputOutputIndex()] = source;
    }
    emit canvasContextChanged(GKD::CANVAS_InOutIndex_Change, inputOutputIndex);
}

void CanvasContext::deleteInputAndOutputSource(PSourceProxy source)
{
    if (!source)
        return;
    SourceProperty &sourceProperty = source->getSourceProperty();
    if (source->prototypeName() == "In") {
        dataPtr->inSourceMap.remove(sourceProperty.getInputOutputIndex());
    } else if (source->prototypeName() == "Out") {
        dataPtr->outSourceMap.remove(sourceProperty.getInputOutputIndex());
    }
    emit canvasContextChanged(GKD::CANVAS_InOutIndex_Change, QVariant());
}

QSharedPointer<QByteArray> CanvasContext::serialize()
{
    QDateTime start_time = QDateTime::currentDateTime();
    CanvasContextSerialize *serialize = new CanvasContextSerialize();

    serialize->set_name(dataPtr->name.toUtf8().data());
    serialize->set_uuid(dataPtr->uuid.toUtf8().data());
    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->size;
        serialize->set_size(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->senceRectf;
        serialize->set_scene_rectf(buffer.data(), buffer.length());
    }

    serialize->set_direction(dataPtr->direction);

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->backgroundColor;
        serialize->set_background_color(buffer.data(), buffer.length());
    }

    serialize->set_scale(dataPtr->scale);
    serialize->set_type(dataPtr->type);
    serialize->set_description(dataPtr->description.toUtf8().data());

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->centerPoint;
        serialize->set_center_pos(buffer.data(), buffer.length());
    }

    serialize->set_version(dataPtr->version.toUtf8().data());
    serialize->set_grid_space(dataPtr->gridSpace);
    serialize->set_grid_flag(dataPtr->gridFlag);
    serialize->set_layer_flag(dataPtr->layerFlag);

    auto sourceMap = serialize->mutable_source_map();
    QMapIterator<QString, PSourceProxy> sourceMapIter(dataPtr->sourceMap);
    while (sourceMapIter.hasNext()) {
        sourceMapIter.next();
        QString key = sourceMapIter.key();
        PSourceProxy source = sourceMapIter.value();
        SourceProxySerialize sourceSerialize;
        source->serialize(&sourceSerialize);
        (*sourceMap)[key.toUtf8().data()] = sourceSerialize;
    }

    auto connectorWireMap = serialize->mutable_connector_wire_map();
    QMapIterator<QString, PConnectorWireContext> connectorMapIter(dataPtr->connectorWireMap);
    while (connectorMapIter.hasNext()) {
        connectorMapIter.next();
        QString key = connectorMapIter.key();
        PConnectorWireContext connectorCtx = connectorMapIter.value();
        ConnectorWireContextSerialize connectorSerialize;
        connectorCtx->serialize(&connectorSerialize);
        (*connectorWireMap)[key.toUtf8().data()] = connectorSerialize;
    }

    serialize->port_list();

    serialize->set_connector_wire_algorithm_name(dataPtr->connectorWireAlgorithmName.toUtf8().data());
    serialize->set_show_arrow(dataPtr->showArrow);
    serialize->set_show_connector_wire_switch(dataPtr->showConnectorWireSwitch);

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->extraInfoMap;
        serialize->set_extre_info_map(buffer.data(), buffer.length());
    }

    serialize->set_show_connector_wire_branch_point(dataPtr->showConnectorWireBranchPoint);
    serialize->set_allow_overlap(dataPtr->allowOverlap);
    serialize->set_highlight_homologous_connectorwire(dataPtr->highlightHomologousConnectorWire);

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->layerPropertyMap;
        serialize->set_layer_property_map(buffer.data(), buffer.length());
    }

    serialize->set_show_navigation_preview(dataPtr->showNavigationPreview);
    serialize->set_locked(dataPtr->locked);
    serialize->set_author(dataPtr->author.toUtf8().data());
    serialize->set_last_modify_time(dataPtr->lastModifyTime.toUtf8().data());
    serialize->set_linestyle(dataPtr->lineStyle);

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->lineColor;
        serialize->set_line_color(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->lineSelectColor;
        serialize->set_line_select_color(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        // 序列化时不保存高亮图元列表
        dataPtr->canvasProperty.setHighLightSourcceList(QStringList());
        dataPtr->canvasProperty.setGotoFromHighLightSourcceList(QStringList());
        outStream << dataPtr->canvasProperty.getProperties();
        serialize->set_canvas_property(buffer.data(), buffer.length());
    }

    serialize->set_show_model_running_sort(dataPtr->showModelRunningSort);

    serialize->set_subsystem_initialized(dataPtr->subsystemInitialized);

    serialize->set_is_support_suspended_connector_wire(dataPtr->isSupportSuspendedConnectorWire);

    int byteSize = serialize->ByteSize();

    QSharedPointer<QByteArray> serializeBuffer = QSharedPointer<QByteArray>(new QByteArray(byteSize, 0));

    serialize->SerializeToArray(serializeBuffer->data(), byteSize);

    delete serialize;
    return serializeBuffer;
}

void CanvasContext::unSerialize(QSharedPointer<QByteArray> serializeBuffer)
{
    QDateTime start_time = QDateTime::currentDateTime();
    CanvasContextSerialize *serialize = new CanvasContextSerialize();
    serialize->ParseFromArray(serializeBuffer->constData(), serializeBuffer->size());
    dataPtr->name = QString::fromUtf8(serialize->name().c_str());
    dataPtr->uuid = QString::fromUtf8(serialize->uuid().c_str());

    {
        QByteArray buffer(serialize->size().c_str(), serialize->size().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->size;
    }

    {
        QByteArray buffer(serialize->scene_rectf().c_str(), serialize->scene_rectf().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->senceRectf;
    }

    dataPtr->direction = (QBoxLayout::Direction)serialize->direction();

    if (dataPtr->senceRectf.isEmpty()) {
        int sceneWidth = 0;
        int sceneHeight = 0;
        if (dataPtr->direction == QBoxLayout::LeftToRight) {
            sceneWidth = dataPtr->size.width();
            sceneHeight = dataPtr->size.height();
        } else {
            sceneWidth = dataPtr->size.height();
            sceneHeight = dataPtr->size.width();
        }
        dataPtr->senceRectf = QRectF(0, 0, 3.77 * sceneWidth, 3.77 * sceneHeight);
    }

    {
        QByteArray buffer(serialize->background_color().c_str(), serialize->background_color().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->backgroundColor;
    }

    dataPtr->scale = serialize->scale();
    if (dataPtr->scale < 0.20) {
        dataPtr->scale = 1.0;
    }
    dataPtr->type = (CanvasContext::Type)serialize->type();
    dataPtr->description = QString::fromUtf8(serialize->description().c_str());

    {
        QByteArray buffer(serialize->center_pos().c_str(), serialize->center_pos().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->centerPoint;
    }

    dataPtr->version = QString::fromUtf8(serialize->version().c_str());
    dataPtr->gridSpace = serialize->grid_space();
    dataPtr->gridFlag = (CanvasContext::GridFlag)serialize->grid_flag();
    dataPtr->layerFlag = (CanvasContext::LayerFlag)serialize->layer_flag();

    for (auto iter = serialize->source_map().cbegin(); iter != serialize->source_map().cend(); ++iter) {
        PSourceProxy source = PSourceProxy(new SourceProxy());
        std::string key = iter->first;
        SourceProxySerialize sourceSerialize = iter->second;
        source->unSerialize(&sourceSerialize);
        dataPtr->sourceMap[QString(key.c_str())] = source;
    }

    for (auto iter = serialize->connector_wire_map().cbegin(); iter != serialize->connector_wire_map().cend(); ++iter) {
        PConnectorWireContext connectorCtx = PConnectorWireContext(new ConnectorWireContext(""));
        std::string key = iter->first;
        ConnectorWireContextSerialize connectorSerialize = iter->second;
        connectorCtx->unSerialize(&connectorSerialize);
        // add by liwenyu 2023.05.25
        // 连接线反序列化之后保存的srcSource和dstSource并不是上一步资源反序列化之后的实例，只是值相等而已，此处把连接线关联的对象和实际对象关联起来
        auto srcSource = connectorCtx->srcSource();
        if (srcSource) {
            auto realSrcSource = dataPtr->sourceMap.value(srcSource->uuid(), nullptr);
            if (realSrcSource) {
                connectorCtx->setSrcSource(realSrcSource);
            }
        }
        auto dstSource = connectorCtx->dstSource();
        if (dstSource) {
            auto realDstSource = dataPtr->sourceMap.value(dstSource->uuid(), nullptr);
            if (realDstSource) {
                connectorCtx->setDstSource(realDstSource);
            }
        }
        // end
        dataPtr->connectorWireMap[QString(key.c_str())] = connectorCtx;
    }

    dataPtr->connectorWireAlgorithmName = QString::fromUtf8(serialize->connector_wire_algorithm_name().c_str());
    dataPtr->showArrow = serialize->show_arrow();
    dataPtr->showConnectorWireSwitch = serialize->show_connector_wire_switch();

    {
        QByteArray buffer(serialize->extre_info_map().c_str(), serialize->extre_info_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->extraInfoMap;
    }

    for each (auto source in dataPtr->sourceMap) {
        addInputAndOutputSource(source);
    }

    dataPtr->showConnectorWireBranchPoint = serialize->show_connector_wire_branch_point();
    dataPtr->allowOverlap = serialize->allow_overlap();
    dataPtr->highlightHomologousConnectorWire = serialize->highlight_homologous_connectorwire();

    {
        QByteArray buffer(serialize->layer_property_map().c_str(), serialize->layer_property_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->layerPropertyMap;
    }

    dataPtr->showNavigationPreview = serialize->show_navigation_preview();
    dataPtr->locked = serialize->locked();
    dataPtr->author = QString::fromUtf8(serialize->author().c_str());
    dataPtr->lastModifyTime = QString::fromUtf8(serialize->last_modify_time().c_str());
    dataPtr->lineStyle = serialize->linestyle();

    {
        QByteArray buffer(serialize->line_color().c_str(), serialize->line_color().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->lineColor;
    }

    {
        QByteArray buffer(serialize->line_select_color().c_str(), serialize->line_select_color().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->lineSelectColor;
    }

    {
        QByteArray buffer(serialize->canvas_property().c_str(), serialize->canvas_property().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->canvasProperty.getProperties();
    }

    dataPtr->showModelRunningSort = serialize->show_model_running_sort();

    dataPtr->subsystemInitialized = serialize->subsystem_initialized();

    dataPtr->isSupportSuspendedConnectorWire = serialize->is_support_suspended_connector_wire();

    dataPtr->isShowAlignLine = dataPtr->type == kElectricalType ? false : true;

    dataPtr->statusFlag.setFlag(kLockedStatus, dataPtr->locked);

    delete serialize;
}

void CanvasContext::setSubSystemClickPos(QPointF pos)
{
    dataPtr->subsystemClickPos = pos;
}

QPointF CanvasContext::subSystemClickPos()
{
    return dataPtr->subsystemClickPos;
}

void CanvasContext::setSenceRectf(QRectF rect)
{
    dataPtr->senceRectf = rect;
}

QRectF CanvasContext::senceRectf()
{
    return dataPtr->senceRectf;
}

bool CanvasContext::isSubsystemInitialized()
{
    return dataPtr->subsystemInitialized;
}

void CanvasContext::setSubsystemInitialized(bool init)
{
    dataPtr->subsystemInitialized = init;
}

bool CanvasContext::isSupportSuspendedConnectorWire()
{
    return dataPtr->isSupportSuspendedConnectorWire;
}

void CanvasContext::setSupportSuspendedConnectorWire(bool value)
{
    dataPtr->isSupportSuspendedConnectorWire = value;
}

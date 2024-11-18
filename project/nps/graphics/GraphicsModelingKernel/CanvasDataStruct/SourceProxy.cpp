#include "SourceProxy.h"
#include "ConnectorWireContext.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "PortContext.h"
#include "SourceProxy.pb.h"
#include "SourceProxyPrivate.h"
#include <QFile>

SourceProxy::SourceProxy(QObject *parent) : QObject(parent)
{
    dataPtr.reset(new SourceProxyPrivate());
}

SourceProxy::~SourceProxy() { }

SourceProxy::SourceProxy(const SourceProxy &other)
{
    SourceProxyPrivate *otherDataPtr = other.dataPtr.data();
    dataPtr.reset(new SourceProxyPrivate(*otherDataPtr));
}

QString SourceProxy::canvasName() const
{
    return dataPtr->canvasName;
}

void SourceProxy::setCanvasName(QString name)
{
    if (dataPtr->canvasName == name)
        return;

    dataPtr->canvasName = name;

    emit sourceChange(GKD::SOURCE_CANVASNAME, QVariant(name));
}

QString SourceProxy::name() const
{
    return dataPtr->name;
}

void SourceProxy::setName(QString name)
{
    if (dataPtr->name == name)
        return;

    dataPtr->name = name;

    emit sourceChange(GKD::SOURCE_NAME, QVariant(name));
}

QString SourceProxy::uuid() const
{
    return dataPtr->uuid;
}

void SourceProxy::setUUID(QString id)
{
    if (dataPtr->uuid == id)
        return;

    dataPtr->uuid = id;
    // 需要同步更新端口信息中所属的资源id
    foreach (auto port, dataPtr->portList) {
        if (port) {
            port->setSourceUUID(id);
        }
    }
}

QString SourceProxy::originalUUID() const
{
    return dataPtr->originalUUID;
}

void SourceProxy::setOriginalUUID(QString id)
{
    if (dataPtr->originalUUID == id)
        return;

    dataPtr->originalUUID = id;
}

QString SourceProxy::prototypeName() const
{
    return dataPtr->prototypeName;
}

void SourceProxy::setPrototypeName(QString prototypeName)
{
    if (dataPtr->prototypeName == prototypeName)
        return;

    dataPtr->prototypeName = prototypeName;

    emit sourceChange(GKD::SOURCE_PROTOTYPENAME, QVariant(prototypeName));
}

QString SourceProxy::moduleType() const
{
    return dataPtr->moduleType;
}

void SourceProxy::setModuleType(QString moduleType)
{
    if (dataPtr->moduleType == moduleType)
        return;

    dataPtr->moduleType = moduleType;
}

QPointF SourceProxy::pos() const
{
    return dataPtr->pos;
}

void SourceProxy::setPos(QPointF pos)
{
    if (dataPtr->pos == pos)
        return;

    dataPtr->pos = pos;

    emit sourceChange(GKD::SOURCE_POS, QVariant(pos));
}

QSizeF SourceProxy::size() const
{
    return dataPtr->size;
}

void SourceProxy::setSize(QSizeF size)
{
    if (dataPtr->size == size)
        return;

    if (moduleType() == GKD::SOURCE_MODULETYPE_CONTROL) {
        // 只处理控制系统的
        QSizeF nowSize = getCombineTransform().mapRect(QRectF(QPointF(0, 0), dataPtr->size)).size();
        if (nowSize.width() > size.width() && nowSize.height() > size.height()) {
            return;
        }
    }

    dataPtr->size = size;
    // dataPtr->scaleTransform = QTransform(); // 重置缩放矩阵

    emit sourceChange(GKD::SOURCE_SIZE, QVariant(size));
}

QTransform SourceProxy::scaleTransform() const
{
    return dataPtr->scaleTransform;
}

void SourceProxy::setScaleTransform(QTransform transform)
{
    if (dataPtr->scaleTransform == transform)
        return;

    dataPtr->scaleTransform = transform;

    emit sourceChange(GKD::SOURCE_SCALE_TRANSFORM, QVariant(transform));
}

QTransform SourceProxy::rotateTransform() const
{
    return dataPtr->rotateTransform;
}

void SourceProxy::setRotateTransform(QTransform transform)
{
    if (dataPtr->rotateTransform == transform)
        return;

    dataPtr->rotateTransform = transform;

    emit sourceChange(GKD::SOURCE_ROTATE_TRANSFORM, QVariant(transform));
}

QTransform SourceProxy::translateTransform() const
{
    return dataPtr->translateTransform;
}

void SourceProxy::setTranslateTransform(QTransform transform)
{
    if (dataPtr->translateTransform == transform)
        return;

    dataPtr->translateTransform = transform;

    emit sourceChange(GKD::SOURCE_TRANSLATE_TRANSFORM, QVariant(transform));
}

QTransform SourceProxy::xAxisFlipTransform() const
{
    return dataPtr->xAxisflipTransform;
}

void SourceProxy::setxAxisFlipTransform(QTransform transform)
{
    if (dataPtr->xAxisflipTransform == transform) {
        return;
    }
    dataPtr->xAxisflipTransform = transform;
}

QTransform SourceProxy::yAxisFlipTransform() const
{
    return dataPtr->yAxisflipTransform;
}

void SourceProxy::setyAxisFlipTransform(QTransform transform)
{
    if (dataPtr->yAxisflipTransform == transform) {
        return;
    }
    dataPtr->yAxisflipTransform = transform;
}

QTransform SourceProxy::getCombineTransform()
{
    return dataPtr->scaleTransform * dataPtr->rotateTransform * dataPtr->xAxisflipTransform
            * dataPtr->yAxisflipTransform * dataPtr->translateTransform;
}

QString SourceProxy::state() const
{
    return dataPtr->state;
}

void SourceProxy::setState(QString state)
{
    if (dataPtr->state == state)
        return;

    dataPtr->state = state;

    emit sourceChange(GKD::SOURCE_STATE, QVariant(state));
}

QString SourceProxy::statetips() const
{
    return dataPtr->statetips;
}

void SourceProxy::setStatetips(QString statetips)
{
    if (dataPtr->statetips == statetips)
        return;

    dataPtr->statetips = statetips;

    emit sourceChange(GKD::SOURCE_STATETIPS, QVariant(statetips));
}

// QStringList SourceProxy::svgImageList() const
// {
//     return dataPtr->svgImageList;
// }

// void SourceProxy::setSvgImageList(QStringList svgImageList)
// {
//     if (dataPtr->svgImageList == svgImageList)
//         return;

//     dataPtr->svgImageList = svgImageList;

//     emit sourceChange(GKD::SOURCE_SVG_IMAGELIST, QVariant(svgImageList));
// }

QVariantMap SourceProxy::svgImageDatas() const
{
    return dataPtr->svgImageMap;
}

void SourceProxy::setSvgImageDatas(const QVariantMap &svgImageDatas)
{
    if (dataPtr->svgImageMap == svgImageDatas)
        return;

    dataPtr->svgImageMap = svgImageDatas;

    emit sourceChange(GKD::SOURCE_SVG_IMAGEMAP, svgImageDatas);
}

bool SourceProxy::isSupportAddOrDelPort() const
{
    return dataPtr->isSupportAddOrDelPort;
}

void SourceProxy::setIsSupportAddOrDelPort(bool isSupport)
{
    if (dataPtr->isSupportAddOrDelPort == isSupport)
        return;

    dataPtr->isSupportAddOrDelPort = isSupport;
}

bool SourceProxy::isHadUpperLimitation() const
{
    return dataPtr->isHadUpperLimitation;
}

void SourceProxy::setIsHadUpperLimitation(bool isUpper)
{
    if (dataPtr->isHadUpperLimitation == isUpper)
        return;

    dataPtr->isHadUpperLimitation = isUpper;
}

bool SourceProxy::isHadLowerLimitation() const
{
    return dataPtr->isHadLowerLimitation;
}

void SourceProxy::setIsHadLowerLimitation(bool isLower)
{
    if (dataPtr->isHadLowerLimitation == isLower)
        return;

    dataPtr->isHadLowerLimitation = isLower;
}

QVariantList SourceProxy::listNameDefaultAnchor() const
{
    return dataPtr->listNameDefaultAnchor;
}

void SourceProxy::setListNameDefaultAnchor(QVariantList listNameAnchor)
{
    if (dataPtr->listNameDefaultAnchor == listNameAnchor)
        return;

    dataPtr->listNameDefaultAnchor = listNameAnchor;
}

QMap<int, QPointF> SourceProxy::nameAngleOffset() const
{
    return dataPtr->nameAngleOffset;
}

void SourceProxy::setNameAngleOffset(QMap<int, QPointF> angleOffset)
{
    dataPtr->nameAngleOffset = angleOffset;
    emitOtherChange();
}

QColor SourceProxy::getStateColor()
{
    return dataPtr->stateColorMap.value(dataPtr->state, QColor(0, 0, 0));
}

QList<QSharedPointer<PortContext>> SourceProxy::portList()
{
    return dataPtr->portList;
}

void SourceProxy::resetPortList(QList<QSharedPointer<PortContext>> portContextList)
{
    if (signalsBlocked()) {
        return;
    }
    int nOldPortListSize = 0;
    int nNewPortListSize = 0;

    for each (QSharedPointer<PortContext> port in dataPtr->portList) {
        auto portType = port->type();
        if (portType == "input") {
            nOldPortListSize++;
        }
    }
    dataPtr->portList.clear();
    dataPtr->portList.append(portContextList);
    for each (QSharedPointer<PortContext> port in dataPtr->portList) {
        auto portType = port->type();
        if (portType == "input") {
            nNewPortListSize++;
        }
    }
    int sizeOffset = nNewPortListSize - nOldPortListSize;
    emit sigResetPortList(sizeOffset);
}

void SourceProxy::addPortContext(QSharedPointer<PortContext> portContext)
{
    if (portContext.isNull()) {
        return;
    }
    foreach (auto port, dataPtr->portList) {
        if (port && port->uuid() == portContext->uuid()) {
            // 已经存在该端口
            return;
        }
    }
    dataPtr->portList.append(portContext);
    dataPtr->dataPointsMap[portContext->uuid()] = QPointF(0, 0);
    emit sigAddPortContext(portContext);
}

void SourceProxy::delPortContext(QSharedPointer<PortContext> portContext)
{
    if (portContext.isNull()) {
        return;
    }
    int index = dataPtr->portList.indexOf(portContext);
    if (-1 != index) {
        dataPtr->portList.removeAt(index);
        dataPtr->dataPointsMap.remove(portContext->uuid());
        emit sigDelPortContext(portContext);
    }
}

QSharedPointer<PortContext> SourceProxy::getPortContextByID(QString uuid)
{
    for (int i = 0; i < dataPtr->portList.size(); i++) {
        if (dataPtr->portList.at(i)->uuid() == uuid)
            return dataPtr->portList.at(i);
    }

    return QSharedPointer<PortContext>();
}

QSharedPointer<PortContext> SourceProxy::getPortContextByName(QString name)
{
    for (auto port : dataPtr->portList) {
        if (port->name() == name) {
            return port;
        }
    }

    return QSharedPointer<PortContext>();
}

void SourceProxy::triggerMouseDoubleClick()
{
    if (dataPtr->canvasContext) {
        emit dataPtr->canvasContext->sourceProxyEdit(CanvasContext::kSourceDoubleClick, dataPtr->uuid);
    }
    emit mouseDoubleClick();
}

void SourceProxy::emitEditProperty()
{
    if (dataPtr->canvasContext) {
        emit dataPtr->canvasContext->sourceProxyEdit(CanvasContext::kSourceEdit, dataPtr->uuid);
    }
    emit editProperty();
}

void SourceProxy::emitOtherChange()
{
    emit sourceChange(GKD::SOURCE_OTHER, QVariant());
}

QByteArray readSvgData(const QString &svgPath)
{
    QFile file(svgPath);
    if (file.open(QFile::ReadOnly)) {
        QByteArray svgData = file.readAll();
        return svgData;
    }
    return QByteArray();
}

void SourceProxy::serialize(SourceProxySerialize *sourceSerialize)
{
    if (!sourceSerialize) {
        return;
    }
    sourceSerialize->set_cavas_name(dataPtr->canvasName.toUtf8().data());
    sourceSerialize->set_name(dataPtr->name.toUtf8().data());
    sourceSerialize->set_uuid(dataPtr->uuid.toUtf8().data());
    sourceSerialize->set_prototype_name(dataPtr->prototypeName.toUtf8().data());
    sourceSerialize->set_module_type(dataPtr->moduleType.toUtf8().data());

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->pos;
        sourceSerialize->set_pos(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->size;
        sourceSerialize->set_size(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->scaleTransform;
        sourceSerialize->set_scale_transform(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->rotateTransform;
        sourceSerialize->set_rotate_transform(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->translateTransform;
        sourceSerialize->set_translate_transform(buffer.data(), buffer.length());
    }

    sourceSerialize->set_state(dataPtr->state.toUtf8().data());
    sourceSerialize->set_state_tips(dataPtr->statetips.toUtf8().data());
    sourceSerialize->set_comment_state(dataPtr->commentState.toUtf8().data());

    auto sourceSerializePortList = sourceSerialize->mutable_portlist();
    for each (auto portPtr in dataPtr->portList) {
        PortContextSerialize portSerialize;
        portPtr->serialize(&portSerialize);
        *((*sourceSerializePortList).Add()) = portSerialize;
    }

    sourceSerialize->set_is_support_addordelete_port(dataPtr->isSupportAddOrDelPort);

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->listNameDefaultAnchor;
        sourceSerialize->set_list_name_default_anchor(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->nameAngleOffset;
        sourceSerialize->set_name_angle_offset(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->svgImageList;
        sourceSerialize->set_svg_image_list(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->svgImageMap;
        sourceSerialize->set_svg_image_map(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->extraInfoMap;
        sourceSerialize->set_extra_info_map(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->annotationMap;
        sourceSerialize->set_annotation_map(buffer.data(), buffer.length());
    }

    sourceSerialize->set_is_had_upper_limitation(dataPtr->isHadUpperLimitation);

    sourceSerialize->set_is_had_lower_limitation(dataPtr->isHadLowerLimitation);

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->dataPointsMap;
        sourceSerialize->set_data_points_map(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->xAxisflipTransform;
        sourceSerialize->set_xaxis_flip_transform(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->yAxisflipTransform;
        sourceSerialize->set_yaxis_flip_transform(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->sourceProperty.getProperties();
        sourceSerialize->set_source_property(buffer.data(), buffer.length());
    }

    sourceSerialize->set_prototype_name_chs(dataPtr->prototypeName_CHS.toUtf8().data());

    sourceSerialize->set_prototype_readable_name(dataPtr->prototypeReadableName.toUtf8().data());
}

void SourceProxy::unSerialize(SourceProxySerialize *sourceSerialize)
{
    if (!sourceSerialize) {
        return;
    }
    dataPtr->canvasName = QString::fromUtf8(sourceSerialize->cavas_name().c_str());
    dataPtr->name = QString::fromUtf8(sourceSerialize->name().c_str());
    dataPtr->uuid = QString::fromUtf8(sourceSerialize->uuid().c_str());
    dataPtr->prototypeName = QString::fromUtf8(sourceSerialize->prototype_name().c_str());
    dataPtr->moduleType = QString::fromUtf8(sourceSerialize->module_type().c_str());

    {
        QByteArray buffer(sourceSerialize->pos().c_str(), sourceSerialize->pos().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->pos;
    }

    {
        QByteArray buffer(sourceSerialize->size().c_str(), sourceSerialize->size().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->size;
    }

    // 兼容之前添加的in out模块
    if (dataPtr->prototypeName == "In" || dataPtr->prototypeName == "Out") {
        dataPtr->size = QSizeF(40, 20);
    }

    {
        QByteArray buffer(sourceSerialize->scale_transform().c_str(), sourceSerialize->scale_transform().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->scaleTransform;
    }

    {
        QByteArray buffer(sourceSerialize->rotate_transform().c_str(), sourceSerialize->rotate_transform().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->rotateTransform;
    }

    {
        QByteArray buffer(sourceSerialize->translate_transform().c_str(),
                          sourceSerialize->translate_transform().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->translateTransform;
    }

    dataPtr->state = QString::fromUtf8(sourceSerialize->state().c_str());
    dataPtr->statetips = QString::fromUtf8(sourceSerialize->state_tips().c_str());
    dataPtr->commentState = QString::fromUtf8(sourceSerialize->comment_state().c_str());
    if(dataPtr->commentState.isEmpty()){//暂时还没状态的设一个默认状态
        setCommentState("normal");
    }
    if(dataPtr->moduleType == GKD::SOURCE_MODULETYPE_CONTROL && dataPtr->state == "through"){//需要将state的直通状态转移过来
        setState("normal");
        setCommentState("through");
    }

    auto serializePortList = sourceSerialize->portlist();
    for (int i = 0; i < serializePortList.size(); i++) {
        PortContextSerialize portSerialize = serializePortList[i];
        PPortContext portContext = PPortContext(new PortContext("", ""));
        portContext->unSerialize(&portSerialize);
        dataPtr->portList.push_back(portContext);
    }
    // 重置老数据中PortContext里面sourceuuid存储不对的问题
    resetPortSourceUUid();

    dataPtr->isSupportAddOrDelPort = sourceSerialize->is_support_addordelete_port();

    {
        QByteArray buffer(sourceSerialize->list_name_default_anchor().c_str(),
                          sourceSerialize->list_name_default_anchor().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->listNameDefaultAnchor;
    }

    {
        QByteArray buffer(sourceSerialize->name_angle_offset().c_str(), sourceSerialize->name_angle_offset().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->nameAngleOffset;
    }

    {
        QByteArray buffer(sourceSerialize->svg_image_list().c_str(), sourceSerialize->svg_image_list().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->svgImageList;
    }

    // 电气画板图片保存位置更换后 需要适配老画板
    if (dataPtr->moduleType == GKD::SOURCE_MODULETYPE_ELECTRICAL) {
        for (int i = 0; i < dataPtr->svgImageList.size(); ++i) {
            if (dataPtr->svgImageList[i].contains("/PluginDataManagerServer/Resources/DrawingBoardIcon/Electrical/")) {
                dataPtr->svgImageList[i].replace("/PluginDataManagerServer/Resources/DrawingBoardIcon/Electrical/",
                                                 "/electricalblock/Resources/electricalblock/");
            }
            if (dataPtr->svgImageList[i].contains("Line2.svg")) {
                dataPtr->svgImageList[i].replace("Line2.svg", "Line.svg");
            }
        }
    }

    {
        QByteArray buffer(sourceSerialize->svg_image_map().c_str(), sourceSerialize->svg_image_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->svgImageMap;
    }

    // 从svgImageList读取数据存入svgImageMap (升级svg数据方式)
    for (int i = 0; i < dataPtr->svgImageList.size(); i++) {
        auto svgFilePath = dataPtr->svgImageList.at(i);
        QByteArray svgData = readSvgData(svgFilePath);
        switch (i) {
        case 0:
            dataPtr->svgImageMap.insert(Kcc::BlockDefinition::SVG_0_PIC, svgData);
            break;
        case 1:
            dataPtr->svgImageMap.insert(Kcc::BlockDefinition::SVG_90_PIC, svgData);
            break;
        case 2:
            dataPtr->svgImageMap.insert(Kcc::BlockDefinition::SVG_180_PIC, svgData);
            break;
        case 3:
            dataPtr->svgImageMap.insert(Kcc::BlockDefinition::SVG_270_PIC, svgData);
            break;
        default:
            break;
        }
    }

    {
        QByteArray buffer(sourceSerialize->extra_info_map().c_str(), sourceSerialize->extra_info_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->extraInfoMap;
    }

    {
        QByteArray buffer(sourceSerialize->annotation_map().c_str(), sourceSerialize->annotation_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->annotationMap;
    }

    dataPtr->isHadUpperLimitation = sourceSerialize->is_had_upper_limitation();

    dataPtr->isHadLowerLimitation = sourceSerialize->is_had_lower_limitation();

    {
        QByteArray buffer(sourceSerialize->data_points_map().c_str(), sourceSerialize->data_points_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->dataPointsMap;
    }

    {
        int length = sourceSerialize->xaxis_flip_transform().length();
        if (length > 0) {
            QByteArray buffer(sourceSerialize->xaxis_flip_transform().c_str(), length);
            QDataStream inStream(&buffer, QIODevice::ReadWrite);
            inStream >> dataPtr->xAxisflipTransform;
        } else {
            dataPtr->xAxisflipTransform = QTransform();
        }
    }

    {
        int length = sourceSerialize->yaxis_flip_transform().length();
        if (length > 0) {
            QByteArray buffer(sourceSerialize->yaxis_flip_transform().c_str(), length);
            QDataStream inStream(&buffer, QIODevice::ReadWrite);
            inStream >> dataPtr->yAxisflipTransform;
        } else {
            dataPtr->yAxisflipTransform = QTransform();
        }
    }

    {
        QByteArray buffer(sourceSerialize->source_property().c_str(), sourceSerialize->source_property().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->sourceProperty.getProperties();
    }

    // 常数模块展示的数据之前存在SOURCE_EXTRA_INFO_CONSTANT_PARAMETER字段中 需要兼容
    if (dataPtr->extraInfoMap.contains("SOURCE_DISPLAY_PARAMETER_DATA")
        && dataPtr->sourceProperty.getDisplayData().toString().isEmpty()) {
        dataPtr->sourceProperty.setDisplayData(dataPtr->extraInfoMap["SOURCE_DISPLAY_PARAMETER_DATA"]);
        dataPtr->extraInfoMap.remove("SOURCE_DISPLAY_PARAMETER_DATA");
    }

    dataPtr->prototypeName_CHS = QString::fromUtf8(sourceSerialize->prototype_name_chs().c_str());

    dataPtr->prototypeReadableName = QString::fromUtf8(sourceSerialize->prototype_readable_name().c_str());
}

void SourceProxy::attachToCanvasContext(CanvasContext *canvasContext)
{
    dataPtr->canvasContext = canvasContext;
}

QMap<QString, QVariant> &SourceProxy::getAnnotationMap()
{
    return dataPtr->annotationMap;
}

void SourceProxy::resetAnnotationMap(QMap<QString, QVariant> &value)
{
    dataPtr->annotationMap = value;
    emit sourceChange(GKD::SOURCE_ANNOTATION_INFO, value);
}

void SourceProxy::setAnnotationProperty(QString key, QVariant value)
{
    dataPtr->annotationMap.insert(key, value);
    emit sourceChange(GKD::SOURCE_ANNOTATION_INFO, value);
}

QMap<QString, QVariant> SourceProxy::getDataPointMap()
{
    return dataPtr->dataPointsMap;
}

void SourceProxy::setDataPointMap(QString key, QVariant value)
{
    dataPtr->dataPointsMap[key] = value;
    emit sourceChange(GKD::SOURCE_DATA_POINT, value);
}

void SourceProxy::reSetDataPointMap(QMap<QString, QVariant> &value)
{
    dataPtr->dataPointsMap = value;
}

void SourceProxy::setBadgeMsg(const QString &msg)
{
    if (dataPtr->badgeMsg == msg) {
        return;
    }
    dataPtr->badgeMsg = msg;
    emit badgeMsgChanged(msg);
}

QString SourceProxy::getBadgeMsg()
{
    return dataPtr->badgeMsg;
}

void SourceProxy::setRunVariableDataMap(QMap<QString, QMap<QString, QVariant>> &dataMap)
{
    dataPtr->runVariableDataMap = dataMap;
    emit runVariableDataChange(dataMap);
}

QTransform SourceProxy::getSRTTransform()
{
    return dataPtr->scaleTransform * dataPtr->rotateTransform * dataPtr->translateTransform;
}

void SourceProxy::resetPortSourceUUid()
{
    for each (auto port in portList()) {
        port->setSourceUUID(uuid());
    }
}

SourceProperty &SourceProxy::getSourceProperty()
{
    return dataPtr->sourceProperty;
}

void SourceProxy::setSourceCreateType(CreateType type)
{
    dataPtr->createType = type;
}

SourceProxy::CreateType SourceProxy::getSourceCreateType()
{
    return dataPtr->createType;
}

QString SourceProxy::debugMsg()
{
    QString msg;
    QString portMsg;
    auto portList = dataPtr->portList;
    foreach (auto port, dataPtr->portList) {
        if (port) {
            QString portInfo = QString("端口id：%1,类型%2,所属资源UUID:%3,原始端口id:%4")
                                       .arg(port->uuid())
                                       .arg(port->type())
                                       .arg(port->sourceUUID())
                                       .arg(port->originalPortUUID());
            portMsg += portInfo;
        }
    }

    msg = QString("资源原型:%1,资源uuid:%2,资源名称:%3,端口信息:%4")
                  .arg(dataPtr->prototypeName)
                  .arg(dataPtr->uuid)
                  .arg(dataPtr->name)
                  .arg(portMsg);

    return msg;
}

QString SourceProxy::prototypeName_CHS() const
{
    return dataPtr->prototypeName_CHS;
}

void SourceProxy::setPrototypeName_CHS(QString prototypeName)
{
    if (dataPtr->prototypeName_CHS == prototypeName)
        return;

    dataPtr->prototypeName_CHS = prototypeName;

    emit sourceChange(GKD::SOURCE_PROTOTYPENAME_CHS, QVariant(prototypeName));
}

void SourceProxy::setReadyDeleteFlag(bool isReadyDelete)
{
    dataPtr->isReadyDelete = isReadyDelete;
}

bool SourceProxy::getReadyDeleteFlag()
{
    return dataPtr->isReadyDelete;
}

QString SourceProxy::getPrototypeReadaleName()
{
    return dataPtr->prototypeReadableName;
}

void SourceProxy::setPrototypeReadableName(const QString &name)
{
    dataPtr->prototypeReadableName = name;
}

QString SourceProxy::getCommentState()
{
    return dataPtr->commentState;
}

void SourceProxy::setCommentState(const QString &state) 
{ 
    dataPtr->commentState = state;
    emit sourceChange(GKD::SOURCE_COMMENTSTATE, QVariant(state));
}

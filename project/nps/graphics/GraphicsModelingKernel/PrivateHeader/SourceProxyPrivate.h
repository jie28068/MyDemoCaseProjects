#pragma once

#include "CanvasContext.h"
#include "NPSPropertyManager.h"
#include "PortContext.h"
#include "Utility.h"

#include <QColor>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QSharedPointer>
#include <QSizeF>
#include <QString>
#include <QStringList>
#include <QTransform>

#define WarringColor "#fcb200"
#define ErrorColor "#ff0004"
#define DisableColor "#7d7d7d"

class PortContext;

class SourceProxyPrivate
{
public:
    SourceProxyPrivate();
    ~SourceProxyPrivate();

    SourceProxyPrivate(const SourceProxyPrivate &other);

    CanvasContext *canvasContext;

    /// @brief 所属的画板名称
    QString canvasName;
    /// @brief 名称
    QString name;
    /// @brief 唯一标识符
    QString uuid;
    /// @brief 原始UUID,资源可以被拷贝粘贴。该值保存复制的原资源id
    QString originalUUID;
    /// @brief 原型名称
    QString prototypeName;
    /// @brief 所属的模块类型，比如电气、控制。通过该类型获取图元工厂实例
    QString moduleType;
    /// @brief 画板中的坐标(场景坐标)
    QPointF pos;
    /// @brief 大小
    QSizeF size;
    /// @brief 缩放变换
    QTransform scaleTransform;
    /// @brief 旋转变换
    QTransform rotateTransform;
    /// @brief 平移变换
    QTransform translateTransform;
    /// @brief x轴翻转变换
    QTransform xAxisflipTransform;
    /// @brief y轴翻转变换
    QTransform yAxisflipTransform;
    /// @brief 端口信息列表
    QList<QSharedPointer<PortContext>> portList;
    /// @brief svg图片路径列表
    QStringList svgImageList;
    /// @brief svg图片数据
    QVariantMap svgImageMap;
    /// @brief 状态
    QString state;
    /// @brief 状态对应的颜色值
    QMap<QString, QColor> stateColorMap;
    /// @brief 状态对应的提示消息
    QString statetips;
    /// @brief 是否支持动态增加、删除端口
    bool isSupportAddOrDelPort;
    /// @brief 模块名称的默认位置
    QVariantList listNameDefaultAnchor;
    /// @brief 存模块名称每个方向的拖动位置
    QMap<int, QPointF> nameAngleOffset;
    /// @brief 模块的附加参数信息，方便后续拓展
    QMap<QString, QVariant> extraInfoMap;
    /// @brief 注解资源的相关参数
    QMap<QString, QVariant> annotationMap;
    /// @brief 是否有上限幅线
    bool isHadUpperLimitation;
    /// @brief 是否有下限幅线
    bool isHadLowerLimitation;
    /// @brief 展示潮流计算等数据的模块的位置
    QMap<QString, QVariant> dataPointsMap;
    /// @brief 画板运行时的模块仿真变量信息
    QMap<QString, QMap<QString, QVariant>> runVariableDataMap;
    /// @brief 资源属性
    SourceProperty sourceProperty;

    SourceProxy::CreateType createType;

    /// @brief 角标提示信息(目前用来显示模块的执行顺序)
    QString badgeMsg;

    /// @brief 模块简述,该字段用于鼠标悬浮时显示模块的信息
    QString prototypeName_CHS;

    /// @brief 资源是否会被删除标志（目前只用于是否清空busselector模块输出端口，不需要序列化处理）
    bool isReadyDelete;

    /// @brief 模块的可读原型名，显示于模型中间的原型名称(在添加该字段之前的画板使用prototypeName)
    QString prototypeReadableName;

    /// @brief 模块的直通禁用状态，与state分开
    QString commentState;
};

SourceProxyPrivate::SourceProxyPrivate()
    : pos(QPointF(0, 0)), size(QSize(40, 20)), scaleTransform(QTransform()), state("normal"), commentState("normal")
{
    canvasContext = nullptr;
    stateColorMap["normal"] = QColor(0, 0, 0);
    stateColorMap["warning"] = QColor(WarringColor);
    stateColorMap["error"] = QColor(ErrorColor);
    stateColorMap["disable"] = QColor(DisableColor);
    stateColorMap["hangup"] = QColor(DisableColor);

    isSupportAddOrDelPort = false;
    isHadUpperLimitation = false;
    isHadLowerLimitation = false;
    originalUUID = "";
    uuid = Utility::createUUID();

    createType = SourceProxy::kCreateNormal;
    isReadyDelete = false;

    // extraInfoMap[GKD::SOURCE_FROMANDGOTO_TAG_INFO] = QString();
}

SourceProxyPrivate::~SourceProxyPrivate() { }

SourceProxyPrivate::SourceProxyPrivate(const SourceProxyPrivate &other)
{
    canvasName = other.canvasName;
    name = other.name;
    uuid = other.uuid;
    prototypeName = other.prototypeName;
    moduleType = other.moduleType;
    pos = other.pos;
    size = other.size;
    scaleTransform = other.scaleTransform;
    rotateTransform = other.rotateTransform;
    translateTransform = other.translateTransform;
    xAxisflipTransform = other.xAxisflipTransform;
    yAxisflipTransform = other.yAxisflipTransform;
    QListIterator<QSharedPointer<PortContext>> portIter(other.portList);
    while (portIter.hasNext()) {
        PPortContext ctx = portIter.next();
        portList.append(PPortContext(new PortContext(*ctx.data())));
    }
    svgImageList = other.svgImageList;
    svgImageMap = other.svgImageMap;
    state = other.state;
    stateColorMap = other.stateColorMap;
    statetips = other.statetips;
    isSupportAddOrDelPort = other.isSupportAddOrDelPort;
    isHadUpperLimitation = other.isHadUpperLimitation;
    isHadLowerLimitation = other.isHadLowerLimitation;
    listNameDefaultAnchor = other.listNameDefaultAnchor;
    nameAngleOffset = other.nameAngleOffset;
    extraInfoMap = other.extraInfoMap;
    annotationMap = other.annotationMap;
    dataPointsMap = other.dataPointsMap;
    sourceProperty = other.sourceProperty;
    prototypeName_CHS = other.prototypeName_CHS;
    prototypeReadableName = other.prototypeReadableName;
    commentState = other.commentState;
}

#pragma once
#include <QBoxLayout>
#include <QColor>
#include <QMap>
#include <QSharedPointer>
#include <QSize>
#include <QString>

#include "CanvasContext.h"
#include "GraphicsKernelDefinition.h"
#include "IConnectorWireAlgorithm.h"
#include "SourceProxy.h"

class SourceProxy;
class ConnectorWireContext;
class PortContext;

const int globalGridSpacing = 10;

class CanvasContextPrivate
{
public:
    CanvasContextPrivate();
    CanvasContextPrivate(const CanvasContextPrivate &other);
    ~CanvasContextPrivate() {};
    CanvasContextPrivate &operator=(const CanvasContextPrivate &other);
    /// @brief 画板名称
    QString name;
    /// @brief 画板唯一标识ID
    QString uuid;
    /// @brief 画板大小，单位mm
    QSize size;
    /// @brief 画板布局方向
    QBoxLayout::Direction direction;
    /// @brief 画板背景颜色
    QColor backgroundColor;
    /// @brief 画板缩放比例
    qreal scale;
    /// @brief 画板类型
    CanvasContext::Type type;
    /// @brief 画板描述信息
    QString description;
    /// @brief 上次打开画板时的场景中心点坐标
    QPointF centerPoint;
    /// @brief 版本信息
    QString version;
    /// @brief 画板背景网格间距
    int gridSpace;
    /// @brief 画板网格属性flag
    CanvasContext::GridFlag gridFlag;
    /// @brief 画板图层属性flag
    CanvasContext::LayerFlag layerFlag;
    /// @brief 画板中的资源映射表，每一个SourceProxy对应一个图元
    QMap<QString, QSharedPointer<SourceProxy>> sourceMap;
    /// @brief 画板中连接线上下文映射表
    QMap<QString, QSharedPointer<ConnectorWireContext>> connectorWireMap;
    /// @brief 画板本身暴露的端口信息，主要用于把已有画板当作模块时使用
    QList<QSharedPointer<PortContext>> portList;
    /// @brief 连接线算法名称
    QString connectorWireAlgorithmName;
    /// @brief 连接线是否显示箭头
    bool showArrow;
    /// @brief 连接线是否显示开关
    bool showConnectorWireSwitch;
    /// @brief 业务钩子服务
    BusinessHooksServer *businessHooksServer;
    /// @brief 画板的附加参数信息，方便后续拓展
    QMap<QString, QVariant> extraInfoMap;
    /// @brief 连接线是否显示分支点
    bool showConnectorWireBranchPoint;
    /// @brief 画板图元是否允许重叠
    bool allowOverlap;
    /// @brief 高亮同源连接线
    bool highlightHomologousConnectorWire;
    /// @brief 保存画板属性map，非两部分保存，
    /// key：usually，保存整体图层
    /// key:special,保存特殊图层，比如某个图层下的某个对象属性
    QVariantMap layerPropertyMap;
    /// @brief 是否显示导航预览窗口
    bool showNavigationPreview;
    /// @brief 画板指定区域放大是否被按下
    bool magnify;
    /// @brief 画板平移是否被按下
    bool screenDrag;
    /// @brief 画板是否被锁定，对应工具栏冻结模式
    bool locked;
    /// @brief 画板作者
    QString author;
    /// @brief 最后修改时间
    QString lastModifyTime;
    /// @brief 连接线样式
    int lineStyle;
    /// @brief 连接线颜色
    QColor lineColor;
    /// @brief 连接线选定颜色
    QColor lineSelectColor;
    /// @brief 潮流计算参数显示 不参与序列化
    bool isShowPowerFlowData;
    /// @brief 画板是否显示对齐线
    bool isShowAlignLine;

    /// @brief 画板的交互模式，该字段不序列化
    InteractionMode interactionMode;

    /// @brief 画板状态，该字段不序列化
    CanvasContext::StatusFlags statusFlag;

    /// @brief 画板in模块map 该字段不序列化
    QMap<int, QSharedPointer<SourceProxy>> inSourceMap;

    /// @brief 画板out模块map 该字段不序列化
    QMap<int, QSharedPointer<SourceProxy>> outSourceMap;

    CanvasProperty canvasProperty;

    /// @brief 显示模型的运行顺序
    bool showModelRunningSort;
    /// @brief 用于标识创建子系统时的鼠标点击位置(不需要序列化保存)
    QPointF subsystemClickPos;

    /// @brief 保存sencerect
    QRectF senceRectf;

    /// @brief 子系统是否初始化(创建子系统之后是否双击查看过，双击显示时对输入输出进行位置调整)
    bool subsystemInitialized;

    /// @brief 当前画板是否支持悬空连接线
    bool isSupportSuspendedConnectorWire;
};

CanvasContextPrivate::CanvasContextPrivate()
    : name(""),
      uuid(""),
      size(QSize(297, 210)),
      direction(QBoxLayout::LeftToRight),
      backgroundColor(QColor(255, 255, 255)),
      scale(1.0),
      type(CanvasContext::kInvalidType),
      description(""),
      author(""),
      centerPoint(QPointF(0, 0)),
      version(CANVAS_VERSION),
      gridSpace(globalGridSpacing),
      gridFlag(CanvasContext::kGridShowDotGrid),
      layerFlag(CanvasContext::kLayerNone),
      connectorWireAlgorithmName(ManhattaAlgorithmName),
      showArrow(true),
      showConnectorWireSwitch(true),
      businessHooksServer(nullptr),
      showConnectorWireBranchPoint(true),
      allowOverlap(false),
      highlightHomologousConnectorWire(false),
      showNavigationPreview(true),
      magnify(false),
      screenDrag(false),
      locked(false),
      lineStyle(Qt::SolidLine),
      lineColor(GKD::WIRE_NORMAL_COLOR),
      lineSelectColor(GKD::WIRE_NORMAL_COLOR),
      isShowPowerFlowData(false),
      isShowAlignLine(false),
      interactionMode(kNormal),
      statusFlag(CanvasContext::kInvalid),
      subsystemClickPos(QPointF()),
      senceRectf(QRectF(0, 0, 2239, 1583))
{
    extraInfoMap[GKD::CANVAS_INPUT_INDEX] = 1;
    extraInfoMap[GKD::CANVAS_OUTPUT_INDEX] = 1;
    showModelRunningSort = false;
    subsystemInitialized = true;
    isSupportSuspendedConnectorWire = false;
}

CanvasContextPrivate::CanvasContextPrivate(const CanvasContextPrivate &other)
{
    *this = other;
}

inline CanvasContextPrivate &CanvasContextPrivate::operator=(const CanvasContextPrivate &other)
{
    name = other.name;
    uuid = other.uuid;
    size = other.size;
    direction = other.direction;
    backgroundColor = other.backgroundColor;
    type = other.type;
    description = other.description;
    centerPoint = other.centerPoint;
    version = other.version;
    gridSpace = other.gridSpace;
    gridFlag = other.gridFlag;
    connectorWireAlgorithmName = other.connectorWireAlgorithmName;
    showArrow = other.showArrow;
    showConnectorWireSwitch = other.showConnectorWireSwitch;
    extraInfoMap = other.extraInfoMap;
    showConnectorWireBranchPoint = other.showConnectorWireBranchPoint;
    allowOverlap = other.allowOverlap;
    highlightHomologousConnectorWire = other.highlightHomologousConnectorWire;
    layerPropertyMap = other.layerPropertyMap;
    showNavigationPreview = other.showNavigationPreview;
    locked = other.locked;
    author = other.author;
    lastModifyTime = other.lastModifyTime;
    lineStyle = other.lineStyle;
    lineColor = other.lineColor;
    lineSelectColor = other.lineSelectColor;
    senceRectf = other.senceRectf;

    sourceMap.clear();
    connectorWireMap.clear();
    portList.clear();
    QMapIterator<QString, QSharedPointer<SourceProxy>> sourceIter(other.sourceMap);
    while (sourceIter.hasNext()) {
        sourceIter.next();
        QSharedPointer<SourceProxy> source = sourceIter.value();
        sourceMap.insert(source->uuid(), QSharedPointer<SourceProxy>(new SourceProxy(*source.data())));
    }

    QMapIterator<QString, QSharedPointer<ConnectorWireContext>> wireIter(other.connectorWireMap);
    while (wireIter.hasNext()) {
        wireIter.next();
        QSharedPointer<ConnectorWireContext> wire = wireIter.value();
        connectorWireMap.insert(wire->uuid(),
                                QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(*wire.data())));
    }

    QListIterator<QSharedPointer<PortContext>> portIter(other.portList);
    while (portIter.hasNext()) {
        QSharedPointer<PortContext> port = portIter.next();
        portList.append(QSharedPointer<PortContext>(new PortContext(*port.data())));
    }

    canvasProperty = other.canvasProperty;
    showModelRunningSort = other.showModelRunningSort;
    subsystemInitialized = other.subsystemInitialized;

    return *this;
}

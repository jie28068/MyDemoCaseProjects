#pragma once
#include <QFont>
#include <QGraphicsItem>
#include <QObject>
#include <QString>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define CircleAnnotationStr "circle"
#define RectAnnotationStr "rect"
#define LineSegmentAnnotationStr "line-segment"
#define LineSegmentArrowAnnotationStr "line-segment-arrow"
#define BrokenLineAnnotationStr "broken-line"
#define BrokenLineArrowAnnotationStr "broken-line-arrow"
#define PolygonAnnotationStr "polygon"
#define ArcAnnotationStr "arc"

#define ImageAnnotationStr "image"
#define TextAnnotationStr "text"

// 图元类型
enum CanvasKernelGraphicsType {
    kTransformProxyGraphics = QGraphicsItem::UserType + 1,
    kPortGraphics,
    kPortRegulator,
    kSourceGraphics,
    kConnectorWireGraphics,
    kConnectorWireRegulator,
    kConnectorWireSwitch,
    kTextGraphics,
    kPortNameTextGraphics,
    kSourceNameTextGraphics,
    kCommentSourceGraphics,
    kCommentSourceTextGraphics,
    kLegendGraphics,
    kGeometricAnnotationGraphics,
    kConnectorWireSegment,
    kDataTextGraphics,
    kVirtualPortGraphics,
    kConnectorRetulationControllerGraphics,
};

static QString CANVAS_VERSION = "20221222";        // 当前画板版本  详细说明见graphics/画板版本说明.md
static QString CANVAS_VERSION_FORMAT = "yyyyMMdd"; // 版本字符串日期格式

/// layerstr
static const QString LayerStr_TransformProxyGraphicsLayer = "kTransformProxyGraphicsLayer";
// 基础图层
static const QString LayerStr_SourceGraphicsLayer = "kSourceGraphicsLayer";
static const QString LayerStr_PortGraphicsLayer = "kPortGraphicsLayer";
static const QString LayerStr_ConnectorWireGraphicsLayer = "kConnectorWireGraphicsLayer";
static const QString LayerStr_TextGraphicsLayer = "kTextGraphicsLayer";
static const QString LayerStr_BusbarPhaseGraphicsLayer = "kBusbarPhaseGraphicsLayer";
// 标注
static const QString LayerStr_CommentGraphicsLayer = "kCommentGraphicsLayer";
// 元件名称
static const QString LayerStr_SourceNameGraphicsLayer = "kSourceNameGraphicsLayer";
// 图例
static const QString LayerStr_LegendGraphicsLayer = "kLegendGraphicsLayer";
/// 图层中文名，用来显示
static const QString DisplayLayer_SourceBasicGraphicsLayer = QObject::tr("Base Layer");    // 基础图层;
static const QString DisplayLayer_SourceNameGraphicsLayer = QObject::tr("Component Name"); // 元件名称;
static const QString DisplayLayer_CommentGraphicsLayer = QObject::tr("Annotation");        // 标注;
static const QString DisplayLayer_LegendGraphicsLayer = QObject::tr("Legend");             // 图例;
static const QString DisplayLayer_ConnectorWireLayer = QObject::tr("Connector Wire");      // 连接线

/// 2、文本字体 （TextFont）
static const QString LayerPropertyTextFont = "TextFont";
///"TextFont" : QFont
/// 3、图层是否可以用户交互，拖拽&
static const QString LayerPropertyInteractive = "Interactive";

// 图层是否可以移动
static const QString LayerPropertyMovable = "Moveable";

static const QString LayerPropertySelectable = "Selectable";

///"Interactive" : bool
/// 4、画笔式样
static const QString LayerPropertyPenStyle = "PenStyle";
static const QString LayerPropertyPenStyle_Color = "PenColor";
static const QString LayerPropertyPenStyle_LineStyle = "LineStyle";
//"PenStyle" : {
//    "PenColor" : QColor,
//    "LineStyle" : QT::PenStyle,
//}
/// 5、填充颜色
static const QString LayerPropertyFillColor = "FillColor";
///"FillColor" : QColor
/// 6、图层是否可见属性
static const QString LayerPropertyVisible = "Visible";
///"Visible" : bool 默认可见

namespace GKD {                                     // 图形建模kernel定义
static const QString WIRE_NORMAL_COLOR = "#000000"; // 连接线正常颜色
// static const QString WIRE_NORMAL_COLOR = "#000000";  //连接线正常颜色
static const QString WIRE_DISABLE_COLOR = "#7d7d7d"; // 连接线失效颜色
static const qreal PORT_SIZE = 8;                    // 端口大小

// 框选样式
static const QString SELECT_LINE_NORMAL_COLOR = "#daefff"; // 选择框线条正常颜色

////////////////////////////////////////////////////////////////////// @brief 图形建模Server与Kennel通信定义
// SourceProxy属性变更Key值
static const QString SOURCE_CANVASNAME = "SOURCE_CANVASNAME";
static const QString SOURCE_NAME = "SOURCE_NAME";
static const QString SOURCE_PROTOTYPENAME = "SOURCE_PROTOTYPENAME";
static const QString SOURCE_POS = "SOURCE_POS";
static const QString SOURCE_SIZE = "SOURCE_SIZE";
static const QString SOURCE_SCALE_TRANSFORM = "SOURCE_SCALE_TRANSFORM";
static const QString SOURCE_ROTATE_TRANSFORM = "SOURCE_ROTATE_TRANSFORM";
static const QString SOURCE_TRANSLATE_TRANSFORM = "SOURCE_TRANSLATE_TRANSFORM";
static const QString SOURCE_STATE = "SOURCE_STATE";
static const QString SOURCE_STATETIPS = "SOURCE_STATETIPS";
// static const QString SOURCE_SVG_IMAGELIST = "SOURCE_SVG_IMAGELIST";
static const QString SOURCE_SVG_IMAGEMAP = "SOURCE_SVG_IMAGEMAP";
static const QString SOURCE_OTHER = "SOURCE_OTHER";           // 其他属性，如DC、AC
static const QString SOURCE_DATA_POINT = "SOURCE_DATA_POINT"; // 展示潮流计算等数据的模块的位置
static const QString SOURCE_PROTOTYPENAME_CHS = "SOURCE_PROTOTYPENAME_CHS";
static const QString SOURCE_COMMENTSTATE = "SOURCE_COMMENTSTATE"; // 模块注释状态：直通、禁用

// 区分SourceProxy的moduleType属性定义值
static const QString SOURCE_MODULETYPE_INVALID = "invalid";            // 无效type
static const QString SOURCE_MODULETYPE_ELECTRICAL = "electrical";      // 电气模块
static const QString SOURCE_MODULETYPE_CONTROL = "control";            // 控制模块
static const QString SOURCE_MODULETYPE_ANNOTATION = "annotation";      // 注释模块
static const QString SOURCE_MODULETYPE_STATEMACHINE = "state_machine"; // 状态机

// 模块--额外信息
static const QString SOURCE_EXTRA_INFO_CONSTANT_PARAMETER = "SOURCE_EXTRA_INFO_CONSTANT_PARAMETER"; // 常数模块常量参数

static const QString SOURCE_ANNOTATION_INFO = "SOURCE_ANNOTATION_INFO";

// 端口--额外信息
static const QString PORT_EXTRA_INFO_POSANGLE = "angle"; // 圆形加法器端口位置角度

// propertychange的key
static const QString LAYERPROPERTY_USUALLY = "layerPropertyUsually"; // 通用图层属性
static const QString LAYERPROPERTY_SOURCE = "layerPropertySource";   // source属性

// CanvasContext属性变更Key值
static const QString CANVAS_NAME = "CANVAS_NAME";
static const QString CANVAS_SIZE = "CANVAS_SIZE";
static const QString CANVAS_DIRECTION = "CANVAS_DIRECTION";
static const QString CANVAS_BACKGROUND_COLOR = "CANVAS_BACKGROUND_COLOR";
static const QString CANVAS_SCALE = "CANVAS_SCALE";
static const QString CANVAS_DESCRIPTION = "CANVAS_DESCRIPTION";
static const QString CANVAS_CENTER_POS = "CANVAS_CENTER_POS";
static const QString CANVAS_VERSION = "CANVAS_VERSION";
static const QString CANVAS_GRID_SPACE = "CANVAS_GRID_SPACE";
static const QString CANVAS_LAYER_FLAG = "CANVAS_LAYER_FLAG";
static const QString CANVAS_WIRE_ALG = "CANVAS_WIRE_ALG";
static const QString CANVAS_AUTHOR = "CANVAS_AUTHOR";
static const QString CANVAS_FREEZE_STATUS = "CANVAS_FREEZE_STATUS";
static const QString CANVAS_LastModify_Time = "CANVAS_LastModify_Time";
static const QString CANVAS_InOutIndex_Change = "CANVAS_InOutIndex_Change";
// extralinfo属性的key值

static const QString CANVAS_GRIDALIGNMENT = "CANVAS_GRIDALIGNMENT"; // 元件网格对齐

static const QString CANVAS_BOARDAUTHOR = "CANVAS_BOARDAUTHOR";     // 作者
static const QString CANVAS_BOARDLASTTIME = "CANVAS_BOARDLASTTIME"; // 最后修改时间
static const QString CANVAS_LINE_STYLE = "CANVAS_LINE_STYLE";
static const QString CANVAS_LINE_COLOR = "CANVAS_LINE_COLOR";
static const QString CANVAS_LINE_SELECTCOLOR = "CANVAS_LINE_SELECTCOLOR";

static const QString CANVAS_INPUT_INDEX = "CANVAS_INPUT_INDEX";       // 画板输入端口index
static const QString CANVAS_OUTPUT_INDEX = "CANVAS_OUTPUT_INDEX";     // 画板输出端口index
static const QString CANVAS_LAYER_PROPERTY = "CANVAS_LAYER_PROPERTY"; // 画板图层属性

// 默认字体
static const QFont DEFAULT_FONT = QFont("Arial", 12);

};

namespace Annotation {

static const QString segmentColor = QString("segmentColor"); // 边框、线段颜色
static const QString segmentWidth = QString("segmentWidth"); // 边框、线段宽度
static const QString pointArray = QString("pointArray");     // 位置数组 画多边形和线段用

// 文本框
//  static const QString textSize = QString("textSize");       //文本框大小
//  static const QString textFamily = QString("textFamily");   //字体
//  static const QString textStyle = QString("textStyle");     //字体风格
//  static const QString isBold = QString("isBold");           //是否加粗
//  static const QString isItalic = QString("isItalic");       //是否斜体
static const QString textColor = QString("textColor"); // 文本颜色
// static const QString isStrikeout = QString("isStrikeout"); //是否有删除线
// static const QString isUnderline = QString("isUnderline"); //是否有下划线
static const QString isShowframe = QString("isShowframe"); // 是否显示边框
static const QString alignment = QString("alignment");     // 对齐方式
static const QString content = QString("content");         // 文本内容
static const QString textFont = QString("textFont");       // 文本字体样式
static const QString textWidth = QString("textWidth");     // 文本框宽度

// 图片框
static const QString iconPath = QString("iconPath"); // 图片路径
static const QString iconName = QString("iconName"); // 图片名称
static const QString iconData = QString("iconData"); // 图片数据

// 线段相关
static const QString isShowArrow = QString("isShowArrow");         // 线段是否显示箭头
static const QString isShowHeadArrow = QString("isShowHeadArrow"); // 线段是否显示前端箭头
static const QString segmentStyle = QString("lineStyle");          // 线型

}

// 模块相关
struct BlockStruct {
    QString prototypeName;
    QString prototypeChsName;
    QString prototypeReadableName;
    int usageCount;
};

// 画板显示数据（潮流计算等数据）
struct showDataStruct {
    QString key;
    QString name;
    QString value;
    QString unit; // 单位
};

// 交互模式
enum InteractionMode {
    // 正常
    kNormal,
    // 区域放大
    kAreaAmplification,
    // 画板拖动
    kHandDrag,
    // 冻结模式
    kLock,
    // 注解
    kAnnotation,
    // 禁止交互
    kForbidden
};

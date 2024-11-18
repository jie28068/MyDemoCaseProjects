#pragma once

#include <QColor>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QPen>
#include <QScrollBar>
#include <QSharedPointer>
#include <QSpinBox>
#include <QToolButton>
#include <QUuid>
#include <QVBoxLayout>
#include <QtCore/qglobal.h>
#include <QtGui>

#include "qwt_scale_map.h"
#include "qwt_text.h"
#include <qwt_plot.h>

#include "CoreLib/ServerBase.h"
#include "CoreLib/ServerManager.h"

#include "CoreLib/GlobalConfigs.h"
#include "server/DataDictionary/IDataDictionaryServer.h"
#include "server/DataDictionary/IElementBase.h"
#include "server/DataDictionary/IVariableBase.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"

#define DefaultXAxisLength "DefaultXAxisLength"

#define CurveTreeViewHideColumns "CurveTreeViewHideColumns"
#define CurveTreeViewType "CurveTreeViewType"
#define IsShowCorsorTableView "IsShowCorsorTableView"
#define IsShowDictionaryView "IsShowDictionaryView"
#define DefaultCurveStyle "DefaultCurveStyle"
#define IsCopyPlotWithCuveData "IsCopyPlotWithCuveData"
#define IsShowVariableTable "IsShowVariableTable"

KCC_USE_NAMESPACE_MONITORPANEL

class PlotDefaultPropertyConfig
{
public:
    PlotDefaultPropertyConfig() { }
    ~PlotDefaultPropertyConfig() { }

    static PlotDefaultPropertyConfig *getInstance(void) { return m_instance; }
    void setDefaultProperty(const PlotDefaultProperty &property)
    {
        m_property.flags |= property.flags;
        if (property.flags & PlotFlag_BgColor)
            m_property.bgColor = property.bgColor;
        if (property.flags & PlotFlag_GridType)
            m_property.gridType = property.gridType;
        if (property.flags & PlotFlag_XDecimal)
            m_property.xDecimal = property.xDecimal;
        if (property.flags & PlotFlag_YDecimal)
            m_property.yDecimal = property.yDecimal;
        if (property.flags & PlotFlag_CurveWidth)
            m_property.curveWidth = property.curveWidth;
        if (property.flags & PlotFlag_CurveStyle)
            m_property.curveStyle = property.curveStyle;
        if (property.flags & PlotFlag_XRange)
            m_property.xRange = property.xRange;
        if (property.flags & PlotFlag_LegendSide)
            m_property.legendSide = property.legendSide;
    }

    bool isBgColor(void) const { return m_property.flags & PlotFlag_BgColor; }
    QColor bgColor(void) const { return m_property.bgColor; }

    bool isGridType(void) const { return m_property.flags & PlotFlag_GridType; }
    int gridType(void) const { return m_property.gridType; }

    bool isXDecimal(void) const { return m_property.flags & PlotFlag_XDecimal; }
    int xDecimal(void) const { return m_property.xDecimal; }

    bool isYDecimal(void) const { return m_property.flags & PlotFlag_YDecimal; }
    int yDecimal(void) const { return m_property.yDecimal; }

    bool isCurveWidth(void) const { return m_property.flags & PlotFlag_CurveWidth; }
    int curveWidth(void) const { return m_property.curveWidth; }

    bool isCurveStyle(void) const { return m_property.flags & PlotFlag_CurveStyle; }
    int curveStyle(void) const { return m_property.curveStyle; }

    bool isXRange(void) const { return m_property.flags & PlotFlag_XRange; }
    qreal xRange(void) const { return m_property.xRange; }

    bool isLegendSide(void) const { return m_property.flags & PlotFlag_LegendSide; }
    int legendSide(void) const { return m_property.legendSide; }

private:
    static PlotDefaultPropertyConfig *m_instance;
    PlotDefaultProperty m_property;
};

#define gDefault PlotDefaultPropertyConfig::getInstance()

enum ItemPlotCanvasCommands {
    ItemPlotCanvasCommand_Selecting = 0,      // 正在用鼠标拉动选择要放大的区域中
    ItemPlotCanvasCommand_Selected = 1,       // 已选中要放大的区域
    ItemPlotCanvasCommand_MouseMove = 2,      // 鼠标跟随显示
    ItemPlotCanvasCommand_ResetCoordinate = 3 // 初始化坐标
};

#ifndef PURE
#define PURE = 0
#endif

enum PlotVaribleInfoType { NoneNumber = -1, Number = 0, NumberArray, NumberArray2D };

typedef struct PlotCurveInfo {
    PlotCurveInfo()
        : Checked(true),
          Index(0),
          Row(0),
          Col(0),
          Color(QColor(255, 255, 17)),
          Width(2),
          ItemPlotCurveObject(nullptr),
          Style(1),
          YAxis(0),
          isVisible(true)
    {
        QVariant var = gConfGet(DefaultCurveStyle);
        if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
            PlotDefaultProperty prop;
            prop.flags = PlotFlags(PlotFlag_CurveWidth);
            prop.curveWidth = gConfGet("General_LineWidth").toInt();
            gDefault->setDefaultProperty(prop);
        }

        if (!var.isNull()) {
            Style = var.toInt();
        }
        if (gDefault->isCurveWidth()) {
            Width = gDefault->curveWidth();
        }
        if (gDefault->isCurveStyle()) {
            Style = gDefault->curveStyle();
        }
    }

    bool Checked;
    QString Name;
    int Index; // 对应于变量数据包里字段的索引，从0开始
    int Row;   //
    int Col;
    QColor Color;
    int Width;
    void *ItemPlotCurveObject;
    int Style = 0; // 0线，1方波
    int plotType = 0;

    int YAxis;      // 0,左Y轴，右Y轴
    bool isVisible; // 是否显示
} *LPPlotCurveInfo;

typedef struct PlotVaribleInfo {
    PlotVaribleInfo() : ElementBase(nullptr), VaribleBase(nullptr), Type(NoneNumber), Rows(0), Cols(0) { }
    QString Name;
    QString Path;
    QString Alias;
    // QVariant		Type;
    PlotVaribleInfoType Type;
    int Rows;
    int Cols;
    PIElementBase ElementBase;
    QString ElementID;
    PIVariableBase VaribleBase;

    QList<PlotCurveInfo> CurveInfoList;

    // bool			IsAddNew;//通过接口添加后没有初始化过为真
} *LPCurveInfo;

enum MarkerType { VLine = 0, HLine = 1, VLine2 = 2, MinLine = 3, MaxLine = 4 };

struct MarkerPoint {
    MarkerPoint() : otherData(0), isOtherData(false) { }
    QPointF value;
    QString name;
    QColor color;
    qreal otherData;
    bool isOtherData;
};

struct MarkerInfo {
    MarkerInfo()
        : color(Qt::blue),
          pos(0),
          type(VLine),
          scale(0.5f),
          isShow(true),
          xBottomIsLog(false),
          id(QUuid::createUuid().toString())
    {
    }

    QString name;
    QColor color;
    bool isShow;
    QString id;
    qreal pos;
    QVector<MarkerPoint> points;
    MarkerType type;
    qreal scale;
    bool xBottomIsLog; // xBottom是否是指数表示
    QString syncId;    // 同步Id,用于游标同步
};

struct AxisRange {
    AxisRange() : min(0), max(0) { }
    double min;
    double max;
};

typedef struct PlotInfo {
    PlotInfo()
        : XType(1),
          XRange(10.0f),
          BackgroundColor(QColor(255, 255, 255)),
          LegendSide(3),
          GridType(1),
          IsAutoYAxis(true),
          YMin(-1),
          YMax(1),
          IsShowTitle(false),
          IsShowSubTitle(false),
          IsShowXTitle(false),
          IsShowYTitle(false),
          XMin(0),
          XMax(10),
          xBottomIsLog(false),
          isLogVisible(false),
          SaveDataMode(0)
          //,DisplayStyle(0)
          ,
          xDecimal(-1),
          yDecimal(-1),
          isXRangeRoll(true),
          IsShowYRightTitle(false),
          IsAutoYRightAxis(true),
          YRightMin(-1),
          YRightMax(1),
          mRow(-1),
          mCol(-1),
          yRightDecimal(-1)

    {
        QVariant var = gConfGet(DefaultXAxisLength);
        if (!var.isNull()) {
            qreal length = var.toDouble();
            if (length > 0) {
                XRange = length;
            }
        }
        if (gDefault->isXRange()) {
            XRange = gDefault->xRange();
        }
        if (gDefault->isLegendSide()) {
            LegendSide = gDefault->legendSide();
        }
    }

    QString id;
    int XType;
    QString Title;
    QString SubTitle;
    double XRange;
    QColor BackgroundColor;
    int LegendSide;
    int GridType;

    bool IsAutoYAxis;
    double YMin;
    double YMax;
    bool IsShowTitle;
    bool IsShowSubTitle;
    QString XTitle;
    bool IsShowXTitle;
    QString YTitle;
    bool IsShowYTitle;
    double XMin;
    double XMax;
    bool xBottomIsLog; // xBottom是否是指数/对数形式呈现
    bool isLogVisible; // 是否显示对数框
    int SaveDataMode;  // 数据保存模式 0只保存当前X轴范围内数据，1保存所有数据
    // int			DisplayStyle;//0超出换页;1超出滚进
    QList<PlotVaribleInfo> VaribleInfoList;
    QList<MarkerInfo> Markers;
    QStringList y_nameList;
    QStringList y_srcList; // y轴源id列表
    QString x_name;
    QString x_src; // x轴源id
    int mRow;      // 该图所在的位置
    int mCol;

    bool isShowYsCaleNum;
    double yScaleNum;

    int xDecimal;      // x轴有效小数
    int yDecimal;      // y轴有效小数
    bool isXRangeRoll; // x轴范围是否可滚动显示
    // todo保存配置//////////////////////////////////////////////////////////////
    QString YRightTitle;
    bool IsShowYRightTitle;
    bool IsAutoYRightAxis;
    double YRightMin;
    double YRightMax;
    int yRightDecimal;

    QList<PlotInfo> childsInfo; // 用于Plot合并与拆分，保存合并前的信息
    ///////////////////////////////////////////////////////////////////
    // 下面只复制plot时临时用，无需保存进xml
    QMap<QwtPlot::Axis, AxisRange> axisRanges;

} *LPPlotInfo;

typedef struct DataAnalyseInfo {
    DataAnalyseInfo() { clear(); }
    void clear()
    {
        maxValue = 0;
        minValue = 0;
        averageValue = 0;
        samplingRate = 0;
        samplingTime = 0;
        isOther = false;
        maxOtherValue = 0;
        minOtherValue = 0;
    }
    double maxValue;
    double minValue;
    double averageValue;
    double samplingRate;
    // double		frequency;
    double samplingTime;
    QString curveName;
    QColor curveColor;
    bool isOther;
    double maxOtherValue;
    double minOtherValue;
} *LPDataAnalyseInfo;

#ifndef RGB
#define RGB(r, g, b)                                                                                                   \
    ((QRgb)(((unsigned char)(b) | ((unsigned short)((unsigned char)(g)) << 8)) | (((QRgb)(unsigned char)(r)) << 16)))
#endif

#define CURVECOLORS_COUNT 16

// RGB(255,105,41),//红
//	RGB(19,159,255),//蓝
//	//RGB(254,67,101),//红
//	RGB(75,159,14),//绿
const QRgb CURVECOLORS[] = { RGB(255, 0, 0),    // 红
                             RGB(0, 0, 255),    // 蓝
                             RGB(0, 255, 0),    // 绿
                             RGB(30, 159, 74),  // 比较深的绿
                             RGB(253, 241, 51), // 黄
                             RGB(191, 79, 31),  // 棕
                             RGB(179, 214, 110), RGB(248, 147, 29), RGB(137, 190, 178), RGB(222, 211, 140),
                             RGB(38, 157, 128),  RGB(255, 0, 255),  RGB(0, 255, 255),   RGB(255, 255, 0),
                             RGB(255, 255, 255), RGB(250, 193, 2) };

enum AxisType {
    AxisType_Invalid = 0x0,
    AxisType_yLeft = 0x1,
    AxisType_xBottom = 0x2,
    AxisType_yRight = 0x4,
    AxisType_xTop = 0x8
};
Q_DECLARE_FLAGS(AxisTypes, AxisType)

struct AxisRanges {
    AxisRanges()
        : types(AxisType_yLeft | AxisType_xBottom),
          yLeftMin(-1),
          yLeftMax(1),
          xBottomMin(0),
          xBottomMax(10),
          yRightMin(-1),
          yRightMax(1),
          xTopMin(0),
          xTopMax(10)
    {
    }

    AxisTypes types;
    qreal yLeftMin;
    qreal yLeftMax;
    qreal xBottomMin;
    qreal xBottomMax;
    qreal yRightMin;
    qreal yRightMax;
    // xtop坐标暂时没有使用//////////////////////////
    qreal xTopMin;
    qreal xTopMax;
};
Q_DECLARE_METATYPE(AxisRanges);

class IDataAnalyseObserver
{
public:
    virtual ~IDataAnalyseObserver() { }

    virtual void update(const QList<DataAnalyseInfo> &datas) = 0;
};

static QVector<QColor> defaultCurveColorVec { QColor("#E6C83C"), QColor("#00993E"), QColor("#C83232"),
                                              QColor("#036EB7") };

static QColor genRandomColor()
{
    uint64_t millisecond =
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count(); // 微秒
    // uint64_t microsecond =
    //         std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
    //                 .count(); // 毫秒
    srand(millisecond);
    return QColor(rand() % 256, rand() % 256, rand() % 256);
}
#ifndef GRAPHICSMODELINGCONST_H
#define GRAPHICSMODELINGCONST_H

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "server/Base/IServerInterfaceBase.h"
#include <QMetaType>
#include <QPoint>
#include <QSharedPointer>
#include <QSize>
#include <QString>
#include <QStringList>

using namespace Kcc::BlockDefinition;

namespace GraphicsModelingConst {

static const qreal CANVAS_SCALE_FACTOR = 100.0f; // 画板缩放比例因子  老->新 除
static const qreal PORT_POSITION_FACTOR = 10.0f; // 端口位置比例因子  老->新 除

static const int GRID_EDGE = 10; // 网格边长

// 控制画板中Block的默认宽高
static const int CONTROL_BLOCK_DEFAULT_WIDTH = 80;
static const int CONTROL_BLOCK_DEFAULT_HEIGHT = 40;
static const int CONTROL_BLOCK_MIN_WIDTH = 40;            ////控制模块最小宽度
static const int CONTROL_BLOCK_MIN_HEIGHT = 40;           ////控制模块最小高度
static const int CONTROL_BLOCK_MIN_HEIGHT_FROM_PORTS = 4; ////控制模块最小高度时端口数量

// 电气构造型Block默认宽高
static const int ELECCOMBINE_BLOCK_MIN_WIDTH = 60;  ////电气构造模块最小宽度
static const int ELECCOMBINE_BLOCK_MIN_HEIGHT = 60; ////电气构造模块最小高度

// 范围表达式解析用的的正则表达式
static const QString REG_BLOCK_BOARD_NAME =
        "^([^:<>/|\\\"\\*\\?\\\\]+)$"; // 不支持的字符*:<>/|?"\即windows可以生产的文件名字符

// 普通模块
static const QString GENERIC_BLOCK = "__generic";
static const QString TEXTTOOL_BLOCK = "TextToolBlock";
static const QString ICONTOOL_BLOCK = "IconToolBlock";

// 普通模块 圆形加法器
static const QString CONTROL_BLOCK_ADD = "Add";
static const int CIRCULARSUM_FIX_SIZE = 40;            ////圆形加法器固定宽高大小值（暂时）
static const int CIRCULARSUM_ONE_PORT_POS_ANGLE = 180; // 圆形加法器一个输入端口时位置角度
static const int CIRCULARSUM_TWO_PORT_POS_ANGLE = 90; // 圆形加法器两个输入端口时,第一个输入端口的位置角度
static const int CIRCULARSUM_PORT_POS_ANGLE_RANGE = 360; // 圆形加法器输入端口位置角度范围
static const int CIRCULARSUM_PORT_POS_ANGLE_OFFSET = 45; // 圆形加法器输入端口位置角度偏移量
static const int CIRCULARSUM_PORT_POS_ANGLE_START = 315; // 圆形加法器输入端口位置角度偏移量

/// @brief 特殊模块 --电气 ////////////////////////////////////////////////////
static const QString SPECIAL_BLOCK_BUSBAR = "Busbar";                               // 母线
static const QString SPECIAL_BLOCK_DOTBUSBAR = "DotBusbar";                         // 点状母线
static const QString SPECIAL_BLOCK_GROUND = "Ground";                               // 地线
static const QString SPECIAL_BLOCK_LINE = "Line";                                   // 线路
const static QString SPECIAL_BLOCK_LOAD = "Load";                                   // 负载
const static QString SPECIAL_BLOCK_RESISTOR = "Resistor";                           // 电阻
const static QString SPECIAL_BLOCK_CAPACITOR = "Capacitor";                         // 电容
const static QString SPECIAL_BLOCK_SM = "SynchronousMachine";                       // 同步发电机
static const QString SPECIAL_BLOCK_TWOWINDINGTRANSFORMER = "TwoWindingTransformer"; // 双绕组变压器模块
///////////////////////////////////////////////////////////////////////////////

/// @brief 特殊模块 --控制 ////////////////////////////////////////////////////
static const QString SPECIAL_BLOCK_CTRLOUT = "Out";
// 特殊模块 控制输入
static const QString SPECIAL_BLOCK_CTRLIN = "In";
// 特殊模块 输入总线模块
static const QString SPECIAL_BLOCK_BUS_CREATOR = "BusCreator";
// 特殊模块 输出总线模块
static const QString SPECIAL_BLOCK_BUS_SELECTOR = "BusSelector";
// 特殊模块 容器模块
// #define SPECIAL_BLOCK_CONTAINER  "Container"
// 特殊模块 插槽模块
static const QString SPECIAL_BLOCK_SLOT = "Slot";
// 特殊模块 复合模块
static const QString SPECIAL_BLOCK_COMPOSITE = "Composite";
// 特殊模块 电气接口模块
static const QString SPECIAL_BLOCK_ELECINTERFACE = "ElectricalInterface";
// 特殊模块 空模块
static const QString SPECIAL_BLOCK_NULL = "Null";
// 特殊模块 fmu模块
static const QString SPECIAL_BLOCK_FMU = "FMU";
// 特殊模块 AND模块
static const QString SPECIAL_BLOCK_AND = "AND";
// 特殊模块 OR模块
static const QString SPECIAL_BLOCK_OR = "OR";
// 特殊模块 max模块
static const QString SPECIAL_BLOCK_MAX = "max";
// 特殊模块 min模块
static const QString SPECIAL_BLOCK_MIN = "min";
// 特殊模块 ToFile模块
static const QString SPECIAL_BLOCK_TOFILE = "ToFile";
// 特殊模块 FromFile模块
static const QString SPECIAL_BLOCK_FROMFILE = "FromFile";

static const QString CONTROL_BLOCK_Sum3Gain = "Sum3Gain";
static const QString CONTROL_BLOCK_SumGain = "SumGain";
static const QString CONTROL_BLOCK_Gain = "Gain";
static const QString CONTROL_BLOCK_InvertedGain = "InvertedGain";
static const QString CONTROL_BLOCK_Time = "time";

///////////////////////////////////////////////////////////////////////////////

static const QString IS_CHOOSE_ELEMENT = "IsChooseElement";

static const QString CONTROL_BLOCK_ICON_PATH =
        ":/controlblock/Resources/controlblock/"; // 画板上加载【控制模块】图片路径
static const QString ELECTRICAL_BLOCK_ICON_PATH =
        ":/electricalblock/Resources/electricalblock/"; // 画板上加载【电气模块】图片路径

// 迁移IUIDrawingBoardServer.h
enum Rule { // 连通规则的优先级大于器件规则的优先级(如果尚未连通，器件参数是没有意义的)
    None = 0x00,
    // 连通规则
    Connect_Mask = 0xffffff00,
    Connect_Normal = 0x01,  // 连通正常
    Connect_Disable = 0x02, // 器件悬挂
    Connect_Hangup = 0x04,  // 器件挂起
    // 器件状态规则
    /**电气**/
    Electrical_Property_Mask = 0xff0000ff,
    Param_Normal = 0x0100,   // 参数正常
    Vol_MisMatch_1 = 0x0200, // 设备的电压等级与锁链母线的电压等级不匹配，差异小于50%
    Vol_MisMatch_2 = 0x0400, // 设备的电压等级与锁链母线的电压等级不匹配，差异大于等于50%
    TWTF_Hv_Less = 0x0800,   // 双绕组变压器高压侧规则
    TWTF_Lv_Less = 0x1000,   // 双绕组变压器低压侧规则
    TWTF_Lv_More = 0x2000,
    TWTF_Hv_More = 0x4000,

    AC_MisMatch = 0x10000,                        // 纯直流元件不能连接到交流母线
    Dt_Miss = 0x20000,                            // 模块无设备类型
    Dt_Delete = 0x40000,                          // 模块设备类型被删除
    AC_PMisMatch = 0x80000,                       // 换流器正级不能连接到交流母线
    AC_NMisMatch = 0x100000,                      // 换流器负极不能连接到交流母线
    Voltage_Source_Converter_PMatch_1 = 0x200000, // 换流器直流侧正极和所连接母线的电压等级不匹配，差异小于50%
    Voltage_Source_Converter_PMatch_2 = 0x400000, // 换流器直流侧正极和所连接母线的电压等级不匹配，差异大于等于50%

    Voltage_Source_Converter_NMatch_1 = 0x800000, // 换流器直流侧负极和所连接母线的电压等级不匹配，差异小于50%
    Voltage_Source_Converter_NMatch_2 = 0x1000000, // 换流器直流侧负极和所连接母线的电压等级不匹配，差异大于等于50%

    Registor_Invalid = 0x2000000, // 电阻两端母线类型不一致
    Diode_Converter_Valve_AMisMatch = 0x4000000,
    Diode_Converter_Valve_PMisMatch = 0x8000000,
    Diode_Converter_Valve_NMisMatch = 0x10000000,
    Electrical_Max = 0x20000000,

};

enum LayoutType {
    netElementsLayer = 0x0000,     // 显示画板所有器件或模块图层
    blockNameLayes = 0x0001,       // 显示模块名称图层
    triphaseLineBoxLayes = 0x0002, // 显示模块单线、三线图层
    annotationsLayer = 0x0004,     // 显示注释类型模块图层
    titleInfoLayer = 0x008,        // 显示画板右下角信息表格图层
    connectingLineLayer = 0x0010,  // 显示控制画板中的连接线
};
Q_DECLARE_FLAGS(LayoutTypes, LayoutType)
Q_DECLARE_OPERATORS_FOR_FLAGS(
        LayoutTypes) // 使得 LayoutType 和 LayoutType，LayoutType 和 LayoutTypes 能够使用或运算符 |，结果为 LayoutTypes

struct LayoutInfo {
    QString layoutName;
    bool isChecked;
};

} // end namespace

//////////////////////////////////////////////////////////////////////////

// 电气模块
static const QString ERROR_INFO_DEVICETYPE_NO =
        QObject::tr("module with no equipment type!"); // 模块无设备类型！不可用！请选择/新建设备类型！
static const QString ERROR_INFO_DEVICETYPE_DEL =
        QObject::tr("module device type has been deleted!"); // 模块设备类型已被删除！不可用！请选择/新建设备类型！
// 控制模块
static const QString ERROR_INFO_PROTOTYPE_DEL = QObject::tr("module prototype has been deleted!"); // 模块原型已被删除！
static const QString ERROR_INFO_PROTOTYPE_MOD_NOUSE =
        QObject::tr("module prototype has been modified! do not use!"); // 模块原型已被修改！不可用!
static const QString ERROR_INFO_PROTOTYPE_MOD_MAYEFFECT =
        QObject::tr("module prototype has been modified! may affect the function!"); // 模块原型已被修改！可能影响功能
static const QString ERROR_INFO_PROTOTYPE_MOD_NOEFFECT = QObject::tr(
        "module prototype has been modified!does not affect the function."); // 模块原型已被修改！不影响功能！
static const QString ERROR_INFO_PROTOTYPE_MOD_INVALID =
        QObject::tr("electrical interface module associated electrical components have been deleted!the electrical "
                    "interface module need to reconfigure!");
// 电气接口模块关联的电气元件已被删除！该电气接口模块需重新配置！
static const QString ERROR_INFO_PROTOTYPE_MOD_NULL =
        QObject::tr("The original model does not exist."); // 模块的原始模型不存在.
static const QString ERROR_INFO_PROTOTYPE_MOD_NULL_EN = "The original model does not exist.";

#endif // GRAPHICSMODELINGCONST_H

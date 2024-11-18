#ifndef __GLOBALDEFINITION__
#define __GLOBALDEFINITION__

#include <QColor>
#include <QDrag>
#include <QMap>
#include <QMimeData>
#include <QObject>
#include <QPainterPath>
#include <QSharedPointer>
#include <QStringList>
#include <QtGlobal>

#include "PublicDefine.h"
#include "server/ModelManagerServer/ModelManagerConst.h"

// using namespace KL_TOOLKIT;
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

// 拖拽时mimedata的key值，
#define DRAG_MIME_DATA_TYPE_NAME "Model/type_name" // 与画板插件约定传递元器件名称使用此key
#define DRAG_MIME_DATA_CUSTOM_NAME "ComponentModel/data"

// 一些常量的定义
namespace Global {
// 数据类型：
static const QString DataType_Double = QString("double");
static const QString DataType_Bool = QString("bool");
static const QString DataType_KccString = QString("KccString");
static const QString DataType_DoubleVector = QString("doubleVector");
static const QString DataType_DoubleComplex = QString("doubleComplex");
static const QString DataType_VoidBuffer = QString("voidBuffer");
static const QString DataType_Int = QString("int");
static const QString DataType_String = QString("string");
static const QString DataType_Enum = QString("enum");
static const QString DataType_Enummap = QString("enummap");
static const QString DataType_Matrix = QObject::tr("Custom Matrix Types");
// 限幅类型
static const QString LimitType_NO = QObject::tr("There Is No");
static const QString LimitType_UPPER = QObject::tr("Upper Limit Range");
static const QString LimitType_LOWER = QObject::tr("Lower Limit Amplitude");
// 输入限制
static const QString REG_VECTOR_DOUBLE = "^((-?\\d+)(\\.\\d+)?([eE](-?\\d+))?)(,(-?\\d+)(\\.\\d+)?([eE](-?\\d+))?)*";
static const QString REG_COMPLEX_DOUBLE = "^((-?\\d+)(\\.\\d+)?([eE](-?\\d+))?),(-?\\d+)(\\.\\d+)?([eE](-?\\d+))?$";
static const QString REG_DOUBLE = "^(-?\\d{1,300})(\\.\\d{1,15})?([eE]([-+]?\\d{1,3}))?$";
static const QString REG_SIGNVECTOR = "^([+-]+)$";
static const QString REG_ADDSIGNVECTOR = "^([+-]{1,3})$";
/// 访问model的role
// 通用role
static const int CategoryRole = Qt::UserRole + 1; // 元器件总分类【设备、控制、测量】
static const int ModuleNameRole = Qt::UserRole + 2; // 模块名，对应[设备、控制、测量],方便获取对应的菜单管理器
static const int GroupNameRole = Qt::UserRole + 3; // 在ToolBox视图中的组名，在树视图中是其父节点名称
static const int ContextTypeRole = Qt::UserRole + 4;       // 交互菜单类型
static const int NameRole = Qt::UserRole + 5;              // 元器件名称
static const int ReadableNameRole = Qt::UserRole + 6;      // 元器件原型的友好名称
static const int DisplayNameRole = Qt::UserRole + 7;       // 元器件中文名称
static const int UrlRole = Qt::UserRole + 8;               // 元器件资源url列表
static const int ItemDisplayTypeRole = Qt::UserRole + 9;   // 元器件资源url列表
static const int ItemDisplayOrderRole = Qt::UserRole + 10; // 元器件排序
static const int ItemNeedDisplay = Qt::UserRole + 11;      // 元件是否显示
static const int DragEnabledRole = Qt::UserRole + 12;      // 元器是否可拖拽
static const int UrlNoDragRole = Qt::UserRole + 13;        // 元器件不可拖拽的Qbytearray资源
static const int PrototypeNameRole = Qt::UserRole + 14;    // 模块原型名，特殊处理加法模块的Sign参数
// 控制元器件专属
static const int ControllerTypeRole = Qt::UserRole + 101; // 控制元器件类型：[代码型，组合型]
static const int BuildInRole = Qt::UserRole + 102;        // 控制元器件是否内建
static const int CanDeleteRole = Qt::UserRole + 103;      // 控制元器件是否可删除
static const int ClassificationRole = Qt::UserRole + 104; // 控制元器件的元器件分类
// 特殊
static const int TypeRole = Qt::UserRole + 401; // 类型，区分元器件节点和父节点，树视图适用

static const QString PROTOTYPENAME = "prototypename"; // 原型名key
static const QString PROTOTYPENAME_SUM = "Sum";       // 加法模块原型名
static const QString PROTOTYPENAME_ADD = "Add";       // 圆形加法模块原型名
static const QString KEYWORDS_SUMADD_SIGN = "Sign";   // 模块参数Sign只能输入+-

// 大分类名称
static const QString DeviceModuleName = QObject::tr("Equipment");
static const QString ControllerModuleName = QObject::tr("control");
static const QString MeasurerModuleName = QObject::tr("measurement");

// 电气分类
static const QString DeviceGroupBus = QObject::tr("Bus bus line");
static const QString DeviceGroupElement = QObject::tr("Basic element");
static const QString DeviceGroupMachine = QObject::tr("motor");
static const QString DeviceGroupTransformer = QObject::tr("transformer");
static const QString DeviceGroupLine = QObject::tr("Transmission line");
static const QString DeviceGroupLoad = QObject::tr("load");
static const QString DeviceGroupDC = QObject::tr("Direct current element");

// 控制分类
static const QString ControllerGroupBasicFunction = QObject::tr("Basic function module");
static const QString ControllerGroupSpecialFunction = QObject::tr("Special function module");
static const QString ControllerGroupFirstOrderOperation = QObject::tr("First-order operation module");
static const QString ControllerGroupCommonlyUsed = QObject::tr("Common modules");
// static const QString ControllerGroupOther = QObject::tr("未分类模块");
static const QString ControllerGroupOther = QObject::tr("Interface module");
static const QString ControllerGroupCustomCode = QObject::tr("Native code type");
static const QString ControllerGroupCustomBoard = QObject::tr("Local stereotypes");
// 测量分类
static const QString MeasurerGroupMeasurer = QObject::tr("Measuring module");

static const QString PluginName_D = QString("D"); // Debug的后缀
// 插件名（layout里面的tagename）
static const QString PluginName_Device = QString("ComponentViewDevice");   // 电气
static const QString PluginName_Control = QString("ComponentViewControl"); // 控制

// 画元件icon的一些颜色及大小
static const QColor TEXTCOLOR_NORMAL = QColor("#252d3f");  // 文字颜色
static const QColor TEXTCOLOR_DISABLE = QColor("#D1D1D1"); // 禁用文字颜色

// static const QColor BOARDCOLOR_SELECTED = QColor("#35405C");//选中时边框颜色
// static const QColor BOARDCOLOR_UNSELECTED = QColor("#BBC3C7");//非选中时边框颜色
static const QColor BOARDCOLOR_DISABLE = QColor("#D3D3D3"); // 禁用边框颜色

static const QColor BGCOLOR_SELECTED = QColor("#DCE6F0");  // 选中时背景颜色
static const QColor BGCOLOR_MOUSEOVER = QColor("#EEF5FB"); // 滑过时背景颜色
static const QColor BGCOLOR_DISABLE = QColor("#E0E0E0");   // 禁用背景颜色

static const QColor BLOCKCOLOR_PROTOTYPE = QColor("#e60012"); // 模块原型最外层的红色
static const QColor BLOCKCOLOR_INSTANCE = QColor("#e0a420");  // 模块实例最外层的绿色

static const int DRAWING_ITEM_WIDTH = 65;
static const int DRAWING_ITEM_HEIGHT = 45;
static const int MAX_LENGTH_NMAE = 64;

// 向导图片页
const static QList<QString> picNames = QList<QString>()
        << QObject::tr("Normal State Image") << QObject::tr("Disabled State Image")
        << QObject::tr("Warning State Image") << QObject::tr("Error State Image"); // PNG名称
const static QList<QString> svgNames = QList<QString>()
        << QObject::tr("0° Image") << QObject::tr("90° Image") << QObject::tr("180° Image")
        << QObject::tr("270° Image"); // SVG名称
// 模块控件类型
const static QVariantMap ControlTyeName = {
    { QObject::tr("Textbox"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeTextbox },
    { QObject::tr("Checkbox"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeCheckbox },
    { QObject::tr("ComboBox"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeComboBox },
    { QObject::tr("ComboBoxInput"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeComboBoxInput },
    { QObject::tr("ComboBoxs"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeComboBoxMultiSelect },
    { QObject::tr("ComboBoxEdit"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeComboBoxDoubleEdit },
    { QObject::tr("Date"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeDate },
    { QObject::tr("Color"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeColor },
    { QObject::tr("Font"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeFont },
    { QObject::tr("Fileselect"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeFileSelect },
    { QObject::tr("Int"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeInt },
    { QObject::tr("IntArray"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeIntVector },
    { QObject::tr("Double"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeDouble },
    { QObject::tr("DoubleArray"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeDoubleVector },
    { QObject::tr("Complex"), Kcc::BlockDefinition::RoleDataDefinition::ControlTypeComplex },
    { QObject::tr("MathExpression"), Kcc::BlockDefinition::RoleDataDefinition::ControlMathExpression },
    { QObject::tr("MatrixType"), Kcc::BlockDefinition::RoleDataDefinition::ControlMatrixType }
};

// 是否显示端口
static const QString isShowPortName = "wizard.data.isShowPortName";

// 变量状态
const static QList<QString> variableState = QList<QString>()
        << QObject::tr("Invisible") << QObject::tr("Visible Not Editable") << QObject::tr("Visible Editable");

// 电气端口分类
const static QList<QString> portTypeList = QList<QString>()
        << QObject::tr("Electrical") << QObject::tr("Bus Bus Line") << QObject::tr("Communication")
        << QObject::tr("Direct Current");

// 生成端口信息
static const int portPosDefine[5][5] = {
    { 5, 0, 0, 0, 0 }, { 3, 7, 0, 0, 0 }, { 2, 5, 8, 0, 0 }, { 2, 4, 6, 8, 0 }, { 1, 3, 5, 7, 9 }
};

// 分类
enum Category {
    Device,     // 设备
    Controller, // 控制
    Measurer,   // 测量
    Unknow      // 未知
};

// 树视图item节点类型
enum Type {
    Collection, // 父节点（树视图中才有）
    Element     // 元器件节点
};

// 控制模块类型
enum ControllerType {
    Code, // 代码型
    Board // 组合型
};

// 控制模块分类
enum ControllerClass // 与BlockDefinition中一致
{
    BasicFunction,       // 内建基础函数模块
    SpecialFunction,     // 内建特殊函数模块
    FirstOrderOperation, // 内建一阶运算模块
    CommonlyUsed,        // 内建常用模块
    Other,               // 内建未分类模块
    //------------Othre之后就是本地库类别
    CustomCode, // 本地代码型
    CustomBoard // 本地组合型
};

// 电气模块分类
enum DeviceClass // 与BlockDefinition中一致，追加前缀Device，避免命名冲突
{
    DeviceBus,         // 母线
    DeviceElement,     // 基本元件
    DeviceMachine,     // 电机
    DeviceTransformer, // 变压器
    DeviceLine,        // 传输线
    DeviceLoad,        // 负载
    DeviceDC           // 直流元件
};

static Category categoryFromName(const QString &strModuleName)
{
    if (strModuleName == DeviceModuleName) {
        return Device;
    } else if (strModuleName == ControllerModuleName) {
        return Controller;
    } else if (strModuleName == MeasurerModuleName) {
        return Measurer;
    } else {
        return Unknow;
    }
}

static QString nameFromCategory(Category nCategory)
{
    switch (nCategory) {
    case Device:
        return DeviceModuleName;
    case Controller:
        return ControllerModuleName;
    case Measurer:
        return MeasurerModuleName;
    default:
        return QString();
    }
}

class GroupClass
{
public:
    // 显示类型name/icon
    enum DisplayType {
        DisplayType_Name = 0, // 文字
        DisplayType_Icon,     // 图标
    };

    // // 分组数据来源
    // enum GroupDataFrom {
    //     GroupDataFrom_Config = 0,  // 配置文件
    //     GroupDataFrom_DataManager, // 数据管理器
    // };

    GroupClass() { }

    virtual ~GroupClass() { }

    struct GroupBlockParamater {
        QString prototypeName; // 唯一,与datamanager的prototypename保持一致，关联key
        QString viewName;      // 显示名称
        QString viewIcon; // 资源路径，只要主资源，其余根据主资源加后缀，如果有需求的话。做成共通
        // bool display;              // 当前block是否显示
        DisplayType displaytype; // 显示类型：name/icon
        // unsigned int displayorder; // 模块在组里面的显示顺序
    };

public:
    QString groupName;       // 对应唯一的组名
    unsigned int groupOrder; // 对应组的显示顺序
    // GroupDataFrom groupDataSource;                     // 分组数据来源
    QMap<QString, GroupBlockParamater> groupBlocksMap; // 用prototypeName作为这个map的键
};

typedef QSharedPointer<GroupClass> PGroupClass;

class GroupConfigClass
{
public:
    GroupConfigClass() { }
    ~GroupConfigClass() { }

public:
    // QString pluginName;                   // 插件名
    // QString cfgFileName;                  // 文件名
    QMap<QString, PGroupClass> mapGroups; // key是组名
};
typedef QSharedPointer<GroupConfigClass> PGroupConfigClass;

}

#endif // __GLOBALDEFINITION__

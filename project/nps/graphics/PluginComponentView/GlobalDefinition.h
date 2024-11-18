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
static const QString DataType_VoidBuffer = QString("VoidBuffer");

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
static const int ControllerTypeRole = Qt::UserRole + 101;        // 控制元器件类型：[代码型，组合型]
static const int BuildInRole = Qt::UserRole + 102;               // 控制元器件是否内建
static const int CanDeleteRole = Qt::UserRole + 103;             // 控制元器件是否可删除
static const int ControlClassificationRole = Qt::UserRole + 104; // 控制元器件的元器件分类
// 电气元器件分类
static const int DeviceClassificationRole = Qt::UserRole + 105; // 控制元器件的元器件分类
// 特殊
static const int TypeRole = Qt::UserRole + 401; // 类型，区分元器件节点和父节点，树视图适用

static const QString PROTOTYPENAME = "prototypename"; // 原型名key
static const QString PROTOTYPENAME_SUM = "Sum";       // 加法模块原型名
static const QString PROTOTYPENAME_ADD = "Add";       // 圆形加法模块原型名
static const QString KEYWORDS_SUMADD_SIGN = "Sign";   // 模块参数Sign只能输入+-

// 大分类名称
static const QString DeviceModuleName = QObject::tr("Electrical");
static const QString ControllerModuleName = QObject::tr("Control");
static const QString MeasurerModuleName = QObject::tr("Measure");

// 电气分类
static const QString DeviceGroupBus = QObject::tr("Bus Bar");
static const QString DeviceGroupElement = QObject::tr("Basic Element");
static const QString DeviceGroupMachine = QObject::tr("Motor");
static const QString DeviceGroupTransformer = QObject::tr("Transformer");
static const QString DeviceGroupLine = QObject::tr("Transmission Line");
static const QString DeviceGroupLoad = QObject::tr("Load");
static const QString DeviceGroupPE = QObject::tr("Power Electronic Element");
static const QString DeviceGroupMeasur = QObject::tr("Measuring Element");
static const QString DeviceGroupNewEnergy = QObject::tr("New Energy Power Generation");
static const QString DeviceGroupLocalCombine = QObject::tr("Local Combine Element");

// 控制分类
static const QString ControllerGroupFunction = QObject::tr("Function Modules");
static const QString ControllerGroupDataSource = QObject::tr("Data Source Modules");
static const QString ControllerGroupEnd = QObject::tr("End Modules");
static const QString ControllerGroupComputation = QObject::tr("Mathematical Operations Modules");
static const QString ControllerGroupLogicOperation = QObject::tr("Logical Operations Modules");
static const QString ControllerGroupSpecial = QObject::tr("Special Modules");
static const QString ControllerGroupCustomCode = QObject::tr("Local Code Type");
static const QString ControllerGroupCustomBoard = QObject::tr("Local Composition");
// designer
static const QString ControllerGroupDiscrete = QObject::tr("Discrete State Modules");
static const QString ControllerGroupContinue = QObject::tr("Continuous State Modules");

// 当判断条件用
static const QString str_DeviceGroupBus = "Bus Bar";
static const QString str_DeviceGroupElement = "Basic Element";
static const QString str_DeviceGroupMachine = "Motor";
static const QString str_DeviceGroupTransformer = "Transformer";
static const QString str_DeviceGroupLine = "Transmission Line";
static const QString str_DeviceGroupLoad = "Load";
static const QString str_DeviceGroupPE = "Power Electronic Element";
static const QString str_DeviceGroupDC = "Direct Current Element";
static const QString str_DeviceGroupMeasur = "Measuring Element";
static const QString str_DeviceGroupNewEnergy = "New Energy Power Generation";
static const QString str_DeviceGroupLocalCombine = "Local Combine Element";

static const QString str_ControllerGroupFunction = "Function Modules";
static const QString str_ControllerGroupDataSource = "Data Source Modules";
static const QString str_ControllerGroupEnd = "End Modules";
static const QString str_ControllerGroupComputation = "Mathematical Operations Modules";
static const QString str_ControllerGroupLogicOperation = "Logical Operations Modules";
static const QString str_ControllerGroupSpecial = "Special Modules";
static const QString str_ControllerGroupCustomCode = "Local Code Type";
static const QString str_ControllerGroupCustomBoard = "Local Composition";
static const QString str_ControllerGroupDiscrete = "Discrete State Modules";
static const QString str_ControllerGroupContinue = "Continuous State Modules";

// 测量分类
static const QString MeasurerGroupMeasurer = QObject::tr("Measuring module");

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

// 翻译类型
static QString TRANSLATION_TYPE_CH = "zh_CN";
static QString TRANSLATION_TYPE_EN = "en_US";

// 控制、电气插件名称
static const QString controlConfigFileName = "ControlModel";
static const QString deviceConfigFileName = "ElectrialModel";

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
    BasicFunction, // 内建基础函数模块
    Datasource,
    End,
    MathematicalOperations,
    Logicaloperations,
    SpecialFunction,
    Points,
    Other,
    CustomCode,  // 本地代码型
    CustomBoard, // 本地组合型
    Discrete,    // 离散模块
    Continue     // 连续模块

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
    DeviceDC,          // 直流元件
    DeviceNewEnergy,   // 新能源
    DeviceMeasur,      // 测量元件
    DeviceLocalCombine // 本地构造元件

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
    enum GroupDataFrom {
        GroupDataFrom_Config = 0,  // 配置文件
        GroupDataFrom_DataManager, // 数据管理器
    };

    GroupClass() { }

    virtual ~GroupClass() { }

    struct GroupBlockParamater {
        QString prototypeName; // 唯一,与datamanager的prototypename保持一致，关联key
        QString viewName;      // 显示名称
        QVariant viewIcon; // 资源，只要主资源，其余根据主资源加后缀，如果有需求的话。做成共通
        bool display;              // 当前block是否显示
        DisplayType displaytype;   // 显示类型：name/icon
        unsigned int displayorder; // 模块在组里面的显示顺序
    };

public:
    QString groupName;                                 // 对应唯一的组名
    unsigned int groupOrder;                           // 对应组的显示顺序
    GroupDataFrom groupDataSource;                     // 分组数据来源
    QMap<QString, GroupBlockParamater> groupBlocksMap; // 用prototypeName作为这个map的键
    bool groupIsExpand;                                // 分组是否展开
};

typedef QSharedPointer<GroupClass> PGroupClass;

class GroupConfigClass
{
public:
    GroupConfigClass() { }
    ~GroupConfigClass() { }

public:
    QString cfgFileName;                  // 文件名
    QMap<QString, PGroupClass> mapGroups; // key是组名
};
typedef QSharedPointer<GroupConfigClass> PGroupConfigClass;
}

#endif // __GLOBALDEFINITION__

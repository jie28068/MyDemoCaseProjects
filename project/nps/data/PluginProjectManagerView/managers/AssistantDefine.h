#ifndef ASSISTANTDEFINE_H
#define ASSISTANTDEFINE_H

#pragma once

#include <QMainWindow>
#include <QObject>
#include <QSharedPointer>
#include <QString>
class KLProject;
namespace Kcc {
namespace BlockDefinition {
class Model;
class Variable;
}
}

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace AssistantDefine {
static const int ModelDataObjectRole = Qt::UserRole + 800; // widget object
/// @brief 当前目标对象
enum WidgetObject {
    WidgetObject_Invalid,
    WidgetObject_DataManagerTree,   // 数据管理器tree
    WidgetObject_DataManagerTable,  // 数据管理器表格
    WidgetObject_DataManagerCommon, // 数据管理器common
    WidgetObject_NetworkTree,       // 网络模型tree
    WidgetObject_NetworkTable       // 网络模型表格
};
/// @brief log类型，比如新建、删除、拷贝等
enum LogType { LogType_New, LogType_Delete, LogType_Copy, LogType_Export, LogType_Import, LogType_Rename };
static const QStringList NUMBERLIST = QStringList() << "double"
                                                    << "bool"
                                                    << "doubleVector"
                                                    << "doubleComplex"
                                                    << "int"
                                                    << "int8_t"
                                                    << "uint8_t"
                                                    << "int16_t"
                                                    << "uint16_t"
                                                    << "int32_t"
                                                    << "uint32_t"
                                                    << "int64_t"
                                                    << "uint64_t"
                                                    << "float";                   // 数值变量类型
static const QString TITLE_DATAMANAGER = QObject::tr("Data Manager");             // 数据管理器
static const QString TITLE_NETWORKMANAGER = QObject::tr("Network Model Manager"); // 网络模型管理器
// datamanager view
static const QString DATAROOT_NAME = QObject::tr("Database");              // 数据库
static const QString GLOBALLIB_NAME = QObject::tr("Global Library");       // 全局库
static const QString CONTROLMODEL = QObject::tr("Control Model Template"); // 控制模块模板
// project
static const QString LOCALLIB_NAME = QObject::tr("Local Library"); // 本地库
// local lib
static const QString DEVICETYPE_NAME = QObject::tr("Device Type");                 // 设备类型
static const QString CODEMODEL_NAME = QObject::tr("Code Model Template");          // 代码型模板
static const QString COMBINEDMODEL_NAME = QObject::tr("Structure Model Template"); // 构造型模板
static const QString COMPLEXMODEL_NAME = QObject::tr("Compound Model Template");   // 复合型模板
// networkmodel
static const QString NETMODEL_NAME = QObject::tr("Network Model"); // 网络模型
// result
static const QString RESULT_NAME = QObject::tr("Simulation Result");             // 仿真结果
static const QString CURRENTSIMULATION_REALNAME = "Current Simulation";          // 当前仿真
static const QString CURRENTSIMULATION_NAME = QObject::tr("Current Simulation"); // 当前仿真

// networkmanager
static const QString ELECTRICAL_ELEMENTS = QObject::tr("Electrical Elements");       // 电气元件
static const QString MODEL_TEMPLATE = QObject::tr("Model Template");                 // 模块模板
static const QString MODEL_INSTANCE = QObject::tr("Model Instance");                 // 模块实例
static const QString COMPLEXMODEL_TEMPLATE = COMPLEXMODEL_NAME;                      // 复合型模板
static const QString COMPLEXMODEL_INSTANCE = QObject::tr("Compound Model Instance"); // 复合型实例
static const QString CONTROLSYSTEM = QObject::tr("Control System");                  // 控制系统
// menu text
static const QString STR_ACT_ACTIVE = QObject::tr("Active");       // 激活
static const QString STR_ACT_INACTIVE = QObject::tr("Inactivate"); // 取消激活
static const QString STR_ACT_RECORD = QObject::tr("Record");       // 记录

static const QString STR_ACT_UP = QObject::tr("Previous Step");                               // 上一步
static const QString STR_ACT_DOWN = QObject::tr("Next Step");                                 // 下一步
static const QString STR_ACT_REFRESH = QObject::tr("Refresh");                                // 刷新
static const QString STR_ACT_OPEN = QObject::tr("Open");                                      // 打开
static const QString STR_ACT_CUT = QObject::tr("Cut");                                        // 剪切
static const QString STR_ACT_COPY = QObject::tr("Copy");                                      // 复制
static const QString STR_ACT_PASTE = QObject::tr("Paste");                                    // 粘贴
static const QString STR_ACT_NEW = QObject::tr("New");                                        // 新建
static const QString STR_ACT_NEWWS = QObject::tr("New WorkSpace");                            // 新建工作空间
static const QString STR_ACT_NEWPROJ = QObject::tr("New Project");                            // 新建项目
static const QString STR_ACT_NEWBOARD = QObject::tr("New Drawing Board");                     // 新建画板
static const QString STR_ACT_NEWDEVICETYPE = QObject::tr("New Device Type");                  // 新建设备类型
static const QString STR_ACT_NEWCOMPLEXINSTANCE = QObject::tr("New Compound Model Instance"); // 新建复合模型实例
static const QString STR_ACT_NEWBLOCKINSTANCE = QObject::tr("New Model Instance");            // 新建模块实例
static const QString STR_ACT_EDIT = QObject::tr("Edit");                                      // 编辑
static const QString STR_ACT_DELETE = QObject::tr("Delete");                                  // 删除
static const QString STR_ACT_RENAME = QObject::tr("Rename");                                  // 重命名
static const QString STR_ACT_IMPORT = QObject::tr("Import");                                  // 导入
static const QString STR_ACT_EXPORT = QObject::tr("Export");                                  // 导出
static const QString STR_ACT_SETTINGS = QObject::tr("Project Settings");                      // 设置
static const QString STR_ACT_UNDO = QObject::tr("Undo");                                      // 重做
static const QString STR_ACT_REDO = QObject::tr("Redo");                                      // 撤销
static const QString STR_ACT_BATCHMODIFY = QObject::tr("Batch Modify");                       // 批量修改
static const QString STR_ACT_SORT = QObject::tr("Sort");                                      // 排序
static const QString STR_ACT_SORTBYNAME = QObject::tr("Sort By Name");                        // 按名称排序
static const QString STR_ACT_SORTBYTIME = QObject::tr("Sort By Time");                        // 按时间排序

static const QString STR_BOARDTYPE_CONTROLSYSTEM = QObject::tr("Control System");       // 控制系统
static const QString STR_BOARDTYPE_ELECTRICALSYSTEM = QObject::tr("Electrical System"); // 电气系统
static const QString STR_BOARDTYPE_COMPLEXMODEL = COMPLEXMODEL_NAME;                    // 复合型模板
static const QString STR_BOARDTYPE_COMBINEMODEL = COMBINEDMODEL_NAME;                   // 构造型模板
static const QString STR_BOARDTYPE_ELECCOMBINEMODEL =
        QObject::tr("Electrical Structure Model Template"); // 电气构造型模板

// table title
static const QString KEYWORD_TITLE_WSNAME = QObject::tr("WorkSpace Name");                      // 工作空间名称
static const QString KEYWORD_TITLE_WSPATH = QObject::tr("WorkSpace Path");                      // 工作空间路径
static const QString KEYWORD_TITLE_NAME = QObject::tr("Name");                                  // 名称
static const QString KEYWORD_TITLE_COUNT = QObject::tr("Count");                                // 数量
static const QString KEYWORD_TITLE_PARAM = QObject::tr("Parameter");                            // 参数
static const QString KEYWORD_TITLE_CREATETIME = QObject::tr("Create Time");                     // 创建时间
static const QString KEYWORD_TITLE_PROJNAME = QObject::tr("Project Name");                      // 项目名
static const QString KEYWORD_TITLE_PROJPATH = QObject::tr("Project Path");                      // 项目路径
static const QString KEYWORD_TITLE_BOARDNAME = QObject::tr("Drawing Board Name");               // 画板名称
static const QString KEYWORD_TITLE_BOARDTYPE = QObject::tr("Drawing Board Type");               // 画板类型
static const QString KEYWORD_TITLE_BOARDDESCRIPTION = QObject::tr("Drawing Board Description"); // 画板描述

// 仿真结果列：仿真结果名称，仿真步长，仿真时间，工频，是否初始化，数据大小，仿真开始时间
static const QString KEYWORD_TITLE_SIMURESULTNAME = QObject::tr("Simulation Result Name"); // 仿真结果名称
static const QString KEYWORD_TITLE_RUNTIME = QObject::tr("Time of Simulation Run Start");  // 仿真开始运行时刻
static const QString KEYWORD_TITLE_DATASIZE = QObject::tr("Data Size (MB)");               // 数据大小(MB)
// 仿真结果subitem列：画板名称，元件名称，变量名称
static const QString KEYWORD_TITLE_VARIABLENAME = QObject::tr("Variable Name"); // 变量名

// 设备类型列：设备类型名称，设备类型所属，设备类型参数详情，编辑时间。
static const QString KEYWORD_TITLE_DEVICETYPE_NAME = QObject::tr("Device Type Name");   // 设备类型名称
static const QString KEYWORD_TITLE_DEVICETYPE_CLASS = QObject::tr("Device Type Class"); // 设备类型所属
static const QString KEYWORD_TITLE_DEVICETYPE_CLASS_GLOBAL = QObject::tr("Global");     // 全局
static const QString KEYWORD_TITLE_DEVICETYPE_CLASS_LOCAL = QObject::tr("Local");       // 本地
static const QString KEYWORD_TITLE_EDITTIME = QObject::tr("Edit Time");                 // 编辑时间
// 电气元件列：元件名，设备类型，画板名，端口相关（端口：连接的母线，端口：接线端口编号，端口：电压等级，端口：类型）
// 电气参数详情，设备类型参数详情，仿真参数详情，编辑时间。
static const QString KEYWORD_TITLE_ELEMENTNAME = QObject::tr("Element Name"); // 元件名
static const QString KEYWORD_TITLE_PATH = QObject::tr("Path");                // 路径
// 代码型模块固定列：模块名，画板名，输入变量，输出变量，状态变量，参数，编辑时间
static const QString KEYWORD_TITLE_MODELNAME = QObject::tr("Model Name");      // 模块名
static const QString KEYWORD_TITLE_INPUTVAR = QObject::tr("Input Variable");   // 输入变量
static const QString KEYWORD_TITLE_OUTPUTVAR = QObject::tr("Output Variable"); // 输出变量
static const QString KEYWORD_TITLE_STATE = QObject::tr("State Variable");      // 状态变量
// 构造型模板列：元件名，输入变量，输出变量，状态变量，参数，编辑时间。
// 控制画板列：元件名，输出变量，输出变量，状态变量，参数，编辑时间。
// 复合型模板列：插槽，模块原型，输出变量，输出变量，编辑时间。
// 复合模型实例：插槽，元件名，输入变量，输出变量，状态变量，参数，编辑时间。
static const QString KEYWORD_TITLE_SLOT = QObject::tr("Slot");                      // 插槽
static const QString KEYWORD_TITLE_MODELPROTOTYPE = QObject::tr("Model Prototype"); // 模块原型
// 构造型实例列:路径，参数名，参数类型,参数值，参数信息。
// 模块实例：参数名，参数类型,参数值，参数信息。
static const QString KEYWORD_TITLE_PARAM_NAME = QObject::tr("Parameter Name"); // 参数名
static const QString KEYWORD_TITLE_TYPE = QObject::tr("Type");                 // 类型
static const QString KEYWORD_TITLE_VALUE = QObject::tr("Value");               // 值
static const QString KEYWORD_TITLE_DESCRIPTION = QObject::tr("Description");   // 备注信息

static const QString TABLE_NAME_CODEBLOCKS = QObject::tr("Code Model");              // 代码型模块
static const QString TABLE_NAME_COMBINEBOARDBLOCKS = QObject::tr("Structure Model"); // 构造型模块
static const QString TABLE_NAME_COMPLEXBOARDBLOCK = QObject::tr("Compound Model");   // 插槽模块
static const QString TABLE_NAME_CONTROLBOARDBLOCKS = QObject::tr("Control Model");   // 控制型模块
static const QString TABLE_NAME_ELECTRICALBLOCKS = QObject::tr("Electrical Model");  // 电气模块
static const QString TABLE_NAME_DEVICETYPES = DEVICETYPE_NAME;                       // 设备类型

static const QString PROTOTYPE_BUSBAR = QString("Busbar");       // Busbar原型名
static const QString PROTOTYPE_DOTBUSBAR = QString("DotBusbar"); // Busbar原型名
static const QString KEYWORD_ELECPARAM_NAME = QString("NAME");   // 电气参数名称

static const QString ERRORINFO_MODELNULL =
        QObject::tr("The model is empty and cannot be deleted!"); // 模型为空，无法删除!
static const QString ERRORINFO_WSNAMENULL = QObject::tr("The WorkSpace Name cannot be empty!"); // 工作空间名不能为空!
static const QString ERRORINFO_PROJECTNAMENULL = QObject::tr("The Project Name cannot be empty!"); // 项目名不能为空!
static const QString ERRORINFO_SIMURESULTNAMENULL =
        QObject::tr("The Simulation Result Name cannot be empty!"); // 仿真结果名不能为空!
static const QString ERRORINFO_MODELNAMENULL = QObject::tr("The Model Name cannot be empty!"); // 模型名不能为空!
static const QString ERRORINFO_DRAWBOARDRUNNING = QObject::tr("The drawing board is running!"); // 画板正在运行中
static const QString TITLE_TABLESET = QObject::tr("Table Set");                                 // 表格设置
static const QString TITLE_OPENDRAWBOARD = QObject::tr("Open Drawboard");                       // 打开画板
static const QString TITLE_SELECTDIRECCTORY = QObject::tr("Select the directory to export");    // 选择导出的目录
static const QString TITLE_IMPORT_EXAMPLEPROJECT = QObject::tr("Import Example Project");       // 导入示例项目
static const QString TITLE_IMPORT_PROJECT = QObject::tr("Import Project");                      // 导入项目
static const QString TITLE_EXPORT_WORKSPACE = QObject::tr("Export WorkSpace");                  // 导出工作空间
static const QString TITLE_SELECT_EXAMPLEPROJECT = QObject::tr("Please select example project"); // 请选择示例项目
// log类型名
static const QString LOG_TYPE_WORKSPACE = QObject::tr("workspace");                     // 工作空间
static const QString LOG_TYPE_PROJECT = QObject::tr("project");                         // 项目
static const QString LOG_TYPE_DEVICETYPE = QObject::tr("device type");                  // 设备类型
static const QString LOG_TYPE_MODEL = QObject::tr("model");                             // 模型
static const QString LOG_TYPE_SIMULATIONRESULT = QObject::tr("simulation result");      // 仿真结果
static const QString LOG_TYPE_COMPLEXINSTANCE = QObject::tr("compound model instance"); // 复合模型实例
static const QString LOG_TYPE_MODELINSTANCE = QObject::tr("model instance");            // 模型实例

static const int TableCol_Width = 110;
static const int SettingTableDlg_Width = 400;
static const int SettingTableDlg_Height = 600;
static const int TABLE_FREEZE_COLUMN_1 = 1;
static const int TABLE_FREEZE_COLUMN_3 = 3;

// 翻译的电气原型
static const QString TRANSPROTO_SYNCMACHINE = QObject::tr("Synchronous Machine");               // 同步发电机
static const QString TRANSPROTO_ASYNCMACHINE = QObject::tr("AsynchronousMachine");              // 异步电机
static const QString TRANSPROTO_PMSG = QObject::tr("Permanent Magnet Synchronous Generator");   // 永磁同步发电机
static const QString TRANSPROTO_AUTOTF = QObject::tr("AutoTransformer");                        // 自耦变压器
static const QString TRANSPROTO_BOOSTERTF = QObject::tr("BoosterTransformer");                  // 升压变压器
static const QString TRANSPROTO_BUSBAR = QObject::tr("Busbar");                                 // 母线
static const QString TRANSPROTO_CAPACITOR = QObject::tr("Capacitor");                           // 电容
static const QString TRANSPROTO_CIRCUITBREAKER = QObject::tr("Circuit Breaker");                // 断路器
static const QString TRANSPROTO_CPLOAD = QObject::tr("Constant Power Load");                    // 恒功率负载
static const QString TRANSPROTO_CURSOURCE = QObject::tr("Current Source");                      // 电流源
static const QString TRANSPROTO_CURRENTSOURCE2T = QObject::tr("Current Source (two-terminal)"); // 电流源（双端）
static const QString TRANSPROTO_CURTF = QObject::tr("CurrentTransformer");                      // 电流互感器
static const QString TRANSPROTO_DCCURSOURCE = QObject::tr("DC Current Source");                 // 直流电流源
static const QString TRANSPROTO_ExternalElectricalNode = QObject::tr("External Electrical Node"); // 外部电气节点
static const QString TRANSPROTO_DCCURRENTSOURCE2T =
        QObject::tr("DC Current Source (two-terminal)");                       // 直流电流源（双端）
static const QString TRANSPROTO_DCLOAD = QObject::tr("DC_Load");               // 直流负载
static const QString TRANSPROTO_DCMACHINE = QObject::tr("DC_Machine");         // 直流电机
static const QString TRANSPROTO_DCVOLSOURCE = QObject::tr("DC_VoltageSource"); // 直流电压源
static const QString TRANSPROTO_DCVOLSOURCE2T = QObject::tr("DC_VoltageSource_two_terminal"); // 直流电压源（双端）
static const QString TRANSPROTO_DCTODCCONVERTER2 = QObject::tr("DCtoDC_Converter2"); // 直流/直流变换器（双端）
static const QString TRANSPROTO_DIODE = QObject::tr("Diode");                        // 二极管
static const QString TRANSPROTO_DIODECONVERTERV = QObject::tr("Diode Converter Valve"); // 二极管换流阀
static const QString TRANSPROTO_DOTBUSBAR = QObject::tr("Dot Busbar");                  // 点状母线
static const QString TRANSPROTO_EXTERNALGRID = QObject::tr("ExternalGrid");             // 外部电网
static const QString TRANSPROTO_FUSE = QObject::tr("Fuse");                             // 熔断器
static const QString TRANSPROTO_GROUND = QObject::tr("Ground");                         // 地线
static const QString TRANSPROTO_HARMONICFILTER = QObject::tr("HarmonicFilter");         // 谐波滤波器
static const QString TRANSPROTO_IGBTDIODE = QObject::tr("IGBT Diode");   // 绝缘栅双极型晶体管并联二极管
static const QString TRANSPROTO_IGBT = QObject::tr("IGBT");              // 绝缘栅双极型晶体管
static const QString TRANSPROTO_INDUCTOR = QObject::tr("Inductor");      // 电感
static const QString TRANSPROTO_IOUT = QObject::tr("Ammeter");           // 电流表
static const QString TRANSPROTO_LINE = QObject::tr("Transmission Line"); // 线路
static const QString TRANSPROTO_PITYPETRANSMISSIONLINE = QObject::tr("Pi Type Transmission Line"); // Pi型传输线
static const QString TRANSPROTO_LOAD = QObject::tr("Load");                                        // 负载
static const QString TRANSPROTO_NTWTF = QObject::tr("Nonideal Two Winding Transformer"); // 非理想型双绕组变压器
static const QString TRANSPROTO_PWMCONVERTER = QObject::tr("PWM_Converter");             // PWM转换器
static const QString TRANSPROTO_RESISTOR = QObject::tr("Resistor");                      // 电阻
static const QString TRANSPROTO_SCIG = QObject::tr("Squirrel Cage Induction Generator"); // 笼型异步发电机
static const QString TRANSPROTO_SGCIPD =
        QObject::tr("Constant Impedance abc Domain Synchronous Generator");       // 恒定阻抗abc域同步发电机
static const QString TRANSPROTO_SGDQ = QObject::tr("SG_DQ");                      // dq0域同步发电机
static const QString TRANSPROTO_STATICGENERATOR = QObject::tr("StaticGenerator"); // 静止发电机
static const QString TRANSPROTO_STATICVARSYS = QObject::tr("StaticVarSystem");    // 静态无功系统
static const QString TRANSPROTO_STEPVOLREGULATOR = QObject::tr("StepVoltageRegulator"); // 步进电压调节器
static const QString TRANSPROTO_TCSC = QObject::tr("TCSC");                             // 可控串联补偿装置
static const QString TRANSPROTO_3WINDTF = QObject::tr("ThreeWindingTransformer");       // 三绕组变压器
static const QString TRANSPROTO_T = QObject::tr("Thyristor Converter Valve");           // 晶闸管换流阀
static const QString TRANSPROTO_TMPTF = QObject::tr("TmpTransformer");                  // 临时变压器
static const QString TRANSPROTO_2WINDTF = QObject::tr("Two Winding Transformer");       // 双绕组变压器
static const QString TRANSPROTO_UOUT = QObject::tr("Voltmeter");                        // 电压表
static const QString TRANSPROTO_VALVE = QObject::tr("Valve");                           // 电子管
static const QString TRANSPROTO_VOLSOURCECONVERTER = QObject::tr("Voltage Source Converter"); // 电压源换流器
static const QString TRANSPROTO_VOLSOURCE = QObject::tr("Voltage Source");                    // 电压源
static const QString TRANSPROTO_VOLSOURCE2T = QObject::tr("Voltage Source (two-terminal)");   // 电压源（双端）
static const QString TRANSPROTO_DC_VOLTAGESOURCE = QObject::tr("DC Voltage Source");          // 直流电压源
static const QString DC_VOLTAGESOURCE_TWO_TERMINAL =
        QObject::tr("DC Voltage Source (two-terminal)");                   // 直流电压源（双端）
static const QString TRANSPROTO_VOLTF = QObject::tr("VoltageTransformer"); // 电压互感器
static const QString TRANSPROTO_WINDGENERATOR = QObject::tr("Direct Driven Wind Turbine Generator"); // 直驱风力发电机
static const QString TRANSPROTO_DFIG = QObject::tr("Doubly Fed Induction Generator"); // 双馈异步风力发电机
static const QString TRANSPROTO_PHOTOVSOURCE = QObject::tr("Photovoltaic Source");    // 光伏源
static const QString TRANSPROTO_PVARRAY = QObject::tr("Photovoltaic Cell Array");     // 光伏电池阵列
static const QString TRANSPROTO_VSCSW =
        QObject::tr("Voltage Source Converter (Switch Mode)"); // 电压源换流器（开关模型）
static const QString TRANSPROTO_VSCAVG =
        QObject::tr("Voltage Source Converter (Average Value Mode)");      // 电压源换流器（平均值模型）
static const QString TRANSPROTO_SHUNTFILTER = QObject::tr("Shunt Filter"); // 并联滤波器
static const QString TRANSPROTO_SERIESREACTOR = QObject::tr("Series Reactor");                      // 串联电抗器
static const QString TRANSPROTO_ENERGYSTORAGEBATTERY = QObject::tr("Energy Storage Battery");       // 储能电池
static const QString TRANSPROTO_SINGLELINECONVERTER = QObject::tr("Single Line Converter");         // 单线变换器
static const QString TRANSPROTO_CONTROLLEDVOLTAGESOURCE = QObject::tr("Controlled Voltage Source"); // 受控电压源
static const QString TRANSPROTO_CONTROLLEDVOLTAGESOURCE_TWO_TERMINAL =
        QObject::tr("Controlled Voltage Source (two-terminal)"); // 受控电压源（双端）
static const QString TRANSPROTO_CONTROLLEDCURRENTSOURCE = QObject::tr("Controlled Current Source"); // 受控电流源
static const QString TRANSPROTO_CONTROLLEDCURRENTSOURCE_TWO_TERMINAL =
        QObject::tr("Controlled Current Source (two-terminal)"); // 受控电流源（双端）
static const QString TRANSPROTO_EXTERNALELECTRICALNODE = QObject::tr("External Electrical Node"); // 外部电气节点
static const QString TRANSPROTO_THYRISTOR = QObject::tr("Thyristor");                             // 晶闸管
static const QString TRANSPROTO_MMC = QObject::tr("Modular Multilevel Converter"); // 模块化多电平换流器
static const QString TRANSPROTO_BATTERY = QObject::tr("Battery");                  // 电池
static const QString TRANSPROTO_BOOSTAVG = QObject::tr("Boost(Average Value Mode)"); // 直流升压模块（平均值模型）
static const QString TRANSPROTO_BUCKAVG = QObject::tr("Buck(Average Value Mode)"); // 直流降压模块（平均值模型）
static const QString TRANSPROTO_DCDC_AVG =
        QObject::tr("DC_DC_Converter(Average Value Mode)"); // 直流转换模块（平均值模型）
static const QString TRANSPROTO_MACHINE = QObject::tr("Wound-rotor Asynchronous Machine");          // 绕组式电机
static const QString TRANSPROTO_GROUNDBUS = QObject::tr("Ground Bus");                              // 接地母线
static const QString TRANSPROTO_ThreeWindingTransformer = QObject::tr("Three Winding Transformer"); // 三绕组变压器
static const QString TRANSPROTO_SquirrelCageAsynchronousMachine =
        QObject::tr("Squirrel-cage Asynchronous Machine"); // 鼠笼式异步电机
static const QString TRANSPROTO_PMSM = QObject::tr("Permanent Magnet Synchronous Machine");//永磁同步电机

static const QString TRANSPROTO_ABS = QObject::tr("abs");                     // 绝对值函数
static const QString TRANSPROTO_ACOS = QObject::tr("acos");                   // 反余弦函数
static const QString TRANSPROTO_ADD = QObject::tr("Add");                     // 加法器
static const QString TRANSPROTO_AFLIPFLOP = QObject::tr("aflipflop");         // 开关翻转函数
static const QString TRANSPROTO_AND = QObject::tr("AND");                     // 逻辑与
static const QString TRANSPROTO_ASIN = QObject::tr("asin");                   // 反正弦函数
static const QString TRANSPROTO_ATAN = QObject::tr("atan");                   // 反正切函数
static const QString TRANSPROTO_ATAN2 = QObject::tr("atan2");                 // 反正切函数2
static const QString TRANSPROTO_BED = QObject::tr("BackwardEulerDerivative"); // 基于后向欧拉法的微分模块
static const QString TRANSPROTO_BEDLAG = QObject::tr("BackwardEulerDerivativeLag"); // 基于后向欧拉法的微分滞后模块
static const QString TRANSPROTO_BEI = QObject::tr("BackwardEulerInertia"); // 基于后向欧拉法的惯性环节模块
static const QString TRANSPROTO_BEILIM = QObject::tr("BackwardEulerInertiaLimiter"); // 惯性模块
static const QString TRANSPROTO_BEINT = QObject::tr("BackwardEulerIntegral"); // 基于后向欧拉法的积分模块
static const QString TRANSPROTO_BEINTLIM =
        QObject::tr("BackwardEulerIntegralLimit"); // 基于后向欧拉法的积分模块(带限幅)
static const QString TRANSPROTO_BELLAG = QObject::tr("BackwardEulerLeadLag"); // 基于后向欧拉法的超前滞后模块
static const QString TRANSPROTO_BEPI = QObject::tr("BackwardEulerPI");        // PI控制模块
static const QString TRANSPROTO_BEPID = QObject::tr("BackwardEulerPID");      // 基于后向欧拉法的PID
static const QString TRANSPROTO_BIAS = QObject::tr("Bias");                   // 为输入添加偏差
static const QString TRANSPROTO_BCREATOR = QObject::tr("BusCreator");         // 输入总线模块
static const QString TRANSPROTO_BSELECTOR = QObject::tr("BusSelector");       // 输出总线模块
static const QString TRANSPROTO_CEIL = QObject::tr("ceil");                   // 向上取整函数
static const QString TRANSPROTO_COMPAREC = QObject::tr("CompareC");           // 与常数比较
static const QString TRANSPROTO_COMPAREZ = QObject::tr("CompareZ");           // 与零比较
static const QString TRANSPROTO_COMPLEXTOMA = QObject::tr("Complex_to_Magnitude_Angle");   // TODO
static const QString TRANSPROTO_COMPLEXTOREALIMG = QObject::tr("Complex_to_RealImag");     // TODO
static const QString TRANSPROTO_CONVERT = QObject::tr("convert");                          // Convert
static const QString TRANSPROTO_CONSTANT = QObject::tr("Constant");                        // 常数模块
static const QString TRANSPROTO_COS = QObject::tr("cos");                                  // 余弦函数
static const QString TRANSPROTO_COSH = QObject::tr("cosh");                                // 双曲余弦函数
static const QString TRANSPROTO_DATATYPECONVERSION = QObject::tr("DataTypeConversion");    // TODO
static const QString TRANSPROTO_DELAY = QObject::tr("delay");                              // 延迟函数
static const QString TRANSPROTO_DEMUX = QObject::tr("DeMux");                              // DeMux
static const QString TRANSPROTO_DIFFERENCE = QObject::tr("Difference");                    // Difference
static const QString TRANSPROTO_DISCRETEDERIVATIVE = QObject::tr("Discrete_Derivative");   // Discrete_Derivative
static const QString TRANSPROTO_DISCRETETIMEINT = QObject::tr("Discrete_Time_Integrator"); // Discrete_Time_Integrator
static const QString TRANSPROTO_DOTPRODUCT = QObject::tr("Dot_product");                   // TODO
static const QString TRANSPROTO_E = QObject::tr("e");                                      // e函数
static const QString TRANSPROTO_ELECINTERFACE = QObject::tr("ElectricalInterface");        // 电气接口
static const QString TRANSPROTO_EXP = QObject::tr("exp");                                  // e为底指数函数
static const QString TRANSPROTO_FACQUAD = QObject::tr("factorialQuadratic");               // 二次函数(阶乘形式)
static const QString TRANSPROTO_FLIPFLOP = QObject::tr("flipflop");                        // 逻辑翻转函数
static const QString TRANSPROTO_FLOOR = QObject::tr("floor");                              // 向下取整函数
static const QString TRANSPROTO_FMU = QObject::tr("FMU");                                  // 功能实体模型单元
static const QString TRANSPROTO_FOURIER = QObject::tr("Fourier");                          // 傅里叶
static const QString TRANSPROTO_FRAC = QObject::tr("frac");                                // 取小数函数
static const QString TRANSPROTO_FROMSPREADSHEET = QObject::tr("From_SpreadSheet");         // TODO
static const QString TRANSPROTO_FROM = QObject::tr("From");                                // From
static const QString TRANSPROTO_FROMFILE = QObject::tr("FromFile");                        // 读文件
static const QString TRANSPROTO_GAIN = QObject::tr("Gain");                                // 增益模块
static const QString TRANSPROTO_GENQUAD = QObject::tr("generalQuadratic");                 // 二次函数(一般形式)
static const QString TRANSPROTO_GOTO = QObject::tr("Goto");                                // Goto
static const QString TRANSPROTO_GRADLIMCONST = QObject::tr("gradlimConst");                // 梯度限制函数
static const QString TRANSPROTO_IN = QObject::tr("In");                                    // 输入模块
static const QString TRANSPROTO_INTLIMCLEAR = QObject::tr("IntegralLimitClear"); // 后向欧拉积分模块(带限幅和清零)
static const QString TRANSPROTO_INTLIMINPUT = QObject::tr("IntegralLimitInput"); // 基于后向欧拉法的积分模块(带输入限幅)
static const QString TRANSPROTO_INTLIM = QObject::tr("IntegratorLimiter"); // 积分限幅模块
static const QString TRANSPROTO_LASTVALUE = QObject::tr("lastvalue");      // 上一次有效迭代输入函数
static const QString TRANSPROTO_LEADLAGLIM = QObject::tr("LeadLagLimiter"); // 基于欧拉法的超前滞后模块(带限幅)
static const QString TRANSPROTO_LIM = QObject::tr("lim");                   // 限幅函数
static const QString TRANSPROTO_LIMFIX = QObject::tr("limfix");   // 超限提示函数(初始化阶段或常量)
static const QString TRANSPROTO_LIMITER = QObject::tr("limiter"); // 限幅
static const QString TRANSPROTO_LIMITS = QObject::tr("limits");   // 超限提示函数
static const QString TRANSPROTO_LN = QObject::tr("ln");           // 自然对数函数
static const QString TRANSPROTO_LOG10 = QObject::tr("log10");     // 10为底的对数函数
static const QString TRANSPROTO_LOOKUPTABLE1D = QObject::tr("Lookuptable_1D");              // 一维插值
static const QString TRANSPROTO_LOOKUPTABLE2D = QObject::tr("Lookuptable_2D");              // 二维插值
static const QString TRANSPROTO_MAGNITUDEA2CPX = QObject::tr("Magnitude_Angle_to_Complex"); // TODO
static const QString TRANSPROTO_MAX = QObject::tr("max");                                   // 最大值函数
static const QString TRANSPROTO_MEMORY = QObject::tr("Memory");                             // Memory
static const QString TRANSPROTO_MIN = QObject::tr("min");                                   // 最小值函数
static const QString TRANSPROTO_MINMAXRUNRESET = QObject::tr("MinMax_Running_Resettable");  // TODO
static const QString TRANSPROTO_MODULO = QObject::tr("modulo");                             // 模函数
static const QString TRANSPROTO_MOVINGAVG = QObject::tr("movingavg");                       // 滑动平均函数
static const QString TRANSPROTO_MULT = QObject::tr("mult");                                 // 乘法模块
static const QString TRANSPROTO_MULTIPORTSWITCH = QObject::tr("MultiportSwitch");           // 多通道选择
static const QString TRANSPROTO_MUX = QObject::tr("Mux");                                   // Mux
static const QString TRANSPROTO_NOT = QObject::tr("NOT");                                   // 逻辑非
static const QString TRANSPROTO_NULL = QObject::tr("Null");                                 // 空模块
static const QString TRANSPROTO_OPERATOR = QObject::tr("Operator");                         // 关系运算模块
static const QString TRANSPROTO_OR = QObject::tr("OR");                                     // 逻辑或
static const QString TRANSPROTO_OUT = QObject::tr("Out");                                   // 输出模块
static const QString TRANSPROTO_PI = QObject::tr("pi");                                     // 圆周率
static const QString TRANSPROTO_PICDRO = QObject::tr("picdro");                             // 逻辑升降函数
static const QString TRANSPROTO_PICTRCONST = QObject::tr("picontrolConst");                 // π控制器
static const QString TRANSPROTO_POLYNOMIAL = QObject::tr("Polynomial");                     // TODO
static const QString TRANSPROTO_POW = QObject::tr("pow");                                   // 幂函数
static const QString TRANSPROTO_PULSEGEN = QObject::tr("PulseGenerator");                   // 脉冲发生器
static const QString TRANSPROTO_RAMP = QObject::tr("Ramp");                                 // 上升下降信号
static const QString TRANSPROTO_RATELIM = QObject::tr("RateLimiter");                       // 变化率限速器
static const QString TRANSPROTO_REALIMG2CPX = QObject::tr("Real_Imag_to_Complex");          // TODO
static const QString TRANSPROTO_REPEATINGSEQUENCE = QObject::tr("Repeating_Sequence");      // 随机周期函数
static const QString TRANSPROTO_ROUND = QObject::tr("round");                               // 就近取整函数
static const QString TRANSPROTO_SAWTOOTHWAVES = QObject::tr("Sawtooth_waves");              // 锯齿波
static const QString TRANSPROTO_SCOPE = QObject::tr("Scope");                               // 波形显示模块
static const QString TRANSPROTO_SELECT = QObject::tr("select");                             // 选择函数
static const QString TRANSPROTO_SELFIX = QObject::tr("selfix");           // 选择函数(限初始化使用)
static const QString TRANSPROTO_SIGN = QObject::tr("Sign");               // 指示输入的符号
static const QString TRANSPROTO_SIMULABCOMM = QObject::tr("SL_Comm");     // SL_Comm
static const QString TRANSPROTO_SIMULABINPUT = QObject::tr("SL_Input");   // SL_Input
static const QString TRANSPROTO_SIMULABOUTPUT = QObject::tr("SL_Output"); // SL_Output
static const QString TRANSPROTO_SIN = QObject::tr("sin");                 // 正弦函数
static const QString TRANSPROTO_SINEWAVEFUNCTION =
        QObject::tr("Sinewave_Function"); // 使用外部信号作为时间源来生成正弦波
static const QString TRANSPROTO_SINEWAVE = QObject::tr("SineWave");                    // 正弦波发生器
static const QString TRANSPROTO_SINH = QObject::tr("sinh");                            // 双曲正弦函数
static const QString TRANSPROTO_SLOT = QObject::tr("Slot");                            // 插槽
static const QString TRANSPROTO_SQR = QObject::tr("sqr");                              // 平方函数
static const QString TRANSPROTO_QSRT = QObject::tr("sqrt");                            // 平方根函数
static const QString TRANSPROTO_STEP = QObject::tr("Step");                            // 阶跃函数
static const QString TRANSPROTO_STRAIGHTLINEAR = QObject::tr("straightLinear");        // 线性函数(直线)
static const QString TRANSPROTO_SUM = QObject::tr("Sum");                              // 加法器
static const QString TRANSPROTO_SWITCH = QObject::tr("Switch");                        // 开关函数
static const QString TRANSPROTO_SYSTRANSFER = QObject::tr("SL_SysTransfer");           // SL_SysTransfer
static const QString TRANSPROTO_TAN = QObject::tr("tan");                              // 正切函数
static const QString TRANSPROTO_TANH = QObject::tr("tanh");                            // 双曲正切函数
static const QString TRANSPROTO_TIME = QObject::tr("time");                            // 仿真时间函数
static const QString TRANSPROTO_TOFILE = QObject::tr("ToFile");                        // 写文件
static const QString TRANSPROTO_TRANSFUNCDISCRETE = QObject::tr("TransFunc_Discrete"); // 传递函数离散化
static const QString TRANSPROTO_TRANSFUNC = QObject::tr("TransFunc");                  // 连续积分传递函数
static const QString TRANSPROTO_TRANSFUNC2DDF = QObject::tr("TransFunc2DiscreteDiffFunc"); // 传递函数离散化转差分方程
static const QString TRANSPROTO_TRANSFUNC2DSS = QObject::tr("TransFunc2DiscreteStateSpace"); // 传递函数转离散状态方程
static const QString TRANSPROTO_TRANSPORTDELAY = QObject::tr("Transport_Delay"); // 延迟模块(带初始值)
static const QString TRANSPROTO_TRIGONOMETRICFUNCTION = QObject::tr("Trigonometric_Function"); // TODO
static const QString TRANSPROTO_TRUNC = QObject::tr("trunc");                                  // 取整函数
static const QString TRANSPROTO_2PI = QObject::tr("twopi");                                    // 二倍圆周率
static const QString TRANSPROTO_UNARYMINUS = QObject::tr("Unary_Minus");                       // 对输入求反
static const QString TRANSPROTO_WEIGHTEDSTM = QObject::tr("Weighted_Sample_Time_Math");        // TODO
static const QString TRANSPROTO_WIND = QObject::tr("Wind");                                    // 风模拟器

static const QString ACCESSNAME_COLSET = "ACCESSNAME_COLSET";               // 列设置
static const QString ACCESSNAME_FILTERCOL = "ACCESSNAME_FILTERCOL";         // 筛选列
static const QString ACCESSNAME_FILTERCONTENT = "ACCESSNAME_FILTERCONTENT"; // 筛选内容
static const QString ACCESSNAME_OPENBOARDNAME = "ACCESSNAME_OPENBOARDNAME"; // 画板打开画面-画板名
static const QString ACCESSNAME_OPENBOARDTYPE = "ACCESSNAME_OPENBOARDTYPE"; // 画板打开画面-类型
// function
/// @brief 把模型中的变量加载到数据字典
/// @param model
/// @param cpxBoardName 实例连接的复合模型实例名
/// @param recursive 是否需要包括model下层的
void addModelVarToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &cpxBoardName = "",
                       bool recursive = true);
/// @brief 删除模型数据字典数据
/// @param model
/// @param cpxBoardName
void deleteModelVarToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &cpxBoardName = "");
/// @brief 重命名数据字典中的变量
/// @param model 模型
/// @param oldModelName
void renameModelVarToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &oldModelName);
/// @brief 把项目结果保存加入数据字典
/// @param project
void addProjectVarToDictionary(QSharedPointer<KLProject> project);
/// @brief 模型是否需要加载变量到数据字典
/// @param model 模型
/// @param project 项目,为空则为当前项目
/// @return
bool modelNeedAddToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, QSharedPointer<KLProject> project = nullptr);

QString getDictVar(const QString &dictVarStart, const QString &var);
/// @brief 获取log string
/// @param logtype 日志类型
/// @param typeName 操作类型名
/// @param contentName 类型具体内容
/// @param sts 状态，成功，失败
/// @return
const QString getOuputLog(const LogType &logtype, const QString &typeName, const QString &contentName, bool sts,
                          const QString &newName = "");

QMainWindow *getMainWindow();
}

#endif
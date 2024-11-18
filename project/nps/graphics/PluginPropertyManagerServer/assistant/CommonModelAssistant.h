#ifndef COMMONMODELASSISTANT_H
#define COMMONMODELASSISTANT_H

#pragma once
#include "PropertyTableModel.h"

namespace Kcc {
namespace BlockDefinition {
class Model;
class VariableGroup;
class Variable;
class DeviceModel;
class ControlBlock;
class ElectricalBlock;
class SlotBlock;
class DrawingBoardClass;
class ElecBoardModel;
class CombineBoardModel;
class ControlBoardModel;
class ComplexBoardModel;
class ElectricalContainerBlock;
class BlockPorts;
}
}
class KLProject;
namespace CMA {
/// @brief 保存数据
enum SAVEDATA { SAVEDATA_Value = 0, SAVEDATA_CheckedVariable, SAVEDATA_All };
/// @brief 控制模块操作
enum Operation {
    Operation_EditCtrProto,
    Operation_EditCtrInstance,
    Operation_NewCtrInstance,
    Operation_EditSlotInstance
};
/// @brief 获取模型范围，仅项目，仅内建，所有
enum ProtoRange { ProtoRange_All, ProtoRange_OnlyProject, ProtoRange_OnlyBuildIn };
static const QString EXCLUED_NOTNUMBER = QString("EXCLUED_NOTNUMBER"); // 剔除非数值的变量
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
                                                    << "float"; // 数值变量类型

static const QString LogTag_ElectricalComponent = QObject::tr("Electrical Components"); // 电气元件
static const QString LogTag_ControlComponent = QObject::tr("Control Elements");         // 控制元件

// 通用
static const QString BLOCK_TAB_NAME_BOARD = QObject::tr("Module Properties");     // 模块属性
static const QString BLOCK_TAB_NAME_PORT = QObject::tr("Port Information");       // 端口信息
static const QString BLOCK_TAB_NAME_BASIC_DATA = QObject::tr("Basic Parameters"); // 基本参数

// 电气
static const QString BLOCK_TAB_NAME_TIDE_PARAM = QObject::tr("Power Flow Parameters"); // 潮流参数
static const QString BLOCK_TAB_NAME_DEV_TYPE = QObject::tr("Device Type");             // 设备类型
static const QString BLOCK_TAB_NAME_ELEC_PARAM = QObject::tr("Equipment Parameters");  // 设备参数
static const QString BLOCK_TAB_NAME_ELEC_SIMU = QObject::tr("Simulation Settings");    // 仿真设置
static const QString BLOCK_TAB_NAME_ELEC_INIT = QObject::tr("Initial Value Settings"); // 初始值设置
static const QString BLOCK_TAB_NAME_ELEC_SAVE = QObject::tr("Save Results");           // 结果保存
static const QString BLOCK_TAB_NAME_PLANT_MODEL = QObject::tr("Model");                // 模型
static const QString BLOCK_TAB_NAME_FEATURECURVE = QObject::tr("Feature Curves");      // 特性曲线
static const QString BLOCK_TAB_NAME_CTRLSIGNAL = QObject::tr("Control Signals");       // 控制信号

// 控制
static const QString BLOCK_TAB_NAME_INPUT = QObject::tr("Input Variables");   // 输入变量
static const QString BLOCK_TAB_NAME_OUTPUT = QObject::tr("Output Variables"); // 输出变量
// #ifdef COMPILER_PRODUCT_DESIGNER
static const QString BLOCK_TAB_NAME_DISCRETE_STATE = QObject::tr("Discrete State Variables"); // 离散状态变量
// #endif
// #ifdef COMPILER_PRODUCT_SIMUNPS
// static const QString BLOCK_TAB_NAME_DISCRETE_STATE =
//         QObject::tr("State Variable"); // 离散状态变量，在SimuNPS就叫状态变量
// #endif
static const QString BLOCK_TAB_NAME_CONTINUE_STATE = QObject::tr("Continue State Variables"); // 连续状态变量
static const QString BLOCK_TAB_NAME_INTERNAL = QObject::tr("Internal Variables");             // 内部变量
static const QString BLOCK_TAB_NAME_CTRL_PARAM = QObject::tr("Parameters");                   // 参数
static const QString BLOCK_TAB_NAME_CTRL_CODE = QObject::tr("Code");                          // 代码
static const QString BLOCK_TAB_NAME_FMU_OVERVIEW = QObject::tr("FMU Overview");               // FMU概览
// 其他
static const QString BLOCK_TAB_NAME_TYPE = QObject::tr("Type");                      // 类型
static const QString BLOCK_TAB_NAME_SLOT = QObject::tr("Slot ");                     // 插槽
static const QString BLOCK_TAB_NAME_VARIABLE = QObject::tr("Variables");             // 变量
static const QString BLOCK_TAB_NAME_ELEC_CONTAINER = QObject::tr("Elec Component");  // 选择电气元件
static const QString BLOCK_TAB_NAME_SELECT_INPUT = QObject::tr("Input Variables");   // 选择输入变量
static const QString BLOCK_TAB_NAME_SELECT_OUTPUT = QObject::tr("Output Variables"); // 选择输出变量

static const QString BLOCK_VARIABLE_UNCHECKED = QObject::tr("Unchecked"); // 未选中
static const QString BLOCK_VARIABLE_CHECKED = QObject::tr("Checked");     // 选中

// 画板
static const QString BOARD_TAB_NAME_INFO = QObject::tr("Basics");                     // 基础
static const QString BOARD_TAB_CONNECTOR = QObject::tr("Connecting Line");            // 连接线
static const QString BOARD_TAB_RIGHTGRAPHICS = QObject::tr("Legend");                 // 图例
static const QString BOARD_TAB_COVER = QObject::tr("Cover");                          // 封面
static const QString BOARD_TAB_NAME_SCRIPT = QObject::tr("Code");                     // 代码
static const QString BOARD_TAB_NAME_SLOT_DEFINITION = QObject::tr("Slot Definition"); // 插槽定义

static const QString TITLE_NEW_COMPLEX_INSTANCE = QObject::tr("Create Compound Model Instance"); // 新建复合模型实例
static const QString TITLE_EDIT_COMPLEX_INSTANCE = QObject::tr("Edit Compound Model Instance"); // 编辑复合模型实例
static const QString TITLE_NEW_BLOCK_INSTANCE = QObject::tr("Create Block Instance");           // 新建模块实例
static const QString TITLE_EDIT_SLOT_INSTANCE = QObject::tr("Edit Slot");                       // 编辑插槽

// 控制模块变量表格title 第一列为路径，第二列为变量名字，第三列为变量值，第四列为备注信息
static const QString HEADER_PATH = QObject::tr("Path");                  // 路径
static const QString HEADER_VARNAME = QObject::tr("Variable Name");      // 变量名
static const QString HEADER_VARVALUE = QObject::tr("Variable Value");    // 变量值
static const QString HEADER_PARAMNAME = QObject::tr("Parameter Name");   // 参数名
static const QString HEADER_PARAMVALUE = QObject::tr("Parameter Value"); // 参数值
static const QString HEADER_DESCRIPTION = QObject::tr("Description");    // 备注
static const QString ATTRIBUTE_NAME = QObject::tr("Attribute Name");     // 属性名
static const QString ATTRIBUTE_VALUE = QObject::tr("Attribute Value");   // 属性值

static const QString SELECT_TYPE = QObject::tr("Select Type");                      //"选择类型"
static const QString REMOVE_TYPE = QObject::tr("Remove Type");                      //"移除类型"
static const QString ELEC_DRAWBOARD_NAME = QObject::tr("Electrical Drawing Board"); // 电气画板
static const QString ELEC_ELEMENT_PROTOTYPENAME = QObject::tr("Element Type");      // 元件类型
static const QString ELEC_ELEMENT_NAME = QObject::tr("Element Name");               // 元件名称

static const QString HEADER_LINE_COMPONENT_NAME = QObject::tr("Line Component Name"); // 线路元件名称
static const QString HEADER_GRID_NODE_NAME = QObject::tr("Grid Node Name");           // 电网节点名称

// 用户手册连接
static const QString USERMANUAL_NEWDEVICE = "New Equipment Type";
// /SimuNPS用户手册/4 电气网络构建/4.2 设备类型/4.2.5 编辑设备类型 .html
static const QString USERMANUAL_EDITDEVICE = "Edit Device Type";
// /SimuNPS用户手册/3 画板/3.1 画板使用/3.1.1 新建画板.html
static const QString USERMANUAL_NEWDRAWBOARD = "New Artboard";
// /SimuNPS用户手册/3 画板/3.1 画板使用/3.1.6 编辑画板.html
static const QString USERMANUAL_EDITDRAWBOARD = "Editing the Artboard";
// /SimuNPS用户手册/3 画板/3.2 画板功能/3.2.2 设置.html
static const QString USERMANUAL_SETTINGS = "Settings";
// /SimuNPS用户手册/8 仿真运行/8.8 潮流计算.html
static const QString USERMANUAL_POWERFLOWCLC = "Load flow calculation";
// /SimuNPS用户手册/9 仿真结果处理/9.3 当前仿真.html
static const QString USERMANUAL_CURSIMU = "Current Simulation";

QList<QSharedPointer<Kcc::BlockDefinition::Variable>>
getVarGroupList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                const QStringList &excludekeys = QStringList());
QStringList getVarGroupCheckList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName);
QList<CustomModelItem> getPropertyModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                const QStringList &groupNamelist, bool readOndy = false,
                                                const QString &path = "",
                                                const QStringList &excludekeys = QStringList(), bool recursive = true);
QList<CustomModelItem> getPropertyModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                const QString &groupName, bool readOndy = false,
                                                const QString &path = "",
                                                const QStringList &excludekeys = QStringList(), bool recursive = true);
bool saveCustomModeListDataToModel(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                   const QList<CustomModelItem> &listdata, const QStringList &groupNamelist,
                                   SAVEDATA savedata = SAVEDATA_Value);
bool saveCustomModeListDataToModel(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                   const QList<CustomModelItem> &listdata, const QString &groupName,
                                   SAVEDATA savedata = SAVEDATA_Value);
// private
bool saveRealBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                       const QString &keyword, const CustomModelItem &item, SAVEDATA savedata = SAVEDATA_Value);
bool isPort(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &keyword);

bool checkNameValid(const QString &name, QSharedPointer<Kcc::BlockDefinition::Model> pmodel, QString &errorinfo);
/// @brief 检查画板名是否重名
/// @param newName
/// @param errorinfo
/// @return
bool checkBoardNameValid(const QString &newName, QString &errorinfo);
/// @brief 该模型是否为画板
/// @param model
/// @return
bool isBoardModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
// 获取原型对应的model
QSharedPointer<Kcc::BlockDefinition::Model> getModelByPrototype(const QString &prototype,
                                                                ProtoRange protorange = ProtoRange_All);
QList<QSharedPointer<Kcc::BlockDefinition::Model>> getProjectModels(const int &modelType = -1);
bool destoryInstance(QSharedPointer<Kcc::BlockDefinition::Model> instance);
bool saveModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
// private
QSharedPointer<KLProject> getCurProject();
bool saveProjectModelByUUID(const QString &modelUUID);
QSharedPointer<Kcc::BlockDefinition::Model> getProjectProtoModel(QSharedPointer<Kcc::BlockDefinition::Model> instance);

bool isEqualO(const double &value);
} // end namespace CMA

#endif
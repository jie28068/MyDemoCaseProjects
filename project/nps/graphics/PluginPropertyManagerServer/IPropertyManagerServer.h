#ifndef IPROPERTYMANAGERSERVER_H
#define IPROPERTYMANAGERSERVER_H

#include <QDateTime>
#include <QDialog>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>

// 服务组的名称
#define SERVER_GROUP_PROPERTYMANAGERSERVER_NAME "PropertyManager"
// 服务接口名称
#define SERVER_INTERFACE_PROPERTYMANAGERSERVER_NAME "IPropertyManagerServer"

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
}
}

namespace Kcc {
struct SimuParams;
}
namespace Kcc {
namespace SimulationManager {
struct RunStepConfigParams;
} // namespace SimulationManager

}
class QtVariantProperty;
class ICanvasScene;
class CanvasContext;
class SourceProxy;

#define ATTRIBUTE_MINIMUM "minimum"
#define ATTRIBUTE_MAXIMUM "maximum"
#define ATTRIBUTE_SINGLESTEP "singleStep"
#define ATTRIBUTE_DECIMALS "decimals"
#define ATTRIBUTE_REGEXP "regExp"
#define ATTRIBUTE_CONSTRAINT "constraint"
#define ATTRIBUTE_ENUMNAMES "enumNames"
#define ATTRIBUTE_ENUMICONS "enumIcons"
#define ATTRIBUTE_FLAGNAMES "flagNames"

namespace Kcc {
namespace PropertyManager {
namespace PMKEY {
static const QString MODEL_OLD_NAME = "MODEL_OLD_NAME";
static const QString MODEL_NAME = "MODEL_NAME";
static const QString MODEL_UUID = "MODEL_UUID";
static const QString MODEL_TYPE = "MODEL_TYPE";
static const QString BOARD_TRIGGER_MODIFY = "BOARD_TRIGGER_MODIFY";

static const QString BoardTypeElectrical = QObject::tr("Electrical System");                    // 电气系统
static const QString BoardTypeControl = QObject::tr("Control System");                          // 控制系统
static const QString BoardTypeCombine = QObject::tr("Structure Model Template");                // 构造型模板
static const QString BoardTypeComplex = QObject::tr("Compound Model Template");                 // 复合型模板
static const QString BoardTypeElecCombine = QObject::tr("Electrical Structure Model Template"); // 电气构造型模板
}

static const QString PFSET_KWD_BASEMVA = QString("baseMVA");
static const QString PFSET_KWD_OUTPUTCLCRESULT = QString("outputClcResult");
static const QString PFSET_KWD_BASEMVAAC = QString("baseMVAac");
static const QString PFSET_KWD_BASEMVADC = QString("baseMVAdc");
static const QString PFSET_KWD_POl = QString("pol");
static const QString PFSET_KWD_POWERFLOW_TYPE = QString("POWERFLOW_TYPE");
static const QString PFSET_KWD_MPOP_PF_ALG = QString("mpop_pf_alg");
static const QString PFSET_KWD_MPOP_PF_CURRENT_BALANCE = QString("mpop_pf_current_balance");
static const QString PFSET_KWD_MPOP_VERBOSE = QString("mpop_verbose");
static const QString PFSET_KWD_MPOP_OUT_ALL = QString("mpop_out_all");
static const QString PFSET_KWD_MPOP_OUT_SYS_SUM = QString("mpop_out_sys_sum");
static const QString PFSET_KWD_MPOP_OUT_AREA_SUM = QString("mpop_out_area_sum");
static const QString PFSET_KWD_MPOP_OUT_BUS = QString("mpop_out_bus");
static const QString PFSET_KWD_MPOP_OUT_BRANCH = QString("mpop_out_branch");
static const QString PFSET_KWD_MPOP_OUT_GEN = QString("mpop_out_gen");
static const QString PFSET_KWD_MPOP_OUT_LIM_ALL = QString("mpop_out_lim_all");
static const QString PFSET_KWD_MPOP_OUT_LIM_V = QString("mpop_out_lim_v");
class IPropertyManagerServer
{
public:
    enum InstanceType { InstanceType_Block, InstanceType_Complex };
    /// @brief 查看与修改所有模块属性
    /// @param pModel 画板model,如果没有画板，就传模块的本身
    /// @param blockuuid 画板中模块的uuid，如果是模块本身，uuid传空
    /// @param readOnly 是否只读
    /// @return true是修改了属性，否则为false
    virtual bool ShowBlockProperty(QSharedPointer<Kcc::BlockDefinition::Model> pModel, bool readOnly = false) = 0;
    /// @brief 查看与修改画板注释模块属性
    /// @param pSourceProxy 画板注释模块
    /// @return true是修改了属性，否则为false
    virtual bool ShowCommentProperty(QSharedPointer<SourceProxy> pSourceProxy,
                                     QSharedPointer<CanvasContext> canvasContext = nullptr) = 0;
    /// @brief 新建设备类型
    /// @param prototypename 原型名
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::DeviceModel> CreateDeviceModel(const QString &prototypename) = 0;
    /// @brief 编辑设备类型
    /// @param devicemodel
    /// @param readOnly 是否只读
    /// @return
    virtual bool EditDeviceModel(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicemodel,
                                 bool readOnly = false) = 0;
    /// @brief 新建复合模型实例
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::Model> CreateComplexInstance(const QString &prototype = "") = 0;
    /// @brief 编辑复合模型实例
    /// @param model 模型
    /// @param readOnly 是否只读
    /// @return
    virtual bool EditComplexInstance(QSharedPointer<Kcc::BlockDefinition::Model> model, bool readOnly = false) = 0;
    /// @brief 创建模块实例
    /// @param model 待创建实例的模块原型
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::Model> CreateBlockInstance(const QString &prototype = "") = 0;
    /// @brief 新建画板
    /// @param initBoardType 指定画板类型，为空则不指定
    /// @param popDlg 打开窗口true,否则直接创建
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> CreateDrawBoard(const QString &initBoardType = "",
                                                                                    bool popDlg = true) = 0;
    /// @brief 画板属性修改。包括新建，修改等
    /// @param pCanvasContext 画板
    /// @param tapname tap页名，为空则是第一个tap
    /// @param readOnly 是否只读
    virtual bool ShowDrawBoardProperty(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pDrawBoard,
                                       const QString tabname = "", bool readOnly = false) = 0;
    /// @brief 显示图层属性
    /// @param pCanvasScene 画板scene
    /// @param tabname tap页名，为空则是第一个tap
    virtual void ShowLayerProperty(QSharedPointer<ICanvasScene> pCanvasScene, const QString tabname = "") = 0;

    /// @brief 仿真参数设置
    /// @param params 仿真参数结构体
    /// @param NpsOrCad SimuNPS或SimuCAD
    virtual void ShowSimuParamProperty(Kcc::SimuParams &params, Kcc::SimulationManager::RunStepConfigParams &stepParams,
                                       int simuState, int NpsOrCad) = 0;

    /// @brief 项目设置
    virtual void ShowProjectProperty() = 0;

    /// @brief 打开字体设置对话框
    /// @param font 初始化字体
    /// @return 选择的字体
    virtual QFont ShowFontSetDialog(const QFont font) = 0;

    /// @brief 发送通知
    /// @param code enum定义的值。
    /// @param parammap 定义的参数map
    virtual void sendNotify(const unsigned int code, const QMap<QString, QVariant> &parammap) = 0;

    /// @brief 警告弹窗//TODO 待删除
    /// @param tips 警告内容
    virtual void openWarningDlg(const QString &tips) = 0;

    /// @brief 清理所有的属性缓存
    virtual void Clear() = 0;
    /// @brief 检查模型是否重名
    /// @param newname 新名称
    /// @param model 模型
    /// @param errorinfo 错误信息，如果返回false
    /// @return
    virtual bool checkNameValid(const QString &newname, QSharedPointer<Kcc::BlockDefinition::Model> model,
                                QString &errorinfo) = 0;
    /// @brief 选择模块
    /// @param titleStr 弹窗标题
    /// @param selectType 选择模块类型
    /// @param filterVar 筛选内容
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::Model>
    selectBlock(const QString &titleStr, const int &selectType,
                QSharedPointer<Kcc::BlockDefinition::Variable> filterVar = nullptr) = 0;

    // /// @brief 打开当前仿真设置画面
    // /// @param curProjectName 当前工程名，比如CAD，NPS不同工程显示不同
    // virtual void ShowCurrentVariables(const QString &curProjectName) = 0;

    // /// @brief 显示当前潮流计算参数设置对话框
    // virtual void ShowPowerFlowCalcSet() = 0;

    virtual ~IPropertyManagerServer() {};
};

typedef QSharedPointer<IPropertyManagerServer> PIPropertyManagerServer;

enum {
    /* 通知元器件属性刷新，这种情况在电气参数属性页点编辑之后，需要通知画板更新
    参数 boardName 画板名称  QString
             blockUUID block的UUID  QString
    */
    Notify_BlockPropertyChanged,
    Notify_DrawingBoardCreate,
    Notify_DrawingBoardModified,
    /*
    通知元器件rule发生变化
    */
    Notify_BlockRuleModify,

    /* 通知画板scene刷新状态 */
    Notify_SceneUpdateState,
    /* 元件仿真输出参数变更，参数别名变更 */
    Notify_BlockSimuOutParamAliasChanged,
    // 新建模块实例
    Notify_CreateBlockInstance,
    // 新建复合模型实例
    Notify_CreateComplexInstance,
    // 新建设备类型
    Notify_CreateDeviceType,
    // 编辑设备类型
    Notify_EditDeviceType
};

}
}

#endif // IPROPERTYMANAGERSERVER_H
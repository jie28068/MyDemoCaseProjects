#pragma once

#include "CommonWrapper.h"
#include "CoreLib/ServerBase.h"
#include "CoreLib/ServerManager.h"
#include "IPropertyManagerServer.h"
#include "KLWidgets/KMessageBox.h"
#include "qtvariantproperty.h"

#define SERVER_VERSION 1

using namespace Kcc::PropertyManager;

class PropertyDialog;
class KCustomDialog;
class ControlBlockPropertyEditor;
class ElectricalBlockPropertyEditor;
class IconBlockProperty;
class ICanvasScene;
class SourceProxy;

struct PropertyWrapperData {
    PropertyWrapperData() : wrapper(nullptr), dialog(nullptr), customdlg(nullptr), customOkBtn(nullptr) { }

    CommonWrapper *wrapper;
    PropertyDialog *dialog;
    KCustomDialog *customdlg;
    QPushButton *customOkBtn;
    QPushButton *customHelpBtn;
};

class PropertyManagerServer : public QObject, public ServerBase, public IPropertyManagerServer
{
    Q_OBJECT
public:
    PropertyManagerServer();
    virtual ~PropertyManagerServer(void);

    virtual void Init();

    virtual void UnInit();

    /// @brief 查看与修改所有模块属性
    /// @param pModel 画板model,如果没有画板，就传模块的本身
    /// @param blockuuid 画板中模块的uuid，如果是模块本身，uuid传空
    /// @param readOnly 是否只读
    /// @return true是修改了属性，否则为false
    virtual bool ShowBlockProperty(QSharedPointer<Kcc::BlockDefinition::Model> pModel, bool readOnly = false);

    /// @brief 查看与修改画板注释模块属性
    /// @param pSourceProxy 画板注释模块
    /// @return true是修改了属性，否则为false
    virtual bool ShowCommentProperty(QSharedPointer<SourceProxy> pSourceProxy,
                                     QSharedPointer<CanvasContext> canvasContext = nullptr);

    /// @brief 新建设备类型
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::DeviceModel> CreateDeviceModel(const QString &prototypename);
    /// @brief 编辑设备类型
    /// @param devicemodel
    /// @param readOnly 是否只读
    /// @return
    virtual bool EditDeviceModel(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicemodel, bool readOnly = false);

    /// @brief 新建复合模型实例
    /// @param prototype 指定复合模型原型名，为空不指定
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::Model> CreateComplexInstance(const QString &prototype = "");
    /// @brief 编辑复合模型实例
    /// @param model 模型
    /// @param readOnly 是否只读
    /// @return
    virtual bool EditComplexInstance(QSharedPointer<Kcc::BlockDefinition::Model> model, bool readOnly = false);
    /// @brief 创建模块实例
    /// @param model 待创建实例的模块原型
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::Model> CreateBlockInstance(const QString &prototype = "");
    /// @brief 新建画板
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> CreateDrawBoard(const QString &initBoardType = "",
                                                                                    bool popDlg = true);
    /// @brief 画板属性修改。包括新建，修改等
    /// @param pCanvasContext 画板
    /// @param isnewBoard 是否新建画板
    /// @param tabname tap页名，为空则是第一个tap
    /// @param readOnly 是否只读
    virtual bool ShowDrawBoardProperty(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pDrawBoard,
                                       const QString tabname = "", bool readOnly = false);

    /// @brief 显示图层属性
    /// @param pCanvasScene 画板scene
    /// @param tabname tap页名，为空则是第一个tap
    virtual void ShowLayerProperty(QSharedPointer<ICanvasScene> pCanvasScene, const QString tabname = "");

    /// @brief 仿真参数设置
    /// @param params 仿真参数结构体
    /// @param RunStepConfigParams 单步仿真参数
    /// @param simuState 仿真状态
    /// @param NpsOrCad SimuNPS或SimuCAD
    virtual void ShowSimuParamProperty(Kcc::SimuParams &params, Kcc::SimulationManager::RunStepConfigParams &stepParams,
                                       int simuState, int NpsOrCad);

    /// @brief 项目设置
    virtual void ShowProjectProperty();

    /// @brief 打开字体设置对话框
    /// @param font 初始化字体
    /// @return 选择的字体
    virtual QFont ShowFontSetDialog(const QFont font);

    /// @brief 发送通知
    /// @param code enum定义的值。
    /// @param parammap 定义的参数map
    virtual void sendNotify(const unsigned int code, const QMap<QString, QVariant> &parammap);

    /// @brief 警告弹窗//lbTODO待删除
    /// @param tips 警告内容
    virtual void openWarningDlg(const QString &tips);

    /// @brief 清理所有的属性缓存
    virtual void Clear();

    /// @brief 检查模型是否重名
    /// @param newname 新名称
    /// @param model 模型
    /// @param errorinfo 错误信息，如果返回false
    /// @return
    virtual bool checkNameValid(const QString &newname, QSharedPointer<Kcc::BlockDefinition::Model> model,
                                QString &errorinfo);
    /// @brief 选择模块
    /// @param titleStr 弹窗标题
    /// @param selectType 选择模块类型
    /// @param filterVar 筛选内容
    /// @return
    virtual QSharedPointer<Kcc::BlockDefinition::Model>
    selectBlock(const QString &titleStr, const int &selectType,
                QSharedPointer<Kcc::BlockDefinition::Variable> filterVar = nullptr);
    // virtual QtVariantProperty *AddGroupProperty(CommonWrapper *wrapper, const QString &strName);

    // virtual QtVariantProperty *AddVariantProperty(CommonWrapper *wrapper, QVariant::Type type,
    //                                               const QString &strName, bool bReadonly = false);

    // virtual QtVariantProperty *AddEnumProperty(CommonWrapper *wrapper, const QString &strName,
    //                                            bool bReadonly = false);

    // virtual QtVariantProperty *AddFlagProperty(CommonWrapper *wrapper, const QString &strName,
    //                                            bool bReadonly = false);

    void refreshBlockRule(QMap<QString, QVariant> params);

    void notifySceneUpdateState(QString boardName);

private:
    void createPropertyWrapperData(CommonWrapper *wrapper, const QString &dlgmemorytype = "",
                                   const QString &tabname = "", const QString &shortcutPath = "",
                                   const QString &blockProto = "");

    void removePropertyWrapper(CommonWrapper *wrapper);

    void showPropertyDialog(CommonWrapper *wrapper, const QString &dlgmemorytype = "", const QString &tabname = "",
                            const QString &shortcutPath = "", const QString &blockProto = "");

    // 判断电气模块是否显示属性页
    bool isNeedShowElectricalProperty(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block);

    // 获取其它控制模块的变量名，用于变量名的更新
    QSet<QString> getOtherCtrlBlockVarNames(QSharedPointer<Kcc::BlockDefinition::ControlBlock> curBlock);

    // 设置电气元件关联的子画板信息（通过电气接口属性页设置获取并设置）
    void
    setElecComponentSubBoardInfo(QSharedPointer<Kcc::BlockDefinition::ElectricalContainerBlock> elecInterfaceBlock);

private slots:
    void onDialogHelpBtnClicked();
    void onDialogOkBtnClicked();

    void onCloseWrapper();

    // 接收仿真引擎消息
    void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param);

private:
    // Dialog是否弹出，弹出Dialog时，修改属性值不调用ProperyWrapper的onValueChanged
    bool m_bIsDialogPopup;

    // 画板使用开始仿真运行。运行时，属性窗口不可编辑
    bool m_bIsDrawingBoardRunning;

    QMap<CommonWrapper *, PropertyWrapperData> m_mapPropertyData;

    /// 保存当前属性窗口关联的画板信息
    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> m_pCurrentDrawingBoardInfo;

    KCustomDialog *m_pCustomDlg;
};

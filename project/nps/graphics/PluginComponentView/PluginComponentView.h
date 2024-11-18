#ifndef PLUGINCOMPONENTVIEW_H
#define PLUGINCOMPONENTVIEW_H

#include "AdjustListViewWidget.h"
#include "ComponentListModel.h"
#include "ConfigInfoManager.h"
#include "ContextMenu.h"
#include "CoreLib/DocHelper.h"
#include "CoreLib/Module.h"
#include "GlobalDefinition.h"
#include "KLProject/KLProject.h"
#include "KLWidgets/KMessageBox.h"
#include "ModelManagerServer/IModelManagerServer.h"
#include "MyToolBox.h"
#include "PluginModuleWizardServer/IPluginModuleWizardServer.h"
#include "ToolsLib/Json/json.h"
#include "plugincomponentview_global.h"
#include "server/CodeManagerServer/ICodeManagerServer.h"
#include "server/PluginComponentServer/IPluginComponentServer.h"
#include "server/PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
#include "server/PropertyManagerServer/IPropertyManagerServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include "server/UILayout/IUIMainLayoutServer.h"
#include "ui_PluginComponentView.h"
#include <QFileInfo>
#include <QMap>
#include <QPointer>
#include <QWidget>

KCC_USE_NAMESPACE_COREMANUI
using namespace Kcc::UIMainLayout;
using namespace Kcc::PluginComponent;
using namespace Kcc::PluginGraphicModeling;
using namespace Kcc::CodeManager;
using namespace Kcc::ModelManager;
using namespace Kcc::ProjectManager;
using namespace Kcc::PluginWizard;
using namespace Kcc::DocHelper;

class PluginComponentServer;

// 每个被注册的模块都必须继承Module
class PluginComponentView : public QWidget, public Module
{
    Q_OBJECT
    // 每个模块都必须使用此宏
    DEFINE_MODULE
public:
    // 参数为模块节点名称
    PluginComponentView(QString strName);
    ~PluginComponentView();

    virtual void init(KeyValueMap &params);
    virtual void unInit(KeyValueMap &saveParams);

    ComponentInfo getGroupComponentInfo(GetDataType type = GetDataType_all);
    QPixmap getComponentIcon(const QString &prototypename, const int &blocktype, bool bcombinedBlock = false);

    void createLocalCodePrototype();
    void deleteLocalCodePrototype(const QString &prototype);
    void editLocalCodePrototype(const QString &prototype);
    void updateView();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *e) override;
    virtual QSize sizeHint() const override { return QSize(340, 400); }

private slots:
    // 代码型交互菜单响应
    void onAddCodeComponent();
    void onImportCodeComponent();
    // 删除模块
    void onRemoveModule(const QString &strName);
    /// @brief 修改控制模块
    /// @param strName
    void onModifyControlComponent(const QString &strName);
    // 组合型交互菜单响应
    void onAddBoardComponent();
    void onModifyBoardComponent(const QString &strName);
    // 通用交互菜单响应
    void onSwitchView();
    void onRefreshView();
    /// 新增内建控制模块
    void onAddBuildInComponent(int nClassification);
    void onExpandAll();
    void onCollapseAll();

    void onAdjustListView();
    void onAddComponentToBoard(const QString &strPrototypeName);
    void onCreateComponentInstance(const QString &strPrototypeName);

    void onRecieveProjectMngMsg(unsigned int code,
                                const NotifyStruct &param); // 接受数据管理服务发送的消息
    void onRecieveBoardMsg(unsigned int code, const NotifyStruct &param);

    void onTreeItemMoved(QStandardItem *item);

    void onToolBoxItemGroupChanged(const QMap<int, QVariant> &mapGroupRoleTemplate);

    /// @brief 新增电气向导
    void onAddElcBuildInComponent(int nClassification);
    /// @brief 修改电气向导
    void onChangeElcBuildInComponent(const QString &strPrototypeName);
    /// @brief 分组是否展开
    /// @param isExpand 
    void onExpandChangedSave(const QString &groupName,const bool  &isExpand);
public slots:
    void onShowHelp(const QString &strPrototypeName);

private:
    // 初始化各种需要用到的服务
    void initServer();
    void initComponentData();
    void initControllerGroup();
    void initDeviceGroup();
    void initComponentIcon();
    QPixmap drawTextIcon(const QString &textstr, const int &blocktype, bool bcombinedBlock = false);
    QList<PModel> getControlPrototype();
    bool getCodePrototype(const QString &name, PControlBlock prototype);
    bool destroyCodePrototype(const QString &name);
    bool modifyCodePrototype(const QString &name, PControlBlock prototype);

    void removeBlcok(const QString &name);
    // 绑定右键菜单
    void bindMenu(const QString &m_strModuleName);

    QString getGroupName(const int &nClassification);

    void saveBuiltInModel(const PModel &block, const int &nClassification);

private:
    enum controlModelType { ProjectCode, otherModel };

private:
    Ui::Form ui;
    PluginComponentServer *m_pComponentServer;
    // 服务智能指针，必须在unInit前释放
    PICoreManUILayoutServer m_pUIServer;     // UI服务接口
    PIModelManagerServer m_pModelServer;     // 模型管理服务接口
    PIProjectManagerServer m_pProjectServer; // 项目管理服务接口

    Kcc::PropertyManager::PIPropertyManagerServer m_pPropertyManagerServer;
    PIPluginGraphicModelingServer m_pDrawingBoardServer; // 画板服务
    PICodeManagerServer m_pCodeManagerServer;            // 代码编辑编译
    Kcc::PluginWizard::PIPluginModuleWizardServer m_pPluginModuleWizardServer;

    QString m_strModuleName; // 框架配置文件里面的 module
                             // 名称，如果一个类实例化多次，就会需要用到，不同的实例可能会有不同的个性处理
    QString m_strProjectName;    // 框架配置文件里面的 projectname，不同工程可能会有不同处理
    Global::Category m_category; // 对应m_strModuleName枚举分类

    MyToolBox *m_pToolBox;
    BaseListModel *m_pToolBoxModel;
    ContextMenuManager *m_pContextMenuManager;

    PConfigInfoManager m_pCfgInfoManager;
    PGroupConfigClass m_pConfigInfo;
    QMap<QString, QMap<int, QPixmap>> m_componentIconMap;
};

#endif // PLUGINCOMPONENTVIEW_H

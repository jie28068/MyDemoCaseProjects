#ifndef PLUGINModuleWIZARD_H
#define PLUGINModuleWIZARD_H

#include "CoreLib/Module.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PluginModuleWizardServer.h"
#include "TableDelegate/ComboxDelegate.h"
#include "TableDelegate/DeviceComboxDelegate.h"
#include "TableDelegate/ElecComboxDelegate.h"
#include "Wizard.h"

class PluginModuleWizard : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginModuleWizard(QString plugin_name);
    ~PluginModuleWizard();

    enum controlModelType { ProjectCode, otherModel };

    virtual void init(KeyValueMap &params) override;
    virtual void unInit(KeyValueMap &params) override;
    /// @brief  新增控制模块向导
    /// @param bIsBuildIn 是否内建
    /// @return
    PModel showNewWizardDialog(const QString &path, QWidget *parent = nullptr);
    /// @brief 修改控制向导
    /// @param model
    /// @return
    bool showChangeWizardDialog(PModel model, const QString &path, QWidget *parent = nullptr);

    /// @brief 新增电气接口模块对话
    PModel showNewElecWizardDialog(QWidget *parent = nullptr);
    /// @brief 修改电气向导
    /// @param str
    bool showChangeElecWizardDialog(PModel model, QWidget *parent = nullptr);

    /// @brief 新建电气设备类型
    PModel showNewDeviceWizardDialog(QWidget *parent = nullptr);
    /// @brief 修改电气设备类型
    /// @param str
    bool showChangeDeviceWizardDialog(PModel model, QWidget *parent = nullptr);

private:
    ComboxDelegate *m_comboxDelegate;
};

#endif // !PLUGINModuleWIZARD_H

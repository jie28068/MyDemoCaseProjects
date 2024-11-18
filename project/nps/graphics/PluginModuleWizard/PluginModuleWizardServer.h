#ifndef PLUGINMODULEWIZARDSERVER_H
#define PLUGINMODULEWIZARDSERVER_H

#include "CoreLib/ServerBase.h"
#include "PluginModuleWizardServer/IPluginModuleWizardServer.h"
#define SERVER_VERSION 1

using namespace Kcc;
// 插件wizard的服务命名空间
using namespace Kcc::PluginWizard;

class PluginModuleWizard;

class PluginModuleWizardServer : public ServerBase, public IPluginModuleWizardServer
{
public:
    // ServerBase
    void Init() override;
    void UnInit() override;

    PluginModuleWizardServer(PluginModuleWizard *pluginModuleWizard);
    virtual ~PluginModuleWizardServer();
    /// @brief 修改控制向导
    virtual PModel NewWizardDialog(const QString &path, QWidget *parent = nullptr) override;
    /// @brief 修改控制向导
    virtual bool ChangeWizardDialog(PModel model, const QString &path, QWidget *parent = nullptr) override;
    /// @brief 新增电气向导
    virtual PModel NewElecWizardDialog(QWidget *parent = nullptr) override;
    /// @brief 修改电气向导
    virtual bool ChangeElecWizardDialog(PModel model, QWidget *parent = nullptr) override;
    /// @brief 新增电气设备类型向导
    virtual PModel NewDeviceWizardDialog(QWidget *parent = nullptr) override;
    /// @brief 修改电气设备类型向导
    /// @param strName
    virtual bool ChangeDeviceWizardDialog(PModel model, QWidget *parent = nullptr) override;

private:
    PluginModuleWizard *m_PluginModuleWizard; // 向导插件
};

#endif

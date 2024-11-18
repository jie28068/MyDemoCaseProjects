#include "PluginModuleWizard.h"
#include "PluginModuleWizardServer.h"
#include "WizardServerMng.h"
void PluginModuleWizardServer::Init()
{
    WizardServerMng::getInstance().init();
}

void PluginModuleWizardServer::UnInit()
{
    WizardServerMng::getInstance().unInit();
}

PluginModuleWizardServer::PluginModuleWizardServer(PluginModuleWizard *pluginModuleWizard)
{
    m_PluginModuleWizard = pluginModuleWizard;
}

PluginModuleWizardServer::~PluginModuleWizardServer() { }

PModel PluginModuleWizardServer::NewWizardDialog(const QString &path, QWidget *parent)
{
    return m_PluginModuleWizard->showNewWizardDialog(path, parent);
}

bool PluginModuleWizardServer::ChangeWizardDialog(PModel model, const QString &path, QWidget *parent)
{
    return m_PluginModuleWizard->showChangeWizardDialog(model, path, parent);
}

PModel PluginModuleWizardServer::NewElecWizardDialog(QWidget *parent)
{
    return m_PluginModuleWizard->showNewElecWizardDialog(parent);
}

bool PluginModuleWizardServer::ChangeElecWizardDialog(PModel model, QWidget *parent)
{
    return m_PluginModuleWizard->showChangeElecWizardDialog(model, parent);
}

PModel PluginModuleWizardServer::NewDeviceWizardDialog(QWidget *parent)
{
    return m_PluginModuleWizard->showNewDeviceWizardDialog(parent);
}

bool PluginModuleWizardServer::ChangeDeviceWizardDialog(PModel model, QWidget *parent)
{
    return m_PluginModuleWizard->showChangeDeviceWizardDialog(model, parent);
}

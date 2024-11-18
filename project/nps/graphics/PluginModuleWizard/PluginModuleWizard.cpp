#include "PluginModuleWizard.h"
#include "KLWidgets/KCustomDialog.h"
#include "WizardServerMng.h"
#include <QTranslator>

REG_MODULE_BEGIN(PluginModuleWizard, "", "PluginModuleWizard")
REG_MODULE_END(PluginModuleWizard)

USE_LOGOUT_("PluginModuleWizard");
PluginModuleWizard::PluginModuleWizard(QString plugin_name) : Kcc::Module(Module_Type_Normal, plugin_name)
{
    // 注册服务
    PropertyMap map;
    map.insert("name", plugin_name);
    RegServer<IPluginModuleWizardServer>(new PluginModuleWizardServer(this), map);
}

PluginModuleWizard::~PluginModuleWizard() { }

void PluginModuleWizard::init(KeyValueMap &params) { }

void PluginModuleWizard::unInit(KeyValueMap &params) { }

PModel PluginModuleWizard::showNewElecWizardDialog(QWidget *parent)
{
    Wizard *wizard = new Wizard(Wizard::ModelType::ElectricalMd);
    KCustomDialog dlg(
            wizard->windowTitle(), wizard, KBaseDlgBox::StandardButton::NoButton, KBaseDlgBox::StandardButton::NoButton,
            parent == nullptr ? dynamic_cast<QWidget *>(WizardServerMng::getInstance().m_pUIServerIF->GetMainUI())
                              : parent);
    dlg.setContentsMargins(1, 1, 1, 1);
    dlg.setHiddenButtonGroup(true);
    dlg.resize(800, 600);
    dlg.memoryGeometry(NPS::DLGMEMORY_WIZARD);
    connect(wizard, SIGNAL(finished(int)), &dlg, SLOT(done(int)));

    if (QDialog::Accepted == dlg.exec()) {
        return wizard->getElecBlock();
    }
    return nullptr;
}

bool PluginModuleWizard::showChangeElecWizardDialog(PModel model, QWidget *parent)
{
    Wizard *wizard = new Wizard(Wizard::ModelType::ElectricalMd, model);
    KCustomDialog dlg(
            wizard->windowTitle(), wizard, KBaseDlgBox::StandardButton::NoButton, KBaseDlgBox::StandardButton::NoButton,
            parent == nullptr ? dynamic_cast<QWidget *>(WizardServerMng::getInstance().m_pUIServerIF->GetMainUI())
                              : parent);
    dlg.setContentsMargins(1, 1, 1, 1);
    dlg.setHiddenButtonGroup(true);
    dlg.resize(800, 600);
    dlg.memoryGeometry(NPS::DLGMEMORY_WIZARD);
    connect(wizard, SIGNAL(finished(int)), &dlg, SLOT(done(int)));
    if (QDialog::Accepted == dlg.exec()) {
        return true;
    }
    return false;
}

PModel PluginModuleWizard::showNewDeviceWizardDialog(QWidget *parent)
{
    Wizard *wizard = new Wizard(Wizard::ModelType::DeviceModelMd);
    KCustomDialog dlg(
            wizard->windowTitle(), wizard, KBaseDlgBox::StandardButton::NoButton, KBaseDlgBox::StandardButton::NoButton,
            parent == nullptr ? dynamic_cast<QWidget *>(WizardServerMng::getInstance().m_pUIServerIF->GetMainUI())
                              : parent);
    dlg.setContentsMargins(1, 1, 1, 1);
    dlg.setHiddenButtonGroup(true);
    dlg.resize(800, 600);
    dlg.memoryGeometry(NPS::DLGMEMORY_WIZARD);
    connect(wizard, SIGNAL(finished(int)), &dlg, SLOT(done(int)));

    if (QDialog::Accepted == dlg.exec()) {
        return wizard->getDeviceBlock();
    }
    return nullptr;
}

bool PluginModuleWizard::showChangeDeviceWizardDialog(PModel model, QWidget *parent)
{
    Wizard *wizard = new Wizard(Wizard::ModelType::DeviceModelMd, model);
    KCustomDialog dlg(
            wizard->windowTitle(), wizard, KBaseDlgBox::StandardButton::NoButton, KBaseDlgBox::StandardButton::NoButton,
            parent == nullptr ? dynamic_cast<QWidget *>(WizardServerMng::getInstance().m_pUIServerIF->GetMainUI())
                              : parent);
    dlg.setContentsMargins(1, 1, 1, 1);
    dlg.setHiddenButtonGroup(true);
    dlg.resize(800, 600);
    dlg.memoryGeometry(NPS::DLGMEMORY_WIZARD);
    connect(wizard, SIGNAL(finished(int)), &dlg, SLOT(done(int)));
    if (QDialog::Accepted == dlg.exec()) {
        return true;
    }
    return false;
}

PModel PluginModuleWizard::showNewWizardDialog(const QString &path, QWidget *parent)
{
    Wizard *wizard = new Wizard(path);
    KCustomDialog dlg(
            wizard->windowTitle(), wizard, KBaseDlgBox::StandardButton::NoButton, KBaseDlgBox::StandardButton::NoButton,
            parent == nullptr ? dynamic_cast<QWidget *>(WizardServerMng::getInstance().m_pUIServerIF->GetMainUI())
                              : parent);
    dlg.setContentsMargins(1, 1, 1, 1);
    dlg.setHiddenButtonGroup(true);
    dlg.resize(800, 600);
    dlg.memoryGeometry(NPS::DLGMEMORY_WIZARD);
    connect(wizard, SIGNAL(finished(int)), &dlg, SLOT(done(int)));

    if (QDialog::Accepted == dlg.exec()) {
        return wizard->getControlBlock();
    }
    return nullptr;
}

bool PluginModuleWizard::showChangeWizardDialog(PModel model, const QString &path, QWidget *parent)
{
    Wizard *wizard = new Wizard(model, path);
    KCustomDialog dlg(
            wizard->windowTitle(), wizard, KBaseDlgBox::StandardButton::NoButton, KBaseDlgBox::StandardButton::NoButton,
            parent == nullptr ? dynamic_cast<QWidget *>(WizardServerMng::getInstance().m_pUIServerIF->GetMainUI())
                              : parent);
    dlg.setContentsMargins(1, 1, 1, 1);
    dlg.setHiddenButtonGroup(true);
    dlg.resize(800, 600);
    dlg.memoryGeometry(NPS::DLGMEMORY_WIZARD);
    connect(wizard, SIGNAL(finished(int)), &dlg, SLOT(done(int)));
    if (QDialog::Accepted == dlg.exec()) {
        return true;
    }
    return false;
}

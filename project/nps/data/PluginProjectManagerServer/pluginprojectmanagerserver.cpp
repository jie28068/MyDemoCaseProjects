#include "pluginprojectmanagerserver.h"
#include "CoreLib/ServerManager.h"
#include "KLWidgets/KMessageBox.h"
#include "QPHelper.h"
#include "UILayout/ICoreMainUIServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <GlobalConfigs.h>

using namespace Kcc::CoreManUI_Layout;

REG_MODULE_BEGIN(PluginProjectManagerServer, "", "PluginProjectManagerServer")
REG_MODULE_END(PluginProjectManagerServer)

USE_LOGOUT_("PluginProjectManagerServer")

PluginProjectManagerServer::PluginProjectManagerServer(QString strName) : Module(Module_Type_Normal, strName)
{
    projectManagerServer = new ProjectManagerServer();
    RegServer<IProjectManagerServer>(projectManagerServer);

    m_bLanguageChanged = false;

    m_openMonitorPanelAction = new QAction(this);
    m_switchLanguage = new QAction(this);
    QObject::connect(m_openMonitorPanelAction, SIGNAL(triggered()), SLOT(onOpenMonitorPanel()));
    QObject::connect(m_switchLanguage, SIGNAL(triggered()), SLOT(onSwitchLanguage()));
    QObject::connect(projectManagerServer, SIGNAL(openMonitor()), SLOT(onOpenMonitorPanel()));
}

PluginProjectManagerServer::~PluginProjectManagerServer() { }

void PluginProjectManagerServer::init(KeyValueMap &params)
{
    if (params.contains("SimuDataSaveFormat")) {
        gConfSet("SimuDataSaveFormat", params["SimuDataSaveFormat"]);
    }

    auto pUIServer = RequestServer<Kcc::CoreManUI_Layout::ICoreManUILayoutServer>();
    if (pUIServer != nullptr) {
        pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerServer", "openMonitorPanel",
                                               m_openMonitorPanelAction);
        pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerServer", "switchLanguage", m_switchLanguage);
    }
}

void PluginProjectManagerServer::unInit(KeyValueMap &saveParams)
{
    Module::unInit(saveParams);
}

void PluginProjectManagerServer::onSwitchLanguage()
{
    // // 切换语言前关闭所有工作区画板,若有未保存画板提示是否保存
    // PICoreManUILayoutServer m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    // if (m_pUIServer) {
    //     m_pUIServer->GetMainUI()->DeleteWorkareaWidgets();
    // }

    QString tipinfo = tr("Language switching requires a reboot to take effect. Do you want to reboot now?");
    if (KMessageBox::question(tipinfo, KMessageBox::Yes | KMessageBox::No) == KMessageBox::Yes) {
        // 通知数据管理器视图
        NotifyStruct notify;
        notify.code = IPM_Notify_SwitchLanguage;
        projectManagerServer->emitNotify(notify);

        // 语言设置，请参考setLanguage说明
        QString language = QPHelper::getLanguage();
        auto pUIServer = RequestServer<Kcc::CoreManUI_Layout::ICoreManUILayoutServer>();
        if (pUIServer) {
            pUIServer->GetMainUI()->DeleteWorkareaWidget(tr("Monitor"));
        }
        if (language == "zh_CN") {
            QLocale locale(QLocale::English, QLocale::UnitedStates);
            QPHelper::setLanguage(locale.name());
        } else {
            QLocale locale(QLocale::Chinese, QLocale::China);
            QPHelper::setLanguage(locale.name());
        }
        gConfSet("SimuNPS_Restart", QVariant(true));
        qApp->exit();
    }
}

void PluginProjectManagerServer::onOpenMonitorPanel()
{
    QMap<QString, QVariant> param;
    param[WORKAREA_TITLE] = tr("Monitor");
    param[WORKAREA_TAB_DO_NOT_RESTORE] = true;
    QString dir = projectManagerServer->GetCurrentProjectAbsoluteDir();
    if (!dir.isEmpty()) {
        param["PanelFilePath"] = dir + "/MonitorPanel.xml";
    }

    auto pUIServer = RequestServer<Kcc::CoreManUI_Layout::ICoreManUILayoutServer>();
    if (pUIServer != nullptr) {
        pUIServer->GetMainUI()->CreateWorkarea("MonitorPanel", param);
    }
}

#include "PluginGISView.h"
#include "CoreLib/ServerManager.h"
#include "GISServerManager.h"

#include <QAction>
#include <QSurfaceFormat>
#include <QWebEngineSettings>

REG_MODULE_BEGIN(PluginGISView, "", "PluginGISView")
REG_MODULE_END(PluginGISView)

USE_LOGOUT_("PluginPluginGISView")

PluginGISView::PluginGISView(QString plugin_name) : Kcc::Module(Module_Type_Normal, plugin_name) { }

PluginGISView::~PluginGISView() { }

void PluginGISView::init(KeyValueMap &params)
{

    GISServerManager &serverManager = GISServerManager::getInstance();
    bool isSucceed = serverManager.loadKLServers();
    if (isSucceed) {
        QAction *openGISAction = new QAction();
        openGISAction->setText("open gis");

        serverManager.mainUIServer->GetMainUI()->RegisterAction("PluginGISView", "openGISAction", openGISAction);
        QObject::connect(openGISAction, &QAction::triggered, this, &PluginGISView::onOpenGISView);
    }
}

void PluginGISView::unInit(KeyValueMap &params) { }

void PluginGISView::onOpenGISView()
{
    QMap<QString, QVariant> paramMap;
    paramMap[WORKAREA_TITLE] = "Baidu Map";
    paramMap[WORKAREA_TAB_DO_NOT_RESTORE] = true;
    GISServerManager &serverManager = GISServerManager::getInstance();
    if (serverManager.mainUIServer) {
        serverManager.mainUIServer->GetMainUI()->CreateWorkarea("GISWorkAreaContentWidget", paramMap);
    }
}

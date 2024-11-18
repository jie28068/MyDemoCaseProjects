#include "GISServerManager.h"
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("PluginPluginGISView")

GISServerManager &GISServerManager::getInstance()
{
    static GISServerManager instance;
    return instance;
}

bool GISServerManager::loadKLServers()
{
    mainUIServer = RequestServer<Kcc::CoreManUI_Layout::ICoreManUILayoutServer>();
    if (!mainUIServer) {
        LOGOUT("ICoreManUILayoutServer has not register", LOG_ERROR);
        return false;
    }
    mainUIServerInterface = RequestServerInterface<Kcc::CoreManUI_Layout::ICoreManUILayoutServer>();
    if (!mainUIServerInterface) {
        LOGOUT("PICoreManUILayoutServer  InterfaceBase has not register", LOG_ERROR);
        return false;
    }

    graphicsModelingServer = RequestServer<Kcc::PluginGraphicModeling::IPluginGraphicModelingServer>();
    if (!graphicsModelingServer) {
        LOGOUT("IPluginGraphicModelingServer unregistered", LOG_ERROR);
        return false;
    }
    graphicsModelingServerIF = RequestServerInterface<Kcc::PluginGraphicModeling::IPluginGraphicModelingServer>();
    if (!graphicsModelingServerIF) {
        LOGOUT("IPluginGraphicModelingServer interfacebase unregistered", LOG_ERROR);
        return false;
    }

    projectManagerServer = RequestServer<Kcc::ProjectManager::IProjectManagerServer>();
    if (!projectManagerServer) {
        LOGOUT("IProjectManagerServer", LOG_ERROR);
        return false;
    }

    projectManagerServerIF = RequestServerInterface<Kcc::ProjectManager::IProjectManagerServer>();
    if (!projectManagerServerIF) {
        LOGOUT("IProjectManagerServer  InterfaceBase未注册", LOG_ERROR);
        return false;
    }

    return true;
}

GISServerManager::GISServerManager() { }

GISServerManager::~GISServerManager() { }

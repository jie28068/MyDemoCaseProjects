#pragma once

#include "CoreLib/ServerBase.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "server/PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include "server/UILayout/IUIMainLayoutServer.h"

class GISServerManager
{
public:
    static GISServerManager &getInstance();

public:
    bool loadKLServers();

public:
    Kcc::CoreManUI_Layout::PICoreManUILayoutServer mainUIServer;
    Kcc::PIServerInterfaceBase mainUIServerInterface;

    Kcc::PluginGraphicModeling::PIPluginGraphicModelingServer graphicsModelingServer;
    Kcc::PIServerInterfaceBase graphicsModelingServerIF;

    Kcc::ProjectManager::PIProjectManagerServer projectManagerServer;
    Kcc::PIServerInterfaceBase projectManagerServerIF;

private:
    GISServerManager();
    ~GISServerManager();
};

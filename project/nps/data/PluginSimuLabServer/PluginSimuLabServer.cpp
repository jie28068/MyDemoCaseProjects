#include "PluginSimuLabServer.h"
#include "CoreLib/ServerManager.h"
#include "SimuLabServer.h"

REG_MODULE_BEGIN(PluginSimuLabServer, "", "PluginSimuLabServer")
REG_MODULE_END(PluginSimuLabServer)

USE_LOGOUT_("PluginSimuLabServer")

PluginSimuLabServer::PluginSimuLabServer(QString strName) : Module(Module_Type_Normal, strName)
{
    RegServer<ISimuLabServer>(new SimuLabServer());
}

PluginSimuLabServer::~PluginSimuLabServer() { }

void PluginSimuLabServer::init(KeyValueMap &params)
{
    Module::init(params);

    // 在以下添加自己的代码
}

void PluginSimuLabServer::unInit(KeyValueMap &saveParams)
{
    Module::unInit(saveParams);

    // 在以下添加自己的代码
}

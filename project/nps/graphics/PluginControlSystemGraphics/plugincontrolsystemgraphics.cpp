#include "plugincontrolsystemgraphics.h"
#include "CoreLib/ServerManager.h"
#include "InternalControlModuleGraphicsFactory.h"
#include "defines.h"

REG_MODULE_BEGIN(PluginControlSystemGraphics, "", "PluginControlSystemGraphics")
REG_MODULE_END(PluginControlSystemGraphics)

PluginControlSystemGraphics::PluginControlSystemGraphics(QString plugin_name)
    : Kcc::Module(Module_Type_Normal, plugin_name)
{
}

PluginControlSystemGraphics::~PluginControlSystemGraphics() { }

void PluginControlSystemGraphics::init(KeyValueMap &params)
{
    factory = QSharedPointer<ICanvasGraphicsObjectFactory>(new InternalControlModuleGraphicsFactory());
    registerCanvasGraphicsObjectFactory(GKD::SOURCE_MODULETYPE_CONTROL, factory);
}

void PluginControlSystemGraphics::unInit(KeyValueMap &params) { }

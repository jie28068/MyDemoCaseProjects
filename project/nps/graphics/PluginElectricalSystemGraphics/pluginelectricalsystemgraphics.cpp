#include "pluginelectricalsystemgraphics.h"
#include "CoreLib/ServerManager.h"
#include "InternalElectricalModuleGraphicsFactory.h"

REG_MODULE_BEGIN(PluginElectricalSystemGraphics, "", "PluginElectricalSystemGraphics")
REG_MODULE_END(PluginElectricalSystemGraphics)

PluginElectricalSystemGraphics::PluginElectricalSystemGraphics(QString plugin_name)
    : Kcc::Module(Module_Type_Normal, plugin_name)
{
}

PluginElectricalSystemGraphics::~PluginElectricalSystemGraphics() { }

void PluginElectricalSystemGraphics::init(KeyValueMap &params)
{
    factory = QSharedPointer<ICanvasGraphicsObjectFactory>(new InternalElectricalModuleGraphicsFactory());
    registerCanvasGraphicsObjectFactory(GKD::SOURCE_MODULETYPE_ELECTRICAL, factory);
}

void PluginElectricalSystemGraphics::unInit(KeyValueMap &params) { }

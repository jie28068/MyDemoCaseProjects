#include "pluginstatemachinegraphics.h"
#include "StateMachineGraphicsFactory.h"

REG_MODULE_BEGIN(PluginStateMachineGraphics, "", "PluginStateMachineGraphics")
REG_MODULE_END(PluginStateMachineGraphics)

PluginStateMachineGraphics::PluginStateMachineGraphics(QString plugin_name)
    : Kcc::Module(Module_Type_Normal, plugin_name)
{
}

PluginStateMachineGraphics::~PluginStateMachineGraphics() { }

void PluginStateMachineGraphics::init(KeyValueMap &params)
{
    factory = QSharedPointer<ICanvasGraphicsObjectFactory>(new StateMachineGraphicsFactory());
    registerCanvasGraphicsObjectFactory(GKD::SOURCE_MODULETYPE_STATEMACHINE, factory);
}

void PluginStateMachineGraphics::unInit(KeyValueMap &params) { }
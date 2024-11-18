#include "SlotBlockDelegate.h"
#include "CoreLib/ServerManager.h"
#include "GraphicsModelingConst.h"
#include "PluginComponentServer/IPluginComponentServer.h"
#include <GraphicsModelingTool.h>

using namespace Kcc::PluginComponent;
USE_LOGOUT_("SlotBlockDelegate")

SlotBlockDelegate::SlotBlockDelegate()
{
    PropertyMap map;
    map.insert("name", PLUGIN_COMPONENT_VIEW_DEVICE);
    PIPluginComponentServer pDevComponentServer = RequestServer<IPluginComponentServer>(map);
    if (pDevComponentServer == nullptr) {
        LOGOUT(PLUGIN_COMPONENT_VIEW_DEVICE + tr("unregistered, program cannot run properly!"),
               LOG_ERROR); // 未注册，程序无法正常运行！
        return;
    }
    ComponentInfo deviceinfo = pDevComponentServer->GetComponentInfo();
    foreach (QString groupname, deviceinfo.listGroup) {
        foreach (QString prototypename, deviceinfo.mapComponents[groupname]) {
            m_allEleModelPrototypeNamelist.append(prototypename);
        }
    }
}

void SlotBlockDelegate::init(PModel model)
{
    m_pSlotBlock = model.dynamicCast<SlotBlock>();
}

void SlotBlockDelegate::uninit() { }

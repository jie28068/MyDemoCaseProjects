#pragma once

#include "CoreUi/Common/CoreManUIServer.h"
#include "KLProject/klproject_global.h"
#include "PropertyManagerServer.h"
#include "UILayout/ICoreMainUIServer.h"
#include "server/CodeManagerServer/ICodeManagerServer.h"
#include "server/ElecSys/IElecSysServer.h"
#include "server/ElectricalModelCheckServer/IElectricalModelCheckServer.h"
#include "server/ModelManagerServer/IModelManagerServer.h"
#include "server/PluginComponentServer/IPluginComponentServer.h"
#include "server/PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
#include "server/ProjectManagerView/IProjectManagerViewServer.h"
#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include "server/SimulationManagerServer/ISimulationManagerServer.h"

using namespace Kcc::SimuNPSAdapter;
using namespace Kcc::PluginGraphicModeling;
using namespace Kcc::CodeManager;
using namespace Kcc::ElecSys;
using namespace Kcc::CoreManUI_Layout;
using namespace Kcc::ModelManager;
using namespace Kcc::PluginComponent;
using namespace Kcc::ProjectManager;
using namespace Kcc::SimulationManager;
using namespace Kcc::ProjectManagerView;
using namespace Kcc::ElectricalModelCheck;

// 属性窗口 服务管理类
class PropertyServerMng
{
public:
    static PropertyServerMng &getInstance();

    bool init();
    void unInit();

private:
    PropertyServerMng();

public:
    PIPluginGraphicModelingServer m_pGraphicsModelingServer; // 画板服务
    PIServerInterfaceBase m_pGraphicsModelingServerIF;       // 画板服务消息
    PICodeManagerServer m_pCodeManagerSvr;
    PISimuNPSDataDictionaryServer m_dataDictionaryServer; // 数据字典服务
    // PIModelDataMngServer m_pModelDataMngServer;           // 数据管理服务
    PIElecSysServer m_pIElecSysServer;
    PropertyManagerServer *propertyServer;                 // 本插件的服务对象
    PICoreManUILayoutServer m_pUIServer;                   // ui
    PIModelManagerServer m_ModelManagerServer;             // 工具箱服务
    PIPluginComponentServer m_ControlComponentServer;      // 控制元件窗口服务
    PIPluginComponentServer m_ElectricalComponentServer;   // 电气元件窗口服务
    PIProjectManagerServer m_projectManagerServer;         // 项目管理服务
    PISimulationManagerServer m_simulationManagerServer;   // 潮流计算服务
    PIServerInterfaceBase m_simulationManagerServerIF;     // 仿真服务消息
    PIProjectManagerViewServer m_ProjectManagerViewServer; // 数据管理器视图服务
    PElectricalModelCheckServer m_pElecModelCheckServer;   // 画板校验服务
};

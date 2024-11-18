#ifndef PROJECTVIEWSERVERS_H
#define PROJECTVIEWSERVERS_H

#pragma once

#include "server/CodeManagerServer/ICodeManagerServer.h"
#include "server/ElecSys/IElecSysServer.h"
#include "server/ModelManagerServer/IModelManagerServer.h"
#include "server/PluginComponentServer/IPluginComponentServer.h"
#include "server/PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
#include "server/PluginModuleWizardServer/IPluginModuleWizardServer.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
#include "server/PropertyManagerServer/IPropertyManagerServer.h"
#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
// 仿真调度
#include "server/ElectricalModelCheckServer/IElectricalModelCheckServer.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include "server/SimulationManagerServer/ISimulationManagerServer.h"
using namespace Kcc::SimulationManager;

using namespace Kcc::CodeManager;
using namespace Kcc::ProjectManager;
KCC_USE_NAMESPACE_COREMANUI

using namespace Kcc::PropertyManager;
using namespace Kcc::PluginComponent;
using namespace Kcc::PluginGraphicModeling;
using namespace Kcc::ModelManager;
using namespace Kcc::PluginWizard;
using namespace Kcc::SimuNPSAdapter;
using namespace Kcc::ElecSys;
using namespace Kcc::ElectricalModelCheck;
using namespace Kcc::MonitorPanelManager;

class ProjectViewServers
{
public:
    static ProjectViewServers &getInstance();

    bool init();
    void unInit();

private:
    ProjectViewServers();

public:
    PIPropertyManagerServer m_pPropertyManagerServer;       // 属性服务
    PIServerInterfaceBase m_pPropertyManagerServerIF;       // 属性服务消息
    PICoreManUILayoutServer m_pUIServer;                    ///< UI服务接口
    PIPluginComponentServer m_pCtrComponentServer;          // 控制元件窗口服务
    PIProjectManagerServer m_projectManagerServer;          // 项目管理服务
    PIServerInterfaceBase m_projectManagerServerIF;         // 项目管理服务接口类，响应通知
    PIPluginGraphicModelingServer m_pGraphicModelingServer; // 画板服务
    PIModelManagerServer m_pModelManagerServer;             // 工具箱服务
    PICodeManagerServer m_pCodeManagerServer;               // code服务
    PIPluginModuleWizardServer m_wizardServer;              // 向导服务
    PISimuNPSDataDictionaryServer m_dataDictionatyServer;   // 数据字典服务
    PIElecSysServer m_pElecSysServer;                       // 电气引擎服务
    PIServerInterfaceBase m_pSimulationManagerServer;       // 仿真调度服务，消息响应用
    PElectricalModelCheckServer m_pElecModelCheckServer;    // 画板校验服务
    PIMonitorPanelPlotServer m_MonitorPanelPlotServer;      // 监控面板服务
};

#endif
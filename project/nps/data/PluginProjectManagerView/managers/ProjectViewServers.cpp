#include "ProjectViewServers.h"
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("ProjectViewServers")

ProjectViewServers &ProjectViewServers::getInstance()
{
    static ProjectViewServers instance;
    return instance;
}

bool ProjectViewServers::init()
{
    bool initSts = true;
    m_pPropertyManagerServer = RequestServer<IPropertyManagerServer>();
    if (m_pPropertyManagerServer == nullptr) {
        LOGOUT("IPropertyManagerServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_pPropertyManagerServerIF = RequestServerInterface<IPropertyManagerServer>();
    if (m_pPropertyManagerServerIF == nullptr) {
        LOGOUT("IPropertyManagerServer InterfaceBase未注册", LOG_ERROR);
        initSts = false;
    }
    m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (m_pUIServer == nullptr) {
        LOGOUT("ICoreManUILayoutServer未注册", LOG_ERROR);
        initSts = false;
    }
    // m_pModelDataMngServer = RequestServer<IModelDataMngServer>();
    // if (m_pModelDataMngServer == nullptr) {
    //     LOGOUT("IModelDataMngServer未注册", LOG_ERROR);
    //     return false;
    // }
    // m_pModelDataMngServerIF = RequestServerInterface<IModelDataMngServer>();
    // if (m_pModelDataMngServerIF == nullptr) {
    //     LOGOUT("IModelDataMngServer InterfaceBase未注册", LOG_ERROR);
    //     return false;
    // }
    PropertyMap propertyMap;
    propertyMap.insert("name", PLUGIN_COMPONENT_VIEW_CONTROL);
    m_pCtrComponentServer = RequestServer<IPluginComponentServer>(propertyMap);
    if (nullptr == m_pCtrComponentServer) {
        LOGOUT(PLUGIN_COMPONENT_VIEW_CONTROL + "未注册，程序无法正常运行！", LOG_ERROR);
        initSts = false;
    }
    m_projectManagerServer = RequestServer<IProjectManagerServer>();
    if (m_projectManagerServer == nullptr) {
        LOGOUT("IProjectManagerServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_projectManagerServerIF = RequestServerInterface<IProjectManagerServer>();
    if (m_projectManagerServerIF == nullptr) {
        LOGOUT("m_projectManagerServer  InterfaceBase未注册", LOG_ERROR);
        initSts = false;
    }
    m_pGraphicModelingServer = RequestServer<IPluginGraphicModelingServer>();
    if (m_pGraphicModelingServer == nullptr) {
        LOGOUT("IPluginGraphicModelingServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_pModelManagerServer = RequestServer<IModelManagerServer>();
    if (m_pModelManagerServer == nullptr) {
        LOGOUT("IModelManagerServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_pCodeManagerServer = RequestServer<ICodeManagerServer>();
    if (m_pCodeManagerServer == nullptr) {
        LOGOUT("ICodeManagerServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_wizardServer = RequestServer<IPluginModuleWizardServer>();
    if (m_wizardServer == nullptr) {
        LOGOUT("IPluginModuleWizardServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_dataDictionatyServer = RequestServer<ISimuNPSDataDictionaryServer>();
    if (m_dataDictionatyServer == nullptr) {
        LOGOUT("ISimuNPSDataDictionaryServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_pElecSysServer = RequestServer<IElecSysServer>();
    if (m_pElecSysServer == nullptr) {
        LOGOUT("IElecSysServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_pSimulationManagerServer = RequestServerInterface<ISimulationManagerServer>();
    if (!m_pSimulationManagerServer) {
        LOGOUT("ISimulationManagerServer InterfaceBase未注册", LOG_ERROR);
        initSts = false;
    }
    m_pElecModelCheckServer = RequestServer<IElectricalModelCheckServer>();
    if (m_pElecModelCheckServer == nullptr) {
        LOGOUT("IElectricalModelCheckServer未注册", LOG_ERROR);
        initSts = false;
    }
    m_MonitorPanelPlotServer = RequestServer<IMonitorPanelPlotServer>();
    if (m_MonitorPanelPlotServer == nullptr) {
        LOGOUT("IMonitorPanelPlotServer未注册", LOG_ERROR);
        initSts = false;
    }
    return initSts;
}

void ProjectViewServers::unInit()
{
    if (m_pPropertyManagerServer != nullptr) {
        m_pPropertyManagerServer.clear();
    }
    if (m_pUIServer != nullptr) {
        m_pUIServer.clear();
    }
    // if (m_pModelDataMngServer != nullptr) {
    //     m_pModelDataMngServer.clear();
    // }
    if (m_pCtrComponentServer != nullptr) {
        m_pCtrComponentServer.clear();
    }
    if (m_projectManagerServer != nullptr) {
        m_projectManagerServer.clear();
    }
    if (m_pGraphicModelingServer != nullptr) {
        m_pGraphicModelingServer.clear();
    }
    if (m_pModelManagerServer != nullptr) {
        m_pModelManagerServer.clear();
    }
    if (m_pCodeManagerServer != nullptr) {
        m_pCodeManagerServer.clear();
    }
    if (m_wizardServer != nullptr) {
        m_wizardServer.clear();
    }
    if (m_dataDictionatyServer != nullptr) {
        m_dataDictionatyServer.clear();
    }
    if (m_pElecSysServer != nullptr) {
        m_pElecSysServer.clear();
    }
    if (m_pSimulationManagerServer != nullptr) {
        m_pSimulationManagerServer.clear();
    }
    if (m_pElecModelCheckServer != nullptr) {
        m_pElecModelCheckServer.clear();
    }
    if (m_MonitorPanelPlotServer != nullptr) {
        m_MonitorPanelPlotServer.clear();
    }
}

ProjectViewServers::ProjectViewServers() { }
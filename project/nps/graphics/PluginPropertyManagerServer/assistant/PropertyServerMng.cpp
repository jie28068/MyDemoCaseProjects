#include "PropertyServerMng.h"
#include "CoreLib/ServerManager.h"
#include "ElecSys/IElecSysServer.h"

USE_LOGOUT_("ModelingServerMng")

PropertyServerMng &PropertyServerMng::getInstance()
{
    static PropertyServerMng instance;
    return instance;
}

bool PropertyServerMng::init()
{
    bool initSts = true;
    m_pGraphicsModelingServer = RequestServer<IPluginGraphicModelingServer>();
    if (!m_pGraphicsModelingServer) {
        LOGOUT("IPluginGraphicModelingServer unregistered", LOG_ERROR);
        initSts = false;
    }
    m_pGraphicsModelingServerIF = RequestServerInterface<IPluginGraphicModelingServer>();
    if (m_pGraphicsModelingServerIF == nullptr) {
        LOGOUT("IPluginGraphicModelingServer interfacebase unregistered", LOG_ERROR);
        initSts = false;
    }

    m_dataDictionaryServer = RequestServer<ISimuNPSDataDictionaryServer>();
    if (!m_dataDictionaryServer) {
        LOGOUT("ISimuNPSDataDictionaryServer unregistered", LOG_ERROR);
        initSts = false;
    }

    m_pCodeManagerSvr = RequestServer<ICodeManagerServer>();
    if (!m_pCodeManagerSvr) {
        LOGOUT("ICodeManagerServer unregistered", LOG_ERROR);
        initSts = false;
    }

    m_pIElecSysServer = RequestServer<IElecSysServer>();
    if (m_pIElecSysServer == nullptr) {
        LOGOUT("IElecSysServer unregistered", LOG_ERROR);
        initSts = false;
    }

    m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!m_pUIServer) {
        LOGOUT(QObject::tr("ICoreManUILayout unregistered"), LOG_ERROR);
        initSts = false;
    }
    m_ModelManagerServer = RequestServer<IModelManagerServer>();
    if (!m_ModelManagerServer) {
        LOGOUT(QObject::tr("IModelManagerServer unregistered"), LOG_ERROR);
        initSts = false;
    }
    PropertyMap map;
    map.insert("name", PLUGIN_COMPONENT_VIEW_CONTROL);
    m_ControlComponentServer = RequestServer<IPluginComponentServer>(map);
    if (!m_ControlComponentServer) {
        LOGOUT(QObject::tr("IPluginComponentServer control unregistered"), LOG_ERROR);
        initSts = false;
    }
    map.clear();
    map.insert("name", PLUGIN_COMPONENT_VIEW_DEVICE);
    m_ElectricalComponentServer = RequestServer<IPluginComponentServer>(map);
    if (!m_ElectricalComponentServer) {
        LOGOUT(QObject::tr("IPluginComponentServer electrical unregistered"), LOG_ERROR);
        initSts = false;
    }
    m_projectManagerServer = RequestServer<IProjectManagerServer>();
    if (!m_projectManagerServer) {
        LOGOUT(QObject::tr("IProjectManagerServer unregistered"), LOG_ERROR);
        initSts = false;
    }
    m_simulationManagerServer = RequestServer<ISimulationManagerServer>();
    if (!m_simulationManagerServer) {
        LOGOUT(QObject::tr("ISimulationManagerServer unregistered"), LOG_ERROR);
        initSts = false;
    }
    m_simulationManagerServerIF = RequestServerInterface<ISimulationManagerServer>();
    if (m_simulationManagerServerIF == nullptr) {
        LOGOUT("ISimulationManagerServer interfacebase unregistered", LOG_ERROR);
        initSts = false;
    }
    m_ProjectManagerViewServer = RequestServer<IProjectManagerViewServer>();
    if (m_ProjectManagerViewServer == nullptr) {
        LOGOUT(QObject::tr("IProjectManagerViewServer unregistered"), LOG_ERROR);
        initSts = false;
    }
    m_pElecModelCheckServer = RequestServer<IElectricalModelCheckServer>();
    if (m_pElecModelCheckServer == nullptr) {
        LOGOUT(QObject::tr("IElectricalModelCheckServer unregistered"), LOG_ERROR);
        initSts = false;
    }
    return initSts;
}

void PropertyServerMng::unInit()
{
    if (m_pGraphicsModelingServer != nullptr) {
        m_pGraphicsModelingServer.clear();
    }
    if (m_dataDictionaryServer != nullptr) {
        m_dataDictionaryServer.clear();
    }
    if (m_pCodeManagerSvr != nullptr) {
        m_pCodeManagerSvr.clear();
    }
    if (m_pIElecSysServer != nullptr) {
        m_pIElecSysServer.clear();
    }
    if (m_pUIServer != nullptr) {
        m_pUIServer.clear();
    }
    if (m_ModelManagerServer != nullptr) {
        m_ModelManagerServer.clear();
    }
    if (m_ControlComponentServer != nullptr) {
        m_ControlComponentServer.clear();
    }
    if (m_ElectricalComponentServer != nullptr) {
        m_ElectricalComponentServer.clear();
    }
    if (m_projectManagerServer != nullptr) {
        m_projectManagerServer.clear();
    }
    if (m_pElecModelCheckServer != nullptr) {
        m_pElecModelCheckServer.clear();
    }
}

PropertyServerMng::PropertyServerMng() { }

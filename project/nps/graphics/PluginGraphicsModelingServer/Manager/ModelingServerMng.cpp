#include "ModelingServerMng.h"
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("ModelingServerMng")

ModelingServerMng::ModelingServerMng() { }

ModelingServerMng &ModelingServerMng::getInstance()
{
    static ModelingServerMng instance;
    return instance;
}

bool ModelingServerMng::init()
{
    m_pProjectMngServer = RequestServer<IProjectManagerServer>();
    if (!m_pProjectMngServer) {
        LOGOUT("IProjectManagerServer", LOG_ERROR);
        return false;
    }

    m_pProjectMngServerIF = RequestServerInterface<IProjectManagerServer>();
    if (!m_pProjectMngServerIF) {
        LOGOUT("IProjectManagerServer  InterfaceBase未注册", LOG_ERROR);
        return false;
    }

    m_pModelManagerServer = RequestServer<IModelManagerServer>();
    if (!m_pModelManagerServer) {
        LOGOUT("PIModelManagerServer", LOG_ERROR);
        return false;
    }

    m_pGraphicsModelingServer =
            RequestServer<IPluginGraphicModelingServer>().dynamicCast<PluginGraphicsModelingServer>();
    if (!m_pGraphicsModelingServer) {
        LOGOUT("PluginGraphicsModelingServer未注册", LOG_ERROR);
        return false;
    }

    m_pGraphicsModelingServerIF = RequestServerInterface<IPluginGraphicModelingServer>();
    if (!m_pGraphicsModelingServerIF) {
        LOGOUT("IPluginGraphicModelingServer  InterfaceBase未注册", LOG_ERROR);
        return false;
    }

    m_pPropertyManagerServer = RequestServer<IPropertyManagerServer>();
    if (!m_pPropertyManagerServer) {
        LOGOUT("IPropertyManagerServer未注册", LOG_ERROR);
        return false;
    }
    m_pPropertyManagerServerIF = RequestServerInterface<IPropertyManagerServer>();
    if (!m_pPropertyManagerServerIF) {
        LOGOUT("IPropertyManagerServer INTERFACE未注册", LOG_ERROR);
        return false;
    }

    m_pDrawingBoardHelperServer = RequestServer<IElectricalModelCheckServer>();
    if (!m_pDrawingBoardHelperServer) {
        LOGOUT("IDrawingBoardHelperServer未注册", LOG_ERROR);
        return false;
    }

    m_pDrawingBoardHelperServerIF = RequestServerInterface<IElectricalModelCheckServer>();
    if (!m_pDrawingBoardHelperServerIF) {
        LOGOUT("IDrawingBoardHelperServer INTERFACE未注册", LOG_ERROR);
        return false;
    }

    // fixme
    // m_pNetworkManagerServerIF = RequestServerInterface<IPluginNetworkModelManagerServer>();
    // if (!m_pNetworkManagerServerIF) {
    //     LOGOUT("IPluginNetworkModelManagerServer INTERFACE未注册", LOG_ERROR);
    //     return false;
    // }

    m_m_pUIServerIF = RequestServerInterface<ICoreManUILayoutServer>();
    if (!m_m_pUIServerIF) {
        LOGOUT("PICoreManUILayoutServer  InterfaceBase未注册", LOG_ERROR);
        return false;
    }
    m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!m_pUIServer) {
        LOGOUT("ICoreManUILayoutServer未注册", LOG_ERROR);
        return false;
    }

    m_pDataDictionaryServer = RequestServer<ISimuNPSDataDictionaryServer>();
    if (!m_pDataDictionaryServer) {
        LOGOUT("ISimuNPSDataDictionaryServer未注册", LOG_ERROR);
        return false;
    }

    m_pSimulationManagerServer = RequestServerInterface<ISimulationManagerServer>();
    if (!m_pSimulationManagerServer) {
        LOGOUT("ISimulationManagerServer InterfaceBase未注册", LOG_ERROR);
        return false;
    }

    m_pElecSysServer = RequestServer<IElecSysServer>();
    if (!m_pElecSysServer) {
        LOGOUT("IElecSysServer未注册", LOG_ERROR);
        return false;
    }

    m_pICodeManagerServer = RequestServer<ICodeManagerServer>();
    if (!m_pICodeManagerServer) {
        LOGOUT("ICodeManagerServer未注册", LOG_ERROR);
        return false;
    }

    m_pLogSeverIF = RequestServerInterface<ILogServer>();
    if (!m_pLogSeverIF) {
        LOGOUT("ILogServer INTERFACE未注册", LOG_ERROR);
        return false;
    }

    PropertyMap electricalMap;
    electricalMap.insert("name", PLUGIN_COMPONENT_VIEW_DEVICE);
    m_pPluginElectricComponentServer = RequestServer<IPluginComponentServer>(electricalMap);
    if (!m_pPluginElectricComponentServer) {
        LOGOUT("m_pPluginElectricComponentServer INTERFACE未注册", LOG_ERROR);
        return false;
    }

    PropertyMap controlMap;
    controlMap.insert("name", PLUGIN_COMPONENT_VIEW_CONTROL);
    m_pPluginControlComponentServer = RequestServer<IPluginComponentServer>(controlMap);
    if (!m_pPluginControlComponentServer) {
        LOGOUT("m_pPluginControlComponentServer INTERFACE未注册", LOG_ERROR);
        return false;
    }

    return true;
}

void ModelingServerMng::unInit()
{
    if (m_pProjectMngServerIF) {
        m_pProjectMngServerIF.clear();
    }
    if (m_pModelManagerServer) {
        m_pModelManagerServer.clear();
    }
    if (m_pUIServer) {
        m_pUIServer.clear();
    }
    if (m_m_pUIServerIF) {
        m_m_pUIServerIF.clear();
    }
    if (m_pPropertyManagerServer) {
        m_pPropertyManagerServer.clear();
    }
    if (m_pPropertyManagerServerIF) {
        m_pPropertyManagerServerIF.clear();
    }
    if (m_pGraphicsModelingServer) {
        m_pGraphicsModelingServer.clear();
    }
    if (m_pDrawingBoardHelperServer) {
        m_pDrawingBoardHelperServer.clear();
    }
    if (m_pDrawingBoardHelperServerIF) {
        m_pDrawingBoardHelperServerIF.clear();
    }
    // if (m_pNetworkManagerServerIF) {
    //     m_pNetworkManagerServerIF.clear();
    // }
    if (m_pSimulationManagerServer) {
        m_pSimulationManagerServer.clear();
    }
    if (m_pElecSysServer) {
        m_pElecSysServer.clear();
    }
    if (m_pDataDictionaryServer) {
        m_pDataDictionaryServer.clear();
    }
    if (m_pICodeManagerServer) {
        m_pICodeManagerServer.clear();
    }
    if (m_pLogSeverIF) {
        m_pLogSeverIF.clear();
    }
}

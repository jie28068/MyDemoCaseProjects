#include "ModelManagerViewServerMng.h"
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("ProjectViewServers")

ModelManagerViewServerMng &ModelManagerViewServerMng::getInstance()
{
    static ModelManagerViewServerMng instance;
    return instance;
}

bool ModelManagerViewServerMng::initServerMng()
{
    if (m_isInited) { // 避免重复初始化
        return m_isInited;
    }

    m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!m_pUIServer) {
        LOGOUT(tr("ICoreManUILayoutServer unregistered"), LOG_ERROR);
        m_isInited = false;
    }

    m_pModelManagerServer = RequestServer<IModelManagerServer>().dynamicCast<ModelManagerServer>();
    if (!m_pModelManagerServer) {
        LOGOUT(tr("ModelManagerServer unregistered"), LOG_ERROR);
        m_isInited = false;
    }

    // 向导服务
    m_pWizardServer = RequestServer<IPluginModuleWizardServer>();
    if (!m_pWizardServer) {
        LOGOUT(tr("IPluginModuleWizardServer unregistered"), LOG_ERROR);
        m_isInited = false;
    }

    // 项目管理服务
    m_pProjectMngServer = RequestServer<IProjectManagerServer>();
    if (!m_pProjectMngServer) {
        LOGOUT(tr("IProjectManagerServer unregistered"), LOG_ERROR);
        m_isInited = false;
    }

    m_pProjectMngServerIF = RequestServerInterface<IProjectManagerServer>();
    if (!m_pProjectMngServerIF) {
        LOGOUT(tr("IProjectManagerServer InterfaceBase unregistered"), LOG_ERROR);
        m_isInited = false;
    }

    // 画板服务
    m_pDrawingBoardServer = RequestServer<IPluginGraphicModelingServer>();
    if (!m_pDrawingBoardServer) {
        LOGOUT(tr("IPluginGraphicModelingServer unregistered"), LOG_ERROR);
        m_isInited = false;
    }

    // // 画板服务接口
    // m_pBoardServerIF = RequestServerInterface<IPluginGraphicModelingServer>();
    // if (!m_pBoardServerIF) {
    //     LOGOUT(tr("IPluginGraphicModelingServer InterfaceBase unregistered"), LOG_ERROR);
    //     m_isInited = false;
    // }

    m_isInited = true;

    return m_isInited;
}

bool ModelManagerViewServerMng::unInitServerMng()
{
    if (m_pUIServer) {
        m_pUIServer.clear();
    }

    if (m_pModelManagerServer) {
        m_pModelManagerServer.clear();
    }

    if (m_pWizardServer) {
        m_pWizardServer.clear();
    }

    if (m_pProjectMngServer) {
        m_pProjectMngServer.clear();
    }

    if (m_pProjectMngServerIF) {
        m_pProjectMngServerIF.clear();
    }

    if (m_pDrawingBoardServer) {
        m_pDrawingBoardServer.clear();
    }

    // if (m_pBoardServerIF) {
    //     m_pBoardServerIF.clear();
    // }

    return true;
}

ModelManagerViewServerMng::ModelManagerViewServerMng()
{
    m_isInited = false;
}

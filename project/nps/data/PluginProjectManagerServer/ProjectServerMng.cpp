#include "ProjectServerMng.h"

#include "CoreLib/ServerManager.h"

USE_LOGOUT_("ProjectServerMng")

ProjectServerMng &ProjectServerMng::getInstance()
{
    static ProjectServerMng instace;
    return instace;
}

bool ProjectServerMng::init()
{
    m_pDataDicServer = RequestServer<ISimuNPSDataDictionaryServer>();
    if (!m_pDataDicServer) {
        LOGOUT(tr("ISimuNPSDataDictionaryServer is not registered"), LOG_ERROR); // ISimuNPSDataDictionaryServer未注册
        return false;
    }
    m_pModelManagerServer = RequestServer<IModelManagerServer>();
    if (!m_pModelManagerServer) {
        LOGOUT(tr("IModelManagerServer is not registered"), LOG_ERROR); // IModelManagerServer未注册
        return false;
    }
    m_GraphicModelServer = RequestServer<IPluginGraphicModelingServer>();
    if (!m_GraphicModelServer) {
        LOGOUT(tr("IPluginGraphicModelingServer is not registered"), LOG_ERROR); // IPluginGraphicModelingServer未注册
        return false;
    }
    return true;
}

void ProjectServerMng::unInit()
{
    if (m_pDataDicServer != nullptr) {
        m_pDataDicServer.clear();
    }
    if (m_pModelManagerServer != nullptr) {
        m_pModelManagerServer.clear();
    }
    if (m_GraphicModelServer != nullptr) {
        m_GraphicModelServer.clear();
    }
}

ProjectServerMng::ProjectServerMng() { }

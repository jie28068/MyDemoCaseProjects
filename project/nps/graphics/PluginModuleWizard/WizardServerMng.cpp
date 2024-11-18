#include "WizardServerMng.h"

#include "CoreLib/ServerManager.h"

USE_LOGOUT_("WizardServerMng")

WizardServerMng &WizardServerMng::getInstance()
{
    static WizardServerMng instance;
    return instance;
}

bool WizardServerMng::init()
{
    m_pCodeManagerServer = RequestServer<ICodeManagerServer>();
    if (!m_pCodeManagerServer) {
        LOGOUT(tr("ICodeManagerServer is not registered"), LOG_ERROR); // ICodeManagerServer未注册
        return false;
    }

    m_pCodeManagerServerIF = RequestServerInterface<ICodeManagerServer>();
    if (!m_pCodeManagerServerIF) {
        LOGOUT(tr("ICodeManagerServer is not registered"), LOG_ERROR); // ICodeManagerServer未注册
        return false;
    }

    m_pProjectServer = RequestServer<IProjectManagerServer>();
    if (!m_pProjectServer) {
        LOGOUT(tr("IProjectManagerServer is not registered"), LOG_ERROR); // IProjectManagerServer未注册
        return false;
    }

    m_pModelServer = RequestServer<IModelManagerServer>();
    if (!m_pModelServer) {
        LOGOUT(tr("IModelManagerServer is not registered"), LOG_ERROR); // IModelManagerServer未注册
        return false;
    }
    // UI 服务
    m_pUIServerIF = RequestServer<ICoreManUILayoutServer>();
    if (!m_pUIServerIF) {
        LOGOUT("PICoreManUILayoutServer  InterfaceBase未注册", LOG_ERROR);
    }

    return true;
}

void WizardServerMng::unInit()
{
    if (m_pCodeManagerServer != nullptr) {
        m_pCodeManagerServer.clear();
    }
    if (m_pProjectServer != nullptr) {
        m_pProjectServer.clear();
    }
    if (m_pModelServer != nullptr) {
        m_pModelServer.clear();
    }
}

WizardServerMng::WizardServerMng() { }

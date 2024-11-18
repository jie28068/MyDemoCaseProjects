#ifndef MODELMANAGERVIEWSERVERMNG_H
#define MODELMANAGERVIEWSERVERMNG_H

#include "CoreLib/ServerBase.h"
using namespace Kcc;

// UI服务
#include <server/UILayout/ICoreMainUIServer.h>
using namespace Kcc::CoreManUI_Layout;

#include "ModelManagerServer.h"

// 向导
#include "PluginModuleWizardServer/IPluginModuleWizardServer.h"
using namespace Kcc::PluginWizard;

// 项目管理服务
#include "ProjectManagerServer/IProjectManagerServer.h"
using namespace Kcc::ProjectManager;

// 画板服务
#include "PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
using namespace Kcc::PluginGraphicModeling;

class ModelManagerViewServerMng : public QObject
{
    Q_OBJECT
public:
    static ModelManagerViewServerMng &getInstance();

    bool initServerMng();
    bool unInitServerMng();
    bool isInited() { return m_isInited; };

private:
    ModelManagerViewServerMng();

public:
    PModelManagerServer m_pModelManagerServer; // 模型工具箱

    PICoreManUILayoutServer m_pUIServer;                 // UI服务接口
    PIPluginModuleWizardServer m_pWizardServer;          // 原型向导
    PIProjectManagerServer m_pProjectMngServer;          // 项目管理
    PIServerInterfaceBase m_pProjectMngServerIF;         // 项目管理服务接口类，响应通知
    PIPluginGraphicModelingServer m_pDrawingBoardServer; // 画板服务

    bool m_isInited;
};

#endif
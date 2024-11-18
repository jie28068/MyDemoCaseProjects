#ifndef MODELINGSERVERMNG_H
#define MODELINGSERVERMNG_H

// 画板插件
#include "plugingraphicsmodelingserver.h"

// 仿真调度
#include "server/SimulationManagerServer/ISimulationManagerServer.h"
using namespace Kcc::SimulationManager;

// 电气服务
#include "server/ElecSys/IElecSysServer.h"
using namespace Kcc::ElecSys;

// 属性页服务
#include "server/PropertyManagerServer/IPropertyManagerServer.h"
using namespace Kcc::PropertyManager;

// 画板助手服务
#include "server/ElectricalModelCheckServer/IElectricalModelCheckServer.h"
using namespace Kcc::ElectricalModelCheck;

// UI服务
#include <server/UILayout/ICoreMainUIServer.h>
using namespace Kcc::CoreManUI_Layout;

#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
using namespace Kcc::SimuNPSAdapter;

// 代码编辑窗口的服务
#include "server/CodeManagerServer/ICodeManagerServer.h"
using namespace Kcc::CodeManager;

#include "ModelManagerServer/IModelManagerServer.h"
using namespace Kcc::ModelManager;

// 项目管理服务
#include "ProjectManagerServer/IProjectManagerServer.h"
using namespace Kcc::ProjectManager;

// 元器件窗口
#include "PluginComponentServer/IPluginComponentServer.h"
using namespace Kcc::PluginComponent;

// 图形建模服务管理类
class ModelingServerMng
{
public:
    static ModelingServerMng &getInstance();

    bool init();
    void unInit();

private:
    ModelingServerMng();

public:
    // 服务接口
    PICoreManUILayoutServer m_pUIServer;              // UI服务接口
    PIServerInterfaceBase m_m_pUIServerIF;            // UI服务，消息响应用

    PIProjectManagerServer m_pProjectMngServer;       // 项目管理服务
    PIServerInterfaceBase m_pProjectMngServerIF;      // 项目管理服务接口类，响应通知

    PIModelManagerServer m_pModelManagerServer;       // 模型工具箱

    PIPropertyManagerServer m_pPropertyManagerServer; // 属性页服务
    PIServerInterfaceBase m_pPropertyManagerServerIF;

    PPluginGraphicsModelingServer m_pGraphicsModelingServer; // 画板服务
    PIServerInterfaceBase m_pGraphicsModelingServerIF;

    PElectricalModelCheckServer m_pDrawingBoardHelperServer;
    PIServerInterfaceBase m_pDrawingBoardHelperServerIF;

    // PIServerInterfaceBase m_pNetworkManagerServerIF; // 网络模型管理器服务

    PIServerInterfaceBase m_pSimulationManagerServer;      // 仿真调度服务，消息响应用

    PIElecSysServer m_pElecSysServer;                      // 电气服务

    PISimuNPSDataDictionaryServer m_pDataDictionaryServer; // 数据字典服务

    PICodeManagerServer m_pICodeManagerServer;             // 代码编辑窗口的服务

    PIServerInterfaceBase m_pLogSeverIF;                   // 日志服务接口

    PIPluginComponentServer m_pPluginElectricComponentServer;      // 电气元器件服务接口

    PIPluginComponentServer m_pPluginControlComponentServer;      // 控制元器件服务接口

private:
    bool m_isInitOk; // 是否初始化成功
};

#endif // MODELINGSERVERMNG_H

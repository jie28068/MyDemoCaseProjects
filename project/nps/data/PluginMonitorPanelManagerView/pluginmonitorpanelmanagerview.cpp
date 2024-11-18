#include "pluginmonitorpanelmanagerview.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "MonitorPanelPlotServer.h"
#include "def.h"
#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include "server/SimulationManagerServer/ISimulationManagerServer.h"

using namespace Kcc::SimulationManager;
using namespace Kcc::SimuNPSAdapter;
PlotDefaultPropertyConfig *PlotDefaultPropertyConfig::m_instance = new PlotDefaultPropertyConfig;

REG_MODULE_BEGIN(PluginMonitorPanelManagerForSimuNPS, "", "PluginMonitorPanelManagerView")
REG_MODULE_END(PluginMonitorPanelManagerForSimuNPS)

PluginMonitorPanelManagerForSimuNPS::PluginMonitorPanelManagerForSimuNPS(QString strName)
    : Module(Module_Type_Plugin, strName)
{
    RegServer<IMonitorPanelPlotServer>(new MonitorPanelPlotServer);
}

PluginMonitorPanelManagerForSimuNPS::~PluginMonitorPanelManagerForSimuNPS() { }

void PluginMonitorPanelManagerForSimuNPS::init(KeyValueMap &params)
{
    Module::init(params);

    if (params.contains(CurveTreeViewHideColumns)) {
        QStringList strList = params[CurveTreeViewHideColumns].split(",");
        gConfSet(CurveTreeViewHideColumns, strList);
    }

    if (params.contains(CurveTreeViewType)) {
        gConfSet(CurveTreeViewType, params[CurveTreeViewType].toInt());
    }

    if (params.contains(IsShowCorsorTableView)) {
        gConfSet(IsShowCorsorTableView, params[IsShowCorsorTableView].toInt());
    }

    if (params.contains(IsShowDictionaryView)) {
        gConfSet(IsShowDictionaryView, params[IsShowDictionaryView].toInt());
    }

    if (params.contains(DefaultCurveStyle)) {
        gConfSet(DefaultCurveStyle, params[DefaultCurveStyle].toInt());
    }

    PIServerInterfaceBase pSm = RequestServerInterface<ISimulationManagerServer>();
    if (pSm) {
        pSm->connectNotify(Notify_DrawingBoardRunning, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSm->connectNotify(Notify_DrawingBoardStopped, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSm->connectNotify(Notify_SimulationPaused, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSm->connectNotify(Notify_SimulationResume, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    }

    PIServerInterfaceBase pDataDiction = RequestServerInterface<ISimuNPSDataDictionaryServer>();
    if (pDataDiction) {
        pDataDiction->connectNotify(Notify_ElementDestroyed, this,
                                    SLOT(onReceiveDataDictionMessage(unsigned int, const NotifyStruct &)));
        pDataDiction->connectNotify(Notify_ElementRenameed, this,
                                    SLOT(onReceiveDataDictionMessage(unsigned int, const NotifyStruct &)));
    }

    PIServerInterfaceBase serverBase = RequestServerInterface<IMonitorPanelPlotServer>();
    if (!serverBase) {
        return;
    } else {
        serverBase->connectNotify(CODE_ADDVARIABLE, this,
                                  SLOT(onReceiveVarPathInfo(unsigned int, const NotifyStruct &)));
    }
}

void PluginMonitorPanelManagerForSimuNPS::unInit(KeyValueMap &saveParams)
{
    Module::unInit(saveParams);
}

void PluginMonitorPanelManagerForSimuNPS::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    PIMonitorPanelPlotServer pImonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!pImonitorServer)
        return;
    MonitorPanelPlotServer *pMonitorServer = static_cast<MonitorPanelPlotServer *>(pImonitorServer.data());
    if (!pMonitorServer)
        return;

    if (code == Notify_DrawingBoardRunning || code == Notify_SimulationResume) {
        pMonitorServer->setSimulateState(true);
    } else if (code == Notify_DrawingBoardStopped || code == Notify_SimulationPaused) {
        pMonitorServer->setSimulateState(false);
    }
}

void PluginMonitorPanelManagerForSimuNPS::onReceiveDataDictionMessage(unsigned int code, const NotifyStruct &param)
{
    PIMonitorPanelPlotServer pImonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!pImonitorServer)
        return;
    MonitorPanelPlotServer *pMonitorServer = static_cast<MonitorPanelPlotServer *>(pImonitorServer.data());
    if (!pMonitorServer)
        return;

    if (code == Notify_ElementDestroyed) {
        const QString &varPath = param.paramMap["name"].toString();
        if (varPath.split(".").size() < 3u)
            return;
        pMonitorServer->removeBoardVar(QStringList() << varPath);
    } else if (code == Notify_ElementRenameed) {
        const QString &oldName = param.paramMap["oldName"].toString();
        const QString &newName = param.paramMap["newName"].toString();
        QMap<QString, QString> tmpMap;
        tmpMap[oldName] = newName;
        pMonitorServer->renameBoardVar(tmpMap);
    }
}

void PluginMonitorPanelManagerForSimuNPS::onReceiveVarPathInfo(unsigned int code, const NotifyStruct &param)
{
    if (code != CODE_ADDVARIABLE) {
        return;
    }

    if (param.paramMap.isEmpty()) {
        return;
    }

    PIMonitorPanelPlotServer pImonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!pImonitorServer)
        return;
    MonitorPanelPlotServer *pMonitorServer = static_cast<MonitorPanelPlotServer *>(pImonitorServer.data());
    if (!pMonitorServer)
        return;

    pMonitorServer->setTransferVar(param.paramMap["variablePaths"].toStringList());
}

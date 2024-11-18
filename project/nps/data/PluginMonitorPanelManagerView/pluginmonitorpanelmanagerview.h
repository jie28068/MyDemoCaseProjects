#ifndef PLUGINMONITORPANELMANAGERVIEW_H
#define PLUGINMONITORPANELMANAGERVIEW_H

#include "pluginmonitorpanelmanagerview_global.h"
#include "server/Base/IServerInterfaceBase.h"
#include <CoreLib/Module.h>

class PluginMonitorPanelManagerForSimuNPS : public QObject, public Module
{
    DEFINE_MODULE
    Q_OBJECT
public:
    PluginMonitorPanelManagerForSimuNPS(QString strName);
    ~PluginMonitorPanelManagerForSimuNPS();
    /// 初始化
    virtual void init(KeyValueMap &params);
    /// 反初始化
    virtual void unInit(KeyValueMap &saveParams);

private slots:
    void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param);
    void onReceiveDataDictionMessage(unsigned int code, const NotifyStruct &param);
    void onReceiveVarPathInfo(unsigned int code, const NotifyStruct &param);
};

#endif // PLUGINMONITORPANELMANAGERVIEW_H

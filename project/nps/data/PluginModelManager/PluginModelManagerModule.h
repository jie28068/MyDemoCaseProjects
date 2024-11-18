#ifndef PLUGINMODELMANAGERMODULE_H
#define PLUGINMODELMANAGERMODULE_H

#include "CoreLib/Module.h"
#include "ModelManagerServer.h"
#include "ModelManagerView.h"
#include "ModelTreeView.h"
#include "ModelTreeWidget.h"
class PluginModelManagerModule : public QWidget, public Module
{
    Q_OBJECT

    DEFINE_MODULE
public:
    PluginModelManagerModule(QString moduleName);
    ~PluginModelManagerModule();

    /// @ 框架相关
    virtual void init(KeyValueMap &params);
    virtual void unInit(KeyValueMap &saveParams);

private:
    ModelManagerServer *m_pModelManagerServer;
    ModelManagerView *m_modelView;  
    ModelTreeWidget *m_modelWidget; // 模型管理窗口

};

#endif // PLUGINMODELMANAGERMODULE_H


#pragma once

#include "CoreLib/Module.h"
#include "CoreLib/ServerBase.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include "server/UILayout/IUIMainLayoutServer.h"

class PluginGISView : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginGISView(QString plugin_name);
    ~PluginGISView();

    virtual void init(KeyValueMap &params) override;

    virtual void unInit(KeyValueMap &params) override;

private slots:
    void onOpenGISView();
};

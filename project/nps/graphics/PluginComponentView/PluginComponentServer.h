#ifndef PLUGINCOMPONENTSERVER_H
#define PLUGINCOMPONENTSERVER_H

#include "server/PluginComponentServer/IPluginComponentServer.h"
// 使用服务必须包含的基类头文件
#include "CoreLib/ServerBase.h"

// 版本设置，一般为1
#define SERVER_VERSION 1

// ServerBase的命名空间
using namespace Kcc;
// 插件A的服务命名空间
using namespace Kcc::PluginComponent;

class PluginComponentView;

class PluginComponentServer : public ServerBase, public IPluginComponentServer
{
public:
    PluginComponentServer(PluginComponentView *pluginComponentView);
    virtual ~PluginComponentServer();
    virtual ComponentInfo GetComponentInfo(GetDataType type = GetDataType_all) override;
    virtual QPixmap GetComponentIcon(const QString &prototypename, const int &blocktype,
                                     bool bcombinedBlock = false) override;
    virtual void openShowHelp(const QString &name) override;

private:
    PluginComponentView *m_PluginComponentView;
};

#endif // PLUGINCOMPONENTSERVER_H
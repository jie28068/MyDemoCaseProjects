#include "PluginComponentServer.h"
#include "PluginComponentView.h"

PluginComponentServer::PluginComponentServer(PluginComponentView *pluginComponentView)
{
    m_PluginComponentView = pluginComponentView;
}

PluginComponentServer::~PluginComponentServer() { }

ComponentInfo PluginComponentServer::GetComponentInfo(GetDataType type)
{
    if (m_PluginComponentView != nullptr) {
        return m_PluginComponentView->getGroupComponentInfo(type);
    }
    return ComponentInfo();
}

QPixmap PluginComponentServer::GetComponentIcon(const QString &prototypename, const int &blocktype, bool bcombinedBlock)
{
    if (m_PluginComponentView != nullptr) {
        return m_PluginComponentView->getComponentIcon(prototypename, blocktype, bcombinedBlock);
    }
    QPixmap defaultPix(67, 47);
    defaultPix.fill(QColor("#FFFFFF"));
    return defaultPix;
}


void PluginComponentServer::openShowHelp(const QString &name)
{
    m_PluginComponentView->onShowHelp(name);
}

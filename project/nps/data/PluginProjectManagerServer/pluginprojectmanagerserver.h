#ifndef PLUGINPROJECTMANAGERSERVER_H
#define PLUGINPROJECTMANAGERSERVER_H

#include "CoreLib/Module.h"
#include "KLWidgets/KCustomDialog.h"
#include "ProjectManagerServer.h"
#include "pluginprojectmanagerserver_global.h"
#include <QAction>
class PLUGINPROJECTMANAGERSERVER_EXPORT PluginProjectManagerServer : public QObject, public Module
{
    Q_OBJECT

    DEFINE_MODULE

public:
    PluginProjectManagerServer(QString strName);
    ~PluginProjectManagerServer();

    void init(KeyValueMap &params) override;
    void unInit(KeyValueMap &saveParams) override;
private slots:
    void onOpenMonitorPanel();
    void onSwitchLanguage();

private:
    ProjectManagerServer *projectManagerServer;

    QAction *m_openMonitorPanelAction;
    QAction *m_switchLanguage;
    bool m_bLanguageChanged;
};

#endif // PLUGINPROJECTMANAGERSERVER_H

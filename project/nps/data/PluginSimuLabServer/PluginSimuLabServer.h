#ifndef PLUGINCADSIMUMANAGER_H
#define PLUGINCADSIMUMANAGER_H

#include "CoreLib/Module.h"
#include "PluginSimuLabServer_global.h"
#include <QObject>

class PluginSimuLabServer : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginSimuLabServer(QString strName);
    ~PluginSimuLabServer();

public:
    virtual void init(KeyValueMap &params);
    virtual void unInit(KeyValueMap &saveParams);

private:
};

#endif // PLUGINCADSIMUMANAGER_H
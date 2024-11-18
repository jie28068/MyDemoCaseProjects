#ifndef PLUGINCONTROLSYSTEMGRAPHICS_H
#define PLUGINCONTROLSYSTEMGRAPHICS_H

#include "CoreLib/Module.h"

#include "defines.h"
#include "plugincontrolsystemgraphics_global.h"

class PluginControlSystemGraphics : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginControlSystemGraphics(QString plugin_name);
    ~PluginControlSystemGraphics();

    virtual void init(KeyValueMap &params) override;

    virtual void unInit(KeyValueMap &params) override;

private:
    QSharedPointer<ICanvasGraphicsObjectFactory> factory;
};

#endif // PLUGINCONTROLSYSTEMGRAPHICS_H

#ifndef PLUGINELECTRICALSYSTEMGRAPHICS_H
#define PLUGINELECTRICALSYSTEMGRAPHICS_H

#include "CoreLib/Module.h"
#include "defines.h"
#include "pluginelectricalsystemgraphics_global.h"

class PluginElectricalSystemGraphics : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginElectricalSystemGraphics(QString plugin_name);
    ~PluginElectricalSystemGraphics();

    virtual void init(KeyValueMap &params) override;

    virtual void unInit(KeyValueMap &params) override;

private:
    QSharedPointer<ICanvasGraphicsObjectFactory> factory;
};

#endif // PLUGINELECTRICALSYSTEMGRAPHICS_H

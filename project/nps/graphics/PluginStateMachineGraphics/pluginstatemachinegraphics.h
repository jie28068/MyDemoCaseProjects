#ifndef PLUGINSTATEMACHINEGRAPHICS_H
#define PLUGINSTATEMACHINEGRAPHICS_H

#include "CoreLib/Module.h"
#include "defines.h"
#include "pluginstatemachinegraphics_global.h"

class PluginStateMachineGraphics : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginStateMachineGraphics(QString plugin_name);
    ~PluginStateMachineGraphics();

    virtual void init(KeyValueMap &params) override;

    virtual void unInit(KeyValueMap &params) override;

private:
    QSharedPointer<ICanvasGraphicsObjectFactory> factory;
};

#endif // PLUGINSTATEMACHINEGRAPHICS_H

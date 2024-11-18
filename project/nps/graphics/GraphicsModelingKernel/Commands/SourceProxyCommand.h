#pragma once
#include "CanvasUndoCommand.h"
#include "SourceProxy.h"

class AddSourceProxyCommand : public CanvasUndoCommand
{
public:
    AddSourceProxyCommand(ICanvasScene *scene, QUndoCommand *parent = 0);
    ~AddSourceProxyCommand();

    void addSourceProxy(PSourceProxy source);

protected:
    void undo();

    void redo();

private:
    void createSources();

    void deleteSources();

private:
    /// @brief 资源对象
    QMap<QString, PSourceProxy> sourcesMap;
};

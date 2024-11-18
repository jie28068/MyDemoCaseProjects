#pragma once

#include "CanvasUndoCommand.h"
#include "ClipBoardMimeData.h"
#include "ConnectorWireContext.h"
#include "SourceProxy.h"

class PasteCommand : public CanvasUndoCommand
{
public:
    PasteCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    void loadClipboardMimeData(ClipBoardMimeData *mimeData);

    void setCutFlag(bool isCut = false);

protected:
    void redo();

    void undo();

private:
    static bool sortInOutSource(const PSourceProxy source1, const PSourceProxy source2);

private:
    QMap<QString, PSourceProxy> originalSourcesMap;

    QMap<QString, PConnectorWireContext> originalConnectorsMap;

    QMap<QString, PSourceProxy> newSourcesMap;

    QMap<QString, PConnectorWireContext> newConnectorsMap;

    QMap<QString, PSourceProxy> oldToNewSourceMap;

    /// @brief 保存复制的端口数据 有些模块删除连接线的时候会删除端口
    QMap<QString, QList<PPortContext>> newPortContext;

    bool isCut;

    bool isUndoRun;
};

#pragma once

#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"
#include "SourceProxy.h"

class SubSystemCommand : public CanvasUndoCommand
{
public:
    SubSystemCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    ~SubSystemCommand();

    void setSelectedRect(const QRectF &rect);

protected:
    void redo();

    void undo();

private:
    /// @brief 鼠标框选的子系统区域
    QRectF selectedRect;

    /// @brief 生成的新的子系统的资源id
    QString subsystemSourceID;

    /// @brief 框选子系统区域内的资源列表
    QList<PSourceProxy> selectedSources;

    /// @brief 框选时连接线两端都被选中的连接线
    QList<PConnectorWireContext> doubleSelectedWireContext;

    /// @brief 框选时只有一端的资源被选中的连接线
    QList<PConnectorWireContext> singleSideSelectedWireContext;
};

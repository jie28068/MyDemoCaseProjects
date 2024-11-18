#pragma once

#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"
#include "SourceProxy.h"

class ElectricalElementDirectConnectCommand : public CanvasUndoCommand
{
public:
    ElectricalElementDirectConnectCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    ~ElectricalElementDirectConnectCommand();

    /// @brief 电气元件模块ID和端口ID
    struct ElectricalElementInfo {
        QString sourceUUID;
        QString portUUID;
    };

    void setElementInfos(QPointF dotbusBarPos, ElectricalElementInfo info1, ElectricalElementInfo info2);

protected:
    void redo();

    void undo();

private:
    /// @brief 电气元件直连时，点状母线的位置
    QPointF busbarPos;

    ElectricalElementInfo electricalElementInfo1;
    ElectricalElementInfo electricalElementInfo2;

    PSourceProxy busbarSourceProxy;
    PConnectorWireContext connectorWireContext1;
    PConnectorWireContext connectorWireContext2;

    /// @brief 下面三个变量主要是辅助 撤销重做操作的，
    /// 保证每次撤销重做生成的点状母线和连接线ID不变，这样才能在其他操作中(比如位置移动)才能引用到之前的数据
    /// 为了修复bug:https://zt.xtkfpt.online/bug-view-10891.html
    QString lastBusbarUUID;
    QString lastWireUUID1;
    QString lastWireUUID2;
};

#pragma once

#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"

#include <QMap>
#include <QPointF>

class PositionCommand : public CanvasUndoCommand
{
public:
    PositionCommand(ICanvasScene *canvas, QUndoCommand *parent = 0);
    ~PositionCommand();

    void addPositionOffset(QString id, QPointF offset);
    void addnameChanged(QString id, QPair<QString, QString> name);
    bool isEmpty() override;

    void setConnectorWireContext(QMap<QString, PConnectorWireContext> last, QMap<QString, PConnectorWireContext> now);

protected:
    virtual void undo();

    virtual void redo();

protected:
    struct proxyPositionOffsetNamechanged {
        QPointF offest;                // 模块名称的偏移量
        QPair<QString, QString> names; // 修改前与修改后模块名称
    };
    // 代理图元位置偏移量与名称
    QMap<QString, proxyPositionOffsetNamechanged> proxyPositionOrNameChanged;
    /// @brief 代理图元所有连接线状态缓存，只需要关注points 和 autoCalculation 就可以了
    /// 缓存的是一个新创建的内存数据，和连接线之前的上下文数据不是同一个
    QMap<QString, PConnectorWireContext> lastConnectorWirePoints;

    QMap<QString, PConnectorWireContext> nowConnectorWirePoints;
};

class SourceNamePositionCommand : public PositionCommand
{
public:
    SourceNamePositionCommand(ICanvasScene *canvas, QUndoCommand *parent = 0);
    ~SourceNamePositionCommand();

protected:
    virtual void undo();

    virtual void redo();

private:
    void updateNamePosition(bool undoFlag);
};

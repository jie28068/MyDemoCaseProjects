#pragma once
#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"

class TransformCommand : public CanvasUndoCommand
{
public:
    struct TransformInfo {
        QTransform oldScaleTransform;
        QTransform oldRotateTransform;
        QTransform oldTranslateTransform;
        QTransform oldxFlipTransform;
        QTransform oldyFlipTransform;
        QTransform newScaleTransform;
        QTransform newRotateTransform;
        QTransform newTranslateTransform;
        QTransform newxFlipTransform;
        QTransform newyFlipTransform;
    };

    TransformCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    bool isEmpty();

    void setOldTransforms(QString id, QTransform scaleTransform, QTransform rotateTransform,
                          QTransform translateTransform, QTransform xFlipTransform, QTransform yFlipTransform);

    void setNewTransforms(QString id, QTransform scaleTransform, QTransform rotateTransform,
                          QTransform translateTransform, QTransform xFlipTransform, QTransform yFlipTransform);

    void setConnectorWireContext(QMap<QString, PConnectorWireContext> last, QMap<QString, PConnectorWireContext> now);

protected:
    void undo();

    void redo();

private:
    QMap<QString, TransformInfo> transformProxyMap;

    /// @brief 代理图元所有连接线状态缓存，只需要关注points 和 autoCalculation 就可以了
    /// 缓存的是一个新创建的内存数据，和连接线之前的上下文数据不是同一个
    QMap<QString, PConnectorWireContext> lastConnectorWirePoints;

    QMap<QString, PConnectorWireContext> nowConnectorWirePoints;
};

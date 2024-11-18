#pragma once

#include "AutoConnectionPromptGraphics.h"
#include "CanvasContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "ConnectorWirePrompt.h"
#include "CopyRightGraphicsObject.h"
#include "CustomUndoStack.h"
#include "GraphicsLayerManager.h"
#include "PortGraphicsObject.h"
#include "PositionCommand.h"
#include "SourceGraphicsObject.h"
#include "SubsystemAreaGraphics.h"
#include "TransformItemGroup.h"

#include <QAction>
#include <QSharedPointer>
#include <QString>
#include <QTimer>
#include <QUndoStack>
#include <QVector>

class CanvasSceneDefaultImplPrivate
{
public:
    CanvasSceneDefaultImplPrivate();
    ~CanvasSceneDefaultImplPrivate();
    QSharedPointer<CanvasContext> canvasContext;
    QMap<QString, TransformProxyGraphicsObject *> proxyGraphicsMap;
    QMap<QString, ConnectorWireGraphicsObject *> connectorGraphicsMap;
    QSharedPointer<CustomUndoStack> undoStack;
    QSharedPointer<GraphicsLayerManager> graphicsLayerManager;
    bool isMouseLeftPressed;
    PositionCommand *positionCommand;
    SourceNamePositionCommand *sourceNamePositionCommand;
    CopyRightGraphicsObject *copyRightGraphicsObject;
    QPolygonF m_pts;
    bool m_isOpenBoardOperation;
    QPointF mouseClickedPos;
    TransformItemGroup *itemGroup;
    bool isLoadingCanvas;
    QTimer *scrollTimer;
    ICanvasView *canvasView;
    QPainterPath transformProxyGraphicsPath;
    // 绘制背景点状线时的点坐标
    QPolygon backgroundPoints;
    // 绘制背景网格线时的线段
    QVector<QLineF> backgroundLines;
    /// @brief 端口自动连接提示图元
    AutoConnectionPromptGraphics *autoConnectionPromptGraphics;

    /// @brief 鼠标点击选中的端口,这个端口必须是可以连接的
    PortGraphicsObject *mouseClickedPort;

    /// @brief 按住ctrl键时点击的代理图层，每次点击都会更新
    TransformProxyGraphicsObject *lastClickedProxyGraphics;

    /// @brief 标记当前场景内是否包含代理图元/图元组正在移动状态
    bool hasItemMoving;
    /// 是否为文本改变，与移动文本做区别
    bool isTextNameChanged;
    /// 端口连接提示
    ConnectorWirePrompt *promptWire;

    bool isIgnoreLayerManager;

    QList<TransformProxyGraphicsObject *> selectedTransformProxys;

    SubsystemAreaGraphics *subsystemAreaGraphics;
};

CanvasSceneDefaultImplPrivate::CanvasSceneDefaultImplPrivate()
{
    undoStack = QSharedPointer<CustomUndoStack>(new CustomUndoStack());
    undoStack->setUndoLimit(100);
    isMouseLeftPressed = false;
    positionCommand = nullptr;
    sourceNamePositionCommand = nullptr;
    m_isOpenBoardOperation = true;
    mouseClickedPos = QPointF(0, 0);
    itemGroup = nullptr;
    isLoadingCanvas = false;
    scrollTimer = nullptr;
    canvasView = nullptr;
    autoConnectionPromptGraphics = nullptr;
    mouseClickedPort = nullptr;
    lastClickedProxyGraphics = nullptr;
    hasItemMoving = false;
    isTextNameChanged = false;
    promptWire = nullptr;
    isIgnoreLayerManager = false;
    copyRightGraphicsObject = nullptr;
    subsystemAreaGraphics = nullptr;
}

CanvasSceneDefaultImplPrivate::~CanvasSceneDefaultImplPrivate() { }
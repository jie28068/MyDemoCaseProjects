#pragma once

#include "ActionManager.h"
#include "Global.h"
#include <QGraphicsView>

class CanvasContext;
class SourceProxy;
class ConnectorWireContext;
class ICanvasScene;
class AnnotationSourceGraphicsObject;
class GraphicsLayerManager;

class ICanvasView : public QGraphicsView
{
    Q_OBJECT
public:
    ICanvasView(QWidget *parent = 0);
    virtual ~ICanvasView(void);

    //  添加模块
    virtual bool addSource(QSharedPointer<SourceProxy> source, bool needUndo) = 0;

    /// @brief 删除模块
    virtual bool deleteSource(QString uuid) = 0;

    // 添加注解图元
    virtual bool addAnnotationSource(QSharedPointer<SourceProxy> source) = 0;

    // 添加连接线
    virtual bool addWire(QSharedPointer<ConnectorWireContext> wire) = 0;

    // 删除连接线
    virtual bool deleteWire(QString uuid) = 0;

    // 加载画板信息
    virtual bool loadCanvas(QSharedPointer<CanvasContext> canvasContext) = 0;

    virtual QSharedPointer<ICanvasScene> getCanvasScene() = 0;

    /// 交互
    virtual void scrollToBlock(QString activeBlockID) = 0; // 定位到指定模块

    virtual void setFullscreenMode(bool bfull) = 0; // 画板全屏变化

    virtual bool getIsFullScreen() = 0;        // 当前画板是否全屏
    virtual void activeFullScreenAction() = 0; // 手动激活全屏Action

    virtual void widgetShow(const QString &canvasName, const QString &canvasuuid) = 0; // 画板显示
    virtual void widgetHide(const QString &canvasName, const QString &canvasuuid) = 0; // 画板隐藏

    virtual QWidget *getToolBar() = 0; // 获取工具栏

    /// @brief 设置运行状态，运行状态不可编辑画板。运行状态停止，需要复归画板状态。
    /// @param activeCanvasUuid 当前显示的画板
    /// @param bRunning 是否运行
    virtual void setRunningStatus(bool bRunning) = 0; // 设置是否运行。

    virtual void setCanCreateBlockList() = 0; // 设置可以创建的模块列表

    virtual QSharedPointer<GraphicsLayerManager> getGraphicsLayerManager() = 0;

    /// @brief 保存时清空组
    virtual void transformItemGroupClear() = 0;

    /// @brief 处理QAction事件
    /// @param type 事件类型
    virtual void processAction(ActionManager::ActionType type, bool checked) = 0;

    /// @brief 获取能容纳当前画板所有模块的最小size
    virtual QRectF getMinRect() = 0;

    /// @brief 把画板内换成svg图片内容
    /// @return
    virtual QSharedPointer<QByteArray> saveAsPngData() = 0;

    virtual void setSearchHighLight(QStringList &uuid) = 0;

    virtual void setNextConstructiveBoardStatus(bool falg) = 0;

    virtual void cleanCache() = 0;

    virtual bool isMiddleButtonPressed() = 0;

signals:
    /// @brief 画板数据开始加载
    void canvasLoadStart();

    /// @brief 画板数据加载完成
    void canvasLoadFinished();

    /// @brief
    void sourceDeleted(QString canvasUUID, QSharedPointer<SourceProxy> source);

    /// @brief 连接线删除信号
    /// @param canvasName 画板名称
    /// @param context 连接线上下文信息
    void connectorWireDeleted(QString canvasUUID, QSharedPointer<ConnectorWireContext> context);

    // 新增连接线信息
    void connectorWireAdded(QString canvasUUID, QSharedPointer<ConnectorWireContext> context);

    // 连接线端口变更新型号
    void connectorWirePortChanged(QString canvasUUID, QSharedPointer<ConnectorWireContext> context);
};

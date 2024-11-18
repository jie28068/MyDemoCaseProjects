#ifndef ICANVASSCENE_H
#define ICANVASSCENE_H

#include "ActionManager.h"
#include "Global.h"
#include <QGraphicsScene>
#include <QUndoStack>

class CanvasContext;
class SourceProxy;
class SourceGraphicsObject;
class PortGraphicsObject;
class ConnectorWireGraphicsObject;
class GraphicsLayer;
class ConnectorWireContext;
class GraphicsLayerManager;
class ICanvasView;
class TransformProxyGraphicsObject;
class PortContext;
class TransformItemGroup;
struct WireParameters;
class CustomUndoStack;

class ICanvasScene : public QGraphicsScene
{
    Q_OBJECT

public:
    ICanvasScene(QObject *parent);
    virtual ~ICanvasScene();

    // 打印接口
    virtual bool print() = 0;

    virtual void initCopyRight() = 0;

    /// @brief 添加SourceProxy到Scene
    /// @param source 模块
    /// @param needUodo 是否需要撤销/重做
    /// @return 代理图元
    virtual TransformProxyGraphicsObject *addSourceToScene(QSharedPointer<SourceProxy> source,
                                                           bool needUodo = true) = 0;

    /// @brief 删除SourceProxy
    /// @param source 删除的模块
    /// @return 是否成功
    virtual bool deleteSourceFromScene(QSharedPointer<SourceProxy> source) = 0;

    /// @brief 根据已有SourceProxy加载
    virtual bool loadFromSourceProxy(QSharedPointer<SourceProxy> source) = 0;

    /// @brief 获取画板上下文信息
    virtual QSharedPointer<CanvasContext> getCanvasContext() = 0;

    virtual void addTransformProxyGraphicsObject(TransformProxyGraphicsObject *transformGraphics) = 0;

    virtual void deleteTransformProxyGraphicsObject(TransformProxyGraphicsObject *transformGraphics) = 0;

    /// @brief 添加连接线图元
    /// @param connectorWireGraphics
    virtual void addConnectorWireGraphicsObject(ConnectorWireGraphicsObject *connectorWireGraphics,
                                                bool canUndo = false) = 0;

    /// @brief 删除连接线图元
    /// @param connectorWireGraphics
    virtual void deleteConnectorWireGraphicsObject(ConnectorWireGraphicsObject *connectorWireGraphics) = 0;

    /// @brief 删除当前画板所有连接线图元
    /// @param connectorWireGraphics
    virtual void deleteAllConnectorWireGraphicsObject() = 0;

    /// @brief 删除当前某个SourceProxy对应的所有连接线图元
    /// @param connectorWireGraphics
    virtual void deleteConnectorWireGraphicsObjectById(QString uuid) = 0;

    /// @brief 根据连接线id，获取连接线图元对象
    /// @param id 连接线id
    /// @return
    virtual ConnectorWireGraphicsObject *getConnectorWireGraphicsByID(QString id) = 0;

    /// @brief 根据id获取代理图元对象
    /// @param id 代理图元id
    /// @return
    virtual TransformProxyGraphicsObject *getTransformProxyGraphicsByID(QString id) = 0;

    /// @brief 根据资源获取代理图层
    /// @param source
    /// @return
    virtual TransformProxyGraphicsObject *getTransformProxyGraphicsBySource(QSharedPointer<SourceProxy> source) = 0;

    /// @brief 获取代理图层对象的列表
    /// @return
    virtual QList<TransformProxyGraphicsObject *> getTransformProxyGraphicsList() = 0;

    /// @brief 获取当前被选中的代理图层列表
    /// @return
    virtual QList<TransformProxyGraphicsObject *> getSelectedTransformProxyGraphicsList() = 0;

    /// @brief 根据端口上下文获取端口图元
    /// @param portCtx
    virtual PortGraphicsObject *getPortGraphics(PPortContext ctx) = 0;

    /// @brief 根据资源和端口ID获取端口图元
    /// @param sourceID
    /// @param portID
    /// @return
    virtual PortGraphicsObject *getPortGraphics(const QString &sourceID, const QString &portID) = 0;

    /// @brief 当前场景的undostack
    /// @return
    virtual QSharedPointer<CustomUndoStack> getUndoStack() = 0;

    /// @brief 当前场景的图层管理类实例
    /// @return
    virtual QSharedPointer<GraphicsLayerManager> getGraphicsLayerManager() = 0;

    /// @brief 获取背景网格线宽度
    /// @return
    virtual unsigned int getGridSpace();

    /// @brief 是否自动对齐网格
    /// @return
    virtual bool isAutoAlignmentGrid();

    /// @brief 是否允许图元重叠(不包含连接线)
    /// @return
    virtual bool allowOverlap();

    /// @brief 获取当前场景的图元组对象
    /// @return
    virtual TransformItemGroup *getTransformItemGroup() = 0;

    /// @brief 设置图元组
    /// @param group
    virtual void setTransformItemGroup(TransformItemGroup *group) = 0;

    /// @brief 当前场景是否正在从内存、磁盘中加载画板
    /// @return
    virtual bool isLoadingCanvas() = 0;

    /// @brief 设置场景的加载状态
    /// @param status
    virtual void setLoadingCanvasStatus(bool status) = 0;

    /// @brief 设置显示当前场景的ICanvasView视图对象
    /// @param view
    virtual void setCanvasView(ICanvasView *view) = 0;

    /// @brief 获取当前场景关联的ICanvasView视图对象
    /// @return
    virtual ICanvasView *getCanvasView() = 0;

    /// @brief 所有代理图层边界组成的painterpath
    /// @return
    virtual QPainterPath getTransformProxyGraphicsPainterPath() = 0;

    /// @brief 查找矩形rect内包含的所有代理图元
    /// @return
    virtual QList<TransformProxyGraphicsObject *> transProxyAtRectF(QRectF rect) = 0;

    /// @brief 设置被鼠标点击的端口图元，每次点击端口都会更新
    /// @param port
    virtual void setMouseClickedPort(PortGraphicsObject *port);

    /// @brief 获取被鼠标点击的端口图元
    /// @return
    virtual PortGraphicsObject *getMouseClickedPort();

    /// @brief 是否有代理图元/图元组正在移动中
    /// @return
    virtual bool getTransformProxyChanging() = 0;

    /// @brief 标记场景中是否有代理图元/图元组在移动中
    /// @param isMoving
    virtual void setTransformProxyChanging(bool isMoving) = 0;

    /// @brief 根据连接线参数批量刷新连接线
    /// @param wireParameters
    virtual void refreshConnectorWires(QList<QSharedPointer<WireParameters>> wireParameters) = 0;

    /// @brief 设置连接线提示的起始端点
    virtual void setPromptWirePorts(PortGraphicsObject *port1, PortGraphicsObject *port2) = 0;

    /// @brief 场景资源是否释放,忽略图层管理处理
    virtual bool isIgnoreLayerManager() = 0;

    virtual void freeSource() = 0;

    /// @brief 处理QAction事件
    /// @param type 事件类型
    virtual void processAction(ActionManager::ActionType type) = 0;

    /// @brief 刷新输入和输出模块显示的序列号
    virtual void refreshInputAndOutPutIndex() = 0;

    virtual void resetSubsystemGraphics() = 0;

    /// @brief 获取图例的boundingRect
    virtual QRectF getCopyRightRect() = 0;

signals:
    /// @brief 代理图元/图元组移动结束信号
    void transformProxyChangedSignal();

public slots:
    virtual void onBackgroundColorChanged(QColor color) = 0;

    virtual void onSizeChanged(QSize size) = 0;

    virtual void onScaleChanged(int scale) = 0;

private:
};

typedef QSharedPointer<ICanvasScene> PCanvasScene;

#endif // ICANVASSCENE_H

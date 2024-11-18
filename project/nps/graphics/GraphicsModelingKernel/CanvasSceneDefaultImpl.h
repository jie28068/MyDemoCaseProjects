#ifndef CANVASSCENEDEFAULTIMPL_H
#define CANVASSCENEDEFAULTIMPL_H

#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "CustomUndoStack.h"
#include "ICanvasScene.h"
#include "KColorDialog.h"
#include <QClipboard>
#include <QKeyEvent>
#include <QScopedPointer>
static const QString BlockTab_BoardLayerConfig = QObject::tr("Layer Configuration"); // 图层配置

class TransformProxyGraphicsObject;
class QClipboard;
class CanvasSceneDefaultImplPrivate;
class ICanvasView;
class AutoConnectionPromptGraphics;

class CanvasSceneDefaultImpl : public ICanvasScene
{
    Q_OBJECT

    friend class CanvasViewDefaultImpl;
    friend class TransformProxyGraphicsObject;
    friend class TransformItemGroup;
    friend class ConnectorWireGraphicsObject;

public:
    enum BrackGound { brackgoundColor, brackgoundImage, foregoundImage };
    CanvasSceneDefaultImpl(QSharedPointer<CanvasContext> canvasContext, QObject *parent = 0);
    ~CanvasSceneDefaultImpl();

    // 打印接口
    virtual bool print();

    virtual void initCopyRight();

    TransformProxyGraphicsObject *addSourceToScene(QSharedPointer<SourceProxy> source, bool needUodo = true);

    bool deleteSourceFromScene(QSharedPointer<SourceProxy> source);

    bool loadFromSourceProxy(QSharedPointer<SourceProxy> source);

    virtual QSharedPointer<CanvasContext> getCanvasContext() override;

    /// @brief 添加资源代理图元
    /// @param transformGraphics
    void addTransformProxyGraphicsObject(TransformProxyGraphicsObject *transformGraphics);

    /// @brief 删除资源代理图元
    /// @param transformGraphics
    void deleteTransformProxyGraphicsObject(TransformProxyGraphicsObject *transformGraphics);

    /// @brief 添加连接线
    /// @param connectorWireGraphics 连接线对象
    /// @param canUndo 是否加入到undostack
    virtual void addConnectorWireGraphicsObject(ConnectorWireGraphicsObject *connectorWireGraphics,
                                                bool canUndo) override;

    virtual void deleteConnectorWireGraphicsObject(ConnectorWireGraphicsObject *connectorWireGraphics) override;

    virtual void deleteAllConnectorWireGraphicsObject() override;

    virtual void deleteConnectorWireGraphicsObjectById(QString uuid) override;

    virtual ConnectorWireGraphicsObject *getConnectorWireGraphicsByID(QString id) override;

    virtual TransformProxyGraphicsObject *getTransformProxyGraphicsByID(QString id) override;

    virtual QList<TransformProxyGraphicsObject *> getTransformProxyGraphicsList() override;

    /// @brief 获取当前被选中的代理图层列表
    /// @return
    virtual QList<TransformProxyGraphicsObject *> getSelectedTransformProxyGraphicsList() override;

    virtual TransformProxyGraphicsObject *
    getTransformProxyGraphicsBySource(QSharedPointer<SourceProxy> source) override;

    virtual QSharedPointer<CustomUndoStack> getUndoStack() override;

    virtual QSharedPointer<GraphicsLayerManager> getGraphicsLayerManager() override;

    virtual bool allowOverlap() override;

    virtual PortGraphicsObject *getPortGraphics(PPortContext ctx);

    virtual PortGraphicsObject *getPortGraphics(const QString &sourceID, const QString &portID);

    virtual bool isLoadingCanvas();

    virtual void setLoadingCanvasStatus(bool status);

    QPointF getMouseClickedPos();

    virtual TransformItemGroup *getTransformItemGroup();

    virtual void setTransformItemGroup(TransformItemGroup *group);

    virtual void setCanvasView(ICanvasView *view);

    virtual ICanvasView *getCanvasView();

    /// @brief 所有代理图层边界组成的painterpath
    /// @return
    virtual QPainterPath getTransformProxyGraphicsPainterPath();

    QList<QGraphicsItem *> selectedItems() const;

    /// @brief 将选中的模块转成图元组(只把连接线和代理图元加入到组内，其他的图元全部取消选中状态)
    void makeSelectedItemAsGroup(QList<QGraphicsItem *> items);

    virtual QList<TransformProxyGraphicsObject *> transProxyAtRectF(QRectF rect);

    /// @brief 获取端口自动连接图元提示图层
    /// @return
    AutoConnectionPromptGraphics *getAutoConnectionPromptGraphics();

    /// @brief 创建端口自动连接的图元提示图层
    /// @return
    AutoConnectionPromptGraphics *createAutoConnectionPromptGraphics();

    virtual void setMouseClickedPort(PortGraphicsObject *port);

    virtual PortGraphicsObject *getMouseClickedPort();

    /// @brief 是否有代理图元/图元组正在移动中
    /// @return
    virtual bool getTransformProxyChanging();

    /// @brief 标记场景中是否有代理图元/图元组在移动中
    /// @param isMoving
    virtual void setTransformProxyChanging(bool isMoving);
    /// @brief 模块名称修改，Undo入堆
    /// @param id 模块ID
    /// @param name 修改前后名称
    void nameChanged(QString id, QPair<QString, QString> name);

    static QVariantMap wireComputation(const QSharedPointer<WireParameters> wireParameter);

    virtual void refreshConnectorWires(QList<QSharedPointer<WireParameters>> wireParameters);

    virtual void setPromptWirePorts(PortGraphicsObject *port1, PortGraphicsObject *port2);

    virtual bool isIgnoreLayerManager();

    void setIgnoreLayerManager(bool ignore);

    /// @brief 根据连接线提示创建连接线
    void createNewConnectWireByPrompt();

    virtual void freeSource();

    virtual void refreshInputAndOutPutIndex();

    virtual void resetSubsystemGraphics();

    /// @brief goto和from模块配对设置图元是否高亮
    void setHightLightTrans(PSourceProxy source);

    /// @brief 获取表格图元的场景区域
    /// @return
    virtual QRectF getCopyRightRect();

    /// @brief 获取所有item组成的区域rect用于场景rect的自适应缩小
    QRectF getAllItemUnitedRect();

public slots:
    virtual void onBackgroundColorChanged(QColor color);

    virtual void onSizeChanged(QSize size);

    virtual void onScaleChanged(int scale);

    virtual void drawBackground(QPainter *painter, const QRectF &rect);

    void onBreakLinkLine();

    void onUpdateModule();

    void onSceneRectChanged(const QRectF &rect);

    void onSourceNamePositionChanged(QPointF pointDelta);

    void onScrollTimerOut();

    void onTransformProxyTransformChanged(QRectF rect, int angle);

    void onNewConnectWireShouldBeCreate(PortGraphicsObject *port1, PortGraphicsObject *port2);

    void onTransformProxySelectedChanged(bool selected);

    void onDeleteSource(QString uuid);

    void onCanvasPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

    void onCanvasGridFlagChanged(int oldFlag, int newFlag);

    void onSceneSelectionsChanged();

    void onRefreshCanvas();

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    virtual void keyPressEvent(QKeyEvent *event) override;

    virtual void keyReleaseEvent(QKeyEvent *event) override;

protected:
    void bindSignals();

    /// @brief 将被选择的图元按照中心点旋转特定角度
    /// @param angle 旋转角度
    void rotateOnCenter(int angle, Qt::Axis axis = Qt::ZAxis);

private:
    bool getPasteStatus();

    /// @brief 鼠标拖动图元，超出视图窗口边界时，自动滚动视图到场景坐标scenePos
    /// @param scenePos
    void autoScroll(QPointF scenePos);

    /// @brief 刷新所有代理图层组成的painterpath
    void refreshTransformProxyGraphicsPainterPath();

    /// @brief 获取上次按住ctrl时点击的代理图层对象
    TransformProxyGraphicsObject *getLastClickedPorxyGraphics();

    void refreshLastClickedProxyGraphics(TransformProxyGraphicsObject *proxyGraphics);

    /// @brief 处理选中图元的变更，鼠标选中结束和快捷键全选时处理
    void processSelectionChanged();

    void processAction(ActionManager::ActionType type);

    void updateActionStatus();
    /// @brief 设置背景/前景图片/颜色
    /// @param
    void setBrackgroundImage(BrackGound gound);

private:
    QScopedPointer<CanvasSceneDefaultImplPrivate> dataPtr;
};

#endif // CANVASSCENEDEFAULTIMPL_H

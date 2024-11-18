#pragma once
#include "BookMark.h"
#include "CanvasContext.h"
#include "CanvasNavigationPrview.h"
#include "ICanvasView.h"
#include "KLWidgets/KMessageBox.h"
#include "SourceNameTextGraphicsObject.h"
#include "exportimage.h"

#include "TransformItemGroup.h"
#include <QGraphicsOpacityEffect>
#include <QScopedPointer>
#include <QWheelEvent>
#include <qfiledialog.h>
#include <qlineedit.h>

class CanvasViewDefaultImplPrivate;
class ViewToolBar;
class SliderScale;
class SearchBox;
class GraphicsLayer;
class BookMark;
class ExportImage;
class ICanvasScene;

class CanvasViewDefaultImpl : public ICanvasView
{
    Q_OBJECT
public:
    CanvasViewDefaultImpl(QSharedPointer<CanvasContext> canvasContext, QWidget *parent = 0);

    ~CanvasViewDefaultImpl();

    virtual bool addSource(QSharedPointer<SourceProxy> source, bool needUndo) override;
    virtual bool deleteSource(QString uuid) override;
    virtual bool addWire(QSharedPointer<ConnectorWireContext> wire) override;
    virtual bool deleteWire(QString uuid) override;

    virtual bool addAnnotationSource(QSharedPointer<SourceProxy> source) override;

    virtual QSharedPointer<ICanvasScene> getCanvasScene() override;

    /// 交互
    virtual void scrollToBlock(QString activeBlockID) override; // 定位到指定模块

    virtual void setFullscreenMode(bool bfull);

    virtual bool getIsFullScreen() override; // 当前画板是否全屏

    virtual void activeFullScreenAction() override; // 手动激活全屏Action

    virtual void widgetShow(const QString &canvasName, const QString &canvasuuid) override;
    virtual void widgetHide(const QString &canvasName, const QString &canvasuuid) override;

    virtual QWidget *getToolBar(); // 获取工具栏

    virtual void setRunningStatus(bool bRunning) override; // 设置是否运行。

    virtual void setCanCreateBlockList();

    virtual QSharedPointer<GraphicsLayerManager> getGraphicsLayerManager();

    virtual void transformItemGroupClear() override;

    QRectF getSyncViewRect();

    virtual QRectF getFitinRect();

    virtual QSharedPointer<QByteArray> saveAsPngData();

    bool saveAsSVGFile(QString filePath);

    virtual QRectF getMinRect();

    virtual void setNextConstructiveBoardStatus(bool falg) override;

    /// @brief 清理视图的缓存，代表该窗口对象将要被释放
    /// 解决问题:
    /// 包含构造型模块作为子模块的画板，在双击打开了构造型模块之后，如果构造型画板增加、删除模块之后，同步之后关闭窗口时，点击否
    /// 会触发重新加载模型，这个时候会创建一个新的canvasContext，对应的uuid和之前的保持一致，那么在addsource的时候会从缓存的视图中查找
    /// 就会把应该同步到新的canvasContext中的数据添加到将要被删除的画板数据中。
    /// 而且由于删除TransformProxyGraphics的时候采用deleteLater(已经修改成删除即delete) 会造成闪退的情况
    virtual void cleanCache() override;

    virtual bool isMiddleButtonPressed() override;

    void resetAnnotationGraphicsObject(SourceGraphicsObject *sourceGraphicsObject);

public slots:

    /// @brief 视图缩放
    /// @param percent 缩放比例(0~5.0)
    void zoom(QString key, QVariant val);

    void onTimerShot(); // 定时器触发 窗口聚焦事件

    void onCreateBlock(QString protype, QString text);

protected:
    /// @brief 初始化场景
    void initScene();

    void bindSignals();

    void createViewToolBar();

    /// @brief 刷新工具栏电压和相位数据
    void refreshToolBarComboBox();

    // 加载画板信息
    virtual bool loadCanvas(QSharedPointer<CanvasContext> canvasContext) override;

    void saveCenterPos();
    void actionZoomInOut(bool zoomin);
    void actionFitIn();

    void setViewRect(const QRectF &fitIn);
    QPointF checkEdge(QPointF prev);

    void highlightElectricSource();

    void sceneSizeChanged();

    void syncPreviewRect();

    void setPreviewWidget(); // 设置导航窗格窗口

    void actionShowAlonePortName();                 // 是否显示端口名
    bool isEixstBlockCtrlCode();                    // 遍历控制画板是否存在构造型与用户自定义型
    bool isEixstHidePort();                         // 画板所有模块中是否存在隐藏端口
    void setFullscreenStatuIcon(bool falg = false); // 设置全屏的样式用以设置图标
    void setLockStatusIcon(bool falg = false);      // 设置锁定的样式用以设置图标

protected:
    virtual void wheelEvent(QWheelEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual void resizeEvent(QResizeEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    virtual void enterEvent(QEvent *event);

    virtual bool viewportEvent(QEvent *event) override;

    virtual void scrollContentsBy(int dx, int dy) override;

    virtual void setSearchHighLight(QStringList &uuid) override;

signals:
    void sceneViewRectChanged(QRectF rect);

private slots:
    void onScaleChanged(const QString &text);
    void onVolLevelChanged(const QString &text);
    void onPhaseTeckChanged(const QString &text);
    void onSliderScaleValueChanged(int value);

    void onSearchBlock(QStringList blockuuid);
    void onCanvasSettingChanged(QString key, QVariant value);

    void onAddSourceChange(QSharedPointer<SourceProxy> source);
    void onDeleteSourceChange(QSharedPointer<SourceProxy> source);
    void onSourceChanged(QString sourcekey, QVariant value);

    /// @brief 响应鼠标在导航预览窗口点击或移动时发出的信号
    /// @param pos 鼠标的场景位置
    void onPreviewSceneChanged(QPointF pos);
    /// @brief 书签切换画板时中心位置
    /// @param uuid
    /// @param pos
    void onPreviewSceneChangeBookMark(QString uuid, QPointF pos);

    void onPreviewSceneChangeFinished(QPointF pos);

    /// @brief 响应画板是否显示/隐藏导航窗口
    /// @param visible
    void onShowNavigationPreview(bool visible);

    /// @brief 注解模块交互模式结束
    void onAnnotationInteractionFinished();
    /// @brief 注解模块被取消
    void onAnnotationCanceled();
    /// @brief 缩放
    void onPreviewScaleChanged(QString uuid, double value);
    /// @brief 场景改变
    /// @param value
    void onsceneRectChanged(const QRectF &value);

    void onActiveProgram();

    void onEditTextName();

    void onFinishEditTextName();
    /// @brief 书签数量限制
    void onBookMarkNumberLimit();

    void onCanvasInteractionModeChanged(QString canvasUUID, int newMode, int oldMode);

    /// brief 画板状态发生变化
    void onCanvasStatusFlagsChanged(CanvasContext::StatusFlag flag);

    void onPreviewEscapeKeypressed(QEvent *event);

private:
    /// @brief 处理QAction事件
    /// @param type 事件类型
    void processAction(ActionManager::ActionType type, bool checked);

    /// @brief 注解模式下，更新鼠标形状
    void updateAnnotationCursor(ActionManager::ActionType type);

    ///  将用到快捷键的action注册给画板
    void registerAction();

    void subsystemInitialize();

private:
    QScopedPointer<CanvasViewDefaultImplPrivate> dataPtr;
};

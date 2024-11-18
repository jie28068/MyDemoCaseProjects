#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "CanvasContext.h"
#include "Global.h"

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

/// @brief 菜单和快捷键管理
class ActionManagerPrivate;
class ActionInfo;
class ICanvasScene;
class ICanvasView;
class CanvasContext;

class GRAPHICSMODELINGKERNEL_EXPORT ActionManager : public QObject
{
    Q_OBJECT
public:
    enum ActionType {
        startType = 0,

        /// @brief 画板相关的action
        Set,                 // 设置
        ZoomIn,              // 放大
        ZoomOut,             // 缩小
        DispNavigationPane,  // 显示、隐藏导航窗格
        ExportBorad,         // 导出画板图形
        Prints,              // 打印
        LayoutManager,       // 图层配置窗口
        SelectAll,           // 全选
        LayoutManagerNoIcon, // 无图标的图层配置窗口

        /// @brief 注解相关的action
        Textbox,    // 文本框
        Icon,       // 图片
        Rectangle,  // 矩形框
        Circular,   // 圆形框
        Segment,    // 线段
        BrokenLine, // 折线
        Arrows,     // 箭头
        LineArrow,  // 折线箭头
        Arc,        // 圆弧
        Polygon,    // 多边形

        /// @brief 旋转变化相关的action
        RotateClockwise,     // 顺时针旋转
        RotateAntiClockwise, // 逆时针旋转
        Rotate180,           // 旋转180度
        FlipVertical,        // 垂直翻转
        FlipHorizontal,      // 水平翻转
        RotateAll,           // 旋转 包含向右旋转、向左旋转、180度旋转

        /// @brief 编辑菜单相关的action
        Copy,   // 复制
        Paste,  // 粘贴
        Cut,    // 剪切
        Delete, // 删除

        /// @brief 撤销重做的的action状态需要和画布的撤销重做堆栈的状态对应
        Undo,       // 撤销
        Redo,       // 重做
        UndoNoIcon, // 撤销无图片
        RedoNoIcon, // 重做无图片

        OpenModel,       // 打开模型
        BreakLinkLine,   // 断开连接线
        updateModule,    // 更新模块 用于已经修改的构造型模块
        ElementProperty, // 元件属性

        Help, // 帮助

        LineAutoLayout,    // Line自动布局
        DrawBoardProperty, // 画板属性窗口

        // 电气仿真快捷
        CaseCreator, // 临时action,生成case文件
        AddScope,    // 添加到波形显示

        // 视图工具栏Action,具有图标的action
        Magnify, // 区域放大

        FitIn,          // 画板自适应比例
        FullScreen,     // 全屏
        PrintDrawBoard, // 打印画板

        Search,         // 搜索
        ScreenDrag,     // 抓手
        Lock,           // 锁定
        SaveImage,      // 保存图片
        LockReplica,    // 右键菜单的锁定（隐藏勾）
        AddBookmark,    // 添加书签
        ManageBookmark, // 管理书签

        // 右键菜单
        //  PowerFactory电气画板

        Colour,       // 着色
        ShowCoverage, // 显示/隐藏图例层
        ShowResult,   // 显示结果层
        // GraphicFreezeMode,		//图形冻结模式

        Export,       // 导出
        ExportFMU,    // 导出FMU
                      // 使用SelectAll			//选择全部
        ShowPortName, // 显示/隐藏模块端口名

        EditAndBrowseData, // 编辑和浏览数据

        // 剩下的电气画板已包含
        BreakAllLinkLine, // 断开所有连接线
        PecadCase,        // 生成pecad
        SwitchBusbarType, // 切换母线类型

        SetReplica, // 右键菜单的设置

        MarkThrough, // 标记为直通

        MarkDisable, // 标记为禁用

        Unmark, // 取消直通/禁用标记

        /// @brief 右键设置背景
        SetBackgrounds,     // 设置背景
        SetBackgroundColor, // 设置背景颜色
        SetForegroundImage, // 设置前景图片
        SetBackgroundImage, // 设置背景图片

        ResetBackgroundColor, // 重置背景颜色
        ResetBackgroundImage, // 重置背景图片
        ResetForegroundImage, // 重置前景图片

        // @brief 导出为预览svg图
        ExportASPreviewSVG,

        // 测试调试时使用
        PrintErrorInfos, // 输出错误信息，(模型中多出了连接线和模块时，输出相关信息)

        EndType, // 标记菜单值结尾

    };

    enum ActionStatus {
        kActionNormal,                // 正常状态，所有的action enable = true
        kActionNotBoard,              // 工作区不在画板窗口，那么画板图元相关操作 enable = false
        kActionBoardItemsNotSelected, // 工作区在画板窗口，但是没有图元被选中
        kActionFreeze,                // 画板冻结状态，不能有操作。
        kActionRunning,               // 画板运行状态，不能有操作。
        kActionErrorSourceSelected,   // 有错误控制模块被选中  "disable" "error"
    };

    /// @brief  获取ActionManager实例对象
    /// @return
    static ActionManager &getInstance();

    /// @brief 获取指定类型的QAction对象
    QAction *getAction(ActionManager::ActionType type);

    QList<QAction *> getActionList(const QList<ActionManager::ActionType> &types);

    /// @brief 关联画板对象
    /// @param canvasView
    void attachCanvasView(ICanvasView *canvasView);

    /// @brief 取消和画板对象的关联
    /// @param canvasView
    void detachCanvasView(ICanvasView *canvasView);

    void detachCanvasView(QString uuid);

    /// @brief 场景菜单
    /// @param menu
    void sceneMenu(QMenu *menu, QSharedPointer<CanvasContext> ctx);

    /// @brief 图元组菜单
    /// @param menu 菜单
    /// @param ctx 画布上下文
    /// @param hasConnectorWires 图元组内的模型资源是否有连接线连接
    /// @param hasControlCodeModel 图元组内的模型是否包含控制代码型模块
    void graphicsGroupMenu(QMenu *menu, QSharedPointer<CanvasContext> ctx, bool hasConnected, bool hasControlCodeModel);

    /// @brief 获取注解模式下,几何图元注解的类型字符串
    /// @return 非注解模式，图片、文字注解返回值为空字符串
    QString getAnnotationTypeString();

    QString getActivateCanvasUUID();

    /// @brief 设置当前激活的画板场景
    /// @param canvasscene 画板场景，为nullptr表示没有画板被激活，可能是示波器窗口
    /// @return
    void setActivateCanvas(QSharedPointer<ICanvasScene> canvasscene);

    QActionGroup *getCommonGroup();

    QActionGroup *getTransformActionGroup();

    QActionGroup *getAnnotationGroup();

    QActionGroup *getCopyAbleActionGroup();

    QActionGroup *getOperateActionGroup();

    QActionGroup *getCanvasActionGroup();

    /// @brief 设置菜单的不可用状态，包含初始状态，画板窗口被关闭时
    void disable();

public slots:
    void onCanvasInteractionModeChanged(QString canvasUUID, int newMode, int oldMode);

    void onClipboardDataChanged();

    void onUndoStatusChanged(bool canUndo);

    void onRedoStatusChanged(bool canRedo);

private slots:
    void onActionTriggered(bool checked);

private:
    ActionManager(QObject *parent = 0);
    ~ActionManager();

    void initCommonActionGroup();

    void initTransformActionGroup();

    void initAnnotationActionGroup();

    void initCopyAbleActionGroup();

    void initOperateActionGroup();

    void initCanvasActionGroup();

    QAction *createAction(ActionType type, ActionInfo *info);

private:
    QScopedPointer<ActionManagerPrivate> dataPtr;
};

Q_DECLARE_METATYPE(ActionManager::ActionType);

#endif // ACTIONMANAGER_H

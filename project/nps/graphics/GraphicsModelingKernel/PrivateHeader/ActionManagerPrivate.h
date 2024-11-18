#pragma once
#include "ActionManager.h"

#include <QAction>
#include <QIcon>
class ActionInfo
{
public:
    ActionInfo();
    ActionInfo(QString text, QKeySequence shortKey = QKeySequence(), const QString icon = "", bool checkable = false,
               const QString tooltipsstr = "");
    /// @brief 菜单文本描述
    QString text;
    /// @brief 菜单图标路径
    QString icon;
    /// @brief 菜单快捷键
    QKeySequence shortCut;
    /// @brief 菜单是否checkable
    bool checkable;
    QString tooltips;
};

ActionInfo::ActionInfo() { }

ActionInfo::ActionInfo(QString text, QKeySequence shortKey, QString icon, bool checkable, const QString tooltipsstr)
{
    this->text = text;
    this->shortCut = shortKey;
    this->icon = icon;
    this->checkable = checkable;
    this->tooltips = tooltipsstr;
}

class ActionManagerPrivate
{
public:
    ActionManagerPrivate();
    QMap<ActionManager::ActionType, QAction *> actionsMap;
    QMap<ActionManager::ActionType, ActionInfo> actionsInfoMap;

    /// @brief 工具栏常用菜单组
    QActionGroup *commonActionGroup;

    /// @brief 图元的旋转变换图元组
    QActionGroup *transformActionGroup;

    /// @brief 注解菜单组
    QActionGroup *annotationGroup;

    /// @brief 编辑菜单组(包含复制、剪切)
    QActionGroup *copyAbleActionGroup;

    /// @brief 操作菜单组(区域放大、画布拖动)
    QActionGroup *operateActionGroup;

    /// @brief 画布相关的菜单组
    QActionGroup *canvasActionGroup;

    /// @brief 关联的视图对象列表
    QList<ICanvasView *> attachedViews;

    /// @brief 当前激活的画板上下文
    QSharedPointer<CanvasContext> activateCanvas;
};

ActionManagerPrivate::ActionManagerPrivate()
{
    commonActionGroup = nullptr;
    transformActionGroup = nullptr;
    annotationGroup = nullptr;
    copyAbleActionGroup = nullptr;
    operateActionGroup = nullptr;
    canvasActionGroup = nullptr;
    QList<QKeySequence> ZoomInSequenceList;
    ZoomInSequenceList << QKeySequence::ZoomIn << QObject::tr("Ctrl+=");
    actionsInfoMap[ActionManager::ZoomIn] = ActionInfo(QObject::tr("Zoomin"), QKeySequence::ZoomIn, QString(""));
    actionsInfoMap[ActionManager::ZoomOut] = ActionInfo(QObject::tr("Zoomout"), QKeySequence::ZoomOut, QString(""));
    actionsInfoMap[ActionManager::Copy] = ActionInfo(QObject::tr("Copy"), QKeySequence::Copy, QString(""));
    actionsInfoMap[ActionManager::Paste] = ActionInfo(QObject::tr("Paste"), QKeySequence::Paste, QString(""));
    actionsInfoMap[ActionManager::Cut] = ActionInfo(QObject::tr("Cut"), QKeySequence::Cut, QString(""));
    actionsInfoMap[ActionManager::Delete] = ActionInfo(QObject::tr("Delete"), QKeySequence::Delete, QString(""));
    actionsInfoMap[ActionManager::Undo] =
            ActionInfo(QObject::tr(""), QKeySequence::Undo, QString(":/GraphicsModelingKernel/menuCancelDisable"),
                       false, QObject::tr("Undo"));
    actionsInfoMap[ActionManager::Redo] =
            ActionInfo(QObject::tr(""), QKeySequence::Redo, QString(":/GraphicsModelingKernel/menuRedoDisable"), false,
                       QObject::tr("Redo"));
    actionsInfoMap[ActionManager::RotateClockwise] =
            ActionInfo(QObject::tr("Rotation CW"), QKeySequence(Qt::CTRL + Qt::Key_R), QString(""));
    actionsInfoMap[ActionManager::RotateAntiClockwise] =
            ActionInfo(QObject::tr("Rotation CCW"), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R), QString(""));
    actionsInfoMap[ActionManager::Rotate180] =
            ActionInfo(QObject::tr("Rotation 180°"), QKeySequence(Qt::CTRL + Qt::Key_Down), QString(""));
    actionsInfoMap[ActionManager::FlipVertical] = ActionInfo(QObject::tr("Flip Vertical"));
    actionsInfoMap[ActionManager::FlipHorizontal] = ActionInfo(QObject::tr("Flip Horizontal"));
    actionsInfoMap[ActionManager::LineAutoLayout] = ActionInfo(QObject::tr("Automatic Layout")); // 自动布局
    actionsInfoMap[ActionManager::OpenModel] = ActionInfo(QObject::tr("Open Model"));
    actionsInfoMap[ActionManager::BreakLinkLine] = ActionInfo(QObject::tr("Disconnect"));
    actionsInfoMap[ActionManager::updateModule] = ActionInfo(QObject::tr("Update Module"));
    actionsInfoMap[ActionManager::DrawBoardProperty] =
            ActionInfo(QObject::tr("Canvas Property"), QKeySequence(),
                       QString(":/GraphicsModelingKernel/menuBoardPropertyNormal"));
    actionsInfoMap[ActionManager::LayoutManager] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), false, QObject::tr("Layer"));
    actionsInfoMap[ActionManager::LayoutManagerNoIcon] =
            ActionInfo(QObject::tr("Layer"), QKeySequence(), QString(""), false, QObject::tr("Layer"));
    actionsInfoMap[ActionManager::CaseCreator] = ActionInfo(QObject::tr("Generate Case File"));
    actionsInfoMap[ActionManager::ElementProperty] = ActionInfo(QObject::tr("Edit")); // 元件属性
    actionsInfoMap[ActionManager::RotateAll] = ActionInfo(QObject::tr("Rotate"));
    actionsInfoMap[ActionManager::Help] = ActionInfo(QObject::tr("Help"));
    actionsInfoMap[ActionManager::ShowPortName] = ActionInfo(QObject::tr("Show/Hide Port Name"));

    // PF相关Action
    actionsInfoMap[ActionManager::Set] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), false, QObject::tr("Set"));
    actionsInfoMap[ActionManager::SetReplica] =
            ActionInfo(QObject::tr("Set"), QKeySequence(), QString(""), false, QObject::tr("Set"));
    actionsInfoMap[ActionManager::Colour] = ActionInfo(QObject::tr("Colour"));
    actionsInfoMap[ActionManager::ShowCoverage] = ActionInfo(QObject::tr("Show/Hide Coverage"));
    actionsInfoMap[ActionManager::ShowResult] = ActionInfo(QObject::tr("Show Result"));
    actionsInfoMap[ActionManager::ExportBorad] = ActionInfo(QObject::tr("Export Canvas")); // 导出画板
    actionsInfoMap[ActionManager::Export] = ActionInfo(QObject::tr("Export"));
    actionsInfoMap[ActionManager::ExportFMU] = ActionInfo(QObject::tr("Export As FMU"));
    actionsInfoMap[ActionManager::Prints] = ActionInfo(QObject::tr("Print"), QKeySequence());
    actionsInfoMap[ActionManager::SelectAll] =
            ActionInfo(QObject::tr("Select All"), QKeySequence::SelectAll, QString(""));
    actionsInfoMap[ActionManager::EditAndBrowseData] = ActionInfo(QObject::tr("Edit And Browser Data"));
    actionsInfoMap[ActionManager::BreakAllLinkLine] = ActionInfo(QObject::tr("Disconnect Connector Wire"));
    actionsInfoMap[ActionManager::DispNavigationPane] = ActionInfo(QObject::tr("Show/Hide Navigation Window"));

    actionsInfoMap[ActionManager::FullScreen] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), false, QObject::tr(""));
    actionsInfoMap[ActionManager::Lock] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Canvas Lock"));
    actionsInfoMap[ActionManager::LockReplica] =
            ActionInfo(QObject::tr("Canvas Lock"), QKeySequence(), QString(""), false, QObject::tr("Canvas Lock"));
    actionsInfoMap[ActionManager::FitIn] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), false, QObject::tr("Auto Fit"));
    actionsInfoMap[ActionManager::Magnify] = ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true,
                                                        QObject::tr("Designated Area Amplification"));
    actionsInfoMap[ActionManager::PrintDrawBoard] = ActionInfo(QObject::tr(""), QKeySequence(Qt::CTRL + Qt::Key_P),
                                                               QString(""), false, QObject::tr("Print Current Canvas"));
    actionsInfoMap[ActionManager::ScreenDrag] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Screen Drag"));
    actionsInfoMap[ActionManager::Textbox] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Text"));
    actionsInfoMap[ActionManager::Icon] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Image"));
    actionsInfoMap[ActionManager::SaveImage] = ActionInfo(QObject::tr(""), QKeySequence(), QString(""), false,
                                                          QObject::tr("Export The Artboard As An Image"));
    actionsInfoMap[ActionManager::Rectangle] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Rectangle"));
    actionsInfoMap[ActionManager::Circular] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Circular"));
    actionsInfoMap[ActionManager::Polygon] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Polygon"));
    actionsInfoMap[ActionManager::Arrows] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Arrow Line"));
    actionsInfoMap[ActionManager::Segment] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("StraightLine"));
    actionsInfoMap[ActionManager::Arc] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Arc"));
    actionsInfoMap[ActionManager::BrokenLine] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Broken Line"));
    actionsInfoMap[ActionManager::LineArrow] =
            ActionInfo(QObject::tr(""), QKeySequence(), QString(""), true, QObject::tr("Broken Line With Arrow"));
    actionsInfoMap[ActionManager::AddBookmark] = ActionInfo(QObject::tr("Add Bookmark"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::ManageBookmark] =
            ActionInfo(QObject::tr("Manager Bookmark"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::PecadCase] = ActionInfo(QObject::tr("Generate PECAD Case"));
    actionsInfoMap[ActionManager::SwitchBusbarType] = ActionInfo(QObject::tr(""));
    actionsInfoMap[ActionManager::SwitchBusbarType] = ActionInfo(QObject::tr(""));
    actionsInfoMap[ActionManager::MarkThrough] = ActionInfo(QObject::tr("Mark Through"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::MarkDisable] = ActionInfo(QObject::tr("Mark Disable"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::Unmark] =
            ActionInfo(QObject::tr("Unmark Through/Disable"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::SetBackgrounds] =
            ActionInfo(QObject::tr("Set Background"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::SetBackgroundColor] =
            ActionInfo(QObject::tr("Set Background Color"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::SetForegroundImage] =
            ActionInfo(QObject::tr("Set Foreground Image"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::SetBackgroundImage] =
            ActionInfo(QObject::tr("Set Background Image"), QKeySequence(), QString(""));

    actionsInfoMap[ActionManager::ResetBackgroundColor] =
            ActionInfo(QObject::tr("Reset Background Color"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::ResetBackgroundImage] =
            ActionInfo(QObject::tr("Reset Background Image"), QKeySequence(), QString(""));
    actionsInfoMap[ActionManager::ResetForegroundImage] =
            ActionInfo(QObject::tr("Reset Foreground Image"), QKeySequence(), QString(""));

    actionsInfoMap[ActionManager::ExportASPreviewSVG] =
            ActionInfo(QObject::tr("Export As SVG Preview Image"), QKeySequence(), QString(""));

    actionsInfoMap[ActionManager::PrintErrorInfos] =
            ActionInfo(QObject::tr("Print Error Infos"), QKeySequence(), QString(""));
}

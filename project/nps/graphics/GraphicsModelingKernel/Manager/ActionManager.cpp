#include "ActionManager.h"
#include "ActionManagerPrivate.h"
#include "CanvasContext.h"
#include "CanvasSceneDefaultImpl.h"
#include "ClipBoardMimeData.h"
#include "ICanvasScene.h"
#include "ICanvasView.h"
#include <QApplication>
#include <QClipboard>

ActionManager::ActionManager(QObject *parent) : QObject(parent)
{
    dataPtr.reset(new ActionManagerPrivate());
    // 创建QAction对象
    for (int i = int(ActionManager::startType) + 1; i < int(ActionManager::EndType); i++) {
        ActionType type = ActionType(i);
        if (dataPtr->actionsInfoMap.contains(type)) {
            createAction(type, &dataPtr->actionsInfoMap[type]);
        }
    }
    initCommonActionGroup();
    initTransformActionGroup();
    initAnnotationActionGroup();
    initCopyAbleActionGroup();
    initOperateActionGroup();
    initCanvasActionGroup();

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &ActionManager::onClipboardDataChanged);

    disable();
}

ActionManager::~ActionManager() { }

void ActionManager::initCommonActionGroup()
{
    if (!dataPtr->commonActionGroup) {
        dataPtr->commonActionGroup = new QActionGroup(this);

        dataPtr->commonActionGroup->addAction(getAction(FitIn));
        dataPtr->commonActionGroup->addAction(getAction(ExportBorad));
        dataPtr->commonActionGroup->addAction(getAction(FullScreen));
        dataPtr->commonActionGroup->addAction(getAction(SaveImage));
        dataPtr->commonActionGroup->addAction(getAction(PrintDrawBoard));
        dataPtr->commonActionGroup->addAction(getAction(LayoutManager));
    }
}

void ActionManager::initTransformActionGroup()
{
    if (!dataPtr->transformActionGroup) {
        dataPtr->transformActionGroup = new QActionGroup(this);
        dataPtr->transformActionGroup->addAction(getAction(RotateAll));
        dataPtr->transformActionGroup->addAction(getAction(RotateClockwise));
        dataPtr->transformActionGroup->addAction(getAction(RotateAntiClockwise));
        dataPtr->transformActionGroup->addAction(getAction(Rotate180));
        dataPtr->transformActionGroup->addAction(getAction(FlipVertical));
        dataPtr->transformActionGroup->addAction(getAction(FlipHorizontal));
    }

    dataPtr->transformActionGroup->setExclusive(true);
}

void ActionManager::initAnnotationActionGroup()
{
    if (!dataPtr->annotationGroup) {
        dataPtr->annotationGroup = new QActionGroup(this);
        dataPtr->annotationGroup->addAction(getAction(Rectangle));
        dataPtr->annotationGroup->addAction(getAction(Circular));
        dataPtr->annotationGroup->addAction(getAction(Textbox));
        dataPtr->annotationGroup->addAction(getAction(Icon));
        dataPtr->annotationGroup->addAction(getAction(Segment));
        dataPtr->annotationGroup->addAction(getAction(Arrows));
        dataPtr->annotationGroup->addAction(getAction(BrokenLine));
        dataPtr->annotationGroup->addAction(getAction(LineArrow));
        dataPtr->annotationGroup->addAction(getAction(Polygon));
        dataPtr->annotationGroup->addAction(getAction(Arc));
    }

    dataPtr->annotationGroup->setExclusive(true);
}

void ActionManager::initCopyAbleActionGroup()
{
    if (!dataPtr->copyAbleActionGroup) {
        dataPtr->copyAbleActionGroup = new QActionGroup(this);
        // dataPtr->copyAbleActionGroup->addAction(getAction(Copy));
        dataPtr->copyAbleActionGroup->addAction(getAction(Cut));
    }
}

void ActionManager::initOperateActionGroup()
{
    if (!dataPtr->operateActionGroup) {
        dataPtr->operateActionGroup = new QActionGroup(this);
        dataPtr->operateActionGroup->addAction(getAction(ScreenDrag));
        dataPtr->operateActionGroup->addAction(getAction(Magnify));
    }
}

void ActionManager::initCanvasActionGroup()
{
    if (!dataPtr->canvasActionGroup) {
        dataPtr->canvasActionGroup = new QActionGroup(this);
        dataPtr->canvasActionGroup->addAction(getAction(Set));
        dataPtr->canvasActionGroup->addAction(getAction(ZoomIn));
        dataPtr->canvasActionGroup->addAction(getAction(ZoomOut));
        dataPtr->canvasActionGroup->addAction(getAction(DispNavigationPane));
        dataPtr->canvasActionGroup->addAction(getAction(ExportBorad));
        dataPtr->canvasActionGroup->addAction(getAction(Prints));
        dataPtr->canvasActionGroup->addAction(getAction(LayoutManager));
        dataPtr->canvasActionGroup->addAction(getAction(SelectAll));
    }
}

QAction *ActionManager::createAction(ActionType type, ActionInfo *info)
{
    if (nullptr == info) {
        return nullptr;
    }
    QAction *action = nullptr;
    if (info->icon.isNull()) {
        action = new QAction(info->text, this);
    } else {
        action = new QAction(QIcon(info->icon), info->text, this);
    }
    action->setData(QVariant::fromValue(type));
    action->setCheckable(info->checkable);

    if (!info->shortCut.isEmpty()) {
        if (type == ActionManager::ZoomIn) {
            QKeySequence sequence(Qt::CTRL + Qt::Key_Equal);
            QList<QKeySequence> shortcuts;
            shortcuts << info->shortCut << sequence;
            action->setShortcuts(shortcuts);
        } else {
            action->setShortcut(info->shortCut);
        }
    }
    if (!info->tooltips.isEmpty()) {
        action->setToolTip(info->tooltips);
    }

    dataPtr->actionsMap[type] = action;
    action->setIconVisibleInMenu(false); // 设置action是否显示图标

    connect(action, &QAction::triggered, this, &ActionManager::onActionTriggered);
    return action;
}

ActionManager &ActionManager::getInstance()
{
    static ActionManager instance;
    return instance;
}

QAction *ActionManager::getAction(ActionManager::ActionType type)
{
    if (type <= startType || type >= EndType) {
        return nullptr;
    }
    return dataPtr->actionsMap.value(type, nullptr);
}

QList<QAction *> ActionManager::getActionList(const QList<ActionManager::ActionType> &types)
{
    QList<QAction *> actions;
    if (types.isEmpty()) {
        return actions;
    }
    QListIterator<ActionManager::ActionType> iterType(types);
    while (iterType.hasNext()) {
        ActionManager::ActionType type = iterType.next();
        QAction *action = getAction(type);
        if (nullptr != action) {
            actions.append(action);
        }
    }
    return actions;
}

void ActionManager::attachCanvasView(ICanvasView *canvasView)
{
    if (nullptr == canvasView) {
        return;
    }
    if (dataPtr->attachedViews.contains(canvasView)) {
        return;
    }
    dataPtr->attachedViews.push_back(canvasView);
}

void ActionManager::detachCanvasView(ICanvasView *canvasView)
{
    if (nullptr == canvasView) {
        return;
    }
    dataPtr->attachedViews.removeOne(canvasView);
}

void ActionManager::detachCanvasView(QString uuid)
{
    ICanvasView *targetView = nullptr;
    for each (auto view in dataPtr->attachedViews) {
        if (view) {
            auto canvasScene = view->getCanvasScene();
            if (canvasScene) {
                auto ctx = canvasScene->getCanvasContext();
                if (ctx) {
                    QString sceneUUID = ctx->uuid();
                    if (sceneUUID == uuid) {
                        targetView = view;
                        break;
                    }
                }
            }
        }
    }
    detachCanvasView(targetView);
}

void ActionManager::sceneMenu(QMenu *pMenu, QSharedPointer<CanvasContext> ctx)
{
    if (!pMenu || !ctx) {
        return;
    }
    pMenu->addAction(getAction(SetReplica));
    pMenu->addAction(getAction(LayoutManagerNoIcon));
    pMenu->addSeparator();
    pMenu->addAction(getAction(DispNavigationPane));
    pMenu->addAction(getAction(ShowCoverage));
    pMenu->addAction(getAction(LockReplica));
    if (ctx->type() == CanvasContext::kElectricalType) // 电气
    {
        pMenu->addSeparator();
        pMenu->addAction(getAction(Prints));
        pMenu->addSeparator();
        pMenu->addAction(getAction(Paste));
        pMenu->addSeparator();
        pMenu->addAction(getAction(SelectAll));
#ifdef COMPILER_SUPPORT_PECAD
        pMenu->addAction(getAction(PecadCase));
#endif
    } else {
        pMenu->addAction(getAction(ShowPortName));
        pMenu->addSeparator();

// 2023.06.19 liwenyu 只有simulab.designer 或者 debug模式下才显示导出画板菜单
// https://zt.xtkfpt.online/bug-view-8062.html
#if ((defined COMPILER_PRODUCT_DESIGNER) || (defined _DEBUG))
        if (ctx->type() == CanvasContext::kControlSystemType) {
            pMenu->addAction(getAction(ExportBorad));
        }
#endif

        pMenu->addAction(getAction(Prints));
        pMenu->addSeparator();
        pMenu->addAction(getAction(Paste));
        pMenu->addSeparator();
        pMenu->addAction(getAction(SelectAll));
#ifdef COMPILER_DEVELOPER_MODE
        pMenu->addAction(getAction(ExportFMU));
#endif
    }
#ifdef COMPILER_DEVELOPER_MODE
    pMenu->addAction(getAction(ExportASPreviewSVG));
    pMenu->addAction(getAction(PrintErrorInfos));
#endif
}

void ActionManager::graphicsGroupMenu(QMenu *menu, QSharedPointer<CanvasContext> ctx, bool hasConnected,
                                      bool hasControlCodeModel)
{
    menu->setMinimumWidth(170);
    menu->addAction(getAction(BreakLinkLine));
    menu->addSeparator();
    menu->addAction(getAction(Copy));
    menu->addAction(getAction(Cut));
    menu->addAction(getAction(Delete));
    menu->addSeparator();
    menu->addAction(getAction(RotateAll));
    if (hasConnected && ctx->canModify()) {
        getAction(BreakLinkLine)->setEnabled(true);
    } else {
        getAction(BreakLinkLine)->setEnabled(false);
    }

    QMenu *secMenu = new QMenu(menu);
    secMenu->setMinimumWidth(250);
    secMenu->addAction(getAction(RotateClockwise));
    secMenu->addAction(getAction(RotateAntiClockwise));
    secMenu->addAction(getAction(Rotate180));
    getAction(RotateAll)->setMenu(secMenu);
    if (ctx->type() != CanvasContext::kElectricalType && hasControlCodeModel) {
        menu->addSeparator();
        menu->addAction(getAction(ShowPortName));
    } else {
        menu->removeAction(getAction(ShowPortName));
    }
}

QString ActionManager::getAnnotationTypeString()
{
    if (!dataPtr->annotationGroup) {
        return "";
    }

    QAction *action = dataPtr->annotationGroup->checkedAction();
    if (!action) {
        return "";
    }

    QVariant var = action->data();
    ActionManager::ActionType type = var.value<ActionManager::ActionType>();
    switch (type) {
    case BrokenLine:
        return BrokenLineAnnotationStr;
    case Rectangle:
        return RectAnnotationStr;
    case Circular:
        return CircleAnnotationStr;
    case Segment:
        return LineSegmentAnnotationStr;
    case Arrows:
        return LineSegmentArrowAnnotationStr;
    case LineArrow:
        return BrokenLineArrowAnnotationStr;
    case Polygon:
        return PolygonAnnotationStr;
    case Arc:
        return ArcAnnotationStr;
    case Textbox:
        return TextAnnotationStr;
    case Icon:
        return ImageAnnotationStr;
    default:
        break;
    }
    return "";
}

QString ActionManager::getActivateCanvasUUID()
{
    return dataPtr->activateCanvas ? dataPtr->activateCanvas->uuid() : "";
}

void ActionManager::setActivateCanvas(QSharedPointer<ICanvasScene> canvasscene)
{
    if (nullptr == canvasscene) {
        dataPtr->activateCanvas.clear();
        getAction(Undo)->setEnabled(false);
        getAction(Redo)->setEnabled(false);
    } else {
        auto ctx = canvasscene->getCanvasContext();
        dataPtr->activateCanvas = ctx;

        auto undoStack = canvasscene->getUndoStack();
        if (undoStack) {
            getAction(Undo)->setEnabled(undoStack->canUndo());
            getAction(Redo)->setEnabled(undoStack->canRedo());
        }
    }
    onClipboardDataChanged();
}

QActionGroup *ActionManager::getCommonGroup()
{
    return dataPtr->commonActionGroup;
}

QActionGroup *ActionManager::getTransformActionGroup()
{
    return dataPtr->transformActionGroup;
}

QActionGroup *ActionManager::getAnnotationGroup()
{
    return dataPtr->annotationGroup;
}

QActionGroup *ActionManager::getCopyAbleActionGroup()
{
    return dataPtr->copyAbleActionGroup;
}

QActionGroup *ActionManager::getOperateActionGroup()
{
    return dataPtr->operateActionGroup;
}

QActionGroup *ActionManager::getCanvasActionGroup()
{
    return dataPtr->canvasActionGroup;
}

void ActionManager::disable()
{

    getAction(Delete)->setEnabled(false);
    getAction(Paste)->setEnabled(false);
    getAction(Undo)->setEnabled(false);
    getAction(Redo)->setEnabled(false);

    if (dataPtr->copyAbleActionGroup) {
        dataPtr->copyAbleActionGroup->setEnabled(false);
    }
    dataPtr->canvasActionGroup->setEnabled(false);
    dataPtr->commonActionGroup->setEnabled(false);
    dataPtr->transformActionGroup->setEnabled(false);
}

void ActionManager::onClipboardDataChanged()
{
    if (!dataPtr->activateCanvas) {
        getAction(Paste)->setEnabled(false);
        return;
    }
    auto clipBoard = QApplication::clipboard();
    if (clipBoard) {
        ClipBoardMimeData *mimeData = dynamic_cast<ClipBoardMimeData *>((QMimeData *)clipBoard->mimeData());
        if (!mimeData) {
            getAction(Paste)->setEnabled(false);
        } else {
            bool pasteEnable = false;
            BusinessHooksServer *hookserver = dataPtr->activateCanvas->getBusinessHooksServer();
            if (hookserver) {
                pasteEnable = hookserver->enableCreate(mimeData->getCanvasType(), mimeData->getSourcesMap());
            }
            auto flag = dataPtr->activateCanvas->getStatusFlags();
            bool isNormal = false;
            isNormal = flag.testFlag(CanvasContext::kLockedStatus)
                    || flag.testFlag(CanvasContext::kSimulationRunningStatus)
                    || flag.testFlag(CanvasContext::kNextConstructiveBorad);
            getAction(Paste)->setEnabled(pasteEnable && !isNormal);
        }
    }
}

void ActionManager::onUndoStatusChanged(bool canUndo)
{
    auto flag = dataPtr->activateCanvas->getStatusFlags();
    bool isLock = flag.testFlag(CanvasContext::kLockedStatus);
    if (!dataPtr->activateCanvas || isLock) {
        getAction(Undo)->setEnabled(false);
        getAction(Redo)->setEnabled(false);
        return;
    }
    getAction(Undo)->setEnabled(canUndo);
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent); // 填充透明色
    QIcon icon(pixmap);
    getAction(ActionManager::Undo)->setIcon(icon);
}

void ActionManager::onRedoStatusChanged(bool canRedo)
{
    if (!dataPtr->activateCanvas) {
        getAction(Redo)->setEnabled(false);
        return;
    }
    getAction(Redo)->setEnabled(canRedo);
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent); // 填充透明色
    QIcon icon(pixmap);
    getAction(Redo)->setIcon(icon);
}

void ActionManager::onActionTriggered(bool checked)
{
    ICanvasView *targetView = nullptr;
    for each (auto view in dataPtr->attachedViews) {
        if (view) {
            auto canvasScene = view->getCanvasScene();
            if (canvasScene) {
                auto ctx = canvasScene->getCanvasContext();
                if (ctx) {
                    QString sceneUUID = ctx->uuid();
                    if (sceneUUID == getActivateCanvasUUID()) {
                        targetView = view;
                        break;
                    }
                }
            }
        }
    }

    if (nullptr == targetView) {
        return;
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (nullptr == action) {
        return;
    }
    QVariant var = action->data();
    ActionManager::ActionType type = var.value<ActionManager::ActionType>();
    targetView->processAction(type, checked);
}

void ActionManager::onCanvasInteractionModeChanged(QString canvasUUID, int newValue, int oldValue)
{
    if (getActivateCanvasUUID() != canvasUUID) {
        return;
    }
    InteractionMode mode = (InteractionMode)newValue;
    switch (mode) {
    case kNormal: {
        QAction *action = dataPtr->annotationGroup->checkedAction();
        if (action) {
            action->setChecked(false);
        }

        action = dataPtr->operateActionGroup->checkedAction();
        if (action) {
            action->setChecked(false);
        }
    } break;
    case kAreaAmplification:
    case kHandDrag: {
        QAction *action = dataPtr->annotationGroup->checkedAction();
        if (action) {
            action->setChecked(false);
        }
    } break;
    case kAnnotation: {
        QAction *action = dataPtr->operateActionGroup->checkedAction();
        if (action) {
            action->setChecked(false);
        }

    } break;
    default: {
    }
    }
}
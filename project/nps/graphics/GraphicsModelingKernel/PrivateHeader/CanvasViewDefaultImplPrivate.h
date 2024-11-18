#pragma once

#include <QSharedPointer>
#include <QString>

#include "AnnotationSourceGraphicsObject.h"
#include "CanvasContext.h"
#include "CanvasNavigationPrview.h"
#include "CanvasViewDefaultImpl.h"
#include "GraphicsInputModelEdit.h"
#include "ICanvasScene.h"
#include "KLWidgets/KCustomDialog.h"

class CanvasViewDefaultImplPrivate
{
public:
    CanvasViewDefaultImplPrivate();
    QSharedPointer<CanvasContext> canvasContext;
    QSharedPointer<ICanvasScene> canvasScene;

    /// @brief 鼠标滚轮滚动时的鼠标坐标
    QPointF wheelMousePos;
    QMap<QAction *, bool> toolbarActionStatus;

    //
    CanvasNavigationPrview *navigationPreview;

    QRect lastSyncPreviewRect;

    bool isCreateArrow;

    AnnotationSourceGraphicsObject *annotationGraphics;

    bool isFullScreen;

    QPointF selectStartPoint;

    ViewToolBar *toolBar;

    SliderScale *sliderScaleWidget;

    KCustomDialog *previewWidget;

    CreateModelEditBox *createBox;

    QPoint clickPos;

    bool isEditTextName;

    bool isPreviewSetPos;

    QPoint previewLastPos;

    /// @brief 工具栏电压筛选框内容
    QString currentVolumeText;

    /// @brief 工具栏母线相数筛选内容
    QString currentPhapseText;

    /// @brief 搜索输入框搜索的资源uuid
    QString searchLineUUIDText;

    bool isMiddleButtonPressed;
};

CanvasViewDefaultImplPrivate::CanvasViewDefaultImplPrivate()
{
    wheelMousePos = QPointF(0, 0);
    annotationGraphics = nullptr;
    isFullScreen = false;
    toolBar = nullptr;
    sliderScaleWidget = nullptr;
    previewWidget = nullptr;
    createBox = nullptr;
    clickPos = QPoint(0, 0);
    isEditTextName = false;
    isPreviewSetPos = false;
    isMiddleButtonPressed = false;
}
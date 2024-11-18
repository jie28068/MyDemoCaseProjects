#include "ICanvasView.h"
#include "CanvasContext.h"
#include "GraphicsFactoryManager.h"
#include "ICanvasScene.h"
#include "SourceProxy.h"

#include <QStyleOption>

ICanvasView::ICanvasView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing, false);
    setCacheMode(QGraphicsView::CacheBackground);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setInteractive(true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setStyleSheet("ICanvasView {border:none;}");
}

ICanvasView::~ICanvasView(void) { }

// void ICanvasView::paintEvent(QPaintEvent *event)
//{
//     Q_UNUSED(event);
//
//     QStyleOption opt;
//     opt.init(this);
//     QPainter p(this);
//     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//     QGraphicsView::paintEvent(event);
// }
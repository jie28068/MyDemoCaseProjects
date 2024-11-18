#include "CanvasNavigationPrview.h"
#include <QAction>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>

CanvasNavigationWidget::CanvasNavigationWidget(CanvasNavigationPrview *view, QWidget *parent) : QWidget(parent)
{
    m_pPreview = view;
    m_pLayout = new QVBoxLayout(this);
    this->setStyleSheet("CanvasNavigationWidget{background-color:#FFFFFF}");
    this->setMinimumSize(50, 40);

    m_pLayout->setContentsMargins(0, 0, 0, 0);
    m_pLayout->addWidget(m_pPreview);
    this->setLayout(m_pLayout);
    this->resize(300, 256);
    setStyleSheet("border:none;");
}

CanvasNavigationWidget::~CanvasNavigationWidget() { }

CanvasNavigationPrview::CanvasNavigationPrview(QWidget *parent) : QGraphicsView(parent)
{
    setInteractive(false);
    isMousePressed = false;
    /*setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setRenderHint(QPainter::HighQualityAntialiasing, false);*/
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    setStyleSheet("border:none;");
}

CanvasNavigationPrview::~CanvasNavigationPrview() { }

void CanvasNavigationPrview::mousePressEvent(QMouseEvent *event)
{
    isMousePressed = true;
    QPointF mousePos = event->pos();
    QPointF scenePos = mapToScene(mousePos.toPoint());
    emit previewSceneRectChanged(scenePos);
    QGraphicsView::mousePressEvent(event);
}

void CanvasNavigationPrview::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (isMousePressed) {
        QPointF mousePos = event->pos();
        QPointF scenePos = mapToScene(mousePos.toPoint());
        emit previewSceneRectChanged(scenePos);
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void CanvasNavigationPrview::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    QPointF mousePos = event->pos();
    QPointF scenePos = mapToScene(mousePos.toPoint());
    emit previewSceneRectChangeFinished(scenePos);
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasNavigationPrview::resizeEvent(QResizeEvent *event)
{
    auto s = scene();
    if (s) {
        fitInView(s->sceneRect(), Qt::KeepAspectRatio);
        viewRect = mapFromScene(senceRect).boundingRect();
        auto view = viewport();
        if (view) {
            view->update();
        }
    }

    QGraphicsView::resizeEvent(event);
}

void CanvasNavigationPrview::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    QPainter painter(this->viewport());
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(QColor(0x00cfff));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(viewRect);
    painter.restore();
}

void CanvasNavigationPrview::onSceneViewChanged(QRectF rect)
{
    viewRect = mapFromScene(rect).boundingRect();
    viewRect.setX(viewRect.x() + 1); // 偏移居中
    senceRect = rect;
    auto view = viewport();
    if (view) {
        view->update();
    }
}

void CanvasNavigationPrview::onPreviewSceneRectChanged(const QRectF &rect)
{
    auto s = scene();
    if (s) {
        setSceneRect(rect);
        fitInView(s->sceneRect(), Qt::KeepAspectRatio);
    }
}

void CanvasNavigationPrview::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        emit escapeKeyPressed(event);
    } else {
        QWidget::keyPressEvent(event);
    }
}

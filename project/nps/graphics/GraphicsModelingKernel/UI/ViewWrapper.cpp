#include "ViewWrapper.h"
#include <QResizeEvent>

ViewWrapper::ViewWrapper(QWidget *parent /*= nullptr*/) : QWidget(parent)
{
    m_opacity = new QGraphicsOpacityEffect(this);
    m_opacity->setOpacity(0.5);
    setGraphicsEffect(m_opacity);
    setVisible(false);
    setWindowFlags(Qt::FramelessWindowHint);
}

void ViewWrapper::showEvent(QShowEvent *event)
{
    setWindowOpacity(0.5);
    QWidget::showEvent(event);
}

void ViewWrapper::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

void ViewWrapper::resizeHandle(const QSize &sizeNew) { }

void ViewWrapper::resizeEvent(QResizeEvent *event)
{
    QSize sizeNew = event->size();
    resizeHandle(sizeNew);

    QWidget::resizeEvent(event);
}

void ViewWrapper::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void ViewWrapper::timerEvent(QTimerEvent *event)
{
    QWidget::timerEvent(event);
}

// 设置范围
void ViewWrapper::setProBarRange(int minimun, int maximun) { }

// 设置进度条的当前值
void ViewWrapper::setProBarValue(int value) { }
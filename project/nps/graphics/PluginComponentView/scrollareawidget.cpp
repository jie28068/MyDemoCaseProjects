#include "scrollareawidget.h"

ScrollAreaWidget::ScrollAreaWidget(QWidget *parent) : QWidget(parent) { }

ScrollAreaWidget::~ScrollAreaWidget() { }

void ScrollAreaWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    emit scrollWidgetSizeChanged(this->height());
}

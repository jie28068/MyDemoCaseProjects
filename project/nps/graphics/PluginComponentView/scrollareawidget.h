#ifndef SCROLLAREAWIDGET_H
#define SCROLLAREAWIDGET_H

#include <QWidget>

// 自定义滚动区域widget
class ScrollAreaWidget : public QWidget
{
    Q_OBJECT

public:
    ScrollAreaWidget(QWidget *parent = nullptr);
    ~ScrollAreaWidget();
signals:
    void scrollWidgetSizeChanged(int height);

protected:
    virtual void resizeEvent(QResizeEvent *e);
};

#endif // SCROLLAREAWIDGET_H

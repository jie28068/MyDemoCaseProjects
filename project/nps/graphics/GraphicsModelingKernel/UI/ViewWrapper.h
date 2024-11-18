#pragma once

#include <QGraphicsOpacityEffect>
#include <QWidget>

class ViewWrapper : public QWidget
{
public:
    explicit ViewWrapper(QWidget *parent = nullptr);

    // 设置进度条范围
    void setProBarRange(int minimun, int maximun);
    // 设置进度条的当前值
    void setProBarValue(int value);
    void setChangedEventSize(QSize size) { m_changedSize = size; }
    QSize getChangedSize() { return m_changedSize; }
    void resizeHandle(const QSize &sizeNew);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    QSize m_changedSize;

    QGraphicsOpacityEffect *m_opacity;
};

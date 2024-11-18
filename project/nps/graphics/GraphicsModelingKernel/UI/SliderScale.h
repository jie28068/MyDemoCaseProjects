#ifndef SLIDERSCALE_H
#define SLIDERSCALE_H

#include "KLineEdit.h"
#include <QGraphicsOpacityEffect>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QSlider>
#include <QTime>
#include <QWidget>

#define MIN_SCALE 20  // 最小缩放值为20%
#define MID_SCALE 100 // 缩放的中间值为100%
#define MAX_SCALE 500 // 最大缩放值为500%

class SliderTip;

// 滑动缩放ui主窗体
class SliderScale : public QWidget
{
    Q_OBJECT
public:
    explicit SliderScale(QWidget *parent = nullptr);
    ~SliderScale();
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void updateScaleValue(int percentvalue);
    // 更新显示缩放算好的值，会发送信号
    void initSiliderValue(int percentvalue); // 初始化，不会发送变更信号

signals:
    void zoomSizeChanged(int percentvalue); // 改变后值传输信号

private:
    void setScaleValue(int percentvalue, bool bemitsignal); // 设置缩放算好的值
    bool eventFilter(QObject *obj, QEvent *evt);

public slots:
    void onTextEditingFinished(); // 文本编辑完成按下回车后才收到
    void onZoomInClicked();
    void onZoomOutClicked();
    void onSliderValueChaned(int percentvalue);

private:
    SliderTip *m_verticalSlider;
    KLineEdit *m_pNavEdit;             // 比例显示框
    QPushButton *m_zoomInButton;       // 放大图标按钮
    QPushButton *m_zoomOutButton;      // 缩小图标按钮
    bool m_isinitOperation;            // 初始化
    QGraphicsOpacityEffect *m_opacity; // 半透明效果
};

// 自定义滑动条
class SliderTip : public QSlider
{
    Q_OBJECT
public:
    explicit SliderTip(QWidget *parent = 0);
    ~SliderTip();

    void setSliderPercentValue(int percentvalue); // 将百分比设置过来。
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

private:
    // 输入滑动条的数据，将它的0-50范围的数据转换为20-100范围内数据，将它的50-100范围的数据转换为100-500范围内数据
    double valueConvertScale(int slidervalue);
    // 输入缩放后的数据，将它20-100范围内数据转换为0-50,将它100-500范围内数据转换为50-100
    int scaleConvertValue(int percentvalue);
    void onValueChanged(int value);

signals:
    void sliderValueChanged(int percentvalue); // 改变后值传输信号

private:
    bool m_mousePressed;
};
#endif // SLIDERSCALE_H

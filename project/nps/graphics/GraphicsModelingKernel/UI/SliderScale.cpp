#include "SliderScale.h"

#include <QHBoxLayout>
#include <QRegExpValidator>
#include <QTimer>

SliderScale::SliderScale(QWidget *parent) : QWidget(parent), m_isinitOperation(false)
{
    m_opacity = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(m_opacity);
    m_opacity->setOpacity(1);

    m_pNavEdit = new KLineEdit(this);
    m_pNavEdit->setText("100%");
    m_pNavEdit->setEnabled(true);
    m_pNavEdit->setFixedSize(44, 18);
    m_pNavEdit->setAlignment(Qt::AlignCenter);
    m_pNavEdit->installEventFilter(this);
    QRegExp rx("^[1-9][0-9][0-9]$ ");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    m_pNavEdit->setValidator(validator);
    // m_pNavEdit->setValidator(new QIntValidator(20, 500, this));

    m_zoomInButton = new QPushButton(this);
    m_zoomInButton->setFixedSize(14, 14);
    m_zoomInButton->setFlat(true);
    m_zoomInButton->setObjectName("zoomInButton");
    m_zoomInButton->setIcon(QIcon(":/GraphicsModelingKernel/zoomIn"));

    m_zoomOutButton = new QPushButton(this);
    m_zoomOutButton->setFixedSize(15, 15);
    m_zoomOutButton->setObjectName("zoomOutButton");
    m_zoomOutButton->setIcon(QIcon(":/GraphicsModelingKernel/zoomOut"));

    m_verticalSlider = new SliderTip(this);
    m_verticalSlider->setValue(50);
    m_verticalSlider->setRange(0, 100);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(m_pNavEdit);
    hBoxLayout->addStretch(1);
    hBoxLayout->addWidget(m_zoomOutButton);
    hBoxLayout->addWidget(m_verticalSlider);
    hBoxLayout->addWidget(m_zoomInButton);

    setLayout(hBoxLayout);
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明

    // connect(m_pNavEdit, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    connect(m_zoomInButton, SIGNAL(clicked()), this, SLOT(onZoomInClicked()), Qt::UniqueConnection);
    connect(m_zoomOutButton, SIGNAL(clicked()), this, SLOT(onZoomOutClicked()), Qt::UniqueConnection);
    connect(m_verticalSlider, SIGNAL(sliderValueChanged(int)), this, SLOT(onSliderValueChaned(int)),
            Qt::UniqueConnection);
    this->hide();
}

SliderScale::~SliderScale() { }

void SliderScale::onZoomInClicked()
{
    if (m_pNavEdit != nullptr) {
        int percentvalue = m_pNavEdit->text().remove("%").toInt();
        updateScaleValue(percentvalue + 5);
    }
}

void SliderScale::onZoomOutClicked()
{
    if (m_pNavEdit != nullptr) {
        int percentvalue = m_pNavEdit->text().remove("%").toInt();
        updateScaleValue(percentvalue - 5);
    }
}

void SliderScale::onTextEditingFinished()
{
    if (m_pNavEdit != nullptr) {
        updateScaleValue(m_pNavEdit->text().remove("%").toInt());
    }
}

void SliderScale::onSliderValueChaned(int percentvalue)
{
    if (m_pNavEdit == nullptr) {
        return;
    }
    if (percentvalue > MAX_SCALE) {
        percentvalue = MAX_SCALE;
    } else if (percentvalue < MIN_SCALE) {
        percentvalue = MIN_SCALE;
    }
    m_pNavEdit->setText(QString("%1%2").arg(QString::number(percentvalue)).arg("%"));
    emit zoomSizeChanged(percentvalue);
}

bool SliderScale::eventFilter(QObject *obj, QEvent *evt)
{
    if (m_pNavEdit != nullptr && obj == m_pNavEdit) {
        if (evt->type() == QEvent::MouseButtonPress) {
            m_pNavEdit->setReadOnly(false);
            m_pNavEdit->setText(QString("%1").arg(m_pNavEdit->text().remove("%")));
            QTimer::singleShot(0, m_pNavEdit, SLOT(selectAll()));
        }

        QKeyEvent *keyEvt = static_cast<QKeyEvent *>(evt);
        if (evt->type() == QEvent::FocusOut || keyEvt->key() == Qt::Key_Enter || keyEvt->key() == Qt::Key_Return) {
            // m_isOnlyDisplay让缩放比例显示框只显示当前用户输入的缩放值，限制在commonLabHandle函数中缩放滑动条反向再设置比例显示框的数值而导致的显示数值误差
            int textpercentvalue = m_pNavEdit->text().remove("%").toInt();
            if (MIN_SCALE > textpercentvalue) {
                updateScaleValue(MIN_SCALE);
            } else if (MAX_SCALE < textpercentvalue) {
                updateScaleValue(MAX_SCALE);
            } else {
                // 必须先发送信号设置画板缩放（避免数值带%导致设置缩放比例失效），再setScaleValue设置比例框和滑动条
                updateScaleValue(textpercentvalue);
            }
            m_pNavEdit->setReadOnly(true);
            return true;
        }
    }
    return QWidget::eventFilter(obj, evt);
}

// 设置缩放算好的值
void SliderScale::setScaleValue(int percentvalue, bool bemitsignal)
{
    if (percentvalue > MAX_SCALE) {
        percentvalue = MAX_SCALE;
    } else if (percentvalue < MIN_SCALE) {
        percentvalue = MIN_SCALE;
    }
    m_pNavEdit->setText(QString("%1%2").arg(QString::number(percentvalue)).arg("%"));
    m_verticalSlider->setSliderPercentValue(percentvalue);
    if (bemitsignal) {
        emit zoomSizeChanged(percentvalue);
    }
}

void SliderScale::initSiliderValue(int percentvalue)
{
    // 外部初始化使用，同步设置slider和text，不会发送信号
    setScaleValue(percentvalue, false);
}

void SliderScale::updateScaleValue(int percentvalue)
{
    // 外部设置，同步设置slider和text，触发信号
    setScaleValue(percentvalue, true);
}

void SliderScale::enterEvent(QEvent *e)
{
    m_opacity->setOpacity(1);
    return QWidget::enterEvent(e);
}

void SliderScale::leaveEvent(QEvent *e)
{
    m_opacity->setOpacity(0.5);
    return QWidget::leaveEvent(e);
}

// 自定义滚动条
SliderTip::SliderTip(QWidget *parent) : QSlider(parent), m_mousePressed(false)
{
    setFixedSize(92, 18);
    this->setOrientation(Qt::Horizontal);
    // connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)), Qt::UniqueConnection);
}

SliderTip::~SliderTip() { }

void SliderTip::mousePressEvent(QMouseEvent *ev)
{
    QSlider::mousePressEvent(ev);
    // 限定必须是鼠标左键按下
    if (ev->button() != Qt::LeftButton) {
        return;
    }
    m_mousePressed = true;
}

void SliderTip::mouseReleaseEvent(QMouseEvent *ev)
{
    QSlider::mouseReleaseEvent(ev);
    m_mousePressed = false;
    onValueChanged(this->value());
}
void SliderTip::mouseMoveEvent(QMouseEvent *ev)
{
    QSlider::mouseMoveEvent(ev);
    if (m_mousePressed) {
        onValueChanged(this->value());
    }
}

// 输入滑动条的数据，将它的0-50范围的数据转换为20-100范围内数据，将它的50-100范围的数据转换为100-500范围内数据
double SliderTip::valueConvertScale(int slidervalue)
{
    double scale_value;
    if (slidervalue > 0 && slidervalue <= 50) {
        scale_value = ((MID_SCALE - MIN_SCALE) * 100 / 50 * slidervalue) / 100 + MIN_SCALE;
    } else if (slidervalue > 50 && slidervalue <= 100) {
        scale_value = ((MAX_SCALE - MID_SCALE) * 100 / 50 * (slidervalue - 50)) / 100 + MID_SCALE;
    } else if (slidervalue == 0)
        scale_value = MIN_SCALE;
    else
        scale_value = 500;
    return scale_value;
}

// 输入缩放后的数据，将它20-100范围内数据转换为0-50,将它100-500范围内数据转换为50-100
int SliderTip::scaleConvertValue(int percentvalue)
{
    int value = 0;
    if (percentvalue >= MIN_SCALE && percentvalue < MID_SCALE)
        value = ((percentvalue - MIN_SCALE) * 100) / ((MID_SCALE - MIN_SCALE) * 100 / 50);
    else if (percentvalue >= MID_SCALE)
        value = 50 + ((percentvalue - MID_SCALE) * 100) / ((MAX_SCALE - MID_SCALE) * 100 / 50);
    return value;
}

void SliderTip::setSliderPercentValue(int percentvalue)
{
    int value = scaleConvertValue(percentvalue);

    if (value > 100)
        value = 100;
    else if (value < 0)
        value = 0;

    this->setValue(value);
}

void SliderTip::onValueChanged(int value)
{
    emit sliderValueChanged(valueConvertScale(value));
}

#include "ColorButton.h"
#include "CoreLib/GlobalConfigs.h"
#include "KLWidgets/KColorDialog.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include <QPainter>

KCC_USE_NAMESPACE_MONITORPANEL

ColorButton::ColorButton(QWidget *parent) : QPushButton(parent), m_color(QColor(0, 0, 0))
{
    this->setFocusPolicy(Qt::NoFocus);
    setColor(m_color);

    connect(this, SIGNAL(clicked()), this, SLOT(onColorDialog()));
}

ColorButton::~ColorButton() { }

void ColorButton::setColor(QColor &clr)
{
    m_color = clr;
    update();
}

QColor ColorButton::getColor(void)
{
    return m_color;
}

void ColorButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.save();
    painter.fillRect(2, 2, size().width() - 4, size().height() - 4, m_color);
    painter.restore();
}

void ColorButton::onColorDialog(void)
{
    QColor clr;
    if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
        clr = QColorDialog::getColor(m_color, this, tr("Chose Color")); // 选择颜色
    } else {
        KColorDialog dlg(tr("Select Color"), nullptr);
        dlg.setCurrentColor(m_color);
        if (dlg.exec() == KColorDialog::Ok)
            return;
        clr = dlg.currentColor();
    }
    if (!clr.isValid())
        return;
    setColor(clr);
}

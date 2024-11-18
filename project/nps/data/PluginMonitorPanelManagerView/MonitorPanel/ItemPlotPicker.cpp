#include "ItemPlotPicker.h"
#include <QColor>
#include <qwt_picker_machine.h>
#include <qwt_plot.h>

ItemPlotPicker::ItemPlotPicker(QWidget *parent) : QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, parent)
{
    setTrackerMode(AlwaysOn);
    setStateMachine(new QwtPickerDragPointMachine());
    setRubberBandPen(QColor(0, 255, 0));
    setRubberBand(VLineRubberBand /*CrossRubberBand*/);
}

ItemPlotPicker::~ItemPlotPicker() { }

QwtText ItemPlotPicker::trackerTextF(const QPointF &pos) const
{
    QwtText text;
    text.setColor(QColor(0, 0, 255));
    text.setBackgroundBrush(QBrush(QColor(0, 0, 0, 80)));
    text.setBorderRadius(5);
    text.setBorderPen(QColor(0, 0, 255));
    text.setText(QString("%1,%2").arg(pos.x()).arg(pos.y()));

    return text;
}

QRect ItemPlotPicker::trackerRect(const QFont &font) const
{
    QRect rect = QwtPlotPicker::trackerRect(font);
    rect.adjust(-4, -10, 0, 0);
    return rect;
}

void ItemPlotPicker::drawRubberBand(QPainter *painter) const
{
    QwtPlotPicker::drawRubberBand(painter);
}

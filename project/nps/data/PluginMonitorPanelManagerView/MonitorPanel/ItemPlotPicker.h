#ifndef ITEMPLOTPICKER_H
#define ITEMPLOTPICKER_H

#include <qwt_plot_picker.h>

class ItemPlotPicker : public QwtPlotPicker
{
    Q_OBJECT

public:
    ItemPlotPicker(QWidget *parent);
    ~ItemPlotPicker();

protected:
    virtual QwtText trackerTextF(const QPointF &pos) const;
    virtual QRect trackerRect(const QFont &font) const;
    virtual void drawRubberBand(QPainter *painter) const;

private:
};

#endif // ITEMPLOTPICKER_H

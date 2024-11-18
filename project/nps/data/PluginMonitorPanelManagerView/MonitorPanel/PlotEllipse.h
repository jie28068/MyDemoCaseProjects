#ifndef PLOTELLIPSE_H
#define PLOTELLIPSE_H
#include <QPainterPath>
#include <qwt_plot.h>
#include <qwt_plot_shapeitem.h>

class PlotEllipse : public QwtPlotShapeItem
{

public:
    PlotEllipse(const QPointF &pos, qreal radius, const QColor &color = Qt::blue, int width = 1,
                const QString &title = QString::null, QwtPlot *plot = nullptr);
    ~PlotEllipse();

private:
    void createEllipse(const QPointF &pos, qreal radius, const QColor &color, int width, const QString &title);
    QPainterPath ellipseShape(const QPointF &pos, qreal radius);

private:
};

#endif // PLOTELLIPSE_H

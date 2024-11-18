#include "PlotEllipse.h"
#include <QPen>

PlotEllipse::PlotEllipse(const QPointF &pos, qreal radius, const QColor &color, int width, const QString &title,
                         QwtPlot *plot)
    : QwtPlotShapeItem()
{
    this->setItemAttribute(QwtPlotItem::Legend, false);
    this->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    this->setBrush(Qt::NoBrush);

    createEllipse(pos, radius, color, width, title);
    if (plot) {
        this->attach(plot);
    }
}

PlotEllipse::~PlotEllipse() { }

void PlotEllipse::createEllipse(const QPointF &pos, qreal radius, const QColor &color, int width, const QString &title)
{
    this->setTitle(title);
    QPen pen(color, width);
    pen.setJoinStyle(Qt::MiterJoin);
    this->setPen(pen);
    this->setShape(ellipseShape(pos, radius));
}

QPainterPath PlotEllipse::ellipseShape(const QPointF &pos, qreal radius)
{
    QRectF rect;
    rect.setSize(QSizeF(radius * 2, radius * 2));
    rect.moveCenter(pos);
    QPainterPath path;
    path.addEllipse(rect);
    return path;
}

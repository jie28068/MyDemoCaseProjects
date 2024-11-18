#ifndef ITEMPLOTLEGEND_H
#define ITEMPLOTLEGEND_H

#include <QPaintEvent>
#include <QWidget>
#include <qwt_legend.h>
#include <qwt_legend_label.h>

class ItemPlotLegendWidget : public QwtLegendLabel
{
    Q_OBJECT
public:
    ItemPlotLegendWidget(QWidget *parent = nullptr);
    ~ItemPlotLegendWidget();
    void setData(const QwtLegendData &data);

    virtual QSize minimumSizeHint() const;
protected slots:
    void paintEvent(QPaintEvent *e);

private:
    QFont m_font;
};

class ItemPlotLegend : public QwtLegend
{
    Q_OBJECT

public:
    ItemPlotLegend(QWidget *parent = nullptr);
    ~ItemPlotLegend();

protected:
    virtual QWidget *createWidget(const QwtLegendData &data) const override;
    virtual void updateWidget(QWidget *widget, const QwtLegendData &data) override;
    void wheelEvent(QWheelEvent *event);
protected slots:
    void onLegendChecked(const QVariant &itemInfo, bool on, int index);

private:
};

#endif // ITEMPLOTLEGEND_H

#ifndef ITEMPLOTCURVE_H
#define ITEMPLOTCURVE_H
#include "def.h"

#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

#define INVALID_VALUE -999999

class ItemPlotCurveSeriesData : public QwtSeriesData<QPointF>
{
public:
    ItemPlotCurveSeriesData();
    ItemPlotCurveSeriesData(const ItemPlotCurveSeriesData &d);
    ItemPlotCurveSeriesData &operator=(const ItemPlotCurveSeriesData &d);
    virtual ~ItemPlotCurveSeriesData();

    // 必需实现下面三个虚函数
    virtual size_t size() const override;
    virtual QPointF sample(size_t i) const override;
    virtual QRectF boundingRect() const override;

    void append(const QPointF &point, const qreal &otherData = 0);
    void reset(void);

    void setType(bool type) { mTypeIsTime = type; }
    void resetSelectedRange();

    void setSelectedRange(double nLeft, double nRight);
    void setRange(int startIndex, int endIndex);
    void clearSelectedRange(void);

    void getNearPoint(double x, QPointF &point, bool &isOtherData, double &otherData);

    QList<QPointF> &data(void);
    qint64 len(void) const { return m_data.size(); }

    bool getXRange(double &xMin, double &xMax);
    bool getYRange(double &yMin, double &yMax);

    DataAnalyseInfo getDataAnalyseInfo(void);
    QPair<qint64, qint64> &getCurrentRange() { return mCurrentRange; }

private:
    qint64 getNearPos(double x);

private:
    QList<QPointF> m_data;
    QPair<qint64, qint64> mCurrentRange; // 下标Index
    bool m_isOther;
    QList<qreal> m_otherData; // 只用于模式2，QuiKIS用,用于保存频率数据
    bool mTypeIsTime;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemPlotCurve : public QwtPlotCurve
{
public:
    ItemPlotCurve(QString strTitle /*,ItemPlotCurve* cloneCurve = nullptr*/);
    virtual ~ItemPlotCurve();

public:
    ItemPlotCurveSeriesData *curveData(void) { return m_curveData; }

    ItemPlotCurve *clone(void);

    QReadWriteLock &getCurveLock();

public:
    virtual void drawSeries(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect,
                            int from, int to) const;

private:
    ItemPlotCurveSeriesData *m_curveData;
    mutable QReadWriteLock mCurveLock;
};

#endif // ITEMPLOTCURVE_H

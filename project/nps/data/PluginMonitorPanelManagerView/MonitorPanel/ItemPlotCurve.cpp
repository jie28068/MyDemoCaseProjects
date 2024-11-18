#include "ItemPlotCurve.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemPlotCurve::ItemPlotCurve(QString strTitle /*,ItemPlotCurve* cloneCurve*/) : QwtPlotCurve(strTitle)
{
    this->setLegendIconSize(QSize(8, 8));

    // 曲线平滑
    //  setCurveAttribute(QwtPlotCurve::Fitted,true);
    // 抗锯齿
    setRenderHint(QwtPlotItem::RenderAntialiased);
    setPaintAttribute(QwtPlotCurve::ImageBuffer, true);
    setRenderThreadCount(0);
    m_curveData = new ItemPlotCurveSeriesData;
    this->setData(m_curveData);
}

ItemPlotCurve::~ItemPlotCurve()
{
    if (mCurveLock.tryLockForWrite()) {
        mCurveLock.unlock();
    }
}

ItemPlotCurve *ItemPlotCurve::clone(void)
{
    ItemPlotCurve *curve = new ItemPlotCurve(this->title().text());
    *(curve->m_curveData) = *m_curveData;
    return curve;
}

QReadWriteLock &ItemPlotCurve::getCurveLock()
{
    QReadLocker tmpLock(&mCurveLock);
    return mCurveLock;
}

void ItemPlotCurve::drawSeries(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect,
                               int from, int to) const
{
    QReadLocker tmpLock(&mCurveLock);
    if (!m_curveData)
        return;
    QList<QPointF> &allData = m_curveData->data();
    qint64 tmpFrom = 0;
    qint64 tmpTo = m_curveData->len() - 1;
    QMap<qint64, qint64> allSec;
    for (qint64 i = tmpFrom; i < allData.size();) {
        if (qIsNaN(allData[i].y()) || qIsInf(allData[i].y())) {
            ++i;
            continue;
        }

        tmpFrom = i;
        for (qint64 j = tmpFrom; j < allData.size();) {
            if (qIsNaN(allData[j].y()) || qIsInf(allData[j].y())) {
                tmpTo = j - 1;
                if (tmpTo < 0)
                    tmpTo = 0;
                i = j;
                break;
            } else {
                ++j;
                i = j;
                tmpTo = j;
                if (tmpTo > allData.size() - 1)
                    tmpTo = allData.size() - 1;
            }
        }

        allSec.insert(tmpFrom, tmpTo);
    }
    for (qint64 &tpfrom : allSec.keys()) {
        QwtPlotCurve::drawSeries(p, xMap, yMap, canvasRect, tpfrom, allSec[tpfrom]);
    }
}

////////////////////////////////////////////////////////////////////////////////

ItemPlotCurveSeriesData::ItemPlotCurveSeriesData() : mCurrentRange(-1, -1), m_isOther(false), mTypeIsTime(true)
{
    d_boundingRect.setRect(0, 0, 0, 0);
}

ItemPlotCurveSeriesData::ItemPlotCurveSeriesData(const ItemPlotCurveSeriesData &d)
{
    m_data = d.m_data;
    m_isOther = d.m_isOther;
    mCurrentRange = d.mCurrentRange;
    m_otherData = d.m_otherData;
    mTypeIsTime = d.mTypeIsTime;
    d_boundingRect = d.d_boundingRect;
}

ItemPlotCurveSeriesData &ItemPlotCurveSeriesData::operator=(const ItemPlotCurveSeriesData &d)
{
    m_data = d.m_data;
    m_isOther = d.m_isOther;
    mCurrentRange = d.mCurrentRange;
    m_otherData = d.m_otherData;
    mTypeIsTime = d.mTypeIsTime;
    d_boundingRect = d.d_boundingRect;
    return *this;
}

ItemPlotCurveSeriesData::~ItemPlotCurveSeriesData() { }

size_t ItemPlotCurveSeriesData::size() const
{
    return m_data.size();

    //  优化缩放,这里暂时未启用,因为nan值在缩放时会飞
    if (mCurrentRange.first == -1 || mCurrentRange.second == -1) {
        return m_data.size();
    }
    qint64 nTemp = m_data.size() - mCurrentRange.first - (mCurrentRange.second - mCurrentRange.first);
    if (nTemp == 1)
        return mCurrentRange.second - mCurrentRange.first + 1;
    else if (nTemp > 1)
        return mCurrentRange.second - mCurrentRange.first + 2;
    return mCurrentRange.second - mCurrentRange.first;
}

QPointF ItemPlotCurveSeriesData::sample(size_t i) const
{
    if (i > m_data.size())
        return QPointF();

    if (mTypeIsTime) {
        return m_data[i];
    } else {
        return QPointF(i, m_data[i].y());
    }

    // 优化缩放,这里暂时未启用,因为nan值在缩放时会飞
    if (mTypeIsTime) {
        if (mCurrentRange.first < 0 || mCurrentRange.second < 0) {
            return m_data[i];
        }

        if (mCurrentRange.first + i <= mCurrentRange.second) {
            return m_data[mCurrentRange.first + i];
        } else {
            return m_data.back();
        }
    } else {
        if (mCurrentRange.first < 0 || mCurrentRange.second < 0) {
            return QPointF(i, m_data[i].y());
        }

        if (mCurrentRange.first + i <= mCurrentRange.second && mCurrentRange.second < m_data.size()) {
            return QPointF(mCurrentRange.first + i, m_data[mCurrentRange.first + i].y());
        } else {
            return QPointF();
        }
    }
}

// 数据范围，如果QwtPlot::setAxisAutoScale 没有设成true,不会进此函数
QRectF ItemPlotCurveSeriesData::boundingRect() const
{
    return d_boundingRect;
}

void ItemPlotCurveSeriesData::append(const QPointF &point, const qreal &otherData)
{
    if (m_data.isEmpty()) {
        if (qIsNaN(point.y()) || qIsInf(point.y())) {
            d_boundingRect.setTop(0);
            d_boundingRect.setBottom(0);
        } else {
            d_boundingRect.setTop(point.y());
            d_boundingRect.setBottom(point.y());
        }
        mCurrentRange.first = 0;
    } else {
        if (point.y() > d_boundingRect.top() && !qIsNaN(point.y()) && !qIsInf(point.y()))
            d_boundingRect.setTop(point.y());
        else if (point.y() < d_boundingRect.bottom() && !qIsNaN(point.y()) && !qIsInf(point.y()))
            d_boundingRect.setBottom(point.y());
    }

    m_data.append(point);
    mCurrentRange.second = m_data.size() - 1;
}

bool pointLessThan(const QPointF &pt1, const QPointF &pt2)
{
    return pt1.x() < pt2.x();
}

void ItemPlotCurveSeriesData::reset(void)
{
    m_data.clear();
    m_otherData.clear();
    d_boundingRect.setRect(0, 0, 0, 0);
    mCurrentRange.first = -1;
    mCurrentRange.second = -1;
}

void ItemPlotCurveSeriesData::resetSelectedRange()
{
    mCurrentRange.first = 0;
    mCurrentRange.second = m_data.size() - 1;
}

void ItemPlotCurveSeriesData::setSelectedRange(double nLeft, double nRight)
{
    mCurrentRange.first = getNearPos(nLeft);
    mCurrentRange.second = getNearPos(nRight);
}

void ItemPlotCurveSeriesData::setRange(int startIndex, int endIndex)
{
    mCurrentRange.first = startIndex;
    mCurrentRange.second = endIndex;
}

void ItemPlotCurveSeriesData::clearSelectedRange(void)
{
    mCurrentRange.first = -1;
    mCurrentRange.second = -1;
}

qint64 ItemPlotCurveSeriesData::getNearPos(double x)
{
    // 数据集空返回-3，计算位置比数据集小返回-1，大返回-2
    if (m_data.isEmpty())
        return -3;
    if (x < 0)
        return -1;

    if (mTypeIsTime) {
        if (m_data.size() == 1) {
            if (x == 0.0) {
                return x;
            } else if (x > 0.0) {
                return -2;
            }
        }

        double step = m_data[1].x() - m_data[0].x();
        int pos = x / step;
        if (pos > m_data.size() - 1)
            pos = -2;
        return pos;
    } else {
        if (x >= 0.0 && x < m_data.size()) {
            return x;
        } else {
            return -2;
        }
    }
}

void ItemPlotCurveSeriesData::getNearPoint(double x, QPointF &point, bool &isOtherData, double &otherData)
{
    qint64 pos = getNearPos(x);
    isOtherData = m_isOther;
    otherData = 0;
    if (pos < 0 || pos >= m_data.size()) {
        point = QPointF();
    } else {
        if (mTypeIsTime) {
            point = m_data[pos];
        } else {
            point = QPointF(pos, m_data[pos].y());
        }
    }
}

QList<QPointF> &ItemPlotCurveSeriesData::data(void)
{
    return m_data;
}

bool ItemPlotCurveSeriesData::getXRange(double &xMin, double &xMax)
{
    if (m_data.isEmpty() || m_data.size() == 1) {
        xMin = 0;
        xMax = 1;
        return true;
    }

    if (mTypeIsTime == 1) {
        xMin = 0;
        xMax = m_data.back().x();
    } else {
        xMin = 0;
        xMax = m_data.size() - 1;
    }

    if (xMin == xMax)
        ++xMax;
    return true;
}

bool ItemPlotCurveSeriesData::getYRange(double &yMin, double &yMax)
{
    if (m_data.isEmpty()) {
        yMin = -1;
        yMax = 1;
        return true;
    }

    bool assigned = false;
    for (int i = 0; i < m_data.size(); ++i) {
        if (qIsNaN(m_data[i].y()) || qIsInf(m_data[i].y())) // 这里屏蔽掉nan值防止y轴飞掉
            continue;

        if (!assigned) {
            yMax = m_data[i].y();
            yMin = yMax;
            assigned = true;
            continue;
        }

        if (yMax < m_data[i].y())
            yMax = m_data[i].y();
        if (yMin > m_data[i].y())
            yMin = m_data[i].y();
    }

    if (yMin == yMax)
        ++yMax;
    return true;
}

DataAnalyseInfo ItemPlotCurveSeriesData::getDataAnalyseInfo(void)
{
    DataAnalyseInfo tempAnalysis;
    if (m_data.isEmpty() || m_data.size() - 1 < mCurrentRange.second || mCurrentRange.first < 0
        || mCurrentRange.second < 0) {
        tempAnalysis.clear();
        return tempAnalysis;
    }

    tempAnalysis.maxValue = m_data[mCurrentRange.first].y();
    tempAnalysis.minValue = tempAnalysis.maxValue;
    tempAnalysis.samplingTime = m_data[mCurrentRange.second].x() - m_data[mCurrentRange.first].x();
    double sampTime = tempAnalysis.samplingTime;
    if (sampTime == 0)
        sampTime = 1;
    tempAnalysis.samplingRate = (mCurrentRange.second - mCurrentRange.first) / sampTime;

    double total = 0;
    for (int i = mCurrentRange.first; i < mCurrentRange.second; ++i) {
        // 计算时屏蔽nan值
        if (qIsNaN(m_data[i].y()) || qIsInf(m_data[i].y()))
            continue;
        if (qIsNaN(tempAnalysis.maxValue) || qIsInf(tempAnalysis.maxValue)) {
            tempAnalysis.maxValue = m_data[i].y();
            tempAnalysis.minValue = m_data[i].y();
            continue;
        }
        total += m_data[i].y();
        if (tempAnalysis.maxValue < m_data[i].y())
            tempAnalysis.maxValue = m_data[i].y();
        if (tempAnalysis.minValue > m_data[i].y())
            tempAnalysis.minValue = m_data[i].y();
    }
    int range = mCurrentRange.second - mCurrentRange.first;
    if (range == 0)
        range = 1;
    tempAnalysis.averageValue = total / range;
    return tempAnalysis;
}

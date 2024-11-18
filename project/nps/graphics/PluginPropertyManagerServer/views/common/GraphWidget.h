#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#pragma once
#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

namespace KEYUIP {
static const QString Keyword_U = "U";
static const QString Keyword_I = "I";
static const QString Keyword_P = "P";
static const QString Keyword_S = "S";
static const QString Keyword_T = "T";
}
struct UIPData {
    UIPData(const double &_U, const double &_I, const double &_P, bool _isPole = false)
        : UValue(_U), IValue(_I), PValue(_P), isPole(_isPole)
    {
    }
    double UValue;
    double IValue;
    double PValue;
    bool isPole; // 是否极点
};

struct PoleXY {
    PoleXY(const double &_x, const double &_y) : x(_x), y(_y) { }
    double x;
    double y;
};

struct GraphDataInfo {
    GraphDataInfo() : min(0), max(0), multiple(1.0), space(5), k(1), unit("") { }
    inline double range() { return max - min; }
    double min; // 坐标轴最小刻度值
    double max; // 坐标轴最大刻度值
    double multiple; // 坐标轴缩放比例，最多写4位数，超过4位用科学计数法。这个string就是科学计数法
    int space;    // 刻度分割
    double k;     // xy放大倍数=屏幕大小/范围
    QString unit; // 单位
};

class GraphWidget : public QWidget
{
public:
    GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();
    // /// @brief 设置UIP列表数据，以U为横坐标，画UI，UP图，最后一个数据为极点
    // /// @param UList 电压列表
    // /// @param IList 电流列表
    // /// @param PList 功率列表
    // void setUIPData(const QList<double> &UList, const QList<double> &IList, const QList<double> &PList);
    /// @brief 设置xy列表数据，最后为极点坐标
    /// @param xList x坐标list
    /// @param yList y坐标list
    /// @param polexy 极点
    /// @param xUnit x轴单位
    /// @param yUnit y轴单位
    void setXYDoubleData(const QList<double> &xList, const QList<double> &yList, const PoleXY &polexy,
                         const QString &xUnit = "", const QString &yUnit = "");

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    /// @brief 数据转换为窗口实际位置。
    /// @param xm 数据x
    /// @param ym 数据y
    /// @return
    const QPointF mathMaptoWinPoint(const double &xm, const double &ym);
    /// @brief xy轴区域内的坐标转换为窗口实际位置
    /// @param scPoint
    /// @return
    const QPointF screenMaptoWinPoint(const QPointF &scPoint);
    /// @brief 获取列表中最大值或最小值
    /// @param list 列表
    /// @param bmax true为最大值，false为最小值
    /// @return
    const double MinMaxValue(const QList<double> &list, bool bmax);
    /// @brief 获取科学计数法的text
    /// @param info
    /// @param lineNo
    /// @return
    const QString getRealGridText(GraphDataInfo info, const int &lineNo);
    /// @brief 获取图形转换相关信息
    /// @param maxMath 数学最大值
    /// @param minMath 数学最小值
    /// @param screenpix 屏幕的宽度或者高度
    /// @param unitstr 单位
    /// @return
    GraphDataInfo getGraphDataInfo(const double maxMath, const double &minMath, const int &screenpix,
                                   const QString &unitstr);
    /// @brief 画垂直x轴网格线
    /// @param painter
    /// @param lineNo
    void DrawGridX(QPainter *painter, const int &lineNo);
    /// @brief 画垂直y轴网格线
    /// @param painter
    /// @param lineNo
    void DrawGridY(QPainter *painter, const int &lineNo);
    /// @brief 画比例信息
    /// @param painter
    /// @param multiple
    /// @param pos
    void DrawMultiple(QPainter *painter, const double &multiple, const QPointF &pos);
    /// @brief 画xy曲线
    /// @param painter
    void DrawXYPoints(QPainter *painter);
    /// @brief 画极点
    /// @param painter
    void DrawPole(QPainter *painter);
    /// @brief 画单位
    /// @param painter
    void DrawUnit(QPainter *painter);

private:
    GraphDataInfo m_xAxisInfo; // x轴信息
    GraphDataInfo m_yAxisInfo; // y轴信息
    QList<double> m_xList;     // x实际坐标
    QList<double> m_yList;     // y实际坐标
    PoleXY m_pole;             // 极点坐标

    // double m_xRange;           // x范围
    // double m_yRange;           // y范围
    // double m_xk;               // 比例系数
    // double m_yk;               // 比例系数
    // int m_xSpace;              // x坐标分割为几个刻度
    // int m_ySpace;              // y坐标分割为几个刻度
    // double m_xMultiple;        // x放大缩小倍数
    // double m_yMultiple;        // y放大缩小倍数
};

#endif
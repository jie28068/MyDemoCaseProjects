#include "GraphWidget.h"

#include <QFontMetrics>
#include <QPainterPath>
#include <QtMath>

#include "CommonModelAssistant.h"

static const int xpadding = 40;                           // xy轴屏幕距离窗口的内边距
static const int ypadding = 40;                           // xy轴屏幕距离窗口的内边距
static const int widgetWidth = 300 + 2 * xpadding;        // 宽度
static const int widgetHeight = 240 + 2 * ypadding;       // 高度
static const QColor TEXTCOLOR_NORMAL = QColor("#252d3f"); // 文字颜色

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent),
      m_xAxisInfo(GraphDataInfo()),
      m_yAxisInfo(GraphDataInfo()),
      m_xList(QList<double>()),
      m_yList(QList<double>()),
      m_pole(0, 0)
{
    this->setFixedSize(widgetWidth, widgetHeight);
}

GraphWidget::~GraphWidget() { }

// void GraphWidget::setUIPData(const QList<double> &UList, const QList<double> &IList, const QList<double> &PList)
// {
//     this->setFixedSize(540, 440);

//     update();
// }

void GraphWidget::setXYDoubleData(const QList<double> &xList, const QList<double> &yList, const PoleXY &polexy,
                                  const QString &xUnit, const QString &yUnit)
{
    m_xAxisInfo = getGraphDataInfo(MinMaxValue(xList, true), MinMaxValue(xList, false), (this->width() - 2 * xpadding),
                                   xUnit);
    m_yAxisInfo = getGraphDataInfo(MinMaxValue(yList, true), MinMaxValue(yList, false), (this->height() - 2 * ypadding),
                                   yUnit);
    m_xList = xList;
    m_yList = yList;
    m_pole = polexy;
    if (m_xList.size() != m_yList.size()) {
        return;
    }
    update();
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    if (CMA::isEqualO(m_xAxisInfo.range()) && CMA::isEqualO(m_yAxisInfo.range()) || m_xList.size() <= 0
        || m_yList.size() <= 0) {
        return;
    }

    QPainter *painter = new QPainter(this);
    for (int n = 0; n <= m_xAxisInfo.space; ++n) // xSpaces可随意设置
    {
        DrawGridX(painter, n);
    }
    DrawMultiple(painter, m_xAxisInfo.multiple, QPoint(width() - xpadding - 20, height() - ypadding / 3 - 2));
    for (int k = 0; k <= m_yAxisInfo.space; ++k) // ySpaces可随意设置
    {
        DrawGridY(painter, k);
    }
    DrawMultiple(painter, m_yAxisInfo.multiple, QPoint(xpadding / 2, ypadding - 10));
    DrawXYPoints(painter);
    DrawPole(painter);
    DrawUnit(painter);
}

const QPointF GraphWidget::mathMaptoWinPoint(const double &xm, const double &ym)
{
    return QPointF(qRound64((xm - m_xAxisInfo.min) * m_xAxisInfo.k) + xpadding,
                   height() - ypadding - qRound64((ym - m_yAxisInfo.min) * m_yAxisInfo.k));
}

const QPointF GraphWidget::screenMaptoWinPoint(const QPointF &scPoint)
{
    return QPointF(scPoint.x() + xpadding, height() - ypadding - scPoint.y());
}

const double GraphWidget::MinMaxValue(const QList<double> &list, bool bmax)
{
    double targetValue = 0.0;
    for (double dvlue : list) {
        if (bmax && targetValue < dvlue) {
            targetValue = dvlue;
        } else if (!bmax && targetValue > dvlue) {
            targetValue = dvlue;
        }
    }
    return targetValue;
}

const QString GraphWidget::getRealGridText(GraphDataInfo info, const int &lineNo)
{
    if (info.space == 0 || CMA::isEqualO(info.multiple)) {
        return QString();
    }
    if (info.multiple > 1) {
        return QString("%1").arg(QString::number(((info.range() / info.space) * lineNo) * info.multiple));
    } else if (info.multiple < 0.01) {
        return QString("%1").arg(QString::number(((info.range() / info.space) * lineNo) * info.multiple));
    } else {
        return QString("%1").arg(QString::number(info.range() / info.space * lineNo));
    }
}

GraphDataInfo GraphWidget::getGraphDataInfo(const double maxMath, const double &minMath, const int &screenpix,
                                            const QString &unitstr)
{
    GraphDataInfo graphInfo;
    double mathRange = maxMath - minMath;
    // 倍数，放大或缩小
    double multiple = 1.0;
    if (mathRange > 10.0) {
        while (mathRange > 10.0) {
            multiple = multiple / 10.0;
            mathRange = mathRange / 10.0;
        }
    } else if (mathRange < 1.0 && mathRange > 0) {
        while (mathRange < 1.0) {
            multiple = multiple * 10.0;
            mathRange = mathRange * 10.0;
        }
    } else if (CMA::isEqualO(mathRange)) {
        graphInfo.min = qFloor(minMath);
        graphInfo.max = graphInfo.min + multiple * 1;
        return graphInfo;
    }

    graphInfo.multiple = multiple;
    graphInfo.space = qCeil(mathRange);
    double range = graphInfo.space / multiple;
    graphInfo.min = qFloor(minMath);
    graphInfo.max = range - graphInfo.min;
    graphInfo.k = screenpix / range;
    if (graphInfo.space < 5) {
        graphInfo.space = graphInfo.space * 2;
    }
    graphInfo.unit = unitstr;
    return graphInfo;
}

void GraphWidget::DrawGridX(QPainter *painter, const int &lineNo)
{
    if (painter == nullptr || m_xAxisInfo.space == 0) {
        return;
    }
    painter->save();
    painter->setPen(QPen(TEXTCOLOR_NORMAL, 1));
    painter->setFont(QFont("Arial", 8));
    int xPixel = ((this->width() - 2 * xpadding) * lineNo / m_xAxisInfo.space);
    // 刻度线上两点
    QPointF xGridPoint(xPixel, 0);
    QPointF xGridTopPoint(xPixel, (this->height() - 2 * ypadding));
    // 屏幕坐标转换成窗口坐标
    QPointF xGridPointQt = screenMaptoWinPoint(xGridPoint);
    QPointF xGridTopPointQt = screenMaptoWinPoint(xGridTopPoint);
    // 绘制刻度线
    painter->drawLine(xGridPointQt, xGridTopPointQt);

    // 写上刻度
    QPointF textPoint = xGridPoint + QPointF(-5, -ypadding / 3);
    QPointF textPointQt = screenMaptoWinPoint(textPoint);
    painter->drawText(textPointQt, getRealGridText(m_xAxisInfo, lineNo));
    painter->restore();
}

void GraphWidget::DrawGridY(QPainter *painter, const int &lineNo)
{
    if (painter == nullptr || m_yAxisInfo.space == 0) {
        return;
    }
    painter->save();
    painter->setPen(QPen(TEXTCOLOR_NORMAL, 1));
    painter->setFont(QFont("Arial", 8));
    int yPixel = (this->height() - 2 * ypadding) * lineNo / m_yAxisInfo.space;

    // 刻度线上两点
    QPointF yGridPoint(0, yPixel);
    QPointF yGridRightPoint((this->width() - 2 * xpadding), yPixel);
    // 屏幕坐标转换成窗口坐标
    QPointF yGridPointQt = screenMaptoWinPoint(yGridPoint);
    QPointF yGridRightPointQt = screenMaptoWinPoint(yGridRightPoint);
    // 绘制刻度线
    painter->drawLine(yGridPointQt, yGridRightPointQt);

    // 写上刻度
    QPointF textPoint = yGridPoint + QPointF(-xpadding / 2, -5);
    QPointF textPointQt = screenMaptoWinPoint(textPoint);
    painter->drawText(textPointQt, getRealGridText(m_yAxisInfo, lineNo));
    painter->restore();
}

void GraphWidget::DrawMultiple(QPainter *painter, const double &multiple, const QPointF &pos)
{
    if (painter == nullptr) {
        return;
    }
    painter->save();
    painter->setPen(QPen(TEXTCOLOR_NORMAL, 1));
    painter->setFont(QFont("Arial", 8));
    double tmpmultiple = multiple;
    if (multiple > 1) {
        int divno = 0;
        while (tmpmultiple > 1) {
            tmpmultiple = tmpmultiple / 10.0;
            ++divno;
        }
        painter->drawText(pos, QString("(e-%1)").arg(QString::number(divno)));
    } else if (multiple < 0.01) {
        int mulno = 0;
        while (tmpmultiple < 1) {
            tmpmultiple = tmpmultiple * 10.0;
            ++mulno;
        }
        painter->drawText(pos, QString("(e+%1)").arg(QString::number(mulno)));
    } else {
        // do nothing
    }
    painter->restore();
}

void GraphWidget::DrawXYPoints(QPainter *painter)
{
    if (painter == nullptr) {
        return;
    }
    if (m_xList.size() != m_yList.size() || m_xList.size() <= 0 || m_yList.size() <= 0) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(Qt::blue, 2));
    QPainterPath path(mathMaptoWinPoint(m_xList[0], m_yList[0]));
    for (int i = 0; i < m_xList.size() - 1; ++i) {
        // 控制点的 x 坐标为 sp 与 ep 的 x 坐标和的一半
        // 第一个控制点 c1 的 y 坐标为起始点 sp 的 y 坐标
        // 第二个控制点 c2 的 y 坐标为结束点 ep 的 y 坐标
        QPointF startPoint(mathMaptoWinPoint(m_xList[i], m_yList[i]));
        QPointF endPoint(mathMaptoWinPoint(m_xList[i + 1], m_yList[i + 1]));
        // QPointF c1 = QPointF((startPoint.x() + endPoint.x()) / 2, startPoint.y());
        // QPointF c2 = QPointF((startPoint.x() + endPoint.x()) / 2, endPoint.y());
        // path.quadTo(c1, c2);
        painter->drawLine(startPoint, endPoint);
        // path.cubicTo(c1, c2, endPoint);
    }
    painter->restore();
    // painter->drawPath(path);
}

void GraphWidget::DrawPole(QPainter *painter)
{
    if (painter == nullptr) {
        return;
    }
    painter->save();
    painter->setPen(QPen(Qt::red, 1));
    QPointF poleQt = mathMaptoWinPoint(m_pole.x, m_pole.y);
    painter->drawEllipse(poleQt, 4, 4);
    painter->setPen(QPen(TEXTCOLOR_NORMAL, 1));
    painter->setFont(QFont("Arial", 10));
    QString textstr = QString("(X:%1,Y:%2)").arg(QString::number(m_pole.x)).arg(QString::number(m_pole.y));
    int textwidth = painter->fontMetrics().width(textstr);
    if (textwidth + poleQt.x() > width() - 5) {
        painter->drawText(poleQt + QPointF(-(poleQt.x() + textwidth - width() + 5), -10), textstr);
    } else {
        painter->drawText(poleQt + QPointF(-10, -10), textstr);
    }
    painter->restore();
}

void GraphWidget::DrawUnit(QPainter *painter)
{
    if (painter == nullptr) {
        return;
    }
    painter->save();
    painter->setPen(QPen(TEXTCOLOR_NORMAL, 1));
    painter->setFont(QFont("Arial", 10));
    int xwidth = painter->fontMetrics().width(m_xAxisInfo.unit);
    int ywidth = painter->fontMetrics().width(m_yAxisInfo.unit);
    painter->drawText(QPointF((width() - xwidth) / 2, height() - ypadding / 3), m_xAxisInfo.unit);
    painter->translate(xpadding / 3, (height() + ywidth) / 2); // 第1步：变换旋转中心到所绘制文字左下角
    painter->rotate(-90);                                      // 第2步： 旋转一定角度
    painter->drawText(0, 0, m_yAxisInfo.unit);                 // 第3步： 绘制文字
    painter->resetMatrix();                                    // 第4步： 恢复坐标旋转
    painter->restore();
    // painter->drawLine(QPointF(0, 0), QPointF(width(), 0));
    // painter->drawLine(QPointF(width(), 0), QPointF(width(), height()));
    // painter->drawLine(QPointF(0, 0), QPointF(0, height()));
    // painter->drawLine(QPointF(0, height()), QPointF(width(), height()));
}

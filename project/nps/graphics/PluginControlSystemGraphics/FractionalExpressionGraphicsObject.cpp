#include "FractionalExpressionGraphicsObject.h"
#include "qmath.h"

#include <QPainter>

FractionalExpressionGraphicsObject::FractionalExpressionGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    letter = TRANSFUNC_CONTINUE_LETTER;
    proxyGraphicsObject = nullptr;
    moleculatItem = nullptr;
    denominatorItem = nullptr;
    proxyGraphicsObject = getTransformProxyGraphicsObject();
    textFont.setPointSizeF(8);
    textFont.setFamily("Microsoft YaHei UI");
}

FractionalExpressionGraphicsObject::~FractionalExpressionGraphicsObject() { }

void FractionalExpressionGraphicsObject::setMolecularExpressions(const QString &molecular)
{
    molecularExpression = molecular;
    if (!moleculatItem) {
        moleculatItem = new QGraphicsTextItem(this);
        moleculatItem->setFont(textFont);
    }
    moleculatItem->setPlainText(molecularExpression);
}

void FractionalExpressionGraphicsObject::setDenominatorExpression(const QString &denominator)
{
    denominatorExpression = denominator;
    if (!denominatorItem) {
        denominatorItem = new QGraphicsTextItem(this);
        denominatorItem->setFont(textFont);
    }
    denominatorItem->setPlainText(denominatorExpression);
}

void FractionalExpressionGraphicsObject::setTextFont(const QFont &font)
{
    textFont = font;
    if (moleculatItem) {
        moleculatItem->setFont(textFont);
    }
    if (denominatorItem) {
        denominatorItem->setFont(textFont);
    }
}

void FractionalExpressionGraphicsObject::updatePosition()
{

    if (moleculatItem && denominatorItem) {
        int molecularTextWidth = moleculatItem->boundingRect().width();
        int denominatorTextWidth = denominatorItem->boundingRect().width();
        int maxTextWidth = qMax(molecularTextWidth, denominatorTextWidth);

        QRectF rc = boundingRect();

        qreal x = rc.left() + (rc.width() - maxTextWidth) / 2;
        qreal y = rc.top() + rc.height() / 2;

        qreal molecularYPos = rc.top() + rc.height() / 2 - moleculatItem->boundingRect().height() + 2;
        qreal molecularXPos = rc.left() + (rc.width() - molecularTextWidth) / 2;
        moleculatItem->setPos(QPointF(molecularXPos, molecularYPos));

        qreal denominatorXPos = rc.left() + (rc.width() - denominatorTextWidth) / 2;
        qreal denominatorYPos = rc.top() + rc.height() / 2 - 2;
        denominatorItem->setPos(QPointF(denominatorXPos, denominatorYPos));
    }
}

void FractionalExpressionGraphicsObject::processDisplayData(QString dataStr)
{

    QStringList parts = dataStr.split('|', QString::SkipEmptyParts);
    if (parts.size() <= 1) {
        return;
    }
    QString numStr = parts[0];
    QString denStr = parts[1];
    QStringList numParts = numStr.split(',', QString::SkipEmptyParts);
    QStringList denParts = denStr.split(',', QString::SkipEmptyParts);

    auto GenExpression = [this](const QStringList &ndParts, QString &ndExpression) {
        QString strPow;
        QString sigOper;
        const int ndPartsSize = ndParts.size();
        for (int i = 0; i < ndPartsSize; i++) {
            const double numVal = ndParts[i].toDouble();
            if (numVal != 0) {
                int pow = ndPartsSize - i - 1;
                if (pow > 1) {
                    strPow = QString(letter) + QString("^%1").arg(pow);
                } else if (pow == 1) {
                    strPow = QString(letter);
                } else {
                    strPow.clear();
                }
                if (ndExpression.isEmpty()) {
                    sigOper.clear();
                } else if (numVal > 0) {
                    sigOper = "+";
                } else {
                    sigOper.clear();
                }
                if (!strPow.isEmpty()) {
                    if (numVal == 1) {
                        ndExpression += sigOper + strPow;
                    } else if (numVal == -1) {
                        ndExpression += "-" + strPow;
                    } else {
                        ndExpression += sigOper + QString::number(numVal) + strPow;
                    }
                } else {
                    ndExpression += sigOper + QString::number(numVal);
                }
            }
        }
    };
    //  分子
    QString numExpression;
    GenExpression(numParts, numExpression);
    setMolecularExpressions(numExpression);
    // 分母
    QString denExpression;
    GenExpression(denParts, denExpression);
    setDenominatorExpression(denExpression);
    updatePosition();
    update();
}

void FractionalExpressionGraphicsObject::onSourceChanged(QString key, QVariant value) { }

void FractionalExpressionGraphicsObject::onSourcePropertyChanged(QString key, QVariant value)
{
    // if (key == SOURCE_DISPLAYDATA_PROPERTY_KEY) {
    //     QString dataStr = value.toString();
    //     processDisplayData(dataStr);
    // }
}

QRectF FractionalExpressionGraphicsObject::boundingRect() const
{
    if (!proxyGraphicsObject) {
        return QRectF();
    }
    return proxyGraphicsObject->boundingRect();
}

void FractionalExpressionGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                               QWidget *widget)
{
    if (moleculatItem && denominatorItem) {
        int molecularTextWidth = moleculatItem->boundingRect().width();
        int denominatorTextWidth = denominatorItem->boundingRect().width();
        int maxTextWidth = qMax(molecularTextWidth, denominatorTextWidth);

        QRectF rc = boundingRect();

        painter->save();

        qreal x = rc.left() + (rc.width() - maxTextWidth) / 2;
        qreal y = rc.top() + rc.height() / 2;

        painter->drawLine(QPointF(x, y), QPointF(x + maxTextWidth, y));

        painter->restore();
    }
}

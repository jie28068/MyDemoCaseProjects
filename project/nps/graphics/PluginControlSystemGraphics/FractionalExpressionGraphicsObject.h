#pragma once

/*
分数表达式
*/

#include "defines.h"

static const QChar TRANSFUNC_CONTINUE_LETTER = 's'; // 连续传递函数表达式
static const QChar TRANSFUNC_DISCRETE_LETTER = 'z'; // 离散传递函数表达式

class FractionalExpressionGraphicsObject : public GraphicsLayer
{
    Q_OBJECT
public:
    FractionalExpressionGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~FractionalExpressionGraphicsObject();

    void setLetter(const QChar &let) { letter = let; }

    void setMolecularExpressions(const QString &molecular);

    void setDenominatorExpression(const QString &denominator);

    void setTextFont(const QFont &font);

    virtual void updatePosition();

    void processDisplayData(QString str);

public slots:
    void onSourceChanged(QString key, QVariant value);

    void onSourcePropertyChanged(QString key, QVariant value);

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QChar letter;
    QString molecularExpression;
    QString denominatorExpression;
    QFont textFont;
    TransformProxyGraphicsObject *proxyGraphicsObject;
    QGraphicsTextItem *moleculatItem;
    QGraphicsTextItem *denominatorItem;
};

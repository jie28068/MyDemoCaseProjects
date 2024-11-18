#pragma once

#include "ControlModuleSourceGraphicsObject.h"
#include "FractionalExpressionGraphicsObject.h"
#include "defines.h"

class TransferFunctionSourceGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    TransferFunctionSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                         const QChar &let, QGraphicsItem *parent = 0);
    virtual ~TransferFunctionSourceGraphicsObject();

    virtual void createInternalGraphics();

public slots:
    virtual void onSourceChange(QString, QVariant);

    virtual void onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue);

private:
    FractionalExpressionGraphicsObject *fractionalExpression;
    QChar letter;
};
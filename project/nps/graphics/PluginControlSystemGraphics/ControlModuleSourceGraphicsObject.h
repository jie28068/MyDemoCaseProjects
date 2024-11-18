#ifndef CONTROLMODULESOURCEGRAPHICSOBJECT_H
#define CONTROLMODULESOURCEGRAPHICSOBJECT_H

#include "ControlSVGGraphicsObject.h"
#include "DisplayParameterTextGraphicsObject.h"
#include "defines.h"

class SourceProtoTypeTextGraphicsObject;

class ControlModuleSourceGraphicsObject : public SourceGraphicsObject
{
    Q_OBJECT

public:
    ControlModuleSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                      QGraphicsItem *parent = 0);
    ~ControlModuleSourceGraphicsObject();

    virtual void createInternalGraphics();

    virtual void updatePosition();

    virtual QSizeF getMinimumSize() const override;

    /// @brief 该模块是否需要显示模块原型名
    bool isShowProTypeName();

    virtual void selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual void userMenu(QSharedPointer<QMenu> menu) override;

public slots:
    virtual void onSourceChange(QString, QVariant);

    virtual void onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue);
    void onDataTextChange(QMap<QString, QMap<QString, QVariant>> dataMap);

protected:
    ControlSVGGraphicsObject *controlSvgGraphics;
    SourceProtoTypeTextGraphicsObject *protoTypeText;
    /// @brief 用于展示会变化的参数 比如常数模块的常数等
    DisplayParameterTextGraphicsObject *displayText;
};

#endif // CONTROLMODULESOURCEGRAPHICSOBJECT_H

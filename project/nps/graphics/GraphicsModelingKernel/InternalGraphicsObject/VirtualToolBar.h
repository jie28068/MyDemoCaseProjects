#pragma once

#include <QGraphicsObject>
#include <QScopedPointer>

class ButtonGraphics;

class VirtualToolBarPrivate;
class VirtualToolBar : public QGraphicsObject
{
    Q_OBJECT
public:
    VirtualToolBar(QGraphicsItem *parent = nullptr);
    ~VirtualToolBar();

    void appendButtonGraphics(ButtonGraphics *button);

    QRectF boundingRect() const override;

private:
    void updateButtonsLayout();

private:
    QScopedPointer<VirtualToolBarPrivate> dataPtr;
};
#include "VirtualToolBar.h"
#include "ButtonGraphics.h"

#include <QList>

class VirtualToolBarPrivate
{
public:
    QList<ButtonGraphics *> buttons;
    QRectF boundingRect;
};

VirtualToolBar::VirtualToolBar(QGraphicsItem *parent) : QGraphicsObject(parent)
{
    dataPtr.reset(new VirtualToolBarPrivate());
}

VirtualToolBar::~VirtualToolBar() { }

void VirtualToolBar::appendButtonGraphics(ButtonGraphics *button)
{
    if (button) {
        dataPtr->buttons.append(button);
        button->setParentItem(this);
        QRectF buttonRect = button->boundingRect();
        dataPtr->boundingRect.setWidth(dataPtr->boundingRect.width() + buttonRect.width());
        if (buttonRect.height() > dataPtr->boundingRect.height()) {
            dataPtr->boundingRect.setHeight(buttonRect.height());
        }
        updateButtonsLayout();
    }
}

QRectF VirtualToolBar::boundingRect() const
{
    return dataPtr->boundingRect;
}

void VirtualToolBar::updateButtonsLayout()
{
    if (dataPtr->buttons.isEmpty()) {
        return;
    }
    int xOffset = 0;
    foreach (auto button, dataPtr->buttons) {
        if (button->isVisible()) {
            button->setPos(QPointF(xOffset, 0));
            xOffset += button->boundingRect().width();
        }
    }
}

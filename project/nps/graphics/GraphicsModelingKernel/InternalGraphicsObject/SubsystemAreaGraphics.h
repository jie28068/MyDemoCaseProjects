#pragma once

#include <QGraphicsObject>
#include <QScopedPointer>

class SubsystemAreaGraphicsPrivate;
class SubsystemAreaGraphics : public QGraphicsObject
{
    Q_OBJECT
public:
    SubsystemAreaGraphics(QGraphicsItem *parent = nullptr);
    ~SubsystemAreaGraphics();

    void setBoundingRect(const QRectF &rc);

    QRectF boundingRect() const override;

public slots:
    void onClicked(int eventID);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void createSubSystem();

private:
    QScopedPointer<SubsystemAreaGraphicsPrivate> dataPtr;
};
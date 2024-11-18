#pragma once

#include <QGraphicsObject>
#include <QPixmap>
#include <QScopedPointer>

class ButtonGraphicsPrivate;
class ButtonGraphics : public QGraphicsObject
{
    Q_OBJECT
public:
    ButtonGraphics(int eventID, QGraphicsItem *parent = nullptr);
    ~ButtonGraphics();

    /// @brief 设置按钮的大小
    /// @param size
    void setSize(const QSize &size);

    /// @brief 设置按钮的背景颜色
    /// @param color
    void setBackgroundColor(const QColor &color);

    /// @brief 设置鼠标悬浮时的背景颜色
    /// @param color
    void setHoverBackgroundColor(const QColor &color);

    QRectF boundingRect() const override;

    void setToolTip(const QString &toolTip);

    void setIcon(const QPixmap &pixmap);

    void setHoverIcon(const QPixmap &pixmap);

signals:
    void clicked(int eventID);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void drawIcon(QPainter *painter, const QPixmap &pixmap);

private:
    QScopedPointer<ButtonGraphicsPrivate> dataPtr;
};
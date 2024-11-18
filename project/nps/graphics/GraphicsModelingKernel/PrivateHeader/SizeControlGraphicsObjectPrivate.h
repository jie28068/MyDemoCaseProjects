#pragma once
#include "SizeControlGraphicsObject.h"
#include "Utility.h"

class SizeControlGraphicsObjectPrivate
{
public:
    SizeControlGraphicsObjectPrivate();
    /// @brief 大小控制块代表控制方向
    SizeControlGraphicsObject::Direction direction;

    /// @brief 控制块id
    QString id;

    /// @brief 鼠标点击时的场景坐标
    QPointF pressPos;

    /// @brief 画刷颜色,填充色
    QColor brushColor;
};

SizeControlGraphicsObjectPrivate::SizeControlGraphicsObjectPrivate()
{
    direction = SizeControlGraphicsObject::kDirectionInvalid;
    id = Utility::createUUID();
    brushColor = QColor(Qt::white);
}
#pragma once

#include <QMap>
#include <QPoint>
#include <QRect>
#include <QString>

class SourceNameTextGraphicsObjectPrivate
{
public:
    SourceNameTextGraphicsObjectPrivate();

    bool isMousePressed;
    QMap<int, QPointF> angleOffset;
    bool isPositionChanged;
    QPointF pointPositionChangedDelta;
    QRectF oldRecf;
    int oldAngle;
    QString originalText;
    bool isChanged; // 文本改变
    bool isHighLight;
    QColor highLightColor;
    QColor normalColor;
    bool isGotoFromHight;
};

SourceNameTextGraphicsObjectPrivate::SourceNameTextGraphicsObjectPrivate()
{
    isMousePressed = false;
    isPositionChanged = false;
    oldAngle = -1;
    isChanged = false;
    isHighLight = false;
    isGotoFromHight = false;
}
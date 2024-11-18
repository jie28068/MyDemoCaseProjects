#pragma once
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include <QMenu>

class SourceGraphicsObjectPrivate
{
public:
    QSharedPointer<SourceProxy> sourceProxy;

    QRectF sourceBoundingRect;

    bool isHover;

    QSizeF minimumSize;

    SourceGraphicsObject::ScaleDirection scaleDirection;
    SourceGraphicsObject::AlignmentType xAlignmentType;
    SourceGraphicsObject::AlignmentType yAlignmentType;
};
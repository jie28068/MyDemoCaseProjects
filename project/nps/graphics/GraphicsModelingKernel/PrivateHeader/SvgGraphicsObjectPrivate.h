#pragma once
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"

#include <QSvgRenderer>

class SvgGraphicsObjectPrivate
{
public:
    SvgGraphicsObjectPrivate();

    PSourceProxy m_pSource;
    SourceGraphicsObject *m_sourceGrapics;

    /// @brief 当前旋转角度
    int m_angle;

    /// @brief key为旋转角度
    QMap<int, QByteArray> m_mapSVGContents;
    QSvgRenderer *m_svgRender;
    QRectF m_svgBoundingRect;
};

SvgGraphicsObjectPrivate::SvgGraphicsObjectPrivate()
{
    m_angle = 0;
    m_sourceGrapics = nullptr;
    m_svgRender = nullptr;
}

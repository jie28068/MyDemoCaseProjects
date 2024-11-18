#pragma once

#include "GraphicsLayer.h"
#include "SourceProxy.h"

class SourceGraphicsObject;
class QSvgRenderer;
class SvgGraphicsObjectPrivate;

/// @brief svg列表图元，目前支持4个角度(0,90,180,270)的旋转SVG显示
class GRAPHICSMODELINGKERNEL_EXPORT SvgGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

public:
    SvgGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~SvgGraphicsObject();

    // /// @brief 设置SVG图片列表，目前最多有效支持的图片数为4张，按照列表顺序分别代表0,90,180,270度的旋转
    // /// @param imagesPath
    // void setSvgImagesPath(QStringList imagesPath);

    void setSvgImageDatas(const QVariantMap &svgImageDatas);

    /// @brief 设置旋转角度，根据旋转角度选择渲染的SVG图片
    /// @param angle
    void setAngle(int angle);

    QSizeF getSVGSize();

    QSvgRenderer *getSvgRenderer() const;

    QRectF getProxyRect() const;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual QRectF boundingRect() const override;

private:
    // void initSvgMap(QStringList picPathList);
    void initSvgMap(const QVariantMap &svgImageDatas);
    void reloadSVGRender();
    void reclacBoundingRect();

public slots:
    void onSourceChange(QString key, QVariant val); // @key:属性名称 @val:属性值

    void onLayerPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

protected:
    QScopedPointer<SvgGraphicsObjectPrivate> dataPtr;

    // PSourceProxy m_pSource;
    // SourceGraphicsObject *m_sourceGrapics;

    // int m_angle;                            //当前旋转角度
    // QMap<int, QByteArray> m_mapSVGContents; // key为旋转角度
    // QSvgRenderer *m_svgRender;
    // QRectF m_svgBoundingRect;
};

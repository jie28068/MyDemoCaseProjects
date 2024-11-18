#include "SawtoothWavesGraphicsObject.h"
#include "SelftDrawSvgGraphicsObject.h"

SawtoothWavesGraphicsObject::SawtoothWavesGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                   QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent), m_selfSvg(nullptr)
{
}

SawtoothWavesGraphicsObject::~SawtoothWavesGraphicsObject() { }

void SawtoothWavesGraphicsObject::createInternalGraphics() { 

    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();

    m_selfSvg = new SelftDrawSvgGraphicsObject(getCanvasScene(),sourceProxy,this,proxyGraphics);
    

    bool allowScale = sourceProperty.isEnableScale();
    if (allowScale) {
        setScaleDirection(SourceGraphicsObject::kScaleAll);
    } else {
        setScaleDirection(SourceGraphicsObject::kScaleNone);
    }

    auto backgroundImageData = sourceProperty.getBackgroundImage();
    if (!backgroundImageData.isNull()) {
        if (protoTypeText) {
            protoTypeText->setVisible(false);
        }
    }

    updatePosition();
}

void SawtoothWavesGraphicsObject::selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_selfSvg)
        return;
    painter->save();
    //auto rc = m_selfSvg->boundingRect();
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    QPolygonF points;
    if(getSourceProxy()) {
        double degress = getSourceProxy()->getSourceProperty().getPhaseDegress().toDouble();
        points = getPointsByDegress(degress);
    }
    painter->drawPolyline(points);
    painter->restore();
}

/* void SawtoothWavesGraphicsObject::updatePosition() 
{ 
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if(!proxyGraphics)
        return;
    QSizeF size = proxyGraphics->boundingRect().size();
    if(size.width() > 40){
        size.setWidth(40);
    }
    if(size.height() > 40){
        size.setHeight(40);
    }

    if(m_selfSvg){
        m_selfSvg->setBoundingRect(QRectF(0,0,size.width(),size.height()));
        m_selfSvg->setPos(proxyGraphics->boundingRect().center() - QPointF(size.width() / 2,size.height() / 2));
    }

} */

void SawtoothWavesGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) { }

QPolygonF SawtoothWavesGraphicsObject::getPointsByDegress(double degress)
{
    QRectF rc = getTransformBoundingRect();
    double moduloDegress = fmod(degress,360);
    moduloDegress = fmod(moduloDegress + 360,360);
    QPolygonF points;
    if(moduloDegress < 90){
        points.append(rc.bottomLeft());
        points.append(QPointF(rc.center().x(), rc.top()));
        points.append(QPointF(rc.center().x(), rc.bottom()));
        points.append(rc.topRight());
    }else if(moduloDegress < 180){
        points.append(QPointF(rc.left(),  rc.bottom() - rc.height() * 0.25));
        points.append(QPointF(rc.left() + rc.width() * 0.375, rc.top()));
        points.append(QPointF(rc.left() + rc.width() * 0.375, rc.bottom()));
        points.append(QPointF(rc.left() + rc.width() * 0.875, rc.top()));
        points.append(QPointF(rc.left() + rc.width() * 0.875, rc.bottom()));
        points.append(QPointF(rc.right(), rc.bottom() - rc.height() * 0.25));
    }else if(moduloDegress < 270){
        points.append(QPointF(rc.left(),  rc.bottom() - rc.height() * 0.5));
        points.append(QPointF(rc.left() + rc.width() * 0.25, rc.top()));
        points.append(QPointF(rc.left() + rc.width() * 0.25, rc.bottom()));
        points.append(QPointF(rc.left() + rc.width() * 0.75, rc.top()));
        points.append(QPointF(rc.left() + rc.width() * 0.75, rc.bottom()));
        points.append(QPointF(rc.right(), rc.bottom() - rc.height() * 0.5));
    }else{
        points.append(QPointF(rc.left(),  rc.bottom() - rc.height() * 0.75));
        points.append(QPointF(rc.left() + rc.width() * 0.125, rc.top()));
        points.append(QPointF(rc.left() + rc.width() * 0.125, rc.bottom()));
        points.append(QPointF(rc.left() + rc.width() * 0.625, rc.top()));
        points.append(QPointF(rc.left() + rc.width() * 0.625, rc.bottom()));
        points.append(QPointF(rc.right(), rc.bottom() - rc.height() * 0.75));
    }
    return points;
}

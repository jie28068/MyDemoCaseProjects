#include "StepSourceGraphicsObject.h"
#include "SelftDrawSvgGraphicsObject.h"

StepSourceGraphicsObject::StepSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                   QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent), m_selfSvg(nullptr)
{
}

StepSourceGraphicsObject::~StepSourceGraphicsObject() { }

void StepSourceGraphicsObject::createInternalGraphics() { 

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

void StepSourceGraphicsObject::selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_selfSvg)
        return;
    painter->save();
    auto rc = m_selfSvg->boundingRect();
    double difference = 0.0;
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    QPolygonF points;
    if(getSourceProxy()) {
        difference = getSourceProxy()->getSourceProperty().getStepDifference().toDouble();
    }
    if(difference >= 0){ //finalvale大于 initalvale 就从左下角画到右上角
        points.append( QPointF(rc.left() + rc.width() * 0.1, rc.bottom() - rc.height() * 0.1) );
        points.append( QPointF(rc.center().x(), rc.bottom() - rc.height() * 0.1) );
        points.append( QPointF(rc.center().x(), rc.top() + rc.height() * 0.1) );
        points.append( QPointF(rc.right() - rc.width() * 0.1, rc.top() + rc.height() * 0.1) );
    }else{
        points.append( QPointF(rc.left() + rc.width() * 0.1, rc.top() + rc.height() * 0.1) );
        points.append( QPointF(rc.center().x(), rc.top() + rc.height() * 0.1) );
        points.append( QPointF(rc.center().x(), rc.bottom() - rc.height() * 0.1) );
        points.append( QPointF(rc.right() - rc.width() * 0.1, rc.bottom() - rc.height() * 0.1) );
    }
    painter->drawPolyline(points);
    painter->restore();
}

void StepSourceGraphicsObject::updatePosition() 
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

}

void StepSourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) { }

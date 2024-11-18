#include "MutiportSwitchGraphicsObject.h"
#include "SelftDrawSvgGraphicsObject.h"

MutiportSwitchGraphicsObject::MutiportSwitchGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                   QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent),m_selfSvg(nullptr)
{
}

MutiportSwitchGraphicsObject::~MutiportSwitchGraphicsObject() 
{    
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    QString prototypeName = sourceProxy->prototypeName();
    SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();

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
}

void MutiportSwitchGraphicsObject::createInternalGraphics() 
{ 
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    QString prototypeName = sourceProxy->prototypeName();
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


void MutiportSwitchGraphicsObject::selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
    QPainterPath path;
    QRectF sourceRect = boundingRect();
    QPolygonF inputPos;
    QPointF outputPos;
    for each(auto port in getSourceProxy()->portList()){
        QPointF pos = QPointF(sourceRect.left() + port->pos().x() * sourceRect.width(),
                                 sourceRect.top() + port->pos().y() * sourceRect.height());
        if(port->type() == "input"){
            inputPos.append(pos);
        }else{
            outputPos = pos;
        }
    }
    QPointF differencePos;
    QPointF aligmentStartPos;
    QPointF endPos;
    QPointF aligmentEndtPos;
    QRectF rect;
    int gridSpace = getCanvasContext()->gridSpace();
    for(int i = 0; i < inputPos.size(); ++i){
        if(i ==0){
            aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(inputPos[i]), gridSpace);
            differencePos = aligmentStartPos - transform().map(inputPos[i]);
            endPos = QPointF(inputPos[i].x() + sourceRect.width() * 0.25, inputPos[i].y());
            aligmentEndtPos = transform().map(endPos) + differencePos;
            path.moveTo(aligmentStartPos);
            path.lineTo(transform().map(endPos) + differencePos);
            path.moveTo(transform().map(endPos - QPointF(0, 3)) + differencePos);
            path.lineTo(transform().map(endPos + QPointF(0, 3)) + differencePos);
            continue;
        }
        aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(inputPos[i]),gridSpace);
        differencePos = aligmentStartPos - transform().map(inputPos[i]);
        endPos = QPointF(inputPos[i].x() + sourceRect.width() * 0.35, inputPos[i].y());
        aligmentEndtPos = transform().map(endPos) + differencePos;
        rect = QRectF(endPos.x(), endPos.y()- 1.5, 4, 3);
        path.moveTo(aligmentStartPos);
        path.lineTo(transform().map(endPos) + differencePos);
        rect = transform().map(rect).boundingRect();
        rect.translate(differencePos);
        path.addRect(rect);
        if(i == 1){
            aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(outputPos),gridSpace);
            differencePos = aligmentStartPos - transform().map(outputPos);
            endPos = QPointF(outputPos.x() - sourceRect.width() * 0.15, outputPos.y());
            aligmentEndtPos = transform().map(endPos) + differencePos;
            path.moveTo(aligmentStartPos);
            path.lineTo(transform().map(endPos) + differencePos);
            path.lineTo(rect.center());
        }
    }
    painter->drawPath(path);
    painter->restore();
}

void MutiportSwitchGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{ 
}

#include "SwitchGraphicsObject.h"
#include "SelftDrawSvgGraphicsObject.h"

SwitchGraphicsObject::SwitchGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                   QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent), m_selfSvg(nullptr)
{
}

SwitchGraphicsObject::~SwitchGraphicsObject() 
{ 
}

void SwitchGraphicsObject::createInternalGraphics() 
{ 
    ControlModuleSourceGraphicsObject::createInternalGraphics();
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();

    m_selfSvg = new SelftDrawSvgGraphicsObject(getCanvasScene(),sourceProxy,this,proxyGraphics);
    
    updatePosition();
}

void SwitchGraphicsObject::selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
        //alignPos = dataPtr->sourceGraphics->transform().map(newPos);
        //alignPos = Utility::pointAlignmentToGrid(alignPos, gridSpace);
    }
    if(inputPos.size() == 3){
        QPointF differencePos;
        QPointF aligmentStartPos;
        QPointF endPos;
        QPointF aligmentEndtPos;
        QRectF rect;
        int gridSpace = getCanvasContext()->gridSpace();

        aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(inputPos[0]),gridSpace);
        differencePos = aligmentStartPos - transform().map(inputPos[0]);
        endPos = QPointF(inputPos[0].x() + sourceRect.width() * 0.35, inputPos[0].y());
        aligmentEndtPos = transform().map(endPos) + differencePos;
        rect = QRectF(endPos.x(), endPos.y()- 1.5, 4, 3);
        path.moveTo(aligmentStartPos);
        path.lineTo(transform().map(endPos) + differencePos);
        rect = transform().map(rect).boundingRect();
        rect.translate(differencePos);
        path.addRect(rect);


        aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(outputPos),gridSpace);
        differencePos = aligmentStartPos - transform().map(outputPos);
        endPos = QPointF(outputPos.x() - sourceRect.width() * 0.15, outputPos.y());
        aligmentEndtPos = transform().map(endPos) + differencePos;
        path.moveTo(aligmentStartPos);
        path.lineTo(transform().map(endPos) + differencePos);
        path.lineTo(rect.center());

        aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(inputPos[1]),gridSpace);
        differencePos = aligmentStartPos - transform().map(inputPos[1]);
        endPos = QPointF(inputPos[1].x() + sourceRect.width() * 0.15, inputPos[1].y());
        aligmentEndtPos = transform().map(endPos) + differencePos;
        path.moveTo(aligmentStartPos);
        path.lineTo(transform().map(endPos) + differencePos);
        path.moveTo(transform().map(endPos - QPointF(0,4)) + differencePos);
        path.lineTo(transform().map(endPos + QPointF(0,4)) + differencePos);

        aligmentStartPos = Utility::pointAlignmentToGrid(transform().map(inputPos[2]),gridSpace);
        differencePos = aligmentStartPos - transform().map(inputPos[2]);
        endPos = QPointF(inputPos[2].x() + sourceRect.width() * 0.35, inputPos[2].y());
        aligmentEndtPos = transform().map(endPos) + differencePos;
        rect = QRectF(endPos.x(), endPos.y()- 1.5, 4, 3);
        path.moveTo(aligmentStartPos);
        path.lineTo(transform().map(endPos) + differencePos);
        rect = transform().map(rect).boundingRect();
        rect.translate(differencePos);
        path.addRect(rect);
    }
    painter->drawPath(path);
    painter->restore();
}

void SwitchGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{ 
}

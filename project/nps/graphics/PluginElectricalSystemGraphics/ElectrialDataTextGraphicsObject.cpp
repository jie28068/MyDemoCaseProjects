#include "ElectrialDataTextGraphicsObject.h"

ElectrialDataTextGraphicsObject::ElectrialDataTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject<QGraphicsTextItem>(canvasScene, parent), dataType(ElectrialDataTextGraphicsObject::InvalidData)
{
    // textItem->setAcceptHoverEvents(false);
    int layerFlag = kTextGraphicsLayer | kMoveableGraphicsLayer | kSelectedGraphicsLayer;
    setLayerFlag((GraphicsLayer::GraphicsLayerFlag)layerFlag);
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setType(kDataTextGraphics);
    QFont font = textItem->font();
    font.setPointSize(4);
    textItem->setFont(font);
    // setLayerFlag(GraphicsLayer::kCommentGraphicsLayer);
}

ElectrialDataTextGraphicsObject::~ElectrialDataTextGraphicsObject() { }

void ElectrialDataTextGraphicsObject::setElectrialDataType(ElectrialDataType type)
{
    dataType = type;
}

ElectrialDataTextGraphicsObject::ElectrialDataType ElectrialDataTextGraphicsObject::getElectrialDataType()
{
    return dataType;
}

void ElectrialDataTextGraphicsObject::updatePosition()
{
    // TransformProxyGraphicsObject *transGraphics = getTransformProxyGraphicsObject();
    // if(!transGraphics) return;
    // 母线潮流计算结果 放在母线左下方
    if (dataType == BusBarLoadFlowData) {
        TransformProxyGraphicsObject *transGraphics = getTransformProxyGraphicsObject();
        if (transGraphics) {
            QRectF rect = transGraphics->getSourceNameBounding();
            QPointF defaultPos(rect.left() + 2, rect.bottom() + 2);
            int angle = transGraphics->getAngle();
            if (angle == 0 || angle == 180) {
                defaultPos = QPointF(rect.left() + 2, rect.bottom() + 2);
            } else if (angle == 90 || angle == 270) {
                defaultPos = QPointF(rect.right() + 2, rect.top());
            }
            QPointF shiftPos(0, 0);
            // 看有没有存默认值
            /*             if
               (transGraphics->getSourceProxy()->getDataPointMap().contains(transGraphics->getSourceProxy()->name())) {
                            shiftPos = transGraphics->getSourceProxy()
                                               ->getDataPointMap()[transGraphics->getSourceProxy()->name()]
                                               .toPointF();
                        } else {
                            transGraphics->getSourceProxy()->setDataPointMap(transGraphics->getSourceProxy()->name(),
               shiftPos);
                        } */
            setPos(defaultPos.x() + shiftPos.x(), defaultPos.y() + shiftPos.y());
        }
    } else if (dataType == ComponentLoadFlowData) { // 电气元件潮流计算结果 放在母线端口朝向的方向
        PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(parentItem());
        if (portGraphics) {
            QRectF rect = portGraphics->boundingRect();
            QPointF centerPos = rect.center();
            QRectF boundRect = boundingRect();
            Qt::AnchorPoint anchor = portGraphics->getAnchorPoint();
            QPointF defaultPos;
            switch (anchor) {
            case Qt::AnchorTop:
                defaultPos = QPointF(centerPos.x() - boundRect.width() / 2, rect.top() - 5 - boundRect.height());
                break;
            case Qt::AnchorLeft:
                defaultPos = QPointF(rect.left() - boundRect.width(), centerPos.y() - boundRect.height() / 2);
                break;
            case Qt::AnchorBottom:
                defaultPos = QPointF(centerPos.x() - boundingRect().width() / 2, rect.bottom() + 5);
                break;
            case Qt::AnchorRight:
                defaultPos = QPointF(rect.right() + 5, centerPos.y() - boundRect.height() / 2);
                break;
            }
            QPointF shiftPos(0, 0);
            // 看有没有存默认值
            /*             if (portGraphics->getSourceGraphics()->getSourceProxy()->getDataPointMap().contains(
                                    portGraphics->getPortContext()->uuid())) {
                            shiftPos = portGraphics->getSourceGraphics()
                                               ->getSourceProxy()
                                               ->getDataPointMap()[portGraphics->getPortContext()->uuid()]
                                               .toPointF();
                        } else {
                            portGraphics->getSourceGraphics()->getSourceProxy()->setDataPointMap(
                                    portGraphics->getPortContext()->uuid(), shiftPos);
                        } */
            setPos(defaultPos.x() + shiftPos.x(), defaultPos.y() + shiftPos.y());
        }
    }
    // GraphicsLayer::updatePosition();
}

void ElectrialDataTextGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    getCanvasScene()->clearSelection();
    // topLevelItem()->setZValue(100);
    // this->stackBefore(topLevelItem());
    // this->setZValue(100);
    TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(topLevelItem());
    if (proxyGraphics) {
        proxyGraphics->setSelected(false);
    }
    clickPos = event->scenePos();
    TextGraphicsObject<QGraphicsTextItem>::mousePressEvent(event);
    event->accept();
}

void ElectrialDataTextGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    TextGraphicsObject<QGraphicsTextItem>::mouseMoveEvent(event);
}

void ElectrialDataTextGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // topLevelItem()->setZValue(0);
    // QPointF point = event->pos();
    if (event->scenePos() != clickPos) {
        QPointF shift = event->scenePos() - clickPos;
        if (dataType == BusBarLoadFlowData) {
            TransformProxyGraphicsObject *transGraphics = getTransformProxyGraphicsObject();
            if (!transGraphics)
                return;
            // 看有没有存默认值
            QPointF lastShift = transGraphics->getSourceProxy()
                                        ->getDataPointMap()[transGraphics->getSourceProxy()->name()]
                                        .toPointF();
            transGraphics->getSourceProxy()->setDataPointMap(transGraphics->getSourceProxy()->name(),
                                                             lastShift + shift);
        } else if (dataType == ComponentLoadFlowData) { // 电气元件潮流计算结果 放在母线端口朝向的方向
            PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(parentItem());
            if (!portGraphics)
                return;
            QPointF lastShift = portGraphics->getSourceGraphics()
                                        ->getSourceProxy()
                                        ->getDataPointMap()[portGraphics->getPortContext()->uuid()]
                                        .toPointF();
            portGraphics->getSourceGraphics()->getSourceProxy()->setDataPointMap(portGraphics->getPortContext()->uuid(),
                                                                                 lastShift + shift);
        }
    }
    clickPos = QPointF(0, 0);
    TextGraphicsObject<QGraphicsTextItem>::mouseReleaseEvent(event);
}

void ElectrialDataTextGraphicsObject::setShowData(QList<showDataStruct> &dataList, SourceGraphicsObject *source)
{
    int nameCompleteSize = 1;
    int valueCompleteSize = 5;
    // 遍历一次数据 获取对齐参数
    for (int i = 0; i < dataList.size(); ++i) {
        if (dataList[i].key.size() > nameCompleteSize) {
            nameCompleteSize = dataList[i].key.size();
        }
        double value = dataList[i].value.toDouble();
        dataList[i].value = QString::number(value, 'f', 3).arg("0", -1, 'f');
        if (dataList[i].value.size() > valueCompleteSize) {
            valueCompleteSize = dataList[i].value.size();
        }
    }

    QString showString;
    QString tipString;
    tipString.append(source->getSourceProxy()->name());
    tipString.append("\n");
    for (auto iter = dataList.begin(); iter != dataList.end();) {
        showString.append(iter->key.rightJustified(nameCompleteSize, ' '));
        showString.append(":");
        showString.append(iter->value.rightJustified(valueCompleteSize, ' '));
        tipString.append(iter->name);
        tipString.append(" = ");
        tipString.append(iter->value);
        tipString.append(iter->unit);

        if (++iter != dataList.end()) {
            showString.append("\n");
            tipString.append("\n");
        }
    }
    this->userShow(true);
    this->setPlainText(showString);
    this->setToolTip(tipString);
    this->updatePosition();
}

QRectF ElectrialDataTextGraphicsObject::boundingRect() const
{
    QRectF rect(0, 0, 5, 5);
    QRectF rcText = textItem->boundingRect();
    if (rcText.width() > rect.width()) {
        rect.setWidth(rcText.width());
    }
    if (rcText.height() > rect.height()) {
        rect.setHeight(rcText.height());
    }
    int adjustSize = 0;
    return rect.adjusted(-adjustSize, -adjustSize, adjustSize, adjustSize);
}

void ElectrialDataTextGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    GraphicsLayer::paint(painter, option, widget);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);
    QPen pen;
    painter->setBrush(QBrush("white"));
    // pen.setWidthF(0.2);
    pen.setColor("#000000");
    if (dataType == BusBarLoadFlowData) {
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
    } else if (dataType == ComponentLoadFlowData) {
        pen.setColor("#808080");
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
    }
    painter->drawRect(boundingRect());
    painter->restore();
}
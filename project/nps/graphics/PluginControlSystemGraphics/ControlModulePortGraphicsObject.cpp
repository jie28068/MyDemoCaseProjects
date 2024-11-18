#include "ControlModulePortGraphicsObject.h"

#include <QPainter>
#include <QPen>

class ControlModulePortGraphicsObjectPrivate
{
public:
    PortNameTextGraphicsObject *portNameGraphics;

    TransformProxyGraphicsObject *transfObj;

    ControlDataTextGraphicsObject *dataTextGraphics;

    Qt::AnchorPoint anchor;

    qreal width;
    qreal height;

    QPointF linkPos;

    QPointF centerPos;
};

ControlModulePortGraphicsObject::ControlModulePortGraphicsObject(ICanvasScene *canvasScene,
                                                                 SourceGraphicsObject *sourceGraphics,
                                                                 QSharedPointer<PortContext> context,
                                                                 QGraphicsItem *parent)
    : PortGraphicsObject(canvasScene, sourceGraphics, context, parent)
{
    setToolTip(context->name());
    dataPtr.reset(new ControlModulePortGraphicsObjectPrivate());
    dataPtr->portNameGraphics = nullptr;
    dataPtr->transfObj = nullptr;
    dataPtr->dataTextGraphics = nullptr;

    dataPtr->width = context->size().width();
    dataPtr->height = context->size().height();

    connect(context.data(), SIGNAL(onPortHideOrShow(bool)), this, SLOT(onPortHideOrShowChange(bool)));
    // 绑定端口变更信号槽
    connect(context.data(), &PortContext::portNameChanged, this, &ControlModulePortGraphicsObject::onPortNameChanged);
}

ControlModulePortGraphicsObject::~ControlModulePortGraphicsObject() { }

QPointF ControlModulePortGraphicsObject::getLinkPos()
{
    if (dataPtr->linkPos.isNull()) {
        QRectF rect = boundingRect();
        QRectF portSceneRect = mapToScene(rect).boundingRect();
        Qt::AnchorPoint anchor = getAnchorPoint();
        QPointF centerPos = getCenterPos();
        // if (getPortContext()->type() == PortOutputType) {
        //     switch (anchor) {
        //     case Qt::AnchorTop:
        //         centerPos.setY(centerPos.y() - dataPtr->height);
        //         break;
        //     case Qt::AnchorRight:
        //         centerPos.setX(centerPos.x() + dataPtr->width);
        //         break;
        //     case Qt::AnchorBottom:
        //         centerPos.setY(centerPos.y() + dataPtr->height);
        //         break;
        //     case Qt::AnchorLeft:
        //         centerPos.setX(centerPos.x() - dataPtr->width);
        //         break;
        //     }
        // }

        // if (getPortContext()->type() == PortInputType) {
        int offset = 0;
        switch (anchor) {
        case Qt::AnchorTop:
            centerPos.setY(centerPos.y() - offset);
            break;
        case Qt::AnchorRight:
            centerPos.setX(centerPos.x() + offset);
            break;
        case Qt::AnchorBottom:
            centerPos.setY(centerPos.y() + offset);
            break;
        case Qt::AnchorLeft:
            centerPos.setX(centerPos.x() - offset);
            break;
        }
        //}
        dataPtr->linkPos = centerPos;
    }

    return dataPtr->linkPos;
}

QPointF ControlModulePortGraphicsObject::getExternalPos()
{
    if (getPortContext().isNull()) {
        return getLinkPos();
    }
    QRectF portSceneRect = mapToScene(boundingRect()).boundingRect();
    QPointF centerPos = getCenterPos();
    int gridSpace = 10;
    ICanvasScene *canvasScene = dynamic_cast<ICanvasScene *>(scene());
    if (canvasScene) {
        gridSpace = canvasScene->getGridSpace();
    }
    Qt::AnchorPoint anchor = getAnchorPoint();
    int offset = 2 * gridSpace;
    switch (anchor) {
    case Qt::AnchorTop:
        centerPos.setY(centerPos.y() - offset);
        break;
    case Qt::AnchorRight:
        centerPos.setX(centerPos.x() + offset);
        break;
    case Qt::AnchorBottom:
        centerPos.setY(centerPos.y() + offset);
        break;
    case Qt::AnchorLeft:
        centerPos.setX(centerPos.x() - offset);
        break;
    }
    return Utility::pointAlignmentToGrid(centerPos, gridSpace);
}

QPointF ControlModulePortGraphicsObject::getCenterPos()
{
    if (dataPtr->centerPos.isNull()) {
        QRectF rc = QRectF(-dataPtr->width / 2, -dataPtr->height / 2, dataPtr->width, dataPtr->height);
        QRectF portSceneRect = mapToScene(rc).boundingRect();
        QPointF centerPos = portSceneRect.center();
        dataPtr->centerPos = centerPos;
    }
    return dataPtr->centerPos;
}

void ControlModulePortGraphicsObject::createInternalGraphics()
{
    dataPtr->portNameGraphics = new PortNameTextGraphicsObject(getCanvasScene(), this);

    // 控制模块显示端口名称时，使用端口中文名，如果中文名为空，则显示变量名称
    if (getPortContext()->displayName() != "") {
        dataPtr->portNameGraphics->setPlainText(getPortContext()->displayName());
    } else {
        dataPtr->portNameGraphics->setPlainText(getPortContext()->name());
    }
    dataPtr->portNameGraphics->updatePosition();
    dataPtr->transfObj = getTransformProxyGraphicsObject();
    if (getPortContext()->type() == PortOutputType) {
        dataPtr->dataTextGraphics = new ControlDataTextGraphicsObject(getCanvasScene(), this);
        dataPtr->dataTextGraphics->userShow(false);
    }

    isShowPortName();
}

void ControlModulePortGraphicsObject::linkChanged()
{
    if (getLinkedConnectorWireList().size() > 0) {
        if (dataPtr->portNameGraphics) {
            dataPtr->portNameGraphics->userShow(false);
        }
    } else {
        if (dataPtr->portNameGraphics) {
            dataPtr->portNameGraphics->userShow(true);
        }
    }

    isShowPortName();
}

bool ControlModulePortGraphicsObject::canMove()
{
    auto sourceGraphics = getSourceGraphics();
    if (sourceGraphics) {
        auto sourceProxy = sourceGraphics->getSourceProxy();
        if (sourceProxy) {
            if (sourceProxy->prototypeName() == "Gain") {
                // 增益模块的输出端口不能移动
                if (isOutputPort()) {
                    return false;
                }
            } else if (sourceProxy->prototypeName() == "Goto" || sourceProxy->prototypeName() == "From"
                       || sourceProxy->prototypeName() == "time" || sourceProxy->prototypeName() == "In"
                       || sourceProxy->prototypeName() == "Out") {
                return false;
            }
        }
    }
    return true;
}

QRectF ControlModulePortGraphicsObject::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath ControlModulePortGraphicsObject::shape() const
{
    QRectF rc = QRectF(-dataPtr->width / 2, -dataPtr->height / 2, dataPtr->width, dataPtr->height);
    QPointF centerPos = rc.center();
    Qt::AnchorPoint anchor = dataPtr->anchor;
    switch (anchor) {
    case Qt::AnchorLeft:
        centerPos.setX(centerPos.x() - rc.width() / 2);
        break;
    case Qt::AnchorRight:
        centerPos.setX(centerPos.x() + rc.width() / 2);
        break;
    case Qt::AnchorBottom:
        centerPos.setY(centerPos.y() + rc.height() / 2);
        break;
    case Qt::AnchorTop:
        centerPos.setY(centerPos.y() - rc.height() / 2);
        break;
    default:
        break;
    }
    rc.moveCenter(centerPos);
    QPainterPath path;
    path.addRect(rc);
    return path;
}

void ControlModulePortGraphicsObject::updatePosition()
{
    PortGraphicsObject::updatePosition();
    dataPtr->anchor = getAnchorPoint();
    dataPtr->linkPos = QPointF();
    dataPtr->centerPos = QPointF();
}

QRectF ControlModulePortGraphicsObject::portNameRect()
{
    if (!dataPtr->portNameGraphics) {
        return QRectF(0, 0, 0, 0);
    }
    if (!getPortContext()->isShowName()) {
        return QRectF(0, 0, 0, 0);
    }
    QString nameStr = dataPtr->portNameGraphics->getPlainText();
    QFont font = dataPtr->portNameGraphics->font();
    QFontMetricsF fm(font);
    return fm.boundingRect(nameStr);
}

void ControlModulePortGraphicsObject::setDataTextShow(const QString &text)
{
    if (dataPtr->dataTextGraphics) {
        if (text == QString()) {
            this->setZValue(0);
            dataPtr->dataTextGraphics->setZValue(0);
            dataPtr->dataTextGraphics->userShow(false);
            dataPtr->dataTextGraphics->setPlainText("");
        } else {
            getTransformProxyGraphicsObject()->setZValue(100);
            this->setZValue(100);
            dataPtr->dataTextGraphics->setZValue(100);
            dataPtr->dataTextGraphics->userShow(true);
            dataPtr->dataTextGraphics->setPlainText(text);
            updatePosition();
        }
    }
}

void ControlModulePortGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    Q_UNUSED(widget);
    Q_UNUSED(option);
    QSharedPointer<PortContext> ctx = getPortContext();
    if (ctx.isNull()) {
        return;
    }
    auto links = getLinkedConnectorWireList();
    if (!links.isEmpty()) {
        // 端口连接之后就不绘制端口了
        return;
    }

    QRectF rect = boundingRect();
    QVector<QPointF> points = calcPoints();
    if (points.size() != 3)
        return;
    painter->save();

    QPen pen;
    pen.setWidth(1);
    pen.setColor(GKD::WIRE_NORMAL_COLOR);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    ICanvasScene *canvasScene = getCanvasScene();
    if (canvasScene && (isCanLinkWithClickedPort() || canvasScene->getMouseClickedPort() == this)) {
        pen.setColor(0x2E9FE6);
    }

    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(points);

    painter->restore();
}

void ControlModulePortGraphicsObject::onPortNameChanged(QString name)
{
    dataPtr->portNameGraphics->setPlainText(getPortContext()->name());
}

QVector<QPointF> ControlModulePortGraphicsObject::calcPoints()
{
    QVector<QPointF> points;

    QRectF rect = boundingRect();
    TransformProxyGraphicsObject *transfObj = getTransformProxyGraphicsObject();
    if (!transfObj)
        return points;
    int angle = transfObj->getAngle();
    rect = rect.adjusted(0.5, 0.5, -0.5, -0.5);
    switch (angle) {
    case 0:
        points.append(QPointF(rect.x(), rect.y()));
        points.append(QPointF(rect.x() + rect.width(), rect.y() + rect.height() / 2));
        points.append(QPointF(rect.x(), rect.y() + rect.height()));
        break;
    case 90:
        points.append(QPointF(rect.x(), rect.y()));
        points.append(QPointF(rect.x() + rect.width() / 2, rect.y() + rect.height()));
        points.append(QPointF(rect.x() + rect.width(), rect.y()));
        break;
    case 180:
        points.append(QPointF(rect.x() + rect.width(), rect.y()));
        points.append(QPointF(rect.x(), rect.y() + rect.height() / 2));
        points.append(QPointF(rect.x() + rect.width(), rect.y() + rect.height()));
        break;
    case 270:
        points.append(QPointF(rect.x(), rect.y() + rect.height()));
        points.append(QPointF(rect.x() + rect.width() / 2, rect.y()));
        points.append(QPointF(rect.x() + rect.width(), rect.y() + rect.height()));
        break;
    default:
        break;
    }

    return points;
}

void ControlModulePortGraphicsObject::isShowPortName()
{
    if (!getSourceGraphics() || !getSourceGraphics()->getSourceProxy() || !dataPtr->portNameGraphics)
        return;

    // 控制模块端口只有一个时，不显示端口名称
    dataPtr->portNameGraphics->userShow(getPortContext()->isShowName());
}

void ControlModulePortGraphicsObject::onPortHideOrShowChange(bool value)
{
    dataPtr->portNameGraphics->userShow(value);
}

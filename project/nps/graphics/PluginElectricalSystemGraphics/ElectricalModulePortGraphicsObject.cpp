#include "ElectricalModulePortGraphicsObject.h"
#include "ElectPortNameGraphics.h"
#include "ElectricalModuleSourceGraphicsObject.h"

#include <QPainter>
#include <qmath.h>

ElectricalModulePortGraphicsObject::ElectricalModulePortGraphicsObject(ICanvasScene *canvasScene,
                                                                       SourceGraphicsObject *sourceGraphics,
                                                                       QSharedPointer<PortContext> context,
                                                                       QGraphicsItem *parent)
    : PortGraphicsObject(canvasScene, sourceGraphics, context, parent), dataTextGraphicsObject(nullptr)
{
    tempAnchor = Qt::AnchorHorizontalCenter;
    dotBusPortAnchor = Qt::AnchorHorizontalCenter;
    portNameGraphics = nullptr;
    PSourceProxy sourceProxy = sourceGraphics->getSourceProxy();
    if (sourceProxy && (sourceProxy->prototypeName() == "Busbar" || sourceProxy->prototypeName() == "DotBusbar")
        && !dataTextGraphicsObject) {
        dataTextGraphicsObject = new ElectrialDataTextGraphicsObject(canvasScene, this);
        dataTextGraphicsObject->setElectrialDataType(ElectrialDataTextGraphicsObject::ComponentLoadFlowData);
        dataTextGraphicsObject->userShow(false);
        context->setIsShowName(false);
        // dataTextGraphicsObject->updatePosition();
        connect(canvasScene->getCanvasContext().data(), SIGNAL(canvasShowPowerFlowFlagChanged(bool)), this,
                SLOT(onReceiveShowPowerFlowChange(bool)));
    }

    isElectricalCombineType = false;
    auto businessHook = getBusinessHook();
    if (businessHook && sourceProxy) {
        isElectricalCombineType = businessHook->isElecCombineModel(sourceProxy->uuid());
    }
}

ElectricalModulePortGraphicsObject::~ElectricalModulePortGraphicsObject() { }

Qt::AnchorPoint ElectricalModulePortGraphicsObject::getAnchorPoint()
{
    if (tempAnchor != Qt::AnchorHorizontalCenter) {
        return tempAnchor;
    }
    auto sourceGraphics = getSourceGraphics();
    if (sourceGraphics && sourceGraphics->getSourceProxy()) {
        PSourceProxy sourceProxy = sourceGraphics->getSourceProxy();
        if (!getLinkedConnectorWireList().isEmpty()) {
            TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
            PortGraphicsObject *otherPort = getLinkedConnectorWireList()[0]->getAnotherPort(this);
            if (sourceProxy->prototypeName() == "Busbar") {
                if (nullptr != otherPort && proxyGraphics) {
                    int angle = proxyGraphics->getAngle();
                    switch (angle) {
                    case 0:
                    case 180:
                        if (otherPort->getCenterPos().y() > getCenterPos().y()) {
                            return Qt::AnchorBottom;
                        } else {
                            return Qt::AnchorTop;
                        }
                        break;
                    case 90:
                    case 270:
                        if (otherPort->getCenterPos().x() > getCenterPos().x()) {
                            return Qt::AnchorRight;
                        } else {
                            return Qt::AnchorLeft;
                        }
                        break;
                    default: {
                    }
                    }
                }
            } else if (sourceProxy->prototypeName() == "DotBusbar") {
                // 重新计算母线端口锚点方向
                if (dotBusPortAnchor == Qt::AnchorHorizontalCenter) {
                    auto algorithm = createConnectorWireAlgorithm(ManhattaAlgorithmName);
                    int maxLength = 0xffffff;
                    int minRightAngleCounts = 0xff;
                    for (int i = Qt::AnchorLeft; i <= Qt::AnchorBottom; i++) {
                        Qt::AnchorPoint movingAnchor = (Qt::AnchorPoint)i;
                        QPointF linkPos = getLinkPos();
                        QPointF linkExternalPos = linkPos;
                        if (movingAnchor == Qt::AnchorBottom) {
                            linkExternalPos.setY(linkExternalPos.y() + 30);
                        } else if (movingAnchor == Qt::AnchorTop) {
                            linkExternalPos.setY(linkExternalPos.y() - 30);
                        } else if (movingAnchor == Qt::AnchorRight) {
                            linkExternalPos.setX(linkExternalPos.x() + 30);
                        } else if (movingAnchor == Qt::AnchorLeft) {
                            linkExternalPos.setX(linkExternalPos.x() - 30);
                        }

                        // 计算点状母线不同锚点方向的连接线坐标点
                        QPolygonF points = algorithm->getAlgorithmPath(
                                otherPort->getLinkPos(), otherPort->getExternalPos(), linkPos, linkExternalPos);
                        // 计算连接线长度，选取长度最短的一个锚点方向的连接线，作为点状母线的锚点方向
                        qDebug() << "anchor:" << movingAnchor << " points:" << points;
                        int length = Utility::getPolygonLineLength(points.toPolygon());
                        if (length < maxLength) {
                            dotBusPortAnchor = movingAnchor;
                            maxLength = length;
                            int angleCounts = Utility::getRightAngleCounts(points.toPolygon());
                            minRightAngleCounts = angleCounts < minRightAngleCounts ? angleCounts : minRightAngleCounts;
                        } else if (length == maxLength) {
                            // 相等的情况下，判断直角个数
                            int angleCounts = Utility::getRightAngleCounts(points.toPolygon());
                            if (angleCounts < minRightAngleCounts) {
                                dotBusPortAnchor = movingAnchor;
                                maxLength = length;
                                minRightAngleCounts = angleCounts;
                            }
                        }
                    }
                }
                return dotBusPortAnchor;
            }
        }
    }
    return PortGraphicsObject::getAnchorPoint();
}

QPointF ElectricalModulePortGraphicsObject::getLinkPos()
{
    return PortGraphicsObject::getLinkPos();
    auto sourceGraphics = getSourceGraphics();
    PSourceProxy sourceProxy = sourceGraphics->getSourceProxy();
    if (sourceProxy->prototypeName() == "Busbar" || sourceProxy->prototypeName() == "DotBusbar") {
        // 母线端口连接点坐标偏移到端口边缘
        QRectF rect = boundingRect();
        QRectF portSceneRect = mapToScene(rect).boundingRect();
        Qt::AnchorPoint anchor = getAnchorPoint();
        QPointF centerPos = portSceneRect.center();
        switch (anchor) {
        case Qt::AnchorTop:
            centerPos.setY(centerPos.y() - portSceneRect.height() / 2.f);
            break;
        case Qt::AnchorRight:
            centerPos.setX(centerPos.x() + portSceneRect.width() / 2.f);
            break;
        case Qt::AnchorBottom:
            centerPos.setY(centerPos.y() + portSceneRect.height() / 2.f);
            break;
        case Qt::AnchorLeft:
            centerPos.setX(centerPos.x() - portSceneRect.width() / 2.f);
            break;
        }
        return centerPos;
    }

    return PortGraphicsObject::getLinkPos();
}

QPointF ElectricalModulePortGraphicsObject::getExternalPos()
{
    QRectF portSceneRect = mapToScene(boundingRect()).boundingRect();
    QPointF centerPos = getLinkPos();
    int gridSpace = 10;
    ICanvasScene *canvasScene = dynamic_cast<ICanvasScene *>(scene());
    if (canvasScene) {
        gridSpace = canvasScene->getGridSpace();
    }
    int counts = 1;
    auto sourceGraphics = getSourceGraphics();
    if (sourceGraphics && sourceGraphics->getSourceProxy()) {
        PSourceProxy sourceProxy = sourceGraphics->getSourceProxy();
        QString prototypeName = sourceProxy->prototypeName();
        if ((prototypeName == "Busbar" || prototypeName == "DotBusbar" || prototypeName == "GroundBus")
            && getLinkedConnectorWireList().size() > 0) {
            counts = 3;
        }
    }
    Qt::AnchorPoint anchor = getAnchorPoint();
    switch (anchor) {
    case Qt::AnchorTop:
        return QPointF(centerPos.rx(), centerPos.ry() - counts * gridSpace);
    case Qt::AnchorRight:
        return QPointF(centerPos.rx() + counts * gridSpace, centerPos.ry());
    case Qt::AnchorBottom:
        return QPointF(centerPos.rx(), centerPos.ry() + counts * gridSpace);
    case Qt::AnchorLeft:
        return QPointF(centerPos.rx() - counts * gridSpace, centerPos.ry());
    }
    return PortGraphicsObject::getExternalPos();
}

void ElectricalModulePortGraphicsObject::updatePosition()
{
    PortGraphicsObject::updatePosition();
    if (dataTextGraphicsObject) {
        dataTextGraphicsObject->updatePosition();
    }
}

void ElectricalModulePortGraphicsObject::createInternalGraphics()
{
    portNameGraphics = new ElectPortNameGraphics(getCanvasScene(), this);

    portNameGraphics->setPlainText(getPortContext()->displayName());
    portNameGraphics->updatePosition();
    isShowPortName();
}

QRectF ElectricalModulePortGraphicsObject::portNameRect()
{
    if (!portNameGraphics) {
        return QRectF(0, 0, 0, 0);
    }
    if (!getPortContext()->isShowName()) {
        return QRectF(0, 0, 0, 0);
    }
    QString nameStr = portNameGraphics->getPlainText();
    QFont font = portNameGraphics->font();
    QFontMetricsF fm(font);
    return fm.boundingRect(nameStr);
}

void ElectricalModulePortGraphicsObject::setTempAnchor(Qt::AnchorPoint anchor)
{
    tempAnchor = anchor;
}

void ElectricalModulePortGraphicsObject::resetPortAnchor()
{
    dotBusPortAnchor = Qt::AnchorHorizontalCenter;
}

void ElectricalModulePortGraphicsObject::linkChanged()
{
    isShowPortName();
}

void ElectricalModulePortGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                               QWidget *widget)
{
    auto sourceGraphics = getSourceGraphics();
    if (sourceGraphics) {
        PSourceProxy sourceProxy = sourceGraphics->getSourceProxy();
        QString prototypeName = sourceProxy->prototypeName();
        if (prototypeName == "GroundBus") {
            return;
        }
        if (prototypeName == "Busbar") {
            QList<ConnectorWireGraphicsObject *> wires = getLinkedConnectorWireList();
            // 母线端口颜色应该根据连接线所连接的元件状态确定
            if (!wires.isEmpty()) {
                bool isVisible = wires[0]->isVisible();
                if (isVisible) {
                    QColor color = sourceProxy->getStateColor();
                    PConnectorWireContext context = wires[0]->getConnectorWireContext();
                    if ((context->srcSource() && context->srcSource()->state() != "normal")
                        && (context->dstSource() && context->dstSource()->state() != "normal")) { }
                    if (!context->connected() || sourceProxy->state() != "normal") {
                        color = QColor(GKD::WIRE_DISABLE_COLOR);
                    }

                    painter->save();
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(color);
                    painter->drawEllipse(boundingRect());
                    painter->restore();
                }
            }
        } else {
            // 未连接的电气元件端口需要绘制
            if (getLinkedConnectorWireList().isEmpty()) {
                painter->save();
                painter->setPen(Qt::NoPen);
                QColor color = sourceProxy->getStateColor();
                painter->setBrush(color);
                QRectF rc = boundingRect();
                painter->drawEllipse(rc.adjusted(1.5, 1.5, -1.5, -1.5));
                painter->restore();
            }
        }
    }
}

void ElectricalModulePortGraphicsObject::autoLayout()
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        PSourceProxy sourceProxy = proxyGraphics->getSourceProxy();
        // 如果端口所属的资源时电气连线时
        if (sourceProxy && sourceProxy->prototypeName() == "Line") {
            bool isAllConnected = true;
            // 母线端口列表
            QList<PortGraphicsObject *> busBarPortList;
            // 检测所有的元件端口是否都已经连接
            QList<PortGraphicsObject *> portGraphicsList = proxyGraphics->getPortGraphicsObjectList();
            for each (PortGraphicsObject *portGraphics in portGraphicsList) {
                if (!portGraphics) {
                    isAllConnected = false;
                    break;
                }
                if (portGraphics->getLinkedConnectorWireList().isEmpty()) {
                    isAllConnected = false;
                    break;
                } else {
                    PortGraphicsObject *busBarPort =
                            portGraphics->getLinkedConnectorWireList().last()->getAnotherPort(portGraphics);
                    if (nullptr != busBarPort) {
                        // 把当前端口连接的母线端口加入到母线端口列表
                        busBarPortList.append(busBarPort);
                    }
                }
            }
            // 当前端口都已经连接了母线,而且获取的母线端口至少有2个
            if (isAllConnected && busBarPortList.size() >= 2) {
                // 计算2个母线端口之间的连线
                auto algorithm = createConnectorWireAlgorithm(ManhattaAlgorithmName);

                PortGraphicsObject *busPort1 = busBarPortList[0];
                PortGraphicsObject *busPort2 = busBarPortList[1];
                if (!busPort1 || !busPort2) {
                    return;
                }

                // 计算两个母线端口连接的时候，锚点的方向，保存在tempAnchor里面
                TransformProxyGraphicsObject *busPort1Proxy = busPort1->getTransformProxyGraphicsObject();
                TransformProxyGraphicsObject *busPort2Porxy = busPort2->getTransformProxyGraphicsObject();
                if (!busPort1Proxy || !busPort2Porxy) {
                    return;
                }
                QPointF portPos1 = busPort1->getLinkPos();
                QPointF portPos2 = busPort2->getLinkPos();
                if ((busPort1Proxy->getAngle() % 180) == (busPort2Porxy->getAngle() % 180)) {
                    int distance = 0;
                    // 说明两个母线要么都是水平或者垂直，方向是一致的
                    if ((busPort1Proxy->getAngle() % 180) == 0) {
                        // 水平
                        if (portPos1.y() > portPos2.y()) {
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort1)->setTempAnchor(Qt::AnchorTop);
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort2)->setTempAnchor(
                                    Qt::AnchorBottom);

                        } else {
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort1)->setTempAnchor(
                                    Qt::AnchorBottom);
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort2)->setTempAnchor(Qt::AnchorTop);
                        }

                    } else {
                        // 垂直
                        if (portPos1.x() > portPos2.x()) {
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort1)->setTempAnchor(Qt::AnchorLeft);
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort2)->setTempAnchor(
                                    Qt::AnchorRight);

                        } else {
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort1)->setTempAnchor(
                                    Qt::AnchorRight);
                            dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort2)->setTempAnchor(Qt::AnchorLeft);
                        }
                    }
                    distance =
                            distance < abs(portPos1.y() - portPos2.y()) ? abs(portPos1.y() - portPos2.y()) : distance;
                    distance =
                            distance < abs(portPos1.x() - portPos2.x()) ? abs(portPos1.x() - portPos2.x()) : distance;
                    if (distance < 100) {
                        return;
                    }

                } else {
                    // 说明母线方向不一致，一个垂直 一个水平
                    PortGraphicsObject *verticalBusPort = (busPort1Proxy->getAngle() % 180) != 0 ? busPort1 : busPort2;
                    PortGraphicsObject *horizontalBusPort =
                            (busPort1Proxy->getAngle() % 180) == 0 ? busPort1 : busPort2;
                    QPointF vertiaclPortPos = verticalBusPort->getLinkPos();
                    QPointF horizontalPortPos = horizontalBusPort->getLinkPos();
                    if (vertiaclPortPos.x() > horizontalPortPos.x()) {
                        dynamic_cast<ElectricalModulePortGraphicsObject *>(verticalBusPort)
                                ->setTempAnchor(Qt::AnchorLeft);
                    } else {
                        dynamic_cast<ElectricalModulePortGraphicsObject *>(verticalBusPort)
                                ->setTempAnchor(Qt::AnchorRight);
                    }

                    if (vertiaclPortPos.y() < horizontalPortPos.y()) {
                        dynamic_cast<ElectricalModulePortGraphicsObject *>(horizontalBusPort)
                                ->setTempAnchor(Qt::AnchorTop);
                    } else {
                        dynamic_cast<ElectricalModulePortGraphicsObject *>(horizontalBusPort)
                                ->setTempAnchor(Qt::AnchorBottom);
                    }
                }

                QPolygonF points = algorithm->getAlgorithmPath(
                        busBarPortList[0]->getLinkPos(), busBarPortList[0]->getExternalPos(),
                        busBarPortList[1]->getLinkPos(), busBarPortList[1]->getExternalPos());
                dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort1)->setTempAnchor(Qt::AnchorHorizontalCenter);
                dynamic_cast<ElectricalModulePortGraphicsObject *>(busPort2)->setTempAnchor(Qt::AnchorHorizontalCenter);
                // 如果连接点的数量小于4个返回
                if (points.size() < 4) {
                    return;
                }
                // 因为manhatta路径算法相邻的两个点必然是直线,计算除端口和延伸点之外的线段的长度
                int maxLineLength = 0;
                int maxIndex = 0;
                // 把最长的线段当做有向线段，计算弧度  maxRadians保存最长线段的弧度
                qreal maxRadians;
                bool isVertial = true;
                QPointF centerPos;
                for (int index = 1; index < points.size() - 2; index++) {
                    QPointF p1 = points[index];
                    QPointF p2 = points[index + 1];
                    qreal length = (p1 - p2).manhattanLength();
                    if (length > maxLineLength) {
                        maxIndex = index;
                        maxLineLength = length;
                        isVertial = p1.y() != p2.y() ? true : false;
                        centerPos = (p1 + p2) / 2;
                        centerPos = Utility::pointAlignmentToGrid(centerPos, 10);
                        maxRadians = qAtan2((p1 - p2).x(), (p1 - p2).y());
                    }
                }
                QRectF sourceSceneRc = proxyGraphics->getSourceGraphicsObject()->getSourceSceneRect();
                QPointF sourceSceneCenter = sourceSceneRc.center();
                QList<QTransform> oldTransForm;
                QList<QTransform> newTransForm;
                oldTransForm.push_back(sourceProxy->scaleTransform());
                oldTransForm.push_back(sourceProxy->rotateTransform());
                oldTransForm.push_back(sourceProxy->translateTransform());
                oldTransForm.push_back(sourceProxy->xAxisFlipTransform());
                oldTransForm.push_back(sourceProxy->yAxisFlipTransform());
                // 中心点偏移量
                QPointF offset = centerPos - sourceSceneCenter;
                sourceProxy->setTranslateTransform(sourceProxy->translateTransform()
                                                   * QTransform::fromTranslate(offset.x(), offset.y()));
                proxyGraphics->applyTransformChanged();
                int angle = proxyGraphics->getAngle();
                if (isVertial) {
                    if (angle == 0 || angle == 180) {
                        proxyGraphics->rotateOnCenter(90);
                    }
                } else {
                    if (angle == 90 || angle == 270) {
                        proxyGraphics->rotateOnCenter(90);
                    }
                }
                offset = portGraphicsList[0]->getLinkPos() - portGraphicsList[1]->getLinkPos();
                // 计算旋转之后当前端口的连线的弧度
                qreal nowRadians = qAtan2(offset.x(), offset.y());
                // 如果元件端口连线的弧度和最长线段的弧度不一致，那么需要再旋转180度
                if (maxRadians != nowRadians) {
                    proxyGraphics->rotateOnCenter(180);
                }

                auto wireList0 = portGraphicsList[0]->getLinkedConnectorWireList();
                auto wireList1 = portGraphicsList[1]->getLinkedConnectorWireList();
                if (!wireList0.isEmpty() && !wireList1.isEmpty()) {
                    auto wire0 = wireList0[0];
                    auto wire1 = wireList1[0];
                    if (wire0->getPoints().size() > 5 && wire1->getPoints().size() > 5) {
                        proxyGraphics->rotateOnCenter(180);
                    }
                }
                newTransForm.push_back(sourceProxy->scaleTransform());
                newTransForm.push_back(sourceProxy->rotateTransform());
                newTransForm.push_back(sourceProxy->translateTransform());
                newTransForm.push_back(sourceProxy->xAxisFlipTransform());
                newTransForm.push_back(sourceProxy->yAxisFlipTransform());
                setTransFormCommand(oldTransForm, newTransForm);
            }
        }
    }
}

void ElectricalModulePortGraphicsObject::isShowPortName()
{
    if (!portNameGraphics)
        return;
    if (getLinkedConnectorWireList().size() > 0) {
        portNameGraphics->userShow(false);
        return;
    }
    portNameGraphics->userShow(true);
}

void ElectricalModulePortGraphicsObject::onReceiveShowPowerFlowChange(bool flag)
{
    if (!dataTextGraphicsObject)
        return;
    if (flag) {
        getTransformProxyGraphicsObject()->setZValue(100);
        this->setZValue(100);
        this->dataTextGraphicsObject->setZValue(100);
        if (getLinkedConnectorWireList().empty())
            return;
        // 获取端口所连接的电气模块
        PortGraphicsObject *otherPort = getLinkedConnectorWireList()[0]->getAnotherPort(this);
        SourceGraphicsObject *otherSource = otherPort->getSourceGraphics();
        BusinessHooksServer *businessHooks = getCanvasScene()->getCanvasContext()->getBusinessHooksServer();
        if (!businessHooks)
            return;
        QList<showDataStruct> dataList =
                dealDataList(businessHooks->getPowerFlowData(otherSource->getSourceProxy()), otherPort, otherSource);
        dataTextGraphicsObject->setShowData(dataList, otherSource);
    } else {
        if (getTransformProxyGraphicsObject()) {
            getTransformProxyGraphicsObject()->setZValue(0);
        }
        for each (auto wire in getLinkedConnectorWireList()) {
            wire->setArrowPoints(QPolygonF());
        }
        this->setZValue(0);
        this->dataTextGraphicsObject->setZValue(0);
        dataTextGraphicsObject->setPlainText("");
        dataTextGraphicsObject->setToolTip("");
        dataTextGraphicsObject->userShow(false);
    }
}

bool ElectricalModulePortGraphicsObject::canMove()
{
    if (isElectricalCombineType) {
        return true;
    }
    auto sourceProxy = getSourceProxy();
    if (sourceProxy && sourceProxy->prototypeName() == "Busbar") {
        return true;
    }
    return false;
}

QList<showDataStruct> ElectricalModulePortGraphicsObject::dealDataList(QList<showDataStruct> &originalDataList,
                                                                       PortGraphicsObject *otherPort,
                                                                       SourceGraphicsObject *otherSource)
{
    QList<showDataStruct> dataList;
    QString busbarUUid = getSourceGraphics()->getSourceProxy()->uuid();
    int portCount = otherSource->getSourceProxy()->portList().count();
    switch (portCount) {
    case 1:
        dataList = originalDataList;
        break;
    // 两个端口则分为
    case 2:
        for each (auto data in originalDataList) {
            QPointF point1 = otherPort->getExternalPos();
            QPointF point2;
            if ((data.key.contains("FBUS")
                 && (otherPort->getPortContext()->name() == "FBUS" || otherPort->getPortContext()->name() == "HFBUS"))
                || (data.key.contains("TBUS")
                    && (otherPort->getPortContext()->name() == "TBUS"
                        || otherPort->getPortContext()->name() == "LFBUS"))) {

                data.key = data.key[data.key.size() - 1];
                dataList.push_back(data);
            }
            if (data.key == "To" && busbarUUid == data.value) {
                Qt::AnchorPoint anchor = otherPort->getAnchorPoint();
                switch (anchor) {
                case Qt::AnchorTop:
                    point2 = QPointF(point1.x(), point1.y() + 2);
                    break;
                case Qt::AnchorLeft:
                    point2 = QPointF(point1.x() + 2, point1.y());
                    break;
                case Qt::AnchorBottom:
                    point2 = QPointF(point1.x(), point1.y() - 2);
                    break;
                case Qt::AnchorRight:
                    point2 = QPointF(point1.x() - 2, point1.y());
                    break;
                }
                for each (auto wire in otherPort->getLinkedConnectorWireList()) {
                    wire->setArrowPoints(Utility::getArrorwPoint(point2, point1, 25.f / 57.3, 8));
                }
            } else if (data.key == "From" && busbarUUid == data.value) {
                Qt::AnchorPoint anchor = otherPort->getAnchorPoint();
                switch (anchor) {
                case Qt::AnchorTop:
                    point2 = QPointF(point1.x(), point1.y() - 2);
                    break;
                case Qt::AnchorLeft:
                    point2 = QPointF(point1.x() - 2, point1.y());
                    break;
                case Qt::AnchorBottom:
                    point2 = QPointF(point1.x(), point1.y() + 2);
                    break;
                case Qt::AnchorRight:
                    point2 = QPointF(point1.x() + 2, point1.y());
                    break;
                }
                for each (auto wire in otherPort->getLinkedConnectorWireList()) {
                    wire->setArrowPoints(Utility::getArrorwPoint(point2, point1, 25.f / 57.3, 8));
                }
            }
        }
        break;
    case 3:
        break;
    }
    return dataList;
}

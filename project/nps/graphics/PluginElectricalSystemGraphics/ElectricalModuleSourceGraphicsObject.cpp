#include "ElectricalModuleSourceGraphicsObject.h"
#include "ElectricalBusbarGraphicsObject.h"
#include "ElectricalDotBusbarGraphicsObject.h"

#include <QDebug>
#include <QPainter>
#include <qmath.h>

ElectricalModuleSourceGraphicsObject::ElectricalModuleSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                           QSharedPointer<SourceProxy> source,
                                                                           QGraphicsItem *parent)
    : SourceGraphicsObject(canvasScene, source, parent), dataTextGraphics(nullptr), protoTypeText(nullptr)
{
    /* //母线监视端口变化
    if(source->prototypeName() == "Busbar" || source->prototypeName() == "DotBusbar")
    {
        connect(source.data(), SIGNAL(sigDelPortContext(QSharedPointer<PortContext>)),
    this,SLOT(onCreatePort(QSharedPointer<PortContext>))); connect(source.data(),
    SIGNAL(sigDelPortContext(QSharedPointer<PortContext>)), this,SLOT(onDeletePort(QSharedPointer<PortContext>)));
    } */
    virtualTopPort = nullptr;
    virtualBottomPort = nullptr;
    m_pBusBarObject = nullptr;
    textOverLapItem = nullptr;
}

ElectricalModuleSourceGraphicsObject::~ElectricalModuleSourceGraphicsObject() { }

void ElectricalModuleSourceGraphicsObject::createInternalGraphics()
{
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    TransformProxyGraphicsObject *transGraphics = getTransformProxyGraphicsObject();
    if (!transGraphics) {
        return;
    }
    setScaleDirection(SourceGraphicsObject::kScaleNone);
    QString prototypeName = sourceProxy->prototypeName();
    if (prototypeName == "Busbar") {
        setYAxisAlignment(SourceGraphicsObject::kAlignmentHalfGrid);
        setScaleDirection(SourceGraphicsObject::kScaleHorizontal);
        m_pBusBarObject = new ElectricalBusbarGraphicsObject(getCanvasScene(), this);
        m_pBusBarObject->createInternalGraphics();
        m_pBusBarObject->updatePosition();
    } else if (prototypeName == "DotBusbar") {
        setXAxisAlignment(SourceGraphicsObject::kAlignmentHalfGrid);
        setYAxisAlignment(SourceGraphicsObject::kAlignmentHalfGrid);
        m_pBusBarObject = new ElectricalDotBusbarGraphicsObject(getCanvasScene(), this);
        m_pBusBarObject->updatePosition();
    } else {

        if (prototypeName == "Line") {
            setYAxisAlignment(SourceGraphicsObject::kAlignmentHalfGrid);
        } else if (prototypeName == "ModularMultilevelConverter" || prototypeName == "VSC_AVG"
                   || prototypeName == "BoostAVG" || prototypeName == "BuckAVG" || prototypeName == "DCDC_AVG") {
            textOverLapItem = new QGraphicsTextItem(transGraphics);
            QFont font;
            font.setPixelSize(6);
            font.setFamily("Microsoft YaHei UI");
            font.setBold(false);
            textOverLapItem->setFont(font);
            if (prototypeName == "ModularMultilevelConverter") {
                textOverLapItem->setPlainText("MMC");
            } else if (prototypeName == "VSC_AVG" || prototypeName == "BoostAVG" || prototypeName == "BuckAVG"
                       || prototypeName == "DCDC_AVG") {
                textOverLapItem->setPlainText("AVG");
            }
            textOverLapItem->setDefaultTextColor(sourceProxy->getStateColor());
            connect(sourceProxy.data(), &SourceProxy::sourceChange, this,
                    &ElectricalModuleSourceGraphicsObject::onSourceChanged);
        }
        SvgGraphicsObject *electricalSvgGraphics = new SvgGraphicsObject(getCanvasScene(), this);
        electricalSvgGraphics->setSvgImageDatas(sourceProxy->svgImageDatas());
        electricalSvgGraphics->updatePosition();
    }

    if (sourceProxy->svgImageDatas().isEmpty()) { // 如果没有图片那么说明是电气构造模块
        QByteArray backgroundImage = sourceProxy->getSourceProperty().getBackgroundImage();
        if (backgroundImage.isNull()) {
            // 如果没有背景图片，才显示原型名
            protoTypeText = new SourceProtoTypeTextGraphicsObject(getCanvasScene(), transGraphics);
            protoTypeText->setPlainText(sourceProxy->prototypeName());
            protoTypeText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
            protoTypeText->updatePosition();
        }
    }

    if (sourceProxy->prototypeName() == "Busbar" || sourceProxy->prototypeName() == "DotBusbar") {
        dataTextGraphics = new ElectrialDataTextGraphicsObject(getCanvasScene(), transGraphics);
        dataTextGraphics->setElectrialDataType(ElectrialDataTextGraphicsObject::BusBarLoadFlowData);
        // dataTextGraphics->updatePosition();
        dataTextGraphics->userShow(false);
        connect(getCanvasScene()->getCanvasContext().data(), SIGNAL(canvasShowPowerFlowFlagChanged(bool)), this,
                SLOT(onReceiveShowPowerFlowChange(bool)));
    }

    if (sourceProxy->prototypeName() == "Busbar" || sourceProxy->prototypeName() == "DotBusbar") {
        // 创建虚拟端口，用于从母线引出连接线
        QSharedPointer<PortContext> portContext1 =
                PPortContext(new PortContext(getSourceProxy()->uuid(), Utility::createUUID()));
        portContext1->setSize(QSize(GKD::PORT_SIZE, GKD::PORT_SIZE));
        portContext1->setIsAbsolutePos(true);
        portContext1->setAnchorPoint(Qt::AnchorTop);
        QSharedPointer<PortContext> portContext2 =
                PPortContext(new PortContext(getSourceProxy()->uuid(), Utility::createUUID()));
        portContext2->setSize(QSize(GKD::PORT_SIZE, GKD::PORT_SIZE));
        portContext2->setIsAbsolutePos(true);
        portContext2->setAnchorPoint(Qt::AnchorBottom);
        virtualTopPort = new BusBarvirtualPortGraphicsObject(getCanvasScene(), this, portContext1, nullptr);
        virtualTopPort->setParentItem(transGraphics);
        // virtualTopPort->setVirtualPortAnchor(BusBarvirtualPortGraphicsObject::topAnchor);
        virtualTopPort->userShow(false);
        virtualBottomPort = new BusBarvirtualPortGraphicsObject(getCanvasScene(), this, portContext2, nullptr);
        virtualBottomPort->setParentItem(transGraphics);
        virtualBottomPort->userShow(false);
        // virtualBottomPort->setVirtualPortAnchor(BusBarvirtualPortGraphicsObject::bottomAnchor);
        connect(transGraphics, SIGNAL(selectedChanged(bool)), this, SLOT(onItemSelectChange(bool)));
        connect(virtualTopPort, SIGNAL(wireHasConnected()), this, SLOT(onWireHasConnected()));
        connect(virtualBottomPort, SIGNAL(wireHasConnected()), this, SLOT(onWireHasConnected()));
    }
}

void ElectricalModuleSourceGraphicsObject::updatePosition()
{
    if (virtualTopPort && virtualBottomPort) {
        virtualTopPort->userShow(false);
        virtualBottomPort->userShow(false);
    }
    QList<QGraphicsItem *> childs = childItems();
    QListIterator<QGraphicsItem *> childIter(childs);
    while (childIter.hasNext()) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(childIter.next());
        if (layer) {
            layer->updatePosition();
            SvgGraphicsObject *svgLayer = dynamic_cast<SvgGraphicsObject *>(layer);
            if (svgLayer && getSourceProxy()) {
                int angle = getTransformProxyGraphicsObject()->getAngle();
                svgLayer->setAngle(angle);
            }
        }
    }

    if (textOverLapItem) {
        textOverLapItem->setPos(5, 5);
    }
}

void ElectricalModuleSourceGraphicsObject::setSourceBoundingRect(QRectF rect)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto businessHook = canvasContext->getBusinessHooksServer();
    if (!businessHook) {
        return;
    }
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    if (businessHook->isElecCombineModel(sourceProxy->uuid())) {
        // 电气构造型大小调整策略和电气元件有区别(应该和控制系统一致)
        SourceGraphicsObject::setSourceBoundingRect(rect);
    } else {
        adjustBoundingRect(rect);
        sourceProxy->setSize(rect.size());
    }
}

bool ElectricalModuleSourceGraphicsObject::isNeedFilp(QString prototypeName)
{
    if (prototypeName == "VoltageSourceConverter" || prototypeName == "DiodeConverterValve"
        || prototypeName == "ThyristorConverterValve" || prototypeName == "Diode" || prototypeName == "VSC_SW"
        || prototypeName == "VSC_AVG" || prototypeName == "IGBT" || prototypeName == "IGBT_Diode"
        || prototypeName == "SingleLineConverter" || prototypeName == "ModularMultilevelConverter"
        || prototypeName == "BoostAVG" || prototypeName == "BuckAVG" || prototypeName == "DCDC_AVG") {
        return true;
    }
    return false;
}

void ElectricalModuleSourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                                 QWidget *widget /* = 0 */)
{

    SourceGraphicsObject::paint(painter, option, widget);
}

void ElectricalModuleSourceGraphicsObject::userMenu(QSharedPointer<QMenu> menu)
{
    if (menu.isNull() || menu->isEmpty()) {
        auto canvasContext = getCanvasContext();
        auto sourceProxy = getSourceProxy();
        if (!canvasContext) {
            return;
        }
        auto businessHook = canvasContext->getBusinessHooksServer();
        if (businessHook && sourceProxy) // 错误/删除的模块右键只有删除
        {
            if (!businessHook->checkBlockState(sourceProxy)) {
                menu->addAction(ActionManager::getInstance().getAction(ActionManager::Delete));
                return;
            }
        }

        SourceGraphicsObject::userMenu(menu);
        auto secMenu = ActionManager::getInstance().getAction(ActionManager::RotateAll)->menu();
        if (secMenu && isNeedFilp(getSourceProxy()->prototypeName())) {
            secMenu->addAction(ActionManager::getInstance().getAction(ActionManager::FlipHorizontal));
            secMenu->addAction(ActionManager::getInstance().getAction(ActionManager::FlipVertical));
        }

        if (sourceProxy) {
            QString prototypeName = sourceProxy->prototypeName();
            if (prototypeName == "Busbar" || prototypeName == "DotBusbar") {
                // 母线或者点状母线
                SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
                QString menuText = QObject::tr("Switch Bus Bar");
                QAction *switchBusbarTypeAction =
                        ActionManager::getInstance().getAction(ActionManager::SwitchBusbarType);
                switchBusbarTypeAction->setText(menuText);
                menu->addAction(switchBusbarTypeAction);
                if (canvasContext->canModify()) {
                    switchBusbarTypeAction->setEnabled(true);
                } else {
                    switchBusbarTypeAction->setEnabled(false);
                }
            }
        }

        if (businessHook) {
            if (businessHook->isBlockCtrlCodeType(getSourceProxy())) {
                menu->addSeparator();
                menu->addAction(ActionManager::getInstance().getAction(ActionManager::Help));
            }
        }
    }
}

void ElectricalModuleSourceGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (virtualTopPort && virtualBottomPort) {
        pressPointF = event->scenePos();
        virtualTopPort->userShow(false);
        virtualBottomPort->userShow(false);
    }
    SourceGraphicsObject::mousePressEvent(event);
}

void ElectricalModuleSourceGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (getCanvasContext()->canModify() && virtualTopPort && virtualBottomPort
            && Utility::lineDistance(pressPointF, event->scenePos()) < 0.5 && pressPointF != QPointF()) {
            pressPointF = QPointF();
            virtualTopPort->userShow(true);
            virtualBottomPort->userShow(true);
            virtualTopPort->setPosition(event->scenePos());
            virtualBottomPort->setPosition(event->scenePos());
        }
    }
}

void ElectricalModuleSourceGraphicsObject::onItemSelectChange(bool flag)
{
    if (virtualTopPort && virtualBottomPort && !flag && !virtualTopPort->isDrawingConnecterWire()
        && !virtualBottomPort->isDrawingConnecterWire()) {
        virtualTopPort->userShow(flag);
        virtualBottomPort->userShow(flag);
    }
}

void ElectricalModuleSourceGraphicsObject::onWireHasConnected()
{
    if (virtualTopPort && virtualBottomPort) {
        virtualTopPort->userShow(false);
        virtualBottomPort->userShow(false);
    }
}

void ElectricalModuleSourceGraphicsObject::onSourceChanged(QString key, QVariant value)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    if (key == GKD::SOURCE_STATE) {
        if (textOverLapItem) {
            textOverLapItem->setDefaultTextColor(sourceProxy->getStateColor());
        }
    }
}

void ElectricalModuleSourceGraphicsObject::onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue)
{
    if (key == SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY) {
        // 背景图
        QByteArray imageData = newValue.toByteArray();
        if (protoTypeText) {
            if (imageData.isNull()) {
                protoTypeText->userShow(true);
            } else {
                protoTypeText->userShow(false);
            }
        }
    }
}

void ElectricalModuleSourceGraphicsObject::onReceiveShowPowerFlowChange(bool flag)
{
    if (!dataTextGraphics)
        return;
    if (flag) {
        getTransformProxyGraphicsObject()->setZValue(100);
        this->dataTextGraphics->setZValue(100);
        BusinessHooksServer *businessHooks = getCanvasScene()->getCanvasContext()->getBusinessHooksServer();
        if (!businessHooks)
            return;
        QList<showDataStruct> dataList = businessHooks->getPowerFlowData(getSourceProxy());
        dataTextGraphics->setShowData(dataList, this);
        if (m_pBusBarObject) {
            double busBarVm = 0.0;
            for each (auto data in dataList) {
                if (data.key == "Vm") {
                    busBarVm = data.value.toDouble();
                }
            }
            m_pBusBarObject->setShowPowerFlow(true);
            m_pBusBarObject->setShowColor(businessHooks->GetBusbarVoltageLevelColor(busBarVm));
        }
    } else {
        getTransformProxyGraphicsObject()->setZValue(0);
        this->setZValue(0);
        this->dataTextGraphics->setZValue(0);
        dataTextGraphics->setPlainText("");
        dataTextGraphics->setToolTip("");
        dataTextGraphics->userShow(false);
        if (m_pBusBarObject) {
            m_pBusBarObject->setShowPowerFlow(false);
        }
    }
}

#include "ControlModuleSourceGraphicsObject.h"
#include "ControlModulePortGraphicsObject.h"
#include "IrRegularAnchorPortGraphics.h"

#include <QApplication>
#include <QClipboard>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsView>
#include <QMimeData>
#include <QPainter>
#include <QStringList>

static const qreal LIMIT_LINE_FIRST_POINT_X_OFFSET = 20;  // 画限幅线第一个点X轴的偏移量
static const qreal LIMIT_LINE_SECOND_POINT_X_OFFSET = 15; // 画限幅线第二个点X轴的偏移量
static const qreal LIMIT_LINE_POINT_Y_OFFSET = 10;        // 画限幅线Y轴的固定偏移量
static const QString EI_PROTOTYPE_NAME = "ElectricalInterface";
static const QString SLOT_PROTOTYPE_NAME = "Slot";

ControlModuleSourceGraphicsObject::ControlModuleSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                     QSharedPointer<SourceProxy> source,
                                                                     QGraphicsItem *parent)
    : SourceGraphicsObject(canvasScene, source, parent),
      controlSvgGraphics(nullptr),
      protoTypeText(nullptr),
      displayText(nullptr)
{
    connect(source.data(), SIGNAL(sourceChange(QString, QVariant)), this, SLOT(onSourceChange(QString, QVariant)));
    connect(source.data(), SIGNAL(runVariableDataChange(QMap<QString, QMap<QString, QVariant>>)), this,
            SLOT(onDataTextChange(QMap<QString, QMap<QString, QVariant>>)));

    QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect(this);
    e->setOffset(2, 2);
    e->setBlurRadius(4);
    e->setColor(QColor(25, 25, 25, 150));
    setGraphicsEffect(e);
}

ControlModuleSourceGraphicsObject::~ControlModuleSourceGraphicsObject() { }

void ControlModuleSourceGraphicsObject::createInternalGraphics()
{
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    QString prototypeName = sourceProxy->prototypeName();
    SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();

    MathMLManager &mathMLMgr = MathMLManager::getInstance();
    if (!mathMLMgr.getMathMLDocument(sourceProxy->prototypeName())) {
        if (!sourceProxy->svgImageDatas().isEmpty()) {
            controlSvgGraphics = new ControlSVGGraphicsObject(getCanvasScene(), proxyGraphics);
            controlSvgGraphics->setSvgImageDatas(sourceProxy->svgImageDatas());
            controlSvgGraphics->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
            controlSvgGraphics->updatePosition();
        } else {
            protoTypeText = new SourceProtoTypeTextGraphicsObject(getCanvasScene(), proxyGraphics);
            if (sourceProxy->prototypeName() == EI_PROTOTYPE_NAME) {
                protoTypeText->setPlainText(sourceProxy->name());
            } else {
                QString prototypeReadableName = sourceProxy->getPrototypeReadaleName();
                if (!prototypeReadableName.isEmpty()) {
                    protoTypeText->setPlainText(prototypeReadableName);
                } else {
                    protoTypeText->setPlainText(sourceProxy->prototypeName());
                }
            }
            protoTypeText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
            protoTypeText->updatePosition();
        }
    }

    QString displayString = sourceProperty.getDisplayData().toString();

    // 如果有需要展示的参数 就创建displayText图元
    // if (source->prototypeName() == "TransFunc_Discrete" || source->prototypeName() == "DiscreteTransFunc"
    //     || source->prototypeName() == "TransFunc") {
    //     displayText->userShow(false);
    // }

    if (displayString != "") {
        QString prototypeName = sourceProxy->prototypeName();
        bool isTransferFunctionModule = (prototypeName == "TransFunc_Discrete" || prototypeName == "TransFunc"
                                         || prototypeName == "DiscreteTransFunc");
        if (!isTransferFunctionModule) {
            displayText = new DisplayParameterTextGraphicsObject(getCanvasScene(), proxyGraphics);
            displayText->setPlainText(displayString);
            displayText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
            displayText->updatePosition();
            if (controlSvgGraphics) {
                controlSvgGraphics->setVisible(false);
            }
            if (protoTypeText) {
                protoTypeText->setVisible(false);
            }
        }
    }

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

void ControlModuleSourceGraphicsObject::updatePosition()
{

    if (controlSvgGraphics && getSourceProxy()) {
        int angle = getTransformProxyGraphicsObject()->getAngle();
        controlSvgGraphics->setAngle(angle);
    }
}

QSizeF ControlModuleSourceGraphicsObject::getMinimumSize() const
{
    // if (controlSvgGraphics) {
    //     return controlSvgGraphics->getSVGSize();
    // }
    return SourceGraphicsObject::getMinimumSize();
}

bool ControlModuleSourceGraphicsObject::isShowProTypeName()
{
    QString protypeName = getSourceProxy()->prototypeName();
    if (protypeName == "Constant" || protypeName == "Goto" || protypeName == "From" || protypeName == "Gain"
        || protypeName == "In" || protypeName == "Out") {
        return false;
    }
    return true;
}

void ControlModuleSourceGraphicsObject::selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                                     QWidget *widget)
{
}

void ControlModuleSourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                              QWidget *widget)
{
    SourceGraphicsObject::paint(painter, option, widget);
}

void ControlModuleSourceGraphicsObject::userMenu(QSharedPointer<QMenu> menu)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    SourceGraphicsObject::userMenu(menu);
    bool canModify = canvasContext->canModify();
    if (canModify) {
        ActionManager &mgr = ActionManager::getInstance();

        menu->addSeparator();
        menu->addAction(mgr.getAction(ActionManager::SetBackgrounds));
        QMenu *brackcolor_menu = new QMenu(menu.data());
        brackcolor_menu->addAction(mgr.getAction(ActionManager::SetBackgroundColor));
        brackcolor_menu->addAction(mgr.getAction(ActionManager::ResetBackgroundColor));
        brackcolor_menu->addSeparator();
        brackcolor_menu->addAction(mgr.getAction(ActionManager::SetBackgroundImage));
        brackcolor_menu->addAction(mgr.getAction(ActionManager::ResetBackgroundImage));
        brackcolor_menu->addSeparator();
        brackcolor_menu->addAction(mgr.getAction(ActionManager::SetForegroundImage));
        brackcolor_menu->addAction(mgr.getAction(ActionManager::ResetForegroundImage));
        mgr.getAction(ActionManager::SetBackgrounds)->setMenu(brackcolor_menu);
    }

    auto businessHook = canvasContext->getBusinessHooksServer();
    if (businessHook) {
        if (businessHook->isBlockCtrlCodeType(getSourceProxy())) {
            menu->addSeparator();
            menu->addAction(ActionManager::getInstance().getAction(ActionManager::Help));
        }
        if (businessHook->isBlockCtrlUserCombined(getSourceProxy())) {
            menu->addAction(ActionManager::getInstance().getAction(ActionManager::ShowPortName));
        }
    }
}

void ControlModuleSourceGraphicsObject::onDataTextChange(QMap<QString, QMap<QString, QVariant>> dataMap)
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics)
        return;
    // QMap<QString, QVariant> inputMap = dataMap.value("input",QMap<QString, QVariant>());
    QMap<QString, QVariant> outputMap = dataMap.value("output", QMap<QString, QVariant>());
    QList<PortGraphicsObject *> portList = proxyGraphics->getPortGraphicsObjectList();
    for each (auto port in portList) {
        if (port->getPortContext()->type() != PortOutputType)
            continue;
        if (getSourceProxy()->prototypeName() == "Add") {
            IrRegularAnchorPortGraphics *portGraphics = dynamic_cast<IrRegularAnchorPortGraphics *>(port);
            if (!portGraphics)
                continue;
            QString text;
            text = outputMap.value(portGraphics->getPortContext()->name(), QVariant()).toString();
            portGraphics->setDataTextShow(text);
        } else {
            ControlModulePortGraphicsObject *portGraphics = dynamic_cast<ControlModulePortGraphicsObject *>(port);
            if (!portGraphics)
                continue;
            QString text;
            text = outputMap.value(portGraphics->getPortContext()->name(), QVariant()).toString();
            portGraphics->setDataTextShow(text);
        }
    }
}

void ControlModuleSourceGraphicsObject::onSourceChange(QString key, QVariant value)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    if (key == GKD::SOURCE_SVG_IMAGEMAP || key == GKD::SOURCE_PROTOTYPENAME
        || key == GKD::SOURCE_NAME) { // SVG图片列表 模块原型名称|模块名称
        if (sourceProxy->svgImageDatas().isEmpty() || !isShowProTypeName()) { // SVG为空
            // 常数模块不显示原型名称，通过GKD::SOURCE_EXTRA_INFO_CONSTANT_PARAMETER显示参数值
            if (isShowProTypeName()) {
                if (!protoTypeText) {
                    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
                    protoTypeText = new SourceProtoTypeTextGraphicsObject(getCanvasScene(), proxyGraphics);
                    protoTypeText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
                }
                if (controlSvgGraphics) {
                    controlSvgGraphics->userShow(false);
                }

                protoTypeText->setPlainText(getSourceProxy()->prototypeName());
                protoTypeText->updatePosition();
                protoTypeText->userShow(true);
            }
        } else { // SVG不为空
            if (!controlSvgGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
                controlSvgGraphics = new ControlSVGGraphicsObject(getCanvasScene(), proxyGraphics);
                controlSvgGraphics->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
            }

            if (protoTypeText) {
                protoTypeText->userShow(false);
            }

            controlSvgGraphics->setSvgImageDatas(getSourceProxy()->svgImageDatas());
            controlSvgGraphics->updatePosition();
            controlSvgGraphics->userShow(true);
        }
    } else if (key == GKD::SOURCE_SIZE) { // 模块大小
        // this->setSourceBoundingRect();
    } else if (key == GKD::SOURCE_STATE) { // 模块状态
        // 当控制模块状态为disable时或模块状态为error时,若剪贴板中复制了改模块,则清除剪贴板,限制该模块粘贴
        QSharedPointer<SourceProxy> source = getSourceProxy();
        if (source) {
            if ((source->moduleType() != GKD::SOURCE_MODULETYPE_ELECTRICAL && value.toString() == "disable")
                || value.toString() == "error") {
                ClipBoardMimeData *mimeData =
                        dynamic_cast<ClipBoardMimeData *>((QMimeData *)QApplication::clipboard()->mimeData());
                if (mimeData) {
                    if (mimeData->IsContainSource(source->uuid())) {
                        QApplication::clipboard()->clear();
                    }
                }
            }
        }
    }
}

void ControlModuleSourceGraphicsObject::onSourcePropertyChanged(QString key, QVariant value, QVariant newValue)
{
    if (key == SOURCE_ENABLE_SCALE_PROPERTY_KEY) {
        bool allowScale = value.toBool();
        if (allowScale) {
            setScaleDirection(SourceGraphicsObject::kScaleAll);
        } else {
            setScaleDirection(SourceGraphicsObject::kScaleNone);
        }

        getTransformProxyGraphicsObject()->setSelected(
                false); // 触发TransformProxy 更新大小缩放控制块（refreshSizeControlGraphics
        getTransformProxyGraphicsObject()->setSelected(true);
    } else if (key == SOURCE_DISPLAYDATA_PROPERTY_KEY) {
        if (controlSvgGraphics) {
            controlSvgGraphics->userShow(false);
        }
        if (protoTypeText) {
            protoTypeText->userShow(false);
        }
        TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
        if (!displayText) {
            // 用户将其他模块切换到当前模块类型时，创建Text在代理图层中心显示
            displayText = new DisplayParameterTextGraphicsObject(getCanvasScene(), proxyGraphics);
            displayText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
        }
        displayText->userShow(true);
        QString strConstantValue = value.toString();
        displayText->setPlainText(strConstantValue);

        displayText->updatePosition();
    } else if (key == SOURCE_INPUTOUTPUT_DISPLAY_INDEX_PROPERTY_KEY) { // 输入输出模块展示的序列号
        QSharedPointer<SourceProxy> source = getSourceProxy();
        if (source->prototypeName() == "In" || source->prototypeName() == "Out") {
            TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
            if (!protoTypeText || !proxyGraphics) {
                return;
            }
            QString strConstantValue = QString::number(value.toInt());
            protoTypeText->setPlainText(strConstantValue);
            protoTypeText->updatePosition();
        }
    } else if (key == SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY) {
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

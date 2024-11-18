#include "CanvasViewDefaultImpl.h"
#include "AnnotationSourceGraphicsObject.h"
#include "CanvasContext.h"
#include "CanvasNavigationPrview.h"
#include "CanvasSceneDefaultImpl.h"
#include "CanvasViewDefaultImplPrivate.h"
#include "CanvasViewManager.h"
#include "GraphicsFactoryManager.h"
#include "GraphicsInputModelEdit.h"
#include "GraphicsLayerManager.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "KLWidgets/KCentralWidget.h"
#include "KLWidgets/KMessageBox.h"
#include "NPSPropertyManager.h"
#include "SliderScale.h"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "SourceProxyCommand.h"
#include "Utility.h"
#include "ViewToolBar.h"
#include "actionmanager.h"
#include "graphicsmodelingkernel.h"

#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QDesktopWidget>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QSvgGenerator>
#include <QTimer>
#include <QToolButton>
#include <Windows.h>

#define EDGESIZE 100
#define PREVIEW_WIDGET_Y_OFFSET 5
#define PREVIEW_WIDGET_X_OFFSET 15

const QColor COLOR_SELECTED(117, 249, 77); // 元器件被搜索文本选中绿色

CanvasViewDefaultImpl::CanvasViewDefaultImpl(QSharedPointer<CanvasContext> canvasContext, QWidget *parent)
    : ICanvasView(parent)
{
    dataPtr.reset(new CanvasViewDefaultImplPrivate());
    dataPtr->canvasContext = canvasContext;

    dataPtr->createBox = new CreateModelEditBox(this);

    initScene();
    createViewToolBar();

    loadCanvas(canvasContext);

    setPreviewWidget();

    centerOn(canvasContext->centerPos());
    // 缩放
    zoom(GKD::CANVAS_SCALE, dataPtr->canvasContext->scale());

    dataPtr->isCreateArrow = false;
    QTimer::singleShot(0, this, SLOT(onTimerShot()));
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform
                   | QPainter::HighQualityAntialiasing);

    ActionManager::getInstance().attachCanvasView(this);

    bindSignals();
    /* if (dataPtr->canvasScene) {
        dataPtr->canvasScene->refreshInputAndOutPutIndex();
    } */

    registerAction();

    setFullscreenStatuIcon();
}

CanvasViewDefaultImpl::~CanvasViewDefaultImpl()
{
    dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(QStringList());
    dataPtr->canvasContext->getCanvasProperty().setGotoFromHighLightSourcceList(QStringList());

    CanvasViewManager::getInstance().removeCanvaView(dataPtr->canvasContext->uuid());
    ActionManager::getInstance().detachCanvasView(this);
    if (dataPtr && dataPtr->canvasScene) {
        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(dataPtr->canvasScene.data());
        if (defaultScene) {
            defaultScene->setIgnoreLayerManager(true);
        }
        dataPtr->canvasScene->freeSource();
    }
}

bool CanvasViewDefaultImpl::addSource(QSharedPointer<SourceProxy> source, bool needUndo)
{
    if (dataPtr->canvasScene.isNull() || !source) {
        return false;
    }
    // 画板上下文已经包含存在了对应的资源，不处理了
    if (dataPtr->canvasContext->getSource(source->uuid())) {
        return true;
    }

    if (!needUndo) {
        if (dataPtr->canvasScene->loadFromSourceProxy(source)) {
            onAddSourceChange(source);

            return true;
        }
    } else {
        if (dataPtr->canvasScene->addSourceToScene(source, needUndo)) {
            onAddSourceChange(source);

            return true;
        }
    }

    return false;
}

bool CanvasViewDefaultImpl::deleteSource(QString uuid)
{
    if (dataPtr->canvasScene.isNull()) {
        return false;
    }

    PSourceProxy pSource = dataPtr->canvasContext->getSource(uuid);
    if (!pSource)
        return false;
    if (dataPtr->canvasScene->deleteSourceFromScene(pSource)) {
        onDeleteSourceChange(pSource);

        return true;
    }

    return false;
}

bool CanvasViewDefaultImpl::addWire(QSharedPointer<ConnectorWireContext> wire)
{
    if (dataPtr->canvasScene.isNull() || !wire) {
        return false;
    }

    ConnectorWireGraphicsObject *wireObj = new ConnectorWireGraphicsObject(dataPtr->canvasScene.data());
    wireObj->loadFromContext(wire);

    dataPtr->canvasScene->addConnectorWireGraphicsObject(wireObj, false);

    return true;
}

bool CanvasViewDefaultImpl::deleteWire(QString uuid)
{
    if (dataPtr->canvasScene.isNull()) {
        return false;
    }
    PConnectorWireContext wire = dataPtr->canvasContext->getConnectorWireContext(uuid);
    if (!wire)
        return false;
    ConnectorWireGraphicsObject *wireObj = dataPtr->canvasScene->getConnectorWireGraphicsByID(uuid);
    if (wireObj) {
        dataPtr->canvasScene->deleteConnectorWireGraphicsObject(wireObj);
        return true;
    }

    return false;
}

bool CanvasViewDefaultImpl::addAnnotationSource(QSharedPointer<SourceProxy> source)
{
    if (dataPtr->canvasScene.isNull()) {
        return false;
    }

    TransformProxyGraphicsObject *transformProxyGraphics = dataPtr->canvasScene->addSourceToScene(source);
    if (transformProxyGraphics && transformProxyGraphics->getSourceGraphicsObject()) {
        dataPtr->annotationGraphics =
                dynamic_cast<AnnotationSourceGraphicsObject *>(transformProxyGraphics->getSourceGraphicsObject());
        if (dataPtr->annotationGraphics) {
            connect(dataPtr->annotationGraphics, SIGNAL(interactionFinished()), this,
                    SLOT(onAnnotationInteractionFinished()));
            connect(dataPtr->annotationGraphics, SIGNAL(cancled()), this, SLOT(onAnnotationCanceled()));
            return true;
        }
    }
    return false;
}

bool CanvasViewDefaultImpl::loadCanvas(QSharedPointer<CanvasContext> canvasContext)
{
    if (canvasContext.isNull()) {
        return false;
    }

    dataPtr->canvasContext->blockSignals(true);
    dataPtr->canvasScene->setLoadingCanvasStatus(true);

    // 电气类相关的画板不支持连接线悬空操作
    if (canvasContext->type() == CanvasContext::kElectricalType
        || canvasContext->type() == CanvasContext::kElecUserDefinedType) {
        canvasContext->setSupportSuspendedConnectorWire(false);
    } else {
        canvasContext->setSupportSuspendedConnectorWire(true);
    }

    // 创建资源图元
    QMap<QString, QSharedPointer<SourceProxy>> allSource = canvasContext->getAllSource();
    QMapIterator<QString, PSourceProxy> iterSource(allSource);
    while (iterSource.hasNext()) {
        iterSource.next();
        PSourceProxy source = iterSource.value();
        source->blockSignals(true);
        dataPtr->canvasScene->loadFromSourceProxy(source);
        source->attachToCanvasContext(canvasContext.data());
        source->blockSignals(false);
    }

    dataPtr->canvasScene->setLoadingCanvasStatus(true);

    QList<PortGraphicsObject *> outputPortList;
    // 创建连接线
    QMap<QString, QSharedPointer<ConnectorWireContext>> allConnectorWireContext =
            canvasContext->getAllConnectorWireContext();
    QMapIterator<QString, PConnectorWireContext> iterConnector(allConnectorWireContext);
    QStringList invalidKeys;
    while (iterConnector.hasNext()) {
        iterConnector.next();
        PConnectorWireContext ctx = iterConnector.value();
        ConnectorWireGraphicsObject *connector = new ConnectorWireGraphicsObject(dataPtr->canvasScene.data());
        connector->blockSignals(true);
        bool isSucceed = connector->loadFromContext(ctx);
        if (!isSucceed) {
            connector->blockSignals(false);
            // 失效的连接线，后置处理，同步模型数据，因为当前信号被阻塞了，所以不在此处删除
            invalidKeys.append(iterConnector.key());
            continue;
        }
        auto outputPort = connector->getOutputTypePortGraphics();
        if (outputPort) {
            outputPortList.push_back(outputPort);
        }
        connector->blockSignals(false);
    }
    dataPtr->canvasScene->setLoadingCanvasStatus(false);
    dataPtr->canvasContext->blockSignals(false);

    // 在这里处理失效的连接线，用来通知模型数据同步更新
    foreach (auto key, invalidKeys) {
        canvasContext->deleteConnectWireContext(key);
    }

    for each (auto outputPort in outputPortList) {
        if (outputPort) {
            outputPort->calcuteConnectorWireBranchPoints();
        }
    }
    CanvasProperty &property = canvasContext->getCanvasProperty();
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(dataPtr->canvasScene.data());
    QObject::connect(&property, &NPSPropertyManager::propertyChanged, defaultScene,
                     &CanvasSceneDefaultImpl::onCanvasPropertyChanged);

    CanvasViewManager::getInstance().addCanvasView(dataPtr->canvasContext->uuid(), this);

    /* 针对电气画板处理，历史bug导致的母线中会多出空余的端口 */
    if (dataPtr->canvasContext->type() == CanvasContext::kElectricalType) {
        // 电气画板
        auto proxyGraphicsList = dataPtr->canvasScene->getTransformProxyGraphicsList();
        foreach (auto proxy, proxyGraphicsList) {
            if (proxy) {
                auto sourceProxy = proxy->getSourceProxy();
                qreal zValue = 0;
                if (sourceProxy
                    && (sourceProxy->prototypeName() == "Busbar" || sourceProxy->prototypeName() == "DotBusbar")) {
                    // 只处理母线
                    auto portList = proxy->getPortGraphicsObjectList();
                    foreach (auto port, portList) {
                        if (!port) {
                            continue;
                        }
                        auto wires = port->getLinkedConnectorWireList();
                        if (wires.isEmpty()) {
                            // 删除母线上没有连接线的端口
                            sourceProxy->delPortContext(port->getPortContext());
                            auto sourceGraphicsObject = proxy->getSourceGraphicsObject();
                            if (sourceGraphicsObject) {
                                sourceGraphicsObject->deletePortGraphicsObject(port);
                            }
                        } else
                            foreach (auto wire, wires) {
                                if (wire) {
                                    qreal wireZValue = wire->zValue();
                                    zValue = wireZValue > zValue ? wireZValue : zValue;
                                }
                            }
                    }
                }
                // 保证母线图元在连接线之上，否则点状母线不好选中
                proxy->setZValue(zValue + 1);
            }
        }
    }

    auto businessHook = canvasContext->getBusinessHooksServer();
    if (businessHook) {
        bool showRunningSort = businessHook->isShowRunningSort();
        canvasContext->enableShowModelRunningSort(showRunningSort);
    }

    subsystemInitialize();

    return true;
}

void CanvasViewDefaultImpl::saveCenterPos()
{
    QRectF visibleRect = mapToScene(0, 0, viewport()->width(), viewport()->height()).boundingRect();
    dataPtr->canvasContext->setCenterPos(visibleRect.center());
}

void CanvasViewDefaultImpl::actionZoomInOut(bool zoomin)
{
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr) {
        return;
    }
    int scalevalue = dataPtr->canvasContext->scale() * 100;
    int dValue; // 变化值
    if (scalevalue >= MID_SCALE)
        dValue = 25;
    else
        dValue = 5;
    if (zoomin) {
        scalevalue += dValue;
    } else {
        scalevalue -= dValue;
    }

    dataPtr->canvasContext->setScale(scalevalue / 100.f);
}

void CanvasViewDefaultImpl::actionFitIn()
{
    // 自适应大小
    QRectF fitinrect = getFitinRect();
    setViewRect(fitinrect);
    syncPreviewRect();
}

void CanvasViewDefaultImpl::setFullscreenMode(bool bfull)
{
    dataPtr->isFullScreen = bfull;
    if (dataPtr->isFullScreen) {
        setFullscreenStatuIcon(true);
    } else {
        setFullscreenStatuIcon();
    }
}

void CanvasViewDefaultImpl::onPreviewScaleChanged(QString uuid, double value)
{
    if (dataPtr->canvasContext->uuid() == uuid) {
        dataPtr->canvasContext->setScale(value);
    }
}

void CanvasViewDefaultImpl::onsceneRectChanged(const QRectF &value)
{
    int i = 1;
    QRectF miniRect = getMinRect();
    miniRect.moveTo(0, 0);
    /* if (!value.contains(miniRect) && !getMinRect().isEmpty()) {
        KMessageBox::information(tr("The content goes beyond the artboard boundary")); // 内容超出画板边界
    } */
}

QRectF CanvasViewDefaultImpl::getMinRect()
{
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr) {
        return QRectF();
    }
    qreal right = 0, bottom = 0;
    QSharedPointer<GraphicsLayerManager> pGraphicsLayer = dataPtr->canvasScene->getGraphicsLayerManager();
    if (pGraphicsLayer == nullptr) {
        return QRectF();
    }

    // 获取画板中所有电气或控制模块元件
    auto transformProxyLayerlist = pGraphicsLayer->getLayers(GraphicsLayer::kTransformProxyGraphicsLayer);
    // 所有类型模块元件总集合
    QList<GraphicsLayer *> allLayerlist;
    allLayerlist.append(transformProxyLayerlist);

    // 计算视图最小尺寸
    for each (auto layer in allLayerlist) {
        if (layer != nullptr) {
            QRectF sourceRect = layer->mapRectToScene(layer->boundingRect());
            QPointF sourcepos = sourceRect.topLeft();
            QSizeF sourcesize = sourceRect.size();
            if (sourcepos.x() + sourcesize.width() > right) {
                right = sourcepos.x() + sourcesize.width();
            }

            if (sourcepos.y() + sourcesize.height() > bottom) {
                bottom = sourcepos.y() + sourcesize.height();
            }
        }
    }

    return QRectF(0, 0, right, bottom);
}

void CanvasViewDefaultImpl::setNextConstructiveBoardStatus(bool falg)
{
    dataPtr->canvasContext->setStatusFlag(CanvasContext::kNextConstructiveBorad, falg);
}

void CanvasViewDefaultImpl::cleanCache()
{
    if (dataPtr->canvasContext) {
        CanvasViewManager::getInstance().removeCanvaView(dataPtr->canvasContext->uuid());
    }
}

bool CanvasViewDefaultImpl::isMiddleButtonPressed()
{
    return dataPtr->isMiddleButtonPressed;
}

void CanvasViewDefaultImpl::resetAnnotationGraphicsObject(SourceGraphicsObject *sourceGraphicsObject)
{
    if (sourceGraphicsObject && sourceGraphicsObject == dataPtr->annotationGraphics) {
        dataPtr->annotationGraphics = nullptr;
    }
}

void CanvasViewDefaultImpl::onActiveProgram()
{
    BusinessHooksServer *hookServer = dataPtr->canvasContext->getBusinessHooksServer();
    if (hookServer && hookServer->isFullScreenWorkArea()) {
        HWND handle = FindWindowA(nullptr, (LPCSTR) "SimuNPS");
        if (handle == nullptr) // 判断是否为空
        {
            return;
        }
        ShowWindow(handle,
                   SW_RESTORE);      // 激活窗口，参数：SW_RESTORE，以程序之前的大小显示
        SetForegroundWindow(handle); // 激活窗口在桌面最前面
    }
}

bool CanvasViewDefaultImpl::getIsFullScreen()
{
    return dataPtr->isFullScreen;
}

void CanvasViewDefaultImpl::activeFullScreenAction()
{
    QAction *action = ActionManager::getInstance().getAction(ActionManager::FullScreen);
    action->activate(QAction::Trigger);
}

void CanvasViewDefaultImpl::widgetShow(const QString &canvasName, const QString &canvasuuid)
{
    if (dataPtr->canvasContext != nullptr && canvasuuid == dataPtr->canvasContext->uuid()) {
        ActionManager::getInstance().setActivateCanvas(this->getCanvasScene());
        // 窗口激活
        dataPtr->canvasContext->setStatusFlag(CanvasContext::kActiveWindow, true);
        auto mode = dataPtr->canvasContext->getInteractionMode();
        onCanvasInteractionModeChanged(dataPtr->canvasContext->uuid(), kNormal, mode);
        ActionManager::getInstance().onCanvasInteractionModeChanged(dataPtr->canvasContext->uuid(), kNormal, mode);
        viewport()->unsetCursor();
        refreshToolBarComboBox();
    }

    if (dataPtr->navigationPreview) {
        if (!dataPtr->isPreviewSetPos) {
            auto rect = this->rect();
            dataPtr->isPreviewSetPos = true;
            QSizeF previewSize = dataPtr->previewWidget->size();
            QPoint previewPos =
                    QPoint(rect.right() - previewSize.width() - PREVIEW_WIDGET_X_OFFSET, PREVIEW_WIDGET_Y_OFFSET);
            dataPtr->previewWidget->move(previewPos);
            dataPtr->navigationPreview->viewport()->update();
        }
        syncPreviewRect();
    }
}

void CanvasViewDefaultImpl::widgetHide(const QString &canvasName, const QString &canvasuuid)
{
    if (dataPtr->canvasContext
        && ActionManager::getInstance().getActivateCanvasUUID() == dataPtr->canvasContext->uuid()) {
        // 窗口未激活
        dataPtr->canvasContext->setStatusFlag(CanvasContext::kActiveWindow, false);
        ActionManager::getInstance().disable();
    }
}

QWidget *CanvasViewDefaultImpl::getToolBar()
{
    return dataPtr->toolBar;
}

void CanvasViewDefaultImpl::setRunningStatus(bool bRunning)
{
    dataPtr->canvasContext->setStatusFlag(CanvasContext::kSimulationRunningStatus, bRunning);
}

QRectF CanvasViewDefaultImpl::getSyncViewRect()
{
    return dataPtr->lastSyncPreviewRect;
}

void CanvasViewDefaultImpl::setViewRect(const QRectF &fitIn)
{
    double scale1 = this->width() / fitIn.width();
    double scale2 = this->height() / fitIn.height();

    double scale = QString::number(scale1 < scale2 ? scale1 : scale2, 'f', 2).toDouble();

    dataPtr->canvasContext->setScale(scale);

    this->centerOn(fitIn.center());
}

QPointF CanvasViewDefaultImpl::checkEdge(QPointF prev)
{
    int x = prev.rx();
    int y = prev.ry();

    if (x < 0) {
        x = EDGESIZE / 2;
    }

    if (x > scene()->sceneRect().width() - EDGESIZE) {
        x = scene()->sceneRect().width() - EDGESIZE;
    }

    if (y < 0) {
        y = EDGESIZE / 2;
    }

    if (y > scene()->sceneRect().height() - EDGESIZE) {
        y = scene()->sceneRect().height() - EDGESIZE;
    }
    return QPointF(x, y);
}

void CanvasViewDefaultImpl::sceneSizeChanged()
{
    if (dataPtr->canvasContext == nullptr || dataPtr->canvasScene == nullptr) {
        return;
    }
    int sceneWidth = 0;
    int sceneHeight = 0;
    if (dataPtr->canvasContext->direction() == QBoxLayout::LeftToRight) {
        sceneWidth = dataPtr->canvasContext->size().width();
        sceneHeight = dataPtr->canvasContext->size().height();
    } else {
        sceneWidth = dataPtr->canvasContext->size().height();
        sceneHeight = dataPtr->canvasContext->size().width();
    }

    // 画布大小的单位为mm,转换成像素
    QRectF sceneRect = QRectF(0, 0, 3.7795 * sceneWidth, 3.7795 * sceneHeight);
    dataPtr->canvasScene->setSceneRect(sceneRect);
    // 同步更新导航窗格
    syncPreviewRect();
}

void CanvasViewDefaultImpl::syncPreviewRect()
{
    // 同步更新导航预览窗口的矩形区域
    if (dataPtr->canvasContext->getShowNavigationPreview()) {
        QRectF visibleRect = mapToScene(0, 0, viewport()->width(), viewport()->height()).boundingRect();
        if (visibleRect.left() < sceneRect().left()) {
            visibleRect.setLeft(sceneRect().left());
        }
        if (visibleRect.right() > sceneRect().right()) {
            visibleRect.setRight(sceneRect().right());
        }
        if (visibleRect.top() < sceneRect().top()) {
            visibleRect.setTop(sceneRect().top());
        }
        if (visibleRect.bottom() > sceneRect().bottom()) {
            visibleRect.setBottom(sceneRect().bottom());
        }
        dataPtr->lastSyncPreviewRect = visibleRect.toRect();
        emit sceneViewRectChanged(visibleRect);
    }
}

void CanvasViewDefaultImpl::highlightElectricSource()
{
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr || dataPtr->toolBar == nullptr
        || dataPtr->canvasContext->type() != CanvasContext::kElectricalType) {
        return;
    }
    BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
    if (hookserver == nullptr) {
        return;
    }
    QString filterPhasestr = dataPtr->toolBar->getToolBarBoxValue(ViewToolBar::DefaultPhaseTechnologies);
    QString filtervolStr = dataPtr->toolBar->getToolBarBoxValue(ViewToolBar::DefaultVoltageLevel).replace("kV", "");
    /*     double filtervoldouble = 0;
        if (!filtervolStr.isEmpty()) {
            filtervoldouble = filtervolStr.toDouble();
        } */
    if (filtervolStr.isEmpty() && filterPhasestr.isEmpty()) {
        dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(QStringList());
        update();
        return;
    }
    QStringList sourceIDS;
    QMap<QString, QSharedPointer<SourceProxy>> allsourcemap = dataPtr->canvasContext->getAllSource();
    foreach (QSharedPointer<SourceProxy> psource, allsourcemap.values()) {
        if (psource != nullptr && (psource->prototypeName() == "Busbar" || psource->prototypeName() == "DotBusbar")) {
            QString velLevel = QString::number(hookserver->getElectricalBlockVoltage(psource));
            QString phaseTech = hookserver->getElectricalBlockPhase(psource);
            if (filtervolStr.isEmpty() && !filterPhasestr.isEmpty() && phaseTech == filterPhasestr) {
                sourceIDS << psource->uuid();
            } else if (!filtervolStr.isEmpty() && filterPhasestr.isEmpty() && velLevel == filtervolStr) {
                sourceIDS << psource->uuid();
            } else if (!filtervolStr.isEmpty() && !filterPhasestr.isEmpty() && velLevel == filtervolStr
                       && phaseTech == filterPhasestr) {
                sourceIDS << psource->uuid();
            }
        }
    }
    dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(sourceIDS);
    update();
}

QRectF CanvasViewDefaultImpl::getFitinRect()
{
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr) {
        return QRectF();
    }
    qreal top = 65535, left = 65535, right = 0, bottom = 0;
    int nVerticalScrollbarWidth = this->verticalScrollBar()->width() + 15;
    int nhorizontalScrollbarHeight = this->horizontalScrollBar()->height() + 15;

    QSharedPointer<GraphicsLayerManager> pGraphicsLayer = dataPtr->canvasScene->getGraphicsLayerManager();
    if (pGraphicsLayer == nullptr) {
        return QRectF();
    }

    // 获取画板中所有电气或控制模块元件
    auto transformProxyLayerlist = pGraphicsLayer->getLayers(GraphicsLayer::kTransformProxyGraphicsLayer);
    // 所有类型模块元件总集合
    QList<GraphicsLayer *> allLayerlist;
    allLayerlist.append(transformProxyLayerlist);

    // 计算视图最小尺寸
    for each (auto layer in allLayerlist) {
        if (layer != nullptr) {
            QRectF sourceRect = layer->mapRectToScene(layer->boundingRect());
            QPointF sourcepos = sourceRect.topLeft();
            QSizeF sourcesize = sourceRect.size();

            if (sourcepos.x() < left) {
                left = sourcepos.x();
            }
            if (sourcepos.y() < top) {
                top = sourcepos.y();
            }

            if (sourcepos.x() + sourcesize.width() > right) {
                right = sourcepos.x() + sourcesize.width();
            }

            if (sourcepos.y() + sourcesize.height() > bottom) {
                bottom = sourcepos.y() + sourcesize.height();
            }
        }
    }
    top -= nVerticalScrollbarWidth;
    left -= nhorizontalScrollbarHeight;
    right += nVerticalScrollbarWidth;
    bottom += nhorizontalScrollbarHeight;

    return QRectF(left, top, right - left, bottom - top);
}

QSharedPointer<QByteArray> CanvasViewDefaultImpl::saveAsPngData()
{
    if (!dataPtr->canvasScene) {
        return QSharedPointer<QByteArray>();
    }

    QRectF avaiableRect = dataPtr->canvasScene->getTransformProxyGraphicsPainterPath().boundingRect();

    avaiableRect = avaiableRect.adjusted(-10, -10, 10, 10);

    QPixmap pixmap(avaiableRect.width(), avaiableRect.height());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto cacheColor = dataPtr->canvasContext->backgroundColor();
    auto gridFlag = dataPtr->canvasContext->gridFlag();
    bool isShowSourceName = dataPtr->canvasContext->getCanvasProperty().isSourceNameVisible();
    bool isCopyRightVisible = dataPtr->canvasContext->getCanvasProperty().isLegendVisible();
    if (isCopyRightVisible) {
        dataPtr->canvasContext->getCanvasProperty().setLegendVisible(false);
    }
    if (isShowSourceName) {
        dataPtr->canvasContext->getCanvasProperty().setSourceNameVisble(false);
    }
    dataPtr->canvasContext->setBackgroundColor(QColor(255, 255, 255, 0));
    dataPtr->canvasContext->setGridFlag(CanvasContext::kGridNoneFlag);
    QRectF dstRect = QRectF(0, 0, avaiableRect.width(), avaiableRect.height());
    dataPtr->canvasScene->render(&painter, dstRect, avaiableRect);
    dataPtr->canvasContext->setBackgroundColor(cacheColor);
    dataPtr->canvasContext->setGridFlag(gridFlag);
    dataPtr->canvasContext->getCanvasProperty().setLegendVisible(isCopyRightVisible);
    dataPtr->canvasContext->getCanvasProperty().setSourceNameVisble(isShowSourceName);

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    QByteArray data = buffer.data();
    QSharedPointer<QByteArray> sharedBytes = QSharedPointer<QByteArray>(new QByteArray(data));
    return sharedBytes;
}

bool CanvasViewDefaultImpl::saveAsSVGFile(QString filePath)
{
    if (!dataPtr->canvasScene || filePath.isEmpty()) {
        return false;
    }

    QRectF avaiableRect = dataPtr->canvasScene->getTransformProxyGraphicsPainterPath().boundingRect();
    avaiableRect = avaiableRect.adjusted(-10, -10, 10, 10);

    {
        QSvgGenerator svgGen;
        svgGen.setFileName(filePath);
        svgGen.setSize(avaiableRect.size().toSize());
        int w = avaiableRect.width();
        int h = avaiableRect.height();
        QRect viewRect = QRect(-w / 2, -h / 2, w, h);
        svgGen.setViewBox(viewRect);
        svgGen.setSize(QSize(-1, -1));
        QPainter painter(&svgGen);
        auto cacheColor = dataPtr->canvasContext->backgroundColor();
        auto gridFlag = dataPtr->canvasContext->gridFlag();
        bool isShowSourceName = dataPtr->canvasContext->getCanvasProperty().isSourceNameVisible();
        bool isCopyRightVisible = dataPtr->canvasContext->getCanvasProperty().isLegendVisible();
        if (isCopyRightVisible) {
            dataPtr->canvasContext->getCanvasProperty().setLegendVisible(false);
        }
        if (isShowSourceName) {
            dataPtr->canvasContext->getCanvasProperty().setSourceNameVisble(false);
        }
        dataPtr->canvasContext->setBackgroundColor(QColor(255, 255, 255, 0));
        dataPtr->canvasContext->setGridFlag(CanvasContext::kGridNoneFlag);
        dataPtr->canvasScene->render(&painter, viewRect, avaiableRect);
        dataPtr->canvasContext->setBackgroundColor(cacheColor);
        dataPtr->canvasContext->setGridFlag(gridFlag);
        dataPtr->canvasContext->getCanvasProperty().setLegendVisible(isCopyRightVisible);
        dataPtr->canvasContext->getCanvasProperty().setSourceNameVisble(isShowSourceName);
    }

    return true;
}

QSharedPointer<ICanvasScene> CanvasViewDefaultImpl::getCanvasScene()
{
    return dataPtr->canvasScene;
}

void CanvasViewDefaultImpl::scrollToBlock(QString activeBlockID)
{
    if (activeBlockID.isEmpty()) {
        return;
    }

    if (dataPtr == nullptr || dataPtr->canvasScene == nullptr || dataPtr->canvasContext == nullptr) {
        return;
    }
    QStringList sourceIDS;
    auto sourceProxy = dataPtr->canvasContext->getSource(activeBlockID);
    if (sourceProxy != nullptr) {
        sourceIDS.append(sourceProxy->uuid());
        dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(sourceIDS);
        centerOn(QPointF(qrand(), qrand())); // 用于刷新切换效果
        QPointF sourcePos = sourceProxy->pos();
        centerOn(sourcePos);
        syncPreviewRect();
        dataPtr->canvasContext->setCenterPos(sourcePos);
    } else {
        dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(sourceIDS);
    }
}

void CanvasViewDefaultImpl::initScene()
{
    QSharedPointer<CanvasContext> canvasContext = dataPtr->canvasContext;
    if (canvasContext.isNull()) {
        return;
    }
    /*     int sceneWidth = 0;
        int sceneHeight = 0;
        if (canvasContext->direction() == QBoxLayout::LeftToRight) {
            sceneWidth = canvasContext->size().width();
            sceneHeight = canvasContext->size().height();
        } else {
            sceneWidth = canvasContext->size().height();
            sceneHeight = canvasContext->size().width();
        }

        // 画布大小的单位为mm,转换成像素
        QRectF sceneRect = QRectF(0, 0, 3.77 * sceneWidth, 3.77 * sceneHeight); */
    QRectF sceneRect = canvasContext->senceRectf();
    dataPtr->canvasScene = QSharedPointer<CanvasSceneDefaultImpl>(new CanvasSceneDefaultImpl(canvasContext, this));
    setScene(dataPtr->canvasScene.data());
    dataPtr->canvasScene->setSceneRect(sceneRect);
    dataPtr->canvasScene->initCopyRight();
    dataPtr->canvasScene->setCanvasView(this);
}

void CanvasViewDefaultImpl::bindSignals()
{

    connect(&BookMark::getInstance(), SIGNAL(previewSceneChanged(QString, QPointF)), this,
            SLOT(onPreviewSceneChangeBookMark(QString, QPointF)));
    connect(&BookMark::getInstance(), SIGNAL(previewScaleChanged(QString, double)), this,
            SLOT(onPreviewScaleChanged(QString, double)));

    connect(dataPtr->canvasScene.data(), SIGNAL(sceneRectChanged(const QRectF)), this,
            SLOT(onsceneRectChanged(const QRectF)));

    connect(dataPtr->canvasContext.data(), SIGNAL(canvasContextChanged(QString, QVariant)), this,
            SLOT(onCanvasSettingChanged(QString, QVariant)));
    connect(dataPtr->canvasContext.data(), SIGNAL(canvasNavigationPreviewVisible(bool)), this,
            SLOT(onShowNavigationPreview(bool)));
    connect(dataPtr->canvasContext.data(), SIGNAL(canvasAddSourceProxy(QSharedPointer<SourceProxy>)), this,
            SLOT(onAddSourceChange(QSharedPointer<SourceProxy>)));
    connect(dataPtr->canvasContext.data(), SIGNAL(canvasDeleteSourceProxy(QSharedPointer<SourceProxy>)), this,
            SLOT(onDeleteSourceChange(QSharedPointer<SourceProxy>)));
    if (dataPtr->canvasContext != nullptr) {
        QMap<QString, QSharedPointer<SourceProxy>> sourcemap = dataPtr->canvasContext->getAllSource();
        for each (QSharedPointer<SourceProxy> psource in sourcemap.values()) {
            if (psource != nullptr) {
                connect(psource.data(), SIGNAL(sourceChange(QString, QVariant)), this,
                        SLOT(onSourceChanged(QString, QVariant)), Qt::UniqueConnection);
            }
        }
    }
    connect(dataPtr->createBox, SIGNAL(createBlock(QString, QString)), this, SLOT(onCreateBlock(QString, QString)));

    connect(dataPtr->canvasContext.data(), &CanvasContext::canvasInteractionModeChanged, this,
            &CanvasViewDefaultImpl::onCanvasInteractionModeChanged);
    connect(dataPtr->canvasContext.data(), &CanvasContext::statusFlagsChanged, this,
            &CanvasViewDefaultImpl::onCanvasStatusFlagsChanged);
}

void CanvasViewDefaultImpl::createViewToolBar()
{
    if (dataPtr->toolBar) {
        return;
    }
    dataPtr->toolBar = new ViewToolBar(this, dataPtr->canvasContext);

    connect(dataPtr->toolBar, SIGNAL(volLevelBoxIndexChanged(const QString &)), this,
            SLOT(onVolLevelChanged(const QString &)));
    connect(dataPtr->toolBar, SIGNAL(phaseTechBoxIndexChanged(const QString &)), this,
            SLOT(onPhaseTeckChanged(const QString &)));

    connect(dataPtr->toolBar, SIGNAL(scaleBoxIndexChanged(const QString &)), this,
            SLOT(onScaleChanged(const QString &)));
    dataPtr->toolBar->setScaleComboboxText(dataPtr->canvasContext->scale() * 100); // 初始化时显示缩放比例

    connect(dataPtr->toolBar->getBookMarkButton(), SIGNAL(pressed()), this, SLOT(OnBookMarkNumberLimit()));

    refreshToolBarComboBox();
}

void CanvasViewDefaultImpl::refreshToolBarComboBox()
{
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr) {
        return;
    }
    if (dataPtr->canvasContext->type() != CanvasContext::kElectricalType) {
        dataPtr->toolBar->setComboBoxVisibleState(false);
        return;
    }
    dataPtr->toolBar->setComboBoxVisibleState(true);
    // 遍历画板上所有模块的电压等级和相技术参数
    BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
    if (hookserver == nullptr) {
        return;
    }
    QList<double> volLevelList;
    QStringList phaseTechList;
    phaseTechList.append("");
    QMap<QString, QSharedPointer<SourceProxy>> allsourcemap = dataPtr->canvasContext->getAllSource();
    QMapIterator<QString, QSharedPointer<SourceProxy>> iter(allsourcemap);
    foreach (QSharedPointer<SourceProxy> psource, allsourcemap.values()) {
        if (psource != nullptr) {
            if (psource->prototypeName() != "Busbar" && psource->prototypeName() != "DotBusbar") {
                continue;
            }
            double velLevel = hookserver->getElectricalBlockVoltage(psource);
            if (!volLevelList.contains(velLevel) /*&& velLevel != 0.0*/) {
                volLevelList.append(velLevel);
            }
            QString phaseTech = hookserver->getElectricalBlockPhase(psource);
            if (!phaseTechList.contains(phaseTech) && !phaseTech.isEmpty()) {
                phaseTechList.append(phaseTech);
            }
        }
    }

    // 电压等级数值从小到大排序
    QStringList volStringList;
    volStringList.append("");
    std::sort(volLevelList.begin(), volLevelList.end());
    for (int i = 0; i < volLevelList.size(); ++i) {
        QString strVol = QString::number(volLevelList.at(i)) + "kV";
        volStringList.append(strVol);
    }

    // 设置工具栏下拉框中的模块电压等级和相技术参数
    dataPtr->toolBar->setComboBoxItemsList(volStringList, ViewToolBar::DefaultVoltageLevel);
    dataPtr->toolBar->setComboBoxItemsList(phaseTechList, ViewToolBar::DefaultPhaseTechnologies);
}

void CanvasViewDefaultImpl::zoom(QString key, QVariant val)
{
    if (key != GKD::CANVAS_SCALE)
        return;

    qreal zoomPercent = val.toDouble();

    if (dataPtr->wheelMousePos != QPointF(0, 0)) {
        setMouseTracking(true);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);

        qreal viewWidth = viewport()->width();
        qreal viewHeight = viewport()->height();

        qreal horizontalPercent = dataPtr->wheelMousePos.x() / viewWidth;
        qreal verticalPercent = dataPtr->wheelMousePos.y() / viewHeight;

        // 鼠标场景坐标
        QPointF mouseScenePoint = mapToScene(dataPtr->wheelMousePos.toPoint());

        QMatrix matrix;
        matrix.setMatrix(zoomPercent, this->matrix().m12(), this->matrix().m21(), zoomPercent, this->matrix().dx(),
                         this->matrix().dy());
        this->setMatrix(matrix);

        // 将scene坐标转换为放大缩小后的坐标
        QPointF viewPoint = this->matrix().map(mouseScenePoint);

        horizontalScrollBar()->setValue(int(viewPoint.x() - viewWidth * horizontalPercent));
        verticalScrollBar()->setValue(int(viewPoint.y() - viewHeight * verticalPercent));

        dataPtr->wheelMousePos = QPointF(0, 0);
        // dataPtr->canvasScene->update();
    } else {
        QMatrix matrix;
        matrix.setMatrix(zoomPercent, this->matrix().m12(), this->matrix().m21(), zoomPercent, this->matrix().dx(),
                         this->matrix().dy());
        this->setMatrix(matrix);
    }
    if (dataPtr->toolBar != nullptr && dataPtr->canvasContext != nullptr) {
        dataPtr->toolBar->setScaleComboboxText(dataPtr->canvasContext->scale() * 100);
    }

    this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    this->setMouseTracking(true);
    saveCenterPos();
    viewport()->update();
    syncPreviewRect();
}

void CanvasViewDefaultImpl::onTimerShot()
{
    syncPreviewRect();
    setFocus();
}

void CanvasViewDefaultImpl::onCanvasSettingChanged(QString key, QVariant value)
{
    if (GKD::CANVAS_SIZE == key || GKD::CANVAS_DIRECTION == key) {
        sceneSizeChanged();
    } else if (GKD::CANVAS_BACKGROUND_COLOR == key) {
        dataPtr->canvasScene->invalidate(dataPtr->canvasScene->sceneRect(), QGraphicsScene::BackgroundLayer);
    } else if (GKD::CANVAS_SCALE == key) {
        zoom(key, value);
    } else if (GKD::CANVAS_InOutIndex_Change == key) {
        dataPtr->canvasScene->refreshInputAndOutPutIndex();
    }
}

void CanvasViewDefaultImpl::onAddSourceChange(QSharedPointer<SourceProxy> source)
{
    if (source != nullptr) {
        connect(source.data(), SIGNAL(sourceChange(QString, QVariant)), this, SLOT(onSourceChanged(QString, QVariant)),
                Qt::UniqueConnection);
    }
    refreshToolBarComboBox();
}

void CanvasViewDefaultImpl::onDeleteSourceChange(QSharedPointer<SourceProxy> source)
{
    if (source != nullptr) {
        disconnect(source.data(), SIGNAL(sourceChange(QString, QVariant)), this,
                   SLOT(onSourceChanged(QString, QVariant)));
    }
    refreshToolBarComboBox();
}

void CanvasViewDefaultImpl::onSourceChanged(QString sourcekey, QVariant value)
{
    if (GKD::SOURCE_OTHER == sourcekey) {
        refreshToolBarComboBox();
    }
}
void CanvasViewDefaultImpl::onPreviewSceneChanged(QPointF pos)
{
    centerOn(pos);
}

void CanvasViewDefaultImpl::onPreviewSceneChangeFinished(QPointF pos)
{
    centerOn(pos);
    syncPreviewRect();
    if (dataPtr->canvasContext) {
        dataPtr->canvasContext->setCenterPos(pos);
    }
}

void CanvasViewDefaultImpl::onPreviewSceneChangeBookMark(QString uuid, QPointF pos)
{
    if (dataPtr->canvasContext->uuid() == uuid) {
        centerOn(pos);
        syncPreviewRect();
        if (dataPtr->canvasContext) {
            dataPtr->canvasContext->setCenterPos(pos);
        }
    }
}

void CanvasViewDefaultImpl::onShowNavigationPreview(bool visible)
{
    if (dataPtr->navigationPreview) {
        if (visible) {
            dataPtr->previewWidget->show();
        } else {
            dataPtr->previewWidget->hide();
        }
        syncPreviewRect();
    }
}

void CanvasViewDefaultImpl::onAnnotationInteractionFinished()
{
    if (dataPtr->annotationGraphics) {
        dataPtr->annotationGraphics->disconnect(this);
        dataPtr->annotationGraphics = nullptr;
    }
}

void CanvasViewDefaultImpl::onAnnotationCanceled()
{
    if (!dataPtr->canvasScene) {
        return;
    }
    if (dataPtr->annotationGraphics && dataPtr->annotationGraphics->getTransformProxyGraphicsObject()) {
        dataPtr->canvasScene->deleteTransformProxyGraphicsObject(
                dataPtr->annotationGraphics->getTransformProxyGraphicsObject());
        dataPtr->annotationGraphics = nullptr;
    }
}

void CanvasViewDefaultImpl::actionShowAlonePortName()
{
    if (!dataPtr->canvasContext) {
        return;
    }
    auto businessHook = dataPtr->canvasContext->getBusinessHooksServer();
    QList<QGraphicsItem *> selecteds = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene().data())->selectedItems();
    bool isHidePort = isEixstHidePort();
    if (selecteds.size() == 0) { // 画板模块端口全部设置
        if (dataPtr->canvasContext->canModify()) {
            auto allSource = dataPtr->canvasContext->getAllSource();

            QMap<QString, PSourceProxy>::Iterator iter;
            for (iter = allSource.begin(); iter != allSource.end(); ++iter) {
                // 内置的本地代码不做处理
                if (businessHook && !businessHook->isBlockCtrlUserCombined(iter.value())) {
                    continue;
                }
                for (int i = 0; i < iter->data()->portList().size(); ++i) {
                    if (iter->data()->portList()[i] != nullptr) {
                        iter->data()->portList()[i]->setIsShowName(isHidePort);
                    }
                }
            }
        }
    } else {
        for each (auto item in selecteds) { // 设置选中的模块端口
            TransformProxyGraphicsObject *proxy = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxy == nullptr) {
                continue;
            }
            PSourceProxy sourceProxy = proxy->getSourceProxy();
            if (businessHook && !businessHook->isBlockCtrlUserCombined(sourceProxy)) {
                continue;
            }
            if (sourceProxy) {
                bool isShowPorts = false;
                for (int i = 0; i < sourceProxy->portList().size(); ++i) {
                    if (sourceProxy->portList()[i] != nullptr) {
                        if (!sourceProxy->portList()[i]->isShowName()) {
                            isShowPorts = true;
                            break;
                        }
                    }
                }
                for (int i = 0; i < sourceProxy->portList().size(); ++i) {
                    if (sourceProxy->portList()[i] != nullptr) {
                        sourceProxy->portList()[i]->setIsShowName(isShowPorts);
                    }
                }
            }
        }
    }
}

bool CanvasViewDefaultImpl::isEixstBlockCtrlCode()
{
    if (!dataPtr->canvasContext) {
        return false;
    }
    auto allSource = dataPtr->canvasContext->getAllSource();
    auto businessHook = dataPtr->canvasContext->getBusinessHooksServer();
    QMap<QString, PSourceProxy>::Iterator iter;
    for (iter = allSource.begin(); iter != allSource.end(); ++iter) {
        if (businessHook && businessHook->isBlockCtrlUserCombined(iter.value())) {
            return true;
        }
    }
    return false;
}

bool CanvasViewDefaultImpl::isEixstHidePort()
{
    if (!dataPtr->canvasContext) {
        return false;
    }
    auto allSource = dataPtr->canvasContext->getAllSource();
    auto businessHook = dataPtr->canvasContext->getBusinessHooksServer();
    QMap<QString, PSourceProxy>::Iterator iter;
    for (iter = allSource.begin(); iter != allSource.end(); ++iter) {
        // 内置的本地代码不做判断
        if (businessHook && !businessHook->isBlockCtrlUserCombined(iter.value())) {
            continue;
        }
        for (int i = 0; i < iter->data()->portList().size(); ++i) {
            if (iter->data()->portList()[i] != nullptr) {
                if (!iter->data()->portList()[i]->isShowName()) {
                    return true;
                }
            }
        }
    }
    return false;
}

void CanvasViewDefaultImpl::setFullscreenStatuIcon(bool falg)
{
    if (falg) {
        ActionManager::getInstance()
                .getAction(ActionManager::FullScreen)
                ->setToolTip(tr("Exit Full Screen")); // 退出全屏展示
    } else {
        ActionManager::getInstance()
                .getAction(ActionManager::FullScreen)
                ->setToolTip(tr("Full Screen")); // 画板全屏显示
    }
    if (dataPtr->toolBar) {
        if (!dataPtr->toolBar->actionToolButtonMap.isEmpty()) {
            QToolButton *button =
                    dataPtr->toolBar
                            ->actionToolButtonMap[ActionManager::getInstance().getAction(ActionManager::FullScreen)];
            if (button) {
                button->setIcon(QIcon());
                if (falg) {
                    button->setProperty("isFull", "true");
                } else {
                    button->setProperty("isFull", "false");
                }
                button->style()->unpolish(button);
                button->style()->polish(button);
                button->update();
            }
        }
    }
}

void CanvasViewDefaultImpl::setLockStatusIcon(bool falg)
{
    if (!falg) {
        ActionManager::getInstance().getAction(ActionManager::Lock)->setToolTip(tr("Canvas Lock")); // 锁定
    } else {
        ActionManager::getInstance().getAction(ActionManager::Lock)->setToolTip(tr("Canvas Unlock")); // 解锁
    }
    if (dataPtr->toolBar) {
        if (!dataPtr->toolBar->actionToolButtonMap.isEmpty()) {
            QToolButton *button =
                    dataPtr->toolBar->actionToolButtonMap[ActionManager::getInstance().getAction(ActionManager::Lock)];
            if (button) {
                button->setIcon(QIcon());
                if (falg) {
                    button->setProperty("isLock", "true");
                } else {
                    button->setProperty("isLock", "false");
                }
                button->style()->unpolish(button);
                button->style()->polish(button);
                button->update();
            }
        }
    }
}

void CanvasViewDefaultImpl::wheelEvent(QWheelEvent *event)
{
    if (dragMode() == QGraphicsView::ScrollHandDrag) {
        return;
    }
    if (dataPtr->annotationGraphics) {
        QPoint pos = event->pos();
        QPointF movePos = mapToScene(pos);
        dataPtr->annotationGraphics->setMovingPos(movePos);
    }
    // 鼠标滚轮滚动同时ctrl键被按下，响应缩放
    if (event->modifiers() == Qt::ControlModifier) {
        dataPtr->wheelMousePos = event->pos();
        qreal scale = dataPtr->canvasContext->scale();
        auto scaleCoefficientListText = dataPtr->toolBar->getScaleCoefficient();
        QStringList scaleCoefficientList;
        QString resultText = NULL;
        bool isVector = false;
        for (int i = 0; i < scaleCoefficientListText.size(); ++i) {
            if (scaleCoefficientListText[i].at(scaleCoefficientListText[i].size() - 1) == '%') {
                scaleCoefficientList << scaleCoefficientListText[i].replace("%", "");
            }
        }

        // 当前比例大小是否为固定比例中的数字
        for (int i = 0; i < scaleCoefficientList.size(); ++i) {
            if (scaleCoefficientList[i].toInt() == qRound(scale * 100)) {
                isVector = true;
                break;
            }
        }

        // 如果当前滚轮不在固定比例列表中
        if (!isVector) {
            double last = 0.0, next = 0.0;
            for (int i = 0; i < scaleCoefficientList.size(); ++i) {
                if (qRound(scale * 100) > scaleCoefficientList[i].toInt()) {
                    last = scaleCoefficientList[i].toInt();
                } else {
                    next = scaleCoefficientList[i].toInt();
                    break;
                }
            }
            if (event->delta() > 0) {
                resultText = QString("%1%2").arg(next).arg("%");
            } else {
                resultText = QString("%1%2").arg(last).arg("%");
            }
        } else {
            for (int i = 0; i < scaleCoefficientList.size(); ++i) { // 获取下次操作缩放大小
                if (scaleCoefficientList[i].toInt() == int(scale * 100)) {
                    if (event->delta() > 0) {
                        if (i == scaleCoefficientList.size() - 1) {
                            resultText =
                                    QString("%1%2").arg(scaleCoefficientList[scaleCoefficientList.size() - 1]).arg("%");
                        } else {
                            resultText = QString("%1%2").arg(scaleCoefficientList[i + 1]).arg("%");
                        }
                    } else {
                        if (i == 0) {
                            resultText = QString("%1%2").arg(scaleCoefficientList[0]).arg("%");
                        } else {
                            resultText = QString("%1%2").arg(scaleCoefficientList[i - 1]).arg("%");
                        }
                    }
                }
            }
        }

        dataPtr->toolBar->onScaleBoxIndexChanged(resultText);
        event->accept();
        return;
    }
    ICanvasView::wheelEvent(event);
    syncPreviewRect();
}

void CanvasViewDefaultImpl::mousePressEvent(QMouseEvent *event)
{
    if (!this->isInteractive() || !dataPtr->canvasContext) {
        ICanvasView::mousePressEvent(event);
        return;
    }

    if (dataPtr->searchLineUUIDText.isEmpty() && dataPtr->currentVolumeText.isEmpty()
        && dataPtr->currentPhapseText.isEmpty()) {
        // add by liwenyu 2023.06.20
        // 电压、相数筛选和搜索框内容都为空,鼠标点击时清理高亮效果，因为高亮还可以由日志窗口的超链接触发
        dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(QStringList());
    }

    auto mode = dataPtr->canvasContext->getInteractionMode();

    if (dataPtr->createBox && dataPtr->canvasContext->canModify() && mode == kNormal
        && event->button() == Qt::LeftButton && !dataPtr->annotationGraphics) {
        dataPtr->createBox->move(event->pos().x() - dataPtr->createBox->getInitialSize().width() / 2,
                                 event->pos().y() - dataPtr->createBox->getInitialSize().height() / 2);
        dataPtr->createBox->setStatus(false, true);
    } else if (dataPtr->createBox && dataPtr->createBox->isActive()) {
        dataPtr->createBox->setStatus(false, false);
    }
    // 鼠标点击时的坐标
    dataPtr->clickPos = event->pos();
    QPointF scenePos = mapToScene(dataPtr->clickPos);

    if (event->button() == Qt::LeftButton) {
        // 鼠标左键点击时
        if (mode == kNormal) {
            if (event->modifiers() == Qt::ControlModifier) {
                // 鼠标点击时，Ctrl键同时按下
                QGraphicsItem *clickItem = dataPtr->canvasScene->itemAt(scenePos);
                if (!clickItem) {
                    // 如果鼠标点击坐标处没有图元，则将模式改为画板拖动模式
                    setDragMode(QGraphicsView::ScrollHandDrag);
                }
            } else if (event->modifiers() == Qt::AltModifier) {
                // 鼠标点击时，Alt键同时按下
            }
        } else if (mode == kAnnotation) {
            if (!dataPtr->canvasContext->getCanvasProperty().isAnnotationVisible()) {
                QString tipinfo = tr("Annotation is not visible,Modify annotation visibility to true");
                // 提示用户是否修改资源为可见
                if (KMessageBox::question(tipinfo, KMessageBox::Yes | KMessageBox::No) == KMessageBox::Yes) {
                    dataPtr->canvasContext->getCanvasProperty().setAnnotationVisible(true);
                }
                return ICanvasView::mousePressEvent(event);
            }
            // 注解模式
            if (dataPtr->annotationGraphics && !dataPtr->annotationGraphics->isInterfactionFinished()) {
                // 注解模式时，注解图元未结束，继续处理当前的鼠标事件
                dataPtr->annotationGraphics->processMousePressEvent(event);
            } else {
                QString annotationTypeStr = ActionManager::getInstance().getAnnotationTypeString();
                if (!annotationTypeStr.isEmpty() && annotationTypeStr != TextAnnotationStr
                    && annotationTypeStr != ImageAnnotationStr) {
                    // 文本和图片不在鼠标点击时处理，在鼠标释放时处理
                    PSourceProxy annotationSource = PSourceProxy(new SourceProxy());
                    annotationSource->setModuleType(GKD::SOURCE_MODULETYPE_ANNOTATION);
                    annotationSource->setSize(QSize(0, 0));
                    annotationSource->setPrototypeName(annotationTypeStr);
                    // 创建注释图层
                    bool flag = addAnnotationSource(annotationSource);
                    if (flag && dataPtr->annotationGraphics) {
                        dataPtr->annotationGraphics->processMousePressEvent(event);
                    }
                }
                event->accept();
                return;
            }
        } else if (mode == kAreaAmplification) {
            dataPtr->selectStartPoint = scenePos;
        }
    } else if (event->button() == Qt::MiddleButton) {
        dataPtr->isMiddleButtonPressed = true;
        // add by liwenyu 2023.07.24 鼠标中键点击时可以拖动画布，设置拖拽模式，并模拟处理一个鼠标左键点击事件
        QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, event->localPos(), Qt::LeftButton, event->buttons(),
                                         Qt::NoModifier);
        setDragMode(QGraphicsView::ScrollHandDrag);
        ICanvasView::mousePressEvent(e);
        delete e;
        return;
        // end
    }
    ICanvasView::mousePressEvent(event);
    event->accept();
}

void CanvasViewDefaultImpl::mouseMoveEvent(QMouseEvent *event)
{
    if (!dataPtr->canvasContext) {
        return ICanvasView::mouseMoveEvent(event);
    }
    // 移动中的鼠标位置
    QPoint movingPos = event->pos();

    if (!dataPtr->createBox->isVisible() && !dataPtr->clickPos.isNull()
        && (movingPos - dataPtr->clickPos).manhattanLength() > 160) {
        // 鼠标的位置和鼠标点击时位置的曼哈顿路径大于指定距离的时候，隐藏键盘输出创建模型的输入框
        dataPtr->createBox->setStatus(false, false);
        dataPtr->clickPos = QPoint();
    }

    if (dataPtr->annotationGraphics) {
        // 注解模式下，注解模块处理该鼠标事件
        dataPtr->annotationGraphics->processMouseMoveEvent(event);
        event->accept();
        return;
    }

    ICanvasView::mouseMoveEvent(event);
    event->accept();
}

void CanvasViewDefaultImpl::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        // add by liwenyu 2023.07.24 鼠标中键点击时可以拖动画布，设置拖拽模式，并模拟处理一个鼠标左键点击事件
        QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonRelease, event->localPos(), Qt::LeftButton,
                                         event->buttons(), Qt::NoModifier);
        saveCenterPos();
        setDragMode(QGraphicsView::NoDrag);
        ICanvasView::mouseReleaseEvent(e);
        delete e;
        setDragMode(QGraphicsView::RubberBandDrag);
        dataPtr->isMiddleButtonPressed = false;
        return;
        // end
    }
    ICanvasView::mouseReleaseEvent(event);
    dataPtr->isMiddleButtonPressed = false;
    if (dataPtr == nullptr || dataPtr->canvasScene == nullptr || dataPtr->canvasContext == nullptr
        || !this->isInteractive()) {
        return;
    }
    InteractionMode mode = dataPtr->canvasContext->getInteractionMode();

    QPointF scenePos = mapToScene(event->pos());

    if (dragMode() == QGraphicsView::ScrollHandDrag) {
        // 画板拖动模式，保存画板中心点坐标
        saveCenterPos();
        if (mode != kHandDrag) {
            setDragMode(QGraphicsView::RubberBandDrag);
        }
    }

    if (event->button() == Qt::LeftButton) {
        if (mode == kAnnotation) {
            // 注解模式
            if (dataPtr->annotationGraphics) {
                // 几何图形注解
                dataPtr->annotationGraphics->processMouseReleaseEvent(event);
            } else {
                // 没有图元对象，那么注解模式应该是图片或者文字
                ActionManager &mgr = ActionManager::getInstance();
                QString annotationTypeStr = mgr.getAnnotationTypeString();
                if (annotationTypeStr == ImageAnnotationStr || annotationTypeStr == TextAnnotationStr) {
                    PSourceProxy source = PSourceProxy(new SourceProxy());
                    source->setModuleType(GKD::SOURCE_MODULETYPE_ANNOTATION);
                    source->setPrototypeName(annotationTypeStr);
                    source->setPos(scenePos);
                    source->setSize(QSizeF(10, 30));

                    if (annotationTypeStr == ImageAnnotationStr) {
                        // 图片
                        BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
                        if (hookserver) {
                            hookserver->createImageAnnotationDialog(source);
                            QString imagePath = source->getAnnotationMap().value(Annotation::iconPath, "").toString();
                            if (!imagePath.isEmpty()) {
                                QImage img;
                                bool isLoaded = img.load(imagePath);
                                if (isLoaded) {
                                    // 如果图片大小大于画布大小，则图片按俩倍的基数缩小
                                    while (sceneRect().width() <= img.size().width()
                                           || sceneRect().height() <= img.size().height()) {
                                        img = img.scaled(img.size().width() / 2, img.height() / 2, Qt::KeepAspectRatio);
                                    }
                                    source->setSize(img.size());
                                    addAnnotationSource(source);
                                    dataPtr->annotationGraphics = nullptr;
                                }
                            }
                            if (dataPtr->canvasContext) {
                                dataPtr->canvasContext->setInteractionMode(kNormal);
                            }
                        }
                    } else {
                        addAnnotationSource(source);
                    }
                }
            }
        } else if (mode == kAreaAmplification && !dataPtr->selectStartPoint.isNull()) {
            if ((dataPtr->selectStartPoint - scenePos).manhattanLength() > 60) {
                QRectF rc(dataPtr->selectStartPoint, scenePos);
                if (rc.width() < 0 && rc.height() < 0) {
                    rc = QRectF(scenePos, dataPtr->selectStartPoint);
                } else if (rc.width() < 0) {
                    QPointF topLeft(scenePos.x(), dataPtr->selectStartPoint.y());
                    QPointF bottomRight(dataPtr->selectStartPoint.x(), scenePos.y());
                    rc = QRectF(topLeft, bottomRight);
                } else if (rc.height() < 0) {
                    QPointF topLeft(dataPtr->selectStartPoint.x(), scenePos.y());
                    QPointF bottomRight(scenePos.x(), dataPtr->selectStartPoint.y());
                    rc = QRectF(topLeft, bottomRight);
                }
                setViewRect(rc);
                dataPtr->selectStartPoint = QPointF();
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (mode == kAnnotation && dataPtr->annotationGraphics) {
            dataPtr->annotationGraphics->processMousePressEvent(event);
        }
    }

    if (isEixstBlockCtrlCode()) {
        ActionManager::getInstance().getAction(ActionManager::ShowPortName)->setVisible(true);
    } else {
        ActionManager::getInstance().getAction(ActionManager::ShowPortName)->setVisible(false);
    }
    event->accept();
}

void CanvasViewDefaultImpl::mouseDoubleClickEvent(QMouseEvent *event)
{

    if (dataPtr->annotationGraphics) {
        dataPtr->annotationGraphics->processMouseDoubleClickEvent(event);
        event->accept();
    }

    if (dataPtr->canvasContext) {
        InteractionMode mode = dataPtr->canvasContext->getInteractionMode();
        auto flags = dataPtr->canvasContext->getStatusFlags();
        bool isRunning = flags.testFlag(CanvasContext::kSimulationRunningStatus);
        if (mode == kNormal && items(event->pos()).size() <= 0 && dataPtr->canvasContext->canModify() && !isRunning
            && dataPtr->canvasScene->selectedItems().size() <= 0 && !dataPtr->isEditTextName
            && !dataPtr->annotationGraphics) {
            dataPtr->createBox->move(event->pos().x() - dataPtr->createBox->getInitialSize().width() / 2,
                                     event->pos().y() - dataPtr->createBox->getInitialSize().height() / 2);
            setCanCreateBlockList();
            dataPtr->createBox->setStatus(true, true);
        }
    }

    ICanvasView::mouseDoubleClickEvent(event);
}

void CanvasViewDefaultImpl::focusOutEvent(QFocusEvent *event)
{
    if (dataPtr->annotationGraphics) {
        dataPtr->annotationGraphics->setStatus(AnnotationSourceGraphicsObject::kStatusCancled);
    }
    ICanvasView::focusOutEvent(event);
}

void CanvasViewDefaultImpl::setPreviewWidget()
{
    // 创建导航预览窗口
    QSize s = size();
    dataPtr->navigationPreview = new CanvasNavigationPrview(this);
    CanvasNavigationPrview *navigationPreview = dataPtr->navigationPreview;
    if (navigationPreview) {
        navigationPreview->setScene(dataPtr->canvasScene.data());
        connect(this, SIGNAL(sceneViewRectChanged(QRectF)), navigationPreview, SLOT(onSceneViewChanged(QRectF)));
        connect(navigationPreview, SIGNAL(previewSceneRectChanged(QPointF)), this,
                SLOT(onPreviewSceneChanged(QPointF)));
        connect(navigationPreview, SIGNAL(previewSceneRectChangeFinished(QPointF)), this,
                SLOT(onPreviewSceneChangeFinished(QPointF)));
        connect(dataPtr->canvasScene.data(), SIGNAL(sceneRectChanged(const QRectF &)), navigationPreview,
                SLOT(onPreviewSceneRectChanged(const QRectF &)));
        connect(navigationPreview, &CanvasNavigationPrview::escapeKeyPressed, this,
                &CanvasViewDefaultImpl::onPreviewEscapeKeypressed);
        dataPtr->previewWidget = new KCustomDialog(tr("Navigation Window"), navigationPreview, KBaseDlgBox::NoButton);
        dataPtr->previewWidget->setObjectName("navigationPreview");
        dataPtr->previewWidget->titleBar()->setEnableDoubleClicked2FullScreen(false);
        dataPtr->previewWidget->titleBar()->setObjectName("navigationPreview");
        dataPtr->previewWidget->setParent(this);
        dataPtr->previewWidget->setBoundedByParent(true);
        dataPtr->previewWidget->resize(295, 230);
        dataPtr->previewWidget->setMinimumSize(QSize(150, 105));
        // dataPtr->previewWidget->setMaximumSize(QSize(600, 420));
        dataPtr->previewWidget->setContentsMargins(15, 0, 15, 1);
        dataPtr->previewWidget->setHiddenButtonGroup(true);
        dataPtr->previewWidget->show();
        // 平台窗口有阴影效果，而阴影效果在导航窗口会导致一些bug，此处禁用阴影效果
        auto frame = dataPtr->previewWidget->findChild<QFrame *>("frame");
        if (frame) {
            frame->setGraphicsEffect(nullptr);
        }
        if (!dataPtr->canvasContext->getShowNavigationPreview()) {
            dataPtr->previewWidget->hide();
        }
        // 导航窗口点击x关闭的时候，隐藏导航窗
        connect(dataPtr->previewWidget->titleBar(), &KTitleBar::signalButtonCloseClicked, [this] {
            if (dataPtr->canvasContext) {
                dataPtr->canvasContext->setShowNavigationPreview(false);
            }
        });
    }
}

void CanvasViewDefaultImpl::resizeEvent(QResizeEvent *event)
{
    ICanvasView::resizeEvent(event);
    auto rect = this->rect();
    if (dataPtr->toolBar != nullptr) {
        dataPtr->toolBar->move(rect.topLeft());
    }

    if (dataPtr->navigationPreview && dataPtr->isPreviewSetPos) {
        QPoint previewPos = dataPtr->previewWidget->pos();
        dataPtr->previewLastPos = dataPtr->previewWidget->pos();
        QRect previewRect = QRect(previewPos, dataPtr->previewWidget->size());
        if (!rect.contains(previewRect, true) || dataPtr->previewLastPos.y() == PREVIEW_WIDGET_Y_OFFSET) {
            QSizeF previewSize = dataPtr->previewWidget->size();
            QPoint previewPos =
                    QPoint(rect.right() - previewSize.width() - PREVIEW_WIDGET_X_OFFSET, PREVIEW_WIDGET_Y_OFFSET);
            dataPtr->previewWidget->move(previewPos);
            dataPtr->navigationPreview->viewport()->update();
            dataPtr->previewLastPos = dataPtr->previewWidget->pos();
        }

        syncPreviewRect();
    }
}

void CanvasViewDefaultImpl::keyPressEvent(QKeyEvent *event)
{
    if (dataPtr->canvasScene) {
        dataPtr->canvasScene->resetSubsystemGraphics();
    }
    if (dataPtr->createBox->isActive() && dataPtr->canvasScene->selectedItems().size() <= 0
        && !dataPtr->isEditTextName) {
        if (dataPtr->createBox->isVisible()) {
            setCanCreateBlockList();
            dataPtr->createBox->setStatus(true, true);
            return;
        } else {
            // 输入文本框没显示的时候只接受符号输入
            if (event->key() >= Qt::Key_Space && event->key() <= Qt::Key_AsciiTilde
                && event->modifiers() != Qt::ControlModifier && event->modifiers() != Qt::AltModifier) {
                setCanCreateBlockList();
                dataPtr->createBox->setStatus(true, true);
                QApplication::sendEvent(dataPtr->createBox->searchLineEdit(), event);
                return;
            }
        }
    }
    if (!dataPtr->canvasContext->canModify()) {
        // 画板冻结状态 屏蔽方向键
        if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right || event->key() == Qt::Key_Down
            || event->key() == Qt::Key_Up)
            return;
    }
    if (!this->isInteractive() && event->key() != Qt::Key_Escape) {
        return ICanvasView::keyPressEvent(event);
    }
    if (event->key() == Qt::Key_Escape) {
        QVector<ActionManager::ActionType> Temp;
        Temp << ActionManager::Icon << ActionManager::Textbox << ActionManager::Rectangle << ActionManager::Circular
             << ActionManager::ScreenDrag << ActionManager::Magnify << ActionManager::Segment << ActionManager::Arrows
             << ActionManager::Polygon << ActionManager::Arc;
        for each (auto i in Temp) {
            if (ActionManager::getInstance().getAction(i)->isChecked()) {
                ActionManager::getInstance().getAction(i)->activate(QAction::Trigger);
            }
        }
    }
    ICanvasView::keyPressEvent(event);
    syncPreviewRect();
}

void CanvasViewDefaultImpl::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

void CanvasViewDefaultImpl::enterEvent(QEvent *event) { }

bool CanvasViewDefaultImpl::viewportEvent(QEvent *event)
{
    return ICanvasView::viewportEvent(event);
}

void CanvasViewDefaultImpl::scrollContentsBy(int dx, int dy)
{
    saveCenterPos();
    syncPreviewRect();
    ICanvasView::scrollContentsBy(dx, dy);
}

void CanvasViewDefaultImpl::setSearchHighLight(QStringList &uuid)
{
    onSearchBlock(uuid);
}

void CanvasViewDefaultImpl::onVolLevelChanged(const QString &text)
{
    dataPtr->currentVolumeText = text;
    highlightElectricSource();
}

void CanvasViewDefaultImpl::onPhaseTeckChanged(const QString &text)
{
    dataPtr->currentPhapseText = text;
    highlightElectricSource();
}

void CanvasViewDefaultImpl::onSliderScaleValueChanged(int value)
{
    qreal percent = value / 100.0;
    if (dataPtr != nullptr && dataPtr->canvasContext != nullptr) {
        dataPtr->canvasContext->setScale(percent);
    }
}

void CanvasViewDefaultImpl::onSearchBlock(QStringList blockuuids)
{
    if (blockuuids.isEmpty()) {
        dataPtr->searchLineUUIDText = "";
        dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(QStringList());
        highlightElectricSource();
    } else {
        dataPtr->searchLineUUIDText = blockuuids[0];
        scrollToBlock(blockuuids[0]);
        dataPtr->canvasContext->getCanvasProperty().setHighLightSourcceList(blockuuids);
    }
}

void CanvasViewDefaultImpl::onScaleChanged(const QString &text)
{
    if (text == tr("Page")) {
        actionFitIn();
    } else {
        QString strText = text;
        strText.replace("%", "");
        bool ok = false;
        int value = strText.toInt(&ok);
        if (ok) {
            dataPtr->canvasContext->setScale(value / 100.f);
        }
    }
}

void CanvasViewDefaultImpl::setCanCreateBlockList()
{
    BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
    if (!hookserver)
        return;
    QList<BlockStruct> dataList = hookserver->getCanCreateBlockList();
    QString canvasTypeString = "Control";
    if (dataPtr->canvasContext->type() == CanvasContext::kElectricalType
        || dataPtr->canvasContext->type() == CanvasContext::kElecUserDefinedType) {
        canvasTypeString = "Electrical";
    }
    dataPtr->createBox->setBlockList(canvasTypeString, dataList);
}

QSharedPointer<GraphicsLayerManager> CanvasViewDefaultImpl::getGraphicsLayerManager()
{
    if (dataPtr->canvasScene) {
        return dataPtr->canvasScene->getGraphicsLayerManager();
    }
    return QSharedPointer<GraphicsLayerManager>();
}

void CanvasViewDefaultImpl::transformItemGroupClear()
{
    if (dataPtr->canvasScene) {
        TransformItemGroup *itemGroup = dataPtr->canvasScene->getTransformItemGroup();
        if (itemGroup) {
            itemGroup->clear();
        }
    }
}

void CanvasViewDefaultImpl::onCreateBlock(QString protype, QString text)
{
    if (protype != "text") {
        BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
        if (!hookserver)
            return;
        hookserver->createNewBlock(protype, mapToScene(dataPtr->clickPos), true, QString());
        dataPtr->createBox->setStatus(false, false);
    } else {
        if (!dataPtr->canvasContext->getCanvasProperty().isAnnotationVisible()) {
            QString tipinfo = tr("Annotation is not visible,Modify annotation visibility to true");
            // 提示用户是否修改资源为可见
            if (KMessageBox::question(tipinfo, KMessageBox::Yes | KMessageBox::No) == KMessageBox::Yes) {
                dataPtr->canvasContext->getCanvasProperty().setAnnotationVisible(true);
            } else {
                dataPtr->createBox->setStatus(false, false);
                return;
            }
        }
        QPointF centerPoint = checkEdge(mapToScene(dataPtr->clickPos));
        PSourceProxy source = PSourceProxy(new SourceProxy());
        source->setModuleType(GKD::SOURCE_MODULETYPE_ANNOTATION);
        source->setPrototypeName(TextAnnotationStr);
        source->setPos(centerPoint);
        source->setSize(QSizeF(0, 0));
        QVariantMap properties;
        properties[Annotation::content] = text;
        QFont font;
        font.setFamily("Microsoft YaHei UI");
        font.setBold(false);
        font = QFont(font, QApplication::desktop());
        properties[Annotation::textFont] = font;
        properties[Annotation::alignment] = 1;
        properties[Annotation::segmentColor] = QColor(GKD::WIRE_NORMAL_COLOR);
        properties[Annotation::textColor] = QColor("black");
        properties[Annotation::isShowframe] = false;
        properties[Annotation::segmentWidth] = 3;
        source->resetAnnotationMap(properties);
        addAnnotationSource(source);
        dataPtr->annotationGraphics->setStatus(AnnotationSourceGraphicsObject::kStatusFinished);
        dataPtr->createBox->setStatus(false, false);
    }
}

void CanvasViewDefaultImpl::onEditTextName()
{
    dataPtr->isEditTextName = true;
}

void CanvasViewDefaultImpl::onFinishEditTextName()
{
    dataPtr->isEditTextName = false;
}

void CanvasViewDefaultImpl::onBookMarkNumberLimit()
{
    int number = BookMark::getInstance().getBookMarkNumbee();
    if (number > BookMark::getInstance().getBookMarkMaxLimit()) {
        ActionManager::getInstance().getAction(ActionManager::AddBookmark)->setEnabled(false);
    } else {
        ActionManager::getInstance().getAction(ActionManager::AddBookmark)->setEnabled(true);
    }
}

void CanvasViewDefaultImpl::onCanvasInteractionModeChanged(QString canvasUUID, int newMode, int oldMode)
{
    InteractionMode mode = (InteractionMode)newMode;
    if (mode != kNormal) {
        dataPtr->createBox->setStatus(false, false);
    }
    switch (mode) {
    case kNormal: {
        viewport()->unsetCursor();
        setDragMode(QGraphicsView::RubberBandDrag);
    } break;
    case kAreaAmplification:
        viewport()->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorZoomIn")));
        setDragMode(QGraphicsView::RubberBandDrag);
        break;
    case kHandDrag: {
        setDragMode(QGraphicsView::ScrollHandDrag);
    } break;
    case kLock:
        viewport()->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorDisable")));
        break;
    case kAnnotation: {
        // 注解模式时，不允许拖动画板
        setDragMode(NoDrag);
    } break;

    default: {
    }
    }
}

void CanvasViewDefaultImpl::onCanvasStatusFlagsChanged(CanvasContext::StatusFlag flag)
{
    if (!dataPtr->canvasContext) {
        return;
    }
    auto flags = dataPtr->canvasContext->getStatusFlags();
    ActionManager &actionMgr = ActionManager::getInstance();

    QString activateCanvasUUID = actionMgr.getActivateCanvasUUID();

    if (dataPtr->canvasContext->uuid() != activateCanvasUUID) {
        return;
    }

    bool isSimulationRunning = flags.testFlag(CanvasContext::kSimulationRunningStatus);
    bool isWindowActivated = flags.testFlag(CanvasContext::kActiveWindow);
    bool isProjectActivaed = flags.testFlag(CanvasContext::kProjectActivate);
    bool isNextConstructiveBorad = flags.testFlag(CanvasContext::kNextConstructiveBorad);

    if (isSimulationRunning || !isWindowActivated || isNextConstructiveBorad) {
        // 界面相关更新
        if (!isWindowActivated) { // 运行状态下不清楚模块选中状态，否则会导致运行状态下无法响应一些模块的action
            dataPtr->canvasScene->clearSelection();
        }

        dataPtr->createBox->setStatus(false, false);

        dataPtr->toolBar->setComboBoxEnableState(ViewToolBar::DefaultPhaseTechnologies, false);
        dataPtr->toolBar->setComboBoxEnableState(ViewToolBar::DefaultVoltageLevel, false);

        dataPtr->toolBar->getBookMarkButton()->setEnabled(false);

        // action更新
        actionMgr.getAction(ActionManager::Undo)->setEnabled(false);
        actionMgr.getAction(ActionManager::Redo)->setEnabled(false);

        actionMgr.getCommonGroup()->setEnabled(false);
        actionMgr.getTransformActionGroup()->setEnabled(false);
        actionMgr.getAnnotationGroup()->setEnabled(false);
        actionMgr.getCopyAbleActionGroup()->setEnabled(false);
        actionMgr.getOperateActionGroup()->setEnabled(false);
        actionMgr.getCanvasActionGroup()->setEnabled(false);
        actionMgr.getAction(ActionManager::Lock)->setEnabled(false);
        actionMgr.getAction(ActionManager::LockReplica)->setEnabled(false);
        if (!isWindowActivated) {
            setInteractive(false);
        } else {
            setInteractive(true);
        }

        setLockStatusIcon(true);
        actionMgr.getAction(ActionManager::Lock)->setChecked(true);

    } else {
        dataPtr->toolBar->getBookMarkButton()->setEnabled(true);
        actionMgr.getCommonGroup()->setEnabled(true);
        actionMgr.getTransformActionGroup()->setEnabled(true);
        actionMgr.getAnnotationGroup()->setEnabled(true);
        actionMgr.getCopyAbleActionGroup()->setEnabled(true);
        actionMgr.getOperateActionGroup()->setEnabled(true);
        actionMgr.getCanvasActionGroup()->setEnabled(true);

        if (!isSimulationRunning || !isNextConstructiveBorad) {
            actionMgr.getAction(ActionManager::Lock)->setEnabled(true);
            actionMgr.getAction(ActionManager::LockReplica)->setEnabled(true);
        }
        setInteractive(true);

        bool hasItemSelected = flags.testFlag(CanvasContext::kItemSelectedStatus);
        if (hasItemSelected) {
            actionMgr.getCopyAbleActionGroup()->setEnabled(true);
            actionMgr.getTransformActionGroup()->setEnabled(true);
        } else {
            actionMgr.getCopyAbleActionGroup()->setEnabled(false);
            actionMgr.getTransformActionGroup()->setEnabled(false);
        }

        bool isLock = flags.testFlag(CanvasContext::kLockedStatus);
        if (isLock || !isProjectActivaed) {
            // 锁定和画板未激活的状态是一致的
            // 锁定状态下不显示网格

            dataPtr->createBox->setStatus(false, false);

            dataPtr->toolBar->setComboBoxEnableState(ViewToolBar::DefaultPhaseTechnologies, false);
            dataPtr->toolBar->setComboBoxEnableState(ViewToolBar::DefaultVoltageLevel, false);

            actionMgr.getAction(ActionManager::Undo)->setEnabled(false);
            actionMgr.getAction(ActionManager::Redo)->setEnabled(false);

            actionMgr.getAnnotationGroup()->setEnabled(false);
            actionMgr.getCopyAbleActionGroup()->setEnabled(false);
            actionMgr.getTransformActionGroup()->setEnabled(false);

            actionMgr.getAction(ActionManager::Delete)->setEnabled(false);

            actionMgr.getAction(ActionManager::updateModule)->setEnabled(false);

            actionMgr.getAction(ActionManager::Lock)->setChecked(true);
            setLockStatusIcon(true);
            actionMgr.getAction(ActionManager::LockReplica)->setText(QObject::tr("Canvas Unlock"));
            if (!isProjectActivaed) {
                // 画板未激活，
                actionMgr.getAction(ActionManager::Lock)->setEnabled(true);
                actionMgr.getAction(ActionManager::LockReplica)->setEnabled(false);
            }
        } else {

            dataPtr->toolBar->setComboBoxEnableState(ViewToolBar::DefaultPhaseTechnologies, true);
            dataPtr->toolBar->setComboBoxEnableState(ViewToolBar::DefaultVoltageLevel, true);

            auto undoStack = dataPtr->canvasScene->getUndoStack();
            if (undoStack) {
                ActionManager &actionMgr = ActionManager::getInstance();
                actionMgr.getAction(ActionManager::Undo)->setEnabled(undoStack->canUndo());
                actionMgr.getAction(ActionManager::Redo)->setEnabled(undoStack->canRedo());
            }

            actionMgr.getAnnotationGroup()->setEnabled(true);
            actionMgr.getAction(ActionManager::Lock)->setEnabled(true);
            actionMgr.getAction(ActionManager::Lock)->setChecked(false);
            setLockStatusIcon();
            actionMgr.getAction(ActionManager::LockReplica)->setEnabled(true);
            actionMgr.getAction(ActionManager::LockReplica)->setText(QObject::tr("Canvas Lock"));
            actionMgr.getAction(ActionManager::updateModule)->setEnabled(true);

            bool itemDeletable = flags.testFlag(CanvasContext::kItemCanDeleteStatus);
            if (itemDeletable) {
                actionMgr.getAction(ActionManager::Delete)->setEnabled(true);
            }
        }
    }
    actionMgr.onClipboardDataChanged();
    if (isNextConstructiveBorad) {
        actionMgr.getAction(ActionManager::Lock)->setEnabled(false);
        actionMgr.getAction(ActionManager::Lock)->setChecked(false);
    }
    dataPtr->canvasScene->invalidate(dataPtr->canvasScene->sceneRect(), QGraphicsScene::BackgroundLayer);
}

void CanvasViewDefaultImpl::onPreviewEscapeKeypressed(QEvent *e)
{
    if (dataPtr->isFullScreen) {
        QKeyEvent escPress(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        auto p = parent();
        if (p) {
            QApplication::sendEvent(p->parent(), &escPress);
        }
    }
}

void CanvasViewDefaultImpl::processAction(ActionManager::ActionType type, bool checked)
{
    if (!dataPtr->canvasContext) {
        return;
    }
    if (dataPtr->canvasScene) {
        dataPtr->canvasScene->resetSubsystemGraphics();
    }
    switch (type) {
    case ActionManager::ZoomIn: {
        actionZoomInOut(true);
    } break;
    case ActionManager::ZoomOut: {
        actionZoomInOut(false);
    } break;
    case ActionManager::Magnify: {
        auto mode = dataPtr->canvasContext->getInteractionMode();
        if (mode != kAreaAmplification) {
            dataPtr->canvasContext->setInteractionMode(kAreaAmplification);
        } else {
            dataPtr->canvasContext->setInteractionMode(kNormal);
        }
        viewport()->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorZoomIn")));
    } break;
    case ActionManager::FitIn: {
        // 自适应大小
        actionFitIn();
    } break;
    case ActionManager::FullScreen: {
        if (dataPtr->canvasContext->getInteractionMode() == kHandDrag) {
            dataPtr->canvasContext->setInteractionMode(kNormal);
        }
        BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
        if (hookserver) {
            hookserver->workAreaInFullScreenChange(dataPtr->canvasContext->name());
        }
    } break;
    // 注解操作
    case ActionManager::Textbox:
    case ActionManager::Icon:
    case ActionManager::Rectangle:
    case ActionManager::Circular:
    case ActionManager::Segment:
    case ActionManager::BrokenLine:
    case ActionManager::Arrows:
    case ActionManager::LineArrow:
    case ActionManager::Polygon:
    case ActionManager::Arc: {
        if (dataPtr->canvasContext) {
            dataPtr->canvasContext->setInteractionMode(kAnnotation);
        }
        updateAnnotationCursor(type);

    } break;

    case ActionManager::ScreenDrag: {
        // 画板拖拽
        auto mode = dataPtr->canvasContext->getInteractionMode();
        if (mode != kHandDrag) {
            dataPtr->canvasContext->setInteractionMode(kHandDrag);
        } else {
            dataPtr->canvasContext->setInteractionMode(kNormal);
        }
    } break;
    case ActionManager::Lock: {
        auto flag = dataPtr->canvasContext->getStatusFlags();
        if (flag) {
            bool isOk = flag.testFlag(CanvasContext::kProjectActivate);
            if (!isOk) {
                blog(tr("Unlock failed, please activate the current drawing board!"), -1); // 解冻失败，请激活当前画板!
            }
        }
        dataPtr->canvasContext->setLocked(checked);
    } break;
    case ActionManager::LockReplica: {
        bool lockStatus = dataPtr->canvasContext->getLocked();
        dataPtr->canvasContext->setLocked(!lockStatus);

    } break;
    case ActionManager::DispNavigationPane: {
        // 显示导航窗口
        bool flag = dataPtr->canvasContext->getShowNavigationPreview();
        dataPtr->canvasContext->setShowNavigationPreview(!flag);
    } break;
    case ActionManager::BreakAllLinkLine: {
        // 断开连接线
        dataPtr->canvasScene->deleteAllConnectorWireGraphicsObject();
    } break;
    case ActionManager::SaveImage: {
        // 导出为图片
        ExportImage exportImageWidget(dataPtr->canvasContext, this);
        exportImageWidget.CreatSaveImage();
        QTimer::singleShot(100, this, SLOT(onActiveProgram())); // 延迟触发程序全屏窗口
    } break;
    case ActionManager::AddBookmark: {
        // 添加书签
        BookMark::getInstance().setCanvasContext(dataPtr->canvasContext);
        BookMark::getInstance().actionCreateAddBookmark();
    } break;
    case ActionManager::ManageBookmark: {
        // 管理书签
        BookMark::getInstance().actionCreateManageBookmark();
    } break;
    case ActionManager::ShowCoverage: {
        // 显示图例
        bool isVisible = dataPtr->canvasContext->getCanvasProperty().isLegendVisible();
        dataPtr->canvasContext->getCanvasProperty().setLegendVisible(!isVisible);
    } break;
    case ActionManager::ShowPortName: {
        // 显示隐藏端口
        actionShowAlonePortName();
    } break;
    case ActionManager::ExportASPreviewSVG: {
        // 导出为项目的预览SVG图
        auto businessHook = dataPtr->canvasContext->getBusinessHooksServer();
        if (businessHook) {
            QString svgFilePath = businessHook->getCurrentProjectDir();
            svgFilePath += "/preview.svg";
            saveAsSVGFile(svgFilePath);
        }

    } break;
    case ActionManager::PrintErrorInfos: {
        auto businessHook = dataPtr->canvasContext->getBusinessHooksServer();
        if (businessHook) {
            businessHook->PrintErrorInfos();
        }
        // #ifdef _DEBUG
        //     QString algorithmName = dataPtr->canvasContext->connectorWireAlgorithmName();
        //     if (algorithmName == ManhattaAlgorithmName) {
        //         dataPtr->canvasContext->setConnectorWireAlgorithmName(BezierAlogrithmName);
        //     } else {
        //         dataPtr->canvasContext->setConnectorWireAlgorithmName(ManhattaAlgorithmName);
        //     }
        // #endif

    } break;
    default: {
        if (dataPtr->canvasScene) {
            dataPtr->canvasScene->processAction(type);
        }
    }
    }
}

void CanvasViewDefaultImpl::updateAnnotationCursor(ActionManager::ActionType type)
{
    switch (type) {
    case ActionManager::Textbox:
        viewport()->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorInput")));
        break;
    case ActionManager::Icon:
        viewport()->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorSelected")));
        break;

    case ActionManager::Rectangle:
    case ActionManager::Circular:
    case ActionManager::Segment:
    case ActionManager::Arrows:
    case ActionManager::Arc:
    case ActionManager::BrokenLine:
    case ActionManager::LineArrow:
    case ActionManager::Polygon:
        viewport()->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorShape")));
        break;
    default: {
    }
    }
}

void CanvasViewDefaultImpl::registerAction()
{
    // 注册aciton
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Copy));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Cut));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Delete));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Paste));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Undo));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Redo));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::ZoomIn));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::ZoomOut));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::RotateClockwise));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::Rotate180));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::RotateAntiClockwise));
    this->addAction(ActionManager::getInstance().getAction(ActionManager::DrawBoardProperty));
}

void CanvasViewDefaultImpl::subsystemInitialize()
{
    /// 子系统中输入输出位置调整
    if (!dataPtr->canvasContext->isSubsystemInitialized()) {
        QMap<QString, QSharedPointer<ConnectorWireContext>> allConnectorWireContext =
                dataPtr->canvasContext->getAllConnectorWireContext();
        QMapIterator<QString, PConnectorWireContext> iterConnector(allConnectorWireContext);
        while (iterConnector.hasNext()) {
            iterConnector.next();
            PConnectorWireContext ctx = iterConnector.value();
            if (!ctx) {
                continue;
            }
            auto srcSourceProxy = ctx->srcSource();
            auto dstSourceProxy = ctx->dstSource();
            if (!srcSourceProxy || !dstSourceProxy) {
                continue;
            }
            QString srcPrototypeName = srcSourceProxy->prototypeName();
            QString dstPrototypeName = dstSourceProxy->prototypeName();
            bool isSrcInOut = (srcPrototypeName == "In" || srcPrototypeName == "Out");
            bool isDstInOut = (dstPrototypeName == "In" || dstPrototypeName == "Out");
            if (isSrcInOut && isDstInOut) {
                continue;
            }
            if (isSrcInOut || isDstInOut) {
                // 只有一端是输入或者输出
                ConnectorWireGraphicsObject *wire = dataPtr->canvasScene->getConnectorWireGraphicsByID(ctx->uuid());
                if (wire) {
                    auto startPortGraphics = wire->getStartPortGraphics();
                    auto endPortGraphics = wire->getEndPortGraphics();
                    if (startPortGraphics && endPortGraphics) {
                        PortGraphicsObject *referPortGraphics = nullptr;
                        TransformProxyGraphicsObject *referProxyGraphics = nullptr;

                        if (isSrcInOut) {
                            referPortGraphics = endPortGraphics;
                            referProxyGraphics = startPortGraphics->getTransformProxyGraphicsObject();
                        } else {
                            referPortGraphics = startPortGraphics;
                            referProxyGraphics = endPortGraphics->getTransformProxyGraphicsObject();
                        }
                        if (!referPortGraphics || !referProxyGraphics) {
                            continue;
                        }
                        Qt::AnchorPoint portAnchor = referPortGraphics->getAnchorPoint();
                        auto referPortContext = referPortGraphics->getPortContext();
                        if (!referPortContext) {
                            continue;
                        }
                        QString referPortType = referPortContext->type();
                        QPointF oldPos = referProxyGraphics->pos();
                        if (portAnchor == Qt::AnchorLeft) {
                            if (referPortType == PortOutputType) {
                                referProxyGraphics->rotateOnCenter(180);
                                oldPos.setX(referPortGraphics->getLinkPos().x() - 200);
                            }
                            oldPos.setY(referPortGraphics->getLinkPos().y() - 10);
                        } else if (portAnchor == Qt::AnchorRight) {
                            if (referPortType == PortInputType) {
                                referProxyGraphics->rotateOnCenter(180);
                                oldPos.setX(referPortGraphics->getLinkPos().x() + 100);
                            }
                            oldPos.setY(referPortGraphics->getLinkPos().y() - 10);
                        } else if (portAnchor == Qt::AnchorTop) {
                            if (referPortType == PortOutputType) {
                                referProxyGraphics->rotateOnCenter(-90);
                            } else {
                                referProxyGraphics->rotateOnCenter(90);
                            }
                            oldPos.setY(referPortGraphics->getExternalPos().y() - 100);
                            oldPos.setX(referPortGraphics->getExternalPos().x() - 20);
                        } else if (portAnchor == Qt::AnchorBottom) {
                            if (referPortType == PortInputType) {
                                referProxyGraphics->rotateOnCenter(-90);
                            } else {
                                referProxyGraphics->rotateOnCenter(90);
                            }
                            oldPos.setY(referPortGraphics->getExternalPos().y() + 100);
                            oldPos.setX(referPortGraphics->getExternalPos().x() - 20);
                        }
                        referProxyGraphics->setPos(oldPos);
                        referProxyGraphics->getSourceProxy()->setPos(oldPos);
                    }
                }
            }
        }
        dataPtr->canvasContext->setSubsystemInitialized(true);
    }
}

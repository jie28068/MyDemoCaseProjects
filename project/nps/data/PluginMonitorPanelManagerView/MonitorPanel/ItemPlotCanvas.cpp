#include "ItemPlotCanvas.h"
#include "ItemPlot.h"
#include "MonitorPanelPlotServer.h"
#include "PlotWidget.h"
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QtConcurrent>

USE_LOGOUT_("ItemPlotCanvas")

ItemPlotCanvas::ItemPlotCanvas(QwtPlot *plot)
    : QwtPlotCanvas(plot),
      // m_isLeftButtonPress(false),
      m_isCtrlPress(false),
      m_isAllowSelectXAxis(false),
      m_isAllowSelectYAxis(false),
      m_wheelZoomValue(1.0f),
      m_dataAnalyseTreeView(nullptr),
      m_menu(nullptr),
      m_dataAnalyseAction(nullptr),
      m_maxLegendWidth(0),
      m_isNotNeedRecord(false),
      m_mousePos(QPointF(INVALID_VALUE, INVALID_VALUE)),
      m_isMouseFollow(false),
      m_currentMarker(nullptr),
      m_xDecimal(-1),
      m_yDecimal(-1),
      m_dataAnalyseObserver(nullptr)
{
    connect(&mFutureWatcher, &QFutureWatcher<QString>::finished, [&]() {
        QApplication::clipboard()->setText(mFutureWatcher.future().result(), QClipboard::Clipboard);
        LOGOUT(tr("Copy complete"), LOG_NORMAL);
    });
    mInitCoordinate = new QAction(tr("Init Coordinate"), this);
    mCopyAllData = new QAction(tr("All"), this);
    mCopyVisualData = new QAction(tr("Visual Area"), this);
    connect(mInitCoordinate, &QAction::triggered, this, &ItemPlotCanvas::onInitCoordinate);
    connect(mCopyAllData, &QAction::triggered, this, &ItemPlotCanvas::onCopyAllDataToClipBoard);
    connect(mCopyVisualData, &QAction::triggered, this, &ItemPlotCanvas::onCopyVisualAreaDataToClipBoard);
    mMainMenu = new QMenu(this);
    mMainMenu->addAction(mInitCoordinate);
    mSubMenu = new QMenu(tr("Copy Data To ClipBoard"), mMainMenu);
    mSubMenu->addAction(mCopyAllData);
    mSubMenu->addAction(mCopyVisualData);
    mMainMenu->addMenu(mSubMenu);

    m_selectRect.types = 0;
    m_tempSelectRect.types = 0;
    setMouseTracking(true);
    // 设置画布相关属性
    setFrameStyle(QFrame::StyledPanel | QFrame::Panel);
    setLineWidth(1);
    setPalette(Qt::white);

    m_dataAnalyseTreeView = new DataAnalyseTreeView(this);
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(m_dataAnalyseTreeView);
    m_dataAnalyseTreeView->setGraphicsEffect(effect);
    effect->setOpacity(0.8);

    m_dataAnalyseTreeView->hide();
    m_dataAnalyseTreeView->installEventFilter(this);
    this->setStyleSheet("ItemPlotCanvas{border:1px solid #333333;}");
    mPProjectMng = RequestServer<IProjectManagerServer>();
}

ItemPlotCanvas::~ItemPlotCanvas() { }

void ItemPlotCanvas::addMarker(const MarkerInfo &info)
{
    MarkerInfo info_tmp = info;
    // info_tmp.xBottomIsLog = m_xBottomIsLog;
    emit addMarkerSignal(info_tmp);
    ItemPlotMarker *marker = new ItemPlotMarker(info_tmp, this);
    m_markerList << marker;
    update();
}

void ItemPlotCanvas::setMarkerInfo(const QList<MarkerInfo> &infos)
{
    emit setMarkerInfoSignal(infos);
    m_markerList.clear();
    for (const MarkerInfo &info : infos) {
        ItemPlotMarker *marker = new ItemPlotMarker(info, this);
        m_markerList << marker;
    }
    update();
}

QList<MarkerInfo> ItemPlotCanvas::getMarkerInfo(void) const
{
    QList<MarkerInfo> infos;
    for (ItemPlotMarker *marker : m_markerList) {
        MarkerInfo info = marker->getMarkerInfo();
        infos.append(info);
    }
    return infos;
}

void ItemPlotCanvas::refreshAllMarkerPoint(void)
{
    for (ItemPlotMarker *marker : m_markerList) {
        marker->refresh(true);
    }
    updateDataAnalyseDataInfo();
}

void ItemPlotCanvas::drawSelected(const AxisRanges &rect)
{
    m_selectRect = rect;
    update();
}

void ItemPlotCanvas::drawMouseMovePoint(QPointF &point)
{
    m_mousePos = point;
    if (point.x() == INVALID_VALUE || point.y() == INVALID_VALUE) {
        m_selectPointList.clear();
        update();
        return;
    }
    getPlotCurveDataNearPoint(point);
    update();
}

void ItemPlotCanvas::zoomSelected(const AxisRanges &rect /*,bool bRecord*/)
{
    m_selectRect = rect;
    if (!m_isAllowSelectXAxis && !m_isAllowSelectYAxis) {
        return;
    } else if (m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
        if (m_selectRect.xBottomMin == m_selectRect.xBottomMax || m_selectRect.yLeftMin == m_selectRect.yLeftMax) {
            clearSelected();
            return;
        }
    } else if (m_isAllowSelectXAxis && !m_isAllowSelectYAxis) {
        if (m_selectRect.xBottomMin == m_selectRect.xBottomMax) {
            clearSelected();
            return;
        }
    } else if (!m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
        if (m_selectRect.yLeftMin == m_selectRect.yLeftMax) {
            clearSelected();
            return;
        }
    }

    if (m_isAllowSelectXAxis) {
        qreal min;
        qreal max;
        if (m_selectRect.xBottomMin > m_selectRect.xBottomMax) {
            min = m_selectRect.xBottomMax;
            max = m_selectRect.xBottomMin;
        } else {
            min = m_selectRect.xBottomMin;
            max = m_selectRect.xBottomMax;
        }
        if (max - min < 0.00000001)
            max = min + 0.00000001;
        setPlotCurveDataSelectRange(min, max);
        setAxisScale(QwtPlot::xBottom, min, max);
    }

    if (m_isAllowSelectYAxis) {
        qreal min;
        qreal max;
        if (m_selectRect.yLeftMin > m_selectRect.yLeftMax) {
            min = m_selectRect.yLeftMax;
            max = m_selectRect.yLeftMin;
        } else {
            min = m_selectRect.yLeftMin;
            max = m_selectRect.yLeftMax;
        }
        if (max - min < 0.00000001)
            max = min + 0.00000001;
        setAxisScale(QwtPlot::yLeft, min, max);

        if (m_selectRect.types & AxisType_yRight && this->plot()->axisEnabled(QwtPlot::yRight)) {
            if (m_selectRect.yRightMin > m_selectRect.yRightMax) {
                min = m_selectRect.yRightMax;
                max = m_selectRect.yRightMin;
            } else {
                min = m_selectRect.yRightMin;
                max = m_selectRect.yRightMax;
            }
            if (max - min < 0.00000001)
                max = min + 0.00000001;
            setAxisScale(QwtPlot::yRight, min, max);
        }
    }

    clearSelected();

    m_wheelZoomValue = 1.0f;
    plot()->replot();
}

void ItemPlotCanvas::wheelZoom(bool b)
{
    AxisRanges oldRect;
    AxisRanges newRect;
    oldRect.xBottomMin = (plot()->axisScaleDiv(QwtPlot::xBottom)).lowerBound();
    oldRect.xBottomMax = (plot()->axisScaleDiv(QwtPlot::xBottom)).upperBound();
    oldRect.yLeftMin = (plot()->axisScaleDiv(QwtPlot::yLeft)).lowerBound();
    oldRect.yLeftMax = (plot()->axisScaleDiv(QwtPlot::yLeft)).upperBound();
    if (plot()->axisEnabled(QwtPlot::yRight)) {
        oldRect.yRightMin = (plot()->axisScaleDiv(QwtPlot::yRight)).lowerBound();
        oldRect.yRightMax = (plot()->axisScaleDiv(QwtPlot::yRight)).upperBound();
    }
    // todo:加xtop坐标的处理，暂时没用

    // 这几步主要用于缩放可以还原到原来
    if (qAbs(m_wheelZoomValue - 1.0f) < 0.0001f) {
        m_wheelZoomRect = oldRect;
    }

    if (b)
        m_wheelZoomValue *= ZOOMIN_VALUE;
    else
        m_wheelZoomValue *= ZOOMOUT_VALUE;

    if (qAbs(m_wheelZoomValue - 1.0f) > 0.0001f) {

        ////////////////////////////////////////
        double centerPos, nLen;
        centerPos = (oldRect.xBottomMax + oldRect.xBottomMin) / 2;
        nLen = (oldRect.xBottomMax - oldRect.xBottomMin) / 2;
        if (b)
            nLen *= ZOOMIN_VALUE;
        else
            nLen *= ZOOMOUT_VALUE;

        if (nLen < 0.00000001)
            nLen = 0.00000001;

        newRect.xBottomMin = centerPos - nLen;
        newRect.xBottomMax = centerPos + nLen;

        centerPos = (oldRect.yLeftMax + oldRect.yLeftMin) / 2;
        nLen = (oldRect.yLeftMax - oldRect.yLeftMin) / 2;
        if (b)
            nLen *= ZOOMIN_VALUE;
        else
            nLen *= ZOOMOUT_VALUE;

        if (nLen < 0.00000001)
            nLen = 0.00000001;

        newRect.yLeftMax = (centerPos + nLen);
        newRect.yLeftMin = (centerPos - nLen);

        if (plot()->axisEnabled(QwtPlot::yRight)) {
            centerPos = (oldRect.yRightMax + oldRect.yRightMin) / 2;
            nLen = (oldRect.yRightMax - oldRect.yRightMin) / 2;
            if (b)
                nLen *= ZOOMIN_VALUE;
            else
                nLen *= ZOOMOUT_VALUE;

            if (nLen < 0.00000001)
                nLen = 0.00000001;
            newRect.yRightMax = (centerPos + nLen);
            newRect.yRightMin = (centerPos - nLen);
        }
        // todo:加xtop坐标的处理，暂时没用
    } else {
        m_wheelZoomValue = 1.0f;
        newRect = m_wheelZoomRect;
    }

    if (m_isAllowSelectXAxis && !m_isAllowSelectYAxis) {
        m_tempSelectRect.xBottomMin = newRect.xBottomMin;
        m_tempSelectRect.xBottomMax = newRect.xBottomMax;
        m_tempSelectRect.yLeftMax = oldRect.yLeftMax;
        m_tempSelectRect.yLeftMin = oldRect.yLeftMin;

        m_tempSelectRect.yRightMax = oldRect.yRightMax;
        m_tempSelectRect.yRightMin = oldRect.yRightMin;

        // todo: 加xtop
    }

    if (!m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
        m_tempSelectRect.xBottomMin = oldRect.xBottomMin;
        m_tempSelectRect.xBottomMax = oldRect.xBottomMax;
        m_tempSelectRect.yLeftMax = newRect.yLeftMax;
        m_tempSelectRect.yLeftMin = newRect.yLeftMin;

        m_tempSelectRect.yRightMax = newRect.yRightMax;
        m_tempSelectRect.yRightMin = newRect.yRightMin;

        // todo: 加xtop
    } else if (m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
        m_tempSelectRect.xBottomMin = newRect.xBottomMin;
        m_tempSelectRect.xBottomMax = newRect.xBottomMax;
        m_tempSelectRect.yLeftMax = newRect.yLeftMax;
        m_tempSelectRect.yLeftMin = newRect.yLeftMin;

        m_tempSelectRect.yRightMax = newRect.yRightMax;
        m_tempSelectRect.yRightMin = newRect.yRightMin;

        // todo: 加xtop
    }

    if (!b && m_isAllowSelectXAxis) {
        double xlim = 0;
        ItemPlot *parentPlot = (ItemPlot *)plot();
        QReadWriteLock &plotLock = parentPlot->getPlotLock();
        plotLock.lockForRead();
        if (parentPlot->getPlotInfo().XType == 0) {
            xlim = 1;
        } else {
            if (mPProjectMng) {
                xlim = mPProjectMng->GetProjectConfig(KL_PRO::STEP_SIZE).toDouble();
            }
        }
        plotLock.unlock();
        if (qAbs(m_tempSelectRect.xBottomMax - m_tempSelectRect.xBottomMin) < xlim) {
            return;
        }
    }

    if (m_isAllowSelectYAxis) {
        if (qAbs(m_tempSelectRect.yLeftMax - m_tempSelectRect.yLeftMin) < 0.00000001) {
            return;
        }
    }

    if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {
        m_tempSelectRect.types = AxisType_xBottom;
        m_tempSelectRect.types |= AxisType_yLeft;
        if (plot()->axisEnabled(QwtPlot::yRight)) {
            m_tempSelectRect.types |= AxisType_yRight;
        }

        QMap<QString, QVariant> paramMap;
        paramMap["SelectRect"].setValue(m_tempSelectRect);
        emit notifyCommand(ItemPlotCanvasCommand_Selected, paramMap);
    }
}

void ItemPlotCanvas::resizeEvent(QResizeEvent *e)
{
    int w = width();
    int h = height();
    if (w > 200)
        w = 200;

    QRect rc(width() - w, 0, w, h);
    m_dataAnalyseTreeView->setGeometry(rc);

    for (ItemPlotMarker *marker : m_markerList) {
        marker->resize();
    }
}

// void ItemPlotCanvas::contextMenuEvent(QContextMenuEvent *e)
//{
//	m_menu->popup(QCursor::pos());
// }

bool ItemPlotCanvas::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_dataAnalyseTreeView) {
        switch (e->type()) {
        case QEvent::Wheel:
        case QEvent::MouseButtonPress:
            e->accept();
            return true;
        default:
            break;
        }
    }

    return QwtPlotCanvas::eventFilter(obj, e);
}

void ItemPlotCanvas::setAxisScale(QwtPlot::Axis axis, double nMin, double nMax)
{
    if (axis == QwtPlot::yLeft && (PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
        if (m_PlotInfo.id == "IMPEDANCE_ANALYSIS:Phase") // 绘制阻抗分析相位图时，指定其Y轴刻度间隔为45。
        {
            // plot()->setAxisMaxMajor(QwtPlot::yLeft, 30);
            plot()->setAxisScale(axis, nMin, nMax, 45); // 相位图

            refreshAllMarkerPoint();
            return;
        }
    }

    plot()->setAxisScale(axis, nMin, nMax);

    // foreach(ItemPlotMarker* marker, m_markerList)
    //{
    //	if(/*marker->markerType() == MarkerType::VLine && */marker->isShow())
    //	{
    //		marker->refresh(true);
    //	}
    // }
    refreshAllMarkerPoint();
}

void ItemPlotCanvas::updateDataAnalyseDataInfo(void)
{
    if (!m_dataAnalyseObserver)
        return;

    QList<DataAnalyseInfo> infos;
    QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
    if (items.size() == 0) {
        m_dataAnalyseObserver->update(infos);
        return;
    }
    for (QwtPlotItemIterator iter = items.begin(); iter != items.end(); iter++) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(*iter);
        if (!curve)
            continue;
        ItemPlotCurveSeriesData *curveData = curve->curveData();
        DataAnalyseInfo info = curveData->getDataAnalyseInfo();
        info.curveName = curve->title().text();
        info.curveColor = curve->pen().color();
        infos.append(info);
    }

    m_dataAnalyseObserver->update(infos);
}

void ItemPlotCanvas::onDataAnalyseActionTriggered(void)
{
    m_dataAnalyseTreeView->model()->clear();
    QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
    if (items.size() == 0) {
        m_dataAnalyseTreeView->model()->refresh();
        return;
    }
    for (QwtPlotItemIterator iter = items.begin(); iter != items.end(); iter++) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(*iter);
        if (!curve)
            continue;
        QReadWriteLock &curveLock = curve->getCurveLock();
        curveLock.lockForRead();
        ItemPlotCurveSeriesData *curveData = curve->curveData();
        DataAnalyseInfo info = curveData->getDataAnalyseInfo();
        curveLock.unlock();
        m_dataAnalyseTreeView->model()->addDateAnalyseInfo(curve->title().text(), info);
    }
    m_dataAnalyseTreeView->model()->refresh();
    m_dataAnalyseTreeView->expandAll();

    m_dataAnalyseTreeView->show();
    m_dataAnalyseTreeView->setFocus();
}

void ItemPlotCanvas::onModifyMarkerName(const QString &markerId, const QString &name)
{
    for (ItemPlotMarker *marker : m_markerList) {
        if (marker->markerId() == markerId) {
            marker->setName(name);
            break;
        }
    }
    emit modifyNotify();
}

void ItemPlotCanvas::onModifyMarkerColor(const QString &markerId, const QColor &color)
{
    for (ItemPlotMarker *marker : m_markerList) {
        if (marker->markerId() == markerId) {
            marker->setColor(color);
            break;
        }
    }
    emit modifyNotify();
}

void ItemPlotCanvas::onModifyMarkerIsShow(const QString &markerId, bool show)
{
    for (ItemPlotMarker *marker : m_markerList) {
        if (marker->markerId() == markerId) {
            marker->setShow(show);
            break;
        }
    }
    emit modifyNotify();
}

void ItemPlotCanvas::onRemoveMarker(const QString &markerId)
{
    for (QList<ItemPlotMarker *>::iterator iter = m_markerList.begin(); iter != m_markerList.end(); iter++) {
        if ((*iter)->markerId() == markerId) {
            delete (*iter);
            m_markerList.erase(iter);
            update();
            break;
        }
    }
    emit modifyNotify();
}

void ItemPlotCanvas::zoomReback(void)
{
    if (m_selectRectList.size() == 0)
        return;
    AxisRanges rect = m_selectRectList.takeLast();

    m_tempSelectRect = rect;

    QMap<QString, QVariant> paramMap;
    paramMap["SelectRect"].setValue(m_tempSelectRect);
    emit notifyCommand(ItemPlotCanvasCommand_Selected, paramMap);
}

void ItemPlotCanvas::zoomBase(bool bforce)
{
    m_selectRectList.clear();
    setAxisScale(QwtPlot::xBottom, m_baseAxisRect.xBottomMin, m_baseAxisRect.xBottomMax);
    setAxisScale(QwtPlot::yLeft, m_baseAxisRect.yLeftMin, m_baseAxisRect.yLeftMax);
    setPlotCurveDataSelectRange(m_baseAxisRect.xBottomMin, m_baseAxisRect.xBottomMax);
    if (plot()->axisEnabled(QwtPlot::yRight)) {
        setAxisScale(QwtPlot::yRight, m_baseAxisRect.yRightMin, m_baseAxisRect.yRightMax);
    }
    m_wheelZoomValue = 1.0f;
    plot()->replot();

    clearSelected();

    refreshAllMarkerPoint();
}

void ItemPlotCanvas::setAxisRange(QwtPlot::Axis axis, qreal nMin, qreal nMax)
{
    switch (axis) {
    case QwtPlot::xBottom: {
        m_baseAxisRect.xBottomMax = nMax;
        m_baseAxisRect.xBottomMin = nMin;
        setPlotCurveDataSelectRange(nMin, nMax);
    } break;
    case QwtPlot::yLeft: {
        m_baseAxisRect.yLeftMax = nMax;
        m_baseAxisRect.yLeftMin = nMin;
    } break;
    case QwtPlot::yRight: {
        m_baseAxisRect.yRightMax = nMax;
        m_baseAxisRect.yRightMin = nMin;
    } break;
    default:
        break;
    }
    m_selectRectList.clear();
    m_wheelZoomValue = 1.0f;
    setAxisScale(axis, nMin, nMax);
}

QList<ItemPlotCanvas::SelectPoint> ItemPlotCanvas::getNeerPoint(QwtPlot *plot, qreal x, qreal y)
{
    QList<SelectPoint> pointList;
    if (!plot)
        return pointList;

    QwtPlotItemList items = plot->itemList(QwtPlotItem::Rtti_PlotCurve);
    if (items.isEmpty())
        return pointList;

    for (QwtPlotItemIterator iter = items.begin(); iter != items.end(); iter++) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(*iter);
        if (!curve || !curve->isVisible())
            continue;

        QReadWriteLock &tmpLock = curve->getCurveLock();
        tmpLock.lockForRead();

        SelectPoint selectPoint;
        selectPoint.color = curve->pen().color();
        selectPoint.name = curve->title().text();
        ItemPlotCurveSeriesData *curveData = curve->curveData();
        curveData->getNearPoint(x, selectPoint.DataPoint, selectPoint.isOtherData, selectPoint.otherData);
        tmpLock.unlock();

        selectPoint.ScreenPoint.setX(plot->transform(QwtPlot::xBottom, selectPoint.DataPoint.x()));
        if (curve->yAxis() == QwtPlot::yLeft)
            selectPoint.ScreenPoint.setY(plot->transform(QwtPlot::yLeft, selectPoint.DataPoint.y()));
        else
            selectPoint.ScreenPoint.setY(plot->transform(QwtPlot::yRight, selectPoint.DataPoint.y()));
        pointList.push_back(selectPoint);
    }
    return pointList;
}

void ItemPlotCanvas::clearSelected(void)
{
    m_selectRect.types = AxisType_Invalid;
    m_tempSelectRect.types = AxisType_Invalid;

    update();
}

void ItemPlotCanvas::setPlotCurveDataSelectRange(double nLeft, double nRight)
{
    QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
    if (items.size() == 0)
        return;

    for (QwtPlotItemIterator iter = items.begin(); iter != items.end(); iter++) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(*iter);
        if (!curve)
            continue;
        QReadWriteLock &curveLock = curve->getCurveLock();
        QWriteLocker guard(&curveLock);
        ItemPlotCurveSeriesData *curveData = curve->curveData();
        curveData->setSelectedRange(nLeft, nRight);
    }
}

void ItemPlotCanvas::clearPlotCurveDataSelectRange(void)
{
    QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
    if (items.size() == 0)
        return;

    for (QwtPlotItemIterator iter = items.begin(); iter != items.end(); iter++) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(*iter);
        if (!curve)
            continue;
        QReadWriteLock &curveLock = curve->getCurveLock();
        QWriteLocker guard(&curveLock);
        ItemPlotCurveSeriesData *curveData = curve->curveData();
        curveData->resetSelectedRange();
    }
}

void ItemPlotCanvas::getPlotCurveDataNearPoint(QPointF &point)
{
    m_selectPointList = getNeerPoint(plot(), point.x(), point.y());
}

void ItemPlotCanvas::onInitCoordinate()
{
    emit notifyCommand(ItemPlotCanvasCommand_ResetCoordinate, QMap<QString, QVariant>());
}

void ItemPlotCanvas::onCopyAllDataToClipBoard()
{
    QApplication::clipboard()->clear();
    LOGOUT(tr("Copying data..."), LOG_NORMAL);
    mFutureWatcher.setFuture(QtConcurrent::run((QThreadPool::globalInstance()), [&]() {
        QString &dataStr = getCurveDataString(true);
        return dataStr;
    }));
}

void ItemPlotCanvas::onCopyVisualAreaDataToClipBoard()
{
    QApplication::clipboard()->clear();
    LOGOUT(tr("Copying data..."), LOG_NORMAL);
    mFutureWatcher.setFuture(QtConcurrent::run((QThreadPool::globalInstance()), [&]() {
        QString &dataStr = getCurveDataString(false);
        return dataStr;
    }));
}

QString ItemPlotCanvas::getCurveDataString(bool isCopyAllData)
{
    ItemPlot *targetPlot = static_cast<ItemPlot *>(plot());
    if (!targetPlot) {
        LOGOUT(tr("Get graph data failed"), LOG_ERROR);
        return QString();
    }

    QString dataStr;
    QStringList srcList;
    srcList << "Time(us)";
    QReadWriteLock &plotLock = targetPlot->getPlotLock();
    QReadLocker guardLocker(&plotLock);

    qint64 maxIndex = -1;
    QList<ItemPlotCurve *> allCurve;
    bool hasCalStep = false;
    double space = -1.0;
    PlotInfo &info = targetPlot->getPlotInfo();
    for (auto &val : info.VaribleInfoList) {
        bool added = false;
        for (auto &curveItem : val.CurveInfoList) {
            if (!curveItem.Checked)
                continue;
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveItem.ItemPlotCurveObject);
            if (!curve || !curve->isVisible())
                continue;
            allCurve << curve->clone(); // 这里克隆，防止数据量大时拷贝过程中示波器被关闭导致软件崩溃
            added = true;
            QReadWriteLock &curveLock = curve->getCurveLock();
            QReadLocker curveGuard(&curveLock);

            qint64 tempSize = 0;
            if (isCopyAllData) {
                tempSize = curve->curveData()->data().size() - 1;
            } else {
                QPair<qint64, qint64> curveRange = curve->curveData()->getCurrentRange();
                if (curveRange.first == curveRange.second && curveRange.second < 0)
                    continue;

                if (curveRange.first < 0.0) {
                    curveRange.first = 0;
                }

                if (curveRange.second < 0.0) {
                    curveRange.second = curve->curveData()->data().size() - 1;
                }
                tempSize = curveRange.second - curveRange.first;
            }
            if (tempSize > maxIndex)
                maxIndex = tempSize;

            if (!hasCalStep) {
                QList<QPointF> &pureData = curve->curveData()->data();
                if (pureData.size() >= 2) {
                    space = pureData[1].x() - pureData[0].x();
                }
                hasCalStep = true;
            }
        }
        if (added) {
            QString tempKey;
            QStringList &tempList = val.Path.split(".");
            if (tempList.size() < 5)
                continue;
            srcList << tempList[2] + "." + tempList[3] + "." + tempList[4];
        }
    }
    dataStr = srcList.join(",").append("\n");

    if (allCurve.isEmpty())
        return dataStr;

    QStringList dataList;
    int col = 0;
    for (auto &val : allCurve) {
        QReadWriteLock &curveLock = val->getCurveLock();
        QReadLocker curveGuard(&curveLock);
        QList<QPointF> &allData = val->curveData()->data();
        QPair<qint64, qint64> curveRange;
        if (!isCopyAllData) {
            curveRange = val->curveData()->getCurrentRange();
            if (curveRange.first == curveRange.second && curveRange.second < 0)
                continue;
            if (curveRange.first < 0.0) {
                curveRange.first = 0;
            }

            if (curveRange.second < 0.0) {
                curveRange.second = allData.size() - 1;
            }
        }

        for (qint64 i = 0; i <= maxIndex; ++i) {
            if (isCopyAllData) {
                if (col == 0) {
                    if (i < allData.size()) {
                        dataList.append(QString::number(allData[i].x() * NUM_SCALE, 'f', 0) + ","
                                        + QString::number(allData[i].y(), 'f', DATADECIMAL));
                    } else {
                        int timestamp = i * space * NUM_SCALE;
                        dataList.append(QString::number(timestamp, 'f', 0) + "," + "noNum");
                    }
                } else {
                    if (i < allData.size()) {
                        dataList[i] = dataList[i] + "," + QString::number(allData[i].y(), 'f', DATADECIMAL);
                    } else {
                        dataList[i] = dataList[i] + "," + "noNum";
                    }
                }
            } else {
                if (col == 0) {
                    if (curveRange.first + i < allData.size()) {
                        dataList.append(QString::number(allData[curveRange.first + i].x() * NUM_SCALE, 'f', 0) + ","
                                        + QString::number(allData[curveRange.first + i].y(), 'f', DATADECIMAL));
                    } else {
                        int timestamp = (curveRange.first + i) * space * NUM_SCALE;
                        dataList.append(QString::number(timestamp, 'f', 0) + "," + "noNum");
                    }
                } else {
                    if (curveRange.first + i < allData.size()) {
                        dataList[i] = dataList[i] + ","
                                + QString::number(allData[curveRange.first + i].y(), 'f', DATADECIMAL);
                    } else {
                        dataList[i] = dataList[i] + "," + "noNum";
                    }
                }
            }
        }
        ++col;
    }

    for (auto &eachRow : dataList) {
        dataStr += eachRow + "\n";
    }
    return dataStr;
}

void ItemPlotCanvas::paintEvent(QPaintEvent *event)
{
    QwtPlotCanvas::paintEvent(event);

    QPainter painter;
    if (!painter.begin(this))
        return;

    if (mIsCovered) {
        painter.setBrush(QColor(238, 245, 251, 100));
        // painter.drawRoundedRect(QRect(rect().x(), rect().y(), rect().width() - 1, rect().height() - 1), 0, 0);
        painter.drawRoundedRect(QRect(rect().x() - 1, rect().y() - 1, rect().width() + 1, rect().height() + 1), 0, 0);
    }

    QPainter::CompositionMode oldMode = painter.compositionMode();

    painter.save();

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, false);

    if (m_mousePos.x() != INVALID_VALUE && m_mousePos.y() != INVALID_VALUE) {
        // 画鼠标跟随取点相关

        QPen pen(QColor(255, 255, 255, 200));
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);

        qreal x = plot()->transform(QwtPlot::xBottom, m_mousePos.x());
        qreal y = plot()->transform(QwtPlot::yLeft, m_mousePos.y());

        painter.setCompositionMode(QPainter::CompositionMode_Difference);
        painter.drawLine(QPoint(0, y - 1), QPoint(width(), y - 1));
        painter.drawLine(QPoint(x - 1, 0), QPoint(x - 1, height()));
        painter.setCompositionMode(oldMode);
        QFont font = painter.font();
        font.setPixelSize(12);
        painter.setFont(font);
        QFontMetrics fontMetrics(font);

        double x_mousePos = 0;
        x_mousePos = m_mousePos.x();
        QString strTemp = QString("x=%1,y=%2")
                                  .arg(QString::number(x_mousePos, 'f', m_xDecimal))
                                  .arg(QString::number(m_mousePos.y(), 'f', m_yDecimal));

        QRect rect;
        rect.setRect(x + 4, y - fontMetrics.height() - 12, fontMetrics.width(strTemp) + 8, fontMetrics.height() + 6);

        if (rect.y() < 0) {
            int h = rect.height();
            rect.setY(0);
            rect.setHeight(h);
        }
        if (rect.right() >= width()) {
            int w = rect.width();
            rect.setX(width() - w - 1);
            rect.setWidth(w);
        }
        painter.setBrush(QBrush(QColor(0, 162, 232, 200)));
        painter.setPen(QColor(0, 0, 128));
        painter.drawRect(rect);

        painter.setPen(QColor(32, 32, 32));
        painter.drawText(rect, Qt::AlignCenter, strTemp);

        rect.adjust(-1, -1, -1, -1);
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(rect, Qt::AlignCenter, strTemp);

        double rectWidth = fontMetrics.width(strTemp) + 8;
        for (QList<SelectPoint>::iterator iter = m_selectPointList.begin(); iter != m_selectPointList.end(); iter++) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(iter->color);

            if (!qIsNaN(iter->DataPoint.y()) && !qIsInf(iter->DataPoint.y())) {
                QRect rc;
                rc.setRect(iter->ScreenPoint.x() - 6, iter->ScreenPoint.y() - 6, 10, 10);
                painter.drawRect(rc);
                painter.drawLine(iter->ScreenPoint.x() - 13, iter->ScreenPoint.y() - 1, iter->ScreenPoint.x() + 12,
                                 iter->ScreenPoint.y() - 1);
                painter.drawLine(iter->ScreenPoint.x() - 1, iter->ScreenPoint.y() - 13, iter->ScreenPoint.x() - 1,
                                 iter->ScreenPoint.y() + 12);
            }

            double x_mousePos = 0;
            x_mousePos = iter->DataPoint.x();
            strTemp = QString("x=%1,y=%2")
                              .arg(QString::number(x_mousePos, 'f', m_xDecimal))
                              .arg(QString::number(iter->DataPoint.y(), 'f', m_yDecimal));
            if (fontMetrics.width(strTemp) + 8 > rectWidth)
                rectWidth = fontMetrics.width(strTemp) + 8;
        }

        int i = 0;
        for (QList<SelectPoint>::iterator iter = m_selectPointList.begin(); iter != m_selectPointList.end(); iter++) {
            strTemp = QString("x=%1,y=%2")
                              .arg(QString::number(iter->DataPoint.x(), 'f', m_xDecimal))
                              .arg(QString::number(iter->DataPoint.y(), 'f', m_yDecimal));

            painter.setBrush(Qt::NoBrush);
            painter.setPen(iter->color);

            painter.setBrush(QBrush(QColor(iter->color.red(), iter->color.green(), iter->color.blue(), 200)));
            rect.setRect(8, 8 + i * 18, rectWidth, 16);
            painter.drawRect(rect);

            painter.setPen(QColor(32, 32, 32));
            painter.drawText(rect, Qt::AlignCenter, strTemp);
            rect.adjust(-1, -1, -1, -1);
            painter.setPen(QColor(255, 255, 255));
            painter.drawText(rect, Qt::AlignCenter, strTemp);
            ++i;
        }
    }

    QRect tempSelectedRect;
    if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {
        tempSelectedRect.setLeft(plot()->transform(QwtPlot::xBottom, m_selectRect.xBottomMin));
        tempSelectedRect.setTop(plot()->transform(QwtPlot::yLeft, m_selectRect.yLeftMax));
        tempSelectedRect.setRight(plot()->transform(QwtPlot::xBottom, m_selectRect.xBottomMax));
        tempSelectedRect.setBottom(plot()->transform(QwtPlot::yLeft, m_selectRect.yLeftMin));
    }

    if (m_isAllowSelectXAxis && !m_isAllowSelectYAxis) {
        if (m_selectRect.types & AxisType_xBottom) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(40, 166, 198, 100));
            painter.drawRect(
                    QRect(tempSelectedRect.left(), 0, tempSelectedRect.right() - tempSelectedRect.left(), height()));

            painter.setCompositionMode(QPainter::CompositionMode_Difference);
            painter.setPen(QColor(Qt::white));

            painter.drawLine(QPointF(tempSelectedRect.left(), 0), QPointF(tempSelectedRect.left(), height()));
            painter.drawLine(QPointF(tempSelectedRect.right(), 0), QPointF(tempSelectedRect.right(), height()));
        }
    } else if (!m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
        if (m_selectRect.types & AxisType_yLeft) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(40, 166, 198, 100));
            painter.drawRect(
                    QRect(0, tempSelectedRect.top(), width(), tempSelectedRect.bottom() - tempSelectedRect.top()));

            painter.setCompositionMode(QPainter::CompositionMode_Difference);
            painter.setPen(QColor(Qt::white));
            painter.drawLine(QPointF(0, tempSelectedRect.top()), QPointF(width(), tempSelectedRect.top()));
            painter.drawLine(QPointF(0, tempSelectedRect.bottom()), QPointF(width(), tempSelectedRect.bottom()));
        }
    } else if (m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
        if (m_selectRect.types & AxisType_yLeft && m_selectRect.types & AxisType_xBottom) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(40, 166, 198, 100));
            painter.drawRect(tempSelectedRect);
            painter.setCompositionMode(QPainter::CompositionMode_Difference);
            painter.setPen(QColor(Qt::white));
            painter.drawLine(QPointF(tempSelectedRect.left(), 0), QPointF(tempSelectedRect.left(), height()));
            painter.drawLine(QPointF(tempSelectedRect.right(), 0), QPointF(tempSelectedRect.right(), height()));
            painter.setPen(QColor(Qt::white));
            painter.drawLine(QPointF(0, tempSelectedRect.top()), QPointF(width(), tempSelectedRect.top()));
            painter.drawLine(QPointF(0, tempSelectedRect.bottom()), QPointF(width(), tempSelectedRect.bottom()));
        }
    } else {
    }
    painter.setCompositionMode(oldMode);

    for (ItemPlotMarker *marker : m_markerList) {
        if (!marker->isShow())
            continue;
        marker->draw(&painter, rect());
    }

    painter.restore();

    painter.end();
}

void ItemPlotCanvas::mousePressEvent(QMouseEvent *event)
{
    QwtPlotCanvas::mousePressEvent(event);
    if (!m_isAllowSelectXAxis && !m_isAllowSelectYAxis)
        return;

    if (event->button() & Qt::LeftButton) {
        // m_isLeftButtonPress = true;

        m_tempSelectRect.types = AxisType_Invalid;

        if (m_isAllowSelectXAxis) {
            m_tempSelectRect.types |= AxisType_xBottom;

            m_tempSelectRect.xBottomMin = plot()->invTransform(QwtPlot::xBottom, event->pos().x());
            m_tempSelectRect.xBottomMax = m_tempSelectRect.xBottomMin;
        }

        if (m_isAllowSelectYAxis) {
            m_tempSelectRect.types |= AxisType_yLeft;
            m_tempSelectRect.yLeftMax = plot()->invTransform(QwtPlot::yLeft, event->pos().y());
            m_tempSelectRect.yLeftMin = m_tempSelectRect.yLeftMax;

            if (plot()->axisEnabled(QwtPlot::yRight)) {
                m_tempSelectRect.types |= AxisType_yRight;
                m_tempSelectRect.yRightMax = plot()->invTransform(QwtPlot::yRight, event->pos().y());
                m_tempSelectRect.yRightMin = m_tempSelectRect.yRightMax;
            }
        }

        if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {

            QMap<QString, QVariant> paramMap;
            paramMap["SelectRect"].setValue(m_tempSelectRect);
            emit notifyCommand(ItemPlotCanvasCommand_Selecting, paramMap);
        }
    }
    m_pannerStartPos = event->pos();

    if (m_currentMarker) {
        m_currentMarker->startMove(event->pos());
    }
}

void ItemPlotCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QwtPlotCanvas::mouseMoveEvent(event);
    if (!m_isAllowSelectXAxis && !m_isAllowSelectYAxis && !m_isMouseFollow)
        return;
    /////////////////////////
    if (event->buttons() & Qt::LeftButton) {
        if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {
            ItemPlot *itp = (ItemPlot *)plot();
            PlotWidget *pw = (PlotWidget *)(itp->parent());
            if (!pw->getSelectState()) {
                pw->setSelectState(true);
            }
            event->accept();
        }

        if (m_currentMarker) {
            if (m_tempSelectRect.xBottomMin - QPointF(event->pos()).x() < 0.2)
                QPointF(event->pos()).setX(m_tempSelectRect.xBottomMin + 0.2);

            m_currentMarker->moveTo(QPointF(event->pos()));
        }
    } else {
        m_currentMarker = nullptr;
        for (ItemPlotMarker *marker : m_markerList) {
            if (!marker->isShow())
                continue;
            if (marker->contains(QPointF(event->pos()))) {
                m_currentMarker = marker;
                break;
            }
        }
        Qt::CursorShape cursorShape = Qt::CrossCursor;
        if (m_currentMarker) {
            if (m_currentMarker->markerType() == HLine)
                cursorShape = Qt::SizeVerCursor;
            else
                cursorShape = Qt::SizeHorCursor;
        }
        this->setCursor(QCursor(cursorShape));
    }

    ///////////////////////////////////////////////////////

    QMap<QString, QVariant> paramMap;

    double x = event->pos().x();
    double y = event->pos().y();

    if (x < 0)
        x = 0;
    if (x > width())
        x = width();
    if (y < 0)
        y = 0;
    if (y > height())
        y = height();

    x = plot()->invTransform(QwtPlot::xBottom, x);
    y = plot()->invTransform(QwtPlot::yLeft, y);
    double y2;
    if (plot()->axisEnabled(QwtPlot::yRight)) {
        y2 = plot()->invTransform(QwtPlot::yRight, y);
    }
    // 鼠标跟随取点////////////////////////////////////////////////////////////////////////
    m_tempMousePos.setX(x);
    m_tempMousePos.setY(y);
    paramMap["MousePos"].setValue(m_tempMousePos);
    if (m_isMouseFollow) {
        emit notifyCommand(ItemPlotCanvasCommand_MouseMove, paramMap);
    }
    // 下面是鼠标圈选的处理/////////////////////////////////////////////////////

    if (event->buttons() & Qt::LeftButton && !m_currentMarker) {
        // if (!m_isLeftButtonPress)
        //     return;
        if (m_isAllowSelectXAxis) {
            m_tempSelectRect.xBottomMax = x;
        }

        if (m_isAllowSelectYAxis) {
            m_tempSelectRect.yLeftMin = y;
        }

        if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {
            QMap<QString, QVariant> paramMap;
            paramMap["SelectRect"].setValue(m_tempSelectRect);
            emit notifyCommand(ItemPlotCanvasCommand_Selecting, paramMap);
        }
    } else if (event->buttons() & Qt::MiddleButton) {
        QwtPlot *plot = this->plot();

        if (m_isAllowSelectXAxis && !m_isAllowSelectYAxis) {
            int dx = event->pos().x() - m_pannerStartPos.x();
            double xMin = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
            double xMax = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
            double yMin = plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
            double yMax = plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
            xMin = plot->transform(QwtPlot::xBottom, xMin);
            xMax = plot->transform(QwtPlot::xBottom, xMax);
            xMin = plot->invTransform(QwtPlot::xBottom, xMin - dx);
            xMax = plot->invTransform(QwtPlot::xBottom, xMax - dx);
            m_tempSelectRect.xBottomMin = (xMin);
            m_tempSelectRect.xBottomMax = (xMax);
            m_tempSelectRect.yLeftMax = (yMax);
            m_tempSelectRect.yLeftMin = (yMin);
        } else if (!m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
            int dy = event->pos().y() - m_pannerStartPos.y();
            double xMin = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
            double xMax = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
            double yMin = plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
            double yMax = plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
            yMin = plot->transform(QwtPlot::yLeft, yMin);
            yMax = plot->transform(QwtPlot::yLeft, yMax);
            yMin = plot->invTransform(QwtPlot::yLeft, yMin - dy);
            yMax = plot->invTransform(QwtPlot::yLeft, yMax - dy);
            m_tempSelectRect.xBottomMin = (xMin);
            m_tempSelectRect.xBottomMax = (xMax);
            m_tempSelectRect.yLeftMax = (yMax);
            m_tempSelectRect.yLeftMin = (yMin);
            if (plot->axisEnabled(QwtPlot::yRight)) {
                yMin = plot->axisScaleDiv(QwtPlot::yRight).lowerBound();
                yMax = plot->axisScaleDiv(QwtPlot::yRight).upperBound();
                yMin = plot->transform(QwtPlot::yRight, yMin);
                yMax = plot->transform(QwtPlot::yRight, yMax);
                yMin = plot->invTransform(QwtPlot::yRight, yMin - dy);
                yMax = plot->invTransform(QwtPlot::yRight, yMax - dy);
                m_tempSelectRect.yRightMax = (yMax);
                m_tempSelectRect.yRightMin = (yMin);
            }
        } else if (m_isAllowSelectXAxis && m_isAllowSelectYAxis) {
            int dx = event->pos().x() - m_pannerStartPos.x();
            double xMin = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
            double xMax = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
            xMin = plot->transform(QwtPlot::xBottom, xMin);
            xMax = plot->transform(QwtPlot::xBottom, xMax);
            xMin = plot->invTransform(QwtPlot::xBottom, xMin - dx);
            xMax = plot->invTransform(QwtPlot::xBottom, xMax - dx);

            int dy = event->pos().y() - m_pannerStartPos.y();
            double yMin = plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
            double yMax = plot->axisScaleDiv(QwtPlot::yLeft).upperBound();
            yMin = plot->transform(QwtPlot::yLeft, yMin);
            yMax = plot->transform(QwtPlot::yLeft, yMax);
            yMin = plot->invTransform(QwtPlot::yLeft, yMin - dy);
            yMax = plot->invTransform(QwtPlot::yLeft, yMax - dy);

            m_tempSelectRect.xBottomMin = (xMin);
            m_tempSelectRect.xBottomMax = (xMax);
            m_tempSelectRect.yLeftMax = (yMax);
            m_tempSelectRect.yLeftMin = (yMin);

            if (plot->axisEnabled(QwtPlot::yRight)) {
                yMin = plot->axisScaleDiv(QwtPlot::yRight).lowerBound();
                yMax = plot->axisScaleDiv(QwtPlot::yRight).upperBound();
                yMin = plot->transform(QwtPlot::yRight, yMin);
                yMax = plot->transform(QwtPlot::yRight, yMax);
                yMin = plot->invTransform(QwtPlot::yRight, yMin - dy);
                yMax = plot->invTransform(QwtPlot::yRight, yMax - dy);
                m_tempSelectRect.yRightMax = (yMax);
                m_tempSelectRect.yRightMin = (yMin);
            }
        }
        if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {
            m_tempSelectRect.types = AxisType_xBottom;
            m_tempSelectRect.types |= AxisType_yLeft;
            if (plot->axisEnabled(QwtPlot::yRight)) {
                m_tempSelectRect.types |= AxisType_yRight;
            }

            QMap<QString, QVariant> paramMap;
            paramMap["SelectRect"].setValue(m_tempSelectRect);
            emit notifyCommand(ItemPlotCanvasCommand_Selected, paramMap);
        }
        m_pannerStartPos = event->pos();
    }
}

void ItemPlotCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    QwtPlotCanvas::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton && event->button() != Qt::MidButton)
        return;

    if (!m_isAllowSelectXAxis && !m_isAllowSelectYAxis)
        return;

    // if (event->button() & Qt::RightButton) {
    //     emit notifyCommand(ItemPlotCanvasCommand_ResetCoordinate, QMap<QString, QVariant>());
    //     return;
    // }

    if (m_pannerStartPos == event->pos())
        return;

    // QList<double> &xTicks = plot()->axisScaleDiv(QwtPlot::xBottom).ticks(QwtScaleDiv::MajorTick);
    // QList<double> &yTicks = plot()->axisScaleDiv(QwtPlot::yLeft).ticks(QwtScaleDiv::MajorTick);

    // if (xTicks.size() >= 2 || yTicks.size() >= 2) {
    //     bool needAbort = false;
    //     if (m_isAllowSelectXAxis) {
    //         if (xTicks[1] - xTicks[0] <= 0.00000001) {
    //             needAbort = true;
    //         }
    //     }

    //    if (m_isAllowSelectYAxis) {
    //        if (yTicks[1] - yTicks[0] <= 0.00000001) {
    //            needAbort = true;
    //        }
    //    }
    //    if (needAbort) {
    //        m_tempSelectRect = AxisRanges();
    //        QMap<QString, QVariant> paramMap;
    //        paramMap["SelectRect"].setValue(m_tempSelectRect);
    //        emit notifyCommand(ItemPlotCanvasCommand_Selecting, paramMap);
    //        return;
    //    }
    //}

    if (m_currentMarker) {
        emit modifyNotify();
        return;
    }

    double x = event->pos().x();
    double y = event->pos().y();
    if (x < 0)
        x = 0;
    if (x > width())
        x = width();
    if (y < 0)
        y = 0;
    if (y > height())
        y = height();

    if (m_isAllowSelectXAxis) {
        m_tempSelectRect.xBottomMax = plot()->invTransform(QwtPlot::xBottom, x);
    }

    if (m_isAllowSelectXAxis) {
        m_tempSelectRect.yLeftMin = plot()->invTransform(QwtPlot::yLeft, y);

        if (plot()->axisEnabled(QwtPlot::yRight)) {
            m_tempSelectRect.yRightMin = plot()->invTransform(QwtPlot::yRight, y);
        }
    }

    if (m_isAllowSelectXAxis || m_isAllowSelectYAxis) {
        qreal min = plot()->axisScaleDiv(QwtPlot::xBottom).lowerBound();
        qreal max = plot()->axisScaleDiv(QwtPlot::xBottom).upperBound();
        if (m_tempSelectRect.xBottomMin < min) {
            m_tempSelectRect.xBottomMin = min;
        } else if (m_tempSelectRect.xBottomMin > max) {
            m_tempSelectRect.xBottomMin = max;
        }
        if (m_tempSelectRect.xBottomMax < min) {
            m_tempSelectRect.xBottomMax = min;
        } else if (m_tempSelectRect.xBottomMax > max) {
            m_tempSelectRect.xBottomMax = max;
        }

        min = plot()->axisScaleDiv(QwtPlot::yLeft).lowerBound();
        max = plot()->axisScaleDiv(QwtPlot::yLeft).upperBound();
        if (m_tempSelectRect.yLeftMin < min) {
            m_tempSelectRect.yLeftMin = min;
        } else if (m_tempSelectRect.yLeftMin > max) {
            m_tempSelectRect.yLeftMin = max;
        }
        if (m_tempSelectRect.yLeftMax < min) {
            m_tempSelectRect.yLeftMax = min;
        } else if (m_tempSelectRect.yLeftMax > max) {
            m_tempSelectRect.yLeftMax = max;
        }

        min = plot()->axisScaleDiv(QwtPlot::yRight).lowerBound();
        max = plot()->axisScaleDiv(QwtPlot::yRight).upperBound();
        if (m_tempSelectRect.yRightMin < min) {
            m_tempSelectRect.yRightMin = min;
        } else if (m_tempSelectRect.yRightMin > max) {
            m_tempSelectRect.yRightMin = max;
        }
        if (m_tempSelectRect.yRightMax < min) {
            m_tempSelectRect.yRightMax = min;
        }

        else if (m_tempSelectRect.yRightMax > max) {
            m_tempSelectRect.yRightMax = max;
        }

        if (m_isAllowSelectXAxis) {
            double xlim = 0;
            ItemPlot *parentPlot = (ItemPlot *)plot();
            QReadWriteLock &plotLock = parentPlot->getPlotLock();
            plotLock.lockForRead();
            if (parentPlot->getPlotInfo().XType == 0) {
                xlim = 1;
            } else {
                if (mPProjectMng) {
                    xlim = mPProjectMng->GetProjectConfig(KL_PRO::STEP_SIZE).toDouble();
                }
            }
            plotLock.unlock();

            if (qAbs(m_tempSelectRect.xBottomMax - m_tempSelectRect.xBottomMin) < xlim) {
                m_tempSelectRect.types = AxisType_Invalid;
                QMap<QString, QVariant> paramMap;
                paramMap["SelectRect"].setValue(m_tempSelectRect);
                emit notifyCommand(ItemPlotCanvasCommand_Selecting, paramMap);
                return;
            }
        }

        if (m_isAllowSelectYAxis) {
            if (qAbs(m_tempSelectRect.yLeftMax - m_tempSelectRect.yLeftMin) < 0.00000001) {
                m_tempSelectRect.types = AxisType_Invalid;
                QMap<QString, QVariant> paramMap;
                paramMap["SelectRect"].setValue(m_tempSelectRect);
                emit notifyCommand(ItemPlotCanvasCommand_Selecting, paramMap);
                return;
            }
        }

        QMap<QString, QVariant> paramMap;
        paramMap["SelectRect"].setValue(m_tempSelectRect);
        emit notifyCommand(ItemPlotCanvasCommand_Selected, paramMap);
    }
}

void ItemPlotCanvas::leaveEvent(QEvent *event)
{
    QwtPlotCanvas::leaveEvent(event);

    QMap<QString, QVariant> paramMap;

    m_tempMousePos.setX(INVALID_VALUE);
    m_tempMousePos.setY(INVALID_VALUE);
    paramMap["MousePos"].setValue(m_tempMousePos);
    emit notifyCommand(ItemPlotCanvasCommand_MouseMove, paramMap);
}

void ItemPlotCanvas::wheelEvent(QWheelEvent *event)
{
    if (!m_isAllowSelectXAxis && !m_isAllowSelectYAxis) {
        event->ignore();
        return;
    } else {
        event->accept();
    }

    if (event->delta() > 0) {
        wheelZoom(true);
    } else {
        wheelZoom(false);
    }

    QMap<QString, QVariant> paramMap;

    double x = event->pos().x();
    double y = event->pos().y();
    if (x < 0)
        x = 0;
    if (x > width())
        x = width();
    if (y < 0)
        y = 0;
    if (y > height())
        y = height();

    x = plot()->invTransform(QwtPlot::xBottom, x);
    y = plot()->invTransform(QwtPlot::yLeft, y);
    // 鼠标跟随取点////////////////////////////////////////////////////////////////////////
    if (m_isMouseFollow) {
        m_tempMousePos.setX(x);
        m_tempMousePos.setY(y);
        paramMap["MousePos"].setValue(m_tempMousePos);
        emit notifyCommand(ItemPlotCanvasCommand_MouseMove, paramMap);
    }
}

void ItemPlotCanvas::contextMenuEvent(QContextMenuEvent *event)
{
    if (!mCanPopMenu)
        return;
    mMainMenu->exec(QCursor::pos());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemPlotMarker::ItemPlotMarker(const MarkerInfo &info, ItemPlotCanvas *canvas)
    : m_canvas(canvas),
      m_markerType(info.type),
      m_scale(info.scale),
      m_pos(0),
      m_value(0),
      m_color(info.color),
      m_id(info.id),
      m_isShow(info.isShow)
{
    connect(this, SIGNAL(refreshMarkerPoint(const QString &, qreal, const QList<MarkerPoint> &)), canvas,
            SIGNAL(refreshMarkerPoint(const QString &, qreal, const QList<MarkerPoint> &)));
    this->setParent(m_canvas);
    // refreshValue();
    resize(true);
}

ItemPlotMarker::~ItemPlotMarker() { }

MarkerInfo ItemPlotMarker::getMarkerInfo(void) const
{
    MarkerInfo info;
    info.color = m_color;
    info.id = m_id;
    info.isShow = m_isShow;
    info.name = m_name;
    info.type = m_markerType;
    info.scale = m_scale;

    return info;
}

void ItemPlotMarker::setScale(qreal scale)
{
    m_scale = scale;
    // refreshValue();
    resize(true);
}

void ItemPlotMarker::setPos(qreal pos)
{
    if (m_markerType == HLine) {
        m_scale = pos / m_canvas->rect().height();
    } else if (m_markerType == VLine || m_markerType == VLine2) {
        m_scale = pos / m_canvas->rect().width();
    }
    m_pos = pos;
    // refreshValue();
    refresh(true);
}

void ItemPlotMarker::setColor(const QColor &clr)
{
    m_color = clr;
    update();
}

void ItemPlotMarker::setName(const QString &name)
{
    m_name = name;
}

void ItemPlotMarker::setShow(bool show)
{
    m_isShow = show;
    if (m_isShow) {
        // refreshValue();
        refresh(true);
    }
    update();
}

QRectF ItemPlotMarker::rect(void) const
{
    QRect rc = m_canvas->rect();
    if (m_markerType == HLine) {
        return QRectF(0, m_pos - 2, rc.width(), 5);
    } else if (m_markerType == VLine || m_markerType == VLine2) {
        return QRectF(m_pos - 2, 0, 5, rc.height());
    }

    return QRectF();
}

bool ItemPlotMarker::contains(const QPointF &pos)
{
    if (!m_isShow)
        return false;
    QRectF rc = rect();
    return rc.contains(pos);
}

void ItemPlotMarker::resize(bool bCalcValue)
{
    if (m_markerType == HLine) {
        m_pos = m_scale * m_canvas->rect().height();
    } else if (m_markerType == VLine || m_markerType == VLine2) {
        m_pos = m_scale * m_canvas->rect().width();
    }

    refresh(bCalcValue);
}

void ItemPlotMarker::refresh(bool bCalcValue)
{
    QList<MarkerPoint> markerPoints;
    if (m_markerType == HLine) {
        if (bCalcValue) {
            m_value = m_canvas->plot()->invTransform(QwtPlot::yLeft, m_pos);
            qreal min = m_canvas->plot()->axisScaleDiv(QwtPlot::yLeft).lowerBound();
            qreal max = m_canvas->plot()->axisScaleDiv(QwtPlot::yLeft).upperBound();
            if (m_value < min) {
                m_value = min;
            } else if (m_value > max) {
                m_value = max;
            }
        }
    } else if (m_markerType == VLine || m_markerType == VLine2) {
        if (bCalcValue) {
            m_value = m_canvas->plot()->invTransform(QwtPlot::xBottom, m_pos);
            qreal min = m_canvas->plot()->axisScaleDiv(QwtPlot::xBottom).lowerBound();
            qreal max = m_canvas->plot()->axisScaleDiv(QwtPlot::xBottom).upperBound();
            if (m_value < min) {
                m_value = min;
            } else if (m_value > max) {
                m_value = max;
            }
        }
        if (m_markerType == VLine) {
            m_points = ItemPlotCanvas::getNeerPoint(m_canvas->plot(), m_value);

            for (ItemPlotCanvas::SelectPoint &point : m_points) {
                MarkerPoint p;
                p.name = point.name;
                p.color = point.color;
                p.value = point.DataPoint;
                p.isOtherData = point.isOtherData;
                p.otherData = point.otherData;
                markerPoints.append(p);
            }
        }
    } else {
    }

    emit refreshMarkerPoint(m_id, m_value, markerPoints);
}

void ItemPlotMarker::startMove(const QPointF &startPos)
{
    m_startPos = startPos;
}

void ItemPlotMarker::moveTo(const QPointF &endPos)
{
    QPointF step = endPos - m_startPos;
    QRect rc = m_canvas->rect();
    if (m_markerType == HLine) {
        m_pos += step.y();
        if (m_pos < 1) {
            m_pos = 1;
            m_startPos.setY(m_pos);
        } else if (m_pos >= rc.height()) {
            m_pos = rc.height() - 1;
            m_startPos.setY(m_pos);
        } else {
            m_startPos = endPos;
        }

        m_scale = m_pos / rc.height();

    } else {
        m_pos += step.x();
        if (m_pos < 1) {
            m_pos = 1;
            m_startPos.setX(m_pos);
        } else if (m_pos >= rc.width()) {
            m_pos = rc.width() - 1;
            m_startPos.setX(m_pos);
        } else {
            m_startPos = endPos;
        }
        m_scale = m_pos / rc.width();
    }
    // m_startPos = endPos;

    // refreshValue();
    refresh(true);
    update();
}

void ItemPlotMarker::draw(QPainter *painter, const QRectF &canvasRect)
{
    QRect rc = m_canvas->rect();
    if (m_markerType == HLine) {
        painter->setPen(QPen(m_color, 1, Qt::DashDotLine));
        painter->drawLine(0, m_pos, rc.width(), m_pos);

        painter->setBrush(m_color);
        painter->setPen(QPen(m_color, 1, Qt::SolidLine));
        QPolygon polygon;
        polygon.setPoints(5, 0, (int)m_pos - 2, 6, (int)m_pos - 2, 9, (int)m_pos, 6, (int)m_pos + 2, 0, (int)m_pos + 2);
        painter->drawPolygon(polygon);

        // if()
    } else if (m_markerType == VLine || m_markerType == VLine2) {
        painter->setPen(QPen(m_color, 1, Qt::DashLine));
        painter->drawLine(m_pos, 0, m_pos, rc.height());
        painter->setBrush(m_color);
        painter->setPen(QPen(m_color, 1, Qt::SolidLine));
        QPolygon polygon;
        polygon.setPoints(5, (int)m_pos - 2, 0, (int)m_pos - 2, 6, (int)m_pos, 9, (int)m_pos + 2, 6, (int)m_pos + 2, 0);
        painter->drawPolygon(polygon);
        if (m_markerType == VLine) {
            painter->setPen(QPen(m_color, 1, Qt::DashLine));
            for (ItemPlotCanvas::SelectPoint &point : m_points) {
                painter->drawLine(0, point.ScreenPoint.y(), rc.width(), point.ScreenPoint.y());
            }
        }
    } else {
    }
}

void ItemPlotMarker::update(void)
{
    m_canvas->update();
}

// void ItemPlotMarker::refreshValue(void)
//{
//	if(m_markerType == MarkerType::HLine)
//	{
//		m_value = m_canvas->plot()->invTransform(QwtPlot::yLeft,m_pos);
//	}
//	else
//	{
//		m_value = m_canvas->plot()->invTransform(QwtPlot::xBottom,m_pos);
//	}
// }

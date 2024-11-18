#include "PlotScrollArea.h"
#include "MonitorPanelPlotServer.h"
USE_LOGOUT_("PlotScrollArea")
PlotScrollArea::PlotScrollArea(QWidget *parent)
    : QScrollArea(parent),
      m_row_split(1),
      m_cols(1),
      m_isCtrlPress(false),
      m_isOnlyShowSelected(false),
      m_isAutoStop(false),
      m_isAllowZoomInXAxis(false),
      m_isAllowZoomInYAxis(false),
      m_isMousePoint(false)
      //, m_selectedWidget(nullptr)
      ,
      m_isSelectedSyncMode(false),
      mSrcPlotWidget(nullptr),
      mMouseClickPos(INVALIDPOS),
      mIsLeftBtnClicked(false),
      mMouseMoved(false),
      mDstPos(-1, -1)
{
    ScrollAreaWidget *w = new ScrollAreaWidget();
    setWidget(w);
    setWidgetResizable(true);
    w->installEventFilter(this);
    w->setStyleSheet("ScrollAreaWidget{background-color:  rgb(255,255,255);border: 0px solid;}");
    setStyleSheet("PlotScrollArea{border: 0px solid;background-color:white;}");
    connect(w, SIGNAL(currentIndex(int, int)), this, SLOT(onUpdateIndex(int, int)));
    // this->setFrameShape(QFrame::NoFrame);
    // setFocusPolicy(Qt::StrongFocus);
    // this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{border:0px solid;margin:8px 1px 8px 0px;}\
    //         QScrollBar::handle:vertical {background-color: #D8D8D8;margin-left: 1px;margin-right:1px;}\
    //         QScrollBar::handle:vertical:hover {background-color: #C8C8C8}\
    //         QScrollBar::handle:vertical:pressed {background-color: #B8B8B8;}");
}

PlotScrollArea::~PlotScrollArea() { }

void PlotScrollArea::relayout(int rows, int cols)
{
    if (rows <= 0 || cols <= 0)
        return;
    if (rows == m_row_split && cols == m_cols)
        return;

    m_row_split = rows;
    m_cols = cols;

    QPair<int, int> &newWH = getSingleWithHeight();
    mRealRow = calRealRow();
    if (mRealRow < m_row_split)
        mRealRow = m_row_split;
    widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);

    // 记录里有则按照记录里的来，没有则重排
    auto it = std::find_if(mOldPosRecord.begin(), mOldPosRecord.end(), [&](PosRecord &pos) {
        if (pos.checkError())
            return false;
        const QPair<int, int> &lay = pos.getRc();
        if (pos.getRealRow() != mRealRow || lay.first != m_row_split || lay.second != m_cols)
            return false;
        return true;
    });

    if (it != mOldPosRecord.end()) {
        widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);
        for (int i = 0; i < m_widgetList.size(); ++i) {
            PlotWidget *pw = TOPW(m_widgetList[i]);
            if (!pw)
                continue;
            ItemPlot *itp = pw->plot();
            if (!itp)
                continue;
            QReadWriteLock &plotLock = itp->getPlotLock();
            plotLock.lockForRead();
            const QPair<int, int> &plotPos = it->getPlotPos(itp->getId());
            plotLock.unlock();

            QRect rect(SCROLLAREAMARGIN + plotPos.second * newWH.first, SCROLLAREAMARGIN + plotPos.first * newWH.second,
                       newWH.first, newWH.second);
            rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
            m_widgetList[i]->setGeometry(rect);
            pw->setPos(plotPos.first, plotPos.second);
        }

    } else {
        for (int k = 0; k < m_widgetList.size(); k++) {
            int i = k / m_cols;
            int j = k % m_cols;
            QRect rect(SCROLLAREAMARGIN + j * newWH.first, SCROLLAREAMARGIN + i * newWH.second, newWH.first,
                       newWH.second);
            rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
            m_widgetList[k]->setGeometry(rect);
            TOPW(m_widgetList[k])->setPos(i, j);
        }
    }

    ScrollAreaWidget *areaW = static_cast<ScrollAreaWidget *>(widget());
    areaW->setRow(mRealRow);
    areaW->setCol(m_cols);
    update();
}

void PlotScrollArea::intelichangeLayout()
{
    if (0 < m_widgetList.size() <= 3) {
        if (m_cols > m_widgetList.size()) {
            m_cols = m_widgetList.size();
        }
    }

    int plotNum = 0;
    plotNum = m_widgetList.size();
    if (plotNum == 0) {
        m_row_split = 1;
        m_cols = 1;
        mRealRow = 1;
    } else {
        int tempRowNum = 0;
        tempRowNum = plotNum / m_cols;
        if (plotNum % m_cols != 0)
            ++tempRowNum;

        if (tempRowNum == 0)
            ++tempRowNum;
        mRealRow = tempRowNum;
        if (mRealRow < m_row_split) {
            m_row_split = mRealRow;
        }
    }

    ScrollAreaWidget *areaW = static_cast<ScrollAreaWidget *>(widget());
    areaW->setRow(mRealRow);
    areaW->setCol(m_cols);

    QPair<int, int> &newWH = getSingleWithHeight();
    widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);

    for (int k = 0; k < plotNum; k++) {
        int i = k / m_cols;
        int j = k % m_cols;
        QRect rect(SCROLLAREAMARGIN + j * newWH.first, SCROLLAREAMARGIN + i * newWH.second, newWH.first, newWH.second);
        rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
        m_widgetList[k]->setGeometry(rect);
        TOPW(m_widgetList[k])->setPos(i, j);
    }
    recordCurrentLayout();
    update();
}

void PlotScrollArea::changeLayoutAfterAdd()
{
    if (fillGpas()) {
        mOldPosRecord.clear();
        recordCurrentLayout();
        return;
    }

    while (mRealRow * m_cols < m_widgetList.size()) {
        ++mRealRow;
    }

    QPair<int, int> &newWH = getSingleWithHeight();
    widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);
    for (int i = 0; i < m_widgetList.size(); ++i) {
        QPair<int, int> pos = TOPW(m_widgetList[i])->getPos();
        if (pos.first == -1 || pos.second == -1)
            continue;
        QRect rect(SCROLLAREAMARGIN + pos.second * newWH.first, SCROLLAREAMARGIN + pos.first * newWH.second,
                   newWH.first, newWH.second);
        rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
        m_widgetList[i]->setGeometry(rect);
    }
    fillGpas();

    ScrollAreaWidget *areaW = static_cast<ScrollAreaWidget *>(widget());
    areaW->setRow(mRealRow);
    areaW->setCol(m_cols);
    mOldPosRecord.clear();
    recordCurrentLayout();
    update();
}

void PlotScrollArea::onlyChangeSize()
{
    int nCount = m_widgetList.size();
    QPair<int, int> &newWH = getSingleWithHeight();
    if (mRealRow < m_row_split)
        mRealRow = m_row_split;
    widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);
    for (int i = 0; i < nCount; ++i) {
        QPair<int, int> pos = ((PlotWidget *)m_widgetList[i])->getPos();
        if (pos.first == -1 || pos.second == -1)
            continue;
        QRect rect(SCROLLAREAMARGIN + pos.second * newWH.first, SCROLLAREAMARGIN + pos.first * newWH.second,
                   newWH.first, newWH.second);
        rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
        TOPW(m_widgetList[i])->setGeometry(rect);
    }
    update();
}

void PlotScrollArea::specificLayout(int realrow, QPair<int, int> &RowAndCol,
                                    QMap<QString, QPair<int, int>> &mAllPlotPos)
{
    if (m_widgetList.isEmpty())
        return;
    mRealRow = realrow;
    m_cols = RowAndCol.second;
    m_row_split = RowAndCol.first;

    ScrollAreaWidget *areaW = static_cast<ScrollAreaWidget *>(widget());
    areaW->setRow(mRealRow);
    areaW->setCol(m_cols);

    QPair<int, int> &newWH = getSingleWithHeight();
    widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);
    for (int i = 0; i < m_widgetList.size(); ++i) {
        PlotWidget *pw = TOPW(m_widgetList[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        if (!itp)
            continue;
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForWrite();
        PlotInfo &itpinfo = itp->getPlotInfo();
        if (!mAllPlotPos.contains(itpinfo.id)) {
            plotLock.unlock();
            continue;
        }
        itpinfo.mRow = mAllPlotPos[itpinfo.id].first;
        itpinfo.mCol = mAllPlotPos[itpinfo.id].second;
        plotLock.unlock();
        QRect rect(SCROLLAREAMARGIN + itpinfo.mCol * newWH.first, SCROLLAREAMARGIN + itpinfo.mRow * newWH.second,
                   newWH.first, newWH.second);
        rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
        m_widgetList[i]->setGeometry(rect);
    }
    update();
}

void PlotScrollArea::layoutAllPlotFromFile(int rows, int cols)
{
    m_row_split = rows;
    m_cols = cols;

    mRealRow = calRealRow();
    if (mRealRow < m_row_split)
        mRealRow = m_row_split;
    ScrollAreaWidget *areaW = static_cast<ScrollAreaWidget *>(widget());
    areaW->setRow(mRealRow);
    areaW->setCol(m_cols);

    // 判断有没有非法的输入
    bool hasIllegalData = false;
    for (int i = 0; i < m_widgetList.size(); ++i) {
        QPair<int, int> pos = TOPW(m_widgetList[i])->getPos();
        if (pos.first == -1 || pos.second == -1) {
            hasIllegalData = true;
            break;
        }
    }

    QPair<int, int> &newWH = getSingleWithHeight();
    widget()->setFixedHeight(newWH.second * mRealRow + 2 * SCROLLAREAMARGIN);
    if (hasIllegalData) {
        for (int k = 0; k < m_widgetList.size(); k++) {
            int i = k / m_cols;
            int j = k % m_cols;
            QRect rect(SCROLLAREAMARGIN + j * newWH.first, SCROLLAREAMARGIN + i * newWH.second, newWH.first,
                       newWH.second);
            rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);

            m_widgetList[k]->setGeometry(rect);
            TOPW(m_widgetList[k])->setPos(i, j);
        }
    } else {
        // 按照从文件读取的数据来
        for (int i = 0; i < m_widgetList.size(); ++i) {
            QPair<int, int> pos = TOPW(m_widgetList[i])->getPos();
            QRect rect(SCROLLAREAMARGIN + pos.second * newWH.first, SCROLLAREAMARGIN + pos.first * newWH.second,
                       newWH.first, newWH.second);
            rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
            m_widgetList[i]->setGeometry(rect);
        }
    }
    recordCurrentLayout();
    update();
}

bool PlotScrollArea::fillGpas()
{
    for (int i = 0; i < mRealRow; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            bool isFinded = false;
            for (int p = 0; p < m_widgetList.size(); ++p) {
                QPair<int, int> pos = TOPW(m_widgetList[p])->getPos();
                if (pos.first == i && pos.second == j) {
                    isFinded = true;
                    break;
                }
            }

            if (!isFinded) {
                // 有空位
                QPair<int, int> &newWH = getSingleWithHeight();
                QRect rect(SCROLLAREAMARGIN + j * newWH.first, SCROLLAREAMARGIN + i * newWH.second, newWH.first,
                           newWH.second);
                rect.adjust(PLOTMARGIN, PLOTMARGIN, -PLOTMARGIN, -PLOTMARGIN);
                PlotWidget *w = TOPW(m_widgetList.back());
                w->setPos(i, j);
                w->setGeometry(rect);
                this->ensureWidgetVisible(w);
                return true;
            }
        }
    }
    return false;
}

void PlotScrollArea::addWidget(QWidget *w)
{
    if (m_widgetList.contains(w))
        return;

    int i = m_widgetList.size() / m_cols;
    int j = m_widgetList.size() % m_cols;

    w->setParent(widget());
    if (m_isOnlyShowSelected)
        w->hide();
    else
        w->show();

    m_widgetList.push_back(w);

    PlotWidget *pw = TOPW(w);
    w->installEventFilter(this);          // 安装事件过滤，主要用到鼠标点击事件，作用是选中
    pw->plot()->installEventFilter(this); //

    ItemPlot *itemPlot = pw->plot();
    itemPlot->setAutoStop(m_isAutoStop);
    itemPlot->setAllowZoomInXAxis(m_isAllowZoomInXAxis);
    itemPlot->setAllowZoomInYAxis(m_isAllowZoomInYAxis);
    itemPlot->setMousePoint(m_isMousePoint);

    if (!m_isSelectedSyncMode) {
        m_plotSyncOperator.add(itemPlot);
    }

    connect(pw->plot(), SIGNAL(modifyNotify()), this, SIGNAL(modifyNotify()));
    connect(pw, &PlotWidget::selectStateChange, this, &PlotScrollArea::emitSelecSignal);
}

void PlotScrollArea::removeWidget(QWidget *w)
{
    if (w == nullptr)
        return;

    int nIndex = m_widgetList.indexOf(w, 0);
    if (nIndex < 0)
        return;

    m_widgetList.removeAt(nIndex);
    PlotWidget *pw = TOPW(w);
    w->removeEventFilter(this); // 移除事件过滤
    pw->plot()->removeEventFilter(this);

    m_plotSyncOperator.remove(pw->plot());
    w->deleteLater(); // 不改为这样的话程序会崩溃
    mOldPosRecord.clear();
}

QList<QWidget *> PlotScrollArea::selectedWidgets(void) const
{
    QList<QWidget *> tempList;
    for (int i = 0; i < m_widgetList.size(); i++) {
        if (!m_widgetList[i])
            continue;
        PlotWidget *w = TOPW(m_widgetList[i]);
        if (!w)
            continue;
        if (w->getSelectState())
            tempList.push_back(m_widgetList[i]);
    }
    return tempList;
}

void PlotScrollArea::removeSelectedWidgets(void)
{
    QList<QWidget *> &tempList = selectedWidgets();
    if (tempList.size() == 0)
        return;
    for (QWidget *w : tempList) {
        removeWidget(w);
    }
    intelichangeLayout();
    aLignItemPlots();
}

bool PlotScrollArea::clearWidgets(void)
{
    QList<QWidget *> tempList = m_widgetList;
    if (tempList.size() == 0)
        return true;
    for (int i = 0; i < tempList.size(); i++) {
        removeWidget(tempList[i]);
    }
    return true;
}

// 通过图表名字去删除
bool PlotScrollArea::removeWidgetFromTitle(const QString &title)
{
    QList<QWidget *> &tempList = m_widgetList;
    if (tempList.size() == 0)
        return false;
    for (QWidget *w : tempList) {
        ItemPlot *itemPlot = ((PlotWidget *)w)->plot();
        //		qDebug() << "+++++++++PlotScrollArea::removeWidgetFromTitle itemPlot->getTitle:" <<
        // itemPlot->getTitle() << " title:" << title << endl;
        if (itemPlot->getId() == title) {
            //			qDebug() << "PlotScrollArea::removeWidgetFromTitle:" << title << endl;
            removeWidget(w);
            return true;
        }
    }
    return false;
}

void PlotScrollArea::setOnlyShowSelected(bool b)
{
    m_isOnlyShowSelected = b;
    if (b) {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;
            if (w->getSelectState())
                continue;
            m_widgetList[i]->setVisible(false);
        }
    } else {
        for (QWidget *w : m_widgetList) {
            w->setVisible(true);
        }
    }
}

bool PlotScrollArea::onlyShowSelected(void)
{
    return m_isOnlyShowSelected;
}

void PlotScrollArea::setSync(bool b)
{
    m_plotSyncOperator.setSync(b);
}

bool PlotScrollArea::isSync(void)
{
    return m_plotSyncOperator.isSync();
}

void PlotScrollArea::setAutoStop(bool b)
{
    for (QWidget *w : m_widgetList) {
        ((PlotWidget *)w)->plot()->setAutoStop(b);
    }
    m_isAutoStop = b;
}

void PlotScrollArea::setStart(bool b)
{
    QStringList allVar;
    for (QWidget *w : m_widgetList) {
        if (b) {
            QStringList &tmpVar = TOPW(w)->plot()->restart();
            for (QString &var : tmpVar) {
                if (allVar.contains(var))
                    continue;
                allVar << var;
            }
        }

        else
            TOPW(w)->plot()->stop();
    }
    if (allVar.isEmpty())
        return;
    PIServerInterfaceBase monitorPanelPlotServerBase = RequestServerInterface<IMonitorPanelPlotServer>();
    if (!monitorPanelPlotServerBase)
        return;

    QMap<QString, QVariant> params;
    params["variablePaths"] = allVar;
    monitorPanelPlotServerBase->emitNotify(CODE_ADDVARIABLE, params);
}

QList<QWidget *> PlotScrollArea::widgets(void)
{
    return m_widgetList;
}

void PlotScrollArea::setAllowZoomInXAxis(bool can)
{
    m_isAllowZoomInXAxis = can;
    for (QWidget *w : m_widgetList) {
        TOPW(w)->plot()->setAllowZoomInXAxis(m_isAllowZoomInXAxis);
    }
}

void PlotScrollArea::setAllowZoomInYAxis(bool can)
{
    m_isAllowZoomInYAxis = can;
    for (QWidget *w : m_widgetList) {
        TOPW(w)->plot()->setAllowZoomInYAxis(m_isAllowZoomInYAxis);
    }
}

void PlotScrollArea::setMousePoint(bool can)
{
    m_isMousePoint = can;
    for (QWidget *w : m_widgetList) {
        TOPW(w)->plot()->setMousePoint(m_isMousePoint);
    }
}

void PlotScrollArea::aLignItemPlots()
{
    if (m_widgetList.size() <= 1)
        return;

    int sampleWidth = ((PlotWidget *)(m_widgetList[0]))->plot()->canvas()->width();
    bool needUpdate = false;
    for (uint i = 1; i < m_widgetList.size(); ++i) {
        PlotWidget *w = TOPW(m_widgetList[i]);
        if (!w || !w->isVisible())
            continue;
        const int &tmpW = w->plot()->canvas()->width();
        if (tmpW != sampleWidth) {
            needUpdate = true;
            break;
        }
    }

    if (!needUpdate)
        return;

    int minW = 0;
    for (uint i = 0; i < m_widgetList.size(); ++i) {
        PlotWidget *w = TOPW(m_widgetList[i]);
        if (!w || !w->isVisible())
            continue;
        QMargins &tpMargin = w->plot()->contentsMargins();
        tpMargin.setLeft(0);
        w->plot()->setContentsMargins(tpMargin);
        const int &tmpW = w->plot()->canvas()->width();
        if (i == 0)
            minW = tmpW;
        if (tmpW < minW)
            minW = tmpW;
    }

    if (minW == 0)
        return;

    for (uint i = 0; i < m_widgetList.size(); ++i) {
        PlotWidget *w = TOPW(m_widgetList[i]);
        if (!w || !w->isVisible())
            continue;
        const int &tmpW = w->plot()->canvas()->width();
        if (tmpW > minW) {
            QMargins &tpMargin = w->plot()->contentsMargins();
            tpMargin.setLeft(tmpW - minW);
            w->plot()->setContentsMargins(tpMargin);
        }
    }
}

void PlotScrollArea::resetSelectedAxis(void)
{
    if (m_plotSyncOperator.isSync()) {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;

            w->plot()->resetAxis();
        }
    } else {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;

            if (!w->getSelectState())
                continue;
            w->plot()->resetAxis();
        }
    }
}

void PlotScrollArea::setSelectedXAxisFull(void)
{
    if (m_plotSyncOperator.isSync()) {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;
            w->plot()->setXAxisFull();
        }
    } else {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;
            if (!w->getSelectState())
                continue;
            w->plot()->setXAxisFull();
        }
    }
}

void PlotScrollArea::setSelectedYAxisFull(void)
{
    if (m_plotSyncOperator.isSync()) {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;
            w->plot()->setYAxisFull();
        }
    } else {
        for (int i = 0; i < m_widgetList.size(); i++) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;
            if (!w->getSelectState())
                continue;
            w->plot()->setYAxisFull();
        }
    }
}

bool PlotScrollArea::isBinging(void) const
{
    for (int i = 0; i < m_widgetList.size(); i++) {
        if (TOPW(m_widgetList[i])->plot()->isBinging())
            return true;
    }
    return false;
}

void PlotScrollArea::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
    onlyChangeSize();
}

void PlotScrollArea::paintEvent(QPaintEvent *event)
{
    QScrollArea::paintEvent(event);
    aLignItemPlots();
}

void PlotScrollArea::onUpdateIndex(int row, int col)
{
    mDstPos.first = row;
    mDstPos.second = col;
}

bool PlotScrollArea::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() != QEvent::MouseButtonPress)
        return QScrollArea::eventFilter(obj, e);
    QMouseEvent *mouseE = (QMouseEvent *)(e);
    if (!mouseE || mouseE->button() != Qt::LeftButton)
        return QScrollArea::eventFilter(obj, e);

    PlotWidget *objw = dynamic_cast<PlotWidget *>(obj);
    ItemPlot *objItm = dynamic_cast<ItemPlot *>(obj);
    if (!objw && !objItm) {
        for (int i = 0; i < m_widgetList.size(); ++i) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;
            w->setSelectState(false);
            if (m_isSelectedSyncMode) {
                m_plotSyncOperator.remove(w->plot());
            }
        }
    } else {
        for (int i = 0; i < m_widgetList.size(); ++i) {
            if (!m_widgetList[i])
                continue;
            PlotWidget *w = TOPW(m_widgetList[i]);
            if (!w)
                continue;

            if (objw != w && objItm != w->plot()) {
                if (!m_isCtrlPress) {
                    w->setSelectState(false);
                    if (m_isSelectedSyncMode) {
                        m_plotSyncOperator.remove(w->plot());
                    }
                }
                continue;
            }

            // if (!m_isAllowZoomInXAxis && !m_isAllowZoomInYAxis) {
            mIsLeftBtnClicked = true;
            mMouseClickPos = mouseE->pos();
            const int &scrollValue = this->verticalScrollBar()->value();
            mMouseClickPos.setY(mMouseClickPos.y() + scrollValue);
            mSrcPlotWidget = TOPW(m_widgetList[i]);
            mSrcPlotWidget->raise();
            //}

            w->setSelectState(!w->getSelectState());
            if (m_isSelectedSyncMode) {
                if (w->getSelectState()) {
                    m_plotSyncOperator.add(w->plot());
                } else {
                    m_plotSyncOperator.remove(w->plot());
                }
            }
        }
    }

    // Q_EMIT selectPlot();
    return QScrollArea::eventFilter(obj, e);
}

void PlotScrollArea::mouseMoveEvent(QMouseEvent *e)
{
    QScrollArea::mouseMoveEvent(e);
    if (!mIsLeftBtnClicked || !mSrcPlotWidget)
        return;

    mMouseMoved = true;
    ScrollAreaWidget *w = static_cast<ScrollAreaWidget *>(widget());
    if (!w)
        return;
    Q_EMIT dragpPloting();
    if (!mSrcPlotWidget->getSelectState()) {
        mSrcPlotWidget->setSelectState(true);
    }
    const int &scrollValue = this->verticalScrollBar()->value();
    QPoint newPos = e->pos();
    newPos.setY(scrollValue + newPos.y());
    w->setCursor(newPos);
    if (mSrcPlotWidget) {
        mSrcPlotWidget->move(newPos.x() - mMouseClickPos.x(), newPos.y() - mMouseClickPos.y() + scrollValue);
    }
}

void PlotScrollArea::mouseReleaseEvent(QMouseEvent *e)
{
    QScrollArea::mouseReleaseEvent(e);
    if (!mSrcPlotWidget || e->button() != Qt::LeftButton)
        return;
    if (!mMouseMoved) {
        clearDragValue();
        mMouseMoved = false;
        return;
    }
    Q_EMIT dragPlotFinish();
    QPair<int, int> srcPos = mSrcPlotWidget->getPos();
    QRect mSrcRect = calRect(srcPos.first, srcPos.second);
    if (mDstPos.first == -1 || mDstPos.second == -1
        || (srcPos.first == mDstPos.first && srcPos.second == mDstPos.second)) {
        mSrcPlotWidget->setGeometry(mSrcRect.x() + PLOTMARGIN, mSrcRect.y() + PLOTMARGIN, mSrcPlotWidget->width(),
                                    mSrcPlotWidget->height());
        clearDragValue();
        return;
    }

    QRect &dstRec = calRect(mDstPos.first, mDstPos.second);
    if (dstRec == INVALIDREC) {
        mSrcPlotWidget->setGeometry(mSrcRect.x() + PLOTMARGIN, mSrcRect.y() + PLOTMARGIN, mSrcPlotWidget->width(),
                                    mSrcPlotWidget->height());
        clearDragValue();
        return;
    }

    for (int i = 0; i < m_widgetList.size(); ++i) {
        if (!m_widgetList[i])
            continue;
        PlotWidget *plot = TOPW(m_widgetList[i]);
        if (plot->getPos().first == mDstPos.first && plot->getPos().second == mDstPos.second) {
            plot->setGeometry(mSrcRect.x() + PLOTMARGIN, mSrcRect.y() + PLOTMARGIN, plot->width(), plot->height());
            plot->setPos(srcPos.first, srcPos.second);
            break;
        }
    }

    QPoint dst(dstRec.left() + PLOTMARGIN, dstRec.top() + PLOTMARGIN);
    mSrcPlotWidget->setGeometry(dst.x(), dst.y(), mSrcPlotWidget->width(), mSrcPlotWidget->height());
    mSrcPlotWidget->setPos(mDstPos.first, mDstPos.second);
    clearDragValue();
    recordCurrentLayout();
    Q_EMIT modifyNotify();
}

void PlotScrollArea::recordCurrentLayout()
{
    auto it = std::find_if(mOldPosRecord.begin(), mOldPosRecord.end(), [&](PosRecord &pos) {
        if (pos.checkError())
            return false;
        const QPair<int, int> &curlay = pos.getRc();
        if (pos.getRealRow() != mRealRow || curlay.first != m_row_split || curlay.second != m_cols)
            return false;
        return true;
    });

    if (it != mOldPosRecord.end()) {
        it->clear();
        for (auto &x : m_widgetList) {
            PlotWidget *pw = TOPW(x);
            if (!pw)
                continue;
            ItemPlot *itp = pw->plot();
            if (!itp)
                continue;
            const QPair<int, int> &plotPos = pw->getPos();
            QReadWriteLock &plotLock = itp->getPlotLock();
            QReadLocker guard(&plotLock);
            QString &id = itp->getId();
            it->recordPlotPos(id, plotPos);
        }
    } else {
        PosRecord tempSaver;
        mOldPosRecord << tempSaver;
        mOldPosRecord.back().setRC(m_row_split, m_cols);
        mOldPosRecord.back().setRealrow(mRealRow);

        for (auto &x : m_widgetList) {
            PlotWidget *pw = TOPW(x);
            if (!pw)
                continue;
            ItemPlot *itp = pw->plot();
            if (!itp)
                continue;
            const QPair<int, int> &plotPos = pw->getPos();
            if (plotPos.first == -1 || plotPos.second == -1) {
                mOldPosRecord.pop_back();
                return;
            }
            QReadWriteLock &plotLock = itp->getPlotLock();
            QReadLocker guard(&plotLock);
            QString &id = itp->getId();
            mOldPosRecord.back().recordPlotPos(id, plotPos);
        }
    }
}

void PlotScrollArea::allSelect(void)
{
    if (!m_isOnlyShowSelected) {
        for (int i = 0; i < m_widgetList.size(); i++) {
            TOPW(m_widgetList[i])->setSelectState(true);
        }
    } else {
        for (int j = 0; j < m_widgetList.size(); j++) {
            if (!m_widgetList[j]->isVisible())
                continue;
            TOPW(m_widgetList[j])->setSelectState(true);
        }
    }
}

void PlotScrollArea::merge(void)
{
    /*QList<PlotWidget*> plotWidgets;
    for(int i = 0; i < m_widgetList.size(); i++)
    {
            if(m_widgetSelectList[i] == true)
            {
                    PlotWidget* plotWidget = ((PlotWidget*)m_widgetList[i]);
                    if(!plotWidget->plot()->canMergePlot())
                    {
                            return;
                    }
                    plotWidgets << plotWidget;
            }
    }
    if(plotWidgets.size() != 2)
    {
            return;
    }
    plotWidgets[1]->plot()->setDeleteAll(false);
    PlotInfo plotInfo = plotWidgets[1]->plot()->getPlotInfo();
    removeWidget(plotWidgets[1]);
    QList<PlotInfo> plotInfos;
    plotInfos << plotInfo;
    plotWidgets[0]->plot()->mergePlot(plotInfos);*/
}

void PlotScrollArea::unmerge(void)
{
    /*QList<PlotWidget*> plotWidgets;
    for(int i = 0; i < m_widgetList.size(); i++)
    {
    if(m_widgetSelectList[i] == true)
    {
    PlotWidget* plotWidget = ((PlotWidget*)m_widgetList[i]);
    if(!plotWidget->plot()->canUnmergePlot())
    {
    return;
    }
    plotWidgets << plotWidget;
    }
    }
    if(plotWidgets.size() != 1)
    {
    return;
    }*/
}

void PlotScrollArea::setCanPopMenu(bool can)
{
    for (int i = 0; i < m_widgetList.size(); ++i) {
        if (!m_widgetList[i])
            continue;
        PlotWidget *w = TOPW(m_widgetList[i]);
        if (!w)
            continue;
        ItemPlot *itp = w->plot();
        if (!itp)
            continue;
        itp->setCanPopMenu(can);
    }
}

QRect PlotScrollArea::calRect(const int row, const int col)
{
    if (row > mRealRow || col > m_cols || row == -1 || col == -1)
        return INVALIDREC;
    ScrollAreaWidget *cenw = static_cast<ScrollAreaWidget *>(widget());
    if (!cenw)
        return INVALIDREC;
    // int areaW = newWH.first->width();
    // int areaH = newWH.first->height();
    //   int wPart = areaW / m_cols;
    //   int hPart = areaH / m_rows;
    QPair<int, int> &newWH = getSingleWithHeight();
    QRect tmpRec(SCROLLAREAMARGIN + newWH.first * col, SCROLLAREAMARGIN + newWH.second * row, newWH.first,
                 newWH.second);
    return tmpRec;
}

QPair<int, int> PlotScrollArea::calCusorPos(const QPoint &pos)
{
    QPair<int, int> tmpP = qMakePair<int, int>(-1, -1);
    ScrollAreaWidget *cenw = static_cast<ScrollAreaWidget *>(widget());
    if (!cenw)
        return tmpP;
    // int areaW = newWH.first->width();
    // int areaH = newWH.first->height();
    //   int wPart = areaW / m_cols;
    //   int hPart = areaH / m_rows;
    QPair<int, int> &newWH = getSingleWithHeight();
    for (int row = 0; row < mRealRow; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            QRect tmpRec(SCROLLAREAMARGIN + newWH.first * col, SCROLLAREAMARGIN + newWH.second * row, newWH.first,
                         newWH.second);
            if (tmpRec.contains(pos)) {
                tmpP.first = row;
                tmpP.second = col;
                return tmpP;
            }
        }
    }
    return tmpP;
}

void PlotScrollArea::clearDragValue()
{
    mSrcPlotWidget = nullptr;
    mIsLeftBtnClicked = false;
    mMouseClickPos = INVALIDPOS;
    mMouseMoved = false;
    mDstPos.first = -1;
    mDstPos.second = -1;
    ScrollAreaWidget *areaWidget = static_cast<ScrollAreaWidget *>(widget());
    if (!areaWidget)
        return;
    areaWidget->cancleSelect();
    areaWidget->setCursor(INVALIDPOS);
}

QPair<int, int> PlotScrollArea::getSingleWithHeight()
{
    int h = (size().height() - 2 * SCROLLAREAMARGIN) / m_row_split;
    int w = (widget()->size().width() - 2 * SCROLLAREAMARGIN) / m_cols;
    w += 1;
    h = h < MINPLOTWIDTH ? MINPLOTWIDTH : h;
    return qMakePair<int, int>(w, h);
}

void PlotScrollArea::emitSelecSignal()
{
    Q_EMIT selectPlot();
}

int PlotScrollArea::calRealRow()
{
    int realRow = m_widgetList.size() / m_cols;
    if (m_widgetList.size() % m_cols != 0)
        ++realRow;
    return realRow;
}

bool PlotScrollArea::event(QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ent = (QKeyEvent *)e;
        if (ent->key() == Qt::Key_Control) {
            m_isCtrlPress = true;
        }
        // if (ent->modifiers() == Qt::ControlModifier && ent->key() == Qt::Key_A) {
        //     allSelect();
        // }

        if (ent->key() == Qt::Key_Escape) {
            for (int i = 0; i < m_widgetList.size(); ++i) {
                if (!m_widgetList[i])
                    continue;
                PlotWidget *w = TOPW(m_widgetList[i]);
                if (!w)
                    continue;

                w->setSelectState(false);
                if (m_isSelectedSyncMode) {
                    m_plotSyncOperator.remove(w->plot());
                }
            }
            // Q_EMIT selectPlot();
        }

    } else if (e->type() == QEvent::KeyRelease) {
        QKeyEvent *ent = (QKeyEvent *)e;
        if (ent->key() == Qt::Key_Control) {
            m_isCtrlPress = false;
        }
    }
    return QScrollArea::event(e);
}

ScrollAreaWidget::ScrollAreaWidget(QWidget *parent /*= nullptr*/)
    : QWidget(parent), m_cols(-1), m_rows(-1), mRub(nullptr), mCursor(INVALIDPOS)
{
    mRub = new QRubberBand(QRubberBand::Rectangle, this);
}

ScrollAreaWidget::~ScrollAreaWidget() { }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ScrollAreaWidget::cancleSelect()
{
    if (mRub)
        mRub->hide();
}

void ScrollAreaWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    p.save();
    QPen tmpPen(Qt::gray);
    tmpPen.setStyle(Qt::SolidLine);
    tmpPen.setWidth(1);
    p.setPen(tmpPen);

    if (m_rows == -1 || m_cols == -1) {
        p.restore();
        return;
    }

    int areaW = this->width() - 2 * SCROLLAREAMARGIN;
    int areaH = this->height() - 2 * SCROLLAREAMARGIN;

    int wPart = areaW / m_cols;
    int hPart = areaH / m_rows;
    wPart += 1;
    // 绘制布局线
    // for (int i = 0; i < m_rows - 1; ++i) {
    //    QPoint st(0, hPart * (i + 1));
    //    QPoint ed(areaW, hPart * (i + 1));

    //    p.drawLine(st, ed);
    //}

    // for (int i = 0; i < m_cols - 1; ++i) {
    //     QPoint st(wPart * (i + 1), 0);
    //     QPoint ed(wPart * (i + 1), areaH);

    //    p.drawLine(st, ed);
    //}

    if (mCursor != INVALIDPOS) {
        bool isFinded = false;
        for (int row = 0; row < m_rows; ++row) {
            if (isFinded)
                break;
            for (int col = 0; col < m_cols; ++col) {
                QRect tmpRec(SCROLLAREAMARGIN + wPart * col, SCROLLAREAMARGIN + hPart * row, wPart, hPart);
                if (tmpRec.contains(mCursor)) {
                    mRub->setGeometry(tmpRec);
                    mRub->show();
                    Q_EMIT currentIndex(row, col);
                    isFinded = true;
                    break;
                }
            }
        }
    }
    p.restore();
}

PosRecord::PosRecord()
{
    mRc.first = -1;
    mRc.second = -1;
}

PosRecord::~PosRecord() { }

void PosRecord::setRC(int r, int c)
{
    mRc.first = r;
    mRc.second = c;
}

const QPair<int, int> &PosRecord::getRc() const
{
    return mRc;
}

void PosRecord::clear()
{
    mPos.clear();
}

void PosRecord::recordPlotPos(const QString &id, const QPair<int, int> &rc)
{
    mPos[id] = rc;
}

QPair<int, int> PosRecord::getPlotPos(const QString &id) const
{
    if (!mPos.contains(id))
        return QPair<int, int>(-1, -1);
    return mPos[id];
}

bool PosRecord::isExit(const QString &id) const
{
    if (mPos.contains(id))
        return true;

    return false;
}

bool PosRecord::checkError()
{
    bool haserror = false;
    if (mRc.first == 0 || mRc.second == 0 || mRealRow == 0)
        haserror = true;
    for (auto x = mPos.begin(); x != mPos.end();) {
        if (x.value().first == -1 || x.value().second == -1) {
            x = mPos.erase(x);
            haserror = true;
        } else {
            ++x;
        }
    }
    return haserror;
}

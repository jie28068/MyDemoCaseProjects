#include "PlotWidget.h"
#include "CoreLib/GlobalConfigs.h"
#include "DataAnalyseTableView.h"
#include <QApplication>
#include <QColorDialog>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>

PlotWidget::PlotWidget(QString strTitle, QWidget *parent)
    : QWidget(parent),
      m_plot(nullptr),
      m_toolbar(nullptr),
      m_markerTableView(nullptr),
      m_isShowMarkerTableView(false),
      m_tabWidget(nullptr),
      m_dataAnalyseTableView(nullptr),
      m_markerDlg(nullptr),
      mIsSelected(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins(2, 2, 2, 2));
    layout->setSpacing(0);

    if (gConfGet(IsShowCorsorTableView).toBool()) {
        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        splitter->setHandleWidth(4);
        splitter->setChildrenCollapsible(false);

        layout->addWidget(splitter);
        m_plot = new ItemPlot(strTitle);
        splitter->addWidget(m_plot);

        m_markerDlg = new MarkerDialog(this);
        m_markerTableView = new MarkerTableView(m_markerDlg);

        m_markerDlg->setTitle(tr(""));
        m_markerDlg->setContentsMargins(0, 0, 0, 0);
        m_markerDlg->setEnableDoubleClicked2FullScreen(false);
        m_markerDlg->setResizeAble(false);

        // qDebug() << "parent width:" << parent->width();
        /*int w = (width()/4)*3;*/
        /*m_markerDlg->setMaximumWidth(200);
        m_markerDlg->setMinimumWidth(500);*/

        m_markerDlg->setFixedWidth(230);

        m_tabWidget = new QTabWidget(this);
        splitter->addWidget(m_tabWidget);
        // m_markerTableView = new MarkerTableView;
        m_tabWidget->addTab(m_markerTableView, tr("Nonius")); // 游标
        m_dataAnalyseTableView = new DataAnalyseTableView;
        m_tabWidget->addTab(m_dataAnalyseTableView, tr("Maximum")); // 最值

        m_markerDlg->setCentralWidget(m_tabWidget);
        m_markerDlg->hide();
        splitter->addWidget(m_markerDlg);

        splitter->setStretchFactor(0, 3);
        splitter->setStretchFactor(1, 1);
        setMarkerTableViewShow(m_isShowMarkerTableView);
        m_plot->setDataAnalyseObserver(m_dataAnalyseTableView);
    } else {
        m_plot = new ItemPlot(strTitle);
        layout->addWidget(m_plot);
    }
    privateConnect();
    m_toolbar = new PlotToolbar(this);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(2, 2);
    shadow->setColor(QColor(128, 128, 128, 191));
    shadow->setBlurRadius(10);
    m_toolbar->setGraphicsEffect(shadow);
    m_toolbar->show(); // 这里先show再hide才能保证里面按钮分布正常，原因未知
    m_toolbar->hide();
    void (PlotToolbar::*triger)(int) = &PlotToolbar::triggered;
    void (ItemPlot::*sendSignal)(int) = &ItemPlot::toolBarTriggerSignal;
    connect(m_toolbar, triger, m_plot, sendSignal);
}

PlotWidget::~PlotWidget() { }

void PlotWidget::setMarkerTableViewShow(bool b)
{
    m_isShowMarkerTableView = b;
    if (!m_isShowMarkerTableView) {
        m_markerDlg->hide();
    } else {
        m_markerDlg->show();
    }
}

void PlotWidget::showDataAnalyseInfoPage(DataInfoType dit)
{
    if (!m_tabWidget)
        return;
    if (!m_isShowMarkerTableView) {
        // m_plot->updateToolbarButtonState();
        m_toolbar->setShowOtherPageActionCheckedState(true);
        setMarkerTableViewShow(true);
    }
    if (dit >= 0 && dit < m_tabWidget->count())
        m_tabWidget->setCurrentIndex((int)dit);
    else
        m_tabWidget->setCurrentIndex(0);
}

void PlotWidget::setSelectState(bool Seleted)
{
    mIsSelected = Seleted;
    Q_EMIT selectStateChange();
    update();
}

QPair<int, int> PlotWidget::getPos()
{
    QPair<int, int> temp;
    temp.first = -1;
    temp.second = -1;

    if (!m_plot)
        return temp;

    QReadWriteLock &plotLock = m_plot->getPlotLock();
    QReadLocker guard(&plotLock);
    PlotInfo &pIn = m_plot->getPlotInfo();
    temp.first = pIn.mRow;
    temp.second = pIn.mCol;
    return temp;
}

void PlotWidget::setPos(int row, int col)
{
    if (!m_plot)
        return;

    QReadWriteLock &plotLock = m_plot->getPlotLock();
    QWriteLocker guard(&plotLock);
    PlotInfo &pIn = m_plot->getPlotInfo();
    pIn.mRow = row;
    pIn.mCol = col;
}

void PlotWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    QList<QToolButton *> allBtn = m_toolbar->findChildren<QToolButton *>();
    for (auto &x : allBtn) {
        if (x->testAttribute(Qt::WA_UnderMouse)) {
            x->setAttribute(Qt::WA_UnderMouse, false);
            QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(30, 30), QPoint(5, 5));
            QApplication::sendEvent(x, &hoverEvent);
            x->update();
        }
    }
    m_toolbar->update();
    m_toolbar->raise();
    m_toolbar->show();
}

void PlotWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    m_toolbar->hide();
}

// isShow为true时一直显示，为false是则根据原来状态置反
void PlotWidget::onSetMarkerTableViewShow(bool isShow)
{
    if (!m_markerTableView)
        return;
    if (isShow)
        m_isShowMarkerTableView = isShow;
    else
        m_isShowMarkerTableView = !m_isShowMarkerTableView;
    setMarkerTableViewShow(m_isShowMarkerTableView);
    m_tabWidget->setCurrentIndex(0); //????????,????????
}

void PlotWidget::hideMarkerTable()
{
    setMarkerTableViewShow(false);
}

void PlotWidget::privateConnect()
{
    QObject::connect(m_plot, SIGNAL(setMarkerTableViewShow(bool)), this, SLOT(onSetMarkerTableViewShow(bool)));

    if (m_markerTableView) {
        QObject::connect(m_plot->canvas(), SIGNAL(addMarkerSignal(const MarkerInfo &)), m_markerTableView->getModel(),
                         SLOT(addMarker(const MarkerInfo &)));
        QObject::connect(m_plot->canvas(), SIGNAL(setMarkerInfoSignal(const QList<MarkerInfo> &)),
                         m_markerTableView->getModel(), SLOT(setMarkerInfo(const QList<MarkerInfo> &)));
        QObject::connect(m_plot->canvas(),
                         SIGNAL(refreshMarkerPoint(const QString &, qreal, const QList<MarkerPoint> &)),
                         m_markerTableView->getModel(),
                         SLOT(refreshMarkerPoint(const QString &, qreal, const QList<MarkerPoint> &)));

        QObject::connect(m_markerTableView->getModel(), SIGNAL(modifyMarkerName(const QString &, const QString &)),
                         m_plot->canvas(), SLOT(onModifyMarkerName(const QString &, const QString &)));
        QObject::connect(m_markerTableView->getModel(), SIGNAL(modifyMarkerColor(const QString &, const QColor &)),
                         m_plot->canvas(), SLOT(onModifyMarkerColor(const QString &, const QColor &)));
        QObject::connect(m_markerTableView->getModel(), SIGNAL(modifyMarkerIsShow(const QString &, bool)),
                         m_plot->canvas(), SLOT(onModifyMarkerIsShow(const QString &, bool)));
        QObject::connect(m_markerTableView->getModel(), SIGNAL(removeMarker(const QString &)), m_plot->canvas(),
                         SLOT(onRemoveMarker(const QString &)));

        QObject::connect(m_plot, SIGNAL(setCurveNames(const QStringList &)), m_markerTableView->getModel(),
                         SLOT(setCurveNames(const QStringList &)));
        QObject::connect(m_plot, SIGNAL(setAxisDecimal(int, int)), m_markerTableView->getModel(),
                         SLOT(setAxisDecimal(int, int)));

        QObject::connect(m_markerTableView->getModel(), SIGNAL(hideView()), this,
                         SLOT(hideMarkerTable())); // 2022.11.15
    }

    if (m_markerDlg)
        QObject::connect(m_markerDlg, SIGNAL(closed(int)), this,
                         SLOT(hideMarkerTable())); // 关闭窗口的时候，隐藏游标列表	2022.11.17
}

void PlotWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    if (!mIsSelected)
        return;
    QPainter p(this);
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(QColor("#2ca6e0"));
    pen.setWidth(2);

    QVector<qreal> pattern;
    pattern.append(3);
    pattern.append(4);
    pen.setDashPattern(pattern);

    p.setPen(pen);
    p.drawRect(1, 1, width() - 2, height() - 2);
    p.restore();
}

//////////////////////////////////////////////////////////////////////////////////////

MarkerTableModel::MarkerTableModel(QObject *parent /*= nullptr*/)
    : QAbstractTableModel(parent), m_xDecimal(-1), m_yDecimal(-1)
{
    m_headerList << tr("") << tr("") << tr("") << tr("Nonius") //"游标"
                 << tr("Position");                            //"位置";

    m_typeIcon[0] = QIcon(":/track");
    m_typeIcon[1] = QIcon(":/image/Y轴游标.png");
    m_typeIcon[2] = QIcon(":/image/X轴游标.png");

    for (int i = 1; i <= gMaxMarketNum; ++i) {
        xAxisPos.push_back(i);
        yAxisPos.push_back(i);
        trackPos.push_back(i);
    }

    pModified = new IsModified;
}

MarkerTableModel::~MarkerTableModel() { }

int MarkerTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_markerList.size();
}

int MarkerTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_headerList.size();
}

QVariant MarkerTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_markerList.size())
        return QVariant();

    int col = index.column();
    int row = index.row();
    if (col == 0) {
        if (role == Qt::ToolTipRole) {
            return tr("Delete"); // 删除
        }
    } else if (col == 1) {
        if (role == Qt::DecorationRole) {
            return m_typeIcon[(int)m_markerList[row].type];
        }
    } else if (col == 2) {
        /*if(role == Qt::DecorationRole)
        {
        QPixmap pixmap(16,16);
        pixmap.fill(m_markerList[row].color);
        return pixmap;
        }
        else*/
        if (role == Qt::EditRole) {
            return m_markerList[row].color;
        } else if (role == Qt::ToolTipRole) {
            return tr("Color"); // 颜色
        }
    } else if (col == 3) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return m_markerList[row].name;
        } else if (role == Qt::CheckStateRole) {
            return m_markerList[row].isShow ? Qt::Checked : Qt::Unchecked;
        }
    } else if (col == 4) {
        if (role == Qt::DisplayRole) {
            qreal pos_tmp;
            pos_tmp = m_markerList[row].pos;
            /*if(m_markerList[row].xBottomIsLog && m_markerList[row].type != HLine)
                    pos_tmp = pow(10,m_markerList[row].pos);*/
            return formatValue(m_markerList[row].type != HLine ? 0 : 1, pos_tmp);
        }
    } else if (col > 4 && col <= m_curveNames.size() + 4) {
        if (m_markerList[row].type == VLine) {
            if (col - 5 < m_markerList[row].points.size() && role == Qt::DisplayRole) {
                if (m_markerList[row].points[col - 5].isOtherData) {
                    if (m_markerList[row].points[col - 5].otherData != 0xffffffff) // QuiKIS项目专用判断
                    {
                        QString strTemp(formatValue(m_markerList[row].type != HLine ? 0 : 1,
                                                    m_markerList[row].points[col - 5].value.y())
                                        + "[" + tr("frequency") + ":"
                                        + formatValue(m_markerList[row].type != HLine ? 0 : 1,
                                                      m_markerList[row].points[col - 5].otherData)); // 频率

                        return strTemp;
                    }
                }
                return formatValue(m_markerList[row].type != HLine ? 0 : 1,
                                   m_markerList[row].points[col - 5].value.y());
            }
        }
    }

    return QVariant();
}

bool MarkerTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    if (!index.isValid() || index.row() >= m_markerList.size())
        return false;

    int col = index.column();
    int row = index.row();

    if (col == 3) {
        if (role == Qt::CheckStateRole) {
            m_markerList[row].isShow = value.toInt();

            emit modifyMarkerIsShow(m_markerList[row].id, m_markerList[row].isShow);
        } else if (role == Qt::EditRole) {
            m_markerList[row].name = value.toString();
            emit modifyMarkerName(m_markerList[row].id, m_markerList[row].name);
        }
    } else if (col == 0) {
        QString name = m_markerList[row].name;
        if (!name.isEmpty()) {
            int type = m_markerList[row].type;
            if (type == VLine2) {
                name.remove(0, 3);
                int pos = name.toInt();
                pModified->x = true;
                xAxisPos.append(pos);
            } else if (type == HLine) {
                name.remove(0, 3);
                int pos = name.toInt();
                pModified->y = true;
                yAxisPos.append(pos);
            } else if (type == VLine) {
                name.remove(0, 2);
                int pos = name.toInt();
                pModified->track = true;
                trackPos.append(pos);
            }
        }

        QString markerId = m_markerList[row].id;
        beginRemoveRows(QModelIndex(), row, row);
        m_markerList.removeAt(row);
        endRemoveRows();
        emit removeMarker(markerId);

        if (m_markerList.isEmpty())
            emit hideView();

        curveColumnsShowOrHide();
        return true;
    } else if (col == 2) {
        if (role == Qt::EditRole) {
            m_markerList[row].color = value.value<QColor>();
            emit modifyMarkerColor(m_markerList[row].id, m_markerList[row].color);
        }
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags MarkerTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    Qt::ItemFlags itemFlag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == 3) {
        itemFlag |= Qt::ItemIsUserCheckable;

        if ((PlotProject)gConfGet("PlotProject").toInt() != QuiKIS)
            itemFlag |= Qt::ItemIsEditable;
    }

    return itemFlag;
}

QVariant MarkerTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (section < m_headerList.size() && role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headerList[section];
    return QVariant();
}

void MarkerTableModel::setMarkerInfo(const QList<MarkerInfo> &info)
{
    beginResetModel();
    // m_markerList.clear();
    // foreach(const MarkerInfo& i, info)
    //{
    //	if(i.type == MarkerType::HLine)continue;
    //	m_markerList.append(i);
    // }
    m_markerList = info;
    endResetModel();
}

QList<MarkerInfo> MarkerTableModel::getMarkerInfo(void) const
{
    return m_markerList;
}

void MarkerTableModel::setCurveNames(const QStringList &curveNames)
{
    beginResetModel();
    for (int i = 0; i < m_markerList.size(); i++) {
        m_markerList[i].points.clear();
    }
    m_headerList.clear();
    m_curveNames = curveNames;
    m_headerList << tr("") << tr("") << tr("") << tr("Nonius") //"游标"
                 << tr("Position") << curveNames;              //"位置"
    endResetModel();

    curveColumnsShowOrHide();
}

void MarkerTableModel::addMarker(const MarkerInfo &info)
{
    beginInsertRows(QModelIndex(), m_markerList.size(), m_markerList.size());

    if (info.name.isEmpty()) {
        if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) // 2022.11.10
        {
            int index = 0;
            QString prefix;
            if (VLine == info.type) {
                if (pModified->track)
                    qSort(trackPos.begin(), trackPos.end());

                prefix = tr("Trace");   // 追踪
                if (trackPos.isEmpty()) // 达到最大允许数量
                    return;
                index = *(trackPos.begin());
                trackPos.removeFirst();
            } else if (VLine2 == info.type) {
                if (pModified->x)
                    qSort(xAxisPos.begin(), xAxisPos.end());

                prefix = tr("X-axis cursor"); // X游标
                if (xAxisPos.isEmpty())       // 达到最大允许数量
                    return;
                index = *(xAxisPos.begin());
                xAxisPos.removeFirst();
            } else if (HLine == info.type) {
                if (pModified->y)
                    qSort(yAxisPos.begin(), yAxisPos.end());

                prefix = tr("Y-axis cursor"); // Y游标
                if (yAxisPos.isEmpty())       // 达到最大允许数量
                    return;
                index = *(yAxisPos.begin());
                yAxisPos.removeFirst();
            }

            // qSort(xAxisPos.begin(), xAxisPos.end());
            m_markerList.append(info);
            m_markerList[m_markerList.size() - 1].name = QString("%1%2").arg(prefix).arg(index); // m_markerList.size()

        } else {
            m_markerList.append(info);
            m_markerList[m_markerList.size() - 1].name = tr("Nonius") + QString::number(m_markerList.size()); // 游标
        }
    }
    endInsertRows();
    curveColumnsShowOrHide();
}

void MarkerTableModel::refreshMarkerPoint(const QString &markerId, qreal pos, const QList<MarkerPoint> &points)
{
    QList<MarkerInfo>::iterator iter = m_markerList.begin();
    int k = 0;
    for (; iter != m_markerList.end(); iter++, k++) {
        if (iter->id == markerId)
            break;
    }
    if (iter == m_markerList.end()) {
        return;
    }
    if (m_curveNames.size() > 0) {
        if (iter->points.size() == 0 && m_curveNames.size() > 0) {
            iter->points.resize(m_curveNames.size());
        }
        for (int i = 0; i < m_curveNames.size(); i++) {
            for (int j = 0; j < points.size(); j++) {
                if (m_curveNames[i] == points[j].name) {
                    iter->points[i] = points[j];
                    break;
                }
            }
        }
    }
    iter->pos = pos;
    emit dataChanged(createIndex(k, 4), createIndex(k, 4 + m_curveNames.size()));
}

void MarkerTableModel::setAxisDecimal(int xDecimal, int yDecimal)
{
    m_xDecimal = xDecimal;
    m_yDecimal = yDecimal;

    beginResetModel();
    endResetModel();
}

QString MarkerTableModel::formatValue(int axis, qreal value) const
{
    int decimal = axis == 0 ? m_xDecimal : m_yDecimal;
    if (decimal < 0) {
        return QString::number(value);
    }
    return QString::number(value, 'f', decimal);
}

void MarkerTableModel::curveColumnsShowOrHide(void)
{
    bool bShow = false;
    for (MarkerInfo &marker : m_markerList) {
        if (marker.type == VLine) {
            bShow = true;
            break;
        }
    }
    MarkerTableView *tableView = (MarkerTableView *)QObject::parent();
    for (int i = 0; i < m_curveNames.size(); i++) {
        if (bShow) {
            tableView->showColumn(5 + i);
        } else {
            tableView->hideColumn(5 + i);
        }
    }
}

MarkerTableView::MarkerTableView(QWidget *parent /*=nullptr*/) : QTableView(parent), m_model(new MarkerTableModel(this))
{
    setMaximumWidth(800);
    this->setModel(m_model);
    setIconSize(QSize(20, 20));
    // this->header()->moveSection(4,2);
    this->setItemDelegateForColumn(0, new MarkerTableButtonDelegate(this));
    // ColorComboBoxItemDelegate *colorComboBoxItemDelegate = new ColorComboBoxItemDelegate(this);
    // colorComboBoxItemDelegate->setIconSize(QSize(16,16));
    // this->setItemDelegateForColumn(1,colorComboBoxItemDelegate);
    this->setItemDelegateForColumn(2, new MarkerTableColorDelegate(this));
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    this->setStyleSheet("font-size:12px;");

    verticalHeader()->setVisible(false); // 2022.11.10 隐藏游标列表竖直表头
    horizontalHeader()->setStyleSheet("QHeaderView::section{backgroung:skyblue;color:black;"
                                      "border-left:0px solid #E5E5E5;"
                                      "border-top:0px solid #E5E5E5;"
                                      "border-right:0.5px solid #E5E5E5;"
                                      "border-bottom:0.5px solid #E5E5E5;"
                                      "padding:4px;}");
}

MarkerTableView::~MarkerTableView() { }

MarkerTableButtonDelegate::MarkerTableButtonDelegate(QObject *parent /*= nullptr*/)
    : m_removeButton(new QStyleOptionButton), QStyledItemDelegate(parent)
{
    initButton();
}

void MarkerTableButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->fillRect(option.rect, Qt::white);
    int w = 16;
    int h = 16;
    m_removeButton->rect.setRect((option.rect.width() - w) / 2 + option.rect.left(),
                                 (option.rect.height() - h) / 2 + option.rect.top(), w, h);

    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, m_removeButton.data(), painter);
}

bool MarkerTableButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                            const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {

    } break;
    case QEvent::MouseButtonRelease: {
        /*QSize size = index.data(Qt::SizeHintRole).toSize();
        qDebug() << "size:" << size;*/
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        /*qDebug() << "mouse pos:" << mouseEvent->pos();
        qDebug() << "mouse pos:" << option.rect;
        qDebug() << "m_removeButton->rect:" << m_removeButton->rect;*/
        if (option.rect.contains(mouseEvent->pos())) //
        {
            model->setData(index, QVariant());
        }
    } break;
    case QEvent::MouseMove: {
        /*QMouseEvent *mouseEvent = (QMouseEvent*)event;
        if(m_removeButton->rect.contains(mouseEvent->pos()))
        {
        QApplication::setOverrideMarker(Qt::PointingHandCursor);
        }*/
    } break;
    case QEvent::Leave: {
        // QApplication::setOverrideCursor(Qt::ArrowCursor);
    } break;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void MarkerTableButtonDelegate::initButton(void)
{
    m_removeButton->text = "";
    m_removeButton->state |= QStyle::State_Enabled;
    m_removeButton->icon = QIcon(":/image/清空.png");
    m_removeButton->iconSize = QSize(16, 16);
    // m_clearButton->state |= QStyle::State_Sunken;
    m_removeButton->palette.setColor(QPalette::ButtonText, Qt::blue);
}
///////////////////////////////////////////////////////////////////////////////////
MarkerTableColorDelegate::MarkerTableColorDelegate(QObject *parent /*= nullptr*/) : QStyledItemDelegate(parent) { }

void MarkerTableColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->fillRect(option.rect, Qt::white);
    int w = 16;
    int h = 16;
    QRect rc((option.rect.width() - w) / 2 + option.rect.left(), (option.rect.height() - h) / 2 + option.rect.top(), w,
             h);

    // painter->save();

    QColor color = index.data(Qt::EditRole).value<QColor>();
    painter->fillRect(rc, color);

    // painter->restore();
}

bool MarkerTableColorDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                           const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
    } break;
    case QEvent::MouseButtonRelease: {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        int w = 16;
        int h = 16;
        QRect rc((option.rect.width() - w) / 2 + option.rect.left(), (option.rect.height() - h) / 2 + option.rect.top(),
                 w, h);
        if (rc.contains(mouseEvent->pos())) {
            QColorDialog colorDialog;
            QColor color = index.data(Qt::EditRole).value<QColor>();
            color = QColorDialog::getColor(color);
            if (color.isValid()) {
                model->setData(index, color, Qt::EditRole);
            }
        }
    } break;
    case QEvent::MouseMove: {
    } break;
    case QEvent::Leave: {
    } break;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

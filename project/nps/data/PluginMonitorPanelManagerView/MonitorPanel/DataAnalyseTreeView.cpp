#include "DataAnalyseTreeView.h"
#include "CoreLib/GlobalConfigs.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include <QToolTip>

KCC_USE_NAMESPACE_MONITORPANEL

DataAnalyseTreeItem::DataAnalyseTreeItem(DataAnalyseTreeItem *parent /*=nullptr*/) : m_parentItem(parent), m_level(0)
{
    if (m_parentItem) {
        m_level = m_parentItem->level() + 1;
    }
}
DataAnalyseTreeItem::~DataAnalyseTreeItem()
{
    qDeleteAll(m_childItems);
}

QVariant DataAnalyseTreeItem::data(int col, int role)
{
    switch (col) {
    case 0: {
        if (role == Qt::DisplayRole) {
            return m_itemData[0];
        }
    } break;
    case 1: {
        if (role == Qt::DisplayRole) {
            if (m_level > 1) {
                return m_itemData[1];
            }
        }
    } break;
    default:
        break;
    }

    if (role == Qt::FontRole) {
        QFont font;
        font.setPixelSize(12);
        return font;
    }

    return QVariant();
}

Qt::ItemFlags DataAnalyseTreeItem::flags(int col)
{
    Qt::ItemFlags itemFlag = Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/;
    return itemFlag;
}

DataAnalyseTreeItem *DataAnalyseTreeItem::child(int i)
{
    if (i < 0 || i >= m_childItems.size())
        return nullptr;

    return m_childItems[i];
}

DataAnalyseTreeItem *DataAnalyseTreeItem::parent(void)
{
    return m_parentItem;
}

int DataAnalyseTreeItem::childCount(void) const
{
    return m_childItems.size();
}

int DataAnalyseTreeItem::pos(void) const
{
    if (!m_parentItem)
        return 0;
    return m_parentItem->m_childItems.indexOf(const_cast<DataAnalyseTreeItem *>(this));
}

bool DataAnalyseTreeItem::setData(int col, const QVariant &val, int role)
{
    if (col < 0 || col >= 1)
        return false;

    return true;
}

int DataAnalyseTreeItem::columnCount(void) const
{
    return 2;
}

bool DataAnalyseTreeItem::append(DataAnalyseTreeItem *item)
{
    m_childItems.append(item);
    return true;
}

bool DataAnalyseTreeItem::remove(int i)
{
    if (i < 0 || i >= m_childItems.size())
        return false;
    delete m_childItems.takeAt(i);
    return true;
}

bool DataAnalyseTreeItem::remove(DataAnalyseTreeItem *item)
{
    QList<DataAnalyseTreeItem *>::iterator iter;
    for (iter = m_childItems.begin(); iter != m_childItems.end(); iter++) {
        if ((*iter) == item)
            break;
    }
    if (iter == m_childItems.end())
        return false;

    delete (*iter);
    m_childItems.erase(iter);
    return true;
}

int DataAnalyseTreeItem::level(void) const
{
    return m_level;
}

void DataAnalyseTreeItem::setLevel(int n)
{
    m_level = n;
}

void DataAnalyseTreeItem::setIcon(QString path)
{
    m_icon.addFile(path);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
DataAnalyseTreeModel::DataAnalyseTreeModel(QObject *parent) : QAbstractItemModel(parent), m_rootItem(nullptr)
{
    m_headerList << tr("Name")   //"名称
                 << tr("Value"); //"值";
    m_rootItem = new DataAnalyseTreeItem();
}

DataAnalyseTreeModel::~DataAnalyseTreeModel()
{
    if (m_rootItem)
        delete m_rootItem;
}

QVariant DataAnalyseTreeModel::headerData(int section, Qt::Orientation orientation,
                                          int role /*= Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            return m_headerList[section];
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant DataAnalyseTreeModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    DataAnalyseTreeItem *item = itemFromIndex(index);

    return item->data(index.column(), role);
}

Qt::ItemFlags DataAnalyseTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    DataAnalyseTreeItem *item = itemFromIndex(index);

    return item->flags(index.column());
}

QModelIndex DataAnalyseTreeModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex() */) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DataAnalyseTreeItem *parentItem = itemFromIndex(parent);
    if (!parentItem)
        return QModelIndex();

    DataAnalyseTreeItem *childItem = parentItem->child(row);
    if (!childItem)
        return QModelIndex();

    return createIndex(row, column, childItem);
}

QModelIndex DataAnalyseTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    DataAnalyseTreeItem *childItem = itemFromIndex(index);
    DataAnalyseTreeItem *parentItem = childItem->parent();
    if (!parentItem || parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->pos(), 0, parentItem);
}

int DataAnalyseTreeModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return itemFromIndex(parent)->childCount();
}

int DataAnalyseTreeModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_headerList.size();
}

bool DataAnalyseTreeModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    DataAnalyseTreeItem *item = itemFromIndex(index);
    if (!item)
        return false;

    bool b = item->setData(index.column(), value, role);
    emit dataChanged(index, index);
    return true;
}

bool DataAnalyseTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                                         int role /*= Qt::EditRole*/)
{
    if (orientation != Qt::Horizontal || role != Qt::EditRole)
        return false;
    if (value == headerData(section, orientation, role))
        return false;

    m_headerList.replace(section, value.toString());
    emit headerDataChanged(orientation, section, section);
    return true;
}

DataAnalyseTreeItem *DataAnalyseTreeModel::root(void) const
{
    return m_rootItem;
}

DataAnalyseTreeItem *DataAnalyseTreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<DataAnalyseTreeItem *>(index.internalPointer());
    }

    return m_rootItem;
}

void DataAnalyseTreeModel::refresh(void)
{
    beginResetModel();
    endResetModel();
}

void DataAnalyseTreeModel::addDateAnalyseInfo(QString &curveName, DataAnalyseInfo &info)
{
    DataAnalyseTreeItem *parentItem = new DataAnalyseTreeItem(m_rootItem);
    m_rootItem->append(parentItem);
    parentItem->setItemData(QList<QVariant>() << curveName);

    QVariant var = GlobalConfigs::getInstance()->get("PlotProject");
    PlotProject plotPro = Default;
    if (var.isValid()) {
        int plotProI = var.toInt();
        plotPro = (PlotProject)plotProI;
    }
    DataAnalyseTreeItem *childItem = new DataAnalyseTreeItem(parentItem);
    if (plotPro != QuiKIS) {
        parentItem->append(childItem);
        childItem->setItemData(QList<QVariant>() << tr("Sampling Rate") << info.samplingRate); // 采样率
    }

    childItem = new DataAnalyseTreeItem(parentItem);
    if (plotPro != QuiKIS) {
        parentItem->append(childItem);
        childItem->setItemData(QList<QVariant>() << tr("Sampling Time") << info.samplingTime); // 采样时间
    }

    childItem = new DataAnalyseTreeItem(parentItem);
    parentItem->append(childItem);
    childItem->setItemData(QList<QVariant>() << tr("Maximum Value") << info.maxValue); // 最大值

    childItem = new DataAnalyseTreeItem(parentItem);
    parentItem->append(childItem);
    childItem->setItemData(QList<QVariant>() << tr("Minimum Value") << info.minValue); // 最小值

    childItem = new DataAnalyseTreeItem(parentItem);
    parentItem->append(childItem);
    childItem->setItemData(QList<QVariant>() << tr("Average Value") << info.averageValue); // 平均值
}

void DataAnalyseTreeModel::clear()
{
    delete m_rootItem;
    m_rootItem = new DataAnalyseTreeItem;
}

////////////////////////////////////////////////////////////////////////////////////////
DataAnalyseTreeView::DataAnalyseTreeView(QWidget *parent) : QTreeView(parent), m_model(nullptr)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->header()->setStretchLastSection(true);
    this->header()->hide();

    m_model = new DataAnalyseTreeModel(this);
    setModel(m_model);

    setColumnWidth(1, 64);
    setMouseTracking(true);
    this->header()->setSectionResizeMode(QHeaderView::Fixed);
}

DataAnalyseTreeView::~DataAnalyseTreeView() { }

void DataAnalyseTreeView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint &mousePos = event->pos();

    QModelIndex &mouseIndex = indexAt(mousePos);
    if (!mouseIndex.isValid()) {
        QToolTip::showText(event->globalPos(), "", this);
        return;
    }
    DataAnalyseTreeItem *item = m_model->itemFromIndex(mouseIndex);
    // if (!item || item->level() != 1) {
    //     QToolTip::showText(event->globalPos(), "", this);
    //     return;
    // }
    QString &dataToDisplay = mouseIndex.data(Qt::DisplayRole).toString();
    QToolTip::showText(event->globalPos(), dataToDisplay, this);
}

void DataAnalyseTreeView::focusOutEvent(QFocusEvent *event)
{
    QTreeView::focusOutEvent(event);
    this->hide();
}

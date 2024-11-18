#include "SettingWidget.h"
#include "treeview.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QHeaderView>

const QString TITLE_DISPLAY = QObject::tr("Display Or Not"); // 是否显示
const QString TITLE_NAME = QObject::tr("Title Name");        // 表头名
// 自定义表头
ColSetTableHeaderView::ColSetTableHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent), m_checkState(ColSetTableHeaderView::NoChecked), m_bCheckboxEnable(true)
{
    setSectionsClickable(true);
    setStretchLastSection(true);
    setHighlightSections(false);
    setSectionResizeMode(QHeaderView::Interactive);

    connect(this, SIGNAL(sectionClicked(int)), this, SLOT(onSectionsClicked(int)));
}

ColSetTableHeaderView::~ColSetTableHeaderView() { }

void ColSetTableHeaderView::setColumnCheckable(int col, bool checkable)
{
    if (m_columnCheckedMap.contains(col)) {
        m_columnCheckedMap[col] = checkable;
    } else {
        m_columnCheckedMap.insert(0, checkable);
    }
    viewport()->update();
}

void ColSetTableHeaderView::onSectionsClicked(int index)
{
    if (0 == index) {
        if (m_checkState == CheckState::PartChecked) {
            emit columuSectionClicked(index, true);
        } else if (m_checkState == CheckState::AllChecked) {
            emit columuSectionClicked(index, false);
        } else if (m_checkState == CheckState::NoChecked) {
            emit columuSectionClicked(index, true);
        }
    }
}

void ColSetTableHeaderView::setHeaderCheckStatus(ColSetTableHeaderView::CheckState checkedStatus)
{
    m_checkState = checkedStatus;
    viewport()->update();
}

void ColSetTableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (m_columnCheckedMap.contains(logicalIndex)) {
        QStyleOptionButton styleOption;
        styleOption.rect = QRect(rect.left() + 5, rect.top() + 6, 16, 16);
        styleOption.state = isEnabled() ? QStyle::State_Enabled : QStyle::State_None;

        QCheckBox *pCheckBox = new QCheckBox(dynamic_cast<QWidget *>(this->parent()));

        if (m_checkState == CheckState::PartChecked) {
            styleOption.state |= QStyle::State_NoChange;
        } else if (m_checkState == CheckState::AllChecked) {
            styleOption.state |= QStyle::State_On;
        } else if (m_checkState == CheckState::NoChecked) {
            styleOption.state |= QStyle::State_Off;
        }

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &styleOption, painter, pCheckBox);
    }
}

// 表格
SettingWidget::SettingWidget(QWidget *parent) : QWidget(parent), m_tableview(nullptr), m_settingTableModel(nullptr)
{
    QGridLayout *pgridLayout = new QGridLayout(this);

    QWidget *pwidgetTable = new QWidget(this);
    pwidgetTable->setObjectName(QString::fromUtf8("widgetTable"));
    QGridLayout *gridLayoutTable = new QGridLayout(pwidgetTable);
    gridLayoutTable->setContentsMargins(1, 1, 1, 1);
    gridLayoutTable->setObjectName(QString::fromUtf8("gridLayoutTable"));
    m_tableview = new KTableView(pwidgetTable);
    m_tableview->verticalHeader()->setVisible(false);
    m_pHeaderView = new ColSetTableHeaderView(Qt::Horizontal, m_tableview);
    m_pHeaderView->setEnabled(true);
    m_pHeaderView->setColumnCheckable(0, true);
    m_pHeaderView->setSectionResizeMode(QHeaderView::Stretch);

    m_tableview->setHorizontalHeader(m_pHeaderView);
    // m_tableview->horizontalHeader()->setFixedHeight(34);
    m_tableview->horizontalHeader()->setStretchLastSection(true);
    m_tableview->setObjectName(QString::fromUtf8("tableView"));

    gridLayoutTable->addWidget(m_tableview, 0, 0, 1, 1);
    pgridLayout->addWidget(pwidgetTable, 1, 0, 1, 1);

    m_settingTableModel = new SettingTableModel(m_tableview);
    m_tableview->setModel(m_settingTableModel);
    connect(m_pHeaderView, SIGNAL(columuSectionClicked(const int &, bool)), this,
            SLOT(onColumnAllSelected(const int &, bool)));
    connect(m_settingTableModel, SIGNAL(itemCheckStatusChanged()), this, SLOT(onItemCheckStatusChanged()));
    this->setLayout(pgridLayout);
}

SettingWidget::~SettingWidget() { }

void SettingWidget::setTableTitle(const QList<TableSetItem> &tabletitlelist, const int freezecol)
{
    if (m_settingTableModel != nullptr && m_tableview != nullptr) {
        m_settingTableModel->updateTableSetting(tabletitlelist, freezecol);
        m_pHeaderView->setHeaderCheckStatus(getHeaderCheckStatus(tabletitlelist));
        m_tableview->setModel(m_settingTableModel);
    }
}

QList<TableSetItem> SettingWidget::getCurrentTableSetList()
{
    if (m_settingTableModel != nullptr) {
        return m_settingTableModel->getCurrentTableInfo();
    }
    return QList<TableSetItem>();
}

ColSetTableHeaderView::CheckState SettingWidget::getHeaderCheckStatus(const QList<TableSetItem> &listdata)
{
    ColSetTableHeaderView::CheckState checkstate = ColSetTableHeaderView::NoChecked;
    for (int row = 0; row < listdata.size(); ++row) {
        if (listdata[row].bDisplay) {
            if (ColSetTableHeaderView::NoChecked == checkstate) {
                if (row == 0) {
                    checkstate = ColSetTableHeaderView::AllChecked;
                } else {
                    checkstate = ColSetTableHeaderView::PartChecked;
                    break;
                }
            }
        } else {
            if (ColSetTableHeaderView::AllChecked == checkstate) {
                checkstate = ColSetTableHeaderView::PartChecked;
                break;
            }
        }
    }
    return checkstate;
}

void SettingWidget::onColumnAllSelected(const int &col, bool selected)
{
    if (m_settingTableModel == nullptr || m_pHeaderView == nullptr) {
        return;
    }
    m_settingTableModel->setAllColumnStatus(selected);
    m_pHeaderView->setHeaderCheckStatus(getHeaderCheckStatus(m_settingTableModel->getCurrentTableInfo()));
}

void SettingWidget::onItemCheckStatusChanged()
{
    if (m_settingTableModel == nullptr || m_pHeaderView == nullptr) {
        return;
    }
    m_pHeaderView->setHeaderCheckStatus(getHeaderCheckStatus(m_settingTableModel->getCurrentTableInfo()));
}

// titleinfomap
SettingTableModel::SettingTableModel(QObject *parent /*= nullptr*/) : m_freezeColno(0)
{
    m_listHeader << TITLE_DISPLAY << TITLE_NAME;
}

SettingTableModel::~SettingTableModel() { }

int SettingTableModel::rowCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_tablePropertyList.size();
}

int SettingTableModel::columnCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_listHeader.count();
}

QVariant SettingTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int row = index.row();
    int col = index.column();
    if (m_tablePropertyList.size() <= row || m_listHeader.size() <= col) {
        return QVariant();
    }

    if (Qt::DisplayRole == role || Qt::EditRole == role) {
        switch (col) {
        case TableColumnName:
            return m_tablePropertyList[row].colName;
        default:
            return QVariant();
        }
    } else if (Qt::CheckStateRole == role && SettingTableModel::TableColumnDisplay == col) {
        return m_tablePropertyList[row].bDisplay ? Qt::Checked : Qt::Unchecked;
    } else if (Qt::BackgroundColorRole == role && row < m_freezeColno) {
        return QColor("#f0f0f0");
    }
    return QVariant();
}

Qt::ItemFlags SettingTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    if (SettingTableModel::TableColumnDisplay == index.column()) {
        if (index.row() >= m_freezeColno) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        } else {
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    }
    return QAbstractTableModel::flags(index);
}

bool SettingTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    if (Qt::CheckStateRole == role) {
        QString tatilename = index.sibling(index.row(), SettingTableModel::TableColumnName).data().toString();
        bool oldvalue = (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) ? true : false;
        bool newvalue = (Qt::CheckState(value.toInt()) == Qt::Checked) ? true : false;
        if (oldvalue != newvalue) {
            TableSetItem nameproperty = m_tablePropertyList[index.row()];
            nameproperty.bDisplay = newvalue;
            m_tablePropertyList[index.row()] = nameproperty;
            emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), columnCount() - 1));
            emit itemCheckStatusChanged();
        }
        return true;
    }

    return setData(index, value, role);
}

QVariant SettingTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_listHeader.count());
            return m_listHeader[section];
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void SettingTableModel::updateTableSetting(const QList<TableSetItem> &tablelist, const int &freezecolno)
{
    beginResetModel();
    clear();
    m_tablePropertyList = tablelist;
    m_freezeColno = freezecolno;
    endResetModel();
}

void SettingTableModel::setAllColumnStatus(bool bdisplay)
{
    for (int row = 0; row < m_tablePropertyList.size(); ++row) {
        if (row < m_freezeColno) {
            continue;
        }
        m_tablePropertyList[row].bDisplay = bdisplay;
        emit dataChanged(this->index(row, SettingTableModel::TableColumnDisplay),
                         this->index(row, SettingTableModel::TableColumnDisplay));
    }
}

void SettingTableModel::clear()
{
    beginResetModel();
    m_tablePropertyList.clear();
    m_freezeColno = 0;
    endResetModel();
}

QList<TableSetItem> SettingTableModel::getCurrentTableInfo()
{
    return m_tablePropertyList;
}

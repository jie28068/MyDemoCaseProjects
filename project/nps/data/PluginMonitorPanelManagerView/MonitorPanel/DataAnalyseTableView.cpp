#include "DataAnalyseTableView.h"

DataAnalyseTableView::DataAnalyseTableView(QWidget *parent)
    : QTableView(parent), m_model(new DataAnalyseTableModel(this))
{
    setModel(m_model);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    horizontalHeader()->setMinimumSectionSize(60); // 设置最小宽度
    verticalHeader()->setVisible(false);           // 隐藏游标列表竖直表头
    horizontalHeader()->setStyleSheet("QHeaderView::section{backgroung:skyblue;color:black;"
                                      "border-left:0px solid #E5E5E5;"
                                      "border-top:0px solid #E5E5E5;"
                                      "border-right:0.5px solid #E5E5E5;"
                                      "border-bottom:0.5px solid #E5E5E5;"
                                      "padding:4px;");
}

DataAnalyseTableView::~DataAnalyseTableView() { }

void DataAnalyseTableView::update(const QList<DataAnalyseInfo> &datas)
{
    m_model->setDatas(datas);
}

void DataAnalyseTableView::onRefreshData(const QList<DataAnalyseInfo> &datas)
{
    m_model->setDatas(datas);
}

/////////////////////////////////////////////////////////////////////////////////
DataAnalyseTableModel::DataAnalyseTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_headerList << tr("Name")           //"曲线名"
                 << tr("Minimum value")  //"最小值"
                 << tr("Maximum value"); //"最大值";
}

DataAnalyseTableModel::~DataAnalyseTableModel() { }

int DataAnalyseTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_datas.size();
}

int DataAnalyseTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_headerList.size();
}

QVariant DataAnalyseTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    switch (index.column()) {
    case 0: {
        if (role == Qt::DisplayRole)
            return m_datas[index.row()].curveName;
        else if (role == Qt::DecorationRole) {
            QPixmap pixmap(20, 6);
            pixmap.fill(m_datas[index.row()].curveColor);
            return pixmap;
        }
    } break;
    case 1: {
        if (role == Qt::DisplayRole)
            return m_datas[index.row()].minValue;
    } break;
    case 2: {
        if (role == Qt::DisplayRole)
            return m_datas[index.row()].maxValue;
    } break;
    }
    return QVariant();
}

bool DataAnalyseTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    return false;
}

Qt::ItemFlags DataAnalyseTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return itemFlags;
}

QVariant DataAnalyseTableModel::headerData(int section, Qt::Orientation orientation,
                                           int role /*= Qt::DisplayRole */) const
{
    if (section < m_headerList.size() && role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headerList.at(section);
    return QVariant();
}

void DataAnalyseTableModel::setDatas(const QList<DataAnalyseInfo> &datas)
{
    beginResetModel();
    m_datas = datas;
    endResetModel();
}

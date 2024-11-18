#include "mttablemodel.h"
#include <QtDebug>

MTTableModel::MTTableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

MTTableModel::MTTableModel(MTTableModel &m):
    QAbstractTableModel(nullptr),
    tableData(m.getTableData())
{

}

void MTTableModel::setData(const QList<QVariantMap> &mapList)
{
    beginResetModel();
//    qDebug()<<"mapList"<<mapList;
    tableData = mapList;
    endResetModel();
}

void MTTableModel::setHeaderList(const QStringList &list)
{
    headerList = list;
}

void MTTableModel::setHeaderMap(QMap<QString, QString> map)
{
    headerTranslator = map;
}

void MTTableModel::setDefaultData(int section, const QVariant& data)
{
    defaultDataMap[section] = data;
}

void MTTableModel::setTableData(int section, QString key, QVariant value)
{
    if(section >= tableData.count())
        return;

    tableData[section][key] = value;
    QModelIndex indexTopLeft = index(section, 0);
    QModelIndex indexBottomRight = index(section, columnCount()-1);
    emit dataChanged(indexTopLeft, indexBottomRight);
}

QVariantMap MTTableModel::getCurTableData(int idx)
{
    return tableData[idx];
}

int MTTableModel::headerSection(QString headerName)
{
    return headerList.indexOf(headerName);
}

int MTTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if(tableData.isEmpty())
        return 0;
    return tableData.count();
}

int MTTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(!headerList.isEmpty())
        return headerList.count();

    if(tableData.isEmpty())
        return 0;

    QVariantMap map = tableData.first();
    return map.count();
}

QVariant MTTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole || role== Qt::EditRole)
    {
        QVariantMap map = tableData[index.row()];
        return map.value(headerList.at(index.column()), defaultData(index.column()));
    }
    return QVariant();
}

QVariant MTTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
            return headerTranslator.value(headerList.at(section), headerList.at(section));
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool MTTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

//    qDebug()<<"MTTableModel::setdata"<<tableData[index.row()][headerList[index.column()]]<<value<<role;
    if(role == Qt::EditRole)
    {
        tableData[index.row()][headerList[index.column()]] = value;
        qDebug()<<tableData[index.row()][headerList[index.column()]];
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

const QVariant MTTableModel::defaultData(int section) const
{
    return defaultDataMap.value(section, "");
}

Qt::ItemFlags MTTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled|Qt::ItemIsEditable;
}


/**
 * @brief MTTableModel::getColumn   从过滤规则filter中按列名colName取出一列数据
 * @param colName   需要取出的列名
 * @param filter    代理的过滤model
 * @return
 */
//template<typename T>
QList<QString> MTTableModel::getColumn(QString colName, QSortFilterProxyModel *filter)
{
    QList<QString> list_ret;
    if(filter)
    {
        if(filter->sourceModel() != this)//过滤代理非法
            return list_ret;
    }

    if(tableData.isEmpty())
        return list_ret;

    QStringList colNameList = tableData.at(0).keys();
    if(!colNameList.contains(colName))//列名非法
    {
        return list_ret;
    }

    if(filter)
    {
        for(int i=0; i<filter->rowCount(); i++)
        {
            QModelIndex index = filter->index(i,0);
            QModelIndex srcIndex = filter->mapToSource(index);
            list_ret<<tableData.at(srcIndex.row()).value(colName).value<QString>();
        }
    }
    else
    {
        for(int i=0; i<rowCount(); i++)
        {
            QModelIndex srcIndex = index(i,0);
            list_ret<<tableData.at(srcIndex.row()).value(colName).value<QString>();
        }
    }
    return list_ret;
}

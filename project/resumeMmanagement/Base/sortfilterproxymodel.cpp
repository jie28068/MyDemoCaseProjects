#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

    m_rxCol0 = "";
    m_rxCol1 = "";
    m_rxCol2 = "";
    m_rxCol3 = "";
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    //获取model中实际的数据
//    QString dataColumn0 =  sourceModel()->index(source_row, 0, source_parent).data(Qt::DisplayRole).toString();
//    QString dataColumn1 =  sourceModel()->index(source_row, 1, source_parent).data(Qt::DisplayRole).toString();
//    QString dataColumn2 =  sourceModel()->index(source_row, 2, source_parent).data(Qt::DisplayRole).toString();
//    QString dataColumn3 =  sourceModel()->index(source_row, 3, source_parent).data(Qt::DisplayRole).toString();

//    if(dataColumn0.contains(m_rxCol0)&&dataColumn1.contains(m_rxCol1)&&dataColumn2.contains(m_rxCol2)&&dataColumn3.contains(m_rxCol3))
//    {
//        return true;
//    }
//    return false;
    QModelIndex index0 = sourceModel()->index(source_row, 0, source_parent);
    QModelIndex index1 = sourceModel()->index(source_row, 1, source_parent);
    QModelIndex index2 = sourceModel()->index(source_row, 2, source_parent);
    QModelIndex index3 = sourceModel()->index(source_row, 3, source_parent);
    return (sourceModel()->data(index0).toString().contains(m_rxCol0)
            && sourceModel()->data(index1).toString().contains(m_rxCol1)
            && sourceModel()->data(index2).toString().contains(m_rxCol2)
            && sourceModel()->data(index3).toString().contains(m_rxCol3));


}

void SortFilterProxyModel::setRxCol0(const QString rx)
{
    m_rxCol0 = rx;
}

void SortFilterProxyModel::setRxCol1(const QString rx)
{
    m_rxCol1 = rx;
}

void SortFilterProxyModel::setRxCol2(const QString rx)
{
    m_rxCol2 = rx;
}

void SortFilterProxyModel::setRxCol3(const QString rx)
{
    m_rxCol3 = rx;
}


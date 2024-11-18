#include "CustomSortFilterProxyModel.h"

#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/PublicDefine.h"
using namespace Kcc::BlockDefinition;

// 排序和筛选
////////////////////////////////////////////////////////////////////////////

void CustomSortFilterProxyModel::setFilterString(const QString &strFilter)
{

    m_strFilterString = strFilter;
    invalidateFilter();
}

void CustomSortFilterProxyModel::setFilterColumn(const int &col)
{

    m_FilterColumn = col;
    invalidateFilter();
}

QVariant CustomSortFilterProxyModel::headerData(int section, Qt::Orientation orientation,
                                                int role /*= Qt::DisplayRole */) const
{
    // QModelIndex source_index = mapToSource(index);
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole) {
            return QString::number(section + 1);
        } else {
            return QVariant();
        }
    }
    return sourceModel()->headerData(section, orientation, role);
}

bool CustomSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (m_strFilterString.isEmpty() || !index.isValid() || (m_FilterColumn < 0)
        || (m_FilterColumn >= sourceModel()->columnCount())) {
        return true;
    }

    QString strValue = index.sibling(index.row(), m_FilterColumn).data().toString();
    return strValue.contains(m_strFilterString, Qt::CaseInsensitive);
}

bool CustomSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // 根据数据类型比较
    QString datatype = left.data(NPS::ModelDataTypeRole).toString();
    if (RoleDataDefinition::ControlTypeTextbox == datatype) {
        return NPS::compareString(left.data(Qt::DisplayRole).toString(), right.data(Qt::DisplayRole).toString());
    } else if (RoleDataDefinition::ControlTypeDouble == datatype) {
        return left.data(Qt::DisplayRole).toDouble() < right.data(Qt::DisplayRole).toDouble();
    } else if (RoleDataDefinition::ControlTypeInt == datatype) {
        return left.data(Qt::DisplayRole).toInt() < right.data(Qt::DisplayRole).toInt();
    } else if (RoleDataDefinition::ControlTypeCheckbox == datatype) {
        return left.data(Qt::CheckStateRole).toInt() < right.data(Qt::CheckStateRole).toInt();
    } else if (RoleDataDefinition::ControlTypeDate == datatype) {
        QDateTime timeleft = QDateTime::fromString(left.data(Qt::DisplayRole).toString(), NPS::DateFormate);
        QDateTime timeright = QDateTime::fromString(right.data(Qt::DisplayRole).toString(), NPS::DateFormate);
        return timeleft < timeright;
    }
    return NPS::compareString(left.data(Qt::DisplayRole).toString(), right.data(Qt::DisplayRole).toString());
}

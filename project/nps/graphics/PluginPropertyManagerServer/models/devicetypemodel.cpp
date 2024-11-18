#include "devicetypemodel.h"
#include "CommonModelAssistant.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include <QFontMetrics>
#include <qmath.h>

using namespace Kcc::BlockDefinition;

static const QString KEYWORDS_DEVICENAME = QObject::tr("Device Type Name");      // 设备类型名称
static const QString KEYWORDS_DEVICEBELONGTO = QObject::tr("Device Type Class"); // 设备类型所属
static const QString KEYWORDS_EDITTIME = QObject::tr("Edit Time");               // 编辑时间
static const QString TYPE_GLOBAL = QObject::tr("Global");                        // 全局
static const QString TYPE_LOCAL = QObject::tr("Local");                          // 本地

DeviceTypeModel::DeviceTypeModel(QObject *parent) : QAbstractTableModel(parent), m_systemDevicetype(nullptr) { }

DeviceTypeModel::~DeviceTypeModel() { }

int DeviceTypeModel::rowCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_data.count();
}

int DeviceTypeModel::columnCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_itemPropertyList.count();
}

QVariant DeviceTypeModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() >= m_data.size() || index.column() >= m_itemPropertyList.size() || m_data[index.row()] == nullptr) {
        return QVariant();
    }

    if ((Qt::DisplayRole == role || Qt::EditRole == role)
        && RoleDataDefinition::ControlTypeCheckbox != m_itemPropertyList[index.column()].DataType) {
        if (KEYWORDS_DEVICENAME == m_itemPropertyList[index.column()].Keywords) {
            return m_data[index.row()]->getName();
        } else if (KEYWORDS_EDITTIME == m_itemPropertyList[index.column()].Keywords) {
            return m_data[index.row()]->getModifyTime().toString("yyyy-MM-dd hh:mm:ss");
        } else if (KEYWORDS_DEVICEBELONGTO == m_itemPropertyList[index.column()].Keywords) {
            return (m_data[index.row()] == m_systemDevicetype && m_systemDevicetype != nullptr) ? TYPE_GLOBAL
                                                                                                : TYPE_LOCAL;
        }
        return NPS::RealValueMaptoControlValue(
                m_data[index.row()]->getDeviceTypeVariableValue(m_itemPropertyList[index.column()].Keywords),
                m_itemPropertyList[index.column()].DataType, m_itemPropertyList[index.column()].DataRange);
    } else if (Qt::CheckStateRole == role
               && m_itemPropertyList[index.column()].DataType == RoleDataDefinition::ControlTypeCheckbox) {
        return NPS::RealValueMaptoControlValue(
                m_data[index.row()]->getDeviceTypeVariableValue(m_itemPropertyList[index.column()].Keywords),
                RoleDataDefinition::ControlTypeCheckbox, m_itemPropertyList[index.column()].DataRange);
    } else if (NPS::ModelDataTypeRole == role) {
        return m_itemPropertyList[index.column()].DataType;
    } else if (NPS::ModelDataKeywordsRole == role) {
        return m_itemPropertyList[index.column()].Keywords;
    } else if (NPS::ModelDataRangeRole == role) {
        return m_itemPropertyList[index.column()].DataRange;
    }

    return QVariant();
}

QVariant DeviceTypeModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_itemPropertyList.count());
            return toLineBreakAtENWord(m_itemPropertyList[section].TitleName);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags DeviceTypeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeCheckbox) {
        return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }

    return QAbstractTableModel::flags(index);
}

void DeviceTypeModel::updateDeviceData(const QList<QSharedPointer<Kcc::BlockDefinition::DeviceModel>> &devicelist,
                                       QSharedPointer<Kcc::BlockDefinition::DeviceModel> sysdevice)
{
    beginResetModel();
    m_data.clear();
    m_itemPropertyList.clear();
    m_data = devicelist;
    m_data.append(sysdevice);
    m_systemDevicetype = sysdevice;
    m_itemPropertyList.append(
            DeviceListItemProperties(KEYWORDS_DEVICENAME, KEYWORDS_DEVICENAME, RoleDataDefinition::ControlTypeTextbox));
    m_itemPropertyList.append(DeviceListItemProperties(KEYWORDS_DEVICEBELONGTO, KEYWORDS_DEVICEBELONGTO,
                                                       RoleDataDefinition::ControlTypeTextbox));
    // 更新数据
    if (sysdevice != nullptr) {
        QList<PVariable> sortlist = CMA::getVarGroupList(sysdevice, RoleDataDefinition::DeviceTypeParameter);
        for (PVariable pvar : sortlist) {
            if (pvar == nullptr) {
                continue;
            }
            m_itemPropertyList.append(DeviceListItemProperties(pvar->getName(), pvar->getDisplayName(),
                                                               pvar->getControlType(), pvar->getControlValue()));
            continue;
        }
    }
    m_itemPropertyList.append(
            DeviceListItemProperties(KEYWORDS_EDITTIME, KEYWORDS_EDITTIME, RoleDataDefinition::ControlTypeDate));
    endResetModel();
}

QSharedPointer<Kcc::BlockDefinition::DeviceModel> DeviceTypeModel::getDeviceModel(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= m_data.size()) {
        return PDeviceModel(nullptr);
    }

    return m_data[index.row()];
}

// ElectricalBlock::DeviceType DeviceTypeModel::getDeviceTypeByName( const QString &strname )
//{
//	for (int i=0;i<m_data.size();++i) {
//		if (m_data[i].name == strname) {
//			return m_data[i];
//			break;
//		}
//	}
//	return ElectricalBlock::DeviceType();
// }

QString DeviceTypeModel::toLineBreakAtENWord(const QString &original) const
{
    int size = original.size();
    QString tmpstr = original;
    for (int i = 0; i < size; ++i) {
        if (alphabet.contains(tmpstr.left(1))) {
            if (i == 0) {
                return original;
            }
            return original.left(i) + "\n" + original.right(size - i);
        }
        tmpstr.remove(0, 1);
    }
    return original;
}

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
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return QString::number(section + 1);
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
    QString datatype = left.data(ModelDataTypeRole).toString();
    if (RoleDataDefinition::ControlTypeTextbox == datatype) {
        return left.data(Qt::DisplayRole).toString() < right.data(Qt::DisplayRole).toString();
    } else if (RoleDataDefinition::ControlTypeDouble == datatype) {
        return left.data(Qt::DisplayRole).toDouble() < right.data(Qt::DisplayRole).toDouble();
    } else if (RoleDataDefinition::ControlTypeInt == datatype) {
        return left.data(Qt::DisplayRole).toInt() < right.data(Qt::DisplayRole).toInt();
    } else if (RoleDataDefinition::ControlTypeCheckbox == datatype) {
        return left.data(Qt::CheckStateRole).toInt() < left.data(Qt::CheckStateRole).toInt();
    }
    return NPS::compareString(left.data(Qt::DisplayRole).toString(), right.data(Qt::DisplayRole).toString());
}

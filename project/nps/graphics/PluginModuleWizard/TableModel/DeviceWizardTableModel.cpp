#include "DeviceWizardTableModel.h"

DeviceWizardTableModel::DeviceWizardTableModel(QObject *parent) : WizardTableAbstractModel<DeviceTableItem>(parent)
{
    m_listHeader << QObject::tr("Name") << QObject::tr("Variable Types") << QObject::tr("Variable Scope")
                 << QObject::tr("Display Name") << QObject::tr("Variable Values") << QObject::tr("Unit of Variable")
                 << QObject::tr("Visible Type") << QObject::tr("ControlType") << QObject::tr("ControlValue");
}

void DeviceWizardTableModel::addRow(const QString &_strName, const QString &_type, const QString &_range,
                                    const QString &_nameCHS, const QString &_value, const int _order,
                                    const QString &_unit, const int _mode, const QString &controltype,
                                    const QVariant &controlValue)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(DeviceTableItem(_strName, _type, _range, _nameCHS, _value, _order, _unit, _mode, controltype,
                                  controlValue));
    endInsertRows();
}

QVariant DeviceWizardTableModel::getParameter(const QString &strName, DeviceWizardTableModel::Column type)
{
    for (const DeviceTableItem &item : m_data) {
        if (item.strName == strName) {
            switch (type) {
            case DeviceWizardTableModel::ParamName:
                return item.strName;
            case DeviceWizardTableModel::ParamType:
                return item.type;
            case DeviceWizardTableModel::ParamRange:
                return item.range;
            case DeviceWizardTableModel::ParamNameCHS:
                return item.nameCHS;
            case DeviceWizardTableModel::ParamValue:
                return item.nvalue;
            case DeviceWizardTableModel::ParamUnit:
                return item.unit;
            case DeviceWizardTableModel::ParamMode:
                return item.nModeType;
            case DeviceWizardTableModel::ControlTyep:
                return item.nControlTyep;
            case DeviceWizardTableModel::ControlValue:
                return item.nControlValue;
            default:
                return QVariant();
            }
        }
    }
    return QVariant();
}

int DeviceWizardTableModel::rowCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int DeviceWizardTableModel::columnCount(const QModelIndex &parent) const
{
    return m_listHeader.count();
}

QVariant DeviceWizardTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    // 取原始数据用EditRole，显示数据用DisplayRole
    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole) {
        int nIndex = m_data[row].nModeType;
        switch (col) {
        case ParamName:
            return m_data[row].strName;
        case ParamType:
            return m_data[row].type;
        case ParamRange:
            return m_data[row].range;
        case ParamNameCHS:
            return m_data[row].nameCHS;
        case ParamValue:
            return m_data[row].nvalue;
        case ParamUnit:
            return m_data[row].unit;
        case ParamMode:
            if (0 == nIndex) {
                return Global::variableState.at(0);
            } else if (1 == nIndex) {
                return Global::variableState.at(1);
            } else if (2 == nIndex) {
                return Global::variableState.at(2);
            }
            return QVariant();
        case ControlTyep:
            return m_data[row].nControlTyep;
        case ControlValue:
            return m_data[row].nControlValue;
        default:
            return QVariant();
        }
    }

    if (role == Qt::EditRole) {
        switch (col) {
        case ParamName:
            return m_data[row].strName;
        case ParamType:
            return m_data[row].type;
        case ParamRange:
            return m_data[row].range;
        case ParamNameCHS:
            return m_data[row].nameCHS;
        case ParamValue:
            return m_data[row].nvalue;
        case ParamUnit:
            return m_data[row].unit;
        case ParamMode:
            return m_data[row].nModeType;
        case ControlTyep:
            return m_data[row].nControlTyep;
        case ControlValue:
            return m_data[row].nControlValue;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags DeviceWizardTableModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractTableModel::flags(index);
    int col = index.column();
    if (col == ParamName) { // 不可修改命名
        flags |= Qt::ItemIsSelectable;
    } else {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool DeviceWizardTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return QAbstractTableModel::setData(index, value, role);
    }

    int row = index.row();
    int col = index.column();
    if (role == Qt::EditRole) {
        switch (col) {
        case ParamType:
            if (m_data[row].type == value.toString()) {
                return true;
            }
            // 类型变更时，同步修改值的显示内容
            m_data[row].type = value.toString();
            emit dataChanged(index, index.sibling(index.row(), ParamType));
            return true;
        case ParamRange:
            m_data[row].range = value.toString();
            break;
        case ParamNameCHS:
            m_data[row].nameCHS = value.toString();
            break;
        case ParamValue:
            m_data[row].nvalue = value.toString();
            break;
        case ParamUnit:
            m_data[row].unit = value.toString();
            break;
        case ParamMode:
            if (m_data[row].nModeType == value.toInt()) {
                return true;
            }
            m_data[row].nModeType = value.toInt();
            emit dataChanged(index, index.sibling(index.row(), ParamMode));
            return true;
        case ControlTyep:
            if (m_data[row].nControlTyep == value.toString()) {
                return true;
            }
            m_data[row].nControlTyep = value.toString();
            emit dataChanged(index, index.sibling(index.row(), ControlTyep));
            return true;
        case ControlValue:
            m_data[row].nControlValue = value.toString();
            break;
        default:
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    return QAbstractTableModel::setData(index, value, role);
}

QVariant DeviceWizardTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_listHeader.count());
            return m_listHeader[section];
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
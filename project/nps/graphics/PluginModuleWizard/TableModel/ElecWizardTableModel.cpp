#include "ElecWizardTableModel.h"
#include "GlobalDefinition.h"

using namespace RoleDataDefinition;

ElecWizardTableModel::ElecWizardTableModel(QObject *parent) : WizardTableAbstractModel<ElecTableItem>(parent)
{
    m_listHeader << QObject::tr("Name") << QObject::tr("Classification") << QObject::tr("Variable Types")
                 << QObject::tr("Display Name") << QObject::tr("Variable Values") << QObject::tr("Variable State")
                 << QObject::tr("Unit of Variable") << QObject::tr("ControlType") << QObject::tr("ControlValue")
                 << QObject::tr("Variable Scope") << QObject::tr("Port Type") << QObject::tr("Port Location")
                 << QObject::tr("Port Dangling Text");
}

ElecWizardTableModel::~ElecWizardTableModel() { }

QVariant ElecWizardTableModel::getParameter(const QString &strName, ElecWizardTableModel::Category classify,
                                            ElecWizardTableModel::Column type)
{
    foreach (const ElecTableItem &item, m_data) {
        if (item.strName == strName && item.classify == classify) {
            switch (type) {
            case ElecWizardTableModel::Column::ParamName:
                return item.strName;
            case ElecWizardTableModel::Column::ParamType:
                return item.type.toString();
            case ElecWizardTableModel::Column::ParamRange:
                return item.range;
            case ElecWizardTableModel::Column::ParamNameCHS:
                return item.nameCHS;
            case ElecWizardTableModel::Column::ParamValue:
                return item.nvalue;
            case ElecWizardTableModel::Column::ParamMode:
                return item.mode;
            case ElecWizardTableModel::Column::ParamUnit:
                return item.unit;
            case ElecWizardTableModel::Column::ParamPortType:
                return item.portType;
            case ElecWizardTableModel::Column::ParemPortPosition:
                return item.portPosition;
            case ElecWizardTableModel::Column::ControlTyep:
                return item.nControlTyep;
            case ElecWizardTableModel::Column::ControlValue:
                return item.nControlValue;
            case ElecWizardTableModel::Column::ParemPortText:
                return item.portText;
            default:
                return QVariant();
            }
        }
    }
    return QVariant();
}

void ElecWizardTableModel::addRow(const QString &_strName, Category _classify, const QVariant &_type,
                                  const QString &_range, const QString &_nameCHS, const QString &_value,
                                  const int &_mode, const int _order, const QString &_unit,
                                  const QString &_nControlTyep, const QVariant &_nControlValue, const int portType,
                                  const QString &portposition, const QString &_portText)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(ElecTableItem(_strName, _classify, _type, _range, _nameCHS, _value, _mode, _order, _unit,
                                _nControlTyep, _nControlValue, portType, portposition, _portText));
    endInsertRows();
}

int ElecWizardTableModel::rowCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int ElecWizardTableModel::columnCount(const QModelIndex &parent) const
{
    return m_listHeader.count();
}

QVariant ElecWizardTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    // 取原始数据用EditRole，显示数据用DisplayRole
    int row = index.row();
    int col = index.column();
    QString str;
    if (role == Qt::DisplayRole) {
        int nIndex = m_data[row].mode;
        int mINdex = m_data[row].portType;
        switch (col) {
        case ParamName:
            return m_data[row].strName;
        case ParamClassify: {
            switch (m_data[row].classify) {
            case PortParameter:
                return QObject::tr("Port");
            case ElectricalParameter:
                return QObject::tr("Electrical Parameters");
            case loadFlowParameter:
                return QObject::tr("Power Flow Parameter");
            case loadFlowResultParameter:
                return QObject::tr("Power Flow Result");
            case simulationParameter:
                return QObject::tr("Simulation Setup");
            case initSimulationParameter:
                return QObject::tr("Initial Value Setting");
            default:
                return QVariant();
            }
        }
        case ParamType:
            return m_data[row].type.toString();
        case ParamRange:
            return m_data[row].range;
        case ParamNameCHS:
            return m_data[row].nameCHS;
        case ParamValue:
            return m_data[row].nvalue;
        case ParamMode:
            if (0 == nIndex) {
                return Global::variableState.at(0);
            } else if (1 == nIndex) {
                return Global::variableState.at(1);
            } else if (2 == nIndex) {
                return Global::variableState.at(2);
            }
        case ParamUnit:
            return m_data[row].unit;
        case ControlTyep: {
            return m_data[row].nControlTyep;
        }
        case ControlValue: {
            return m_data[row].nControlValue;
        }
        case ParamPortType:
            str = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
            if (str != QObject::tr("Port")) {
                return tr("");
            }
            if (0 == mINdex) {
                return Global::portTypeList.at(0);
            } else if (1 == mINdex) {
                return Global::portTypeList.at(1);
            } else if (2 == mINdex) {
                return Global::portTypeList.at(2);
            } else if (3 == mINdex) {
                return Global::portTypeList.at(3);
            }
        case ParemPortPosition:
            if (m_data[row].portPosition.isEmpty()
                && index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString() == tr("Port")) {
                return QObject::tr("(0,0)");
            }
            return m_data[row].portPosition;
        case ParemPortText:
            return m_data[row].portText;
        default:
            return QVariant();
        }
    }

    if (role == ElecCategoryRole) {
        // 整行的Category属性
        return m_data[row].classify;
    }

    if (role == Qt::EditRole) {
        switch (col) {
        case ParamName:
            return m_data[row].strName;
        case ParamClassify:
            return m_data[row].classify;
        case ParamType:
            return m_data[row].type.toString();
        case ParamRange:
            return m_data[row].range;
        case ParamNameCHS:
            return m_data[row].nameCHS;
        case ParamValue:
            return m_data[row].nvalue;
        case ParamMode:
            return m_data[row].mode;
        case ParamUnit:
            return m_data[row].unit;
        case ControlTyep:
            return m_data[row].nControlTyep;
        case ControlValue:
            return m_data[row].nControlValue;
        case ParamPortType:
            return m_data[row].portType;
        case ParemPortPosition:
            return m_data[row].portPosition;
        case ParemPortText:
            return m_data[row].portText;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags ElecWizardTableModel::flags(const QModelIndex &index) const
{
    auto str = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
    auto flags = QAbstractTableModel::flags(index);
    int col = index.column();
    int row = index.row();
    if (col == ParamClassify || col == ParamName) { // 不可修改命名与分类
        flags |= Qt::ItemIsSelectable;
    } else {
        flags |= Qt::ItemIsEditable;
    }

    if ((str != QObject::tr("Port") && col == ParamPortType) || (str != QObject::tr("Port") && col == ParemPortPosition)
        || (str != QObject::tr("Port") && col == ParemPortText)) {
        flags &= ~Qt::ItemIsEditable;
    }
    return flags;
}

bool ElecWizardTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
            if (m_data[row].range == value.toString()) {
                return true;
            }
            m_data[row].range = value.toString();
            emit dataChanged(index, index.sibling(index.row(), ParamRange));
            return true;
        case ParamNameCHS:
            m_data[row].nameCHS = value.toString();
            break;
        case ParamValue:
            m_data[row].nvalue = value.toString();
            break;
        case ParamMode:
            if (m_data[row].mode == value.toInt()) {
                return true;
            }
            m_data[row].mode = value.toInt();
            emit dataChanged(index, index.sibling(index.row(), ParamMode));
            return true;
        case ParamUnit:
            m_data[row].unit = value.toString();
            break;
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
        case ParamPortType:
            if (m_data[row].portType == value.toInt()) {
                return true;
            }
            m_data[row].portType = value.toInt();
            emit dataChanged(index, index.sibling(index.row(), ParamPortType));
            return true;
        case ParemPortPosition:
            m_data[row].portPosition = value.toString();
            break;
        case ParemPortText:
            m_data[row].portText = value.toString();
            break;
        default:
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    return QAbstractTableModel::setData(index, value, role);
}

QVariant ElecWizardTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_listHeader.count());
            return m_listHeader[section];
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool ElecWizardTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.column() != ElecWizardTableModel::ParamClassify) {
        return QSortFilterProxyModel::lessThan(left, right);
    }

    return left.data(ElecWizardTableModel::ElecCategoryRole).toInt()
            < right.data(ElecWizardTableModel::ElecCategoryRole).toInt();
}
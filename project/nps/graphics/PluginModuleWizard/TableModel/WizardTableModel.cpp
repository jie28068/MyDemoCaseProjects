#include "WizardTableModel.h"
#include "GlobalDefinition.h"

using namespace RoleDataDefinition;

WizardTableModel::WizardTableModel(QObject *parent /*= nullptr*/) : WizardTableAbstractModel<TableItem>(parent)
{
    m_listHeader << QObject::tr("Name") << QObject::tr("Classification") << QObject::tr("Variable Types")
                 << QObject::tr("Variable Values") << QObject::tr("Display Name") << QObject::tr("Port Visible")
                 << (QObject::tr("Clipping Type")) << QObject::tr("Visible Type") << QObject::tr("ControlType")
                 << QObject::tr("ControlValue") << QObject::tr("Variable Description");
}

WizardTableModel::~WizardTableModel() { }

void WizardTableModel::addRow(const QString &strName, Category category, const QString &strType, const QVariant &value,
                              const QString &strNameCHS, const bool isShowportName, const int &nlimitTytpe,
                              const int &nnmodetype, const QString &_nControlTyep, const QVariant &_nControlValue,
                              const QString &strDescription, const int &Index)

{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(TableItem(strName, category, strType, value, strNameCHS, isShowportName, nlimitTytpe, nnmodetype,
                            _nControlTyep, _nControlValue, strDescription, Index));
    endInsertRows();
}

void WizardTableModel::setPrototypeName(const QString &prototypename)
{
    m_prototypeName = prototypename;
}

PVariable WizardTableModel::initVarible(PVariable variable, const QString &strName, int order)
{
    for (const TableItem &item : m_data) {
        if (item.strName == strName) {
            variable->setName(strName);
            variable->setDisplayName(item.strNameCHS);
            variable->setDataType(item.strParamType);
            variable->setDescription(item.strDescription);
            variable->setData(RoleDataDefinition::ValueRole, item.value);
            variable->setData(RoleDataDefinition::ShowModeRole, item.nModeType);
            variable->setData(Global::isShowPortName, item.isShowPortName);
            variable->setOrder(order);
            if (0 == item.nLimitTytpe) {
                variable->setLimitationType(Variable::LimitType::NoLimitation);
            } else if (1 == item.nLimitTytpe) {
                variable->setLimitationType(Variable::LimitType::UpperLimitation);
            } else if (2 == item.nLimitTytpe) {
                variable->setLimitationType(Variable::LimitType::LowerLimitation);
            }
            if (Global::ControlTyeName.contains(item.nControlTyep)) {
                variable->setControlType(Global::ControlTyeName[item.nControlTyep].toString());
            } else {
                variable->setControlType(ControlTypeTextbox);
            }

            variable->setControlVlaue(item.nControlValue);
            break;
        }
    }
    return variable;
}

int WizardTableModel::rowCount(const QModelIndex &parent /*= QModelIndex() */) const
{
    return m_data.count();
}

int WizardTableModel::columnCount(const QModelIndex &parent /*= QModelIndex() */) const
{
    return m_listHeader.count();
}

QVariant WizardTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
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
        case Name:
            return m_data[row].strName;
        case ParamCategory: {
            switch (m_data[row].nCategory) {
            case Input:
                return QObject::tr("Input Variables");
            case Output:
                return QObject::tr("Output Variables");
            case DisState:
                // #ifdef COMPILER_PRODUCT_DESIGNER
                return QObject::tr("Discrete State variable");
                // #endif
                // #ifdef COMPILER_PRODUCT_SIMUNPS
                //                 return QObject::tr("State Variable");
                // #endif
            case ConState:
                return QObject::tr("Continue State Variable");
            case Internal:
                return QObject::tr("Internal Variables");
            case Param:
                return QObject::tr("Module Parameters");
            default:
                return QVariant();
            }
        }
        case ParamType:
            return m_data[row].strParamType;
        case ParamValue:
            return m_data[row].value.toString();
        case NameCHS:
            return m_data[row].strNameCHS;
        case LimitType: {
            int nIndex = m_data[row].nLimitTytpe;
            if (0 == nIndex) {
                return Global::LimitType_NO;
            } else if (1 == nIndex) {
                return Global::LimitType_UPPER;
            } else if (2 == nIndex) {
                return Global::LimitType_LOWER;
            }
        }
        case ControlTyep: {
            return m_data[row].nControlTyep;
        }
        case ControlValue: {
            return m_data[row].nControlValue;
        }
        case Description:
            return m_data[row].strDescription;
        case ParamMode:
            if (0 == nIndex) {
                return Global::variableState.at(0);
            } else if (1 == nIndex) {
                return Global::variableState.at(1);
            } else if (2 == nIndex) {
                return Global::variableState.at(2);
            }
        default:
            return QVariant();
        }
    }
    if (role == Qt::EditRole) {
        switch (col) {
        case Name:
            return m_data[row].strName;
        case ParamCategory:
            return m_data[row].nCategory;
        case ParamType:
            return m_data[row].strParamType;
        case ParamValue:
            return m_data[row].value;
        case NameCHS:
            return m_data[row].strNameCHS;
        case LimitType:
            return m_data[row].nLimitTytpe;
        case ControlTyep:
            return m_data[row].nControlTyep;
        case ControlValue:
            return m_data[row].nControlValue;
        case Description:
            return m_data[row].strDescription;
        case ParamMode:
            return m_data[row].nModeType;
        default:
            return QVariant();
        }
    }
    if (role == CategoryRole) {
        // 整行的Category属性
        return m_data[row].nCategory;
    }
    if (role == Qt::CheckStateRole && col == IsshowPortName
        && (m_data[row].nCategory == Input || m_data[row].nCategory == Output)) {
        if (m_data[row].isShowPortName) {
            return Qt::Checked;
        } else {
            return Qt::Unchecked;
        }
    }
    if (Global::PrototypeNameRole == role) {
        return m_prototypeName;
    }
    return QVariant();
}

Qt::ItemFlags WizardTableModel::flags(const QModelIndex &index) const
{
    auto str = index.sibling(index.row(), WizardTableModel::ParamType).data().toString();
    auto flags = QAbstractTableModel::flags(index);
    int col = index.column();
    int row = index.row();
    if (col == ParamType || col == NameCHS || col == Description || col == ParamValue || col == LimitType
        || col == ControlTyep || col == ControlValue || col == ParamMode) {
        flags |= Qt::ItemIsEditable;
    } else if (col == IsshowPortName) {
        flags |= Qt::ItemIsEnabled;
        flags |= Qt::ItemIsSelectable;
        flags |= Qt::ItemIsUserCheckable;
    }
    // 矩阵类型控件值与控件类型不可编辑
    if (((str.startsWith("Matrix") || str.startsWith("RowVector") || str.startsWith("Vector")) && col == ControlTyep)
        || ((str.startsWith("Matrix") || str.startsWith("RowVector") || str.startsWith("Vector"))
            && col == ControlValue)) {
        flags &= ~Qt::ItemIsEditable;
    }

    return flags;
}

bool WizardTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid()) {
        return QAbstractTableModel::setData(index, value, role);
    }

    int row = index.row();
    int col = index.column();
    if (role == Qt::EditRole) {
        switch (col) {
        case ParamType:
            if (m_data[row].strParamType == value.toString()) {
                // 未变化
                return true;
            }
            // 类型变更时，同步修改值的显示内容
            m_data[row].strParamType = value.toString();
            if (m_data[row].strParamType == Global::DataType_Bool) {
                m_data[row].value = m_data[row].value.toBool();
            } else if (Global::DataType_DoubleVector == m_data[row].strParamType
                       || Global::DataType_DoubleComplex == m_data[row].strParamType) {
                m_data[row].value = "0,0";
            } else if (Global::DataType_Double == m_data[row].strParamType
                       || Global::DataType_KccString == m_data[row].strParamType) {
                m_data[row].value = "0";
            } else {
                m_data[row].value = m_data[row].value.toString();
            }
            // emit dataChanged(index, index.sibling(index.row(), ParamValue));
            return true;
        case LimitType:
            if (m_data[row].nLimitTytpe == value.toInt()) {
                return true;
            }
            m_data[row].nLimitTytpe = value.toInt();
            emit dataChanged(index, index.sibling(index.row(), LimitType));
            return true;

        case ParamValue:
            m_data[row].value = value;
            break;
        case NameCHS:
            m_data[row].strNameCHS = value.toString();
            break;
        case Description:
            m_data[row].strDescription = value.toString();
            break;
        case ControlTyep:
            if (m_data[row].nControlTyep == value.toString()) {
                return true;
            }
            m_data[row].nControlTyep = value.toString();
            emit dataChanged(index, index.sibling(index.row(), ControlTyep));
            return true;
        case ControlValue:
            m_data[row].nControlValue = value;
            break;
        case ParamMode:
            if (m_data[row].nModeType == value.toInt()) {
                return true;
            }
            m_data[row].nModeType = value.toInt();
            emit dataChanged(index, index.sibling(index.row(), ParamMode));
            return true;
        default:
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    if (Qt::CheckStateRole == role && col == IsshowPortName) {
        m_data[row].isShowPortName = value.toBool();
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractTableModel::setData(index, value, role);
}

QVariant WizardTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_listHeader.count());
            return m_listHeader[section];
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool WizardTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.column() != WizardTableModel::ParamCategory) {
        return QSortFilterProxyModel::lessThan(left, right);
    }
    return left.data(WizardTableModel::CategoryRole).toInt() < right.data(WizardTableModel::CategoryRole).toInt();
}
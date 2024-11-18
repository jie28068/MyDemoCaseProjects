#include "ColorMatchModel.h"

#include <QColor>
#include <QComboBox>
#include <QLineEdit>
#include <QSize>

#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include "commenteditwdiget.h"
// 


USE_LOGOUT_("ColorMatchModel")

// tablemodel
ColorMatchModel::ColorMatchModel(QObject *parent) : m_bReadOnly(false), m_useColor(true) { }

ColorMatchModel::~ColorMatchModel() { }

int ColorMatchModel::rowCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_data.size();
}

int ColorMatchModel::columnCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_listHeader.size();
}

QVariant ColorMatchModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int col = index.column();
    int row = index.row();
    int totalcol = columnCount();
    if (col >= totalcol || row >= rowCount()) {
        return QVariant();
    }
    if (m_data.isEmpty() || !m_data[row].isValid())
    {
        return QVariant();
    } 
    switch (role)
    {
        case Qt::DisplayRole:
        if(RoleDataDefinition::ControlTypeColor != m_data[row].m_typelist[col]){
            return m_data[row].m_datalist[col]; 
        }
        break;
        case Qt::DecorationRole:
        if(RoleDataDefinition::ControlTypeColor == m_data[row].m_typelist[col]){
            return m_data[row].m_datalist[col]; 
        }            
        break;
        /* userrole */
        case NPS::CustomMenuTypeRole:
        break;
        case NPS::ModelDataTypeRole:           
            return m_data[row].m_typelist[col];            
        case NPS::ModelDataRealValueRole:
            return m_data[row].m_datalist[col]; 
        case NPS::ModelDataEditRole:
            if (m_bReadOnly) {
                return false;
            }
            return m_data[row].m_caneditlist[col];
        case NPS::ModelDataRangeRole:
        break;
        case NPS::ModelDataKeywordsRole:
        break;
        case NPS::ModelDataLineStyle:
        break;
        case NPS::ModelDataLineWidth:
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant ColorMatchModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_listHeader.count());
            return (m_listHeader[section]);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags ColorMatchModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    if (RoleDataDefinition::ControlTypeCheckbox == index.data(NPS::ModelDataTypeRole).toString()) {
        if (index.data(NPS::ModelDataEditRole).toBool()) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        } else {
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    } else if (index.data(NPS::ModelDataEditRole).toBool()
               && RoleDataDefinition::ControlTypeColor != index.data(NPS::ModelDataTypeRole).toString()
               && RoleDataDefinition::ControlTypeFont != index.data(NPS::ModelDataTypeRole).toString()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

bool ColorMatchModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    if (!index.isValid() || (role != Qt::EditRole && role != Qt::CheckStateRole)) {
        return false;
    }
    updateValue(index.row(),index.column(),value);
    return QAbstractTableModel::setData(index, value, role);
}

void ColorMatchModel::setModelData(const QList<ColorMatchModelItem> &listdata, const QStringList &listheader)
{
    beginResetModel();
    m_listHeader.clear();
    m_data.clear();
    m_listHeader = listheader;
    m_data = listdata;
    endResetModel();
}


QVariantList ColorMatchModel::getNewListData()
{
    QVariantList datalist;
    foreach(ColorMatchModelItem item,m_data){
        datalist<<item.m_datalist.at(1)<<item.m_datalist.at(2);
    }
    return datalist;
}


void ColorMatchModel::setModelDataReadOnly(bool bReadOnly)
{
    beginResetModel();
    m_bReadOnly = bReadOnly;
    endResetModel();
}

void ColorMatchModel::setModelDataColorUse(bool use)
{
    beginResetModel();
    m_useColor = use;
    endResetModel();
}

void ColorMatchModel::updateValue(int row,int col,QVariant val, bool bsendSignal)
{
    ColorMatchModelItem olditem = m_data[row];
    if((olditem.m_typelist[col] == RoleDataDefinition::ControlTypeDouble) && (row > 0)){
        //
        if(m_data[row].m_datalist[col].toDouble() < m_data[row-1].m_datalist[col].toDouble() ){
            if(val.toDouble() >= m_data[row-1].m_datalist[col].toDouble()){
                LOGOUT(tr("The lower limit cannot be higher than the upper level"),LOG_ERROR);
                return;
            }
        }else{
            if(val.toDouble() <= m_data[row-1].m_datalist[col].toDouble()){
                LOGOUT(tr("The upper limit cannot be lower than the previous level"),LOG_ERROR);
                return;
            }
        }
    }
    m_data[row].m_datalist[col] = val;
    ColorMatchModelItem item = m_data[row];
    emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
    if (bsendSignal) {
        emit modelDataItemChanged(olditem, item);
    }
}

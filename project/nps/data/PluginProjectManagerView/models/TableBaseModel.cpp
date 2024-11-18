#include "TableBaseModel.h"

#include "CoreLib/ServerManager.h"
#include "ProjectViewServers.h"

USE_LOGOUT_("TableBaseModel")

TableBaseModel::TableBaseModel(QObject *parent)
    : QAbstractTableModel(parent), m_curProjActive(false), m_DrawboardRunningSts(false)
{
}

TableBaseModel::~TableBaseModel() { }

int TableBaseModel::rowCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    if (TableBaseModel::ModelDataType_Invalid == m_ModelDataType) {
        return 0;
    }

    if (TableBaseModel::ModelDataType_Tree == m_ModelDataType) {
        return m_data.size();
    } else if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
               || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
        return m_instanceData.size();
    }

    return m_modelData.size();
}

int TableBaseModel::columnCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    if (TableBaseModel::ModelDataType_Invalid == m_ModelDataType) {
        return 0;
    }
    return m_ModelItemList.count();
}

QVariant TableBaseModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!isModelDataValid(index)) {
        return QVariant();
    }
    if ((Qt::DisplayRole == role || Qt::ToolTipRole == role)
        && index.data(NPS::ModelDataTypeRole).toString() != RoleDataDefinition::ControlTypeCheckbox) {
        if (Qt::ToolTipRole == role
            && (NPS::KEYWORDS_SIMUPARAM_DTM == index.data(NPS::ModelDataKeywordsRole).toString()
                || NPS::KEYWORDS_SIMUPARAM_WF == index.data(NPS::ModelDataKeywordsRole).toString())) {
            return NPS::WF_DTM_TIPS;
        }
        return m_ModelItemList[index.column()].otherPropertyMap.contains(ITEMKEY::DISPLAY_ADD_SYMBOL)
                ? m_ModelItemList[index.column()].otherPropertyMap[ITEMKEY::DISPLAY_ADD_SYMBOL].toString()
                        + NPS::RealValueMaptoControlValue(TModelData(index, TableBaseModel::ModelData_Value),
                                                          index.data(NPS::ModelDataTypeRole).toString(),
                                                          index.data(NPS::ModelDataRangeRole))
                                  .toString()
                : NPS::RealValueMaptoControlValue(TModelData(index, TableBaseModel::ModelData_Value),
                                                  index.data(NPS::ModelDataTypeRole).toString(),
                                                  index.data(NPS::ModelDataRangeRole));
    } else if (Qt::EditRole == role
               && index.data(NPS::ModelDataTypeRole).toString() != RoleDataDefinition::ControlTypeCheckbox) {
        return NPS::RealValueMaptoControlValue(TModelData(index, TableBaseModel::ModelData_Value),
                                               index.data(NPS::ModelDataTypeRole).toString(),
                                               index.data(NPS::ModelDataRangeRole));
    } else if (Qt::CheckStateRole == role
               && index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeCheckbox) {
        return NPS::RealValueMaptoControlValue(TModelData(index, TableBaseModel::ModelData_Value),
                                               RoleDataDefinition::ControlTypeCheckbox, "");
    } else if (Qt::DecorationRole == role
               && index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeColor) {
        return QColor(TModelData(index, TableBaseModel::ModelData_Value).toString());
    } else if (Qt::FontRole == role
               && index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeFont) {
        return NPS::RealValueMaptoControlValue(TModelData(index, TableBaseModel::ModelData_Value),
                                               RoleDataDefinition::ControlTypeFileSelect, "");
    } else if (NPS::ModelDataRealValueRole == role) {
        return TModelData(index, TableBaseModel::ModelData_Value);
    } else if (NPS::ModelDataKeywordsRole == role) {
        if (TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType
            || TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType) {
            return m_instanceData[index.row()].getName();
        }
        return m_ModelItemList[index.column()].Keywords;
    } else if (NPS::ModelDataTypeRole == role) {
        if (TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType
            || TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType) {
            if (AssistantDefine::KEYWORD_TITLE_VALUE == m_ModelItemList[index.column()].Keywords) {
                return m_instanceData[index.row()].pVar->getControlType();
            }
            return RoleDataDefinition::ControlTypeTextbox;
        }
        return m_ModelItemList[index.column()].DataType;
    } else if (NPS::ModelDataRangeRole == role) {
        if (TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType
            || TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType) {
            if (AssistantDefine::KEYWORD_TITLE_VALUE == m_ModelItemList[index.column()].Keywords) {
                return m_instanceData[index.row()].pVar->getControlValue();
            }
            return QVariant();
        }
        return m_ModelItemList[index.column()].DataRange;
    } else if (NPS::ModelDataEditRole == role) {
        if (!m_curProjActive || m_DrawboardRunningSts) {
            return false;
        }
        return TModelData(index, TableBaseModel::ModelData_Edit).toBool();
    } else if (TableBaseModel::CustomRole_VariableClass == role) {
        if (TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType
            || TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType) {
            return GroupNameMapToVariableClass(m_instanceData[index.row()].groupName);
        }
        return m_ModelItemList[index.column()].varClass;
    } else if (TableBaseModel::CustomRole_ModelDataType == role) {
        return m_ModelDataType;
    } else if (TableBaseModel::CustomRole_PrototypeName == role) {
        return m_ModelItemList[index.column()].prototypename;
    } else if (TableBaseModel::CustomRole_NotUseColor == role) {
        if (!m_curProjActive) {
            return false;
        }
        return m_ModelItemList[index.column()].otherPropertyMap.value(ITEMKEY::NOT_USECOLOR, false).toBool();
    } else if (Qt::TextColorRole == role) {
        if (!index.data(NPS::ModelDataEditRole).toBool()
            && !index.data(TableBaseModel::CustomRole_NotUseColor).toBool()) {
            return QColor(NPS::Color_NotEditable);
        }
    } else if (TableBaseModel::CustomRole_CurProjectActiveSts == role) {
        return m_curProjActive;
    } else if (TableBaseModel::CustomRole_DrawboardRunningSts == role) {
        return m_DrawboardRunningSts;
    } else if (TableBaseModel::CustomRole_DevicePrototype == role) {
        return m_devicePrototype;
    } else if (AssistantDefine::ModelDataObjectRole == role) {
        return m_widgetObject;
    } else if (TableBaseModel::CustomRole_CanOpen == role) {
        if (TableBaseModel::ModelDataType_Tree == m_ModelDataType
            || TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
            || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
            return false;
        } else if ((AssistantDefine::KEYWORD_TITLE_MODELNAME == index.data(NPS::ModelDataKeywordsRole).toString()
                    || AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == index.data(NPS::ModelDataKeywordsRole).toString()
                    || AssistantDefine::KEYWORD_TITLE_SLOT == index.data(NPS::ModelDataKeywordsRole).toString())
                   && m_curProjActive && m_modelData[index.row()]->getParentModel() != nullptr) {
            return true;
        }
        return false;
    }

    return QVariant();
}

QVariant TableBaseModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (section >= m_ModelItemList.count()) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            if (TableBaseModel::HeaderBreak_Unit == m_headerBreak) {
                return NPS::toLineBreakAtENWord(m_ModelItemList[section].TitleName);
            }
            return m_ModelItemList[section].TitleName;
        } else if (Qt::UserRole == role) {
            return m_ModelItemList[section].TitleName;
        }
    }
    return QVariant();
}

Qt::ItemFlags TableBaseModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeCheckbox) {
        if (index.data(NPS::ModelDataEditRole).toBool()) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        } else {
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    } else if (index.data(NPS::ModelDataEditRole).toBool()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

bool TableBaseModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    return false;
}

// userdefine function
void TableBaseModel::setReadOnlyData(const QList<QStringList> &listdata, const QStringList &titlelist)
{
    beginResetModel();
    clear();
    m_ModelDataType = TableBaseModel::ModelDataType_Tree;

    for (QString titlestr : titlelist) {
        m_ModelItemList.append(TableBaseModelItem(titlestr, titlestr));
    }
    m_data = listdata;
    endResetModel();
}

TableBaseModel::ModelDataType TableBaseModel::getModelDataType()
{
    return m_ModelDataType;
}

QSharedPointer<Kcc::BlockDefinition::Model> TableBaseModel::getModelByIndex(const QModelIndex &index)
{
    if (!isModelDataValid(index)) {
        return PModel(nullptr);
    }
    if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
        || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
        if (m_instanceData[index.row()].isValid() && m_instanceModel != nullptr) {
            return m_instanceData[index.row()].model;
        }
        return PModel(nullptr);
    } else if (TableBaseModel::ModelDataType_ComplexBoardInstances == m_ModelDataType) {
        PModel model = m_modelData[index.row()];
        if (model == nullptr) {
            return PModel(nullptr);
        }
        if (AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == index.data(NPS::ModelDataKeywordsRole).toString()) {
            PSlotBlock slotModel = model.dynamicCast<SlotBlock>();
            if (slotModel == nullptr) {
                return PModel(nullptr);
            }
            return slotModel->getTargetModel();
        }
        return model;
    }

    return m_modelData[index.row()];
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>> TableBaseModel::getModelsByIndexs(const QModelIndexList &indexList)
{
    QList<PModel> modelList;
    for (QModelIndex index : indexList) {
        PModel model = getModelByIndex(index);
        if (model != nullptr && !modelList.contains(model)) {
            modelList.append(model);
        }
    }
    return modelList;
}

void TableBaseModel::emitHeaderDataChanged()
{
    if (columnCount() > 0) {
        emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
    }
}

void TableBaseModel::SetProjectRuningSts(bool runing)
{
    beginResetModel();
    m_DrawboardRunningSts = runing;
    endResetModel();
}

bool TableBaseModel::isModelDataValid(const QModelIndex &index) const
{
    if (TableBaseModel::ModelDataType_Invalid == m_ModelDataType) {
        return false;
    }
    if (!index.isValid() || index.column() >= columnCount() || index.row() >= rowCount()) {
        return false;
    }
    if (TableBaseModel::ModelDataType_Tree == m_ModelDataType) {
        if (index.column() >= m_data[index.row()].size()) {
            return false;
        }
        return true;
    }
    if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
        || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
        return m_instanceData[index.row()].isValid() && m_instanceModel != nullptr;
    }

    if (m_modelData[index.row()] == nullptr) {
        return false;
    }
    return true;
}

QVariant TableBaseModel::TModelData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (TableBaseModel::ModelDataType_Tree != m_ModelDataType) {
        return QVariant();
    }

    if (!isModelDataValid(index)) {
        return QVariant();
    }

    if (TableBaseModel::ModelData_Edit == modeldata) {
        return false;
    } else if (TableBaseModel::ModelData_Value == modeldata) {
        return m_data[index.row()][index.column()];
    }
    return QVariant();
}

void TableBaseModel::clear()
{
    m_data.clear();
    m_ModelItemList.clear();
    m_modelData.clear();
    m_ModelDataType = TableBaseModel::ModelDataType_Invalid;
    m_conElecBusBarMap.clear();
    m_drawBoardList.clear();
    m_instanceModel = PModel(nullptr);
    m_instanceData.clear();
    m_headerBreak = TableBaseModel::HeaderBreak_None;
}

QString TableBaseModel::getTableName()
{
    switch (m_ModelDataType) {
    case TableBaseModel::ModelDataType_BuildInBlocks:
    case TableBaseModel::ModelDataType_CodeBlocks:
    case TableBaseModel::ModelDataType_CodeInstance:
        return AssistantDefine::TABLE_NAME_CODEBLOCKS;
    case TableBaseModel::ModelDataType_CombineBoardBlocks:
    case TableBaseModel::ModelDataType_CombineBoardInstance:
        return AssistantDefine::TABLE_NAME_COMBINEBOARDBLOCKS;
    case TableBaseModel::ModelDataType_ComplexBoardBlocks:
    case TableBaseModel::ModelDataType_ComplexBoardInstances:
        return AssistantDefine::TABLE_NAME_COMPLEXBOARDBLOCK;
    case TableBaseModel::ModelDataType_ControlBoardBlocks:
        return AssistantDefine::TABLE_NAME_CONTROLBOARDBLOCKS;
    case TableBaseModel::ModelDataType_ElectricalBlocks:
        return AssistantDefine::TABLE_NAME_ELECTRICALBLOCKS;
    case TableBaseModel::ModelDataType_GlobalDeviceTypes:
    case TableBaseModel::ModelDataType_ProjectDeviceTypes:
        return AssistantDefine::TABLE_NAME_DEVICETYPES;
    default:
        break;
    }
    return QString();
}

QList<QSharedPointer<Kcc::BlockDefinition::Variable>>
TableBaseModel::getVarGroupList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QStringList &grouplist,
                                bool recursive, const QStringList &excludekeys) const
{
    if (pmodel == nullptr || grouplist.size() <= 0) {
        return QList<PVariable>();
    }
    QList<PVariable> sortlist;
    for (QString groupname : grouplist) {
        sortlist.append(getVarGroupList(pmodel, groupname, recursive, excludekeys));
    }

    return sortlist;
}

QList<QSharedPointer<Kcc::BlockDefinition::Variable>>
TableBaseModel::getVarGroupList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                                bool recursive, const QStringList &excludekeys) const
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return QList<PVariable>();
    }
    QList<PVariable> allVariableLlist;
    if (recursive == true) {
        for (PModel cmodel : pmodel->getChildModels().values()) {
            if (cmodel == nullptr) {
                continue;
            }
            allVariableLlist.append(getVarGroupList(cmodel, groupName, recursive, excludekeys));
        }
    }
    PVariableGroup variablegroup = pmodel->getVariableGroup(groupName);
    if (variablegroup == nullptr) {
        return QList<PVariable>();
    }
    QList<PVariable> sortlist = variablegroup->getVariableMap().values();
    QMutableListIterator<PVariable> iter(sortlist);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() == nullptr) {
            iter.remove();
            continue;
        }
        if (excludekeys.contains(iter.value()->getName())
            || (excludekeys.contains(ITEMKEY::EXCLUDE_PORT) && isPort(pmodel, iter.value()->getName()))
            || (excludekeys.contains(ITEMKEY::EXCLUDE_NOTNUMBER)
                && !AssistantDefine::NUMBERLIST.contains(iter.value()->getDataType()))) {
            iter.remove();
            continue;
        }

        if ((ElectricalBlock::Type == pmodel->getModelType() || DeviceModel::Type == pmodel->getModelType())
            && RoleDataDefinition::PortGroup != groupName) {
            if (Variable::Invisiable == iter.value()->getShowMode()) {
                iter.remove();
            }
        }
    }
    qSort(sortlist.begin(), sortlist.end(), [&sortlist](PVariable lhs, PVariable rhs) -> bool {
        if (lhs->getOrder() == 0 && rhs->getOrder() == 0) {
            return lhs->getName() < rhs->getName();
        }
        return lhs->getOrder() < rhs->getOrder();
    });
    allVariableLlist.append(sortlist);
    return allVariableLlist;
}

QStringList TableBaseModel::getVarGroupStringList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                  const QString &groupName, bool recursive,
                                                  const QStringList &excludekeys) const
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return QStringList();
    }
    QStringList varlist;
    QList<PVariable> sortlist = getVarGroupList(pmodel, groupName, recursive, excludekeys);
    for (PVariable pvar : sortlist) {
        if (pvar != nullptr) {
            varlist.append(pvar->getName());
        }
    }
    return varlist;
}

QList<ModelRowItem> TableBaseModel::getModelRowItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                        const QStringList &groupList, bool recursive, bool readOndy,
                                                        const QStringList &excludekeys, const QString &path)
{
    if (pmodel == nullptr || groupList.size() <= 0) {
        return QList<ModelRowItem>();
    }
    QList<ModelRowItem> listdata;
    for (QString groupName : groupList) {
        listdata.append(getModelRowItemList(pmodel, groupName, recursive, readOndy, excludekeys, path));
    }
    return listdata;
}

QList<ModelRowItem> TableBaseModel::getModelRowItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                        const QString &groupName, bool recursive, bool readOndy,
                                                        const QStringList &excludekeys, const QString &path)
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return QList<ModelRowItem>();
    }
    QList<ModelRowItem> modellist;
    if (recursive) {
        for (PModel cmodel : pmodel->getChildModels().values()) {
            if (cmodel == nullptr) {
                continue;
            }
            QString tmppath;
            if (path.isEmpty()) {
                tmppath = cmodel->getName();
            } else {
                tmppath = path + "/" + cmodel->getName();
            }
            modellist.append(getModelRowItemList(cmodel, groupName, recursive, readOndy, excludekeys, tmppath));
        }
    }
    QList<PVariable> sortlist = getVarGroupList(pmodel, groupName, false, excludekeys);
    for (int i = 0; i < sortlist.size(); ++i) {
        if (sortlist[i] == nullptr) {
            continue;
        }
        modellist.append(ModelRowItem(path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                                      sortlist[i], pmodel, groupName));
    }
    return modellist;
}

QList<TableBaseModelItem> TableBaseModel::getTableBaseModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                                    const QString &prototypeName,
                                                                    const QString &groupName, bool recursive,
                                                                    const QStringList &excludekeys, const QString &path)
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return QList<TableBaseModelItem>();
    }
    TableBaseModel::VariableClass varclass = GroupNameMapToVariableClass(groupName);
    QList<TableBaseModelItem> tableItemllist;
    if (recursive) {
        for (PModel cmodel : pmodel->getChildModels().values()) {
            if (cmodel == nullptr) {
                continue;
            }
            QString tmppath;
            if (path.isEmpty()) {
                tmppath = cmodel->getName();
            } else {
                tmppath = path + "/" + cmodel->getName();
            }
            tableItemllist.append(
                    getTableBaseModelItemList(cmodel, prototypeName, groupName, recursive, excludekeys, tmppath));
        }
    }
    QList<PVariable> sortlist = getVarGroupList(pmodel, groupName, false, excludekeys);
    for (int i = 0; i < sortlist.size(); ++i) {
        if (sortlist[i] == nullptr) {
            continue;
        }
        tableItemllist.append(TableBaseModelItem(
                path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                sortlist[i]->getControlType(), sortlist[i]->getControlValue(), varclass, prototypeName));
    }
    return tableItemllist;
}

bool TableBaseModel::isPort(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &keyword) const
{
    if (pmodel == nullptr || keyword.isEmpty()) {
        return false;
    }
    PBlock pblock = pmodel.dynamicCast<Block>();
    if (pblock == nullptr) {
        return false;
    }
    QList<PVariable> pvarList =
            pblock->findVariableInGroup(RoleDataDefinition::PortGroup, RoleDataDefinition::NameRole, keyword);
    if (pvarList.size() > 0) {
        return true;
    }
    return false;
}

QMap<int, QModelIndexList> TableBaseModel::getSameRowIndexes(const QModelIndexList &indexlist)
{
    QMap<int, QModelIndexList> sameRowIndexMap;
    foreach (QModelIndex tmpindex, indexlist) {
        if (!tmpindex.isValid()) {
            continue;
        }
        if (sameRowIndexMap.contains(tmpindex.row())) {
            sameRowIndexMap[tmpindex.row()].append(tmpindex);
        } else {
            QModelIndexList indexlist;
            indexlist.append(tmpindex);
            sameRowIndexMap.insert(tmpindex.row(), indexlist);
        }
    }
    return sameRowIndexMap;
}

QString TableBaseModel::VariableClassMapToGroupName(const TableBaseModel::VariableClass &varclass) const
{
    switch (varclass) {
    case TableBaseModel::VariableClass_CtrInput:
        return RoleDataDefinition::InputSignal;
    case TableBaseModel::VariableClass_CtrOutPut:
        return RoleDataDefinition::OutputSignal;
    case TableBaseModel::VariableClass_CtrParam:
        return RoleDataDefinition::Parameter;
    case TableBaseModel::VariableClass_CtrCState:
        return RoleDataDefinition::ContinueStateVariable;
    case TableBaseModel::VariableClass_CtrDState:
        return RoleDataDefinition::DiscreteStateVariable;
    case TableBaseModel::VariableClass_ElecParam:
        return RoleDataDefinition::ElectricalParameter;
    case TableBaseModel::VariableClass_ElecDeviceType:
        return RoleDataDefinition::DeviceTypeParameter;
    case TableBaseModel::VariableClass_ElecSimuParam:
        return RoleDataDefinition::SimulationParameter;
    case TableBaseModel::VariableClass_ElecInitParam:
        return RoleDataDefinition::InitializationParameter;
    case TableBaseModel::VariableClass_ElecBusbarParam:
        return RoleDataDefinition::ElectricalParameter;
    case TableBaseModel::VariableClass_ElecPFResult:
        return RoleDataDefinition::LoadFlowResultParameter;
    default:
        break;
    }
    return QString();
}

const QModelIndex TableBaseModel::findKeywordIndex(const QModelIndex &index, const QString &keyword)
{
    QModelIndex tmpindex;
    for (int col = 0; col < columnCount(); ++col) {
        tmpindex = index.sibling(index.row(), col);
        if (tmpindex.isValid() && keyword == tmpindex.data(NPS::ModelDataKeywordsRole).toString()) {
            return tmpindex;
        }
    }
    return QModelIndex();
}

TableBaseModel::VariableClass TableBaseModel::GroupNameMapToVariableClass(const QString &groupName) const
{
    if (groupName.isEmpty()) {
        return TableBaseModel::VariableClass_Other;
    }

    if (RoleDataDefinition::InputSignal == groupName) {
        return TableBaseModel::VariableClass_CtrInput;
    } else if (RoleDataDefinition::OutputSignal == groupName) {
        return TableBaseModel::VariableClass_CtrOutPut;
    } else if (RoleDataDefinition::Parameter == groupName) {
        return TableBaseModel::VariableClass_CtrParam;
    } else if (RoleDataDefinition::DiscreteStateVariable == groupName) {
        return TableBaseModel::VariableClass_CtrDState;
    } else if (RoleDataDefinition::ContinueStateVariable == groupName) {
        return TableBaseModel::VariableClass_CtrCState;
    } else if (RoleDataDefinition::ElectricalParameter == groupName) {
        return TableBaseModel::VariableClass_ElecParam;
    } else if (RoleDataDefinition::DeviceTypeParameter == groupName) {
        return TableBaseModel::VariableClass_ElecDeviceType;
    } else if (RoleDataDefinition::SimulationParameter == groupName) {
        return TableBaseModel::VariableClass_ElecSimuParam;
    } else if (RoleDataDefinition::InitializationParameter == groupName) {
        return TableBaseModel::VariableClass_ElecInitParam;
    } else if (RoleDataDefinition::LoadFlowResultParameter == groupName) {
        return TableBaseModel::VariableClass_ElecPFResult;
    }
    return TableBaseModel::VariableClass_Other;
}

#include "TableModelDataModel.h"

#include "CoreLib/ServerManager.h"
#include "CustomTreeModel.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "ProjDataManager.h"
#include "ProjectViewServers.h"

using namespace Kcc::BlockDefinition;

USE_LOGOUT_("TableModelDataModel")

TableModelDataModel::TableModelDataModel(QSharedPointer<ProjDataManager> pProjDataManager, QObject *parent)
    : TableBaseModel(parent), m_pProjDataManager(pProjDataManager)
{
}

TableModelDataModel::~TableModelDataModel() { }

bool TableModelDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isModelDataValid(index)) {
        return false;
    }
    if (Qt::CheckStateRole != role && Qt::EditRole != role) {
        return false;
    }
    if (TableBaseModel::ModelDataType_Invalid == m_ModelDataType
        || TableBaseModel::ModelDataType_Tree == m_ModelDataType) {
        return false;
    }
    QVariant realValue = value;
    if (Qt::CheckStateRole == role) {
        realValue = (Qt::CheckState(value.toInt()) == Qt::Checked);
    }

    QString datatype = index.data(NPS::ModelDataTypeRole).toString();
    if ((NPS::isNumberDataType(datatype) && realValue.toString().trimmed().isEmpty())) {
        LOGOUT(NPS::LOG_VALUENULLSTRING(getTableName(), index.sibling(index.row(), 0).data().toString(),
                                        headerData(index.column(), Qt::Horizontal, Qt::UserRole).toString()),
               LOG_WARNING);
        return false;
    }

    if (realValue == index.data(NPS::ModelDataRealValueRole)) {
        return false;
    }
    OldNewDataStruct onds = getOldNewDataStruct(index.row(), realValue, index.data(NPS::ModelDataTypeRole).toString(),
                                                QModelIndexList() << index);
    if (onds.isValid()) {
        emit ModelValueChanged(QList<OldNewDataStruct>() << onds);
        return true;
    }
    return TableBaseModel::setData(index, value, role);
}

void TableModelDataModel::setCustomTreeModel(CustomTreeModel *pTreeModel)
{
    m_pTreeModel = pTreeModel;
}

void TableModelDataModel::updateTableData(const QModelIndex &index, int &freezeCol)
{
    freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_1;
    QList<QStringList> listdata;
    QStringList titlelist;
    if (!index.isValid() || m_pTreeModel == nullptr) {
        setReadOnlyData(listdata, titlelist);
        return;
    }
    QStandardItem *item = m_pTreeModel->itemFromIndex(index);
    if (item == nullptr) {
        return;
    }
    m_devicePrototype = "";
    m_boardActiveStsMap.clear();
    m_curProjActive = m_pTreeModel->getProjectActiveSts(index);
    int itemnodetype = index.data(CustomTreeModel::CustomRole_NodeType).toInt();
    if (CustomTreeModel::NodeType_DataRoot == itemnodetype) {
        titlelist << AssistantDefine::KEYWORD_TITLE_WSNAME << AssistantDefine::KEYWORD_TITLE_WSPATH;
        QMap<QString, WorkSpaceInfo> allws = m_pProjDataManager->getAllWorkSpace();
        for (QString wsname : allws.keys()) {
            listdata.append(QStringList() << wsname << allws[wsname].getWorkspaceAbsolutePath());
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_GlobalLib == itemnodetype
               || CustomTreeModel::NodeType_GlobalCtrBlock == itemnodetype
               || CustomTreeModel::NodeType_LocalLib == itemnodetype
               || (index.parent().isValid()
                   && CustomTreeModel::NodeType_GlobalCtrBlock
                           == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt())) {
        titlelist.append(AssistantDefine::KEYWORD_TITLE_NAME);
        for (int row = 0; row < item->rowCount(); ++row) {
            if (item->child(row, 0) != nullptr) {
                listdata.append(QStringList() << item->child(row, 0)->text());
            }
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_GlobalDeviceType == itemnodetype
               || CustomTreeModel::NodeType_DeviceType == itemnodetype
               || CustomTreeModel::NodeType_CodeBlock == itemnodetype
               || CustomTreeModel::NodeType_NetworkDeviceType == itemnodetype
               || CustomTreeModel::NodeType_NetworkElecElement == itemnodetype
               || CustomTreeModel::NodeType_NetworkBlockInstance == itemnodetype
               || CustomTreeModel::NodeType_NetworkComplexInstance == itemnodetype
               || CustomTreeModel::NodeType_NetworkControlSystem == itemnodetype) {
        titlelist << AssistantDefine::KEYWORD_TITLE_NAME << AssistantDefine::KEYWORD_TITLE_COUNT;
        for (int row = 0; row < item->rowCount(); ++row) {
            if (item->child(row, 0) != nullptr) {
                listdata.append(
                        QStringList()
                        << item->child(row, 0)->text()
                        << QString::number(
                                   m_pTreeModel->getModels(m_pTreeModel->indexFromItem(item->child(row, 0))).size()));
            }
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_Workspace == itemnodetype) {
        titlelist << AssistantDefine::KEYWORD_TITLE_PROJNAME << AssistantDefine::KEYWORD_TITLE_PROJPATH;
        QMap<QString, WorkSpaceInfo> allws = m_pProjDataManager->getAllWorkSpace();
        for (QString wsname : allws.keys()) {
            if (wsname == index.data()) {
                for (QString projname : allws[wsname].allprojects.keys()) {
                    listdata.append(QStringList() << projname << allws[wsname].getProjectAbsolutePath(projname));
                }
            }
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_Project == itemnodetype) {
        titlelist << AssistantDefine::KEYWORD_TITLE_NAME << AssistantDefine::KEYWORD_TITLE_PARAM;
        PKLProject pProject = m_pTreeModel->getProjectByIndex(index);
        if (pProject != nullptr) {
            QVariantMap projcfgmap = pProject->getProjectConfig();
            listdata.append(QStringList() << NPS::SIMUPARAMS_STEP_SIZE << projcfgmap[KL_PRO::STEP_SIZE].toString());
            listdata.append(QStringList()
                            << NPS::SIMUPARAMS_START_TIME << projcfgmap[KL_PRO::SIMU_START_TIME].toString());
            listdata.append(QStringList() << NPS::SIMUPARAMS_TIME << projcfgmap[KL_PRO::RUN_TIME].toString());
            QString algTypeStr = valueMaptoAlgorithm(projcfgmap[KL_PRO::ODE_SOLVER]);
            listdata.append(QStringList() << NPS::SIMUPARAMS_ALGTYPE << algTypeStr);
            listdata.append(QStringList() << NPS::SIMUPARAMS_FRE << projcfgmap[KL_PRO::FREQUENCY].toString());
            listdata.append(QStringList()
                            << NPS::SIMUPARAMS_INIT
                            << (projcfgmap[KL_PRO::ISINITIALIZED].toBool() ? NPS::SIMUPARAMS_INIT_SYS_DEF
                                                                           : NPS::SIMUPARAMS_INIT_USER_DEF));
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_SimuResult == itemnodetype) {
        titlelist << AssistantDefine::KEYWORD_TITLE_SIMURESULTNAME << NPS::SIMUPARAMS_STEP_SIZE
                  << NPS::SIMUPARAMS_START_TIME << NPS::SIMUPARAMS_TIME << NPS::SIMUPARAMS_ALGTYPE
                  << NPS::SIMUPARAMS_FRE << NPS::SIMUPARAMS_INIT << AssistantDefine::KEYWORD_TITLE_DATASIZE
                  << AssistantDefine::KEYWORD_TITLE_RUNTIME;
        QList<SimulationResultInfo> simuList = m_pTreeModel->getSimuResultInfoList(index);
        PKLProject project = m_pTreeModel->getProjectByIndex(index);
        if (project == nullptr) {
            setReadOnlyData(listdata, titlelist);
            return;
        }
        for (SimulationResultInfo info : simuList) {
            listdata.append(QStringList()
                            << (info.name == AssistantDefine::CURRENTSIMULATION_REALNAME
                                        ? AssistantDefine::CURRENTSIMULATION_NAME
                                        : info.name)
                            << info.config[KL_PRO::STEP_SIZE].toString()
                            << info.config[KL_PRO::SIMU_START_TIME].toString()
                            << info.config[KL_PRO::RUN_TIME].toString()
                            << valueMaptoAlgorithm(info.config[KL_PRO::ODE_SOLVER])
                            << info.config[KL_PRO::FREQUENCY].toString()
                            << (info.config[KL_PRO::ISINITIALIZED].toBool() ? NPS::SIMUPARAMS_INIT_SYS_DEF
                                                                            : NPS::SIMUPARAMS_INIT_USER_DEF)
                            << QString::number(project->getSimulationDataFileSize(info.name) / (1024.0 * 1024.0))
                            << info.recordTime.toString("yyyy-MM-dd hh:mm:ss"));
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_SimuResultSubItem == itemnodetype) {
        titlelist << AssistantDefine::KEYWORD_TITLE_BOARDNAME << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME
                  << AssistantDefine::KEYWORD_TITLE_VARIABLENAME;
        SimulationResultInfo resultSubInfo = m_pTreeModel->getSimuResultInfo(index);
        for (ParamIdentifier detailItem : resultSubInfo.params) {
            listdata.append(QStringList() << detailItem.board_name << detailItem.block_name << detailItem.port_name);
        }
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_DeviceTypeClass == itemnodetype) {
        m_devicePrototype = index.data(CustomTreeModel::CustomRole_PrototypeName).toString();
        freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_1;
        if (index.parent().isValid()
            && CustomTreeModel::NodeType_GlobalDeviceType
                    == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            setModelData(m_pTreeModel->getModels(index), TableBaseModel::ModelDataType_GlobalDeviceTypes);
        } else {
            setModelData(m_pTreeModel->getModels(index), TableBaseModel::ModelDataType_ProjectDeviceTypes);
        }
    } else if (CustomTreeModel::NodeType_CodeBlockClass == itemnodetype
               || CustomTreeModel::NodeType_NetworkBlockTemplate == itemnodetype) {
        freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_1;
        setModelData(m_pTreeModel->getModels(index), TableBaseModel::ModelDataType_CodeBlocks);
    } else if (CustomTreeModel::NodeType_NetworkControlBlock == itemnodetype) {
        freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_1;
        setModelData(m_pTreeModel->getModels(index), TableBaseModel::ModelDataType_ControlBlocks);
    } else if (CustomTreeModel::NodeType_NetworkBlockInstanceClass == itemnodetype) {
        freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_3;
        setModelData(m_pTreeModel->getModels(index), TableBaseModel::ModelDataType_ControlInstances,
                     index.data().toString());
    } else if (CustomTreeModel::NodeType_ElecElementClass == itemnodetype) {
        freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_3;
        PKLProject project = m_pTreeModel->getProjectByIndex(index);
        if (m_curProjActive && project != nullptr
            && project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
            m_boardActiveStsMap = project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
        }
        setModelData(m_pTreeModel->getModels(index), TableBaseModel::ModelDataType_ElectricalBlocks,
                     index.data(CustomTreeModel::CustomRole_PrototypeName).toString());
    } else if (CustomTreeModel::NodeType_CombinedModel == itemnodetype) {
        getDrawBoardReadOnlyData(m_pTreeModel->getModels(index), listdata, titlelist);
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_ComplexModel == itemnodetype
               || CustomTreeModel::NodeType_NetworkComplexTemplate == itemnodetype) {
        getDrawBoardReadOnlyData(m_pTreeModel->getModels(index), listdata, titlelist);
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_Network == itemnodetype) {
        getDrawBoardReadOnlyData(m_pTreeModel->getModels(index), listdata, titlelist);
        setReadOnlyData(listdata, titlelist);
    } else if (CustomTreeModel::NodeType_Model == itemnodetype
               || CustomTreeModel::NodeType_ModelInstance == itemnodetype) {
        freezeCol = AssistantDefine::TABLE_FREEZE_COLUMN_1;
        PModel pmodel = m_pTreeModel->getModel(index);
        if (pmodel == nullptr) {
            setModelData(QList<PModel>(), TableBaseModel::ModelDataType_Invalid);
            return;
        }

        if (ElecBoardModel::Type == pmodel->getModelType() || ElecCombineBoardModel::Type == pmodel->getModelType()) {
            titlelist << AssistantDefine::KEYWORD_TITLE_NAME << AssistantDefine::KEYWORD_TITLE_COUNT;
            for (int crow = 0; crow < item->rowCount(); ++crow) {
                QStandardItem *citem = item->child(crow, 0);
                if (citem == nullptr) {
                    continue;
                }
                QModelIndex cindex = m_pTreeModel->indexFromItem(citem);
                if (!cindex.isValid()) {
                    continue;
                }
                listdata.append(QStringList() << item->child(crow, 0)->text()
                                              << QString::number(m_pTreeModel->getModels(cindex).size()));
            }
            setReadOnlyData(listdata, titlelist);
        } else if (CombineBoardModel::Type == pmodel->getModelType()) {
            if (pmodel->isInstance()) {
                setModelData(QList<PModel>() << pmodel, TableBaseModel::ModelDataType_CombineBoardInstance);
            } else {
                setModelData(pmodel->getChildModels().values(), TableBaseModel::ModelDataType_CombineBoardBlocks);
            }
        } else if (ComplexBoardModel::Type == pmodel->getModelType()) {
            if (pmodel->isInstance()) {
                setModelData(pmodel->getChildModels().values(), TableBaseModel::ModelDataType_ComplexBoardInstances);
            } else {
                setModelData(pmodel->getChildModels().values(), TableBaseModel::ModelDataType_ComplexBoardBlocks);
            }
        } else if (ControlBoardModel::Type == pmodel->getModelType()) {
            setModelData(pmodel->getChildModels().values(), TableBaseModel::ModelDataType_ControlBoardBlocks);
        } else if (ControlBlock::Type == pmodel->getModelType() && pmodel->isInstance()) {
            setModelData(QList<PModel>() << pmodel, TableBaseModel::ModelDataType_CodeInstance);
        } else if (ControlBlock::Type == pmodel->getModelType() && !pmodel->isInstance()) {
            setModelData(QList<PModel>() << pmodel, TableBaseModel::ModelDataType_BuildInBlocks);
        } else {
            setModelData(QList<PModel>(), TableBaseModel::ModelDataType_Invalid);
        }
    } else {
        setReadOnlyData(listdata, titlelist);
    }
    m_drawBoardList = ProjectViewServers::getInstance().m_projectManagerServer->GetAllBoardModel();
}

void TableModelDataModel::batchModifyModel(const QModelIndexList &indexlist, const QVariant &newvalue,
                                           const QString &valuetype, QList<OldNewDataStruct> &modifylist)
{
    modifylist.clear();
    // 同一行数据对应同一个block,key 行，value是当前行所有需要修改的index
    QMap<int, QModelIndexList> sameBlockIndexMap = NPS::getSameRowIndexes(indexlist);
    for (int row : sameBlockIndexMap.keys()) {
        OldNewDataStruct onebs = getOldNewDataStruct(row, newvalue, valuetype, sameBlockIndexMap[row]);
        if (onebs.isValid()) {
            modifylist.append(onebs);
        }
    }
}

void TableModelDataModel::modifyData(const QList<OldNewDataStruct> &modifylist, bool bredo)
{
    QStringList saveModelUUIDList;
    QList<PModel> emitVerifyModels;
    for (OldNewDataStruct item : modifylist) {
        bool change = false;
        if (!item.isValid()) {
            continue;
        }
        int rowno = -1;
        PModel pmod = getModelItemByDataStruct(item, rowno);
        if (pmod == nullptr) {
            continue;
        }
        if (pmod->getModelType() == DeviceModel::Type) {
            change = modifyDeviceTypeModel(pmod, rowno, item.getValueMap(bredo));
        } else if (pmod->getModelType() == ElectricalBlock::Type) {
            change = modifyElectricalBlock(pmod, rowno, item.getValueMap(bredo));
        } else if (pmod->isInstance()) {
            if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
                || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
                change = modifyBlockInstance(pmod, item.getValueMap(bredo));
            } else if (TableBaseModel::ModelDataType_ControlInstances == m_ModelDataType) {
                change = modifyControlInstances(pmod, rowno, item.getValueMap(bredo));
            }
        }
        if (change) {
            pmod->setModifyTime(QDateTime::currentDateTime());
            PModel topParentModel = pmod->getTopParentModel();
            if (topParentModel != nullptr && ProjectViewServers::getInstance().m_pGraphicModelingServer != nullptr
                && !ProjectViewServers::getInstance().m_pGraphicModelingServer->hasDrawingBoardById(
                        topParentModel->getObjectUuid())
                && !saveModelUUIDList.contains(topParentModel->getModelUuid())) {
                saveModelUUIDList.append(topParentModel->getModelUuid());
            }
            if (topParentModel != nullptr && !emitVerifyModels.contains(topParentModel)) {
                emitVerifyModels.append(topParentModel);
            }
        }
    }
    for (QString realUUID : saveModelUUIDList) {
        saveProjectModelByUUID(realUUID);
    }
    for (PModel vmodel : emitVerifyModels) {
        if (vmodel != nullptr) {
            emit VerifyDrawBoard(vmodel->getUUID(), vmodel->getModelType());
        }
    }
}

void TableModelDataModel::pasteDeviceModel(QSharedPointer<Kcc::BlockDefinition::Model> devModel,
                                           const QModelIndex &sourceIndex, QList<OldNewDataStruct> &modifylist)
{
    modifylist.clear();
    if (devModel == nullptr || !sourceIndex.isValid()) {
        return;
    }
    PModel model = getModelByIndex(sourceIndex);
    if (model == nullptr || model == devModel) {
        return;
    }
    PDeviceModel sourceModel = devModel.dynamicCast<DeviceModel>();
    PDeviceModel curDevModel = model.dynamicCast<DeviceModel>();
    if (curDevModel == nullptr || sourceModel == nullptr
        || sourceModel->getDeviceTypePrototypeName() != curDevModel->getDeviceTypePrototypeName()) {
        return;
    }

    OldNewDataStruct oldnewStruct(curDevModel->getUUID(), curDevModel->getModelType());
    bool haveChanged = false;
    QString errorinfo = "";
    QString name = "";
    QString datatypestr = "";
    QVariant oldvalue = QVariant();
    QVariant newvalue = QVariant();
    QList<PVariable> sortList = getVarGroupList(curDevModel, RoleDataDefinition::DeviceTypeParameter, false);
    for (PVariable pvar : sortList) {
        oldvalue = QVariant();
        newvalue = QVariant();
        if (pvar == nullptr) {
            continue;
        }
        PVariable sourceVar = sourceModel->getVariableByRoleData(RoleDataDefinition::DeviceTypeParameter,
                                                                 RoleDataDefinition::NameRole, pvar->getName());
        if (sourceVar == nullptr || pvar->getControlType() != sourceVar->getControlType()) {
            continue;
        }
        oldvalue = curDevModel->getVariableData(pvar, RoleDataDefinition::ValueRole);
        newvalue = sourceModel->getVariableData(sourceVar, RoleDataDefinition::ValueRole);
        if (oldvalue != newvalue) {
            oldnewStruct.addChangedItem(TableBaseModel::VariableClass_ElecDeviceType, pvar->getName(), oldvalue,
                                        newvalue);
            haveChanged = true;
        }
    }
    modifylist.append(oldnewStruct);
}

void TableModelDataModel::emitDataChange(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (TableBaseModel::ModelDataType_Invalid == m_ModelDataType
        || TableBaseModel::ModelDataType_Tree == m_ModelDataType
        || TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
        || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType || model == nullptr) {
        return;
    }
    QList<QWeakPointer<Model>> connectList;
    for (int row = 0; row < m_modelData.size(); ++row) {
        if (m_modelData[row] == nullptr) {
            continue;
        }
        if (m_modelData[row] == model) {
            emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
            break;
        }
        connectList.clear();
        connectList = m_modelData[row]->getConnectedModel(ModelConnSlot)
                + m_modelData[row]->getConnectedModel(ModelConnDeviceType)
                + m_modelData[row]->getConnectedModel(ModelConnElectricInterface);
        for (QWeakPointer<Model> wmodel : connectList) {
            if (wmodel == nullptr) {
                continue;
            }
            PModel smodel = wmodel.toStrongRef();
            if (smodel != nullptr && smodel->getObjectUuid() == model->getObjectUuid()) {
                emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
                break;
            }
        }
    }
}

QVariant TableModelDataModel::TModelData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (TableBaseModel::ModelDataType_Invalid == m_ModelDataType
        || TableBaseModel::ModelDataType_Tree == m_ModelDataType) {
        return TableBaseModel::TModelData(index, modeldata);
    }
    if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
        || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
        return BlockInstanceData(index, modeldata);
    }

    if (!isModelDataValid(index)) {
        return QVariant();
    }

    if (TableBaseModel::ModelDataType_CodeBlocks == m_ModelDataType
        || TableBaseModel::ModelDataType_CombineBoardBlocks == m_ModelDataType
        || TableBaseModel::ModelDataType_ControlBoardBlocks == m_ModelDataType
        || TableBaseModel::ModelDataType_BuildInBlocks == m_ModelDataType) {
        return ControlBlockData(m_modelData[index.row()], index, modeldata);
    } else if (TableBaseModel::ModelDataType_ComplexBoardBlocks == m_ModelDataType
               || TableBaseModel::ModelDataType_ComplexBoardInstances == m_ModelDataType) {
        return ComplexSlotBlockData(m_modelData[index.row()], index, modeldata);
    } else if (TableBaseModel::ModelDataType_GlobalDeviceTypes == m_ModelDataType
               || TableBaseModel::ModelDataType_ProjectDeviceTypes == m_ModelDataType) {
        return DeviceTypeModelData(m_modelData[index.row()], index, modeldata);
    } else if (TableBaseModel::ModelDataType_ElectricalBlocks == m_ModelDataType) {
        return ElectricalBlockData(m_modelData[index.row()], index, modeldata);
    } else if (TableBaseModel::ModelDataType_ControlInstances == m_ModelDataType) {
        return ControlInstancesData(index, modeldata);
    } else if (TableBaseModel::ModelDataType_ControlBlocks == m_ModelDataType) {
        return ControlBlockData(m_modelData[index.row()], index, modeldata, true);
    }

    return QVariant();
}

QVariant TableModelDataModel::ControlBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                               const QModelIndex &index, TableBaseModel::ModelData modeldata,
                                               bool editable) const
{
    if (pmodel == nullptr) {
        return QVariant();
    }

    if (AssistantDefine::KEYWORD_TITLE_MODELNAME == m_ModelItemList[index.column()].Keywords
        || AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return editable;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pmodel->getName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_BOARDNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return editable;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pmodel->getParentModelName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_INPUTVAR == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return editable;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return getVarGroupStringList(pmodel, RoleDataDefinition::InputSignal).join(",");
        }
    } else if (AssistantDefine::KEYWORD_TITLE_OUTPUTVAR == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return editable;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return getVarGroupStringList(pmodel, RoleDataDefinition::OutputSignal).join(",");
        }
    } else if (AssistantDefine::KEYWORD_TITLE_STATE == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return editable;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return getVarGroupStringList(pmodel, RoleDataDefinition::DiscreteStateVariable).join(",");
        }
    } else if (AssistantDefine::KEYWORD_TITLE_PARAM == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return editable;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return getVarGroupStringList(pmodel, RoleDataDefinition::Parameter).join(",");
        }
    } else if (AssistantDefine::KEYWORD_TITLE_EDITTIME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pmodel->getModifyTime();
        }
    }
    return QVariant();
}

QVariant TableModelDataModel::ComplexSlotBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                   const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (pmodel == nullptr) {
        return QVariant();
    }

    PSlotBlock psblock = pmodel.dynamicCast<SlotBlock>();
    if (psblock == nullptr) {
        return QVariant();
    }
    if (TableBaseModel::ModelData_Edit == modeldata) {
        return false;
    }
    if (AssistantDefine::KEYWORD_TITLE_SLOT == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Value == modeldata) {
            return psblock->getName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Value == modeldata) {
            if (!psblock->isInstance()) {
                return QVariant();
            }
            if (psblock->getTargetModel() != nullptr) {
                return psblock->getTargetModel()->getName();
            }
            return QVariant();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_MODELPROTOTYPE == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Value == modeldata) {
            return psblock->getFilterInfo(RoleDataDefinition::FilerProtoTypeRole);
        }
    } else if (AssistantDefine::KEYWORD_TITLE_INPUTVAR == m_ModelItemList[index.column()].Keywords) {
        if (psblock->isInstance()) {
            return getSlotInstanceGroupStringList(psblock, RoleDataDefinition::InputSignal);
        } else {
            return getVarGroupStringList(psblock, RoleDataDefinition::InputSignal).join(",");
        }
    } else if (AssistantDefine::KEYWORD_TITLE_OUTPUTVAR == m_ModelItemList[index.column()].Keywords) {
        if (psblock->isInstance()) {
            return getSlotInstanceGroupStringList(psblock, RoleDataDefinition::OutputSignal);
        } else {
            return getVarGroupStringList(psblock, RoleDataDefinition::OutputSignal).join(",");
        }
    } else if (AssistantDefine::KEYWORD_TITLE_STATE == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Value == modeldata) {
            return getSlotInstanceGroupStringList(psblock, RoleDataDefinition::DiscreteStateVariable);
        }
    } else if (AssistantDefine::KEYWORD_TITLE_PARAM == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Value == modeldata) {
            return getSlotInstanceGroupStringList(psblock, RoleDataDefinition::Parameter);
        }
    } else if (AssistantDefine::KEYWORD_TITLE_EDITTIME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Value == modeldata) {
            return psblock->getModifyTime();
        }
    }
    return QVariant();
}

QVariant TableModelDataModel::DeviceTypeModelData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                  const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (pmodel == nullptr) {
        return QVariant();
    }

    PDeviceModel pdevmodel = pmodel.dynamicCast<DeviceModel>();
    if (pdevmodel == nullptr) {
        return QVariant();
    }

    if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pdevmodel->getName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_CLASS == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_ModelDataType == TableBaseModel::ModelDataType_GlobalDeviceTypes
                    ? AssistantDefine::KEYWORD_TITLE_DEVICETYPE_CLASS_GLOBAL
                    : AssistantDefine::KEYWORD_TITLE_DEVICETYPE_CLASS_LOCAL;
        }
    } else if (AssistantDefine::KEYWORD_TITLE_EDITTIME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pdevmodel->getModifyTime();
        }
    } else if (pdevmodel->getDeviceTypePVariable(m_ModelItemList[index.column()].Keywords) != nullptr) {
        return getParamValue(pdevmodel, RoleDataDefinition::DeviceTypeParameter,
                             m_ModelItemList[index.column()].Keywords, modeldata);
    }
    return QVariant();
}

QVariant TableModelDataModel::ElectricalBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                  const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (pmodel == nullptr) {
        return QVariant();
    }
    PElectricalBlock pemodel = pmodel.dynamicCast<ElectricalBlock>();
    if (pemodel == nullptr) {
        return QVariant();
    }

    if (AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pemodel->getName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            if (pemodel->getDeviceModel() != nullptr) {
                return pemodel->getDeviceModel()->getName();
            }
            return QVariant();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_BOARDNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pemodel->getParentModelName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_EDITTIME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return pemodel->getModifyTime();
        }
    } else if (TableBaseModel::VariableClass_ElecBusbarParam == m_ModelItemList[index.column()].varClass) {
        // busbar参数
        QString uuid = pemodel->getUUID() + pemodel->getParentModelUUID();
        if (m_conElecBusBarMap.contains(uuid)) {
            for (QString keystr : m_conElecBusBarMap[uuid].keys()) {
                if (m_ModelItemList[index.column()].Keywords.contains(keystr)
                    && m_conElecBusBarMap[uuid].value(keystr) != nullptr) {
                    switch (modeldata) {
                    case TableBaseModel::ModelData_Value: {
                        if (m_ModelItemList[index.column()].Keywords
                            == keystr + "|" + NPS::KEYWORD_ELECTRICALPARAM_NAME) {
                            return m_conElecBusBarMap[uuid][keystr]->getName();
                        } else {
                            return getParamValue(m_conElecBusBarMap[uuid][keystr],
                                                 RoleDataDefinition::ElectricalParameter,
                                                 m_ModelItemList[index.column()].Keywords.split("|")[1],
                                                 TableBaseModel::ModelData_Value);
                        }
                    }
                    case TableBaseModel::ModelData_Edit: {
                        return false; // 母线只显示不可编辑
                    }
                    default:
                        break;
                    }
                    return QVariant();
                }
            }
        }
    } else if (TableBaseModel::VariableClass_ElecParam == m_ModelItemList[index.column()].varClass) {
        // 电气参数
        return getParamValue(pemodel, RoleDataDefinition::ElectricalParameter, m_ModelItemList[index.column()].Keywords,
                             modeldata);
    } else if (TableBaseModel::VariableClass_ElecDeviceType == m_ModelItemList[index.column()].varClass) {
        // 设备类型参数
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        }
        if (pemodel->getDeviceModel() != nullptr) {
            return getParamValue(pemodel->getDeviceModel(), RoleDataDefinition::DeviceTypeParameter,
                                 m_ModelItemList[index.column()].Keywords, modeldata);
        }
    } else if (TableBaseModel::VariableClass_ElecSimuParam == m_ModelItemList[index.column()].varClass) {
        // 仿真参数
        return getParamValue(pemodel, RoleDataDefinition::SimulationParameter, m_ModelItemList[index.column()].Keywords,
                             modeldata);
    } else if (TableBaseModel::VariableClass_ElecInitParam == m_ModelItemList[index.column()].varClass) {
        // // 初始值参数
        return getParamValue(pemodel, RoleDataDefinition::InitializationParameter,
                             m_ModelItemList[index.column()].Keywords, modeldata);
    } else if (TableBaseModel::VariableClass_ElecPFResult == m_ModelItemList[index.column()].varClass) {
        // // 潮流计算结果
        return getParamValue(pemodel, RoleDataDefinition::LoadFlowParameter, m_ModelItemList[index.column()].Keywords,
                             modeldata);
    }
    return QVariant();
}

QVariant TableModelDataModel::BlockInstanceData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (!isModelDataValid(index)) {
        return QVariant();
    }

    if (AssistantDefine::KEYWORD_TITLE_PATH == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_instanceData[index.row()].getPath();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_VARIABLENAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            if (RoleDataDefinition::Parameter != m_instanceData[index.row()].groupName) {
                return m_instanceData[index.row()].getName();
            } else {
                return QString();
            }
        }
    } else if (AssistantDefine::KEYWORD_TITLE_PARAM_NAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            if (RoleDataDefinition::Parameter == m_instanceData[index.row()].groupName) {
                return m_instanceData[index.row()].getName();
            } else {
                return QString();
            }
        }
    } else if (AssistantDefine::KEYWORD_TITLE_TYPE == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_instanceData[index.row()].pVar->getData(RoleDataDefinition::DataTypeRole);
        }
    } else if (AssistantDefine::KEYWORD_TITLE_VALUE == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            if (RoleDataDefinition::InputSignal == m_instanceData[index.row()].groupName) {
                return false;
            }
            return NPS::isVarCanEdit(m_instanceData[index.row()].model, m_instanceData[index.row()].getName());
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_instanceData[index.row()].model->getVariableData(m_instanceData[index.row()].pVar,
                                                                      RoleDataDefinition::ValueRole);
        }
    } else if (AssistantDefine::KEYWORD_TITLE_DESCRIPTION == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_instanceData[index.row()].pVar->getData(RoleDataDefinition::DescriptionRole);
        }
    }

    return QVariant();
}

QVariant TableModelDataModel::ControlInstancesData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const
{
    if (!isModelDataValid(index)) {
        return QVariant();
    }

    if (AssistantDefine::KEYWORD_TITLE_MODELNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_modelData[index.row()]->getName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_BOARDNAME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            QList<QWeakPointer<Kcc::BlockDefinition::Model>> slotModel =
                    m_modelData[index.row()]->getConnectedModel(ModelConnSlot);
            if (slotModel.size() <= 0 || slotModel[0] == nullptr || slotModel[0].toStrongRef() == nullptr) {
                return QString();
            }
            return slotModel[0].toStrongRef()->getParentModelName();
        }
    } else if (AssistantDefine::KEYWORD_TITLE_EDITTIME == m_ModelItemList[index.column()].Keywords) {
        if (TableBaseModel::ModelData_Edit == modeldata) {
            return false;
        } else if (TableBaseModel::ModelData_Value == modeldata) {
            return m_modelData[index.row()]->getModifyTime();
        }
    } else {
        return getControlInstanceValue(index, modeldata);
    }

    return QVariant();
}

bool TableModelDataModel::modifyDeviceTypeModel(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const int &row,
                                                const QMap<int, QMap<QString, QVariant>> &modfymap)
{
    if (pmodel == nullptr) {
        return false;
    }
    PDeviceModel pdevmodel = pmodel.dynamicCast<DeviceModel>();
    if (pdevmodel == nullptr) {
        return false;
    }
    bool changed = false;
    for (int varclass : modfymap.keys()) {
        if (TableBaseModel::VariableClass_ElecDeviceType != varclass) {
            continue;
        }
        for (QString keyword : modfymap[varclass].keys()) {
            PVariable pavr = pdevmodel->getDeviceTypePVariable(keyword);
            if (pavr == nullptr) {
                continue;
            }
            LOGOUT(NPS::outputLog(AssistantDefine::TABLE_NAME_DEVICETYPES, pdevmodel->getName(), pavr->getDisplayName(),
                                  pdevmodel->getDeviceTypeVariableValue(keyword), modfymap[varclass][keyword], keyword,
                                  pavr->getControlType(), pavr->getControlValue()));
            pdevmodel->setVariableData(pavr, RoleDataDefinition::ValueRole, modfymap[varclass][keyword]);
            changed = true;
        }
    }
    if (changed) {
        emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
    }
    return changed;
}

bool TableModelDataModel::modifyElectricalBlock(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const int &row,
                                                const QMap<int, QMap<QString, QVariant>> &modfymap)
{
    if (pmodel == nullptr) {
        return false;
    }
    PElectricalBlock peblock = pmodel.dynamicCast<ElectricalBlock>();
    if (peblock == nullptr) {
        return false;
    }
    bool changed = false;
    QVariant oldvalue = QVariant();
    for (int varclass : modfymap.keys()) {
        for (QString keyword : modfymap[varclass].keys()) {
            oldvalue = QVariant();
            PVariable pavr(nullptr);
            if (TableBaseModel::VariableClass_ElecParam == varclass) {
                oldvalue = peblock->getElectricalVariableValue(keyword);
                pavr = peblock->getElectricalPVariable(keyword);
            } else if (TableBaseModel::VariableClass_ElecSimuParam == varclass) {
                oldvalue = peblock->getSimulationVariableValue(keyword);
                pavr = peblock->getSimulationPVariable(keyword);
            } else if (TableBaseModel::VariableClass_ElecInitParam == varclass) {
                oldvalue = peblock->getInitializationVariableValue(keyword);
                pavr = peblock->getInitializationPVariable(keyword);
            }
            if (pavr == nullptr) {
                continue;
            }
            LOGOUT(NPS::outputLog(AssistantDefine::TABLE_NAME_ELECTRICALBLOCKS, peblock->getName(),
                                  pavr->getDisplayName(), oldvalue, modfymap[varclass][keyword], keyword,
                                  pavr->getControlType(), pavr->getControlValue()));
            peblock->setVariableData(pavr, RoleDataDefinition::ValueRole, modfymap[varclass][keyword]);
            changed = true;
        }
    }
    if (changed) {
        emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
    }
    return changed;
}

bool TableModelDataModel::modifyBlockInstance(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                              const QMap<int, QMap<QString, QVariant>> &modfymap)
{
    if (pmodel == nullptr) {
        return false;
    }
    PModel parentModel = pmodel;
    while (parentModel->getParentModel() != nullptr) {
        parentModel = parentModel->getParentModel();
    }

    bool changed = false;
    QList<int> changeRowList;
    QVariant oldvalue = QVariant();
    for (int varclass : modfymap.keys()) {
        for (QString keyword : modfymap[varclass].keys()) {
            oldvalue = QVariant();
            PVariable pavr =
                    pmodel->getVariableByRoleData(VariableClassMapToGroupName(TableBaseModel::VariableClass(varclass)),
                                                  RoleDataDefinition::NameRole, keyword);
            if (pavr == nullptr) {
                continue;
            }
            oldvalue = pmodel->getVariableData(pavr, RoleDataDefinition::ValueRole);
            if (oldvalue == modfymap[varclass][keyword]) {
                continue;
            }
            changeRowList.append(getInstanceVarRow(
                    pmodel->getUUID(), VariableClassMapToGroupName(TableBaseModel::VariableClass(varclass)), keyword));
            LOGOUT(NPS::outputLog(getTableName(), parentModel != nullptr ? parentModel->getName() : pmodel->getName(),
                                  getInstanceKeywordPath(pmodel, pavr->getName()), oldvalue,
                                  modfymap[varclass][keyword], keyword, pavr->getControlType(),
                                  pavr->getControlValue()));
            pmodel->setVariableData(pavr, RoleDataDefinition::ValueRole, modfymap[varclass][keyword]);
            changed = true;
        }
    }
    for (int row : changeRowList) {
        emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
    }
    return changed;
}

bool TableModelDataModel::modifyControlInstances(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const int &row,
                                                 const QMap<int, QMap<QString, QVariant>> &modfymap)
{
    if (pmodel == nullptr) {
        return false;
    }
    bool changed = false;
    QVariant oldvalue = QVariant();
    for (int varclass : modfymap.keys()) {
        for (QString keyword : modfymap[varclass].keys()) {
            PModel realModel = findRealModel(pmodel, keyword);
            if (realModel == nullptr) {
                continue;
            }
            oldvalue = QVariant();
            PVariable pavr = realModel->getVariableByRoleData(
                    VariableClassMapToGroupName(TableBaseModel::VariableClass(varclass)), RoleDataDefinition::NameRole,
                    keyword.section("/", -1));
            if (pavr == nullptr) {
                continue;
            }
            oldvalue = realModel->getVariableData(pavr, RoleDataDefinition::ValueRole);
            if (oldvalue == modfymap[varclass][keyword]) {
                continue;
            }

            LOGOUT(NPS::outputLog(AssistantDefine::TABLE_NAME_CONTROLBOARDBLOCKS, pmodel->getName(), keyword, oldvalue,
                                  modfymap[varclass][keyword], pavr->getName(), pavr->getControlType(),
                                  pavr->getControlValue()));
            realModel->setVariableData(pavr, RoleDataDefinition::ValueRole, modfymap[varclass][keyword]);
            changed = true;
        }
    }
    if (changed) {
        emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
    }
    return changed;
}

void TableModelDataModel::setModelData(QList<QSharedPointer<Kcc::BlockDefinition::Model>> models,
                                       TableBaseModel::ModelDataType modeldatatype, const QString &prototypeName)
{
    beginResetModel();
    clear();
    m_ModelDataType = modeldatatype;
    m_headerBreak = TableBaseModel::HeaderBreak_Unit;
    if (TableBaseModel::ModelDataType_CodeInstance == modeldatatype
        || TableBaseModel::ModelDataType_CombineBoardInstance == modeldatatype) {
        if (models.size() <= 0) {
            m_instanceModel = PModel(nullptr);
        } else {
            m_instanceModel = models[0];
        }
        m_instanceData =
                getModelRowItemList(m_instanceModel,
                                    QStringList() << RoleDataDefinition::InputSignal << RoleDataDefinition::OutputSignal
                                                  << RoleDataDefinition::Parameter,
                                    true);
    } else {
        m_modelData = models;
    }

    if (TableBaseModel::ModelDataType_CodeBlocks == m_ModelDataType
        || TableBaseModel::ModelDataType_ControlBlocks == m_ModelDataType) {
        // 代码型模块固定列：模块名，画板名，输入变量，输出变量，状态变量，参数，编辑时间
        updateModelItemList(
                QStringList() << AssistantDefine::KEYWORD_TITLE_MODELNAME << AssistantDefine::KEYWORD_TITLE_BOARDNAME
                              << AssistantDefine::KEYWORD_TITLE_INPUTVAR << AssistantDefine::KEYWORD_TITLE_OUTPUTVAR
                              << AssistantDefine::KEYWORD_TITLE_STATE << AssistantDefine::KEYWORD_TITLE_PARAM
                              << AssistantDefine::KEYWORD_TITLE_EDITTIME,
                QStringList() << AssistantDefine::KEYWORD_TITLE_MODELNAME);
    } else if (TableBaseModel::ModelDataType_BuildInBlocks == m_ModelDataType) {
        // 内建模块固定列：模块名，输入变量，输出变量，状态变量，参数
        updateModelItemList(
                QStringList() << AssistantDefine::KEYWORD_TITLE_MODELNAME << AssistantDefine::KEYWORD_TITLE_INPUTVAR
                              << AssistantDefine::KEYWORD_TITLE_OUTPUTVAR << AssistantDefine::KEYWORD_TITLE_STATE
                              << AssistantDefine::KEYWORD_TITLE_PARAM,
                QStringList() << AssistantDefine::KEYWORD_TITLE_MODELNAME);
    } else if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType) {
        // 模块实例：参数名，参数类型,参数值，参数信息。
        updateModelItemList(QStringList()
                            << AssistantDefine::KEYWORD_TITLE_VARIABLENAME << AssistantDefine::KEYWORD_TITLE_PARAM_NAME
                            << AssistantDefine::KEYWORD_TITLE_TYPE << AssistantDefine::KEYWORD_TITLE_VALUE
                            << AssistantDefine::KEYWORD_TITLE_DESCRIPTION);
    } else if (TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
        // 构造型实例列:路径，参数名，参数类型,参数值，参数信息。
        updateModelItemList(QStringList()
                            << AssistantDefine::KEYWORD_TITLE_PATH << AssistantDefine::KEYWORD_TITLE_VARIABLENAME
                            << AssistantDefine::KEYWORD_TITLE_PARAM_NAME << AssistantDefine::KEYWORD_TITLE_TYPE
                            << AssistantDefine::KEYWORD_TITLE_VALUE << AssistantDefine::KEYWORD_TITLE_DESCRIPTION);
    } else if (TableBaseModel::ModelDataType_CombineBoardBlocks == m_ModelDataType
               || TableBaseModel::ModelDataType_ControlBoardBlocks == m_ModelDataType) {
        // 构造型模板列：元件名，输入变量，输出变量，状态变量，参数，编辑时间。
        // 控制画板列：元件名，输出变量，输出变量，状态变量，参数，编辑时间。
        updateModelItemList(
                QStringList() << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME << AssistantDefine::KEYWORD_TITLE_INPUTVAR
                              << AssistantDefine::KEYWORD_TITLE_OUTPUTVAR << AssistantDefine::KEYWORD_TITLE_STATE
                              << AssistantDefine::KEYWORD_TITLE_PARAM << AssistantDefine::KEYWORD_TITLE_EDITTIME,
                QStringList() << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME);
    } else if (TableBaseModel::ModelDataType_ComplexBoardBlocks == m_ModelDataType) {
        // 复合型模板列：插槽，模块原型，输入变量，输出变量，编辑时间。
        updateModelItemList(
                QStringList() << AssistantDefine::KEYWORD_TITLE_SLOT << AssistantDefine::KEYWORD_TITLE_MODELPROTOTYPE
                              << AssistantDefine::KEYWORD_TITLE_INPUTVAR << AssistantDefine::KEYWORD_TITLE_OUTPUTVAR
                              << AssistantDefine::KEYWORD_TITLE_EDITTIME,
                QStringList() << AssistantDefine::KEYWORD_TITLE_SLOT);
    } else if (TableBaseModel::ModelDataType_ComplexBoardInstances == m_ModelDataType) {
        // 复合模型实例：插槽，元件名，输入变量，输出变量，状态变量，参数，编辑时间。
        updateModelItemList(
                QStringList() << AssistantDefine::KEYWORD_TITLE_SLOT << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME
                              << AssistantDefine::KEYWORD_TITLE_INPUTVAR << AssistantDefine::KEYWORD_TITLE_OUTPUTVAR
                              << AssistantDefine::KEYWORD_TITLE_STATE << AssistantDefine::KEYWORD_TITLE_PARAM
                              << AssistantDefine::KEYWORD_TITLE_EDITTIME,
                QStringList() << AssistantDefine::KEYWORD_TITLE_SLOT << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME);
    } else if (TableBaseModel::ModelDataType_GlobalDeviceTypes == m_ModelDataType
               || TableBaseModel::ModelDataType_ProjectDeviceTypes == m_ModelDataType) {
        // 设备类型列：设备类型名称，设备类型所属，设备类型参数详情，编辑时间。
        for (PModel pmod : models) {
            if (pmod == nullptr) {
                continue;
            }
            PDeviceModel pdevmodel = pmod.dynamicCast<DeviceModel>();
            if (pdevmodel == nullptr) {
                continue;
            }
            updateModelItemList(QStringList() << AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME
                                              << AssistantDefine::KEYWORD_TITLE_DEVICETYPE_CLASS,
                                QStringList() << AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME);
            // 设备类型参数
            updateModelItemList(pdevmodel, RoleDataDefinition::DeviceTypeParameter,
                                TableBaseModel::VariableClass_ElecDeviceType, pdevmodel->getDeviceTypePrototypeName());
            updateModelItemList(QStringList() << AssistantDefine::KEYWORD_TITLE_EDITTIME);
            break;
        }
    } else if (TableBaseModel::ModelDataType_ElectricalBlocks == m_ModelDataType) {
        updateElecElements(models, prototypeName);
    } else if (TableBaseModel::ModelDataType_ControlInstances == m_ModelDataType) {
        updateModelItemList(QStringList() << AssistantDefine::KEYWORD_TITLE_MODELNAME
                                          << AssistantDefine::KEYWORD_TITLE_BOARDNAME,
                            QStringList() << AssistantDefine::KEYWORD_TITLE_MODELNAME);
        updateTitleInfo(models);
        updateModelItemList(QStringList() << AssistantDefine::KEYWORD_TITLE_EDITTIME);
    }
    endResetModel();
}

void TableModelDataModel::updateElecElements(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &listdata,
                                             const QString &prototypeName)
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr || listdata.size() <= 0) {
        return;
    }

    PElectricalBlock pbusbarblock = ProjectViewServers::getInstance()
                                            .m_projectManagerServer->CreateBlockModel(AssistantDefine::PROTOTYPE_BUSBAR)
                                            .dynamicCast<ElectricalBlock>();
    QStringList titleList;
    if (ProjectViewServers::getInstance().m_projectManagerServer->HaveDeviceModel(prototypeName)) {
        titleList << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME << AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME
                  << AssistantDefine::KEYWORD_TITLE_BOARDNAME;
    } else {
        titleList << AssistantDefine::KEYWORD_TITLE_ELEMENTNAME << AssistantDefine::KEYWORD_TITLE_BOARDNAME;
    }
    updateModelItemList(titleList, titleList);
    for (PModel pmod : listdata) {
        if (pmod == nullptr) {
            continue;
        }
        PElectricalBlock pemodel = ProjectViewServers::getInstance()
                                           .m_projectManagerServer->CreateBlockModel(pmod->getPrototypeName())
                                           .dynamicCast<ElectricalBlock>();
        PDeviceModel pdevmodel = ProjectViewServers::getInstance().m_projectManagerServer->GetSystemDeviceModel(
                pmod->getPrototypeName());
        if (pemodel == nullptr) {
            continue;
        }
        // busbar端口
        if (AssistantDefine::PROTOTYPE_BUSBAR != pemodel->getPrototypeName()
            && AssistantDefine::PROTOTYPE_DOTBUSBAR != pemodel->getPrototypeName()) {
            QList<PVariable> portlist = getVarGroupList(pemodel, RoleDataDefinition::PortGroup);
            for (PVariable portVar : portlist) {
                if (pbusbarblock == nullptr) {
                    break;
                }
                if (portVar == nullptr) {
                    continue;
                }
                updateModelItemList(pbusbarblock, RoleDataDefinition::ElectricalParameter,
                                    TableBaseModel::VariableClass_ElecBusbarParam, "", portVar->getName(),
                                    portVar->getDisplayName().isEmpty() ? portVar->getName()
                                                                        : portVar->getDisplayName());
            }
            addElecConBusbarItem(listdata);
        }

        // 电气参数
        updateModelItemList(pemodel, RoleDataDefinition::ElectricalParameter, TableBaseModel::VariableClass_ElecParam,
                            pemodel->getPrototypeName());
        // 设备类型
        if (pdevmodel != nullptr) {
            updateModelItemList(pdevmodel, RoleDataDefinition::DeviceTypeParameter,
                                TableBaseModel::VariableClass_ElecDeviceType, pemodel->getPrototypeName());
        }

        // 仿真参数
        updateModelItemList(pemodel, RoleDataDefinition::SimulationParameter,
                            TableBaseModel::VariableClass_ElecSimuParam, pemodel->getPrototypeName());
        // 初始值参数
        updateModelItemList(pemodel, RoleDataDefinition::InitializationParameter,
                            TableBaseModel::VariableClass_ElecInitParam, pemodel->getPrototypeName());
        // 潮流计算结果
        updateModelItemList(pemodel, RoleDataDefinition::LoadFlowResultParameter,
                            TableBaseModel::VariableClass_ElecPFResult, pemodel->getPrototypeName());

        break;
    }
    updateModelItemList(QStringList() << AssistantDefine::KEYWORD_TITLE_EDITTIME);
}

void TableModelDataModel::addElecConBusbarItem(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &listdata)
{
    if (listdata.size() <= 0) {
        return;
    }

    for (PModel pmod : listdata) {
        if (pmod == nullptr
            || AssistantDefine::PROTOTYPE_BUSBAR == pmod->getPrototypeName()
                    && AssistantDefine::PROTOTYPE_DOTBUSBAR == pmod->getPrototypeName()) {
            continue;
        }
        PElectricalBlock peblock = pmod.dynamicCast<ElectricalBlock>();
        if (peblock == nullptr) {
            continue;
        }
        QList<PVariable> portList = getVarGroupList(peblock, RoleDataDefinition::PortGroup);
        if (portList.size() <= 0) {
            continue;
        }
        PModel pParentModel = peblock->getParentModel();
        if (pParentModel.isNull()) {
            continue;
        }
        QList<PBlockConnector> adjConnectors = pParentModel->getAdjacentConnector(peblock->getUUID());
        QMap<QString, PElectricalBlock> busbarblockmap; // 以端口name为key，另一侧block为value
        for (PVariable portVar : portList) {
            if (portVar == nullptr) {
                continue;
            }
            bool findconnect = false;
            for (PBlockConnector pcon : adjConnectors) {
                if (pcon->getDstModel() != nullptr && (pcon->getDstModel()->getUUID() == peblock->getUUID())
                    && portVar->getUUID() == pcon->getDstPortUuid()) {
                    // dstblock就是连着当前端口，另一端就是连接的模块
                    if (pcon->getSrcModel() != nullptr) {
                        busbarblockmap[portVar->getName()] = (pcon->getSrcModel()).dynamicCast<ElectricalBlock>();
                    } else {
                        busbarblockmap[portVar->getName()] = QSharedPointer<ElectricalBlock>();
                    }
                    findconnect = true;
                    break;
                } else if (pcon->getSrcModel() != nullptr && (pcon->getSrcModel()->getUUID() == peblock->getUUID())
                           && portVar->getUUID() == pcon->getSrcPortUuid()) {
                    // srcBlock就是连着当前端口，另一端就是连接的模块
                    if (pcon->getDstModel() != nullptr) {
                        busbarblockmap[portVar->getName()] = (pcon->getDstModel()).dynamicCast<ElectricalBlock>();
                    } else {
                        busbarblockmap[portVar->getName()] = QSharedPointer<ElectricalBlock>();
                    }
                    findconnect = true;
                    break;
                }
            }
            if (!findconnect) {
                busbarblockmap[portVar->getName()] = QSharedPointer<ElectricalBlock>();
            }
        }
        m_conElecBusBarMap[peblock->getUUID() + peblock->getParentModelUUID()] = busbarblockmap;
    }
}

void TableModelDataModel::updateTitleInfo(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> models)
{
    for (PModel model : models) {
        if (model == nullptr || hasAddProtoType(model->getPrototypeName())) {
            continue;
        }
        m_ModelItemList.append(getTableBaseModelItemList(model, model->getPrototypeName(),
                                                         RoleDataDefinition::OutputSignal, true,
                                                         QStringList() << ITEMKEY::EXCLUDE_NOTNUMBER));
        m_ModelItemList.append(getTableBaseModelItemList(model, model->getPrototypeName(),
                                                         RoleDataDefinition::ContinueStateVariable, true,
                                                         QStringList() << ITEMKEY::EXCLUDE_NOTNUMBER));
        m_ModelItemList.append(getTableBaseModelItemList(model, model->getPrototypeName(),
                                                         RoleDataDefinition::DiscreteStateVariable, true,
                                                         QStringList() << ITEMKEY::EXCLUDE_NOTNUMBER));
        m_ModelItemList.append(getTableBaseModelItemList(model, model->getPrototypeName(),
                                                         RoleDataDefinition::Parameter, true,
                                                         QStringList() << ITEMKEY::EXCLUDE_NOTNUMBER));
    }
}

bool TableModelDataModel::hasAddProtoType(const QString &prototypename)
{
    for (TableBaseModelItem item : m_ModelItemList) {
        if (prototypename == item.prototypename) {
            return true;
        }
    }
    return false;
}

void TableModelDataModel::getDrawBoardReadOnlyData(QList<QSharedPointer<Kcc::BlockDefinition::Model>> models,
                                                   QList<QStringList> &listdata, QStringList &titlelist)
{
    listdata.clear();
    titlelist.clear();
    QString boardtype = "";
    titlelist << AssistantDefine::KEYWORD_TITLE_BOARDNAME << AssistantDefine::KEYWORD_TITLE_BOARDTYPE
              << AssistantDefine::KEYWORD_TITLE_BOARDDESCRIPTION;
    for (PModel pmod : models) {
        if (pmod == nullptr) {
            continue;
        }
        boardtype = "";
        if (CombineBoardModel::Type == pmod->getModelType()) {
            boardtype = AssistantDefine::STR_BOARDTYPE_COMBINEMODEL;
        } else if (ComplexBoardModel::Type == pmod->getModelType()) {
            boardtype = AssistantDefine::STR_BOARDTYPE_COMPLEXMODEL;
        } else if (ElecBoardModel::Type == pmod->getModelType()) {
            boardtype = AssistantDefine::STR_BOARDTYPE_ELECTRICALSYSTEM;
        } else if (ElecCombineBoardModel::Type == pmod->getModelType()) {
            boardtype = AssistantDefine::STR_BOARDTYPE_ELECCOMBINEMODEL;
        } else if (ControlBoardModel::Type == pmod->getModelType()) {
            boardtype = AssistantDefine::STR_BOARDTYPE_CONTROLSYSTEM;
        }
        listdata.append(QStringList() << pmod->getName() << boardtype << pmod->getDescription());
    }
}

QVariant TableModelDataModel::getSlotInstanceGroupStringList(QSharedPointer<Kcc::BlockDefinition::SlotBlock> slot,
                                                             const QString &groupName) const
{
    if (slot == nullptr || groupName.isEmpty() || !slot->isInstance()
        || ProjectViewServers::getInstance().m_pElecSysServer == nullptr) {
        return QVariant();
    }
    PModel targetModel = slot->getTargetModel();
    if (targetModel == nullptr) {
        return QVariant();
    }
    if (ElectricalBlock::Type == targetModel->getModelType()) {
        if (RoleDataDefinition::InputSignal == groupName) {
            return ProjectViewServers::getInstance()
                    .m_pElecSysServer->GetInputParamMap(targetModel->getPrototypeName())
                    .keys()
                    .join(",");
        } else if (RoleDataDefinition::OutputSignal == groupName) {
            return ProjectViewServers::getInstance()
                    .m_pElecSysServer
                    ->GetOutputParamMap(targetModel->getPrototypeName(),
                                        targetModel
                                                ->getValueByRoleData(RoleDataDefinition::ElectricalGroup,
                                                                     RoleDataDefinition::VariableTypeRole,
                                                                     RoleDataDefinition::PhaseNumber)
                                                .toUInt())
                    .keys()
                    .join(",");
        }
        return QVariant();
    }

    return getVarGroupStringList(targetModel, groupName).join(",");
}

void TableModelDataModel::updateModelItemList(const QStringList &titlelist, const QStringList &notUseColorList)
{
    QVariantMap otherPropertyMap;
    for (auto &titlestr : titlelist) {
        if (!titlestr.isEmpty()) {
            otherPropertyMap.clear();
            if (notUseColorList.contains(titlestr)) {
                otherPropertyMap.insert(ITEMKEY::NOT_USECOLOR, true);
            }
            if (AssistantDefine::KEYWORD_TITLE_EDITTIME == titlestr) {
                m_ModelItemList.append(TableBaseModelItem(titlestr, titlestr, RoleDataDefinition::ControlTypeDate,
                                                          NPS::DateFormate, TableBaseModel::VariableClass_Other, "",
                                                          otherPropertyMap));
            } else {
                m_ModelItemList.append(TableBaseModelItem(titlestr, titlestr, RoleDataDefinition::ControlTypeTextbox,
                                                          "", TableBaseModel::VariableClass_Other, "",
                                                          otherPropertyMap));
            }
        }
    }
}

void TableModelDataModel::updateModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                              const QString &groupName, TableBaseModel::VariableClass groupvarclass,
                                              const QString &prototypename, const QString keyAddstr,
                                              const QString titleAddstr)
{
    QList<PVariable> varlist;
    // 是加载模块的数据，只要去除name和电气参数的端口。
    if (keyAddstr.isEmpty()) {
        varlist = getVarGroupList(model, groupName, false,
                                  RoleDataDefinition::ElectricalParameter == groupName
                                          ? QStringList() << NPS::KEYWORD_ELECTRICALPARAM_NAME << ITEMKEY::EXCLUDE_PORT
                                          : QStringList());
    } else {
        // 作为连接线的母线数据
        varlist = getVarGroupList(model, groupName, false,
                                  RoleDataDefinition::ElectricalParameter == groupName
                                          ? QStringList() << ITEMKEY::EXCLUDE_PORT << NPS::KEYWORDS_BUSBAR_BUS
                                          : QStringList());
    }
    QVariantMap otherpropertymap;
    for (PVariable pvar : varlist) {
        if (pvar == nullptr) {
            continue;
        }
        otherpropertymap.clear();
        if (TableBaseModel::VariableClass_ElecBusbarParam == groupvarclass) {
            m_ModelItemList.append(
                    TableBaseModelItem(keyAddstr + "|" + pvar->getName(), titleAddstr + ":" + pvar->getDisplayName(),
                                       pvar->getControlType(), pvar->getControlValue(), groupvarclass, prototypename));
        } else {
            if (DeviceModel::Type == model->getModelType()
                && NPS::PROTOTYPENAME_VSC
                        == model->getValueByRoleData(RoleDataDefinition::DeviceTypeGroup,
                                                     RoleDataDefinition::VariableTypeRole,
                                                     RoleDataDefinition::DeviceTypePrototypeName)
                                   .toString()
                && NPS::KEYWORD_VSC_VDCREF == pvar->getName()) {
                otherpropertymap.insert(ITEMKEY::DISPLAY_ADD_SYMBOL, "±");
            }
            bool isBreakerList = false;
            if (ElectricalBlock::Type == model->getModelType()
                && NPS::PROTOTYPENAME_CIRCUITBREAKER == model->getPrototypeName()
                && (NPS::KEYWORDS_CIRCUITBREAKER_STATUS0 == pvar->getName()
                    || NPS::KEYWORDS_CIRCUITBREAKER_STATUS1 == pvar->getName())) {
                isBreakerList = true;
            }
            m_ModelItemList.append(
                    TableBaseModelItem(pvar->getName(), pvar->getDisplayName(),
                                       isBreakerList ? NPS::DataType_BreakerTimeNodeList : pvar->getControlType(),
                                       pvar->getControlValue(), groupvarclass, prototypename, otherpropertymap));
        }
    }
}

QVariant TableModelDataModel::getParamValue(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                            const QString &groupName, const QString &keyword,
                                            TableBaseModel::ModelData modeldata) const
{
    if (pmodel == nullptr || groupName.isEmpty() || keyword.isEmpty()) {
        return QVariant();
    }

    PVariable pvar = pmodel->getVariableByRoleData(groupName, RoleDataDefinition::NameRole, keyword);
    if (pvar == nullptr) {
        return QVariant();
    }
    if (TableBaseModel::ModelData_Value == modeldata) {
        return pmodel->getVariableData(pvar, RoleDataDefinition::ValueRole);
    } else if (TableBaseModel::ModelData_Edit == modeldata) {
        if (ElectricalBlock::Type == pmodel->getModelType()) {
            bool boardActive = m_boardActiveStsMap.value(pmodel->getParentModelUUID(), false).toBool();
            if (!boardActive) {
                return false;
            } else if (groupName == RoleDataDefinition::ElectricalParameter
                       && (NPS::PROTOTYPENAME_SHUNTFILTER == pmodel->getPrototypeName()
                           || NPS::PROTOTYPENAME_SERIESREACTOR == pmodel->getPrototypeName())) {
                return isEditable(pmodel, pvar);
            } else {
                return pvar->getShowMode() == Variable::VisiableWritable ? true : false;
            }
        } else if (DeviceModel::Type == pmodel->getModelType()) {
            return isDeviceParamEditable(pmodel, pvar);
        } else {
            return true;
        }
    }
    return QVariant();
}

QVariant TableModelDataModel::getControlInstanceValue(const QModelIndex index,
                                                      TableBaseModel::ModelData modeldata) const
{
    if (TableBaseModel::ModelData_Edit == modeldata) {
        if (m_ModelItemList[index.column()].prototypename == m_modelData[index.row()]->getPrototypeName()) {
            return true;
        }
        return false;
    } else if (TableBaseModel::ModelData_Value == modeldata) {
        if (m_ModelItemList[index.column()].prototypename != m_modelData[index.row()]->getPrototypeName()) {
            return QVariant();
        }
        PModel realModel = findRealModel(m_modelData[index.row()], m_ModelItemList[index.column()].Keywords);
        if (realModel == nullptr) {
            return QVariant();
        }
        return realModel->getValueByRoleData(VariableClassMapToGroupName(m_ModelItemList[index.column()].varClass),
                                             RoleDataDefinition::NameRole,
                                             m_ModelItemList[index.column()].Keywords.section('/', -1));
    }
    return QVariant();
}

OldNewDataStruct TableModelDataModel::getOldNewDataStruct(const int &row, const QVariant &newvalue,
                                                          const QString &valuedatatype,
                                                          const QList<QModelIndex> &indexList)
{
    OldNewDataStruct oldnewStruct;
    PModel curModel = getModelByIndex(this->index(row, 0));
    if (curModel == nullptr) {
        return OldNewDataStruct();
    }

    oldnewStruct.modelUUID = curModel->getUUID();
    oldnewStruct.modelType = curModel->getModelType();
    bool haveChanged = false;
    QString errorinfo = "";
    QString name = "";
    QString datatypestr = "";
    QVariant oldvalue = QVariant();
    for (QModelIndex index : indexList) {
        errorinfo = "";
        name = headerData(index.column(), Qt::Horizontal, Qt::UserRole).toString();
        datatypestr = index.data(NPS::ModelDataTypeRole).toString();
        oldvalue = index.data(NPS::ModelDataRealValueRole);

        if (!index.data(NPS::ModelDataEditRole).toBool()) {
            LOGOUT(QObject::tr("%1[%2]parameter[%3]can't modifiable!")
                           .arg(getTableName())
                           .arg(curModel->getName())
                           .arg(name),
                   LOG_WARNING);
            continue;
        }
        if (valuedatatype != datatypestr) {
            LOGOUT(QObject::tr("%1[%2]parameter[%3]does not match the parameter type to be modified,can't modifiable!")
                           .arg(getTableName())
                           .arg(curModel->getName())
                           .arg(name),
                   LOG_WARNING);
            continue;
        }
        if (!NPS::checkValueAvailable(name, datatypestr, index.data(NPS::ModelDataRangeRole), newvalue, errorinfo)) {
            LOGOUT(QObject::tr("%1[%2]parameter[%3]input value error:%4")
                           .arg(getTableName())
                           .arg(curModel->getName())
                           .arg(name)
                           .arg(errorinfo),
                   LOG_WARNING);
            continue;
        }
        if (!checkElecVarModifyValid(curModel, newvalue, index, indexList, errorinfo)) {
            LOGOUT(errorinfo, LOG_WARNING);
            continue;
        }
        if (oldvalue != newvalue) {
            oldnewStruct.addChangedItem(index.data(TableBaseModel::CustomRole_VariableClass).toInt(),
                                        index.data(NPS::ModelDataKeywordsRole).toString(), oldvalue, newvalue);
            addRelativeChangeItem(oldnewStruct, curModel, index);
            haveChanged = true;
        }
    }
    if (haveChanged) {
        return oldnewStruct;
    }
    return OldNewDataStruct();
}

void TableModelDataModel::addRelativeChangeItem(OldNewDataStruct &oldnewStruct,
                                                QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                const QModelIndex &index)
{
    if (model == nullptr || !index.isValid() || !oldnewStruct.isValid() || DeviceModel::Type != model->getModelType()
        || NPS::PROTOTYPENAME_LOAD
                != model->getValueByRoleData(RoleDataDefinition::DeviceTypeGroup, RoleDataDefinition::VariableTypeRole,
                                             RoleDataDefinition::DeviceTypePrototypeName)
                           .toString()
        || NPS::KEYWORDS_LOAD_TYPE != index.data(NPS::ModelDataKeywordsRole).toString()) {
        return;
    }
    int loadTypeValue = oldnewStruct
                                .getItemValue(index.data(TableBaseModel::CustomRole_VariableClass).toInt(),
                                              index.data(NPS::ModelDataKeywordsRole).toString())
                                .toInt();
    QMap<QString, bool> editMap;
    switch (loadTypeValue) {
    case NPS::LOADTYPE_RLC: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_RL: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    case NPS::LOADTYPE_RC: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_R: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    case NPS::LOADTYPE_LC: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_L: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    case NPS::LOADTYPE_C: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_OPEN: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    default:
        break;
    }
    QVariant oldR;
    QVariant oldL;
    QVariant oldC;
    for (QString keyword : editMap.keys()) {
        if (!editMap[keyword]) {
            QModelIndex sibindex = findKeywordIndex(index, keyword);
            oldnewStruct.addChangedItem(index.data(TableBaseModel::CustomRole_VariableClass).toInt(), keyword,
                                        sibindex.isValid() ? sibindex.data(NPS::ModelDataRealValueRole) : 0, 0);
        }
    }
}

bool TableModelDataModel::checkElecVarModifyValid(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                  const QVariant &newvalue, const QModelIndex targetIndex,
                                                  const QList<QModelIndex> &indexList, QString &errorInfo)
{
    errorInfo = "";
    if (model == nullptr) {
        errorInfo = tr("model is null,can't change value");
    }

    if (NPS::PROTOTYPENAME_SYNCMACHINE != model->getPrototypeName()
        || TableBaseModel::VariableClass_ElecSimuParam
                != targetIndex.data(TableBaseModel::CustomRole_VariableClass).toInt()) {
        return true;
    }
    QString keywordsstr = targetIndex.data(NPS::ModelDataKeywordsRole).toString();
    if (NPS::KEYWORD_SYNCMACHINE_TE2F != keywordsstr && NPS::KEYWORD_SYNCMACHINE_TF2M != keywordsstr
        && NPS::KEYWORD_SYNCMACHINE_TM2M != keywordsstr) {
        return true;
    }
    // T_e2f<=T_f2m<=T_m2m
    PVariable te2fvar = model->getVariableByRoleData(RoleDataDefinition::SimulationParameter,
                                                     RoleDataDefinition::NameRole, NPS::KEYWORD_SYNCMACHINE_TE2F);
    PVariable tf2mvar = model->getVariableByRoleData(RoleDataDefinition::SimulationParameter,
                                                     RoleDataDefinition::NameRole, NPS::KEYWORD_SYNCMACHINE_TF2M);
    PVariable tm2mvar = model->getVariableByRoleData(RoleDataDefinition::SimulationParameter,
                                                     RoleDataDefinition::NameRole, NPS::KEYWORD_SYNCMACHINE_TM2M);
    if (tm2mvar == nullptr || tf2mvar == nullptr || tm2mvar == nullptr) {
        errorInfo = tr("model data is null,can't change value");
        return false;
    }
    QVariant Te2fValue = model->getVariableData(te2fvar, RoleDataDefinition::ValueRole);
    QVariant Tf2mValue = model->getVariableData(tf2mvar, RoleDataDefinition::ValueRole);
    QVariant Tm2mValue = model->getVariableData(tm2mvar, RoleDataDefinition::ValueRole);
    for (QModelIndex index : indexList) {
        if (NPS::KEYWORD_SYNCMACHINE_TE2F == index.data(NPS::ModelDataKeywordsRole).toString()) {
            Te2fValue = newvalue;
        } else if (NPS::KEYWORD_SYNCMACHINE_TF2M == index.data(NPS::ModelDataKeywordsRole).toString()) {
            Tf2mValue = newvalue;
        } else if (NPS::KEYWORD_SYNCMACHINE_TM2M == index.data(NPS::ModelDataKeywordsRole).toString()) {
            Tm2mValue = newvalue;
        }
    }
    if (Tf2mValue.toDouble() < Te2fValue.toDouble()) {
        // 仿真设置中参数[%1]的值需要大于等于[%2]的值，请重新设置！
        errorInfo = QObject::tr("%1[%2]The value of parameter [%3] in simulation settings needs to be greater than or "
                                "equal to the value of [%4], please reset!")
                            .arg(getTableName())
                            .arg(model->getName())
                            .arg(tf2mvar->getDisplayName())
                            .arg(te2fvar->getDisplayName());
        return false;
    }
    if (Tm2mValue.toDouble() < Tf2mValue.toDouble()) {
        // 仿真设置中参数[%1]的值需大于等于[%2]的值，请重新设置！
        errorInfo = QObject::tr("%1[%2]The value of parameter [%3] in simulation settings needs to be greater than or "
                                "equal to the value of [%4], please reset!")
                            .arg(getTableName())
                            .arg(model->getName())
                            .arg(tm2mvar->getDisplayName())
                            .arg(tf2mvar->getDisplayName());
        return false;
    }
    return true;
}

QSharedPointer<Kcc::BlockDefinition::Model> TableModelDataModel::getModelItemByDataStruct(const OldNewDataStruct &item,
                                                                                          int &rowindex)
{
    rowindex = -1;
    if (TableBaseModel::ModelDataType_CodeInstance == m_ModelDataType
        || TableBaseModel::ModelDataType_CombineBoardInstance == m_ModelDataType) {
        for (int i = 0; i < m_instanceData.size(); ++i) {
            if (m_instanceData[i].isValid() && item.modelUUID == m_instanceData[i].model->getUUID()) {
                rowindex = i;
                return m_instanceData[i].model;
            }
        }
        return PModel(nullptr);
    }
    for (int rowno = 0; rowno < m_modelData.size(); ++rowno) {
        if (m_modelData[rowno] != nullptr && m_modelData[rowno]->getUUID() == item.modelUUID) {
            rowindex = rowno;
            return m_modelData[rowno];
        }
    }
    return PModel(nullptr);
}

int TableModelDataModel::getInstanceVarRow(const QString &modelUUID, const QString &groupName, const QString &varName)
{
    if (TableBaseModel::ModelDataType_CodeInstance != m_ModelDataType
        && TableBaseModel::ModelDataType_CombineBoardInstance != m_ModelDataType) {
        return -1;
    }

    for (int i = 0; i < m_instanceData.size(); ++i) {
        if (m_instanceData[i].isValid() && modelUUID == m_instanceData[i].model->getUUID()
            && varName == m_instanceData[i].pVar->getName() && groupName == m_instanceData[i].groupName) {
            return i;
        }
    }
    return -1;
}

bool TableModelDataModel::saveProjectModelByUUID(const QString &modelUUID)
{
    if (modelUUID.isEmpty() || m_pProjDataManager == nullptr) {
        return false;
    }
    PKLProject pCurProject = m_pProjDataManager->getCurrentProject();
    if (pCurProject == nullptr) {
        return false;
    }
    PModel model = pCurProject->findModel(modelUUID);
    if (model == nullptr) {
        return false;
    }
    return pCurProject->saveModel(model);
}

QString TableModelDataModel::getInstanceKeywordPath(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                    const QString &keyword)
{
    if (model == nullptr) {
        return keyword;
    }
    QStringList pathList;
    PModel parentModel = model;
    while (parentModel != nullptr) {
        pathList.prepend(parentModel->getName());
        parentModel = parentModel->getParentModel();
    }
    if (pathList.size() >= 1) {
        pathList.pop_front();
    }
    if (pathList.size() > 0) {
        return pathList.join("/") + "/" + keyword;
    }
    return keyword;
}

bool TableModelDataModel::isEditable(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                     QSharedPointer<Kcc::BlockDefinition::Variable> var) const
{
    if (var == nullptr || model == nullptr) {
        return false;
    }
    int shuntValue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                               NPS::KEYWORDS_SHUNTFILTER_SHUNTTYPE)
                             .toInt();
    if (var->getName() == NPS::KEYWORDS_SHUNTFILTER_R) {
        return var->getShowMode() == Variable::VisiableWritable;
    } else if (var->getName() == NPS::KEYWORDS_SHUNTFILTER_L) {
        if (NPS::SHUNTTYPE_RC == shuntValue) {
            return false;
        } else {
            return true;
        }
    } else if (var->getName() == NPS::KEYWORDS_SHUNTFILTER_C) {
        if (NPS::SHUNTTYPE_RL == shuntValue) {
            return false;
        } else {
            return true;
        }
    } else if (var->getName() == NPS::KEYWORDS_SHUNTFILTER_RP) {
        if (NPS::SHUNTTYPE_RLC_R == shuntValue || NPS::SHUNTTYPE_RLCC_R == shuntValue) {
            return true;
        } else {
            return false;
        }
    } else if (var->getName() == NPS::KEYWORDS_SHUNTFILTER_CS) {
        if (NPS::SHUNTTYPE_RLCC_R == shuntValue) {
            return true;
        } else {
            return false;
        }
    }
    return var->getShowMode() == Variable::VisiableWritable;
}

bool TableModelDataModel::isDeviceParamEditable(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                QSharedPointer<Kcc::BlockDefinition::Variable> var) const
{
    if (model == nullptr || DeviceModel::Type != model->getModelType() || var == nullptr) {
        return false;
    }
    if (NPS::PROTOTYPENAME_LOAD
        == model->getValueByRoleData(RoleDataDefinition::DeviceTypeGroup, RoleDataDefinition::VariableTypeRole,
                                     RoleDataDefinition::DeviceTypePrototypeName)
                   .toString()) {
        int loadType = model->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter, RoleDataDefinition::NameRole,
                                                 NPS::KEYWORDS_LOAD_TYPE)
                               .toInt();
        if (var->getName() == NPS::KEYWORDS_LOAD_R) {
            if (NPS::LOADTYPE_RLC == loadType || NPS::LOADTYPE_RL == loadType || NPS::LOADTYPE_RC == loadType
                || NPS::LOADTYPE_R == loadType) {
                return true;
            } else {
                return false;
            }
        } else if (var->getName() == NPS::KEYWORDS_LOAD_L) {
            if (NPS::LOADTYPE_RLC == loadType || NPS::LOADTYPE_RL == loadType || NPS::LOADTYPE_LC == loadType
                || NPS::LOADTYPE_L == loadType) {
                return true;
            } else {
                return false;
            }
        } else if (var->getName() == NPS::KEYWORDS_LOAD_C) {
            if (NPS::LOADTYPE_RLC == loadType || NPS::LOADTYPE_RC == loadType || NPS::LOADTYPE_LC == loadType
                || NPS::LOADTYPE_C == loadType) {
                return true;
            } else {
                return false;
            }
        }
    } else if (NPS::PROTOTYPENAME_BATTERY
               == model->getValueByRoleData(RoleDataDefinition::DeviceTypeGroup, RoleDataDefinition::VariableTypeRole,
                                            RoleDataDefinition::DeviceTypePrototypeName)
                          .toString()) {
        int chargeDynamics =
                model->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter, RoleDataDefinition::NameRole,
                                          NPS::KEYWORDS_BATTERY_CHARGEDYNAMICS)
                        .toInt();
        if (var->getName() == NPS::KEYWORDS_BATTERY_R1 || var->getName() == NPS::KEYWORDS_BATTERY_T1) {
            if (chargeDynamics >= NPS::CHARGEDYNAMICS_1) {
                return true;
            } else {
                return false;
            }
        } else if (var->getName() == NPS::KEYWORDS_BATTERY_R2 || var->getName() == NPS::KEYWORDS_BATTERY_T2) {
            if (chargeDynamics >= NPS::CHARGEDYNAMICS_2) {
                return true;
            } else {
                return false;
            }
        } else if (var->getName() == NPS::KEYWORDS_BATTERY_R3 || var->getName() == NPS::KEYWORDS_BATTERY_T3) {
            if (chargeDynamics >= NPS::CHARGEDYNAMICS_3) {
                return true;
            } else {
                return false;
            }
        } else if (var->getName() == NPS::KEYWORDS_BATTERY_R4 || var->getName() == NPS::KEYWORDS_BATTERY_T4) {
            if (chargeDynamics >= NPS::CHARGEDYNAMICS_4) {
                return true;
            } else {
                return false;
            }
        } else if (var->getName() == NPS::KEYWORDS_BATTERY_R5 || var->getName() == NPS::KEYWORDS_BATTERY_T5) {
            if (chargeDynamics >= NPS::CHARGEDYNAMICS_5) {
                return true;
            } else {
                return false;
            }
        }
    }

    return var->getShowMode() == Variable::VisiableWritable;
}

QSharedPointer<Kcc::BlockDefinition::Model>
TableModelDataModel::findRealModel(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &keyword) const
{
    if (model == nullptr || keyword.isEmpty()) {
        return model;
    }
    if (keyword.contains("/")) {
        QString childName = keyword.section("/", 0, 0);
        for (PModel cmodel : model->getChildModels().values()) {
            if (cmodel->getName() == childName) {
                return findRealModel(cmodel, keyword.section("/", 1));
            }
        }
    }
    return model;
}

QString TableModelDataModel::valueMaptoAlgorithm(const QVariant &value)
{
    QStringList algNamelist = QStringList()
            << Kcc::SimulationManager::ODE_SOLVER_DISCRETE << Kcc::SimulationManager::ODE_SOLVER_1
            << Kcc::SimulationManager::ODE_SOLVER_2 << Kcc::SimulationManager::ODE_SOLVER_3
            << Kcc::SimulationManager::ODE_SOLVER_4 << Kcc::SimulationManager::ODE_SOLVER_5
            << Kcc::SimulationManager::ODE_SOLVER_8 << Kcc::SimulationManager::ODE_SOLVER_14X
            << Kcc::SimulationManager::ODE_SOLVER_1BE;
    if (value.toInt() < algNamelist.size() && value.toInt() >= 0) {
        return algNamelist[value.toInt()];
    }
    return QString();
}

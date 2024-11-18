#include "CommonModelAssistant.h"

#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"

USE_LOGOUT_("CommonModelAssistant")
using namespace Kcc::BlockDefinition;

namespace CMA {

QList<QSharedPointer<Kcc::BlockDefinition::Variable>>
getVarGroupList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                const QStringList &excludekeys)
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return QList<PVariable>();
    }
    PVariableGroup variablegroup = pmodel->getVariableGroup(groupName);
    if (variablegroup == nullptr) {
        return QList<PVariable>();
    }
    QList<PVariable> sortlist = variablegroup->getVariableMap().values();
    QMutableListIterator<PVariable> iter(sortlist);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() != nullptr
                    && (NPS::KEYWORD_ELECTRICALPARAM_NAME == iter.value()->getName()
                        || excludekeys.contains(iter.value()->getName())
                        || (RoleDataDefinition::ElectricalParameter == groupName
                            && isPort(pmodel, iter.value()->getName())))
            || (excludekeys.contains(EXCLUED_NOTNUMBER) && !NUMBERLIST.contains(iter.value()->getDataType()))) {
            iter.remove();
            continue;
        }
        if ((ElectricalBlock::Type == pmodel->getModelType() || DeviceModel::Type == pmodel->getModelType())
            && RoleDataDefinition::PortGroup != groupName && RoleDataDefinition::ResultSaveVariables != groupName) {
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
    return sortlist;
}

QStringList getVarGroupCheckList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName)
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return QStringList();
    }
    QList<PVariable> sortList = getVarGroupList(pmodel, groupName);
    QStringList checkList;
    for (PVariable pvar : sortList) {
        if (pvar != nullptr && pvar->getData(RoleDataDefinition::VariableCheckedRole).toBool()) {
            checkList.append(pvar->getName());
        }
    }
    return checkList;
}

QList<CustomModelItem> getPropertyModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                const QStringList &groupNamelist, bool readOndy, const QString &path,
                                                const QStringList &excludekeys, bool recursive)
{
    if (pmodel == nullptr || groupNamelist.size() <= 0) {
        return QList<CustomModelItem>();
    }

    QList<CustomModelItem> listdata;
    for (QString groupName : groupNamelist) {
        listdata.append(getPropertyModelItemList(pmodel, groupName, readOndy, path, excludekeys, recursive));
    }

    return listdata;
}

QList<CustomModelItem> getPropertyModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                const QString &groupName, bool readOndy, const QString &path,
                                                const QStringList &excludekeys, bool recursive)
{

    if (pmodel == nullptr || groupName.isEmpty()) {
        return QList<CustomModelItem>();
    }
    QList<CustomModelItem> cmodellist;
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
            cmodellist.append(getPropertyModelItemList(cmodel, groupName, readOndy, tmppath, excludekeys));
        }
    }
    QList<CustomModelItem> modellist;
    QList<PVariable> sortlist = getVarGroupList(pmodel, groupName, excludekeys);
    QVariantMap otherpropertymap;
    for (int i = 0; i < sortlist.size(); ++i) {
        if (sortlist[i] == nullptr) {
            continue;
        }
        otherpropertymap.clear();
        otherpropertymap.insert(KEY_GROUPNAME, groupName);
        if (!path.isEmpty()) {
            otherpropertymap.insert(KEY_UUID, pmodel->getUUID());
            otherpropertymap.insert(KEY_INDEX, i);
        }
        if (ControlBlock::Type == pmodel->getModelType()) {
            otherpropertymap.insert(KEY_DISCRIPTION, sortlist[i]->getDescription());
        }

        if (pmodel->getVariableData(sortlist[i], RoleDataDefinition::VariableCheckedRole).toBool()) {
            otherpropertymap.insert(
                    KEY_VARIABLECHECKED,
                    pmodel->getVariableData(sortlist[i], RoleDataDefinition::VariableCheckedRole).toBool());
            otherpropertymap.insert(
                    KEY_VARIABLECHECKEDTIME,
                    pmodel->getVariableData(sortlist[i], RoleDataDefinition::VariableCheckedTimeRole).toDateTime());
        }
        if (DeviceModel::Type == pmodel->getModelType()
            && NPS::PROTOTYPENAME_VSC
                    == pmodel->getValueByRoleData(RoleDataDefinition::DeviceTypeGroup,
                                                  RoleDataDefinition::VariableTypeRole,
                                                  RoleDataDefinition::DeviceTypePrototypeName)
                               .toString()
            && NPS::KEYWORD_VSC_VDCREF == sortlist[i]->getName()) {
            otherpropertymap.insert(KEY_DISPLAYVALUEADD, "±");
        }

        if (ElectricalBlock::Type == pmodel->getModelType() || DeviceModel::Type == pmodel->getModelType()) {
            modellist.append(CustomModelItem(
                    sortlist[i]->getName(),
                    path.isEmpty() ? sortlist[i]->getDisplayName() : path + "/" + sortlist[i]->getDisplayName(),
                    pmodel->getVariableData(sortlist[i], RoleDataDefinition::ValueRole), sortlist[i]->getControlType(),
                    readOndy ? false : (sortlist[i]->getShowMode() == Variable::VisiableWritable ? true : false),
                    sortlist[i]->getControlValue(), otherpropertymap));
        } else if (SlotBlock::Type == pmodel->getModelType()) {
            modellist.append(CustomModelItem(
                    path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                    path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                    pmodel->getVariableData(sortlist[i], RoleDataDefinition::ValueRole), sortlist[i]->getControlType(),
                    readOndy ? false
                             : (pmodel->getVariableData(sortlist[i], RoleDataDefinition::VariableCheckedEnableRole)
                                        .toBool()),
                    sortlist[i]->getControlValue(), otherpropertymap));
        } else {
            modellist.append(CustomModelItem(
                    path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                    path.isEmpty() ? sortlist[i]->getName() : path + "/" + sortlist[i]->getName(),
                    pmodel->getVariableData(sortlist[i], RoleDataDefinition::ValueRole), sortlist[i]->getControlType(),
                    readOndy ? false : NPS::isVarCanEdit(pmodel, sortlist[i]->getName()),
                    sortlist[i]->getControlValue(), otherpropertymap));
        }
    }
    // userdefine block
    if (CombineBoardModel::Type == pmodel->getModelType()) {
        qSort(cmodellist.begin(), cmodellist.end(),
              [&](const CustomModelItem &lhs, const CustomModelItem &rhs) -> bool {
                  if (!lhs.keywords.contains("/") || !rhs.keywords.contains("/")) {
                      return lhs.keywords < rhs.keywords;
                  }
                  if (lhs.keywords.leftRef(lhs.keywords.lastIndexOf("/"))
                      == rhs.keywords.leftRef(rhs.keywords.lastIndexOf("/"))) {
                      return lhs.otherPropertyMap.value(KEY_INDEX).toInt()
                              < rhs.otherPropertyMap.value(KEY_INDEX).toInt();
                  }
                  return lhs.keywords < rhs.keywords;
              });
    }
    return modellist + cmodellist;
}

bool saveCustomModeListDataToModel(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                   const QList<CustomModelItem> &listdata, const QStringList &groupNamelist,
                                   SAVEDATA savedata)
{
    if (pmodel == nullptr || listdata.size() <= 0 || groupNamelist.size() <= 0) {
        return false;
    }
    bool change = false;
    for (QString groupName : groupNamelist) {
        if (saveCustomModeListDataToModel(pmodel, listdata, groupName, savedata)) {
            change = true;
        }
    }
    return change;
}

bool saveCustomModeListDataToModel(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                   const QList<CustomModelItem> &listdata, const QString &groupName, SAVEDATA savedata)
{
    if (pmodel == nullptr || groupName.isEmpty()) {
        return false;
    }
    bool bchange = false;
    QVariant oldvalue = QVariant();
    for (CustomModelItem item : listdata) {
        if (!item.isValid()) {
            continue;
        }
        oldvalue = QVariant();
        if (item.keywords.contains("/")) {
            PModel cmodel = pmodel->findChildModelRecursive(item.otherPropertyMap[KEY_UUID].toString());
            if (cmodel == nullptr) {
                continue;
            }
            PControlBlock childcblock = cmodel.dynamicCast<ControlBlock>();
            if (childcblock == nullptr) {
                continue;
            }
            if (saveRealBlockData(childcblock, groupName, item.keywords.section('/', -1), item, savedata)) {
                bchange = true;
            }
        } else {
            if (saveRealBlockData(pmodel, groupName, item.keywords, item, savedata)) {
                bchange = true;
            }
        }
    }
    return bchange;
}

bool saveRealBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                       const QString &keyword, const CustomModelItem &item, SAVEDATA savedata)
{
    if (pmodel == nullptr || keyword.isEmpty() || groupName.isEmpty()) {
        return false;
    }
    PVariable pavr = nullptr;
    bool bvaluechange = false;
    if (ElectricalBlock::Type == pmodel->getModelType() && RoleDataDefinition::ResultSaveVariables == groupName) {
        PVariableGroup pgroup = pmodel->getVariableGroup(groupName);
        if (pgroup == nullptr) {
            pgroup = pmodel->createVariableGroup(groupName);
        }
        pavr = pmodel->getVariableByRoleData(groupName, RoleDataDefinition::NameRole, keyword);
        if (pavr == nullptr) {
            pavr = pgroup->createVariable();
            pmodel->setVariableData(pavr, RoleDataDefinition::NameRole, keyword);
            pmodel->setVariableData(pavr, RoleDataDefinition::DataTypeRole, NPS::DataType_Bool);
            pmodel->setVariableData(pavr, RoleDataDefinition::VariableControlType,
                                    RoleDataDefinition::ControlTypeCheckbox);
            pmodel->setVariableData(pavr, RoleDataDefinition::ShowModeRole, Variable::VisiableWritable);
        }
    } else {
        pavr = pmodel->getVariableByRoleData(groupName, RoleDataDefinition::NameRole, keyword);
    }
    if (pavr == nullptr) {
        // LOGOUT(QObject::tr("not found: %1").arg(keyword), LOG_WARNING);
        return false;
    }
    if (SAVEDATA_All == savedata || SAVEDATA_Value == savedata) {
        if (item.value != pmodel->getVariableData(pavr, RoleDataDefinition::ValueRole)) {
            LOGOUT(NPS::outputLog(ElectricalBlock::Type == pmodel->getModelType() ? CMA::LogTag_ElectricalComponent
                                                                                  : CMA::LogTag_ControlComponent,
                                  pmodel->getName(), item.name,
                                  pmodel->getVariableData(pavr, RoleDataDefinition::ValueRole), item.value,
                                  pavr->getName(), pavr->getControlType(), pavr->getControlValue()));
            pmodel->setVariableData(pavr, RoleDataDefinition::ValueRole, item.value);
            bvaluechange = true;
        }
        if (item.otherPropertyMap.contains(KEY_DISCRIPTION)
            && item.otherPropertyMap[KEY_DISCRIPTION].toString()
                    != pmodel->getVariableData(pavr, RoleDataDefinition::DescriptionRole).toString()) {
            LOGOUT(NPS::outputLog(ElectricalBlock::Type == pmodel->getModelType() ? CMA::LogTag_ElectricalComponent
                                                                                  : CMA::LogTag_ControlComponent,
                                  pmodel->getName(),
                                  QObject::tr("%1:%2").arg(item.name).arg(QObject::tr("Description")),
                                  pmodel->getVariableData(pavr, RoleDataDefinition::DescriptionRole).toString(),
                                  item.otherPropertyMap[KEY_DISCRIPTION].toString(), pavr->getName(),
                                  pavr->getControlType(), pavr->getControlValue()));
            pmodel->setVariableData(pavr, RoleDataDefinition::DescriptionRole,
                                    item.otherPropertyMap[KEY_DISCRIPTION].toString());
            bvaluechange = true;
        }
    }
    if (SAVEDATA_All == savedata || SAVEDATA_CheckedVariable == savedata) {
        if (!item.otherPropertyMap.contains(KEY_VARIABLECHECKED)
            || item.otherPropertyMap[KEY_VARIABLECHECKED].toBool()
                    == pmodel->getVariableData(pavr, RoleDataDefinition::VariableCheckedRole).toBool()) {
            return bvaluechange;
        }
        if (ElectricalBlock::Type == pmodel->getModelType()) {
            if (RoleDataDefinition::ResultSaveVariables != groupName) {
                return bvaluechange;
            }
            if (item.otherPropertyMap[KEY_VARIABLECHECKED].toBool()) {
                LOGOUT(NPS::LOG_STRING(CMA::LogTag_ElectricalComponent, pmodel->getName(),
                                       QObject::tr("%1 Variable:%2").arg(BLOCK_TAB_NAME_ELEC_SAVE).arg(pavr->getName()),
                                       BLOCK_VARIABLE_UNCHECKED, BLOCK_VARIABLE_CHECKED));
                pmodel->setVariableData(pavr, RoleDataDefinition::VariableCheckedRole, true);
                pmodel->setVariableData(pavr, RoleDataDefinition::VariableCheckedTimeRole,
                                        QDateTime::currentDateTime());
                bvaluechange = true;
            } else {
                LOGOUT(NPS::LOG_STRING(CMA::LogTag_ElectricalComponent, pmodel->getName(),
                                       QObject::tr("%1 Variable:%2").arg(BLOCK_TAB_NAME_ELEC_SAVE).arg(pavr->getName()),
                                       BLOCK_VARIABLE_CHECKED, BLOCK_VARIABLE_UNCHECKED));
                PVariableGroup pgroup = pmodel->getVariableGroup(groupName);
                if (pgroup != nullptr) {
                    pgroup->removeVariable(pavr);
                    bvaluechange = true;
                }
            }
        } else {
            LOGOUT(NPS::LOG_STRING(CMA::LogTag_ControlComponent, pmodel->getName(),
                                   QObject::tr("%1 Variable:%2").arg(BLOCK_TAB_NAME_ELEC_SAVE).arg(item.name),
                                   pmodel->getVariableData(pavr, RoleDataDefinition::VariableCheckedRole).toBool()
                                           ? BLOCK_VARIABLE_CHECKED
                                           : BLOCK_VARIABLE_UNCHECKED,
                                   item.otherPropertyMap[KEY_VARIABLECHECKED].toBool() ? BLOCK_VARIABLE_CHECKED
                                                                                       : BLOCK_VARIABLE_UNCHECKED));
            pmodel->setVariableData(pavr, RoleDataDefinition::VariableCheckedRole,
                                    item.otherPropertyMap[KEY_VARIABLECHECKED].toBool());
            pmodel->setVariableData(pavr, RoleDataDefinition::VariableCheckedTimeRole, QDateTime::currentDateTime());
            bvaluechange = true;
        }
    }

    return bvaluechange;
}

bool isPort(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &keyword)
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
bool checkNameValid(const QString &name, QSharedPointer<Kcc::BlockDefinition::Model> pmodel, QString &errorinfo)
{
    errorinfo = "";
    if (pmodel == nullptr) {
        errorinfo = NPS::ERRORINFO_MODELNULL;
        return false;
    }
    if (name.isEmpty()) {
        if (DeviceModel::Type == pmodel->getModelType()) {
            errorinfo = NPS::ERRORINFO_DEVICETYPENAME_EMPTY;
        } else if (isBoardModel(pmodel)) {
            errorinfo = NPS::ERRORINFO_BOARDNAME_EMPTY;
        } else {
            errorinfo = NPS::ERRORINFO_BLOCKNAME_EMPTY;
        }
        return false;
    }
    if (name.length() > BLOCK_NAME_MAX_LENGHT) {
        errorinfo = NPS::ERRORINFO_NAME_EXCEEDLENGTH;
        return false;
    }
    if (pmodel->getName() == name) {
        return true;
    }
    if (pmodel->getParentModel() != nullptr) {
        for (PModel cmodel : pmodel->getParentModel()->getChildModels().values()) {
            if (cmodel != nullptr && cmodel->getName() == name) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
        return true;
    } else if (pmodel->isInstance()) {
        // 实例不能和同类型的重名
        QList<PModel> smodels = getProjectModels(pmodel->getModelType());
        for (PModel smodel : smodels) {
            if (smodel == nullptr) {
                continue;
            }
            if ((QString::compare(smodel->getName(), name, Qt::CaseInsensitive) == 0)
                && pmodel->getPrototypeName() == smodel->getPrototypeName()) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
            for (PModel instanceModel : smodel->getInstanceList()) {
                if (instanceModel != nullptr
                    && (QString::compare(instanceModel->getName(), name, Qt::CaseInsensitive) == 0)) {
                    errorinfo = NPS::ERRORINFO_REPEAT;
                    return false;
                }
            }
        }
        return true;
    } else if (isBoardModel(pmodel)) {
        if (!checkBoardNameValid(name, errorinfo)) {
            return false;
        }
        for (PModel instanceModel : pmodel->getInstanceList()) {
            if (instanceModel != nullptr && instanceModel->getName() == name) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
    } else {
        if (PropertyServerMng::getInstance().m_projectManagerServer != nullptr
            && PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(name)) {
            errorinfo = NPS::ERRORINFO_REPEAT;
            return false;
        }
        for (PModel instanceModel : pmodel->getInstanceList()) {
            if (instanceModel != nullptr && instanceModel->getName() == name) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
        return true;
    }
}

bool checkBoardNameValid(const QString &newName, QString &errorinfo)
{
    // 检查画板是否重名
    if (PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(newName, DuplicateCheck_ToolBox)) {
        errorinfo = QObject::tr("The name of [%1] is already exist in system models,please rename!").arg(newName);
        return false;
    } else if (PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(newName,
                                                                                     DuplicateCheck_CurProject)) {
        errorinfo = QObject::tr("The name of [%1] is already exist in project models,please rename!").arg(newName);
        return false;
    } else if (PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(newName,
                                                                                     DuplicateCheck_SystemBuildIn)) {
        errorinfo = QObject::tr("The name of [%1] is already exist in system models,please rename!").arg(newName);
        return false;
    }
    if (!newName.contains(QRegExp(NPS::REG_FILE_NAME))) {
        // 画板名称不合法，名称只能是英文、数字、中文、下划线、横杠，小括号，长度最多128个字符
        errorinfo = QObject::tr(
                "The name of the drawing board is illegal. the name can only be in english, numbers, chinese, "
                "underscores, horizontal bars, and parentheses, with a maximum length of 128 characters");
        return false;
    }
    // 校验新建画板名称是否为监控面板或Monitor
    if (!QString::compare(newName, "监控面板")) {
        errorinfo = QObject::tr("The drawing board named [%1] is not allowed.")
                            .arg("监控面板"); // 不允许新建名为“监控面板”的画板
        return false;
    }
    if (!QString::compare(newName, "Monitor", Qt::CaseInsensitive)) {
        errorinfo = QObject::tr("The drawing board named [%1] is not allowed.")
                            .arg("Monitor"); // 不允许新建名为“Monitor”的画板
        return false;
    }
    return true;
}

bool isBoardModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }
    if (ControlBoardModel::Type == model->getModelType() || ComplexBoardModel::Type == model->getModelType()
        || ElecBoardModel::Type == model->getModelType()
        || ((CombineBoardModel::Type == model->getModelType() || ElecCombineBoardModel::Type == model->getModelType())
            && model->getParentModel() == nullptr && !model->isInstance())) {
        return true;
    }
    return false;
}

QSharedPointer<Kcc::BlockDefinition::Model> getModelByPrototype(const QString &prototype, ProtoRange protorange)
{
    if (prototype.isEmpty() || PropertyServerMng::getInstance().m_ModelManagerServer == nullptr) {
        return PModel(nullptr);
    }
    if (ProtoRange_All == protorange || ProtoRange_OnlyBuildIn == protorange) {
        QString toolkitName;
        if (PropertyServerMng::getInstance().m_ModelManagerServer->ContainToolkitModel(prototype, toolkitName)) {
            return PropertyServerMng::getInstance().m_ModelManagerServer->GetToolkitModel(prototype, toolkitName);
        }
    }
    if (ProtoRange_All == protorange || ProtoRange_OnlyProject == protorange) {
        PKLProject pCurProject = getCurProject();
        if (pCurProject == nullptr) {
            return PModel(nullptr);
        }
        QList<PModel> projectModels = pCurProject->getAllModel();
        for (PModel model : projectModels) {
            if (model != nullptr && model->getPrototypeName() == prototype) {
                return model;
            }
        }
    }
    return PModel(nullptr);
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>> getProjectModels(const int &modelType)
{
    PKLProject pCurProject = getCurProject();
    if (pCurProject == nullptr) {
        return QList<PModel>();
    }
    if (modelType == -1) {
        return pCurProject->getAllModel();
    }
    return pCurProject->getAllModel(modelType);
}

bool destoryInstance(QSharedPointer<Kcc::BlockDefinition::Model> instance)
{
    PModel protoModel = getProjectProtoModel(instance);
    if (protoModel != nullptr) {
        protoModel->destroyInstance(instance->getUUID());
    }
    return false;
}

bool saveModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || PropertyServerMng::getInstance().m_pGraphicsModelingServer == nullptr) {
        return false;
    }
    QString targetUUID = model->getModelUuid();
    QString objectUUID = model->getObjectUuid();
    PModel parentModel = model->getParentModel();
    while (parentModel) {
        targetUUID = parentModel->getModelUuid();
        objectUUID = parentModel->getObjectUuid();
        parentModel = parentModel->getParentModel();
    }
    // board open not save
    if (PropertyServerMng::getInstance().m_pGraphicsModelingServer->hasDrawingBoardById(objectUUID)) {
        return false;
    }
    return saveProjectModelByUUID(targetUUID);
}

// private
QSharedPointer<KLProject> getCurProject()
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return PKLProject(nullptr);
    }
    return PropertyServerMng::getInstance().m_projectManagerServer->GetCurProject();
}

bool saveProjectModelByUUID(const QString &modelUUID)
{
    if (modelUUID.isEmpty()) {
        return false;
    }
    PKLProject pCurProject = getCurProject();
    if (pCurProject == nullptr) {
        return false;
    }
    PModel model = pCurProject->findModel(modelUUID);
    if (model == nullptr) {
        return false;
    }
    return pCurProject->saveModel(model);
}

QSharedPointer<Kcc::BlockDefinition::Model> getProjectProtoModel(QSharedPointer<Kcc::BlockDefinition::Model> instance)
{
    if (instance == nullptr) {
        return PModel(nullptr);
    }
    if (!instance->isInstance()) {
        return PModel(nullptr);
    }
    QList<PModel> models = getProjectModels(instance->getModelType());
    for (PModel protoModel : models) {
        if (protoModel != nullptr && protoModel->getPrototypeName() == instance->getPrototypeName()) {
            return protoModel;
        }
    }
    return PModel(nullptr);
}

bool isEqualO(const double &value)
{
    if (fabs(value) < 1e-15) {
        return true;
    }
    return false;
}
}
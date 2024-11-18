#include "AssistantDefine.h"

#include "GlobalAssistant.h"
#include "KLProject/KLProject.h"
#include "ProjectViewServers.h"

namespace AssistantDefine {
void addModelVarToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &cpxBoardName, bool recursive)
{
    if (model == nullptr || ProjectViewServers::getInstance().m_projectManagerServer == nullptr
        || ProjectViewServers::getInstance().m_dataDictionatyServer == nullptr) {
        return;
    }
    // 只有normal的模块可以添加
    if (Model::StateNormal != model->getState() && Model::StateWarring != model->getState()) {
        return;
    }

    // // 若顶层电气画板未激活则返回
    // if (model->getTopParentModel() && model->getTopParentModel()->getModelType() == ElecBoardModel::Type) {
    //     auto topModel = model->getTopParentModel();
    //     auto project = ProjectViewServers::getInstance().m_projectManagerServer->GetCurProject();
    //     QVariantMap boardactivemap;
    //     if (project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
    //         boardactivemap = project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
    //     }
    //     if (!boardactivemap.contains(topModel->getUUID())) {
    //         return;
    //     } else if (boardactivemap.value(topModel->getUUID()).toBool() == false) {
    //         return;
    //     }
    // }

    QString curDict = ProjectViewServers::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
    QList<PVariable> varlist = model->getResultSaveVariableList();
    for (PVariable pvar : varlist) {
        if (pvar == nullptr) {
            continue;
        }
        QString dicvar = NPS::getDictBoardBlockVarStr(curDict, model, cpxBoardName, pvar->getName());
        if (!dicvar.isEmpty()) {
            ProjectViewServers::getInstance().m_dataDictionatyServer->AddDataDictionary(dicvar);
        }
    }
    if (recursive) {
        for (PModel cmodel : model->getChildModels().values()) {
            if (cmodel == nullptr) {
                continue;
            }
            addModelVarToDict(cmodel, cpxBoardName, recursive);
        }
    }
}

void deleteModelVarToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &cpxBoardName)
{
    if (model == nullptr || ProjectViewServers::getInstance().m_projectManagerServer == nullptr
        || ProjectViewServers::getInstance().m_dataDictionatyServer == nullptr) {
        return;
    }
    QString curDict = ProjectViewServers::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
    QString dicvar = NPS::getDictBoardBlockVarStr(curDict, model);
    if (!dicvar.isEmpty()) {
        ProjectViewServers::getInstance().m_dataDictionatyServer->DelDataDictionary(dicvar);
    }
}

void renameModelVarToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &oldname)
{
    if (model == nullptr || ProjectViewServers::getInstance().m_projectManagerServer == nullptr
        || ProjectViewServers::getInstance().m_dataDictionatyServer == nullptr) {
        return;
    }
    // 只有normal的模块可以添加
    if (Model::StateNormal != model->getState()) {
        return;
    }

    QString curDict = ProjectViewServers::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
    if (ComplexBoardModel::Type == model->getModelType()) {
        if (model->isInstance()) {
            ProjectViewServers::getInstance().m_dataDictionatyServer->RenameDictionaryElement(
                    getDictVar(curDict, oldname), model->getName());
        }
    } else if (ControlBoardModel::Type == model->getModelType() || ElecBoardModel::Type == model->getModelType()) {
        ProjectViewServers::getInstance().m_dataDictionatyServer->RenameDictionaryElement(getDictVar(curDict, oldname),
                                                                                          model->getName());
    } else {
        QString dicvar = NPS::getDictBoardBlockVarStr(curDict, model, "", "", oldname);
        if (!dicvar.isEmpty()) {
            ProjectViewServers::getInstance().m_dataDictionatyServer->RenameDictionaryElement(dicvar, model->getName());
        }
    }
}

void addProjectVarToDictionary(QSharedPointer<KLProject> project)
{
    // 先清除原先的数据字典数据
    if (ProjectViewServers::getInstance().m_dataDictionatyServer == nullptr
        || ProjectViewServers::getInstance().m_pModelManagerServer == nullptr) {
        return;
    }
    ProjectViewServers::getInstance().m_dataDictionatyServer->ClearAllCurrentDataDictionary();
    if (project == nullptr) {
        return;
    }
    QVariantMap boardactivemap;
    if (project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
        boardactivemap = project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
    }
    for (PModel model : project->getAllModel()) {
        if (model == nullptr) {
            continue;
        }
        if (ElecBoardModel::Type == model->getModelType()) {
            if (boardactivemap.contains(model->getUUID()) && boardactivemap.value(model->getUUID()).toBool()) {
                addModelVarToDict(model, "", true);
            }
        } else if (CombineBoardModel::Type == model->getModelType() || ControlBlock::Type == model->getModelType()) {
            for (PModel instanceModel : model->getInstanceList()) {
                if (modelNeedAddToDict(instanceModel, project)) {
                    addModelVarToDict(instanceModel, "", true);
                }
            }
        } else if (ControlBoardModel::Type == model->getModelType()) {
            addModelVarToDict(model, "", true);
        }
    }
}

bool modelNeedAddToDict(QSharedPointer<Kcc::BlockDefinition::Model> model, QSharedPointer<KLProject> project)
{
    if (model == nullptr) {
        return false;
    }
    if (Model::StateNormal != model->getState() && Model::StateWarring != model->getState()) {
        return false;
    }
    if (project == nullptr) {
        if (ProjectViewServers::getInstance().m_projectManagerServer != nullptr) {
            project = ProjectViewServers::getInstance().m_projectManagerServer->GetCurProject();
        }
    }
    if (project == nullptr) {
        return false;
    }
    QVariantMap boardactivemap;
    if (project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
        boardactivemap = project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
    }
    PModel parent = model;
    while (parent->getParentModel() != nullptr) {
        parent = parent->getParentModel();
    }
    if (ElecBoardModel::Type == parent->getModelType()) {
        if (boardactivemap.contains(parent->getUUID()) && boardactivemap.value(parent->getUUID()).toBool()) {
            return true;
        } else {
            return false;
        }
    } else if (CombineBoardModel::Type == parent->getModelType() || ControlBlock::Type == parent->getModelType()) {
        if (parent->isInstance() && parent->getConnectedModel(ModelConnSlot).size() > 0) {
            return true;
        } else {
            return false;
        }
    } else if (ControlBoardModel::Type == parent->getModelType()) {
        return true;
    } else if (ComplexBoardModel::Type == parent->getModelType() && parent->isInstance()) {
        return true;
    }

    return false;
}
QString getDictVar(const QString &dictVarStart, const QString &var)
{
    QStringList dictList = dictVarStart.split(".");
    if (dictList.size() <= 0) {
        return var;
    } else if (1 == dictList.size() || 2 == dictList.size()) {
        return QString("%1.%2").arg(dictVarStart).arg(var);
    } else if (3 == dictList.size()) {
        return QString("%1/%2").arg(dictVarStart).arg(var);
    }
    return QString();
}

const QString getOuputLog(const LogType &logtype, const QString &typeName, const QString &contentName, bool sts,
                          const QString &newName)
{
    switch (logtype) {
    case LogType_New: {
        if (sts) {
            return QObject::tr("Create %1 [%2]!").arg(typeName).arg(contentName);
        } else if (contentName.isEmpty()) {
            return QObject::tr("Failed to create %1!").arg(typeName);
        } else {
            return QObject::tr("Failed to create %1 [%2]!").arg(typeName).arg(contentName);
        }
    }
    case LogType_Delete: {
        if (sts) {
            return QObject::tr("Delete %1 [%2]!").arg(typeName).arg(contentName);
        } else {
            return QObject::tr("Failed to delete %1 [%2]!").arg(typeName).arg(contentName);
        }
    }
    case LogType_Copy: {
        if (sts) {
            return QObject::tr("Copy %1 [%2]!").arg(typeName).arg(contentName);
        } else {
            return QObject::tr("Failed to copy %1 [%2]!").arg(typeName).arg(contentName);
        }
    }
    case LogType_Export: {
        if (sts) {
            return QObject::tr("Export %1 [%2] completed!").arg(typeName).arg(contentName);
        } else {
            return QObject::tr("Export %1 [%2] failed!").arg(typeName).arg(contentName);
        }
    }
    case LogType_Import: {
        if (sts) {
            return QObject::tr("Import %1 [%2] completed!").arg(typeName).arg(contentName);
        } else if (contentName.isEmpty()) {
            return QObject::tr("Import %1 failed!").arg(typeName);
        } else {
            return QObject::tr("Import %1 [%2] failed!").arg(typeName).arg(contentName);
        }
    }
    case LogType_Rename: {
        if (sts) {
            return QObject::tr("Rename %1 from [%2] to [%3]!").arg(typeName).arg(contentName).arg(newName);
        } else {
            return QObject::tr("Rename %1 failed!").arg(typeName);
        }
    }
    default:
        break;
    }
    return QString();
}

QMainWindow *getMainWindow()
{
    if (ProjectViewServers::getInstance().m_pUIServer != nullptr
        && ProjectViewServers::getInstance().m_pUIServer->GetMainUI() != nullptr) {
        return dynamic_cast<QMainWindow *>(ProjectViewServers::getInstance().m_pUIServer->GetMainUI());
    }
    return nullptr;
}
}
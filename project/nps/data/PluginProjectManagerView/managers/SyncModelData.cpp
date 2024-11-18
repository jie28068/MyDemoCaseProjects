#include "SyncModelData.h"

#include "Associate.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "ManagerCommonWidget.h"
#include "ProjectViewServers.h"

using namespace Kcc::BlockDefinition;

SyncModelData::SyncModelData(ManagerCommonWidget *mcwidget, QObject *parent)
    : QObject(parent), m_ManagerWidget(mcwidget)
{
}

SyncModelData::~SyncModelData() { }

void SyncModelData::connectProjectAllModels(QSharedPointer<KLProject> project)
{
    if (project == nullptr) {
        return;
    }
    connectModels(project->getAllModel());
}

void SyncModelData::connectModels(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &models)
{
    for (PModel model : models) {
        if (model == nullptr) {
            continue;
        }
        connectModel(model);
    }
}

void SyncModelData::connectModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return;
    }
    connectRealModel(model);
    for (PModel cmodel : model->getChildModels()) {
        connectModel(cmodel);
    }
    if (!model->isInstance()
        && (ControlBlock::Type == model->getModelType() || CombineBoardModel::Type == model->getModelType()
            || ComplexBoardModel::Type == model->getModelType())) {
        for (PModel instanceModel : model->getInstanceList()) {
            connectRealModel(instanceModel);
            for (PModel instancecmodel : instanceModel->getChildModels()) {
                connectModel(instancecmodel);
            }
        }
    }
}

void SyncModelData::disconnectProjectAllModels(QSharedPointer<KLProject> project)
{
    if (project == nullptr) {
        return;
    }
    disconnectModels(project->getAllModel());
}

void SyncModelData::disconnectModels(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &models)
{
    for (PModel model : models) {
        disconnectModel(model);
    }
}

void SyncModelData::disconnectModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return;
    }
    disconnectRealModel(model);
    for (PModel cmodel : model->getChildModels()) {
        disconnectModel(cmodel);
    }
    if (!model->isInstance()
        && (ControlBlock::Type == model->getModelType() || CombineBoardModel::Type == model->getModelType()
            || ComplexBoardModel::Type == model->getModelType())) {
        for (PModel instanceModel : model->getInstanceList()) {
            if (instanceModel == nullptr) {
                continue;
            }
            disconnectRealModel(instanceModel);
            for (PModel instancecmodel : instanceModel->getChildModels()) {
                disconnectModel(instancecmodel);
            }
        }
    }
}

void SyncModelData::onModelSignalChildModelAdded(QSharedPointer<Kcc::BlockDefinition::Model> childModel)
{
    if (AssistantDefine::modelNeedAddToDict(childModel)) {
        AssistantDefine::addModelVarToDict(childModel, "", true);
    }
    connectModel(childModel);
    updateWidget();
}

void SyncModelData::onModelSignalChildModelRemoved(QSharedPointer<Kcc::BlockDefinition::Model> childModel)
{
    if (ProjectViewServers::getInstance().m_projectManagerServer != nullptr
        && ProjectViewServers::getInstance().m_dataDictionatyServer != nullptr) {
        QString curDict = ProjectViewServers::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
        Model *model = dynamic_cast<Model *>(sender());
        if (model == nullptr) {
            return;
        }
        QString dictVar = NPS::getDictBoardBlockVarStr(curDict, childModel, "", "", "", model->sharedFromThis());
        if (!dictVar.isEmpty()) {
            ProjectViewServers::getInstance().m_dataDictionatyServer->DelDataDictionary(dictVar);
        }
    }

    disconnectModel(childModel);
    updateWidget();
}

void SyncModelData::onModelSignalVariableAdded(QSharedPointer<Kcc::BlockDefinition::Variable> variable) { }

void SyncModelData::onModelSignalVariableRemoved(QSharedPointer<Kcc::BlockDefinition::Variable> variable) { }

void SyncModelData::onModelSignalVariableChanged(QSharedPointer<Kcc::BlockDefinition::Variable> variable,
                                                 const QString &role, const QVariant &oldData, const QVariant &newData)
{
    if (variable != nullptr
        && RoleDataDefinition::ModelState == variable->getData(RoleDataDefinition::VariableTypeRole).toString()) {
        Model *model = dynamic_cast<Model *>(sender());
        if (model == nullptr) {
            return;
        }
        if (AssistantDefine::modelNeedAddToDict(model->sharedFromThis())) {
            AssistantDefine::addModelVarToDict(model->sharedFromThis());
        } else {
            AssistantDefine::deleteModelVarToDict(model->sharedFromThis());
        }
    }
}

void SyncModelData::onModelSignalModelNameChanged(const QString &oldName, const QString &newName)
{
    updateWidget();
    Model *model = dynamic_cast<Model *>(sender());
    if (model == nullptr) {
        return;
    }
    if (AssistantDefine::modelNeedAddToDict(model->sharedFromThis())) {
        AssistantDefine::renameModelVarToDict(model->sharedFromThis(), oldName);
    }
}

void SyncModelData::onModelSignalModelPrototypeNameChanged(const QString &oldName, const QString &newName)
{
    updateWidget();
}

void SyncModelData::onModelSignalModelSynchronized() { }

void SyncModelData::onModelSignalPortChanged()
{
    updateWidget();
}

void SyncModelData::onModelSignalConnectorChanged()
{
    updateWidget();
}

void SyncModelData::onModelSignalConnectorAdded(QSharedPointer<Kcc::BlockDefinition::BlockConnector> connector)
{
    updateWidget();
}

void SyncModelData::onModelSignalConnectorRemoved(const QString &uuid)
{
    updateWidget();
}

void SyncModelData::onSignalModelAssociated(PAssociate conn)
{
    if (conn.isNull() || conn->getTarget() == nullptr || conn->getSource() == nullptr) {
        return;
    }
    PModel srcModel = conn->getSource();
    PModel targetModel = conn->getTarget();
    if (targetModel == nullptr) {
        return;
    }

    if (SlotBlock::Type != srcModel->getModelType()) {
        return;
    }
    if ((ControlBlock::Type == targetModel->getModelType() || CombineBoardModel::Type == targetModel->getModelType())
        && targetModel->isInstance()) {
        AssistantDefine::addModelVarToDict(targetModel, srcModel->getParentModelName(), true);
    }
}

void SyncModelData::onSignalModelDisassociated(QSharedPointer<Kcc::BlockDefinition::Model> targetModel,
                                               const QString &type)
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr
        || ProjectViewServers::getInstance().m_dataDictionatyServer == nullptr || targetModel == nullptr) {
        return;
    }
    Model *model = dynamic_cast<Model *>(sender());
    if (model == nullptr) {
        return;
    }
    if (SlotBlock::Type != model->getModelType()) {
        return;
    }

    if (targetModel->isInstance()
        && (ControlBlock::Type == targetModel->getModelType()
            || CombineBoardModel::Type == targetModel->getModelType())) {
        QString curDict = ProjectViewServers::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
        QString dictVar = NPS::getDictBoardBlockVarStr(curDict, targetModel, model->getParentModelName(), "");
        if (!dictVar.isEmpty()) {
            ProjectViewServers::getInstance().m_dataDictionatyServer->DelDataDictionary(dictVar);
        }
    }
}

void SyncModelData::onSignalInstanceAdd(QSharedPointer<Kcc::BlockDefinition::Model> instanceModel)
{
    connectModel(instanceModel);
    updateWidget();
}

void SyncModelData::onSignalInstanceRemoved(QSharedPointer<Kcc::BlockDefinition::Model> instanceModel)
{
    disconnectModel(instanceModel);
    updateWidget();
}

void SyncModelData::updateWidget()
{
    if (m_ManagerWidget == nullptr) {
        return;
    }
    m_ManagerWidget->SyncModelDataToUI();
}

void SyncModelData::connectRealModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return;
    }
    // 绑定信号,model发起的
    connect(model.data(), &Model::signalChildModelAdded, this, &SyncModelData::onModelSignalChildModelAdded,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalChildModelRemoved, this, &SyncModelData::onModelSignalChildModelRemoved,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalVariableAdded, this, &SyncModelData::onModelSignalVariableAdded,
            Qt::UniqueConnection);
    // connect(model.data(), &Model::signalVariableRemoved, this,
    // &SyncModelData::onModelSignalVariableRemoved,Qt::UniqueConnection);
    connect(model.data(), &Model::signalVariableChanged, this, &SyncModelData::onModelSignalVariableChanged,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalModelNameChanged, this, &SyncModelData::onModelSignalModelNameChanged,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalModelPrototypeNameChanged, this,
            &SyncModelData::onModelSignalModelPrototypeNameChanged, Qt::UniqueConnection);
    // connect(model.data(), &Model::signalModelSynchronized, this, &SyncModelData::onModelSignalModelSynchronized,
    //         Qt::UniqueConnection);
    // connect(m_boardModel, &Model::signalPortChanged, this,
    // &SyncModelData::onModelSignalPortChanged,Qt::UniqueConnection);
    // connect(model.data(), &Model::signalConnectorChanged, this, &SyncModelData::onModelSignalConnectorChanged,
    // Qt::UniqueConnection);
    connect(model.data(), &Model::signalConnectorAdded, this, &SyncModelData::onModelSignalConnectorAdded,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalConnectorRemoved, this, &SyncModelData::onModelSignalConnectorRemoved,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalModelAssociated, this, &SyncModelData::onSignalModelAssociated,
            Qt::UniqueConnection);
    connect(model.data(), &Model::signalModelDisassociated, this, &SyncModelData::onSignalModelDisassociated,
            Qt::UniqueConnection);
    if (!model->isInstance()) {
        connect(model.data(), &Model::signalInstanceAdd, this, &SyncModelData::onSignalInstanceAdd,
                Qt::UniqueConnection);
        connect(model.data(), &Model::signalInstanceRemoved, this, &SyncModelData::onSignalInstanceRemoved,
                Qt::UniqueConnection);
    }
}

void SyncModelData::disconnectRealModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return;
    }
    // 绑定信号,model发起的
    disconnect(model.data(), &Model::signalChildModelAdded, this, &SyncModelData::onModelSignalChildModelAdded);
    disconnect(model.data(), &Model::signalChildModelRemoved, this, &SyncModelData::onModelSignalChildModelRemoved);
    // disconnect(model.data(), &Model::signalVariableAdded, this, &SyncModelData::onModelSignalVariableAdded);
    // disconnect(model.data(), &Model::signalVariableRemoved, this,
    // &SyncModelData::onModelSignalVariableRemoved);
    disconnect(model.data(), &Model::signalVariableChanged, this, &SyncModelData::onModelSignalVariableChanged);
    disconnect(model.data(), &Model::signalModelNameChanged, this, &SyncModelData::onModelSignalModelNameChanged);
    disconnect(model.data(), &Model::signalModelPrototypeNameChanged, this,
               &SyncModelData::onModelSignalModelPrototypeNameChanged);
    // disconnect(model.data(), &Model::signalModelSynchronized, this, &SyncModelData::onModelSignalModelSynchronized);
    // connect(m_boardModel, &Model::signalPortChanged, this, &SyncModelData::onModelSignalPortChanged);
    // disconnect(model.data(), &Model::signalConnectorChanged, this, &SyncModelData::onModelSignalConnectorChanged);
    disconnect(model.data(), &Model::signalConnectorAdded, this, &SyncModelData::onModelSignalConnectorAdded);
    disconnect(model.data(), &Model::signalConnectorRemoved, this, &SyncModelData::onModelSignalConnectorRemoved);
    disconnect(model.data(), &Model::signalModelAssociated, this, &SyncModelData::onSignalModelAssociated);
    disconnect(model.data(), &Model::signalModelDisassociated, this, &SyncModelData::onSignalModelDisassociated);
    if (!model->isInstance()) {
        disconnect(model.data(), &Model::signalInstanceAdd, this, &SyncModelData::onSignalInstanceAdd);
        disconnect(model.data(), &Model::signalInstanceRemoved, this, &SyncModelData::onSignalInstanceRemoved);
    }
}

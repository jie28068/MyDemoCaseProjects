#include "CustomMineData.h"

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "KLProject/KLProject.h"
using namespace Kcc::BlockDefinition;

CustomMineData::CustomMineData(CustomMineData::Operation ope)
    : QMimeData(), m_operation(ope), m_pKLProject(nullptr), m_model(nullptr)
{
}

CustomMineData::~CustomMineData() { }

void CustomMineData::setOperation(CustomMineData::Operation ope)
{
    m_operation = ope;
}

void CustomMineData::addKLProject(QSharedPointer<KLProject> pProject)
{
    clearCMineData();
    m_pKLProject = pProject;
}

void CustomMineData::addModels(QSharedPointer<KLProject> pProject,
                               QList<QSharedPointer<Kcc::BlockDefinition::Model>> addModels)
{
    clearCMineData();
    m_pKLProject = pProject;
    m_Models = addModels;
}

CustomMineData::Operation CustomMineData::getOperation()
{
    return m_operation;
}

QSharedPointer<KLProject> CustomMineData::getKLProject()
{
    return m_pKLProject;
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>> CustomMineData::getModels(const int &modelType,
                                                                             const QString &prototype)
{
    QList<PModel> list;
    if (modelType == -1) {
        if (prototype.isEmpty()) {
            return m_Models;
        }
        for (PModel model : m_Models) {
            if (model == nullptr) {
                continue;
            }
            if (DeviceModel::Type == modelType) {
                PDeviceModel devmodel = model.dynamicCast<DeviceModel>();
                if (devmodel == nullptr) {
                    continue;
                }
                if (prototype == devmodel->getDeviceTypePrototypeName()) {
                    list.append(model);
                }
                continue;
            }
            if (prototype == model->getPrototypeName()) {
                list.append(model);
            }
        }
        return list;
    }
    for (PModel model : m_Models) {
        if (model == nullptr || modelType != model->getModelType()) {
            continue;
        }

        if (prototype.isEmpty()) {
            list.append(model);
        } else if (DeviceModel::Type == modelType) {
            PDeviceModel devmodel = model.dynamicCast<DeviceModel>();
            if (devmodel == nullptr) {
                continue;
            }
            if (prototype == devmodel->getDeviceTypePrototypeName()) {
                list.append(model);
            }
        } else if (prototype == model->getPrototypeName()) {
            list.append(model);
        }
    }
    return list;
}

void CustomMineData::clearCMineData()
{
    m_pKLProject = PKLProject(nullptr);
    m_model = PModel(nullptr);
    m_Models.clear();
}

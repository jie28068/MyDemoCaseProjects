#ifndef CUSTOMMINEDATA_H
#define CUSTOMMINEDATA_H

#pragma once
#include <QMimeData>
#include <QSharedPointer>

class KLProject;
namespace Kcc {
namespace BlockDefinition {
class Model;
class DeviceModel;
}
}

class CustomMineData : public QMimeData
{
public:
    enum Operation { Operation_Invalid, Operation_Copy, Operation_Cut };
    CustomMineData(CustomMineData::Operation ope);
    ~CustomMineData();
    void setOperation(CustomMineData::Operation ope);
    void addKLProject(QSharedPointer<KLProject> pProject);
    void addModels(QSharedPointer<KLProject> pProject, QList<QSharedPointer<Kcc::BlockDefinition::Model>> addModels);
    CustomMineData::Operation getOperation();
    QSharedPointer<KLProject> getKLProject();
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> getModels(const int &modelType = -1,
                                                                 const QString &prototype = "");

private:
    void clearCMineData();

private:
    QSharedPointer<KLProject> m_pKLProject;
    QSharedPointer<Kcc::BlockDefinition::Model> m_model;
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_Models;
    Operation m_operation;
};

#endif
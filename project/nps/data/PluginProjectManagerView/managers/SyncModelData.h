#ifndef SYNCMODELDATA_H
#define SYNCMODELDATA_H

#pragma once
#include "KLProject/KLProject.h"
#include <QList>
#include <QObject>
#include <QSharedPointer>
namespace Kcc {
namespace BlockDefinition {
class Model;
class Variable;
class BlockConnector;
}
}
class ManagerCommonWidget;

class SyncModelData : public QObject
{
    Q_OBJECT
public:
    SyncModelData(ManagerCommonWidget *mcwidget, QObject *parent = nullptr);
    ~SyncModelData();
    /// @brief 连接项目所有模型
    /// @param project
    void connectProjectAllModels(QSharedPointer<KLProject> project);
    /// @brief 连接多个模型
    /// @param models
    void connectModels(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &models);
    /// @brief 连接一个模型
    /// @param model
    void connectModel(QSharedPointer<Kcc::BlockDefinition::Model> model);

    /// @brief 断开项目所有model的连接
    /// @param project
    void disconnectProjectAllModels(QSharedPointer<KLProject> project);
    /// @brief 断开多个模型的连接
    /// @param models 模型
    void disconnectModels(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &models);
    /// @brief 断开单个模型的连接
    /// @param model 模型
    void disconnectModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
private slots:
    void onModelSignalChildModelAdded(QSharedPointer<Kcc::BlockDefinition::Model> childModel);
    void onModelSignalChildModelRemoved(QSharedPointer<Kcc::BlockDefinition::Model> childModel);
    void onModelSignalVariableAdded(QSharedPointer<Kcc::BlockDefinition::Variable> variable);
    void onModelSignalVariableRemoved(QSharedPointer<Kcc::BlockDefinition::Variable> variable);
    void onModelSignalVariableChanged(QSharedPointer<Kcc::BlockDefinition::Variable> variable, const QString &role,
                                      const QVariant &oldData, const QVariant &newData);

    void onModelSignalModelNameChanged(const QString &oldName, const QString &newName);
    void onModelSignalModelPrototypeNameChanged(const QString &oldName, const QString &newName);
    void onModelSignalModelSynchronized();
    void onModelSignalPortChanged();
    void onModelSignalConnectorChanged();
    void onModelSignalConnectorAdded(QSharedPointer<Kcc::BlockDefinition::BlockConnector> connector);
    void onModelSignalConnectorRemoved(const QString &uuid);
    void onSignalModelAssociated(PAssociate conn);
    void onSignalModelDisassociated(QSharedPointer<Kcc::BlockDefinition::Model> targetModel, const QString &type);
    void onSignalInstanceAdd(QSharedPointer<Kcc::BlockDefinition::Model> instanceModel);
    void onSignalInstanceRemoved(QSharedPointer<Kcc::BlockDefinition::Model> instanceModel);

private:
    void updateWidget();
    void connectRealModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void disconnectRealModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void addBoardVarToDict();

private:
    ManagerCommonWidget *m_ManagerWidget;
};

#endif
#ifndef TABLEMODELDATAMODEL_H
#define TABLEMODELDATAMODEL_H

#pragma once
#include "TableBaseModel.h"
class CustomTreeModel;
class ProjDataManager;

class TableModelDataModel : public TableBaseModel
{
public:
    TableModelDataModel(QSharedPointer<ProjDataManager> pProjDataManager, QObject *parent = nullptr);
    ~TableModelDataModel();

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    // userdefine
    /// @brief 设置treemodel
    /// @param pTreeModel
    void setCustomTreeModel(CustomTreeModel *pTreeModel);
    /// @brief 更新对应的table数据
    /// @param index treemodel的index
    void updateTableData(const QModelIndex &index, int &freezeCol);
    /// @brief 批量处理
    /// @param indexlist 批量处理的index列表
    /// @param newvalue 批量处理改为的新值
    /// @param valuetype 批量处理当前index的数据类型
    /// @param modifylist 修改的数据列表，用来undo,redo
    virtual void batchModifyModel(const QModelIndexList &indexlist, const QVariant &newvalue, const QString &valuetype,
                                  QList<OldNewDataStruct> &modifylist);
    /// @brief 修改数据，给undo，redo调用
    /// @param modifylist 修改的数据列表
    /// @param bredo true为redo，否则为undo
    void modifyData(const QList<OldNewDataStruct> &modifylist, bool bredo);
    /// @brief 表格中粘贴设备类型，是覆盖所有数据值
    /// @param devModel 待粘贴的设备类型
    /// @param sourceIndex 表格实际index
    /// @param modifylist 修改的数据列表，用来undo,redo
    void pasteDeviceModel(QSharedPointer<Kcc::BlockDefinition::Model> devModel, const QModelIndex &sourceIndex,
                          QList<OldNewDataStruct> &modifylist);
    /// @brief 刷新数据
    /// @param model
    void emitDataChange(QSharedPointer<Kcc::BlockDefinition::Model> model);

protected:
    virtual QVariant TModelData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const;
    // 返回对应数据
    QVariant ControlBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QModelIndex &index,
                              TableBaseModel::ModelData modeldata, bool editable = false) const;
    QVariant ComplexSlotBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QModelIndex &index,
                                  TableBaseModel::ModelData modeldata) const;
    QVariant DeviceTypeModelData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QModelIndex &index,
                                 TableBaseModel::ModelData modeldata) const;
    QVariant ElectricalBlockData(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QModelIndex &index,
                                 TableBaseModel::ModelData modeldata) const;
    QVariant BlockInstanceData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const;
    QVariant ControlInstancesData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const;
    // 修改数据
    bool modifyDeviceTypeModel(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const int &row,
                               const QMap<int, QMap<QString, QVariant>> &modfymap);
    bool modifyElectricalBlock(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const int &row,
                               const QMap<int, QMap<QString, QVariant>> &modfymap);
    bool modifyBlockInstance(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                             const QMap<int, QMap<QString, QVariant>> &modfymap);
    bool modifyControlInstances(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const int &row,
                                const QMap<int, QMap<QString, QVariant>> &modfymap);

private:
    /// @brief 更新表格实际的table模型数据
    /// @param models 模型列表
    /// @param modeldatatype 模型类型
    /// @param prototypeName 模型原型，如果是电气元件分类，设备类型分类等
    void setModelData(QList<QSharedPointer<Kcc::BlockDefinition::Model>> models,
                      TableBaseModel::ModelDataType modeldatatype, const QString &prototypeName = "");
    // electrical model
    void updateElecElements(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &listdata,
                            const QString &prototypeName);
    void addElecConBusbarItem(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &listdata);
    /// @brief 更新title
    /// @param models 模型列表
    void updateTitleInfo(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> models);
    /// @brief 是否已经追加了当前原型名的列
    /// @param prototypename
    /// @return
    bool hasAddProtoType(const QString &prototypename);
    // readonly table
    void getDrawBoardReadOnlyData(QList<QSharedPointer<Kcc::BlockDefinition::Model>> models,
                                  QList<QStringList> &listdata, QStringList &titlelist);
    // slot model
    QVariant getSlotInstanceGroupStringList(QSharedPointer<Kcc::BlockDefinition::SlotBlock> slot,
                                            const QString &groupName) const;
    // common
    void updateModelItemList(const QStringList &titlelist, const QStringList &notUseColorList = QStringList());
    void updateModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &groupName,
                             TableBaseModel::VariableClass groupvarclass, const QString &prototypename,
                             const QString keyAddstr = "", const QString titleAddstr = "");
    /// @brief 获取电气模型组内数据
    /// @param pmodel
    /// @param groupName
    /// @param keyword
    /// @param modeldata
    /// @return
    QVariant getParamValue(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                           const QString &keyword, TableBaseModel::ModelData modeldata) const;
    /// @brief 获取控制实例模型组内数据
    /// @param index
    /// @param modeldata
    /// @return
    QVariant getControlInstanceValue(const QModelIndex index, TableBaseModel::ModelData modeldata) const;
    /// @brief 获取新老数据结构，修改数据时候用
    /// @param row 所在行
    /// @param newvalue 新值
    /// @param valuedatatype 目标修改类型
    /// @param indexList index列表
    /// @return
    OldNewDataStruct getOldNewDataStruct(const int &row, const QVariant &newvalue, const QString &valuedatatype,
                                         const QList<QModelIndex> &indexList);
    /// @brief 修改一个参数引起另一个参数的值变化，比如负载Type变化，RLC不可编辑的值改为0
    /// @param oldnewStruct 数据结构
    /// @param model 当前模型
    /// @param index 当前index
    void addRelativeChangeItem(OldNewDataStruct &oldnewStruct, QSharedPointer<Kcc::BlockDefinition::Model> model,
                               const QModelIndex &index);
    /// @brief 检查电气模块变量是否可修改，有些模块有特殊处理，如同步电机的仿真参数，有大小关系
    /// @param model 模型
    /// @param newvalue 修改的值
    /// @param targetIndex 想修改的位置
    /// @param indexList 总共修改的位置
    /// @param errorInfo 错误信息
    /// @return
    bool checkElecVarModifyValid(QSharedPointer<Kcc::BlockDefinition::Model> model, const QVariant &newvalue,
                                 const QModelIndex targetIndex, const QList<QModelIndex> &indexList,
                                 QString &errorInfo);
    /// @brief 根据OldNewDataStruct数据获取模型
    /// @param item OldNewDataStruct
    /// @param rowindex 所在行
    /// @return
    QSharedPointer<Kcc::BlockDefinition::Model> getModelItemByDataStruct(const OldNewDataStruct &item, int &rowindex);
    /// @brief 获取模块实例和构造实例的变量行
    /// @param modelUUID 模型id
    /// @param groupName 组名
    /// @param varName 变量名
    /// @return
    int getInstanceVarRow(const QString &modelUUID, const QString &groupName, const QString &varName);
    /// @brief 保存模型数据到项目
    /// @param modelUUID 模型uuid
    /// @return
    bool saveProjectModelByUUID(const QString &modelUUID);
    /// @brief 获取实例的路径，不包括最上层
    /// @param model 实例model
    /// @param keyword 关键字
    /// @return
    QString getInstanceKeywordPath(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &keyword);
    /// @brief 滤波和RLC的电气参数是否可编辑
    /// @param model
    /// @param var
    /// @return
    bool isEditable(QSharedPointer<Kcc::BlockDefinition::Model> model,
                    QSharedPointer<Kcc::BlockDefinition::Variable> var) const;
    /// @brief 设备类型参数是否可以编辑
    /// @param model
    /// @param var
    /// @return
    bool isDeviceParamEditable(QSharedPointer<Kcc::BlockDefinition::Model> model,
                               QSharedPointer<Kcc::BlockDefinition::Variable> var) const;
    QSharedPointer<Kcc::BlockDefinition::Model> findRealModel(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                              const QString &keyword) const;
    QString valueMaptoAlgorithm(const QVariant &value);

private:
    CustomTreeModel *m_pTreeModel;
    QSharedPointer<ProjDataManager> m_pProjDataManager;
};

#endif
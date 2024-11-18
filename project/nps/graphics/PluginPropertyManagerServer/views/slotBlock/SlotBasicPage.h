#ifndef SLOTBASICPAGE_H
#define SLOTBASICPAGE_H

#pragma once

#include "TypeItemView.h"
#include "tableheaderview.h"
#include "ui_slotproperty.h"

class KCustomDialog;
class ControlBlockTypeSelectView;
namespace Kcc {
namespace BlockDefinition {
class SlotBlock;
class Model;
class Variable;
class VariableGroup;
}
}

namespace Kcc {
namespace ElecSys {
struct ElecParamDef;
}
}

// 插槽基础属性页
class SlotBasicPage : public CWidget
{
    Q_OBJECT
public:
    enum PortOpe { PortOpe_Invalid, PortOpe_Add, PortOpe_Delete, PortOpe_UpdateOrder };
    SlotBasicPage(QSharedPointer<Kcc::BlockDefinition::SlotBlock> pSlotBlock, QWidget *parent = nullptr);
    ~SlotBasicPage();
    bool saveData(const QStringList &inputCheckedList, const QStringList &outputCheckedList);
    QString getSelectBlockDef();
    const int getPhase(); // 获取相位，电气元件特有
    QSharedPointer<Kcc::BlockDefinition::Model> getSelectModel();
    virtual bool checkLegitimacy(QString &errorinfo) override;
    virtual void setCWidgetReadOnly(bool bReadOnly) override;

signals:
    void slotTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);

private slots:
    void onEditClicked(bool checked);
    void onRemoveTypeClicked();
    void onSelectTypeClicked();
    void onCurrentTextChanged(const QString &textstr);

private:
    void initUI();
    QString getPath(const QString &strName);
    QString getName();
    void renameName(const QString &selectPrototype);
    /// @brief 保存slot自定义数据
    /// @param varlist
    /// @param groupName
    /// @param clear
    /// @return
    bool saveVariables(const QStringList &varlist, const QString &groupName);
    /// @brief 获取自定义变量列表
    /// @param pSlotBlock
    /// @param groupname
    /// @return
    QStringList getVariableNameList(QSharedPointer<Kcc::BlockDefinition::SlotBlock> pSlotBlock,
                                    const QString &groupname);
    /// @brief 保存选择了block的数据
    /// @param groupname
    /// @param checkedList
    /// @param bclear
    /// @return
    bool saveGroupVariables(const QString &groupname, const QStringList &checkedList);
    /// @brief 保存新block数据到slot
    /// @param elecvarmap
    /// @param groupname
    /// @param checkList
    /// @return
    bool saveElectricalGroupVariables(const QMap<QString, Kcc::ElecSys::ElecParamDef> &elecvarmap,
                                      const QString &groupname, const QStringList &checkList);
    /// @brief 保存新的block数据到slot
    /// @param checkVarMap 勾选的变量
    /// @param groupname
    /// @param checkList
    /// @return
    bool saveControlGroupVariables(const QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>> &checkVarMap,
                                   const QString &groupname, const QStringList &checkList);
    /// @brief 更新端口
    /// @param slotBlock
    /// @param portGroup
    /// @param groupname
    /// @param pvar
    /// @param ope
    /// @return
    bool updatePort(QSharedPointer<Kcc::BlockDefinition::SlotBlock> slotBlock,
                    QSharedPointer<Kcc::BlockDefinition::VariableGroup> portGroup, const QString &groupname,
                    QSharedPointer<Kcc::BlockDefinition::Variable> pvar, SlotBasicPage::PortOpe ope);
    /// @brief 开始设置group
    /// @param groupName
    /// @return
    QSharedPointer<Kcc::BlockDefinition::VariableGroup> beginResetGroup(const QString &groupName);
    /// @brief 获取控制模块选中的变量map
    /// @param sortList 所有变量列表
    /// @param checkedList 选中的变量列表
    /// @return
    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>>
    getVarMap(const QList<QSharedPointer<Kcc::BlockDefinition::Variable>> &sortList, const QStringList &checkedList);
    /// @brief 获取用户自定义变量列表
    /// @param groupName 组名
    /// @return
    QStringList getUserDefineList(const QString &groupName);
    /// @brief 给端口排序
    /// @param portGroup
    /// @param portList
    void sortPortOrder(QSharedPointer<Kcc::BlockDefinition::VariableGroup> portGroup, const QStringList &portList);
    /// @brief 模型资源设置给slot
    /// @param model 模型
    /// @return
    bool savePicToSlot(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 是否可以更改相位的电气元件
    /// @param model 选择的模型
    /// @param phaseList phase列表
    /// @return
    bool canChangePhase(QSharedPointer<Kcc::BlockDefinition::Model> model, QList<int> &phaseList);
    /// @brief 更新相位状态，显示隐藏
    /// @param model 模型
    void updatePhaseWidgetSts(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 获取初始相位
    /// @param model 模型
    /// @return
    const int getModelInitPhase(QSharedPointer<Kcc::BlockDefinition::Model> model);

private:
    Ui::SlotProperty ui;
    QString m_SelectedBlockName;
    KCustomDialog *m_SelectBlockDlg;
    QSharedPointer<Kcc::BlockDefinition::Model> m_selectModel;
    QSharedPointer<Kcc::BlockDefinition::SlotBlock> m_SlotBlock;
};

#endif
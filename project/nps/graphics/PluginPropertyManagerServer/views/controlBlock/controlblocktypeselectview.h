#ifndef CONTROLBLOCKTYPESELECTVIEW_H
#define CONTROLBLOCKTYPESELECTVIEW_H

#include "CommonModelAssistant.h"
#include "PropertyTableModel.h"
#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QStandardItemModel>
#include <QWidget>

#include "ui_ControlBlockTypeSelect.h"

namespace Kcc {
namespace PluginComponent {
struct ComponentInfo;
}
}

class CustomTableModel;
struct GroupInfo {
    GroupInfo(const QString &_groupName = "") : groupName(_groupName) { }
    QString groupName;
    QList<CustomModelItem> groupModelList; // 组内数据列表
    bool bElectrical;                      // group类型，控制或电气
    QStringList groupTitle;                // 组的title显示名称列表
};

class ControlBlockTypeSelectView : public QWidget
{
    Q_OBJECT

public:
    enum SelectType {
        SelectType_Invalid,
        SelectType_ProjectCtrPrototoType,
        SelectType_CtrPrototoType,
        SelectType_ElecPrototoType,
        SelectType_AllPrototoType,
        SelectType_CtrInstance,
        SelectType_ElecInstance,
        SelectType_AllInstance,
    };
    ControlBlockTypeSelectView(ControlBlockTypeSelectView::SelectType selectType,
                               QSharedPointer<Kcc::BlockDefinition::Variable> filterVariable =
                                       QSharedPointer<Kcc::BlockDefinition::Variable>(nullptr),
                               QWidget *parent = nullptr);
    ~ControlBlockTypeSelectView();
    QSharedPointer<Kcc::BlockDefinition::Model> getSelectedModel();

public slots:
    void onClickedTreeItem(const QModelIndex &index);
    void onDoubleClickedTableItem(const QModelIndex &index);

signals:
    void typeSelect(QSharedPointer<Kcc::BlockDefinition::Model> selectmodel);

private:
    void InitUI(ControlBlockTypeSelectView::SelectType selectType,
                QSharedPointer<Kcc::BlockDefinition::Variable> filterVariable);
    void addGroupInfo(const Kcc::PluginComponent::ComponentInfo &info, bool electrical);
    void addInstances(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> instanceModels,
                      const QString &groupName);
    void addGroupInfo(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> elecboards,
                      const QString &filterprototype); // 添加电气画板模块
    QList<CustomModelItem> getBlockList(const QString &groupname);
    QSharedPointer<Kcc::BlockDefinition::Model> findSelectInstacne();
    QSharedPointer<Kcc::BlockDefinition::Model> findElectricalBlock();
    QSharedPointer<Kcc::BlockDefinition::Model> findControlInstance();
    QSharedPointer<Kcc::BlockDefinition::Model> findSelectPrototype();
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> getInstanceList(int modelType);
    QString getCurGroupName();
    QModelIndex getTableCurIndex();

private:
    Ui::ControlBlockTypeSelectView ui;
    QStandardItemModel *m_ptreeModel;
    CustomTableModel *m_pTableModel;
    QMap<QString, GroupInfo> m_groupInfoMap; // 所有组的信息
    ControlBlockTypeSelectView::SelectType m_selectType;
};

// 自定义table
class CustomTableModel : public PropertyTableModel
{
    Q_OBJECT
public:
    enum Column { Column_Name = 0, Column_CHSName };
    CustomTableModel(QObject *parent = nullptr);
    ~CustomTableModel();
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // 自定义函数
    void updateBlockNameList(const GroupInfo &groupInfo);
    QSharedPointer<Kcc::BlockDefinition::Model> findBoardModel(const QString &boardname);
    QString getProtoRange(QSharedPointer<Kcc::BlockDefinition::Model> boardmodel);
    QString getNameRange(QSharedPointer<Kcc::BlockDefinition::Model> boardmodel, const QString &prototype);

private:
    bool m_bElecBlock;
    QMap<QString, QString> m_ChsMapToProtoType;
};

#endif // CONTROLBLOCKTYPESELECTVIEW_H

#ifndef COMPLEXINSTANCEWIDGET_H
#define COMPLEXINSTANCEWIDGET_H

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "KLineEdit.h"
#include "TypeItemView.h"
#include "customtablewidget.h"
#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QSharedPointer>
#include <QTableWidget>

namespace Kcc {
namespace BlockDefinition {
class Model;
class SlotBlock;
}
}
class KCustomDialog;

class ComplexInstanceWidget : public CWidget
{
    Q_OBJECT

public:
    enum Operation { Operation_NewInstance, Operation_EditInstance };
    ComplexInstanceWidget(QSharedPointer<Kcc::BlockDefinition::Model> complexModel,
                          ComplexInstanceWidget::Operation ope, const QString &cpxProtoName = "",
                          QWidget *parent = nullptr);
    ~ComplexInstanceWidget();
    QSharedPointer<Kcc::BlockDefinition::Model> getComplexInstance();
    virtual bool saveData() override;
    virtual bool checkLegitimacy(QString &errorinfo) override;

private slots:
    void onTableDoubleClicked(const QModelIndex &index);
    void onTableMenuRequested(const QPoint &point);
    void onComplexProtoChanged(const QString &textstr);
    void onClearElement();

private:
    void initUI();
    QSharedPointer<Kcc::BlockDefinition::Model> getCurComplexPrototype();
    QList<CustomModelItem> getModelList(QSharedPointer<Kcc::BlockDefinition::Model> model);
    QSharedPointer<Kcc::BlockDefinition::SlotBlock> getSlotByIndex(const QModelIndex &index);
    void setSlotElement(const QModelIndex &index, QSharedPointer<Kcc::BlockDefinition::Model> elementModel);
    QString getInstanceName();
    void renameComplexInstanceSlot(QSharedPointer<Kcc::BlockDefinition::Model> instanceModel,
                                   QSharedPointer<Kcc::BlockDefinition::Model> protoModel);
    /// @brief 确认当前模块是否被其他模块连接，是否确认继续连接
    /// @param model 模块
    /// @return
    bool connectedComfirm(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 如果是电气模块，需要确认是否相位匹配，不匹配要提示是否继续选择
    /// @param slot 插槽
    /// @param model 模型
    /// @return
    bool phasecomfirm(QSharedPointer<Kcc::BlockDefinition::SlotBlock> slot,
                      QSharedPointer<Kcc::BlockDefinition::Model> model);

private:
    QSharedPointer<Kcc::BlockDefinition::Model> m_complexModel;
    ComplexInstanceWidget::Operation m_InstanceOpe;
    QString m_cpxProtoModelNameInit; // 初始值复合模型原型名
    QSharedPointer<Kcc::BlockDefinition::Model> m_cacheComplexInstance;
    KLineEdit *m_leditInstanceName;
    QComboBox *m_cboxComplexProto;
    TableView *m_tableView;
    PropertyTableModel *m_customTableModel;
    KCustomDialog *m_selectInstanceDlg;
    QAction *m_clearElement;
    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Model>> m_SlotTargetModelMap; // uuid,key
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_allComplexBoards;
};

#endif // COMPLEXINSTANCEWIDGET_H

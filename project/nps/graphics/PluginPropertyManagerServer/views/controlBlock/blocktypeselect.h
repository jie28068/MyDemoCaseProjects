#ifndef BLOCKTYPESELECT_H
#define BLOCKTYPESELECT_H

#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QWidget>

#include "CommonModelAssistant.h"
#include "ElideLineText.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLineEdit.h"
#include "TypeItemView.h"
#include "controlblocktypeselectview.h"
namespace Kcc {
namespace BlockDefinition {
class ControlBlock;
}
}
class BlockTypeSelect : public CWidget
{
    Q_OBJECT
public:
    BlockTypeSelect(QSharedPointer<Kcc::BlockDefinition::Model> model, CMA::Operation ope, bool canChangeType = false,
                    QWidget *parent = nullptr);
    ~BlockTypeSelect();
    bool saveData(QSharedPointer<Kcc::BlockDefinition::Model> model);
    virtual bool checkLegitimacy(QString &errorinfo) override;

signals:
    void blockTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);
    // TODO typechange

private slots:
    void onEditClicked(bool checked);
    void onSelectTypeClicked();
    void onRemoveTypeClicked();

private:
    void initUI();
    void refreshUIData();
    void creatTypeMenu();
    void setTypeInfo();
    QString getPath(const QString &busbarname);
    QString getBlockName() const;
    void setUIPortCount(int count);
    void setUIStepSize();
    int getNewPortCount();
    int getInitPortCout();
    bool canChangePriority();
    /// @brief 是否可以修改端口数量
    /// @return
    bool canChangePort();
    void setUIPriority(int count);
    int getNewPriority() const;
    int getInitPriority();
    void changeInstanceModel(QSharedPointer<Kcc::BlockDefinition::Model> protoModel);

private:
    KCustomDialog *m_customDlg;

    QSharedPointer<Kcc::BlockDefinition::Model> m_pModel;
    CMA::Operation m_Operation;
    bool m_canChangeType; // 可否更改类型

    KLineEdit *m_lineEdit;
    QPushButton *m_typebutton;
    QPushButton *m_editButton;
    ElideLineText *m_typepath;
    QLabel *m_pBusPortNumLabel;
    KLineEdit *m_pBusPortNumEdit;
    QAction *m_pActSelectType;
    QAction *m_pActRemoveType;
    QLabel *m_priorityLabel;
    KLineEdit *m_LineEditPriority;
    // designer需求 设置子系统仿真步长
    QLabel *m_StepSizeLabel;
    KLineEdit *m_LineEditStepSize;

    QString m_oldName;
    int m_nInitPortCount; // 可变端口模块中可变端口的初始数量
};

#endif // BLOCKTYPESELECT_H

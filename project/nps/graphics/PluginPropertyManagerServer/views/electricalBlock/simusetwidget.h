#ifndef SIMUSETWIDGET_H
#define SIMUSETWIDGET_H

#include "customtablewidget.h"
#include "dispersedselect.h"

namespace Kcc {
namespace BlockDefinition {
class ElectricalBlock;
}
}

class SimuSetWidget : public CWidget
{
    Q_OBJECT

public:
    SimuSetWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, QWidget *parent = nullptr);
    ~SimuSetWidget();
    QList<CustomModelItem> getTableList();
    DispersedSelect *getDispersedSelect();
    virtual bool checkLegitimacy(QString &errorinfo) override;
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    bool saveData();

private:
    void initUI();
    QVariant getWFValue();
    CustomModelItem getItemByKeyWords(const QString &keywords);

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElectricalBlock;
    CustomTableWidget *m_pTableWidget;
    DispersedSelect *m_pDispersed;
};

class InitSetWidget : public CustomTableWidget
{
    Q_OBJECT
public:
    InitSetWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, QWidget *parent = nullptr);
    ~InitSetWidget();
    virtual bool saveData() override;

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElectricalBlock;
};
#endif // SIMUSETWIDGET_H

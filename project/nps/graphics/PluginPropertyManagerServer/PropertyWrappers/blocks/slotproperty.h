#ifndef SLOTPROPERTY_H
#define SLOTPROPERTY_H

#include "SlotBasicPage.h"
#include "TypeItemView.h"
#include "blockproperty.h"
#include "varselectwidget.h"

namespace Kcc {
namespace ElecSys {
struct ElecParamDef;
}
}
// 插槽
class SlotProperty : public BlockProperty
{
    Q_OBJECT
public:
    SlotProperty(QSharedPointer<Kcc::BlockDefinition::SlotBlock> pSlotBlock, bool bReadOnly = false);
    ~SlotProperty();

    virtual void init() override;

    virtual QPixmap getBlockPixmap() override;
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;
    QString getHelpUrl() override;

private:
    void refreshWidget(QSharedPointer<Kcc::BlockDefinition::Model> model);
    // 电气参数转为modelitem
    QList<CustomModelItem> getModelItemList(const QMap<QString, Kcc::ElecSys::ElecParamDef> &elecvarmap);
    void setModelItemListChecked(QList<CustomModelItem> &listdata, bool bchecked);
    void setModelItemListChecked(QList<CustomModelItem> &listdata, const QStringList &checkList);
    void getInputOutputList(QSharedPointer<Kcc::BlockDefinition::Model> model, QList<CustomModelItem> &inputList,
                            QList<CustomModelItem> &outputList, const int &phaseNo = 0);

private slots:
    void onSlotTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);

private:
    QSharedPointer<Kcc::BlockDefinition::SlotBlock> m_pSlotBlock;
    SlotBasicPage *m_basicPage;
    VarSelectWidget *m_pInputTableWidget;
    VarSelectWidget *m_pOutPutTableWidget;
};

#endif // SLOTPROPERTY_H

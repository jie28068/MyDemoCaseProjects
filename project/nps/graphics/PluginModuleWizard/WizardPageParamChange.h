#ifndef WIZARDPAGEPARAMCHANGE_H
#define WIZARDPAGEPARAMCHANGE_H

#include "WizardPageParamNew.h"
// 模块参数页 (修改)
class ComponentParamPageOnModify : public ComponentParamPage
{
public:
    ComponentParamPageOnModify(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent = nullptr)
        : ComponentParamPage(pBlock, npBlock, parent)
    {
    }

    explicit ComponentParamPageOnModify(PElectricalBlock pElecBlock, PElectricalBlock npElecBlock,
                                        QWidget *parent = nullptr)
        : ComponentParamPage(pElecBlock, npElecBlock, parent)
    {
    }

    explicit ComponentParamPageOnModify(PDeviceModel pDeviceBlock, PDeviceModel npDeviceBlock,
                                        QWidget *parent = nullptr)
        : ComponentParamPage(pDeviceBlock, npDeviceBlock, parent)
    {
    }

    virtual void initializePage() override;
};

#endif

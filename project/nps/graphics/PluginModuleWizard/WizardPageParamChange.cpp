#include "WizardPageParamChange.h"

void ComponentParamPageOnModify::initializePage()
{
    if (getIsDevice()) {
        pageDevice(m_pDevice);
    } else if (getIsElec()) {
        pageElecInit(m_pElecBloc);
    } else {
        pageInit(m_pBlock);
    }
    // 刷新表格
    if (tableView) {
        tableView->updateFrozenTableGeometry();
    }
}
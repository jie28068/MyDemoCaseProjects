#ifndef __WIZARDPAGEBASE_H__
#define __WIZARDPAGEBASE_H__

#include "CoreLib/ServerManager.h"
#include "KLineEdit.h"
#include "KMessageBox.h"
#include "Wizard.h"
#include "WizardServerMng.h"

const static int MAX_VARIABE_NUM = 300;
const static int MAX_VARIABE_LENGTH = 32;

const std::nullptr_t TXYNUll = nullptr;

// 基类
class WizardPageBase : public QWizardPage
{
public:
    WizardPageBase(PControlBlock pBlock, PControlBlock n_pBlock, QWidget *parent = nullptr)
        : QWizardPage(parent), m_pBlock(n_pBlock), controlBlock(pBlock)
    {
    }

    explicit WizardPageBase(PElectricalBlock pElecBloc, PElectricalBlock n_pElecBloc, QWidget *parent = nullptr)
        : QWizardPage(parent), m_pElecBloc(n_pElecBloc), elecBlock(pElecBloc)
    {
    }

    explicit WizardPageBase(PDeviceModel pDevice, PDeviceModel n_pDevice, QWidget *parent = nullptr)
        : QWizardPage(parent), m_pDevice(n_pDevice), deviceBlock(pDevice)
    {
    }

    // explicit WizardPageBase(PDeviceModel pDevice, PDeviceModel n_pDevice, PIModelManagerServer pModelServer,
    //                         PIModelDataMngServer pModelDataMngServer, QWidget *parent = nullptr)
    //     : QWizardPage(parent),
    //       m_pDevice(n_pDevice),
    //       m_pModelServer(pModelServer),
    //       m_pModelDataMngServer(pModelDataMngServer),
    //       deviceBlock(pDevice)
    // {
    // }

protected:
    PControlBlock m_pBlock;       // 控制
    PElectricalBlock m_pElecBloc; // 电气
    PDeviceModel m_pDevice;       // 电气设备类型

    // 原始数据的block
    PControlBlock controlBlock;
    PElectricalBlock elecBlock;
    PDeviceModel deviceBlock;
};

#endif // __WIZARDPAGE_H__

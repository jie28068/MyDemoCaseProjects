#pragma once

#include "BlockDelegateBase.h"

// 电气模块
class ElecBlockDelegate : public BlockDelegateBase
{
    Q_OBJECT
public:
    ElecBlockDelegate();

protected:
    virtual void init(PModel model);
    virtual void uninit();

private:
    PElectricalBlock m_pElectricalBlock;
};

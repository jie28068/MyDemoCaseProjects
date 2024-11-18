#pragma once

#include "BlockDelegateBase.h"

class ControlBlockDelegate : public BlockDelegateBase
{
public:
    ControlBlockDelegate();

protected:
    virtual void init(PModel model);
    virtual void uninit();

private:
    PControlBlock m_pControlBlock;
};

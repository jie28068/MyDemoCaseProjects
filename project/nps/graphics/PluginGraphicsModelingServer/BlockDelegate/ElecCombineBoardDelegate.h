#ifndef ELECCOMBINEBOARDDELEGATE_H
#define ELECCOMBINEBOARDDELEGATE_H

#pragma once

#include "BlockDelegateBase.h"

class ElecCombineBoardDelegate : public BlockDelegateBase
{
public:
    ElecCombineBoardDelegate();
    ~ElecCombineBoardDelegate();

    virtual bool onDoubleClicked(SourceProxy *source);

protected:
    virtual void init(PModel model);
    virtual void uninit();

private:
    PElecCombineBoardModel m_pElecCombineBoard;
};

#endif
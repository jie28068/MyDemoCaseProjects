#ifndef COMBINEBOARDDELEGATE_H
#define COMBINEBOARDDELEGATE_H

#pragma once

#include "BlockDelegateBase.h"

class CombineBoardDelegate : public BlockDelegateBase
{
public:
    CombineBoardDelegate();
    ~CombineBoardDelegate();

    virtual bool onDoubleClicked(SourceProxy *source);
    // virtual void reloadBlock(PSourceProxy pSource);

protected:
    virtual void init(PModel model);
    virtual void uninit();

private:
    PCombineBoardModel m_pCombineBoard;
};

#endif
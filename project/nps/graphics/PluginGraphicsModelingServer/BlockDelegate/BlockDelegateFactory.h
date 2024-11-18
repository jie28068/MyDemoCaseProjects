#pragma once

#include "BlockDelegateBase.h"

class BlockDelegateFactory
{
public:
    static BlockDelegateBase *create(PModel model);
};

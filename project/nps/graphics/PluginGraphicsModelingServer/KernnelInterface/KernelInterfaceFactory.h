#pragma once

#include "KernnelInterface/BaseKernelInterface.h"

class KernelInterfaceFactory
{
public:
    static PBaseKernelInterface createKernelInterface(PModel pBoardModel);
};

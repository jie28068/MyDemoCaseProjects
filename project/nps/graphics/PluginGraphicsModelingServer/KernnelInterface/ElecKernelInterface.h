#pragma once

#include "BaseKernelInterface.h"

class ElecKernelInterface : public BaseKernelInterface
{
    Q_OBJECT

protected:
    virtual void loadCanvasAfter(PModel pBoardModel, PCanvasContext pCanvasCtx);
};

typedef QSharedPointer<ElecKernelInterface> PElecKernelInterface;

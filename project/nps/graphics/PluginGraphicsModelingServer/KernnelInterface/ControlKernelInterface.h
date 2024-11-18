#pragma once

#include "BaseKernelInterface.h"

class ControlKernelInterface : public BaseKernelInterface
{
    Q_OBJECT

protected:
    virtual void loadCanvasAfter(PModel pBoardModel, PCanvasContext pCanvasCtx);
};

typedef QSharedPointer<ControlKernelInterface> PControlKernelInterface;

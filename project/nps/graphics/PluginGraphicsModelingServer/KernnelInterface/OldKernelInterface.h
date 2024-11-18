#pragma once

#include "BaseKernelInterface.h"

class OldKernelInterface : public BaseKernelInterface
{
    Q_OBJECT

public:
    virtual PCanvasContext loadCanvas(PDrawingBoardClass pBoardClass);
};

typedef QSharedPointer<OldKernelInterface> POldKernelInterface;

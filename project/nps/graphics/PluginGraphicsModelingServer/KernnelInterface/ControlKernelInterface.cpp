#include "ControlKernelInterface.h"
#include "BaseKernelInterface.h"
#include "Business/ControlCanvasHooksServer.h"

void ControlKernelInterface::loadCanvasAfter(PModel pBoardModel, PCanvasContext pCanvasCtx)
{
    if (pBoardModel == nullptr || pCanvasCtx == nullptr) {
        return;
    }
    BaseKernelInterface::loadCanvasAfter(pBoardModel, pCanvasCtx);

    pCanvasCtx->setShowArrow(true);
    pCanvasCtx->setShowConnectorWireSwitch(false);
    pCanvasCtx->setAllowOverlap(false);
    pCanvasCtx->setHighlightHomologousConnectorWire(true);
}

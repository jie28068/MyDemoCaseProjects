#include "ElecKernelInterface.h"
#include "Business/ElectricalCanvasHooksServer.h"

void ElecKernelInterface::loadCanvasAfter(PModel pBoardModel, PCanvasContext pCanvasCtx)
{
    if (pBoardModel == nullptr || pCanvasCtx == nullptr) {
        return;
    }
    BaseKernelInterface::loadCanvasAfter(pBoardModel, pCanvasCtx);

    pCanvasCtx->setShowArrow(false);
    pCanvasCtx->setShowConnectorWireSwitch(true);
    pCanvasCtx->setAllowOverlap(true);
    pCanvasCtx->setHighlightHomologousConnectorWire(false);
}

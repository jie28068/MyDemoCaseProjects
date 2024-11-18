#include "CanvasBusinessHooksServerFactory.h"

#include "ControlCanvasHooksServer.h"
#include "ElectricalCanvasHooksServer.h"

BusinessHooksServer *CanvasBusinessHooksServerFactory::create(QSharedPointer<CanvasContext> pcanvascontext,
                                                              QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    switch (pcanvascontext->type()) {
    case CanvasContext::kElectricalType:
    case CanvasContext::kElecUserDefinedType:
        return new ElectricalCanvasHooksServer(pcanvascontext, model);
    case CanvasContext::kControlSystemType:
    case CanvasContext::kUserDefinedType:
    case CanvasContext::kUserDefinedFrameType:
        return new ControlCanvasHooksServer(pcanvascontext, model);
    default:
        break;
    }

    return nullptr;
}

#include "ElecContainerBlockDelegate.h"
#include "CanvasWidget.h"
#include "GraphicsModelingTool.h"

ElecContainerBlockDelegate::ElecContainerBlockDelegate() { }

void ElecContainerBlockDelegate::init(PModel model)
{
    m_pElecContainerBlock = model.dynamicCast<ElectricalContainerBlock>();
}

void ElecContainerBlockDelegate::uninit() { }

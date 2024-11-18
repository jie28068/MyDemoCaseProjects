#include "ElecBlockDelegate.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingTool.h"

ElecBlockDelegate::ElecBlockDelegate() { }

void ElecBlockDelegate::init(PModel model)
{
    PElectricalBlock pElectricalBlock = model.dynamicCast<ElectricalBlock>();
    if (!pElectricalBlock) {
        return;
    }

    m_pElectricalBlock = pElectricalBlock;
}

void ElecBlockDelegate::uninit() { }

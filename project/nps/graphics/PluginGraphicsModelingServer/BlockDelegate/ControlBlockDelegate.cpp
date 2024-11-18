#include "ControlBlockDelegate.h"

ControlBlockDelegate::ControlBlockDelegate() { }

void ControlBlockDelegate::init(PModel model)
{
    m_pControlBlock = model.dynamicCast<ControlBlock>();
}

void ControlBlockDelegate::uninit() { }

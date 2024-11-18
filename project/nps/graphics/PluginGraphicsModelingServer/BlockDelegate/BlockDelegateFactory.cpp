#include "BlockDelegateFactory.h"
#include "CombineBoardDelegate.h"
#include "ControlBlockDelegate.h"
#include "ElecBlockDelegate.h"
#include "ElecCombineBoardDelegate.h"
#include "ElecContainerBlockDelegate.h"
#include "GraphicsModelingConst.h"
#include "SlotBlockDelegate.h"


BlockDelegateBase *BlockDelegateFactory::create(PModel model)
{
    int modelType = model->getModelType();

    if (modelType == ControlBlock::Type) { // 控制
        return new ControlBlockDelegate();
    } else if (modelType == ElectricalContainerBlock::Type) { // 电气容器
        return new ElecContainerBlockDelegate();
    } else if (modelType == SlotBlock::Type) { // 槽模块
        return new SlotBlockDelegate();
    } else if (modelType == CombineBoardModel::Type) { // 构造Model
        return new CombineBoardDelegate();
    } else if (modelType == ElectricalBlock::Type) { // 电气模块
        return new ElecBlockDelegate();
    } else if (modelType == ElecCombineBoardModel::Type) { // 电气构造Model
        return new ElecCombineBoardDelegate();
    }
    return new BlockDelegateBase();
}

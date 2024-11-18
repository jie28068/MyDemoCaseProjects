#include "KernelInterfaceFactory.h"
#include "ElecBoardModel.h"
#include "KernnelInterface/ControlKernelInterface.h"
#include "KernnelInterface/ElecKernelInterface.h"
#include "KernnelInterface/OldKernelInterface.h"

PBaseKernelInterface KernelInterfaceFactory::createKernelInterface(PModel pBoardModel)
{
    if (pBoardModel == nullptr) {
        PBaseKernelInterface(new BaseKernelInterface());
    }
    if (pBoardModel->hasGrapics()) { // 包含画板序列化数据
        int type = pBoardModel->getModelType();
        if (ElecBoardModel::Type == type || ElecCombineBoardModel::Type == type) { // 电气
            return PElecKernelInterface(new ElecKernelInterface());
        } else {
            return PControlKernelInterface(new ControlKernelInterface());
        }
    } else { // 兼容老数据同步接口
        return POldKernelInterface(new OldKernelInterface());
    }

    return PBaseKernelInterface(new BaseKernelInterface());
}

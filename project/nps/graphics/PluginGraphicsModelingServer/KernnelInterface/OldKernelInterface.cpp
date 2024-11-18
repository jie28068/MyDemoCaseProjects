#include "OldKernelInterface.h"
#include "Data/DataConvertor.h"
#include "KernelTouch.h"
#include "VersionPatch.h"

PCanvasContext OldKernelInterface::loadCanvas(PDrawingBoardClass pBoardClass)
{
    // 不存在序列化数据，直接从BoardClass转换到CanvasContext
    PCanvasContext pCanvasContext = DataConvertor::convertToCanvasContext(pBoardClass);

    m_pKernelTouch->initTouch(pBoardClass, pCanvasContext);

    loadCanvasAfter(pBoardClass, pCanvasContext); // 后处理

    m_boardModel = pBoardClass;
    m_pRTCanvasContext = pCanvasContext;                                               // 画板实时数据
    m_pCanvasContextCache = PCanvasContext(new CanvasContext(*pCanvasContext.data())); // 数据缓存

    // 调用保存画板序列化数据,保证下次打开画板时能使用序列化的数据
    BaseKernelInterface::saveCanvas();
    if (ModelingServerMng::getInstance().m_pProjectMngServer) {
        ModelingServerMng::getInstance().m_pProjectMngServer->SaveBoardModel(m_boardModel);
    }

    return pCanvasContext;
}

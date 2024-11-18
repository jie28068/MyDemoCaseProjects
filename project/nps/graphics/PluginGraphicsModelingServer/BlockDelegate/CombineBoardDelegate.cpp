#include "CombineBoardDelegate.h"
#include "CanvasWidget.h"

CombineBoardDelegate::CombineBoardDelegate() { }

CombineBoardDelegate::~CombineBoardDelegate() { }

bool CombineBoardDelegate::onDoubleClicked(SourceProxy *source)
{
    // 打开该构造画板
    if (m_pModel) {
        auto topBoardModel = m_boardModel->getTopParentModel(); // 顶层画板
        if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
            return false;
        }
        auto *canvasWidget = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(
                topBoardModel->getUUID());
        if (canvasWidget) {
            canvasWidget->openCurrentModel(m_pModel);
            // 下一层为电气构造画板时，画板被冻结且不能解锁，非子系统
            if (!m_pModel->getPrototypeName().startsWith(SUBSYSTEM_PROTO_PREFIX)) {
                canvasWidget->setNextConstructiveBoard(true);
            }
        }
    }

    return false;
}

void CombineBoardDelegate::init(PModel model)
{
    m_pCombineBoard = model.dynamicCast<CombineBoardModel>();
}

void CombineBoardDelegate::uninit() { }

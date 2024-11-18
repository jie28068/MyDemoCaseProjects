#include "BaseKernelInterface.h"
#include "Business/CanvasBusinessHooksServerFactory.h"
#include "CanvasWidget.h"
#include "ElecBoardModel.h"
#include "GraphicsModelingKernel/Utility.h"
#include "Manager/ModelingServerMng.h"
#include "ModelWidget.h"
#include "VersionPatch.h"
#include <GraphicsModelingTool.h>

BaseKernelInterface::BaseKernelInterface()
{
    m_pKernelTouch = PKernelTouch(new KernelTouch(this));
}

PCanvasContext BaseKernelInterface::loadCanvas(PModel pBoardModel)
{
    if (!pBoardModel->hasGrapics())
        return PCanvasContext();
    PCanvasContext pCanvasContext = pBoardModel->getCanvasContext();
    if (pCanvasContext == nullptr) {
        return PCanvasContext();
    }
    /// 复制画板后,复制的画板与原画板uuid和画板名会发生改变,但是external内的图像数据未改变,在这里处理
    QString boardUUID = pBoardModel->getUUID();
    QString boardName = pBoardModel->getName();
    if (boardUUID != pCanvasContext->uuid()) {
        pCanvasContext->setUUID(boardUUID);
    }
    if (boardName != pCanvasContext->name()) {
        pCanvasContext->setName(boardName);
    }
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return PCanvasContext();
    }
    // 将画板的激活状态，加入上下文中
    if (pCanvasContext->type() == CanvasContext::Type::kElectricalType) {
        auto drawingBoardIsActivated = ModelingServerMng::getInstance()
                                               .m_pProjectMngServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE)
                                               .toMap();
        bool isActivated = drawingBoardIsActivated.value(pCanvasContext->uuid(), false).toBool();
        pCanvasContext->setStatusFlag(CanvasContext::kProjectActivate, isActivated);
    } else {
        // 只有电气画板有激活状态
        pCanvasContext->setStatusFlag(CanvasContext::kProjectActivate, true);
    }

    m_pKernelTouch->initTouch(pBoardModel, pCanvasContext);
    loadCanvasAfter(pBoardModel, pCanvasContext); // 后处理

    m_boardModel = pBoardModel;
    m_pRTCanvasContext = pCanvasContext;                                               // 画板实时数据
    m_pCanvasContextCache = PCanvasContext(new CanvasContext(*pCanvasContext.data())); // 数据缓存

    // 版本补丁
    VersionPatch patch;
    patch.putPatch(pBoardModel, pCanvasContext);

    return pCanvasContext;
}

void BaseKernelInterface::saveCanvas()
{
    if (!m_pRTCanvasContext || !m_pCanvasContextCache) {
        return;
    }

    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    if (m_pRTCanvasContext->getLastModifyTime() != dateStr) {
        m_pRTCanvasContext->setLastModifyTime(dateStr);
    }

    m_pCanvasContextCache.clear();
    m_pCanvasContextCache = PCanvasContext(new CanvasContext(*m_pRTCanvasContext.data())); // 数据缓存
}

void BaseKernelInterface::readRTCanvasContext(PModel pBoardModel)
{
    PCanvasContext pCanvasCtx = pBoardModel->getCanvasContext();
    if (pCanvasCtx == nullptr || m_pRTCanvasContext == nullptr) {
        return;
    }
    // 画板相关
    pCanvasCtx->blockSignals(true);

    pCanvasCtx->setDirection(m_pRTCanvasContext->direction());
    pCanvasCtx->setSize(m_pRTCanvasContext->size());
    pCanvasCtx->setScale(m_pRTCanvasContext->scale());
    pCanvasCtx->setCenterPos(m_pRTCanvasContext->centerPos());
    pCanvasCtx->setLocked(m_pRTCanvasContext->getLocked());
    pCanvasCtx->setName(m_pRTCanvasContext->name());
    pCanvasCtx->setBackgroundColor(m_pRTCanvasContext->backgroundColor());
    pCanvasCtx->setGridFlag(m_pRTCanvasContext->gridFlag());

    // 连接线相关
    pCanvasCtx->setLineStyle(m_pRTCanvasContext->lineStyle());
    pCanvasCtx->setLineColor(m_pRTCanvasContext->lineColor());
    pCanvasCtx->setLineSelectColor(m_pRTCanvasContext->lineSelectColor());
    // 图例相关
    pCanvasCtx->setAuthor(m_pRTCanvasContext->author());
    pCanvasCtx->setDescription(m_pRTCanvasContext->description());

    // 属性
    pCanvasCtx->getCanvasProperty().getProperties() = m_pRTCanvasContext->getCanvasProperty().getProperties();

    pCanvasCtx->blockSignals(false);
}

void BaseKernelInterface::emitContentsChanged()
{
    if (m_pRTCanvasContext == nullptr) {
        return;
    }
    if (m_pRTCanvasContext->isShowPowerFlowData()) {
        clearPowerFlowResult();
    }
    emit contentsChanged();
}

void BaseKernelInterface::addBlockToView(PModel pModel, QSharedPointer<ICanvasView> canvasView, bool needUndo)
{
    if (!pModel || m_pRTCanvasContext == nullptr)
        return;

    m_pRTCanvasContext->blockSignals(true);

    CanvasWidget *canvasWidget =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(m_boardModel->getUUID());
    if (canvasWidget) {
        auto boardWidget = canvasWidget->getCurModelWidget();
        if (boardWidget) {
            auto board = boardWidget->getModel();
            if (board) {
                board->addChildModel(pModel);
            }
        }
    } else {
        m_boardModel->addChildModel(pModel);
    }

    m_pKernelTouch->addBlockToView(pModel);

    m_pRTCanvasContext->blockSignals(false);

    emitContentsChanged();
}

void BaseKernelInterface::deleteBlockFromView(PModel pModel, QSharedPointer<ICanvasView> canvasView)
{
    if (!pModel || m_pRTCanvasContext == nullptr)
        return;

    m_pRTCanvasContext->blockSignals(true);

    m_pKernelTouch->deleteBlockFromeView(pModel);

    m_boardModel->removeChildModel(pModel->getUUID());

    m_pRTCanvasContext->blockSignals(false);

    emitContentsChanged();
}

void BaseKernelInterface::addConnector(PBlockConnector connector, QSharedPointer<ICanvasView> canvasView)
{
    if (m_pRTCanvasContext == nullptr)
        return;
    m_pRTCanvasContext->blockSignals(true);

    m_boardModel->addConnector(connector);

    m_pKernelTouch->addConnectorToView(connector);

    m_pRTCanvasContext->blockSignals(false);

    emitContentsChanged();
}

void BaseKernelInterface::deleteConnector(QString connectorUUID, QSharedPointer<ICanvasView> canvasView)
{
    if (m_pRTCanvasContext == nullptr)
        return;
    m_pRTCanvasContext->blockSignals(true);

    m_boardModel->removeConnector(connectorUUID);

    m_pRTCanvasContext->blockSignals(false);

    emitContentsChanged();
}

void BaseKernelInterface::loadCanvasAfter(PModel pBoardModel, PCanvasContext pCanvasCtx)
{
    if (pBoardModel == nullptr || pCanvasCtx == nullptr) {
        return;
    }
    int boardType = pBoardModel->getModelType();
    pCanvasCtx->setType(Utility::getCanvasTypeByModelType(boardType));

    // 设置业务钩子
    auto businessHook = CanvasBusinessHooksServerFactory::create(pCanvasCtx, pBoardModel);
    pCanvasCtx->setBusinessHooksServer(businessHook);
}

void BaseKernelInterface::clearPowerFlowResult()
{
    // 电气画板需要清空
    if (m_boardModel == nullptr) {
        return;
    }

    if (m_boardModel->getModelType() != ElecBoardModel::Type) {
        return;
    }

    for (auto model : m_boardModel->getChildModels()) {
        PElectricalBlock peblock = model.dynamicCast<ElectricalBlock>();
        if (peblock != nullptr && peblock->getLoadFlowResultVariableGroup()) {
            // mode设置为0（不可见）
            QList<QString> keylist = peblock->getLoadFlowResultVariableGroup()->getVariableMapNames();
            foreach (QString keyword, keylist) {
                if (peblock->getLoadFlowResultVariableGroup()->hasVariableName(keyword)) {
                    auto var = peblock->getLoadFlowResultVariableGroup()->getVariableByName(keyword);
                    if (var) {
                        var->setDefaultValue(QVariant());
                        var->setShowMode(Variable::Invisiable);
                    }
                }
            }
        }
    }
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServerIF != nullptr) {
        NotifyStruct notifyStruct;
        notifyStruct.code = Notify_PowerFlowResultClear;
        notifyStruct.paramMap["BoardName"] = m_boardModel->getName();
        notifyStruct.paramMap["BoardType"] = m_boardModel->getModelType();
        notifyStruct.paramMap["BoardUUID"] = m_boardModel->getUUID();
        ModelingServerMng::getInstance().m_pGraphicsModelingServerIF->emitNotify(notifyStruct);
        ModelingServerMng::getInstance().m_pGraphicsModelingServer->setBoardPowerFlowState(m_boardModel->getUUID(),
                                                                                           false);
    }
}

void BaseKernelInterface::saveDataPointMap(QString uuid)
{
    PSourceProxy pSource = m_pCanvasContextCache->getSource(uuid);
    if (pSource) {
        pSource->reSetDataPointMap(m_pRTCanvasContext->getSource(uuid)->getDataPointMap());
    }
}

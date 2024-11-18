#include "DataConvertor.h"
#include "KLModelDefinitionCore/SyncHelper.h"

PCanvasContext DataConvertor::convertToCanvasContext(PDrawingBoardClass pDrawBoardClass)
{
    // 画板
    QString uuid = pDrawBoardClass->getUUID();
    PCanvasContext pCanvasContext = QSharedPointer<CanvasContext>(new CanvasContext(uuid));
    if(pCanvasContext==nullptr)
    {
        return PCanvasContext();
    }
    SyncHelper::syncCanvasContext(pCanvasContext, pDrawBoardClass);

    // 业务模块
    auto blockMap = pDrawBoardClass->getChildModels();
    auto iter = blockMap.constBegin();
    while (iter != blockMap.constEnd()) {
        PModel pBlock = iter.value();
        if (!pBlock) {
            iter++;
            continue;
        }
        PSourceProxy pSource = PSourceProxy(new SourceProxy());
        if(pSource==nullptr)
        {
            return PCanvasContext();
        }
        SyncHelper::syncSource(pBlock, pSource.data());
        pCanvasContext->addSourceProxy(pSource);

        // 端口列表
        SyncHelper::parseBlockPorts(pBlock.data(), pSource.data(), pDrawBoardClass.data(), true);

        iter++;
    }

    // 连接线
    auto connectorMap = pDrawBoardClass->getConnectors();
    for (auto connector : connectorMap) {
        QString uuid = connector->getUUID();
        PConnectorWireContext pNewCtx = PConnectorWireContext(new ConnectorWireContext(uuid));
        if(pNewCtx==nullptr)
        {
            continue;
        }
        SyncHelper::syncConnectorWireContext(connector, pNewCtx, pCanvasContext.data());
        pCanvasContext->addConnectWireContext(pNewCtx);
    }

    return pCanvasContext;
}

#include "BlockDelegateMng.h"
#include "BlockDelegate/BlockDelegateFactory.h"

bool BlockDelegateMng::bindBlockDelegate(PModel model)
{
    if (!model)
        return false;

    PBlockDelegateBase pBlockDelegateBase = PBlockDelegateBase(BlockDelegateFactory::create(model));
    if (!pBlockDelegateBase)
        return false;

    pBlockDelegateBase->initBlock(model, this);

    // 放入查找表
    QString blockID = model->getUUID();
    if (!m_mapBlockDelegate.contains(blockID)) {
        m_mapBlockDelegate[blockID] = pBlockDelegateBase;
    }

    return true;
}

void BlockDelegateMng::unbindBlockDelegate(PModel model)
{
    if (!model)
        return;

    QString blockID = model->getUUID();
    if (m_mapBlockDelegate.contains(blockID)) {
        m_mapBlockDelegate.value(blockID)->uninitBlock(model);
        m_mapBlockDelegate.remove(blockID);
    }
}

PBlockDelegateBase BlockDelegateMng::getBlockDelegate(PModel model)
{
    QString blockID = model->getUUID();

    if (m_mapBlockDelegate.contains(blockID)) {
        return m_mapBlockDelegate[blockID];
    }

    return PBlockDelegateBase();
}

BlockDelegateMng::BlockDelegateMng(PModel pDrawBoard)
{
    m_boardModel = pDrawBoard;
}

BlockDelegateMng::~BlockDelegateMng()
{
    m_mapBlockDelegate.clear();
}

#ifndef BLOCKDELEGATEMNG_H
#define BLOCKDELEGATEMNG_H

#include "BlockDelegate/BlockDelegateBase.h"
#include "Manager/ModelingServerMng.h"

#include <QObject>

class EleBusDelegate;

class BlockDelegateMng : public QObject
{
    Q_OBJECT
public:
    BlockDelegateMng(PModel pBoardModel);
    ~BlockDelegateMng();

    //////////////////////////////////////////////////////////////////////////
    bool bindBlockDelegate(PModel model);   // 绑定模块业务代理,并初始化模块业务
    void unbindBlockDelegate(PModel model); // 解绑模块代理

    PBlockDelegateBase getBlockDelegate(PModel model); // 获取通用代理

public:
    PModel m_boardModel;

private:
    QMap<QString, PBlockDelegateBase> m_mapBlockDelegate; // 模块业务代理查找表

    QList<EleBusDelegate *> m_listBusDelegate; // 母线业务代理
};

typedef QSharedPointer<BlockDelegateMng> PBlockDelegateMng;

#endif // BLOCKDELEGATEMNG_H
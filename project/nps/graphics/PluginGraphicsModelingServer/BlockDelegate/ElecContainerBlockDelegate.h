#pragma once

#include "BlockDelegateBase.h"

class ElecContainerBlockDelegate : public BlockDelegateBase
{
    Q_OBJECT
public:
    ElecContainerBlockDelegate();

    // virtual bool onEditProperty(PSourceProxy source);
    // virtual void reloadBlock(PSourceProxy pSource);

protected:
    virtual void init(PModel model);
    virtual void uninit();

    // void parseContainerBlock(PSourceProxy sourceProxy); // 解析电气接口模块中的端口信息

private:
    PElectricalContainerBlock m_pElecContainerBlock;
};

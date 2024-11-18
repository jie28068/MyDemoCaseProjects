#pragma once

#include "BlockDelegateBase.h"

class SlotBlockDelegate : public BlockDelegateBase
{
    Q_OBJECT
public:
    SlotBlockDelegate();

    // virtual bool onEditProperty(PSourceProxy source);
    // virtual void reloadBlock(PSourceProxy pSource);

protected:
    virtual void init(PModel model);
    virtual void uninit();

    // void parseSlotBlock(PSourceProxy sourceProxy); // 解析槽模块中的端口信息

private:
    PSlotBlock m_pSlotBlock;

    QList<QString> m_allEleModelPrototypeNamelist; // 存放电气元件原型名称
};

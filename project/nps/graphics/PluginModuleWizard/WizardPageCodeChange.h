#ifndef WIZARDPAGECODECHANGE_H
#define WIZARDPAGECODECHANGE_H

#include "WizardPageCodeNew.h"

// 模块代码页 (修改)
class ComponentCodePageOnModify : public ComponentCodePage
{
public:
    ComponentCodePageOnModify(PControlBlock pBlock, PControlBlock npBlock, const QString &path,

                              QWidget *parent = nullptr)
        : ComponentCodePage(pBlock, npBlock, path, parent)
    {
    }

    virtual void initializePage() override;
    virtual bool validatePage() override;
    virtual void cleanupPage() override;
    void setOldBlock(PControlBlock block);

private:
    PControlBlock m_oldBlock; // 保存block数据，用于区分修改后的block和原来block的差分。
};

#endif

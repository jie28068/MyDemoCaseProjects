#include "WizardPageCodeChange.h"

void ComponentCodePageOnModify::initializePage()
{
    PControlBlock block = m_pBlock;
    if (!block->getInputVariableGroup()) {
        block->createVariableGroup(RoleDataDefinition::InputSignal);
    }
    if (!block->getOutputVariableGroup()) {
        block->createVariableGroup(RoleDataDefinition::OutputSignal);
    }
    if (!block->getDiscreteStateVariableGroup()) {
        block->createVariableGroup(RoleDataDefinition::DiscreteStateVariable);
    }
    // #ifdef COMPILER_PRODUCT_DESIGNER
    if (!block->getContinueStateVariableGroup()) {
        block->createVariableGroup(RoleDataDefinition::ContinueStateVariable);
    }
    // #endif
    if (!block->getControlVariableGroup()) {
        block->createVariableGroup(RoleDataDefinition::Parameter);
    }
    if (!block->getInternalVariableGroup()) {
        block->createVariableGroup(RoleDataDefinition::InternalVariable);
    }
    if (block.isNull()) {
        block = QSharedPointer<ControlBlock>(new ControlBlock);
    }
    // 第一次进入
    if (m_blockCache->getPrototypeName().isEmpty()) {
        if (m_pBlock->getPrototypeName() == block->getPrototypeName() && !isVariableChanged(block, m_pBlock)) {
            m_pCodeEdit->setComponent(*(block.data()), false, "");
        } else {
            m_pCodeEdit->setComponent(*(m_pBlock.data()), true, block->getCodeText());
        }
    } else {
        if (m_pBlock->getPrototypeName() == block->getPrototypeName() && !isVariableChanged(block, m_pBlock)) {
            if (m_pBlock->getCodeText() == block->getCodeText()
                || !(m_blockCache->getPrototypeName() == m_pBlock->getPrototypeName()
                     && !isVariableChanged(m_blockCache, m_pBlock))) {
                m_pCodeEdit->setComponent(*(block.data()), false, "");
            } else {
                m_pCodeEdit->setComponent(*(m_pBlock.data()), false, block->getCodeText());
            }
        } else {
            if (m_blockCache->getPrototypeName() == m_pBlock->getPrototypeName()
                && !isVariableChanged(m_blockCache, m_pBlock)) {
                m_pCodeEdit->setComponent(*(m_blockCache.data()), false, block->getCodeText());
            } else {
                m_pCodeEdit->setComponent(*(m_pBlock.data()), true, block->getCodeText());
            }
        }
    }
    if (m_pCodeEdit)
        oldText = m_pCodeEdit->getText();
}

bool ComponentCodePageOnModify::validatePage()
{
    // 判断block版本ID的变化情况
    if (m_oldBlock->getPrototypeName() != m_pBlock->getPrototypeName()) {
        // 说明是新增模块，verID变为0.0.0
        m_pBlock->setVerID(tr("0.0.0"));
    } else {
        QString oldverid = m_oldBlock->getVerID();
        QRegExp veridRegexp("(\\d+)\\.(\\d+)\\.(\\d+)");
        if (veridRegexp.indexIn(oldverid) < 0) {
            oldverid = "0.0.0";
        }
        QStringList veridlist = oldverid.split(".", QString::SkipEmptyParts);
        bool isVarChanged = isVariableChanged(m_oldBlock, m_pBlock);

        if (isVarChanged) {
            // 模块名不变,变量名、类型、个数等变化，第一参数+1（错误）
            veridlist[0] = QString::number(veridlist[0].toInt() + 1);
            m_pBlock->setVerID(veridlist.join("."));
        } else if (!isVarChanged && m_oldBlock->getCodeText() != m_pCodeEdit->getText()) {
            // 代码修改、第二参数+1（警告)
            veridlist[1] = QString::number(veridlist[1].toInt() + 1);
            m_pBlock->setVerID(veridlist.join("."));
        } else {
            // 变量中文名、变量说明，第三参数+1（不影响）
            veridlist[2] = QString::number(veridlist[2].toInt() + 1);
            m_pBlock->setVerID(veridlist.join("."));
        }
    }
    return ComponentCodePage::validatePage();
}

void ComponentCodePageOnModify::cleanupPage()
{
    ComponentCodePage::cleanupPage();
}

void ComponentCodePageOnModify::setOldBlock(PControlBlock block)
{
    if (block) {
        m_oldBlock = qSharedPointerDynamicCast<ControlBlock>(block->copy());
    }
}

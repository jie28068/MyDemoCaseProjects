#include "WizardPageCodeNew.h"
#include <QSplitter>
#include <QTextEdit>

// 自定义编译结果显示框，默认高度40
class CustomTextEdit : public QTextEdit
{
public:
    explicit CustomTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) { }

    QSize sizeHint() const override { return QSize(100, 40); }
};

ComponentCodePage::ComponentCodePage(PControlBlock pBlock, PControlBlock npBlock, const QString &path,
                                     QWidget *parent /*= nullptr*/)
    : WizardPageBase(pBlock, npBlock, parent),
      m_blockCache(new ControlBlock),
      m_path(path),
      oldText(""),
      isSaveOldText(false)
{
    m_pCodeWidget = WizardServerMng::getInstance().m_pCodeManagerServer->CreateCodeEditWidget();
    m_pCodeEdit = dynamic_cast<ICodeEditWidget *>(m_pCodeWidget);
    Q_ASSERT(m_pCodeEdit != nullptr);

    m_pTextEdit = new CustomTextEdit(this);
    m_pTextEdit->setReadOnly(true);

    QSplitter *pSplitter = new QSplitter(Qt::Vertical, this);
    pSplitter->setContentsMargins(0, 0, 0, 0);
    pSplitter->addWidget(m_pCodeWidget);
    pSplitter->addWidget(m_pTextEdit);
    pSplitter->setChildrenCollapsible(true);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(pSplitter);
    setLayout(pLayout);

    // m_pCodeWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_pTextEdit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    WizardServerMng::getInstance().m_pCodeManagerServerIF->connectNotify(
            Notify_CodeEditorTextChanged, this, SLOT(onCodeEditorTextChanged(unsigned int, const NotifyStruct &)));
}

ComponentCodePage::~ComponentCodePage()
{
    WizardServerMng::getInstance().m_pCodeManagerServer->DestroyCodeEditWidget(m_pCodeWidget);
}

void ComponentCodePage::initializePage()
{
    PControlBlock block = QSharedPointer<ControlBlock>(new ControlBlock);
    // 第一次进入画面
    if (m_blockCache->getPrototypeName().isEmpty()) {
        // 无模板，无需对比,直接生成代码
        if (block->getPrototypeName().isEmpty()) {
            m_pCodeEdit->setComponent(*(m_pBlock.data()), true, "");
        } else {
            if (block->getPrototypeName() == m_pBlock->getPrototypeName() && !isVariableChanged(block, m_pBlock)) {
                m_pCodeEdit->setComponent(*(block.data()), false, "");
            } else {
                m_pCodeEdit->setComponent(*(m_pBlock.data()), true, block->getCodeText());
            }
        }
    } else {
        // 第二次进入。无模板
        if (block->getPrototypeName().isEmpty()) {
            if (m_blockCache->getPrototypeName() == m_pBlock->getPrototypeName()
                        && !isVariableChanged(m_blockCache, m_pBlock)
                || (m_blockCache->getPrototypeName() == m_pBlock->getPrototypeName() && isSaveOldText)) {
                m_pCodeEdit->setComponent(*(m_blockCache.data()), false, "");
            } else {
                m_pCodeEdit->setComponent(*(m_pBlock.data()), true, "");
            }
        } else {
            /*有模板不可能相等
            if (block.prototypeName == m_pBlock->prototypeName && !isVariableChanged(block, *m_pBlock)) {
                    // 参数等信息与缓存一致
                    if (block.codeText == m_pBlock->codeText || !(m_blockCache.prototypeName == m_pBlock->prototypeName
            && !isVariableChanged(m_blockCache, *m_pBlock))) { m_pCodeEdit->setComponent(block, false, ""); } else {
                            m_pCodeEdit->setComponent(*m_pBlock, false, block.codeText);
                    }
            } else {*/
            if (m_blockCache->getPrototypeName() == m_pBlock->getPrototypeName()
                        && !isVariableChanged(m_blockCache, m_pBlock)
                || (m_blockCache->getPrototypeName() == m_pBlock->getPrototypeName() && isSaveOldText)) {
                m_pCodeEdit->setComponent(*(m_blockCache.data()), false, block->getCodeText());
            } else {
                m_pCodeEdit->setComponent(*(m_pBlock.data()), true, block->getCodeText());
            }
        }
    }
    if (m_pCodeEdit)
        oldText = m_pCodeEdit->getText();
}

void ComponentCodePage::cleanupPage()
{
    QString newText = m_pCodeEdit->getText();
    if (oldText != newText) {
        // 提示用户是否修改资源为可见
        if (KMessageBox::information(tr("Do you want to save changes to the current page?"),
                                     KMessageBox::Yes | KMessageBox::No)
            == KMessageBox::Yes) {
            // 返回上一页时保存当前代码
            m_pBlock->setCodeText(newText);
            isSaveOldText = true;
        } else {
            m_pBlock->setCodeText(oldText);
            isSaveOldText = false;
        }
    }
    if (m_pBlock->getCodeText().isEmpty()) {
        m_pBlock->setCodeText(newText);
    }
    m_blockCache = qSharedPointerDynamicCast<ControlBlock>(m_pBlock->copy());
}

bool ComponentCodePage::validatePage()
{
    // 结束时将代码保存至block
    saveContorlModel();
    // 生成端口并保存
    savePortInfo();
    return true;
}

void ComponentCodePage::compile()
{
    m_pBlock->setCodeText(m_pCodeEdit->getText());
    m_blockCache = qSharedPointerDynamicCast<ControlBlock>(m_pBlock->copy());
    wizard()->button(QWizard::FinishButton)->setEnabled(false);
    // 文件夹不存在时创建
    QDir isExist(m_path);
    if (!isExist.exists()) {
        isExist.mkpath(m_path);
    }
    WizardServerMng::getInstance().m_pCodeManagerServer->BuildComponentDll(m_path, *(m_blockCache.data()),
                                                                           &ComponentCodePage::buildCallback, this);
}

bool ComponentCodePage::isVariableChanged(PControlBlock left, PControlBlock right)
{
    if (isVariableChanged(left->getInputVariableGroup(), right->getInputVariableGroup())) {
        return true;
    }
    if (isVariableChanged(left->getOutputVariableGroup(), right->getOutputVariableGroup())) {
        return true;
    }
    if (isVariableChanged(left->getDiscreteStateVariableGroup(), right->getDiscreteStateVariableGroup())) {
        return true;
    }
    // #ifdef COMPILER_PRODUCT_DESIGNER
    if (isVariableChanged(left->getContinueStateVariableGroup(), right->getContinueStateVariableGroup())) {
        return true;
    }
    // #endif
    if (isVariableChanged(left->getInternalVariableGroup(), right->getInternalVariableGroup())) {
        return true;
    }
    if (isVariableChanged(left->getControlVariableGroup(), right->getControlVariableGroup())) {
        return true;
    }
    return false;
}

bool ComponentCodePage::isVariableChanged(PVariableGroup mapVariablesLeft, PVariableGroup mapVariablesRight)
{
    if (mapVariablesLeft.isNull()) {
        if (mapVariablesRight.isNull()) {
            return false;
        } else {
            if (mapVariablesRight->getVariableMapNames().size() == 0) {
                return false;
            }
            return true;
        }
    } else {
        if (mapVariablesRight.isNull()) {
            return true;
        }
    }

    if (mapVariablesLeft->getVariableMap().size() != mapVariablesRight->getVariableMap().size()) {
        return true;
    }

    for (QString strName : mapVariablesLeft->getVariableMapNames()) {

        if (!mapVariablesRight->hasVariableName(strName)) {
            return true;
        }

        if (mapVariablesRight->findVariable(RoleDataDefinition::NameRole, strName).first()->getDataType()
            != mapVariablesLeft->findVariable(RoleDataDefinition::NameRole, strName).first()->getDataType()) {
            return true;
        }
    }

    return false;
}

bool ComponentCodePage::buildCallback(void *pParam, const QString &strInfo, CompileCode code)
{
    ComponentCodePage *pThis = static_cast<ComponentCodePage *>(pParam);
    pThis->m_pTextEdit->append(strInfo);
    if (code == Succeed) {
        pThis->wizard()->button(QWizard::FinishButton)->setEnabled(true);
    } else {
        pThis->wizard()->button(QWizard::FinishButton)->setEnabled(false);
    }
    return true;
}

void ComponentCodePage::onCodeEditorTextChanged(unsigned int code, const NotifyStruct &param)
{
    if (!param.paramMap["name"].canConvert<ICodeEditWidget *>()) {
        return;
    }
    ICodeEditWidget *codeeditor = param.paramMap["name"].value<ICodeEditWidget *>();
    if (code == Notify_CodeEditorTextChanged && codeeditor == m_pCodeEdit) {
        m_pTextEdit->clear();
        m_pTextEdit->append(tr("Please compile"));
        wizard()->button(QWizard::FinishButton)->setEnabled(false);
    }
}

bool ComponentCodePage::saveContorlModel()
{
    // 结束时将代码保存至block
    if (controlBlock) {
        controlBlock->setName(m_pBlock->getName());
        controlBlock->setCodeText(m_pCodeEdit->getText());
        controlBlock->setPrototypeName_CHS(m_pBlock->getPrototypeName_CHS());           // 中文名
        controlBlock->setPrototypeName_Readable(m_pBlock->getPrototypeName_Readable()); // 简称
        controlBlock->setDescription(m_pBlock->getDescription());                       // 说明
        controlBlock->setResource(m_pBlock->getResource());                             // 图片资源
        controlBlock->setPrototypeName(m_pBlock->getPrototypeName());                   // 原型名
        controlBlock->setCodeText(m_pBlock->getCodeText());                             // 代码段
        controlBlock->setPriority(m_pBlock->getPriority());                             // 优先级
        controlBlock->setVerID(m_pBlock->getVerID());                                   // 版本号
        controlBlock->setDirectFeedThrough(m_pBlock->getDirectFeedThrough());           // 直接馈通
        //  main组
        PVariableGroup mainGroup = m_pBlock->getVariableGroup(RoleDataDefinition::MainGroup);
        if (mainGroup) {
            if (mainGroup->getVariableMapNames().size() > 0) {
                copyVariable(mainGroup, RoleDataDefinition::MainGroup);
            } else {
                auto group = controlBlock->getVariableGroup(RoleDataDefinition::MainGroup);
                if (group) {
                    group->clearVariableMap();
                }
            }
        }
        // 输入变量
        PVariableGroup inputGroup = m_pBlock->getVariableGroup(RoleDataDefinition::InputSignal);
        if (inputGroup) {
            if (inputGroup->getVariableMapNames().size() > 0) {
                copyVariable(inputGroup, RoleDataDefinition::InputSignal);
            } else {
                auto group = controlBlock->getVariableGroup(RoleDataDefinition::InputSignal);
                if (group) {
                    group->clearVariableMap();
                }
            }
        }
        // 输出变量
        PVariableGroup outputGroup = m_pBlock->getVariableGroup(RoleDataDefinition::OutputSignal);
        if (outputGroup) {
            if (outputGroup->getVariableMapNames().size() > 0) {
                copyVariable(outputGroup, RoleDataDefinition::OutputSignal);
            } else {
                auto group = controlBlock->getVariableGroup(RoleDataDefinition::OutputSignal);
                if (group) {
                    group->clearVariableMap();
                }
            }
        }
        // 状态变量
        PVariableGroup disStateGroup = m_pBlock->getVariableGroup(RoleDataDefinition::DiscreteStateVariable);
        if (disStateGroup) {
            if (disStateGroup->getVariableMapNames().size() > 0) {
                copyVariable(disStateGroup, RoleDataDefinition::DiscreteStateVariable);
            } else {
                controlBlock->removeVariableGroup(RoleDataDefinition::DiscreteStateVariable);
            }
        }
        // #ifdef COMPILER_PRODUCT_DESIGNER
        // 连续状态变量
        PVariableGroup conStateGroup = m_pBlock->getVariableGroup(RoleDataDefinition::ContinueStateVariable);
        if (conStateGroup) {
            if (conStateGroup->getVariableMapNames().size() > 0) {
                copyVariable(conStateGroup, RoleDataDefinition::ContinueStateVariable);
            } else {
                controlBlock->removeVariableGroup(RoleDataDefinition::ContinueStateVariable);
            }
        }
        // #endif
        // 内部变量
        PVariableGroup internalGroup = m_pBlock->getVariableGroup(RoleDataDefinition::InternalVariable);
        if (internalGroup) {
            if (internalGroup->getVariableMapNames().size() > 0) {
                copyVariable(internalGroup, RoleDataDefinition::InternalVariable);
            } else {
                controlBlock->removeVariableGroup(RoleDataDefinition::InternalVariable);
            }
        }
        // 模块参数
        PVariableGroup paramGroup = m_pBlock->getVariableGroup(RoleDataDefinition::Parameter);
        if (paramGroup) {
            if (paramGroup->getVariableMapNames().size() > 0) {
                copyVariable(paramGroup, RoleDataDefinition::Parameter);
            } else {
                controlBlock->removeVariableGroup(RoleDataDefinition::Parameter);
            }
        }
        // end
    }
    return true;
}

void ComponentCodePage::savePortInfo()
{
    auto controlport = controlBlock->getPortManager();
    if (!controlport) {
        controlport = controlBlock->createVariableGroup(RoleDataDefinition::PortGroup);
    }
    auto portGroup = controlport->getVariableSortByOrder();
    // 把原有的输入variable用临时变量保存
    QList<PVariable> inTempGroup;
    // 把原有的输出variable用临时变量保存
    QList<PVariable> outTempGroup;
    for (auto i : portGroup) {
        if (i->getPortType() == Variable::ControlIn) {
            inTempGroup.push_back(i);
        } else {
            outTempGroup.push_back(i);
        }
    }
    // 将新的输入输出变量生成新的输入输出端口临时组
    auto inVariable = controlBlock->getInputVariableGroup();
    if (inVariable) {
        auto inGroup = inVariable->getVariableSortByOrder();
        copyPortVariable(inTempGroup, inGroup, true);
    }
    auto outVariable = controlBlock->getOutputVariableGroup();
    if (outVariable) {
        auto outGroup = outVariable->getVariableSortByOrder();
        copyPortVariable(outTempGroup, outGroup, false);
    }
    // 输出组重新设置序号
    for (int i = 0; i < outTempGroup.size(); ++i) {
        outTempGroup[i]->setOrder(inTempGroup.size() + i);
    }
    // 将俩个组拼起来
    auto newPortGroup = controlBlock->getPortManager();
    // 清除原有的数据
    newPortGroup->clearVariableMap();
    for (auto var : inTempGroup) {
        auto newVar = newPortGroup->createVariable();
        newVar->setUUID(var->getUUID());
        newVar->detach();
        newVar->copyFromVariable(var);
    }
    for (auto var : outTempGroup) {
        auto newVar = newPortGroup->createVariable();
        newVar->setUUID(var->getUUID());
        newVar->detach();
        newVar->copyFromVariable(var);
    }
}

void ComponentCodePage::copyVariable(PVariableGroup groupValue, const QString &str)
{
    auto group = controlBlock->createVariableGroup(str);
    for (auto &variable : groupValue->getVariableSortByOrder()) {
        auto var = group->getVariable(variable->getUUID());
        if (var) {
            var->copyFromVariable(variable);
        } else {
            auto newVar = group->createVariable();
            newVar->setUUID(variable->getUUID());
            newVar->detach();
            newVar->copyFromVariable(variable);
        }
    }
    for (auto &uuid : group->getVariableMap().keys()) {
        if (!groupValue->hasVariable(uuid)) {
            group->removeVariable(uuid);
        }
    }
}

void ComponentCodePage::copyPortVariable(QList<PVariable> &oldVariable, const QList<PVariable> &newVariable,
                                         bool isCtrlIn)
{
    int total = newVariable.size();
    // 数量相同时
    if (oldVariable.size() == newVariable.size()) {
        for (int i = 0; i < oldVariable.size(); ++i) {
            copyPortVariableDate(oldVariable[i], newVariable[i], isCtrlIn, i, total);
        }
    } else if (oldVariable.size() > newVariable.size()) {
        // 数量少了
        for (int i = 0; i < newVariable.size(); ++i) {
            copyPortVariableDate(oldVariable[i], newVariable[i], isCtrlIn, i, total);
        }
        // 删除多余的
        for (int i = oldVariable.size() - 1; i >= newVariable.size(); --i) {
            oldVariable.removeAt(i);
        }
    } else if (oldVariable.size() < newVariable.size()) {
        // 数量多了
        for (int i = 0; i < oldVariable.size(); ++i) {
            copyPortVariableDate(oldVariable[i], newVariable[i], isCtrlIn, i, total);
        }
        // 新建
        for (int i = oldVariable.size(); i < newVariable.size(); ++i) {
            auto variable = controlBlock->getPortManager()->createVariable();
            copyPortVariableDate(variable, newVariable[i], isCtrlIn, i, total);
            oldVariable.push_back(variable);
        }
    }
}

void ComponentCodePage::copyPortVariableDate(PVariable oldVariable, PVariable newVariable, bool isCtrlIn, int index,
                                             int total)
{
    oldVariable->setName(newVariable->getName());
    oldVariable->setPortType(isCtrlIn ? Variable::ControlIn : Variable::ControlOut);
    oldVariable->setOrder(index);
    oldVariable->setDisplayName(newVariable->getDisplayName());
    oldVariable->setDataType(newVariable->getDataType());
    oldVariable->setShowMode(newVariable->getData(Global::isShowPortName).toBool() ? Variable::VisiableReadOnly
                                                                                   : Variable::Invisiable);
    if (total < 5) {
        int rownum = total < 5 ? (total - 1) : 4;
        oldVariable->setPresetPosition(QPoint(isCtrlIn ? 0 : 10, Global::portPosDefine[rownum][index]));
    } else {
        oldVariable->setPresetPosition(QPoint(isCtrlIn ? 0 : 10, 10.0 / (total + 1) * (index + 1)));
    }
}

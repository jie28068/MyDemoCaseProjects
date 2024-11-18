#include "blockpropertycontrol.h"
#include "PropertyServerMng.h"
#include <QDebug>
#include <qpixmap.h>

// 注册及请求服务所需要的头文件
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("ControlBlockPropertyEditor")

ControlBlockPropertyEditor::ControlBlockPropertyEditor(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                       CMA::Operation ope, bool isReadOnly, bool canChangeType)
    : BlockProperty(model, isReadOnly),
      m_pCacheInitModel(nullptr),
      m_pTypeSelectWidget(nullptr),
      m_pCodeWidget(nullptr),
      m_pVariableWidget(nullptr),
      m_pDisStateVarWidget(nullptr),
      m_pConStateVarWidget(nullptr),
      m_pOutputVarWidget(nullptr),
      m_pInputVarWidget(nullptr),
      m_pParamWidget(nullptr),
      m_resuleSaveWidget(nullptr),
      m_pFMUPropertyWidget(nullptr),
      m_Operation(ope),
      m_canChangeType(canChangeType),
      m_pSignalSelectWidget(nullptr)
{
}

ControlBlockPropertyEditor::~ControlBlockPropertyEditor() { }

void ControlBlockPropertyEditor::init()
{
    BlockProperty::init();
    refreshAllWidget(m_pModel);
}

QString ControlBlockPropertyEditor::getTitle()
{
    if (CMA::Operation_NewCtrInstance == m_Operation) {
        return CMA::TITLE_NEW_BLOCK_INSTANCE;
    } else if (CMA::Operation_EditSlotInstance == m_Operation) {
        return CMA::TITLE_EDIT_SLOT_INSTANCE;
    }

    return BlockProperty::getTitle();
}

QPixmap ControlBlockPropertyEditor::getBlockPixmap()
{
    if (PropertyServerMng::getInstance().m_ControlComponentServer == nullptr || m_pModel == nullptr) {
        return QPixmap();
    }
    return PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentIcon(
            m_pModel->getPrototypeName(), m_pModel->isInstance() ? BLOCKTYPE_INSTANCE : BLOCKTYPE_PROTOTYPE,
            CombineBoardModel::Type == m_pModel->getModelType());
    // QVariant picvalur = m_pModel->getResource().value(PNG_NORMAL_PIC, QPixmap());
    // if (picvalur.canConvert<QPixmap>()) {
    //     QPixmap pix = picvalur.value<QPixmap>();
    //     if (!pix.isNull()) {
    //         return pix;
    //     }
    // }

    // return drawTextIcon(m_pModel->getName(), m_pModel->isInstance(),
    //                     m_pModel->getModelType() == CombineBoardModel::Type);
}

CommonWrapper::LeftWidgetType ControlBlockPropertyEditor::getLeftWidgetType()
{
    if (CMA::Operation_NewCtrInstance == m_Operation || CMA::Operation_EditSlotInstance == m_Operation
        || CMA::Operation_EditCtrInstance == m_Operation) {
        return CommonWrapper::LeftWidgetType_BlockInstance;
    } else if (CMA::Operation_EditCtrProto == m_Operation) {
        return CommonWrapper::LeftWidgetType_BlockType;
    }
    return BlockProperty::getLeftWidgetType();
}

void ControlBlockPropertyEditor::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted && nullptr != m_pModel) {
        BlockProperty::onDialogExecuteResult(code);
        // TODO
        //  if (m_pModel->getPrototypeName() != m_pModel->getPrototypeName()) {
        //      m_pModel->swap(m_pModel);
        //      m_bPropertyIsChanged = true;
        //  }

        // if (setVarNameInBoard()) {
        //     m_bPropertyIsChanged = true;
        // }

        if (m_pDisStateVarWidget != nullptr
            && CMA::saveCustomModeListDataToModel(m_pModel, m_pDisStateVarWidget->getNewListData(),
                                                  RoleDataDefinition::DiscreteStateVariable)) {
            m_bPropertyIsChanged = true;
        }
        // #ifdef COMPILER_PRODUCT_DESIGNER
        if (m_pConStateVarWidget != nullptr
            && CMA::saveCustomModeListDataToModel(m_pModel, m_pConStateVarWidget->getNewListData(),
                                                  RoleDataDefinition::ContinueStateVariable)) {
            m_bPropertyIsChanged = true;
        }
        // #endif
        if (m_pOutputVarWidget != nullptr
            && CMA::saveCustomModeListDataToModel(m_pModel, m_pOutputVarWidget->getNewListData(),
                                                  RoleDataDefinition::OutputSignal)) {
            m_bPropertyIsChanged = true;
        }
        if (m_pParamWidget != nullptr
            && CMA::saveCustomModeListDataToModel(m_pModel, m_pParamWidget->getNewListData(),
                                                  RoleDataDefinition::Parameter)) {
            m_bPropertyIsChanged = true;
            updateDirectFeedThrough(m_pModel);
        }

        if (m_pTypeSelectWidget != nullptr && m_pTypeSelectWidget->saveData(m_pModel)) {
            m_bPropertyIsChanged = true;
        }
        if (m_resuleSaveWidget != nullptr && m_resuleSaveWidget->saveData(m_pModel)) {
            m_bPropertyIsChanged = true;
        }
        if (m_pVariableWidget != nullptr && m_pVariableWidget->saveData(m_pModel)) {
            m_bPropertyIsChanged = true;
        }
        if (m_pFMUPropertyWidget != nullptr && m_pFMUPropertyWidget->saveData(m_pModel)) {
            m_bPropertyIsChanged = true;
        }
        if (m_pSignalSelectWidget != nullptr && m_pSignalSelectWidget->saveData(m_pModel)) {
            m_bPropertyIsChanged = true;
        }
        if (m_bPropertyIsChanged) {
            m_pModel->setModifyTime(QDateTime::currentDateTime());
            CMA::saveModel(m_pModel);
        }
    } else {
        if (m_pFMUPropertyWidget != nullptr) {
            restoreBlockGroupData(m_pCacheInitModel);
        }

        if (CMA::Operation_NewCtrInstance == m_Operation) {
            CMA::destoryInstance(m_pModel);
            CMA::destoryInstance(m_pModel);
            m_pModel = nullptr;
        }

        m_bPropertyIsChanged = false;
    }
}

QString ControlBlockPropertyEditor::getHelpUrl()
{
    return m_pModel->getPrototypeName();
}

QSharedPointer<Kcc::BlockDefinition::Model> ControlBlockPropertyEditor::getCurrentModel()
{
    return m_pModel;
}

void ControlBlockPropertyEditor::onFMUBlockChanged(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    BlockProperty::init();
    refreshAllWidget(model, false);
    emit viewChanged();
}

void ControlBlockPropertyEditor::onBlockTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    m_pModel = model;
    this->init();
    emit viewChanged();
}

void ControlBlockPropertyEditor::refreshAllWidget(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                  const bool &isInitial)
{
    // 模块类型变化的时候更新所有widget
    if (nullptr == model && m_Operation != CMA::Operation_NewCtrInstance) {
        return;
    }
    if (nullptr == m_pTypeSelectWidget) {
        m_pTypeSelectWidget = new BlockTypeSelect(model, m_Operation, m_canChangeType);
        connect(m_pTypeSelectWidget, SIGNAL(blockTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model>)), this,
                SLOT(onBlockTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model>)));
    }
    addWidget(m_pTypeSelectWidget, CMA::BLOCK_TAB_NAME_TYPE);

    addCodeTextWidget();
    // 复合模型实例和构造型模块中暂不显示变量窗口
    if (canAddVarWidget(model)) {
        if (m_pVariableWidget == nullptr) {
            m_pVariableWidget = new VariableWidget(model);
        }
        addWidget(m_pVariableWidget, CMA::BLOCK_TAB_NAME_VARIABLE);
    } else {
        if (m_pVariableWidget != nullptr) {
            delete m_pVariableWidget;
            m_pVariableWidget = nullptr;
        }
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_VARIABLE);
    }
    // 加载显示FMU模块的窗口
    if (model != nullptr && model->getPrototypeName() == PROTOTYPENAME_FMU) {
        if (m_pFMUPropertyWidget == nullptr) {
            if (m_pModel != nullptr) {
                m_pCacheInitModel = m_pModel->copy();
            }
            m_pFMUPropertyWidget = new FMUBlockPropertyWidget(model.dynamicCast<ControlBlock>());
            connect(m_pFMUPropertyWidget, SIGNAL(onFMUBlockChanged(QSharedPointer<Kcc::BlockDefinition::Model>)), this,
                    SLOT(onFMUBlockChanged(QSharedPointer<Kcc::BlockDefinition::Model>)));
        }
        addWidget(m_pFMUPropertyWidget, CMA::BLOCK_TAB_NAME_FMU_OVERVIEW);
    } else {
        if (m_pFMUPropertyWidget != nullptr) {
            delete m_pFMUPropertyWidget;
            m_pFMUPropertyWidget = nullptr;
        }
    }

    // 状态
    refreshCustomTableWidget(m_pDisStateVarWidget, CMA::BLOCK_TAB_NAME_DISCRETE_STATE, model);
    // #ifdef COMPILER_PRODUCT_DESIGNER
    refreshCustomTableWidget(m_pConStateVarWidget, CMA::BLOCK_TAB_NAME_CONTINUE_STATE, model);
    // #endif

    // 输入
    refreshCustomTableWidget(m_pInputVarWidget, CMA::BLOCK_TAB_NAME_INPUT, model);

    // 输出
    refreshCustomTableWidget(m_pOutputVarWidget, CMA::BLOCK_TAB_NAME_OUTPUT, model);

    // 参数
    addParameterWidget(model);

    // 结果保存
    if (model != nullptr && blockCanCheckVar(model)) {
        if (m_resuleSaveWidget == nullptr) {
            m_resuleSaveWidget = new VarSelectWidget(Model::StateNormal == model->getState()
                                                     || Model::StateWarring == model->getState());
        }
        m_resuleSaveWidget->updateCheckedVariableInfo(model, "", isInitial);
        addWidget(m_resuleSaveWidget, CMA::BLOCK_TAB_NAME_ELEC_SAVE);
    } else {
        if (m_resuleSaveWidget != nullptr) {
            delete m_resuleSaveWidget;
            m_resuleSaveWidget = nullptr;
        }
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_ELEC_SAVE);
    }

    setPropertyEditableStatus(m_readOnly);
}

void ControlBlockPropertyEditor::refreshCustomTableWidget(CustomTableWidget *&pwidget, const QString &tabname,
                                                          QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    QList<CustomModelItem> listdata = getWidgetModelList(tabname, model);
    if (listdata.size() > 0) {
        if (pwidget == nullptr) {
            pwidget = new CustomTableWidget();
        }
        QStringList listheader;
        bool havepath = false;
        for (CustomModelItem item : listdata) {
            if (item.keywords.contains("/")) {
                havepath = true;
            }
        }
        if (havepath) {
            listheader = QStringList() << CMA::HEADER_PATH
                                       << (CMA::BLOCK_TAB_NAME_CTRL_PARAM == tabname ? CMA::HEADER_PARAMNAME
                                                                                     : CMA::HEADER_VARNAME)
                                       << (CMA::BLOCK_TAB_NAME_CTRL_PARAM == tabname ? CMA::HEADER_PARAMVALUE
                                                                                     : CMA::HEADER_VARVALUE)
                                       << CMA::HEADER_DESCRIPTION;
        } else {
            listheader = QStringList() << (CMA::BLOCK_TAB_NAME_CTRL_PARAM == tabname ? CMA::HEADER_PARAMNAME
                                                                                     : CMA::HEADER_VARNAME)
                                       << (CMA::BLOCK_TAB_NAME_CTRL_PARAM == tabname ? CMA::HEADER_PARAMVALUE
                                                                                     : CMA::HEADER_VARVALUE)
                                       << CMA::HEADER_DESCRIPTION;
        }
        pwidget->setListData(listdata, listheader);
        addWidget(pwidget, tabname);
    } else {
        delete pwidget;
        pwidget = nullptr;
        addWidget(nullptr, tabname);
    }
}

QList<CustomModelItem> ControlBlockPropertyEditor::getWidgetModelList(const QString &tabname,
                                                                      QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || tabname.isEmpty()) {
        return QList<CustomModelItem>();
    }

    if (CMA::BLOCK_TAB_NAME_INPUT == tabname) {
        return CMA::getPropertyModelItemList(model, RoleDataDefinition::InputSignal, true);
    } else if (CMA::BLOCK_TAB_NAME_OUTPUT == tabname) {
        return CMA::getPropertyModelItemList(model, RoleDataDefinition::OutputSignal);
    } else if (CMA::BLOCK_TAB_NAME_DISCRETE_STATE == tabname) {
        return CMA::getPropertyModelItemList(model, RoleDataDefinition::DiscreteStateVariable);
        // #ifdef COMPILER_PRODUCT_DESIGNER
    } else if (CMA::BLOCK_TAB_NAME_CONTINUE_STATE == tabname) {
        return CMA::getPropertyModelItemList(model, RoleDataDefinition::ContinueStateVariable);
        // #endif
    } else if (CMA::BLOCK_TAB_NAME_CTRL_PARAM == tabname) {
        return CMA::getPropertyModelItemList(model, RoleDataDefinition::Parameter);
    }
    return QList<CustomModelItem>();
}

void ControlBlockPropertyEditor::addParameterWidget(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr
        || (CMA::getPropertyModelItemList(model, RoleDataDefinition::Parameter).size() <= 0
            && (model->getPrototypeName() != NPS::PROTOTYPENAME_BUSSELECTOR))) {
        if (m_pSignalSelectWidget != nullptr) {
            delete m_pSignalSelectWidget;
            m_pSignalSelectWidget = nullptr;
        }
        if (m_pParamWidget != nullptr) {
            delete m_pParamWidget;
            m_pParamWidget = nullptr;
        }
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_CTRL_PARAM);
        return;
    }
    if (model->getPrototypeName() == NPS::PROTOTYPENAME_BUSSELECTOR) {
        if (m_pSignalSelectWidget == nullptr) {
            m_pSignalSelectWidget =
                    new SignalSelectWidget(model, (model->isInstance() || NPS::isCombineBlockSubModel(model)));
        }
        addWidget(m_pSignalSelectWidget, CMA::BLOCK_TAB_NAME_CTRL_PARAM);
    } else {
        if (m_pParamWidget == nullptr) {
            m_pParamWidget = new ControlParameterWidget(model);
        } else {
            m_pParamWidget->updateCustomTableWidget();
        }
        addWidget(m_pParamWidget, CMA::BLOCK_TAB_NAME_CTRL_PARAM);
    }
}

void ControlBlockPropertyEditor::addCodeTextWidget()
{
    if (m_pCodeWidget != nullptr) {
        delete m_pCodeWidget;
        m_pCodeWidget = nullptr;
    }
    PControlBlock pcblock(nullptr);
    if (m_pModel == nullptr || PropertyServerMng::getInstance().m_ModelManagerServer == nullptr) {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_CTRL_CODE);
        return;
    }
    pcblock = m_pModel.dynamicCast<ControlBlock>();
    if (ControlBlock::Type != m_pModel->getModelType() || pcblock == nullptr) {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_CTRL_CODE);
        return;
    }
    QString toolkit;

    // 代码
    if (!pcblock->getCodeText().isEmpty() && m_pModel->getPrototypeName() != PROTOTYPENAME_FMU
        && !PropertyServerMng::getInstance().m_ModelManagerServer->ContainToolkitModel(m_pModel->getPrototypeName(),
                                                                                       toolkit)) {
        if (m_pCodeWidget == nullptr && PropertyServerMng::getInstance().m_pCodeManagerSvr != nullptr) {
            // 这里需要动态转换指针
            m_pCodeWidget = PropertyServerMng::getInstance().m_pCodeManagerSvr->CreateCodeEditWidget();
        }
        if (m_pCodeWidget != nullptr) {
            ICodeEditWidget *pCodeEdit = dynamic_cast<ICodeEditWidget *>(m_pCodeWidget);
            if (pCodeEdit != nullptr) {
                // 只读
                pCodeEdit->setText(pcblock->getCodeText());
                pCodeEdit->setReadOnly(true);
                pCodeEdit->setToolBarVisible(false);
            }
            addWidget(m_pCodeWidget, CMA::BLOCK_TAB_NAME_CTRL_CODE);
        } else {
            addWidget(nullptr, CMA::BLOCK_TAB_NAME_CTRL_CODE);
        }
        return;
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_CTRL_CODE);
    }
}

bool ControlBlockPropertyEditor::blockCanCheckVar(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }
    if (CMA::Operation_NewCtrInstance == m_Operation) {
        return false;
    }
    PModel parentModel = model;
    while (parentModel->getParentModel() != nullptr) {
        parentModel = parentModel->getParentModel();
    }
    if (CombineBoardModel::Type == parentModel->getModelType()) {
        if (parentModel->isInstance() && model->getConnectedModel(ModelConnSlot).size() > 0) {
            return true;
        } else {
            return false;
        }
    } else if (ControlBlock::Type == parentModel->getModelType() && parentModel->isInstance()) {
        if (parentModel->getConnectedModel(ModelConnSlot).size() > 0) {
            return true;
        } else {
            return false;
        }
    } else if (ControlBoardModel::Type == parentModel->getModelType()) {
        return true;
    }
    return false;
}

bool ControlBlockPropertyEditor::canAddVarWidget(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    // 复合模型实例和构造型模块中暂不显示变量窗口
    if (model == nullptr || model->isInstance() || model->getModelType() == CombineBoardModel::Type) {
        return false;
    }
    QList<PVariable> varlist = CMA::getVarGroupList(model, RoleDataDefinition::Parameter)
            + CMA::getVarGroupList(model, RoleDataDefinition::DiscreteStateVariable)
            + CMA::getVarGroupList(model, RoleDataDefinition::ContinueStateVariable);
    if (varlist.size() > 0) {
        return true;
    }
    return false;
}

void ControlBlockPropertyEditor::restoreBlockGroupData(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    // 还原VariableGroup
    for (auto &newVariableGroup : m_pModel->getVariableGroupList()) {
        auto variableGroup = model->getVariableGroup(newVariableGroup->getGroupType());
        if (variableGroup) {
            newVariableGroup->copyFromGroup(variableGroup);
        } else {
            newVariableGroup->clearVariableMap();
        }
    }
}

void ControlBlockPropertyEditor::updateDirectFeedThrough(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || model->isInstance()) {
        return;
    }
    if (NPS::TRANSFUNC_PROTOTYPENAMES.contains(model->getPrototypeName())) {
        QString numStr = model->getValueByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                   NPS::KEYWORD_TRANS_NUM)
                                 .toString();
        QString denStr = model->getValueByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                   NPS::KEYWORD_TRANS_DEN)
                                 .toString();
        bool isFeedThrough = false;
        if (getMaxPower(numStr) == getMaxPower(denStr)) {
            isFeedThrough = true;
        }
        if ((model->getDirectFeedThrough() && !isFeedThrough) || (!model->getDirectFeedThrough() && isFeedThrough)) {
            model->setDirectFeedThrough(isFeedThrough);
        }
        return;
    } else if (NPS::PROTOTYPENAME_STATESPACE == model->getPrototypeName()) {
        QStringList numStrList = model->getValueByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                           NPS::KEYWORD_STATESPACE_D)
                                         .toString()
                                         .split(',');
        bool isFeedThrough = false;
        for (int i = 0; i < numStrList.size(); ++i) {
            if (!CMA::isEqualO(numStrList[i].toDouble())) {
                isFeedThrough = true;
                break;
            }
        }
        if (model->getDirectFeedThrough() != isFeedThrough) {
            model->setDirectFeedThrough(isFeedThrough);
        }
    } else if (CombineBoardModel::Type == model->getModelType()) {
        for (PModel cmodel : model->getChildModels().values()) {
            updateDirectFeedThrough(cmodel);
        }
    }
}

int ControlBlockPropertyEditor::getMaxPower(const QString &str)
{
    if (str.isEmpty()) {
        return 0;
    }
    QStringList noStrList = str.split(',');
    for (int i = 0; i < noStrList.size(); ++i) {
        if (!CMA::isEqualO(noStrList[i].toDouble())) {
            return noStrList.size() - 1 - i;
        }
    }
    return 0;
}

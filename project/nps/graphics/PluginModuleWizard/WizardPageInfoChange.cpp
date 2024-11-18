#include "WizardPageInfoChange.h"

void ComponentInfoPageOnModify::initializePage()
{
    setSubTitle(QObject::tr("Fill in module information, only modify module attributes."));
    if (getIsDevice()) { //*设备类型*//
        getNmaeLineEdit()->setEnabled(false);
        setModelComboBox(false);
        setRelevanceElecComboBox(false);
        setField("nameValue", deviceBlock->getPrototypeName());
        // 赋值给临时模块
        temporaryDeviceModuleAssignment();
        PVariableGroup deviceGroup = deviceBlock->getDeviceTypeVariableGroup();
        if (deviceGroup) {
            auto deviceMap = deviceGroup->getVariableSortByOrder();
            QStringList strlist = initParamList(deviceMap, RoleDataDefinition::DeviceTypeParameter);
            setField("paramValue", strlist.join(","));
        }
        // 简述
        setField("nameCHSValue", deviceBlock->getPrototypeName_CHS());
        // 简写
        setField("nameReadValue", deviceBlock->getPrototypeName_Readable());
        // 说明
        setField("explainValue", deviceBlock->getDescription());
        setRelevanceElecComboBox(deviceBlock->getDeviceTypePrototypeName());
        return;
    } else if (getIsElec()) { //*电气*//
        getNmaeLineEdit()->setEnabled(false);
        setModelComboBox(false);
        setField("nameValue", elecBlock->getPrototypeName());
        if (getNameCHSLineEdit()->text().isEmpty()) {
            setTips("");
        }
        if (!WizardServerMng::getInstance().m_pModelServer) {
            setTips(tr("Local parameter error"));
            return;
        }
        if (!WizardServerMng::getInstance().m_pModelServer->ContainToolkitModel(
                    elecBlock->getPrototypeName(), const_cast<QString &>(KL_TOOLKIT::ELECTRICAL_TOOLKIT))) {
            setTips(tr("Unable to get information about module [%1]").arg(elecBlock->getPrototypeName()));
            setControlIde(false);
            return;
        }
        // 赋值给临时模块
        temporaryElecModuleAssignment();
        //  各参数列表
        setElecModuleType(m_pElecBloc);
    } else {                                                //*控制*//
        setField("name", controlBlock->getPrototypeName()); // 设置模块名
        if (!WizardServerMng::getInstance().m_pModelServer) {
            setTips(tr("Local parameter error"));
            return;
        }
        // 赋值给临时模块
        temporaryControlModuleAssignment();
        setBlockEditable(false);

        QStringList inputList;
        QStringList outputList;
        QStringList paramList;
        QStringList interList;
        QStringList disStateList;
        QStringList conStateList;
        if (controlBlock->getInputVariableGroup()) {
            inputList = keyReorder(controlBlock->getInputVariableGroup()->getVariableMapNames(), controlBlock,
                                   _inputVariables);
        }
        if (controlBlock->getOutputVariableGroup()) {
            outputList = keyReorder(controlBlock->getOutputVariableGroup()->getVariableMapNames(), controlBlock,
                                    _outputVariables);
        }
        if (controlBlock->getControlVariableGroup()) {
            paramList = keyReorder(controlBlock->getControlVariableGroup()->getVariableMapNames(), controlBlock,
                                   _parameters);
        }
        if (controlBlock->getInternalVariableGroup()) {
            interList = keyReorder(controlBlock->getInternalVariableGroup()->getVariableMapNames(), controlBlock,
                                   _InternalVariables);
        }
        if (controlBlock->getDiscreteStateVariableGroup()) {
            disStateList = keyReorder(controlBlock->getDiscreteStateVariableGroup()->getVariableMapNames(),
                                      controlBlock, _discreteStateVariables);
        }
        // #ifdef COMPILER_PRODUCT_DESIGNER
        PVariableGroup pConStateGroup = controlBlock->getContinueStateVariableGroup();
        if (pConStateGroup) {
            conStateList = keyReorder(pConStateGroup->getVariableMapNames(), controlBlock, _continueStateVariables);
        }
        // #endif

        setField("template", controlBlock->getTemplates().isEmpty() ? "" : controlBlock->getTemplates()[0]);
        setField("display_name", controlBlock->getPrototypeName_CHS());
        setField("simple_name", controlBlock->getPrototypeName_Readable());
        setField("description", controlBlock->getDescription());
        setField("var_input", inputList.join(","));
        setField("var_output", outputList.join(","));

        setField("var_disState", disStateList.join(","));
        setField("var_conState", conStateList.join(","));
        setField("var_inner", interList.join(","));
        setField("var_param", paramList.join(","));
        setField("model_priority", controlBlock->getPriority());
        setField("m_DirectFeedThrough", controlBlock->getDirectFeedThrough());
    }
}

QStringList ComponentInfoPageOnModify::keyReorder(QStringList keylist, PControlBlock block, type _type)
{
    keylist.removeAll("");
    if (_type == type::_inputVariables) {
        for (const QString &str : keylist) {
            indexsortList << indexsort(str,
                                       block->getInputVariableGroup()
                                               ->findVariable(RoleDataDefinition::NameRole, str)
                                               .first()
                                               ->getOrder());
        }
    } else if (_type == type::_outputVariables) {
        for (const QString &str : keylist) {
            indexsortList << indexsort(str,
                                       block->getOutputVariableGroup()
                                               ->findVariable(RoleDataDefinition::NameRole, str)
                                               .first()
                                               ->getOrder());
        }
    } else if (_type == type::_parameters) {
        for (const QString &str : keylist) {
            indexsortList << indexsort(str,
                                       block->getControlVariableGroup()
                                               ->findVariable(RoleDataDefinition::NameRole, str)
                                               .first()
                                               ->getOrder());
        }
    } else if (_type == type::_InternalVariables) {
        for (const QString &str : keylist) {
            indexsortList << indexsort(str,
                                       block->getInternalVariableGroup()
                                               ->findVariable(RoleDataDefinition::NameRole, str)
                                               .first()
                                               ->getOrder());
        }
    } else if (_type == type::_discreteStateVariables) {
        for (const QString &str : keylist) {
            indexsortList << indexsort(str,
                                       block->getDiscreteStateVariableGroup()
                                               ->findVariable(RoleDataDefinition::NameRole, str)
                                               .first()
                                               ->getOrder());
        }
    } else if (_type == type::_continueStateVariables) {
        for (const QString &str : keylist) {
            indexsortList << indexsort(str,
                                       block->getContinueStateVariableGroup()
                                               ->findVariable(RoleDataDefinition::NameRole, str)
                                               .first()
                                               ->getOrder());
        }
    }
    qSort(indexsortList.begin(), indexsortList.end(),
          [](const indexsort &sort1, const indexsort &sort2) { return sort1.index < sort2.index; });

    QStringList _keyList;
    for (indexsort _struct : indexsortList) {
        _keyList << _struct.key;
    }
    indexsortList.clear();
    return _keyList;
}

bool ComponentInfoPageOnModify::isValidName(const QString &strName) const
{
    // 暂时屏蔽 分离插件时添加
    //  return m_strOldBlockName == strName ||
    //  !m_pPluginComponentServer->GetExistBlockNames().contains(strName);
    return true; // 分离时去掉
}

void ComponentInfoPageOnModify::temporaryControlModuleAssignment()
{
    // 临时模块赋值
    m_pBlock->setName(controlBlock->getName());
    m_pBlock->setPrototypeName_CHS(controlBlock->getPrototypeName_CHS());           // 中文名
    m_pBlock->setPrototypeName(controlBlock->getPrototypeName());                   // 原型名
    m_pBlock->setPrototypeName_Readable(controlBlock->getPrototypeName_Readable()); // 简称
    m_pBlock->setDescription(controlBlock->getDescription());                       // 说明
    m_pBlock->setResource(controlBlock->getResource());                             // 图片资源
    m_pBlock->setCodeText(controlBlock->getCodeText());                             // 代码段
    m_pBlock->setPriority(controlBlock->getPriority());                             // 优先级
    m_pBlock->setDirectFeedThrough(controlBlock->getDirectFeedThrough());           // 直接馈通
    //  main组
    PVariableGroup mainGroup = controlBlock->getVariableGroup(RoleDataDefinition::MainGroup);
    if (mainGroup) {
        if (mainGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::MainGroup)->copyFromGroup(mainGroup);
        }
    }
    // 输入变量
    PVariableGroup inputGroup = controlBlock->getVariableGroup(RoleDataDefinition::InputSignal);
    if (inputGroup) {
        if (inputGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::InputSignal)->copyFromGroup(inputGroup);
        }
    }
    // 输出变量
    PVariableGroup outputGroup = controlBlock->getVariableGroup(RoleDataDefinition::OutputSignal);
    if (outputGroup) {
        if (outputGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::OutputSignal)->copyFromGroup(outputGroup);
        }
    }
    // 状态变量
    PVariableGroup disStateGroup = controlBlock->getVariableGroup(RoleDataDefinition::DiscreteStateVariable);
    if (disStateGroup) {
        if (disStateGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::DiscreteStateVariable)->copyFromGroup(disStateGroup);
        }
    }
    PVariableGroup conStateGroup = controlBlock->getVariableGroup(RoleDataDefinition::ContinueStateVariable);
    if (conStateGroup) {
        if (conStateGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::ContinueStateVariable)->copyFromGroup(conStateGroup);
        }
    }
    // 内部变量
    PVariableGroup internalGroup = controlBlock->getVariableGroup(RoleDataDefinition::InternalVariable);
    if (internalGroup) {
        if (internalGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::InternalVariable)->copyFromGroup(internalGroup);
        }
    }
    // 模块参数
    PVariableGroup paramGroup = controlBlock->getVariableGroup(RoleDataDefinition::Parameter);
    if (paramGroup) {
        if (paramGroup->getVariableMapNames().size() > 0) {
            m_pBlock->createVariableGroup(RoleDataDefinition::Parameter)->copyFromGroup(paramGroup);
        }
    }
    // end
}

void ComponentInfoPageOnModify::temporaryDeviceModuleAssignment()
{
    if (deviceBlock) {
        m_pDevice->setName(deviceBlock->getName());
        m_pDevice->setPrototypeName(deviceBlock->getPrototypeName());
        m_pDevice->setDeviceTypePrototypeName(deviceBlock->getDeviceTypePrototypeName());
        m_pDevice->setPrototypeName_CHS(deviceBlock->getPrototypeName_CHS());
        m_pDevice->setPrototypeName_Readable(deviceBlock->getPrototypeName_Readable());
        m_pDevice->setDescription(deviceBlock->getDescription());
        PVariableGroup deviceGroup = deviceBlock->getVariableGroup(RoleDataDefinition::DeviceTypeParameter);
        if (deviceGroup) {
            if (deviceGroup->getVariableMapNames().size() > 0) {
                m_pDevice->createVariableGroup(RoleDataDefinition::DeviceTypeParameter)->copyFromGroup(deviceGroup);
            }
        }
        PVariableGroup mainGroup = deviceBlock->getVariableGroup(RoleDataDefinition::MainGroup);
        if (mainGroup) {
            if (mainGroup->getVariableMapNames().size() > 0) {
                m_pDevice->createVariableGroup(RoleDataDefinition::MainGroup)->copyFromGroup(mainGroup);
            }
        }
    }
}

void ComponentInfoPageOnModify::temporaryElecModuleAssignment()
{
    // 临时模块赋值
    if (elecBlock) {
        // 设置初值
        m_pElecBloc->setName(elecBlock->getName());                                     // 名称
        m_pElecBloc->setPrototypeName_CHS(elecBlock->getPrototypeName_CHS());           // 中文名
        m_pElecBloc->setPrototypeName_Readable(elecBlock->getPrototypeName_Readable()); // 简称
        m_pElecBloc->setDescription(elecBlock->getDescription());                       // 描述
        // m_pElecBloc->setClassification(elecBlock->getClassification());                 // 元器件分类
        m_pElecBloc->setPhaseNumber(elecBlock->getPhaseNumber()); // 相数
        m_pElecBloc->setResource(elecBlock->getResource());       // 图片
        //  main组
        PVariableGroup mainGroup = elecBlock->getVariableGroup(RoleDataDefinition::MainGroup);
        if (mainGroup) {
            if (mainGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::MainGroup)->copyFromGroup(mainGroup);
            }
        }
        // 端口
        PVariableGroup portGroup = elecBlock->getVariableGroup(RoleDataDefinition::PortGroup);
        if (portGroup) {
            if (portGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::PortGroup)->copyFromGroup(portGroup);
            }
        }
        // 电气
        PVariableGroup elecGroup = elecBlock->getVariableGroup(RoleDataDefinition::ElectricalParameter);
        if (elecGroup) {
            if (elecGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::ElectricalParameter)->copyFromGroup(elecGroup);
            }
        }
        // 潮流参数
        PVariableGroup loadGroup = elecBlock->getVariableGroup(RoleDataDefinition::LoadFlowParameter);
        if (loadGroup) {
            if (loadGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::LoadFlowParameter)->copyFromGroup(loadGroup);
            }
        }
        // 潮流结果
        PVariableGroup loadResultGroup = elecBlock->getVariableGroup(RoleDataDefinition::LoadFlowResultParameter);
        if (loadResultGroup) {
            if (loadResultGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::LoadFlowResultParameter)
                        ->copyFromGroup(loadResultGroup);
            }
        }
        // 仿真
        PVariableGroup simulationGroup = elecBlock->getVariableGroup(RoleDataDefinition::SimulationParameter);
        if (simulationGroup) {
            if (simulationGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::SimulationParameter)
                        ->copyFromGroup(simulationGroup);
            }
        }
        // 初始值
        PVariableGroup initGroup = elecBlock->getVariableGroup(RoleDataDefinition::InitializationParameter);
        if (initGroup) {
            if (initGroup->getVariableMapNames().size() > 0) {
                m_pElecBloc->createVariableGroup(RoleDataDefinition::InitializationParameter)->copyFromGroup(initGroup);
            }
        }
        // end
    }
}

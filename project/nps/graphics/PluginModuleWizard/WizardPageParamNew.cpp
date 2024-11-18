#include "WizardPageParamNew.h"

ComponentParamPage::ComponentParamPage(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent /*= nullptr*/)
    : WizardPageBase(pBlock, npBlock, parent),
      isElec(false),
      isTemplate(false),
      mElectricalGroup(new VariableGroup),
      mLoadFlowGroup(new VariableGroup),
      mSimulationGroup(new VariableGroup),
      mInitializationGroup(new VariableGroup),
      mDeviceGroup(new VariableGroup),
      isDevice(false)
{
    // 创建消息栏
    messageLabel = new QLabel;
    WizardTableProxyModel *pProxyModel = new WizardTableProxyModel(this);
    pProxyModel->setSourceModel(&m_model);
    tableView = new MyTableView(messageLabel, pProxyModel, this);

    tableView->setItemDelegate(new ComboxDelegate(this));
    tableView->sortByColumn(WizardTableModel::ParamCategory, Qt::AscendingOrder);

    setSubTitle(tr("If you need to change the number of variables, go back to the previous page."));
    Init(pProxyModel, m_model.getHeaderNames());
    connect(tableView->itemDelegate(), SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)), this,
            SIGNAL(completeChanged()));
    connect(tableView, &QTableView::pressed, this, &ComponentParamPage::onActivated);
}

ComponentParamPage::ComponentParamPage(PElectricalBlock pElecBloc, PElectricalBlock npElecBloc, QWidget *parent)
    : WizardPageBase(pElecBloc, npElecBloc, parent),
      isElec(true),
      isTemplate(false),
      mElectricalGroup(new VariableGroup),
      mLoadFlowGroup(new VariableGroup),
      mSimulationGroup(new VariableGroup),
      mInitializationGroup(new VariableGroup),
      mDeviceGroup(new VariableGroup),
      isDevice(false)
{
    // 创建消息栏
    messageLabel = new QLabel;
    // 创建表格
    ElecWizardTableProxyModel *pElecModel = new ElecWizardTableProxyModel(this);
    pElecModel->setSourceModel(&m_elecModel);
    tableView = new MyTableView(messageLabel, pElecModel, this);
    // 设置委托
    tableView->setItemDelegate(new ElecComboxDelegate(this));
    // 升序
    tableView->sortByColumn(ElecWizardTableModel::ParamClassify, Qt::AscendingOrder);
    // 布局
    setSubTitle(tr("If you need to change the number of variables, go back to the previous page."));
    Init(pElecModel, m_elecModel.getHeaderNames());
}
ComponentParamPage::ComponentParamPage(PDeviceModel pDeviceModel, PDeviceModel npDeviceModel, QWidget *parent)
    : WizardPageBase(pDeviceModel, npDeviceModel, parent),
      isElec(false),
      isTemplate(false),
      mElectricalGroup(new VariableGroup),
      mLoadFlowGroup(new VariableGroup),
      mSimulationGroup(new VariableGroup),
      mInitializationGroup(new VariableGroup),
      mDeviceGroup(new VariableGroup),
      isDevice(true)
{
    // 创建消息栏
    messageLabel = new QLabel;
    DveiceTableProxyModel *pDveiceModel = new DveiceTableProxyModel(this);
    pDveiceModel->setSourceModel(&m_deviceModel);
    tableView = new MyTableView(messageLabel, pDveiceModel, this);
    tableView->setItemDelegate(new DeviceComboxDelegate(this));

    // 布局
    setSubTitle(tr("If you need to change the number of variables, go back to the previous page."));
    Init(pDveiceModel, m_deviceModel.getHeaderNames());
}

void ComponentParamPage::initializePage()
{
    if (isDevice) {
        pageDevice();
    } else if (isElec) {
        pageElecInit();
    } else {
        pageInit();
    }
    // 刷新表格
    if (tableView) {
        tableView->updateFrozenTableGeometry();
    }
}

void ComponentParamPage::getPageInfo()
{
    QStringList inParam = field("var_input").toString().split(",", QString::SkipEmptyParts);
    QStringList outParam = field("var_output").toString().split(",", QString::SkipEmptyParts);
    QStringList disStatusParam = field("var_disState").toString().split(",", QString::SkipEmptyParts);
    QStringList conStatusParam = field("var_conState").toString().split(",", QString::SkipEmptyParts);
    QStringList innerParam = field("var_inner").toString().split(",", QString::SkipEmptyParts);
    QStringList param = field("var_param").toString().split(",", QString::SkipEmptyParts);
    // order序号组
    int inParamOrder = 0;
    int outParamOrder = 0;
    int disStatusParamOrder = 0;
    int conStatusParamOrder = 0;
    int innerParamOrder = 0;
    int paramOrder = 0;

    for (const QString &key : inParam) {
        if (m_pBlock->getInputVariableGroup().isNull()) {
            break;
        }
        PVariable variable = m_pBlock->getInputVariableGroup()->getVariableByName(key);
        if (variable == nullptr) {
            variable = m_pBlock->getInputVariableGroup()->createVariable();
        }
        m_model.initVarible(variable, key, inParamOrder++);
    }
    for (const QString &key : outParam) {
        if (m_pBlock->getOutputVariableGroup().isNull()) {
            break;
        }
        PVariable variable = m_pBlock->getOutputVariableGroup()->getVariableByName(key);
        if (variable == nullptr) {
            variable = m_pBlock->getOutputVariableGroup()->createVariable();
        }
        m_model.initVarible(variable, key, outParamOrder++);
    }
    for (const QString &key : disStatusParam) {
        if (m_pBlock->getDiscreteStateVariableGroup().isNull()) {
            break;
        }
        PVariable variable = m_pBlock->getDiscreteStateVariableGroup()->getVariableByName(key);
        if (variable == nullptr) {
            variable = m_pBlock->getDiscreteStateVariableGroup()->createVariable();
        }
        m_model.initVarible(variable, key, disStatusParamOrder++);
    }
    for (const QString &key : conStatusParam) {
        if (m_pBlock->getContinueStateVariableGroup().isNull()) {
            break;
        }
        PVariable variable = m_pBlock->getContinueStateVariableGroup()->getVariableByName(key);
        if (variable == nullptr) {
            variable = m_pBlock->getContinueStateVariableGroup()->createVariable();
        }
        m_model.initVarible(variable, key, conStatusParamOrder++);
    }
    for (const QString &key : innerParam) {
        if (m_pBlock->getInternalVariableGroup().isNull()) {
            break;
        }
        PVariable variable = m_pBlock->getInternalVariableGroup()->getVariableByName(key);
        if (variable == nullptr) {
            variable = m_pBlock->getInternalVariableGroup()->createVariable();
        }
        m_model.initVarible(variable, key, innerParamOrder++);
    }
    for (const QString &key : param) {
        if (m_pBlock->getControlVariableGroup().isNull()) {
            break;
        }
        PVariable variable = m_pBlock->getControlVariableGroup()->getVariableByName(key);
        if (variable == nullptr) {
            variable = m_pBlock->getControlVariableGroup()->createVariable();
        }
        m_model.initVarible(variable, key, paramOrder++);
    }
}

void ComponentParamPage::cleanupPage()
{
    // 返回上一页时缓存当前内容
    if (isDevice) {
    } else if (isElec) {
        saveElecParam();
    } else {
        getPageInfo();
    }
    // 刷新表格
    if (tableView) {
        tableView->updateFrozenTableGeometry();
    }
}

bool ComponentParamPage::validatePage()
{
    if (isDevice) {
        QStringList paramParam = field("paramValue").toString().split(",", QString::SkipEmptyParts);
        deleteMoreThanVariable(m_pDevice->getDeviceTypeVariableGroup(), paramParam);
        setValueDevice(paramParam);
        if (deviceBlock) {
            deviceBlock->setName(m_pDevice->getName());
            deviceBlock->setPrototypeName(m_pDevice->getPrototypeName());
            deviceBlock->setDeviceTypePrototypeName(m_pDevice->getDeviceTypePrototypeName());
            PVariableGroup deviceGroup = m_pDevice->getVariableGroup(RoleDataDefinition::DeviceTypeParameter);
            if (deviceGroup) {
                if (deviceGroup->getVariableMapNames().size() > 0) {
                    copyVariable(deviceGroup, RoleDataDefinition::DeviceTypeParameter, deviceBlock);
                } else {
                    deviceBlock->removeVariableGroup(RoleDataDefinition::DeviceTypeParameter);
                }
            }
            PVariableGroup mainGroup = m_pDevice->getVariableGroup(RoleDataDefinition::MainGroup);
            if (mainGroup) {
                if (mainGroup->getVariableMapNames().size() > 0) {
                    copyVariable(mainGroup, RoleDataDefinition::MainGroup, deviceBlock);
                } else {
                    auto group = deviceBlock->getVariableGroup(RoleDataDefinition::MainGroup);
                    if (group) {
                        group->clearVariableMap();
                    }
                }
            }
        }
    } else if (isElec) {
        saveElecParam();
        // 保存时将临时的模块赋值给要保存的模块
        saveTempElecModel(elecBlock);
    } else {
        // 生成参数信息
        getPageInfo();
    }
    return true;
}

bool ComponentParamPage::isComplete() const
{
    if (isDevice) {
        return true;
    } else if (isElec) {
        return true;
    } else {
        QStringList inParam = field("var_input").toString().split(",", QString::SkipEmptyParts);
        QStringList outParam = field("var_output").toString().split(",", QString::SkipEmptyParts);
        QStringList disStatusParam = field("var_disState").toString().split(",", QString::SkipEmptyParts);
        QStringList conStatusParam = field("var_conState").toString().split(",", QString::SkipEmptyParts);
        QStringList innerParam = field("var_inner").toString().split(",", QString::SkipEmptyParts);
        QStringList param = field("var_param").toString().split(",", QString::SkipEmptyParts);

        QModelIndex index = m_model.index(0, WizardTableModel::ParamType, QModelIndex());
        int totalsize = inParam.size() + outParam.size() + disStatusParam.size() + conStatusParam.size()
                + innerParam.size() + param.size();
        for (int i = 0; i < totalsize; ++i) {
            QString paramType = index.sibling(i, WizardTableModel::ParamType).data(Qt::EditRole).toString();
            QString variableName = index.sibling(i, WizardTableModel::Name).data(Qt::EditRole).toString();
            QString data = index.sibling(i, WizardTableModel::ParamValue).data(Qt::EditRole).toString();
            QString variableNameCHS = index.sibling(i, WizardTableModel::NameCHS).data(Qt::EditRole).toString();
            QString description = index.sibling(i, WizardTableModel::Description).data(Qt::EditRole).toString();
            if (paramType == Global::DataType_DoubleComplex || paramType == Global::DataType_Double
                || paramType == Global::DataType_DoubleVector) {
                if (!checkParamValue(variableName, data, paramType))
                    return false;
            }
            if (variableNameCHS.length() > (MAX_VARIABE_LENGTH)) {
                setTipsInfo(tr("Variable [%1] shows name [%2] longer than %3")
                                    .arg(variableName)
                                    .arg(variableNameCHS)
                                    .arg(MAX_VARIABE_LENGTH));
                return false;
            }
            if (description.length() > 1024) {
                setTipsInfo(tr("Variable description length is up to 1024 characters"));
                return false;
            }
        }

        setTipsInfo("");
        return true;
    }
}

void ComponentParamPage::setElecGroup(QList<PVariableGroup> lists)
{
    isTemplate = true;
    mElectricalGroup = lists[0];     // 电气组
    mLoadFlowGroup = lists[1];       // 潮流组
    mSimulationGroup = lists[2];     // 仿真组
    mInitializationGroup = lists[3]; // 初始值组
}

void ComponentParamPage::setDeviceGroup(PVariableGroup list)
{
    isTemplate = true;
    mDeviceGroup = list;
}

void ComponentParamPage::Init(CustomProxyModel *proxyModel, const QStringList &names)
{
    QGridLayout *layout = new QGridLayout;
    QLabel *labelColum = new QLabel(tr("Filtering Columns")); // 筛选列
    QComboBox *screenComboBox = new QComboBox;
    screenComboBox->addItems(names);
    QLabel *labelValue = new QLabel(tr("Filtering Values")); // 筛选值
    QLineEdit *screenLine = new QLineEdit;
    screenLine->setPlaceholderText(tr("The default is name search")); // 默认为名称搜索
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(labelColum);
    hlayout->addWidget(screenComboBox);
    hlayout->addWidget(labelValue);
    hlayout->addWidget(screenLine);
    layout->addLayout(hlayout, 0, 0);
    layout->addWidget(tableView, 1, 0, 1, 1);
    layout->addWidget(messageLabel, 2, 0, 1, 1);
    setLayout(layout);
    // 表格过滤文本
    connect(screenLine, &QLineEdit::textChanged, [=](const QString &str) { proxyModel->setFilterString(str); });
    // 表格过滤重新绘制第一列边框
    connect(screenLine, &QLineEdit::textChanged,
            [=](const QString &str) { tableView->updateFrozenTableGeometry(proxyModel->filteredRowCount); });
    // 表格过滤列
    connect(screenComboBox, &QComboBox::currentTextChanged,
            [=](const QString &str) { proxyModel->setFilterColumn(str, screenComboBox->currentIndex()); });
}

void ComponentParamPage::setControlGroup(PControlBlock controlBlock)
{
    if (controlBlock != nullptr) {
        m_pBlock->createVariableGroup(RoleDataDefinition::InputSignal)
                ->copyFromGroup(controlBlock->getInputVariableGroup());
        m_pBlock->createVariableGroup(RoleDataDefinition::OutputSignal)
                ->copyFromGroup(controlBlock->getOutputVariableGroup());
        m_pBlock->createVariableGroup(RoleDataDefinition::DiscreteStateVariable)
                ->copyFromGroup(controlBlock->getDiscreteStateVariableGroup());
        // #ifdef COMPILER_PRODUCT_DESIGNER
        m_pBlock->createVariableGroup(RoleDataDefinition::ContinueStateVariable)
                ->copyFromGroup(controlBlock->getContinueStateVariableGroup());
        // #endif
        m_pBlock->createVariableGroup(RoleDataDefinition::InternalVariable)
                ->copyFromGroup(controlBlock->getInternalVariableGroup());
        m_pBlock->createVariableGroup(RoleDataDefinition::Parameter)
                ->copyFromGroup(controlBlock->getControlVariableGroup());
    } else {
        m_pBlock->createVariableGroup(RoleDataDefinition::InputSignal);
        m_pBlock->createVariableGroup(RoleDataDefinition::OutputSignal);
        m_pBlock->createVariableGroup(RoleDataDefinition::DiscreteStateVariable);
        // #ifdef COMPILER_PRODUCT_DESIGNER
        m_pBlock->createVariableGroup(RoleDataDefinition::ContinueStateVariable);
        // #endif
        m_pBlock->createVariableGroup(RoleDataDefinition::InternalVariable);
        m_pBlock->createVariableGroup(RoleDataDefinition::Parameter);
    }
}

void ComponentParamPage::copyVariable(PVariableGroup groupValue, const QString &str, PModel model)
{
    auto group = model->createVariableGroup(str);
    for (auto &variable : groupValue->getVariableMap()) {
        auto var = group->getVariable(variable->getUUID());
        if (str != RoleDataDefinition::PortGroup) { // 非端口组，删除端口位置，文本，类型
            variable->detach();
            if (variable->hasRole(RoleDataDefinition::PortPresetPosition))
                variable->removeRole(RoleDataDefinition::PortPresetPosition);
            if (variable->hasRole(RoleDataDefinition::PortType))
                variable->removeRole(RoleDataDefinition::PortType);
            if (variable->hasRole(RoleDataDefinition::DescriptionRole))
                variable->removeRole(RoleDataDefinition::DescriptionRole);
        }
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

void ComponentParamPage::setVariableValue(PVariable variable, WizardTableModel::Category category,
                                          const QString &strName)
{
    QString controlTypeName;
    if (variable->getControlType().size() < 3) {
        controlTypeName = Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox);
    } else {
        controlTypeName = Global::ControlTyeName.key(variable->getControlType());
        if (controlTypeName == "") {
            controlTypeName = Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox);
        }
    }
    if (variable->getDataType() == "") {
        m_model.addRow(strName, category, Global::DataType_Double, "0",
                       variable->getDisplayName() == "" ? strName : variable->getDisplayName(),
                       variable->getData(Global::isShowPortName).toBool(), 0,
                       variable->getData(RoleDataDefinition::ShowModeRole).toInt(), controlTypeName,
                       variable->getControlValue(), variable->getDescription(), variable->getOrder());
    } else {
        m_model.addRow(strName, category, variable->getDataType(), variable->getDefaultValue(),
                       variable->getDisplayName() == "" ? strName : variable->getDisplayName(),
                       variable->getData(Global::isShowPortName).toBool(), variable->getLimitationType(),
                       variable->getData(RoleDataDefinition::ShowModeRole).toInt(), controlTypeName,
                       variable->getControlValue(), variable->getDescription(), variable->getOrder());
    }
}

void ComponentParamPage::pageInit(PControlBlock templateBlock)
{
    QStringList inParam = field("var_input").toString().split(",", QString::SkipEmptyParts);
    QStringList outParam = field("var_output").toString().split(",", QString::SkipEmptyParts);
    QStringList disStatusParam = field("var_disState").toString().split(",", QString::SkipEmptyParts);
    QStringList conStatusParam = field("var_conState").toString().split(",", QString::SkipEmptyParts);
    QStringList innerParam = field("var_inner").toString().split(",", QString::SkipEmptyParts);
    QStringList param = field("var_param").toString().split(",", QString::SkipEmptyParts);

    PControlBlock block;
    // 新增模块
    if (!m_pBlock->getTemplates().isEmpty() && templateBlock == nullptr) {
        // 获取模板的内容， 并将其参数合并过来
        block = qSharedPointerDynamicCast<ControlBlock>(WizardServerMng::getInstance().m_pModelServer->GetToolkitModel(
                m_pBlock->getTemplates().first(), KL_TOOLKIT::CONTROL_TOOLKIT));
        setControlGroup(block);
    } else if (m_pBlock->getTemplates().isEmpty() && templateBlock == nullptr) {
        setControlGroup(PControlBlock());
    }
    // 修改模块
    if (templateBlock != nullptr) {
        m_pBlock = templateBlock;
    }
    // 初始化
    setVariableNameByOrder(m_pBlock->getInputVariableGroup(), inParam, RoleDataDefinition::InputSignal);
    setVariableNameByOrder(m_pBlock->getOutputVariableGroup(), outParam, RoleDataDefinition::OutputSignal);
    setVariableNameByOrder(m_pBlock->getDiscreteStateVariableGroup(), disStatusParam,
                           RoleDataDefinition::DiscreteStateVariable);
    setVariableNameByOrder(m_pBlock->getContinueStateVariableGroup(), conStatusParam,
                           RoleDataDefinition::ContinueStateVariable);
    setVariableNameByOrder(m_pBlock->getInternalVariableGroup(), innerParam, RoleDataDefinition::InternalVariable);
    setVariableNameByOrder(m_pBlock->getControlVariableGroup(), param, RoleDataDefinition::Parameter);
    // 初始化表格内容，从模板中填充参数的值
    m_model.clear();
    m_model.setPrototypeName(field("name").toString());
    initModelData(WizardTableModel::Input, inParam);
    initModelData(WizardTableModel::Output, outParam);
    initModelData(WizardTableModel::DisState, disStatusParam);
    initModelData(WizardTableModel::ConState, conStatusParam);
    initModelData(WizardTableModel::Internal, innerParam);
    initModelData(WizardTableModel::Param, param);
}

void ComponentParamPage::initModelData(WizardTableModel::Category category, const QStringList &listVariableNames)
{
    auto GetVarNameFunc = [this, category](const PVariableGroup pGroup, const QString &strName) -> bool {
        if (!pGroup.isNull()) {
            if (pGroup->hasVariableName(strName)) {
                PVariable variable = pGroup->findVariable(RoleDataDefinition::NameRole, strName).first();
                setVariableValue(variable, category, strName);
                return true;
            }
        }
        return false;
    };
    foreach (const QString &strName, listVariableNames) {
        if (GetVarNameFunc(m_pBlock->getInputVariableGroup(), strName)) {
            continue;
        }
        if (GetVarNameFunc(m_pBlock->getOutputVariableGroup(), strName)) {
            continue;
        }
        if (GetVarNameFunc(m_pBlock->getControlVariableGroup(), strName)) {
            continue;
        }
        if (GetVarNameFunc(m_pBlock->getDiscreteStateVariableGroup(), strName)) {
            continue;
        }
        // #ifdef COMPILER_PRODUCT_DESIGNER
        if (GetVarNameFunc(m_pBlock->getContinueStateVariableGroup(), strName)) {
            continue;
        }
        // #endif
        if (GetVarNameFunc(m_pBlock->getInternalVariableGroup(), strName)) {
            continue;
        }

        m_model.addRow(strName, category, Global::DataType_Double, "0", "", false, 0, 1, 0, 0, "", 0);
    }
}

bool ComponentParamPage::checkParamValue(const QString &strName, const QString &strText, const QString &paramType) const
{
    int count = 0;
    foreach (const QString &str, strText.split(',', QString::SkipEmptyParts)) {
        ++count;
        if (str.length() > MAX_VARIABE_LENGTH) {
            setTipsInfo(tr("Variable [%1] median value [%2] is longer than %3")
                                .arg(strName)
                                .arg(str)
                                .arg(MAX_VARIABE_LENGTH));
            return false;
        }
    }
    if (paramType == Global::DataType_DoubleComplex && count != 2) {
        setTipsInfo(tr("The variable [%1] requires two inputs,as:1,2").arg(strName));
        return false;
    }
    return true;
}

void ComponentParamPage::setVariableNameByOrder(PVariableGroup group, QStringList nameList, QString groupName)
{ // 判断当前行是否有值，无值删除组
    if (nameList.size() == 0) {
        if (group) {
            m_pBlock->removeVariableGroup(groupName);
        }
    } else {
        if (!group) {
            group = m_pBlock->createVariableGroup(groupName);
        }
        auto orderVariables = group->getVariableSortByOrder();
        if (orderVariables.size() == nameList.size()) { // 变量数量不变时
            for (int i = 0; i < nameList.size(); ++i) {
                orderVariables[i]->setName(nameList[i]);
            }
        } else if (orderVariables.size() < nameList.size()) { // 变量数量变多时
            for (int i = 0; i < orderVariables.size(); ++i) {
                orderVariables[i]->setName(nameList[i]);
            }
            // 创建新的variable,并设置order
            for (int i = orderVariables.size(); i < nameList.size(); ++i) {
                auto variable = group->createVariable();
                variable->setName(nameList[i]);
                variable->setOrder(i);
            }
        } else { // 变量数量变少时
            for (int i = 0; i < nameList.size(); ++i) {
                orderVariables[i]->setName(nameList[i]);
            }
            // 删除多余的variable，按order
            for (int i = nameList.size(); i < orderVariables.size(); ++i) {
                auto var = group->findVariable(RoleDataDefinition::OrderRole, nameList.size());
                if (!var.isEmpty()) {
                    group->removeVariable(var.first());
                }
            }
        }
    }
}

void ComponentParamPage::pageElecInit(PElectricalBlock elecBlock)
{
    if (elecBlock != nullptr) {
        //  修改模块时
        m_pElecBloc = elecBlock;
        mElectricalGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::ElectricalParameter);
        if (mElectricalGroup && mElectricalGroup->getVariableMapNames().size() == 0) {
            m_pElecBloc->removeVariableGroup(RoleDataDefinition::ElectricalParameter);
        }
        mLoadFlowGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::LoadFlowParameter);
        if (mLoadFlowGroup && mLoadFlowGroup->getVariableMapNames().size() == 0) {
            m_pElecBloc->removeVariableGroup(RoleDataDefinition::LoadFlowParameter);
        }
        mLoadFlowResultGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::LoadFlowResultParameter);
        if (mLoadFlowResultGroup && mLoadFlowResultGroup->getVariableMapNames().size() == 0) {
            m_pElecBloc->removeVariableGroup(RoleDataDefinition::LoadFlowResultParameter);
        }
        mSimulationGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::SimulationParameter);
        if (mSimulationGroup && mSimulationGroup->getVariableMapNames().size() == 0) {
            m_pElecBloc->removeVariableGroup(RoleDataDefinition::SimulationParameter);
        }
        mInitializationGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::InitializationParameter);
        if (mInitializationGroup && mInitializationGroup->getVariableMapNames().size() == 0) {
            m_pElecBloc->removeVariableGroup(RoleDataDefinition::InitializationParameter);
        }
    }
    QStringList portParam = field("protValue").toString().split(",", QString::SkipEmptyParts);
    QStringList paramParam = field("paramValue").toString().split(",", QString::SkipEmptyParts);
    QStringList tideParam = field("tideValue").toString().split(",", QString::SkipEmptyParts);
    QStringList tideResultParam = field("tideResultValue").toString().split(",", QString::SkipEmptyParts);
    QStringList simuParam = field("simuValue").toString().split(",", QString::SkipEmptyParts);
    QStringList initParam = field("initValue").toString().split(",", QString::SkipEmptyParts);
    if (isTemplate) {
        // 从模板中获取参数
        isTemplate = false;
        m_pElecBloc->createVariableGroup(RoleDataDefinition::ElectricalParameter)->copyFromGroup(mElectricalGroup);
        m_pElecBloc->createVariableGroup(RoleDataDefinition::LoadFlowParameter)->copyFromGroup(mLoadFlowGroup);
        m_pElecBloc->createVariableGroup(RoleDataDefinition::LoadFlowResultParameter)
                ->copyFromGroup(mLoadFlowResultGroup);
        m_pElecBloc->createVariableGroup(RoleDataDefinition::SimulationParameter)->copyFromGroup(mSimulationGroup);
        m_pElecBloc->createVariableGroup(RoleDataDefinition::InitializationParameter)
                ->copyFromGroup(mInitializationGroup);
    } else {
        // 生成唯一组
        if ((portParam.size() != 0 || paramParam.size() != 0) && !m_pElecBloc->getElectricalVariableGroup()) {
            mElectricalGroup = m_pElecBloc->createVariableGroup(RoleDataDefinition::ElectricalParameter);
        }
        if (tideParam.size() != 0 && !m_pElecBloc->getLoadFlowVariableGroup()) {
            mLoadFlowGroup = m_pElecBloc->createVariableGroup(RoleDataDefinition::LoadFlowParameter);
        }
        if (tideResultParam.size() != 0 && !m_pElecBloc->getLoadFlowResultVariableGroup()) {
            mLoadFlowResultGroup = m_pElecBloc->createVariableGroup(RoleDataDefinition::LoadFlowResultParameter);
        }
        if (simuParam.size() != 0 && !m_pElecBloc->getSimulationVariableGroup()) {
            mSimulationGroup = m_pElecBloc->createVariableGroup(RoleDataDefinition::SimulationParameter);
        }
        if (initParam.size() != 0 && !m_pElecBloc->getInitializationVariableGroup()) {
            mInitializationGroup = m_pElecBloc->createVariableGroup(RoleDataDefinition::InitializationParameter);
        }
    }
    //*添加新变量*//
    int elecOrder = 0;       // 电气序号
    int tideOrder = 0;       // 潮流参数序号
    int tideResultOrder = 0; // 潮流结果序号
    int simuOrder = 0;       // 仿真序号
    int initOrder = 0;       // 初始值序号

    foreach (const QString &key, portParam) // 端口
    {
        if (m_pElecBloc->getPortManager()) {
            if (!m_pElecBloc->getPortManager()->getVariableByName(key)) {
                PVariable var = m_pElecBloc->getPortManager()->createVariable();
                var->setName(key);
            }
        }
    }

    foreach (const QString &key, portParam) // 电气 端口
    {
        if (!m_pElecBloc->getElectricalPVariable(key)) {
            PVariable var = m_pElecBloc->getElectricalVariableGroup()->createVariable();
            initElecVariable(key, var, ElecWizardTableModel::Category::PortParameter, elecOrder++);
        } else {
            initElecVariable(key, m_pElecBloc->getElectricalPVariable(key),
                             ElecWizardTableModel::Category::PortParameter, elecOrder++);
        }
    }
    foreach (const QString &key, paramParam) // 电气 参数
    {
        if (!m_pElecBloc->getElectricalPVariable(key)) {
            PVariable var = m_pElecBloc->getElectricalVariableGroup()->createVariable();
            initElecVariable(key, var, ElecWizardTableModel::Category::ElectricalParameter, elecOrder++);
        } else {
            initElecVariable(key, m_pElecBloc->getElectricalPVariable(key),
                             ElecWizardTableModel::Category::ElectricalParameter, elecOrder++);
        }
    }
    foreach (const QString &key, tideParam) // 潮流参数
    {
        if (!m_pElecBloc->getLoadFlowPVariable(key)) {
            PVariable var = m_pElecBloc->getLoadFlowVariableGroup()->createVariable();
            initElecVariable(key, var, ElecWizardTableModel::Category::loadFlowParameter, tideOrder++);
        } else {
            initElecVariable(key, m_pElecBloc->getLoadFlowPVariable(key),
                             ElecWizardTableModel::Category::loadFlowParameter, tideOrder++);
        }
    }
    foreach (const QString &key, tideResultParam) // 潮流结果
    {
        if (!m_pElecBloc->getLoadFlowResultPVariable(key)) {
            PVariable var = m_pElecBloc->getLoadFlowResultVariableGroup()->createVariable();
            initElecVariable(key, var, ElecWizardTableModel::Category::loadFlowResultParameter, tideResultOrder++);
        } else {
            initElecVariable(key, m_pElecBloc->getLoadFlowResultPVariable(key),
                             ElecWizardTableModel::Category::loadFlowResultParameter, tideResultOrder++);
        }
    }
    foreach (const QString &key, simuParam) // 仿真设置
    {
        if (!m_pElecBloc->getSimulationPVariable(key)) {
            PVariable var = m_pElecBloc->getSimulationVariableGroup()->createVariable();
            initElecVariable(key, var, ElecWizardTableModel::Category::simulationParameter, simuOrder++);
        } else {
            initElecVariable(key, m_pElecBloc->getSimulationPVariable(key),
                             ElecWizardTableModel::Category::simulationParameter, simuOrder++);
        }
    }
    foreach (const QString &key, initParam) // 初始值设置
    {
        if (!m_pElecBloc->getInitializationPVariable(key)) {
            PVariable var = m_pElecBloc->getInitializationVariableGroup()->createVariable();
            initElecVariable(key, var, ElecWizardTableModel::Category::initSimulationParameter, initOrder++);
        } else {
            initElecVariable(key, m_pElecBloc->getInitializationPVariable(key),
                             ElecWizardTableModel::Category::initSimulationParameter, initOrder++);
        }
    }
    // end
    //  初始化表格内容，从模板中填充参数的值
    m_elecModel.clear();
    initElecModelData(ElecWizardTableModel::PortParameter, portParam);
    initElecModelData(ElecWizardTableModel::ElectricalParameter, paramParam);
    initElecModelData(ElecWizardTableModel::loadFlowParameter, tideParam);
    initElecModelData(ElecWizardTableModel::loadFlowResultParameter, tideResultParam, true);
    initElecModelData(ElecWizardTableModel::simulationParameter, simuParam);
    initElecModelData(ElecWizardTableModel::initSimulationParameter, initParam);
}

void ComponentParamPage::initElecModelData(ElecWizardTableModel::Category category,
                                           const QStringList &listVariableNames, bool isResultTide)
{
    // 生成端口信息
    foreach (const QString &strName, listVariableNames) {
        PVariable portValue = m_pElecBloc->getPortManager()->getVariableByName(strName);
        PVariable elecValue = m_pElecBloc->getElectricalPVariable(strName);
        PVariable InitializationValue = m_pElecBloc->getInitializationPVariable(strName);
        PVariable imulationValue = m_pElecBloc->getSimulationPVariable(strName);
        PVariable LoadFlowValue = m_pElecBloc->getLoadFlowPVariable(strName);
        PVariable LoadFResultlowValue = m_pElecBloc->getLoadFlowResultPVariable(strName);
        // 电气 + 端口
        if (elecValue) {
            initElecAddRow(category, elecValue, false, portValue);
            continue;
        }
        // 潮流参数
        if (LoadFlowValue && !isResultTide) {
            initElecAddRow(category, LoadFlowValue);
            continue;
        }
        // 潮流结果
        if (LoadFResultlowValue && isResultTide) {
            initElecAddRow(category, LoadFResultlowValue, true);
            continue;
        }
        // 仿真
        if (imulationValue) {
            initElecAddRow(category, imulationValue);
            continue;
        }
        // 初始值
        if (InitializationValue) {
            initElecAddRow(category, InitializationValue);
            continue;
        }
        m_elecModel.addRow(strName, category, Global::DataType_Double, "", "", "0", 1, 1, "", "", "");
    }
}

void ComponentParamPage::saveElecParam()
{
    int portOrder = 0;       // 端口序号
    int elecOrder = 0;       // 电气序号
    int tideOrder = 0;       // 潮流参数序号
    int tideResultOrder = 0; // 潮流结果序号
    int simuOrder = 0;       // 仿真序号
    int initOrder = 0;       // 初始值序号
    QStringList portParam = field("protValue").toString().split(",", QString::SkipEmptyParts);
    QStringList paramParam = field("paramValue").toString().split(",", QString::SkipEmptyParts);
    paramParam.append(portParam);
    QStringList tideParam = field("tideValue").toString().split(",", QString::SkipEmptyParts);
    QStringList tideResultParam = field("tideResultValue").toString().split(",", QString::SkipEmptyParts);
    QStringList simuParam = field("simuValue").toString().split(",", QString::SkipEmptyParts);
    QStringList initParam = field("initValue").toString().split(",", QString::SkipEmptyParts);
    // 删除多余参数的组
    deleteMoreThanVariable(m_pElecBloc->getPortManager(), portParam);
    deleteMoreThanVariable(m_pElecBloc->getElectricalVariableGroup(), paramParam);
    deleteMoreThanVariable(m_pElecBloc->getLoadFlowVariableGroup(), tideParam);
    deleteMoreThanVariable(m_pElecBloc->getLoadFlowResultVariableGroup(), tideResultParam);
    deleteMoreThanVariable(m_pElecBloc->getSimulationVariableGroup(), simuParam);
    deleteMoreThanVariable(m_pElecBloc->getInitializationVariableGroup(), initParam);
    // end
    //**设置表格**//
    foreach (const QString &key, portParam) // 电气 端口
    {
        // 端口保存
        PVariableGroup portGroup;
        if (m_pElecBloc->getPortManager().isNull()) {
            portGroup = m_pElecBloc->createVariableGroup(RoleDataDefinition::PortGroup);
        } else {
            portGroup = m_pElecBloc->getPortManager();
        }
        PVariable portVariable = portGroup->getVariableByName(key);
        if (!portVariable) {
            portVariable = portGroup->createVariable();
        }
        portVariable->setName(key);
        // 端口类型
        int type = m_elecModel
                           .getParameter(key, ElecWizardTableModel::Category::PortParameter,
                                         ElecWizardTableModel::Column::ParamPortType)
                           .toInt();
        if (type == 0) {
            portVariable->setPortType(Variable::PortType::Electrical);
        } else if (type == 1) {
            portVariable->setPortType(Variable::PortType::Busbar);
        } else if (type == 2) {
            portVariable->setPortType(Variable::PortType::Electrical_AC);
        } else if (type == 3) {
            portVariable->setPortType(Variable::PortType::Electrical_DC);
        }
        // 端口位置
        QString str = m_elecModel
                              .getParameter(key, ElecWizardTableModel::Category::PortParameter,
                                            ElecWizardTableModel::Column::ParemPortPosition)
                              .toString();
        QStringList strList = str.split(",");
        double num1 = 0;
        double num2 = 0;
        if (strList.size() >= 2) {
            num1 = strList[0].remove(0, 1).toDouble();
            strList[1].chop(1);
            num2 = strList[1].toDouble();
        }

        portVariable->setPresetPosition(QPointF(num1, num2));
        // end
        portVariable->setOrder(portOrder);
        initElecVariable(key, portVariable, ElecWizardTableModel::Category::PortParameter, portOrder);
        portOrder++;
        elecOrder++;
    }
    foreach (const QString &key, paramParam) // 电气 参数
    {
        if (portParam.contains(key)) { // 将端口组中的数据给电气参数中的端口变量
            if (m_pElecBloc->getPortManager()) {
                m_pElecBloc->getElectricalPVariable(key)->copyFromVariable(
                        m_pElecBloc->getPortManager()->getVariableByName(key));
            }
        } else {
            initElecVariable(key, m_pElecBloc->getElectricalPVariable(key),
                             ElecWizardTableModel::Category::ElectricalParameter, elecOrder++);
        }
    }
    foreach (const QString &key, tideParam) // 潮流参数
    {
        initElecVariable(key, m_pElecBloc->getLoadFlowPVariable(key), ElecWizardTableModel::Category::loadFlowParameter,
                         tideOrder++);
    }
    foreach (const QString &key, tideResultParam) // 潮流结果
    {
        initElecVariable(key, m_pElecBloc->getLoadFlowResultPVariable(key),
                         ElecWizardTableModel::Category::loadFlowResultParameter, tideResultOrder++);
    }
    foreach (const QString &key, simuParam) // 仿真设置
    {
        initElecVariable(key, m_pElecBloc->getSimulationPVariable(key),
                         ElecWizardTableModel::Category::simulationParameter, simuOrder++);
    }
    foreach (const QString &key, initParam) // 初始值设置
    {
        initElecVariable(key, m_pElecBloc->getInitializationPVariable(key),
                         ElecWizardTableModel::Category::initSimulationParameter, initOrder++);
    }
}

bool ComponentParamPage::getIsElec()
{
    return isElec;
}

void ComponentParamPage::deleteMoreThanVariable(PVariableGroup group, const QList<QString> &strNames)
{
    if (group) {
        QList<QString> names = group->getVariableMapNames();
        names.removeAll("");
        for (auto str : names) {
            if (!strNames.contains(str)) {
                group->removeVariable(group->getVariableByName(str));
            }
        }
    }
}

void ComponentParamPage::saveTempElecModel(PElectricalBlock elecBlock)
{
    if (elecBlock) {
        // 设置初值
        elecBlock->setName(m_pElecBloc->getName());
        elecBlock->setPrototypeName_CHS(m_pElecBloc->getPrototypeName_CHS());           // 中文名
        elecBlock->setPrototypeName_Readable(m_pElecBloc->getPrototypeName_Readable()); // 简称
        elecBlock->setDescription(m_pElecBloc->getDescription());                       // 描述
        // elecBlock->setClassification(m_pElecBloc->getClassification());                 // 元器件分类
        elecBlock->setPhaseNumber(m_pElecBloc->getPhaseNumber()); // 相数
        elecBlock->setResource(m_pElecBloc->getResource());       // 图片
        //  main组
        PVariableGroup mainGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::MainGroup);
        if (mainGroup) {
            if (mainGroup->getVariableMapNames().size() > 0) {
                copyVariable(mainGroup, RoleDataDefinition::MainGroup, elecBlock);
            } else {
                auto group = elecBlock->getVariableGroup(RoleDataDefinition::MainGroup);
                if (group) {
                    group->clearVariableMap();
                }
            }
        }
        // 端口
        PVariableGroup portGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::PortGroup);
        if (portGroup) {
            if (portGroup->getVariableMapNames().size() > 0) {
                copyVariable(portGroup, RoleDataDefinition::PortGroup, elecBlock);
            } else {
                auto group = elecBlock->getVariableGroup(RoleDataDefinition::PortGroup);
                if (group) {
                    group->clearVariableMap();
                }
            }
        }
        // 电气
        PVariableGroup elecGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::ElectricalParameter);
        if (elecGroup) {
            if (elecGroup->getVariableMapNames().size() > 0) {
                copyVariable(elecGroup, RoleDataDefinition::ElectricalParameter, elecBlock);
            } else {
                elecBlock->removeVariableGroup(RoleDataDefinition::ElectricalParameter);
            }
        }
        // 潮流参数
        PVariableGroup loadGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::LoadFlowParameter);
        if (loadGroup) {
            if (loadGroup->getVariableMapNames().size() > 0) {
                copyVariable(loadGroup, RoleDataDefinition::LoadFlowParameter, elecBlock);
            } else {
                elecBlock->removeVariableGroup(RoleDataDefinition::LoadFlowParameter);
            }
        }
        // 潮流结果
        PVariableGroup loadResultGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::LoadFlowResultParameter);
        if (loadResultGroup) {
            if (loadResultGroup->getVariableMapNames().size() > 0) {
                copyVariable(loadResultGroup, RoleDataDefinition::LoadFlowResultParameter, elecBlock);
            } else {
                elecBlock->removeVariableGroup(RoleDataDefinition::LoadFlowResultParameter);
            }
        }
        // 仿真
        PVariableGroup simulationGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::SimulationParameter);
        if (simulationGroup) {
            if (simulationGroup->getVariableMapNames().size() > 0) {
                copyVariable(simulationGroup, RoleDataDefinition::SimulationParameter, elecBlock);
            } else {
                elecBlock->removeVariableGroup(RoleDataDefinition::SimulationParameter);
            }
        }
        // 初始值
        PVariableGroup initGroup = m_pElecBloc->getVariableGroup(RoleDataDefinition::InitializationParameter);
        if (initGroup) {
            if (initGroup->getVariableMapNames().size() > 0) {
                copyVariable(initGroup, RoleDataDefinition::InitializationParameter, elecBlock);
            } else {
                elecBlock->removeVariableGroup(RoleDataDefinition::InitializationParameter);
            }
        }
    }
    // end
}

void ComponentParamPage::pageDevice(PDeviceModel deviceBlock)
{
    // 修改时
    if (deviceBlock != nullptr) {
        m_pDevice = deviceBlock;
        mDeviceGroup = m_pDevice->getVariableGroup(RoleDataDefinition::DeviceTypeParameter);
    }
    QStringList paramParam = field("paramValue").toString().split(",", QString::SkipEmptyParts);
    // 若是模板，从模板中获取参数
    if (isTemplate) {
        isTemplate = false;
        m_pDevice->createVariableGroup(RoleDataDefinition::DeviceTypeParameter)->copyFromGroup(mDeviceGroup);
        mDeviceGroup = m_pDevice->getVariableGroup(RoleDataDefinition::DeviceTypeParameter);
    } else {
        // 生成唯一组
        if (paramParam.size() != 0 && !m_pDevice->getDeviceTypeVariableGroup()) {
            mDeviceGroup = m_pDevice->createVariableGroup(RoleDataDefinition::DeviceTypeParameter);
        }
    }
    // 赋值
    setValueDevice(paramParam);

    m_deviceModel.clear();

    setTableValueDevice(paramParam);
}

bool ComponentParamPage::getIsDevice()
{
    return isDevice;
}

void ComponentParamPage::setValueDevice(const QStringList &paramParam)
{
    int Order = 0;
    foreach (const QString &key, paramParam) {
        if (!m_pDevice->getDeviceTypePVariable(key)) { // 不存在时
            PVariable var = mDeviceGroup->createVariable();
            var->setName(key);
            var->setDisplayName(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamNameCHS).toString());
            var->setDataType(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamType).toString());
            var->setDataRange(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamRange).toString());
            var->setDefaultValue(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamValue).toString());
            var->setDataUnit(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamUnit).toString());
            var->setOrder(Order++);
            var->setData(RoleDataDefinition::ShowModeRole,
                         m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamMode).toInt());
            QString controlTypeName = m_deviceModel.getParameter(key, DeviceWizardTableModel::ControlTyep).toString();
            if (Global::ControlTyeName.contains(controlTypeName)) {
                var->setControlType(Global::ControlTyeName[controlTypeName].toString());
            } else { // 默认文本框
                var->setControlType(
                        Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox));
            }
            var->setControlVlaue(m_deviceModel.getParameter(key, DeviceWizardTableModel::ControlValue).toString());
        } else { // 存在时
            PVariable var = m_pDevice->getDeviceTypePVariable(key);
            if (!m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamType).toString().isEmpty()) {
                var->setName(key);
                var->setDisplayName(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamNameCHS).toString());
                var->setDataType(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamType).toString());
                var->setDataRange(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamRange).toString());
                var->setDefaultValue(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamValue).toString());
                var->setDataUnit(m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamUnit).toString());
                var->setOrder(Order++);
                var->setData(RoleDataDefinition::ShowModeRole,
                             m_deviceModel.getParameter(key, DeviceWizardTableModel::ParamMode).toInt());
                QString controlTypeName =
                        m_deviceModel.getParameter(key, DeviceWizardTableModel::ControlTyep).toString();
                if (Global::ControlTyeName.contains(controlTypeName)) {
                    var->setControlType(Global::ControlTyeName[controlTypeName].toString());
                } else { // 默认文本框
                    var->setControlType(
                            Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox));
                }
                var->setControlVlaue(m_deviceModel.getParameter(key, DeviceWizardTableModel::ControlValue).toString());
            }
        }
    }
}

void ComponentParamPage::setTableValueDevice(const QStringList &paramParam)
{
    foreach (const QString &strName, paramParam) {
        PVariable deviceValue = m_pDevice->getDeviceTypePVariable(strName);
        if (!deviceValue.isNull()) {
            QString controlTypeName;
            if (deviceValue->getControlType().size() < 3) {
                controlTypeName =
                        Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox);
            } else {
                controlTypeName = Global::ControlTyeName.key(deviceValue->getControlType());
                if (controlTypeName == "") {
                    controlTypeName =
                            Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox);
                }
            }
            // 初始化表格内容，从模板中填充参数的值
            if (deviceValue->getDataType() == "") { // 初始时
                m_deviceModel.addRow(deviceValue->getName(), Global::DataType_Double,
                                     deviceValue->getDataRange().toString(), deviceValue->getDisplayName(),
                                     deviceValue->getDefaultValue().toString(), deviceValue->getOrder(),
                                     deviceValue->getDataUnit(),
                                     deviceValue->getData(RoleDataDefinition::ShowModeRole).toInt(), controlTypeName,
                                     deviceValue->getControlValue());
            } else {
                m_deviceModel.addRow(deviceValue->getName(), deviceValue->getDataType(),
                                     deviceValue->getDataRange().toString(), deviceValue->getDisplayName(),
                                     deviceValue->getDefaultValue().toString(), deviceValue->getOrder(),
                                     deviceValue->getDataUnit(),
                                     deviceValue->getData(RoleDataDefinition::ShowModeRole).toInt(), controlTypeName,
                                     deviceValue->getControlValue());
            }
        } else {
            m_deviceModel.addRow(
                    strName, Global::DataType_Double, "", "", "", 1, "", 0,
                    Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox), "");
        }
    }
}

void ComponentParamPage::onActivated(const QModelIndex &index)
{
    if (index.isValid()) {
        int column = index.column();
        auto controlType = index.sibling(index.row(), WizardTableModel::ControlTyep).data().toString();
        if (WizardTableModel::ControlValue == column
            && (controlType == Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeInt))) {
            setTipsInfo(tr("The set is used to take the range, such as [x,xx),[means greater than or equal to,) means "
                           "less than, and x is empty means no upper/lower limit"));
        } else if (WizardTableModel::ControlValue == column
                   && (controlType
                       == Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeDate))) {
            setTipsInfo(tr("You can specify the date format, such as yyyyMMdd HH:mm:ss.zzz"));
        } else if (WizardTableModel::ControlValue == column
                   && (controlType
                       == Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox))) {
            setTipsInfo(tr("A regular expression can be "
                           "specified,as:^([+-]{2,7})$,^([*/]{2,})$,^([+-]{2,7})$,[^A-Za-z0-9]"));
        } else if (WizardTableModel::ControlValue == column
                   && (controlType
                               == Global::ControlTyeName.key(
                                       Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeComboBox)
                       || controlType
                               == Global::ControlTyeName.key(
                                       Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeComboBoxInput)
                       || controlType
                               == Global::ControlTyeName.key(
                                       Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeComboBoxMultiSelect))) {
            setTipsInfo(
                    tr("Format: ## is equal sign, $$is comma. For example :strA##3$$strB##7$$strC##5,be equivalent to"
                       "strA=3,strB=7,strC=5"));
        } else if (WizardTableModel::ControlValue == column
                   && (controlType == controlType
                       == Global::ControlTyeName.key(
                               Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeComboBoxDoubleEdit))) {
            setTipsInfo(tr("Such as: x, y, z | [double_x double_y], x, y, z is a double type. [double_x,double_y] is "
                           "the range"));
        } else if (WizardTableModel::ControlValue == column
                   && (controlType
                       == Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeDouble))) {
            setTipsInfo(tr("You need to set a range, which is the same form as int. Difference: x and xx can be "
                           "doubles."));
        } else {
            setTipsInfo("");
        }
    }
}

void ComponentParamPage::initElecVariable(const QString &key, PVariable var, ElecWizardTableModel::Category category,
                                          int order)
{
    if (!m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamType).toString().isEmpty()) {
        if (var) {
            var->setName(key);
            var->setDisplayName(m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamNameCHS).toString());
            var->setDataType(m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamType).toString());
            var->setDataRange(m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamRange).toString());
            var->setDisplayName(m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamNameCHS).toString());
            var->setDefaultValue(m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamValue).toString());
            var->setData(RoleDataDefinition::ShowModeRole,
                         m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamMode).toInt());
            var->setDataUnit(m_elecModel.getParameter(key, category, ElecWizardTableModel::ParamUnit).toString());
            QString controlTypeName =
                    m_elecModel.getParameter(key, category, ElecWizardTableModel::ControlTyep).toString();
            if (Global::ControlTyeName.contains(controlTypeName)) {
                var->setControlType(Global::ControlTyeName[controlTypeName].toString());
            } else { // 默认文本框
                var->setControlType(
                        Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox));
            }
            var->setControlVlaue(
                    m_elecModel.getParameter(key, category, ElecWizardTableModel::ControlValue).toString());
            var->setDescription(
                    m_elecModel.getParameter(key, category, ElecWizardTableModel::ParemPortText).toString());
            var->setOrder(order);
        }
    } else { // 修改模块时
        if (!var) {
            return;
        }
        var->setName(key);
    }
}

void ComponentParamPage::initElecAddRow(ElecWizardTableModel::Category category, PVariable elecValue,
                                        bool isPowerResult, PVariable portvalue)
{
    QString controlTypeName;
    if (elecValue->getControlType().size() < 3) {
        controlTypeName = Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox);
    } else {
        controlTypeName = Global::ControlTyeName.key(elecValue->getControlType());
        if (controlTypeName == "") {
            controlTypeName = Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeTextbox);
        }
    }
    // 端口位置与类型
    int pType = 0;
    QString str;
    if (portvalue) {
        str = QString("(%1,%2)").arg(portvalue->getPresetPosition().x()).arg(portvalue->getPresetPosition().y());
        Variable::PortType type = portvalue->getPortType();
        switch (type) {
        case Variable::Electrical:
            pType = 0;
            break;
        case Variable::Busbar:
            pType = 1;
            break;
        case Variable::Electrical_AC:
            pType = 2;
            break;
        case Variable::Electrical_DC:
            pType = 3;
            break;
        default:
            break;
        }
    }
    // end
    if (category == ElecWizardTableModel::Category::PortParameter) { // 行-端口
        if (elecValue->getDataType() == "") { // 初始化，端口与电气参数使用的是同一variable
            m_elecModel.addRow(elecValue->getName(), category, Global::DataType_Double,
                               elecValue->getDataRange().toString(), elecValue->getDisplayName(),
                               elecValue->getDefaultValue().toString(), 2, elecValue->getOrder(), "", controlTypeName,
                               elecValue->getControlValue(), pType, str, portvalue->getDescription());
        } else {
            m_elecModel.addRow(elecValue->getName(), category,
                               (elecValue->getDataType() == Global::DataType_Int) ? Global::DataType_Double
                                                                                  : elecValue->getDataType(),
                               elecValue->getDataRange().toString(), elecValue->getDisplayName(),
                               elecValue->getDefaultValue().toString(),
                               elecValue->getData(RoleDataDefinition::ShowModeRole).toInt(), elecValue->getOrder(),
                               elecValue->getDataUnit(), controlTypeName, elecValue->getControlValue(), pType, str,
                               portvalue->getDescription());
        }
    } else if (elecValue->getDataType() == "") { // 初始时
        m_elecModel.addRow(elecValue->getName(), category, Global::DataType_Double,
                           elecValue->getDataRange().toString(), elecValue->getDisplayName(),
                           elecValue->getDefaultValue().toString(), isPowerResult ? 0 : 1, elecValue->getOrder(), "",
                           controlTypeName, elecValue->getControlValue());
    } else {
        m_elecModel.addRow(elecValue->getName(), category, elecValue->getDataType(),
                           elecValue->getDataRange().toString(), elecValue->getDisplayName(),
                           elecValue->getDefaultValue().toString(),
                           elecValue->getData(RoleDataDefinition::ShowModeRole).toInt(), elecValue->getOrder(),
                           elecValue->getDataUnit(), controlTypeName, elecValue->getControlValue());
    }
}

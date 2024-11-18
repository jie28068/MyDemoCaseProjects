#include "WizardPageInfoNew.h"
#include "KLWidgets/KCustomDialog.h"
#include <QListView>
#include <QMessageBox>

ComponentInfoPage::ComponentInfoPage(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent /*= nullptr*/)
    : WizardPageBase(pBlock, npBlock, parent), isElec(false), m_pParamPage(nullptr), isDevice(false)
{
    ui.setupUi(this);

    // #ifdef COMPILER_PRODUCT_DESIGNER
    ui.lineEditConState->setVisible(true);
    ui.label_12->setVisible(true);
    ui.pushButtonAddConState->setVisible(true);
    ui.label_6->setText(tr("Discrete state variable"));
    // #endif
    // #ifdef COMPILER_PRODUCT_SIMUNPS
    //     ui.lineEditConState->setVisible(false);
    //     ui.label_12->setVisible(false);
    //     ui.pushButtonAddConState->setVisible(false);
    //     ui.label_6->setText(tr("State Variable"));
    // #endif

    setSubTitle(tr("Enter the module information. You can select an existing module from the template to quickly "
                   "create a custom module."));

    registerField("template", ui.comboBoxTemplate, "currentText", SIGNAL(currentIndexChanged(QString)));
    registerField("name*", ui.lineEditName);
    registerField("display_name*", ui.lineEditDisplayName);
    registerField("simple_name", ui.lineEditSimpleName);
    registerField("description", ui.lineEditDescription);
    registerField("var_input", ui.lineEditInput);
    registerField("var_output", ui.lineEditOutput);
    registerField("var_disState", ui.lineEditDisState);
    registerField("var_conState", ui.lineEditConState);
    registerField("var_inner", ui.lineEditInner);
    registerField("var_param", ui.lineEditParam);
    registerField("model_priority", ui.lineEditPriority);
    registerField("m_DirectFeedThrough", ui.checkBox);

    QRegExpValidator *pNameValidator = new QRegExpValidator(QRegExp("^[a-zA-Z][a-zA-Z0-9_]*$"), this);
    QRegExpValidator *pVariableValidator = new QRegExpValidator(QRegExp("([a-zA-Z][a-zA-Z0-9_]*,)*"), this);
    ui.lineEditPriority->setValidator(new QRegExpValidator(QRegExp("([0-9]){0,1}$")));
    ui.lineEditName->setValidator(pNameValidator);
    ui.lineEditInput->setValidator(pVariableValidator);
    ui.lineEditOutput->setValidator(pVariableValidator);
    ui.lineEditDisState->setValidator(pVariableValidator);
    ui.lineEditConState->setValidator(pVariableValidator);
    ui.lineEditInner->setValidator(pVariableValidator);
    ui.lineEditParam->setValidator(pVariableValidator);
    // 直接馈通默认勾选(离散模块手动取消勾选)
    ui.checkBox->setChecked(true);
    ui.comboBoxTemplate->addItem("");
    if (WizardServerMng::getInstance().m_pModelServer) {
        QList<PModel> blockList =
                WizardServerMng::getInstance().m_pModelServer->GetToolkitModels(KL_TOOLKIT::CONTROL_TOOLKIT);
        QStringList blockNameList;
        for (PModel block : blockList) {
            if (block->getPrototypeName() == "") {
                continue;
            }
            blockNameList << block->getPrototypeName();
        }
        ui.comboBoxTemplate->addItems(blockNameList);
    }

    connect(ui.comboBoxTemplate, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxIndexChanged(QString)));

    connect(ui.pushButtonAddInput, SIGNAL(clicked(bool)), this, SLOT(onAddButtonClicked()));
    connect(ui.pushButtonAddOutput, SIGNAL(clicked(bool)), this, SLOT(onAddButtonClicked()));
    connect(ui.pushButtonAddDisState, SIGNAL(clicked(bool)), this, SLOT(onAddButtonClicked()));
    connect(ui.pushButtonAddConState, SIGNAL(clicked(bool)), this, SLOT(onAddButtonClicked()));
    connect(ui.pushButtonAddInner, SIGNAL(clicked(bool)), this, SLOT(onAddButtonClicked()));
    connect(ui.pushButtonAddParam, SIGNAL(clicked(bool)), this, SLOT(onAddButtonClicked()));

    connect(ui.lineEditName, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditDisplayName, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditSimpleName, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditDescription, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditInput, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditOutput, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditDisState, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditConState, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditInner, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(ui.lineEditParam, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
}

ComponentInfoPage::ComponentInfoPage(PElectricalBlock pBlock, PElectricalBlock npBlock, QWidget *parent)
    : WizardPageBase(pBlock, npBlock, parent), isElec(true), m_pParamPage(nullptr), isDevice(false)
{
    classificationLists << Global::DeviceGroupBus << Global::DeviceGroupElement << Global::DeviceGroupMachine
                        << Global::DeviceGroupTransformer << Global::DeviceGroupLine << Global::DeviceGroupLoad
                        << Global::DeviceGroupDC;
    phaseNumberLists << tr("1") << tr("2") << tr("3");
    setSubTitle(tr("Please fill in the following parameters as prompted"));
    QRegExpValidator *wordRegex = new QRegExpValidator(QRegExp("^[a-zA-Z][a-zA-Z0-9_]*$"), this);
    QRegExpValidator *wordListRegex = new QRegExpValidator(QRegExp("([a-zA-Z][a-zA-Z0-9_]*,)*"), this);
    QString hintStr = tr("Options, separated by commas");
    QString descStr = tr("Optional, defaults to the prototype name");
    // 创建标签和文本框
    modelTypeLabel = new QLabel(tr("Parameter Templates"));
    modelType = new QComboBox;
    modelType->setView(new QListView());
    modelType->addItem("");
    modelType->addItems(getAllElecModelProeryName());
    QLabel *nameLabel = new QLabel(tr("Prototype Name"));
    nameLineEdit = new KLineEdit;
    nameLineEdit->setValidator(wordRegex);
    nameLineEdit->setPlaceholderText(tr("Mandatory fields must begin with English"));
    QLabel *nameCHSLabel = new QLabel(tr("Brief Description"));
    nameCHSLineEdit = new KLineEdit;
    nameCHSLineEdit->setPlaceholderText(tr("Required Fields"));
    QLabel *nameReadLable = new QLabel(tr("For Short"));
    nameReadLineEdit = new KLineEdit;
    nameReadLineEdit->setPlaceholderText(descStr);
    QLabel *showExplainLabel = new QLabel(tr("Detailed Description"));
    showExplainLineEdit = new KLineEdit;
    showExplainLineEdit->setPlaceholderText(tr("Optional, module description"));
    QLabel *portLabel = new QLabel(tr("Port"));
    portLineEdit = new KLineEdit;
    portLineEdit->setPlaceholderText(hintStr);
    portLineEdit->setValidator(wordListRegex);
    // 端口初始化值设置
    QPushButton *portValueBbutton = new QPushButton;
    QLabel *paramLabel = new QLabel(tr("Electrical Parameters"));
    paramLineEdit = new KLineEdit;
    paramLineEdit->setPlaceholderText(hintStr);
    paramLineEdit->setValidator(wordListRegex);
    QLabel *tideLabel = new QLabel(tr("Power Flow Parameter"));
    tideLineEdit = new KLineEdit;
    tideLineEdit->setPlaceholderText(hintStr);
    tideLineEdit->setValidator(wordListRegex);
    QLabel *tideResultLabel = new QLabel(tr("Power Flow Result"));
    tideResultLineEdit = new KLineEdit;
    tideResultLineEdit->setPlaceholderText(hintStr);
    tideResultLineEdit->setValidator(wordListRegex);
    QLabel *simuLabel = new QLabel(tr("Simulation Setup"));
    simuLineEdit = new KLineEdit;
    simuLineEdit->setPlaceholderText(hintStr);
    simuLineEdit->setValidator(wordListRegex);
    QLabel *initLabel = new QLabel(tr("Initial Value Setting"));
    initLineEdit = new KLineEdit;
    initLineEdit->setPlaceholderText(hintStr);
    initLineEdit->setValidator(wordListRegex);
    elecLabelTips = new QLabel;
    QLabel *phaseLabel = new QLabel(tr("Number of Phases"));
    phaseComboBox = new QComboBox;
    phaseComboBox->setView(new QListView());
    phaseComboBox->addItems(phaseNumberLists);
    // 创建网格布局
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(modelTypeLabel, 0, 0, 1, 1);
    layout->addWidget(modelType, 0, 1, 1, 1);
    layout->addWidget(nameLabel, 1, 0, 1, 1);
    layout->addWidget(nameLineEdit, 1, 1, 1, 1);
    layout->addWidget(nameCHSLabel, 2, 0, 1, 1);
    layout->addWidget(nameCHSLineEdit, 2, 1, 1, 1);
    layout->addWidget(nameReadLable, 3, 0, 1, 1);
    layout->addWidget(nameReadLineEdit, 3, 1, 1, 1);
    layout->addWidget(showExplainLabel, 4, 0, 1, 1);
    layout->addWidget(showExplainLineEdit, 4, 1, 1, 1);
    layout->addWidget(portLabel, 5, 0, 1, 1);
    layout->addWidget(portLineEdit, 5, 1, 1, 1);
    layout->addWidget(paramLabel, 6, 0, 1, 1);
    layout->addWidget(paramLineEdit, 6, 1, 1, 1);
    layout->addWidget(tideLabel, 7, 0, 1, 1);
    layout->addWidget(tideLineEdit, 7, 1, 1, 1);
    layout->addWidget(tideResultLabel, 8, 0, 1, 1);
    layout->addWidget(tideResultLineEdit, 8, 1, 1, 1);
    layout->addWidget(simuLabel, 9, 0, 1, 1);
    layout->addWidget(simuLineEdit, 9, 1, 1, 1);
    layout->addWidget(initLabel, 10, 0, 1, 1);
    layout->addWidget(initLineEdit, 10, 1, 1, 1);
    layout->addWidget(phaseLabel, 11, 0, 1, 1);
    QHBoxLayout *hBoxLayout2 = new QHBoxLayout;
    hBoxLayout2->addWidget(phaseComboBox);
    QSpacerItem *spacer2 = new QSpacerItem(800, 0, QSizePolicy::Expanding, QSizePolicy::Maximum);
    hBoxLayout2->addItem(spacer2);
    layout->addLayout(hBoxLayout2, 11, 1, 1, 1);

    // 创建垂直布局
    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addLayout(layout);
    QSpacerItem *spacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(spacer3);
    verticalLayout->addWidget(elecLabelTips);
    setLayout(verticalLayout);

    registerField("nameValue", nameLineEdit);             // 原型名称
    registerField("protValue", portLineEdit);             // 端口
    registerField("paramValue", paramLineEdit);           // 电气参数
    registerField("tideValue", tideLineEdit);             // 潮流参数
    registerField("tideResultValue", tideResultLineEdit); // 潮流结果
    registerField("simuValue", simuLineEdit);             // 仿真设置
    registerField("initValue", initLineEdit);             // 初始值设置
    registerField("nameCHSValue", nameCHSLineEdit);       // 中文名称
    registerField("nameReadValue", nameReadLineEdit);     // 简写
    registerField("explainValue", showExplainLineEdit);   // 说明

    // 更新文本框状态
    connect(nameLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(nameCHSLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(portLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(paramLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(tideLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(tideResultLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(simuLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(initLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(nameReadLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(showExplainLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    // 模块类型
    connect(modelType, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxIndexChanged(QString)));
}

ComponentInfoPage::ComponentInfoPage(PDeviceModel pBlock, PDeviceModel npBlock, QWidget *parent)
    : WizardPageBase(pBlock, npBlock, parent), isElec(false), m_pParamPage(nullptr), isDevice(true)
{
    setSubTitle(tr("Please fill in the following parameters as prompted"));
    QList<PDeviceModel> devideModelsa = WizardServerMng::getInstance().m_pProjectServer->GetSystemDeviceModels();
    for (PDeviceModel value : devideModelsa) {
        if (value->getName() == "") {
            continue;
        }
        devideModelNames << value->getName();
    }
    QRegExpValidator *wordRegex = new QRegExpValidator(QRegExp("^[a-zA-Z][a-zA-Z0-9_]*$"), this);
    QRegExpValidator *wordListRegex = new QRegExpValidator(QRegExp("([a-zA-Z][a-zA-Z0-9_-]*,)*"), this);
    QString hintStr = tr("Options, separated by commas");
    QString descStr = tr("Optional, defaults to the prototype name");

    // 布局界面
    modelTypeLabel = new QLabel(tr("Parameter Templates"));
    modelType = new QComboBox;
    modelType->addItem("");
    modelType->setView(new QListView());
    modelType->addItems(devideModelNames);
    QLabel *nameLabel = new QLabel(tr("Prototype Name"));
    nameLineEdit = new KLineEdit;
    nameLineEdit->setValidator(wordRegex);
    nameLineEdit->setPlaceholderText(tr("Mandatory fields must begin with English"));
    QLabel *elecLabel = new QLabel(tr("Associated electrical prototype"));
    relevanceElecComboBox = new QComboBox;
    relevanceElecComboBox->setView(new QListView());
    relevanceElecComboBox->addItems(getAllElecModelProeryName());
    QLabel *param = new QLabel(tr("Parameters"));
    paramLineEdit = new KLineEdit;
    paramLineEdit->setPlaceholderText(hintStr);
    paramLineEdit->setValidator(wordListRegex);
    QLabel *nameCHSLabel = new QLabel(tr("Brief Description"));
    nameCHSLineEdit = new KLineEdit;
    nameCHSLineEdit->setPlaceholderText(tr("Optional items"));
    QLabel *nameReadLable = new QLabel(tr("For Short"));
    nameReadLineEdit = new KLineEdit;
    nameReadLineEdit->setPlaceholderText(tr("Optional items"));
    QLabel *showExplainLabel = new QLabel(tr("Detailed Description"));
    showExplainLineEdit = new KLineEdit;
    showExplainLineEdit->setPlaceholderText(tr("Optional items"));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(modelTypeLabel, 0, 0);
    layout->addWidget(modelType, 0, 1);
    layout->addWidget(nameLabel, 1, 0);
    layout->addWidget(nameLineEdit, 1, 1);
    layout->addWidget(elecLabel, 2, 0);
    layout->addWidget(relevanceElecComboBox, 2, 1);
    layout->addWidget(nameCHSLabel, 3, 0);
    layout->addWidget(nameCHSLineEdit, 3, 1);
    layout->addWidget(nameReadLable, 4, 0);
    layout->addWidget(nameReadLineEdit, 4, 1);
    layout->addWidget(showExplainLabel, 5, 0);
    layout->addWidget(showExplainLineEdit, 5, 1);
    layout->addWidget(param, 6, 0);
    layout->addWidget(paramLineEdit, 6, 1);

    // 创建垂直布局 提示消息
    elecLabelTips = new QLabel;
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(layout);
    QSpacerItem *spacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(spacer3);
    vLayout->addWidget(elecLabelTips);
    setLayout(vLayout);

    registerField("nameValue", nameLineEdit);           // 原型名称
    registerField("paramValue", paramLineEdit);         // 参数
    registerField("nameCHSValue", nameCHSLineEdit);     // 简述
    registerField("nameReadValue", nameReadLineEdit);   // 简称
    registerField("explainValue", showExplainLineEdit); // 详细说明

    connect(nameLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(paramLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(nameReadLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(nameCHSLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(showExplainLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    // 模块类型
    connect(modelType, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxIndexChanged(QString)));
}

bool ComponentInfoPage::isComplete() const
{
    // 设备类型
    if (isDevice) {
        if (nameLineEdit->text().isEmpty()) {
            setTips(tr("Prototype names cannot be empty!"));
            return false;
        }
        QString name = QString("%1_OriginalType").arg(field("nameValue").toString().trimmed());
        if (devideModelNames.contains(name, Qt::CaseInsensitive)) {
            setTips(tr("Prototype name [%1] already exists, please rename it!").arg(name));
            return false;
        }
        QStringList listVariableNames;
        if (!invalidateVariableName(paramLineEdit->text(), listVariableNames, tr("param"))) {
            return false;
        }
        // 原型名称只能是英文，数字和下划线组成，最大长度为64个字符
        if (field("nameValue").toString().trimmed().length() > Global::MAX_LENGTH_NMAE) {
            setTips(tr("Prototype names should be in English only, consisting of numbers and underscores, and have a "
                       "maximum length of 64 characters"));
            return false;
        }

        if (!nameReadLineEdit->text().isEmpty()) {
            if (!nameReadLineEdit->text().contains(QRegExp("^[a-zA-Z0-9_]{1,64}$"))) {
                // 简称必须为英文,数字或下划线，长度限制64！
                setTips(tr(
                        "The abbreviation must be in English ,underline,or a number. The length is limited to 64！"));
                return false;
            }
        }
        if (!nameCHSLineEdit->text().contains(QRegExp("^.{0,1024}$"))
            || !showExplainLineEdit->text().contains(QRegExp("^.{0,1024}$"))) {
            setTips(tr("Length limit 1024!")); // 长度限制1024！
            return false;
        }
        setTips("");
        return true;
    } else if (isElec) { // 电气
        if (nameLineEdit->text().isEmpty()) {
            setTips(tr("Prototype names cannot be empty!"));
            return false;
        }
        if (nameCHSLineEdit->text().trimmed().isEmpty() && nameCHSLineEdit->isEnabled()) {
            setTips(tr("Brief description cannot be empty!")); // 简述不能为空！
            return false;
        }
        if (!nameCHSLineEdit->text().contains(QRegExp("^.{0,1024}$"))
            || !showExplainLineEdit->text().contains(QRegExp("^.{0,1024}$"))) {
            setTips(tr("Length limit 1024!")); // 长度限制1024！
            return false;
        }
        if (!isValidName(nameLineEdit->text()) || nameLineEdit->text() == modelType->currentText()) {
            setTips(tr("Module with name [%1] already exists!").arg(nameLineEdit->text()));
            return false;
        }
        if (nameLineEdit->text().length() > Global::MAX_LENGTH_NMAE) {
            setTips(tr("Prototype names should be in English only, consisting of numbers and underscores, and have a "
                       "maximum length of 64 characters"));
            return false;
        }
        if (!nameReadLineEdit->text().isEmpty()) {
            if (!nameReadLineEdit->text().contains(QRegExp("^[a-zA-Z0-9_]{1,64}$"))) {
                // 简称必须为英文,数字或下划线，长度限制64！
                setTips(tr(
                        "The abbreviation must be in English ,underline,or a number. The length is limited to 64！"));
                return false;
            }
        }
        QStringList listVariableNames;
        // 潮流参数与潮流结果的变量可以相同
        QStringList tideResultNames = tideResultLineEdit->text().split(',', QString::SkipEmptyParts);
        QStringList tideNames = tideLineEdit->text().split(',', QString::SkipEmptyParts);
        QStringList result;
        foreach (const QString &str, tideNames) {
            if (!tideResultNames.contains(str)) {
                result.append(str);
            }
        }
        result << tideResultNames;
        QString strNames = result.join(",");
        // end
        if (!invalidateVariableName(portLineEdit->text(), listVariableNames, tr("Port"))) {
            return false;
        }
        if (!invalidateVariableName(paramLineEdit->text(), listVariableNames, tr("Electrical parameters"))) {
            return false;
        }
        if (!invalidateVariableName(strNames, listVariableNames, tr("Power flow parameter"))) {
            return false;
        }
        if (!invalidateVariableName(simuLineEdit->text(), listVariableNames, tr("Simulation setup"))) {
            return false;
        }
        if (!invalidateVariableName(initLineEdit->text(), listVariableNames, tr("Simulation initial value"))) {
            return false;
        }
        if (nameLineEdit->isVisible()) { // 修改，无该模块时
            setTips("");
        }
        if (!paramLineEdit->isEnabled()) { // 修改，无该模块时
            return false;
        }
        return true;
    } else {
        // 控制
        if (ui.lineEditName->text().isEmpty()) {
            setTips(tr("Prototype names cannot be empty!"));
            return false;
        }
        if (!isValidName(ui.lineEditName->text())) {
            setTips(tr("Module with name [%1] already exists!").arg(ui.lineEditName->text()));
            return false;
        }
        if (ui.lineEditDisplayName->text().trimmed().isEmpty()) {
            setTips(tr("Brief description cannot be empty!")); // 简述不能为空！
            return false;
        }
        if (!ui.lineEditDisplayName->text().contains(QRegExp("^.{0,1024}$"))
            || !ui.lineEditDescription->text().contains(QRegExp("^.{0,1024}$"))) {
            setTips(tr("Length limit 1024!")); // 长度限制1024！
            return false;
        }
        if (ui.lineEditName->text().length() > Global::MAX_LENGTH_NMAE) {
            setTips(tr("Prototype names should be in English only, consisting of numbers and underscores, and have a "
                       "maximum length of 64 characters"));
            return false;
        }
        if (!ui.lineEditSimpleName->text().isEmpty()) {
            if (!ui.lineEditSimpleName->text().contains(QRegExp("^[a-zA-Z0-9_]{1,64}$"))) {
                setTips(tr("The abbreviation must be in English ,underline,or a number. The length is limited to "
                           "64！"));
                return false;
            }
        }
        QString stateName = tr("State variable");
        // #ifdef COMPILER_PRODUCT_DESIGNER
        stateName = tr("Discrete state variable");
        // #endif
        // #ifdef COMPILER_PRODUCT_SIMUNPS
        //         stateName = tr("State variable");
        // #endif
        QStringList listVariableNames;
        if (!invalidateVariableName(ui.lineEditInput->text(), listVariableNames, tr("Input variables"))) {
            return false;
        }
        if (!invalidateVariableName(ui.lineEditOutput->text(), listVariableNames, tr("Output variables"))) {
            return false;
        }
        if (!invalidateVariableName(ui.lineEditConState->text(), listVariableNames, tr("Continuous state variable"))) {
            return false;
        }
        if (!invalidateVariableName(ui.lineEditDisState->text(), listVariableNames, stateName)) {
            return false;
        }
        if (!invalidateVariableName(ui.lineEditInner->text(), listVariableNames, tr("Internal variables"))) {
            return false;
        }
        if (!invalidateVariableName(ui.lineEditParam->text(), listVariableNames, tr("Module parameters"))) {
            return false;
        }
        setTips("");
        return true;
    }
}

bool ComponentInfoPage::validatePage()
{
    // 设备类型
    if (isDevice) {
        // 名称 = 名称+"-"+关联电气
        if (nameLineEdit->isEnabled()) {
            QString name = QString("%1_OriginalType").arg(field("nameValue").toString().trimmed());
            m_pDevice->setPrototypeName(name);
            m_pDevice->setName(name);
            m_pDevice->setDeviceTypePrototypeName(relevanceElecComboBox->currentText());
        }
        // 保存模块简称
        if (!nameReadLineEdit->text().isEmpty()) {
            m_pDevice->setPrototypeName_Readable(field("nameReadValue").toString());
        }
        // 保存模块中文名称
        if (!nameCHSLineEdit->text().isEmpty()) {
            m_pDevice->setPrototypeName_CHS(field("nameCHSValue").toString());
        }
        // 保存模块介绍
        if (!showExplainLineEdit->text().isEmpty()) {
            m_pDevice->setDescription(field("explainValue").toString());
        }
        return true;
    } else if (isElec) { // 电气
        // 保存模块原型名称
        m_pElecBloc->setPrototypeName(field("nameValue").toString());
        m_pElecBloc->setName(field("nameValue").toString());
        // 保存模块简称
        if (nameReadLineEdit->text().isEmpty()) {
            m_pElecBloc->setPrototypeName_Readable(field("nameValue").toString());
        } else {
            m_pElecBloc->setPrototypeName_Readable(field("nameReadValue").toString());
        }
        // 保存模块中文名称
        m_pElecBloc->setPrototypeName_CHS(field("nameCHSValue").toString());
        // 保存模块介绍
        m_pElecBloc->setDescription(field("explainValue").toString());
        // 设置相数
        if (phaseComboBox->currentIndex() == phaseNumberLists.at(0).toInt() - 1) {
            m_pElecBloc->setPhaseNumber(1);
        } else if (phaseComboBox->currentIndex() == phaseNumberLists.at(1).toInt() - 1) {
            m_pElecBloc->setPhaseNumber(2);
        } else if (phaseComboBox->currentIndex() == phaseNumberLists.at(2).toInt() - 1) {
            m_pElecBloc->setPhaseNumber(3);
        }
        return true;
    } else {
        QString strTemplate = field("template").toString();
        if (!strTemplate.isEmpty()) {
            // 设置模块模板
            strTemplate = field("template").toString();
            QStringList list;
            list << strTemplate;
            m_pBlock->setTemplates(list);
        } else {
            m_pBlock->getTemplates().clear();
        }
        m_pBlock->setName(field("name").toString());
        m_pBlock->setPrototypeName(field("name").toString());
        m_pBlock->setPrototypeName_CHS(field("display_name").toString().trimmed());
        if (ui.lineEditSimpleName->text().isEmpty()) {
            m_pBlock->setPrototypeName_Readable(field("name").toString());
        } else {
            m_pBlock->setPrototypeName_Readable(field("simple_name").toString());
        }
        m_pBlock->setDescription(field("description").toString());
        if (ui.lineEditPriority->text().isEmpty()) {
            m_pBlock->setPriority(5);
        } else {
            m_pBlock->setPriority(field("model_priority").toInt());
        }
        m_pBlock->setDirectFeedThrough(field("m_DirectFeedThrough").toBool());
        // 区分prototypeName和prototypeName_Readable，prototypeName不带空白字符
        m_pBlock->setPrototypeName(m_pBlock->getPrototypeName().remove(QRegExp("\\s")));
        return true;
    }
}

void ComponentInfoPage::setBlockEditable(bool edit)
{
    // 修改模块的时候不可编辑模块版本、名称
    ui.comboBoxTemplate->setVisible(edit);
    ui.lineEditName->setEnabled(edit);
    ui.label_2->setVisible(edit);
}

void ComponentInfoPage::onAddButtonClicked()
{
    DialogVariableInput *dvi = new DialogVariableInput;

    KCustomDialog dlg;
    dlg.setCentralWidget(dvi);
    dlg.setHiddenButtonGroup(true);
    dlg.setTitle(dvi->windowTitle());
    connect(dvi, SIGNAL(finished(int)), &dlg, SLOT(done(int)));

    if (QDialog::Accepted != dlg.exec()) {
        return;
    }
    if (dvi->text().isEmpty()) {
        return;
    }

    QObject *pSender = sender();
    KLineEdit *pTargetLineEdit = nullptr;
    if (pSender == ui.pushButtonAddInput) {
        pTargetLineEdit = ui.lineEditInput;
    } else if (pSender == ui.pushButtonAddOutput) {
        pTargetLineEdit = ui.lineEditOutput;
    } else if (pSender == ui.pushButtonAddConState) {
        pTargetLineEdit = ui.lineEditConState;
    } else if (pSender == ui.pushButtonAddDisState) {
        pTargetLineEdit = ui.lineEditDisState;
    } else if (pSender == ui.pushButtonAddInner) {
        pTargetLineEdit = ui.lineEditInner;
    } else if (pSender == ui.pushButtonAddParam) {
        pTargetLineEdit = ui.lineEditParam;
    } else {
        return;
    }
    if (pTargetLineEdit->text().isEmpty()) {
        pTargetLineEdit->setText(dvi->text());
    } else {
        QString strCurrentText = pTargetLineEdit->text();
        if (strCurrentText.endsWith(',')) {
            pTargetLineEdit->setText(strCurrentText + dvi->text());
        } else {
            pTargetLineEdit->setText(strCurrentText + "," + dvi->text());
        }
    }
}

void ComponentInfoPage::setElecParameterPage(ComponentParamPage *pParamPage)
{
    m_pParamPage = pParamPage;
}

void ComponentInfoPage::onComboBoxIndexChanged(const QString &strText)
{
    if (isDevice) {
        if (strText.isEmpty()) {
            setField("paramValue", "");
            return;
        }
        auto deviceBlock = ModelSystem::getInstance()->getSystemModel(strText).dynamicCast<DeviceModel>();
        if (deviceBlock) {
            setRelevanceElecComboBox(deviceBlock->getDeviceTypePrototypeName());
            PVariableGroup deviceGroup = deviceBlock->getDeviceTypeVariableGroup();
            if (deviceGroup) {
                auto deviceMap = deviceGroup->getVariableSortByOrder();
                QStringList strlist = initParamList(deviceMap, RoleDataDefinition::DeviceTypeParameter);
                setField("paramValue", strlist.join(","));
                strlist.clear();
            }
            // 参数模板
            if (m_pParamPage) {
                m_pParamPage->setDeviceGroup(deviceGroup);
            }
        }
        return;
    }
    if (isElec) {
        if (strText.isEmpty()) {
            return;
        }
        auto elecBlock = qSharedPointerDynamicCast<ElectricalBlock>(
                WizardServerMng::getInstance().m_pModelServer->GetToolkitModel(strText,
                                                                               KL_TOOLKIT::ELECTRICAL_TOOLKIT));
        QStringList portlists;
        setElecModuleType(elecBlock);
        return;
    }

    if (strText.isEmpty()) {
        m_pBlock->setCodeText(QString());
        return;
    }

    auto block = qSharedPointerDynamicCast<ControlBlock>(
            WizardServerMng::getInstance().m_pModelServer->GetToolkitModel(strText, KL_TOOLKIT::CONTROL_TOOLKIT));
    if (!block) {
        setTips(tr("Failed to get the template type"));
        return;
    }

    bool bKeepOld = false;
    if (!ui.lineEditInput->text().isEmpty() || !ui.lineEditOutput->text().isEmpty()
        || !ui.lineEditConState->text().isEmpty() || !ui.lineEditInner->text().isEmpty()
        || !ui.lineEditParam->text().isEmpty() || !ui.lineEditDisState->text().isEmpty()) {
        bKeepOld =
                KMessageBox::information(this, tr("Changing The Template"), tr("Do you keep the current parameters?"),
                                         KMessageBox::Yes | KMessageBox::No)
                == KMessageBox::Yes;
    }

    if (block->getInputVariableGroup()) {
        addVariableFromTemplate(ui.lineEditInput, block->getInputVariableGroup()->getVariableMapNames(), bKeepOld);
    }
    if (block->getOutputVariableGroup()) {
        addVariableFromTemplate(ui.lineEditOutput, block->getOutputVariableGroup()->getVariableMapNames(), bKeepOld);
    }
    if (block->getDiscreteStateVariableGroup()) {
        addVariableFromTemplate(ui.lineEditDisState, block->getDiscreteStateVariableGroup()->getVariableMapNames(),
                                bKeepOld);
    }
    // #ifdef COMPILER_PRODUCT_DESIGNER
    if (block->getContinueStateVariableGroup()) {
        addVariableFromTemplate(ui.lineEditConState, block->getContinueStateVariableGroup()->getVariableMapNames(),
                                bKeepOld);
    }
    // #endif
    if (block->getInternalVariableGroup()) {
        addVariableFromTemplate(ui.lineEditInner, block->getInternalVariableGroup()->getVariableMapNames(), bKeepOld);
    }
    if (block->getControlVariableGroup()) {
        addVariableFromTemplate(ui.lineEditParam, block->getControlVariableGroup()->getVariableMapNames(), bKeepOld);
    }
}

bool ComponentInfoPage::isValidName(const QString &strName) const
{
    // 设备类型
    if (isDevice) {
        return true;
    }
    // 控制与电气
    if (WizardServerMng::getInstance().m_pProjectServer) {
        if (WizardServerMng::getInstance().m_pProjectServer->IsExistModel(strName)) {
            return false;
        }
    }
    return true;
}

bool ComponentInfoPage::invalidateVariableName(const QString &strText, QStringList &listTotalVariable,
                                               const QString &variableType) const
{
    int count = 0;
    foreach (const QString &str, strText.split(',', QString::SkipEmptyParts)) {
        ++count;
        if (count > MAX_VARIABE_NUM) {
            setTips(tr("%1 has more variables than %2").arg(variableType).arg(MAX_VARIABE_NUM));
            return false;
        }
        if (str.length() > MAX_VARIABE_LENGTH) {
            setTips(tr("The %1 variable name [%2] is longer than %3")
                            .arg(variableType)
                            .arg(str)
                            .arg(MAX_VARIABE_LENGTH));
            return false;
        }
        if (listTotalVariable.contains(str)) {
            setTips(tr("Duplicate variable names exist [%1]").arg(str));
            return false;
        }
        listTotalVariable.append(str);
    }
    return true;
}

void ComponentInfoPage::addVariableFromTemplate(QLineEdit *pLineEdit, const QStringList &listTemplateVariable,
                                                bool bKeepOld) const
{
    if (listTemplateVariable.isEmpty()) {
        if (!bKeepOld) {
            pLineEdit->setText("");
        }
        return;
    }

    if (!bKeepOld) {
        pLineEdit->setText(listTemplateVariable.join(","));
    } else {
        QStringList listCurrentVariable = pLineEdit->text().split(",", QString::SkipEmptyParts) + listTemplateVariable;
        listCurrentVariable.removeDuplicates();
        pLineEdit->setText(listCurrentVariable.join(","));
    }
}

bool ComponentInfoPage::getIsDevice()
{
    return isDevice;
}

bool ComponentInfoPage::getIsElec()
{
    return isElec;
}

QLineEdit *ComponentInfoPage::getNmaeLineEdit()
{
    return nameLineEdit;
}

void ComponentInfoPage::setPhaseComboBox(int value)
{
    switch (value) {
    case 1:
        phaseComboBox->setCurrentIndex(0);
        break;
    case 2:
        phaseComboBox->setCurrentIndex(1);
        break;
    case 3:
        phaseComboBox->setCurrentIndex(2);
        break;
    default:
        phaseComboBox->setCurrentIndex(0);
        break;
    }
}

void ComponentInfoPage::setControlIde(bool value)
{
    nameLineEdit->setEnabled(value);
    portLineEdit->setEnabled(value);
    paramLineEdit->setEnabled(value);
    tideLineEdit->setEnabled(value);
    simuLineEdit->setEnabled(value);
    initLineEdit->setEnabled(value);
    phaseComboBox->setEnabled(value);
    modelType->setEnabled(value);
    nameCHSLineEdit->setEnabled(value);
    nameReadLineEdit->setEnabled(value);
    showExplainLineEdit->setEnabled(value);
}

void ComponentInfoPage::setModelComboBox(bool value)
{
    modelType->setVisible(value);
    modelTypeLabel->setVisible(value);
}

QLineEdit *ComponentInfoPage::getNameCHSLineEdit()
{
    return nameCHSLineEdit;
}

QStringList ComponentInfoPage::getAllElecModelProeryName()
{
    QList<Kcc::BlockDefinition::PModel> elecModels =
            WizardServerMng::getInstance().m_pModelServer->GetToolkitModels(KL_TOOLKIT::ELECTRICAL_TOOLKIT);
    QStringList strs;
    auto elecModelIter = elecModels.begin();
    while (elecModelIter != elecModels.end()) {
        if (elecModelIter->data()->getPrototypeName() == "") {
            ++elecModelIter;
            continue;
        }
        strs.append(elecModelIter->data()->getPrototypeName());
        ++elecModelIter;
    }
    return strs;
}

void ComponentInfoPage::setElecModuleType(PElectricalBlock elecBlock)
{
    QStringList portNameList; // 端口列表
    QStringList strlist;      // 参数列表
    // 中文名称
    setField("nameCHSValue", elecBlock->getPrototypeName_CHS());
    // 简写
    setField("nameReadValue", elecBlock->getPrototypeName_Readable());
    // 说明
    setField("explainValue", elecBlock->getDescription());
    // 端口
    PVariableGroup portGroup = elecBlock->getPortManager();
    if (portGroup) {
        auto varaiableList = portGroup->getVariableSortByOrder();
        for (auto variable : varaiableList) {
            QString str = variable->getName();
            if (!portNameList.contains(str)) {
                portNameList << str;
            }
        }
    }
    setField("protValue", portNameList.join(","));
    // 电气参数
    PVariableGroup elecGroup = elecBlock->getElectricalVariableGroup();
    if (elecGroup) {
        auto varaiableList = elecGroup->getVariableSortByOrder();
        strlist = initParamList(varaiableList, RoleDataDefinition::ElectricalParameter, portNameList);
        setField("paramValue", strlist.join(","));
        strlist.clear();
        portNameList.clear();
    }
    // 潮流参数
    PVariableGroup loadGroup = elecBlock->getLoadFlowVariableGroup();
    if (loadGroup) {
        auto varaiableList = loadGroup->getVariableSortByOrder();
        strlist = initParamList(varaiableList, RoleDataDefinition::LoadFlowParameter);
        setField("tideValue", strlist.join(","));
        strlist.clear();
    }
    // 潮流结果
    PVariableGroup loadResultGroup = elecBlock->getLoadFlowResultVariableGroup();
    if (loadResultGroup) {
        auto varaiableList = loadResultGroup->getVariableSortByOrder();
        strlist = initParamList(varaiableList, RoleDataDefinition::LoadFlowResultParameter);
        setField("tideResultValue", strlist.join(","));
        strlist.clear();
    }
    // 仿真设置
    PVariableGroup simGroup = elecBlock->getSimulationVariableGroup();
    if (simGroup) {
        auto varaiableList = simGroup->getVariableSortByOrder();
        strlist = initParamList(varaiableList, RoleDataDefinition::SimulationParameter);
        setField("simuValue", strlist.join(","));
        strlist.clear();
    }
    // 初始值设置
    PVariableGroup initGroup = elecBlock->getInitializationVariableGroup();
    if (initGroup) {
        auto varaiableList = initGroup->getVariableSortByOrder();
        strlist = initParamList(varaiableList, RoleDataDefinition::InitializationParameter);
        setField("initValue", strlist.join(","));
        strlist.clear();
    }
    // 相数
    setPhaseComboBox(elecBlock->getPhaseNumber());
    // 参数模板
    if (m_pParamPage) {
        m_pParamPage->setElecGroup(QList<PVariableGroup>() << elecGroup << loadGroup << simGroup << initGroup);
    }
}

void ComponentInfoPage::setTips(const QString &strTips) const
{
    if (isDevice) {
        elecLabelTips->setText(strTips);
        return;
    }
    if (isElec) {
        elecLabelTips->setText(strTips);
        return;
    }
    ui.labelTips->setText(strTips);
}

QStringList ComponentInfoPage::initParamList(const QList<PVariable> &list, const QString &type,
                                             QStringList &portNameList)
{
    QStringList strlist;
    for (auto variable : list) {
        QString str = variable->getName();
        if (type == RoleDataDefinition::ElectricalParameter) {
            if (!strlist.contains(str) && !portNameList.contains(str)) {
                strlist << str;
            }
        } else {
            if (!strlist.contains(str)) {
                strlist << str;
            }
        }
    }
    return strlist;
}

void ComponentInfoPage::setRelevanceElecComboBox(const QString &str)
{
    if (getAllElecModelProeryName().contains(str)) {
        relevanceElecComboBox->setCurrentIndex(relevanceElecComboBox->findText(str));
    }
}

void ComponentInfoPage::setRelevanceElecComboBox(bool falg)
{
    if (relevanceElecComboBox) { // 设备类型的关联电气原型控件
        relevanceElecComboBox->setEnabled(falg);
    }
}

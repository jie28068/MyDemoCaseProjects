#include "blocktypeselect.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLWidgets/KMessageBox.h"
#include "PropertyServerMng.h"

#include <QApplication>
// 日志
#include "CoreLib/ServerManager.h"
USE_LOGOUT_("BlockTypeSelect")

// 特殊模块 控制输出
#define SPECIAL_BLOCK_CTRLOUT "Out"
// 特殊模块 控制输入
#define SPECIAL_BLOCK_CTRLIN "In"
// 特殊模块 插槽模块
#define SPECIAL_BLOCK_SLOT "Slot"
// 特殊模块 电气接口模块
#define SPECIAL_BLOCK_ELECINTERFACE "ElectricalInterface"

static const QRegExp REG_BUS_PORTS_COUNT("^(\\d+)([eE](\\d+))?$");
static const QRegExp REG_BUS_PRIORITY("^[0-9]$");
static const QRegExp REG_DOUBLE("^((0\\.\\d+)|(([1-9]{1}\\d*)(\\.\\d+)?))([eE](-?\\d+))?$");
static const int INVALID_PORT_PRIORITY = -1;

BlockTypeSelect::BlockTypeSelect(QSharedPointer<Kcc::BlockDefinition::Model> model, CMA::Operation ope,
                                 bool canChangeType, QWidget *parent)
    : CWidget(parent),
      m_pModel(model),
      m_customDlg(nullptr),
      m_Operation(ope),
      m_canChangeType(canChangeType),
      m_lineEdit(nullptr),
      m_typebutton(nullptr),
      m_editButton(nullptr),
      m_typepath(nullptr),
      m_pBusPortNumLabel(nullptr),
      m_pBusPortNumEdit(nullptr),
      m_priorityLabel(nullptr),
      m_LineEditPriority(nullptr),
      m_pActSelectType(nullptr),
      m_pActRemoveType(nullptr),
      m_nInitPortCount(0)
{
    if (m_pModel != nullptr) {
        m_oldName = m_pModel->getName();
    }

    initUI();
    refreshUIData();
}

BlockTypeSelect::~BlockTypeSelect()
{
    if (m_customDlg != nullptr) {
        delete m_customDlg;
        m_customDlg = nullptr;
    }
}

bool BlockTypeSelect::saveData(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }
    bool bachange = false;
    if (m_oldName != getBlockName()) {
        model->setName(getBlockName());
        LOGOUT(NPS::outputLog(CMA::LogTag_ControlComponent, m_oldName, QObject::tr("Name"), m_oldName, getBlockName()));
        bachange = true;
    }
#ifdef COMPILER_PRODUCT_DESIGNER
    // 子系统单独保存仿真步长
    if (CombineBoardModel::Type == m_pModel->getModelType()) {
        m_pModel->setChildSysStepSize(m_LineEditStepSize->text());
        bachange = true;
    }
#endif
    if (ControlBlock::Type == model->getModelType() || CombineBoardModel::Type == model->getModelType()) {
        int oldpriority = model->getPriority();
        if (m_LineEditPriority != nullptr && !m_LineEditPriority->isHidden() && oldpriority != getNewPriority()) {
            model->setPriority(getNewPriority());
            LOGOUT(NPS::outputLog(CMA::LogTag_ControlComponent, m_oldName, QObject::tr("priority"), oldpriority,
                                  getNewPriority()));
            bachange = true;
        }
        PControlBlock pcblock = model.dynamicCast<ControlBlock>();
        if (pcblock == nullptr || pcblock->isInstance() || NPS::isCombineBlockSubModel(pcblock)) {
            return bachange;
        }

        // 可变端口模块
        if (NPS::VARIABLE_INPUT_PORTS_PROTOTYPENAMES.contains(pcblock->getPrototypeName())
            || NPS::VARIABLE_OUTPUT_PORTS_PROTOTYPENAMES.contains(pcblock->getPrototypeName())
            || pcblock->getPrototypeName() == NPS::PROTOTYPENAME_SYSTRANSFER) {
            int nNewPortsCount = 0;  // 模块新的端口总数量
            int nInitPortsCount = 0; // 模块当前端口总数数量

            if (pcblock->getPortManager() != nullptr) {
                nInitPortsCount = pcblock->getPortManager()->getVariableMap().size();
            }

            if (pcblock->getPrototypeName() == NPS::PROTOTYPENAME_FROMFILE
                || pcblock->getPrototypeName() == NPS::PROTOTYPENAME_TOFILE
                || pcblock->getPrototypeName() == NPS::PROTOTYPENAME_SYSTRANSFER) { // 无固定端口的模块
                auto var = pcblock->getControlPVariable(NPS::KEYWORDS_SYSTRANSFER_BSEND);
                if (var != nullptr) {
                    if (pcblock->getVariableData(var, RoleDataDefinition::ValueRole).toBool()) {
                        if (pcblock->getInputVariableGroup()) {
                            nInitPortsCount = pcblock->getInputVariableGroup()->getVariableMap().size();
                        }
                    } else {
                        if (pcblock->getOutputVariableGroup()) {
                            nInitPortsCount = pcblock->getOutputVariableGroup()->getVariableMap().size();
                        }
                    }
                }
                nNewPortsCount = m_pBusPortNumEdit->text().toInt();
            } else if (pcblock->getPrototypeName()
                       == NPS::PROTOTYPENAME_SIMULABCOMM) { // “*2”操作为输入输出端口同时增加模块
                nNewPortsCount = m_pBusPortNumEdit->text().toInt() * 2;
            } else if (pcblock->getPrototypeName()
                       == NPS::PROTOTYPENAME_MULTIPORTSWITCH) { // “+2”操作为加上MultiportSwitch模块的固定输入输出端口
                int strDataPortDefaultCase =
                        pcblock->getControlVariableValue(NPS::KEYWORDS_MULTIPORTSWITCH_DEFAULTCASE).toInt();
                if (1 == strDataPortDefaultCase) { // "Additional data port"
                    // MultiportSwitch模块有一个固定输入端口和一个附加输入端口
                    nNewPortsCount = m_pBusPortNumEdit->text().toInt() + 3;
                } else {
                    nNewPortsCount = m_pBusPortNumEdit->text().toInt() + 2;
                }
            } else { // “+1”操作为加上模块的固定端口
                nNewPortsCount = m_pBusPortNumEdit->text().toInt() + 1;
            }

            QString strName = pcblock->getPrototypeName();
            if (strName == NPS::PROTOTYPENAME_SYSTRANSFER || strName == NPS::PROTOTYPENAME_MULTIPORTSWITCH) {
                pcblock->updataVariableInfo(nInitPortsCount,
                                            nNewPortsCount); // 端口数量改变，则刷新模块输入输出变量信息
                pcblock->parseDynamicBlockPorts(); // 端口数量改变，根据输入输出变量数量，增减模块输入输出端口数量
                bachange = true;
            } else {
                if (nInitPortsCount != nNewPortsCount) {
                    pcblock->updataVariableInfo(nInitPortsCount,
                                                nNewPortsCount); // 端口数量改变，则刷新模块输入输出变量信息
                    pcblock->parseDynamicBlockPorts(); // 端口数量改变，根据输入输出变量数量，增减模块输入输出端口数量

                    if (getNewPortCount() > 0 && m_nInitPortCount != getNewPortCount()) {
                        bachange = true;
                    }
                }
            }
        }
    }

    return bachange;
}

bool BlockTypeSelect::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (!CMA::checkNameValid(getBlockName(), m_pModel, errorinfo)) {
        return false;
    }
    if (m_pModel == nullptr) {
        return true;
    }

    if (NPS::PROTOTYPENAME_FMU != m_pModel->getPrototypeName()) {
        if (NPS::AT_LEAST_TWO_PORTS_PROTOTYPENAMES.contains(m_pModel->getPrototypeName())) {
            int nPortCount = getNewPortCount();
            if (2 > nPortCount || 300 < nPortCount) {
                // 模块端口数超出了限制范围，有效范围为2-300。请重新输入！
                errorinfo =
                        QObject::tr("The number of module ports exceeds the limit range, with a valid range of 2-300. "
                                    "please re-enter!");
                return false;
            }
        }
    }

    if (NPS::AT_LEAST_ONE_PORT_PROTOTYPENAMES.contains(m_pModel->getPrototypeName())) {
        int nPortCount = getNewPortCount();
        if (1 > nPortCount || 300 < nPortCount) {
            // 模块端口数超出了限制范围，有效范围为1-300。请重新输入！
            errorinfo = QObject::tr("The number of module ports exceeds the limit range, with a valid range of 1-300. "
                                    "please re-enter!");
            return false;
        }
    }
    return true;
}

void BlockTypeSelect::onEditClicked(bool checked)
{
    // zan wu
}

void BlockTypeSelect::onSelectTypeClicked()
{
    if (!m_canChangeType) {
        return;
    }
    ControlBlockTypeSelectView::SelectType selectType = ControlBlockTypeSelectView::SelectType_Invalid;
    if (CMA::Operation_NewCtrInstance == m_Operation) {
        selectType = ControlBlockTypeSelectView::SelectType_ProjectCtrPrototoType;
    } else if (CMA::Operation_EditSlotInstance == m_Operation) {
        selectType = ControlBlockTypeSelectView::SelectType_CtrInstance;
    }
    if (PropertyServerMng::getInstance().propertyServer != nullptr) {
        PModel selectModel =
                PropertyServerMng::getInstance().propertyServer->selectBlock(QObject::tr("Select Module"), selectType);
        if (selectModel != nullptr) {
            if (CMA::Operation_NewCtrInstance == m_Operation) {
                changeInstanceModel(selectModel);
            } else if (CMA::Operation_EditSlotInstance == m_Operation) {
                m_pModel = selectModel;
            } else {
                m_pModel = selectModel;
            }
            refreshUIData();
            emit blockTypeChanged(m_pModel);
        }
    }
}

void BlockTypeSelect::onRemoveTypeClicked()
{
    m_pModel = PModel(nullptr);
    refreshUIData();
    emit blockTypeChanged(m_pModel);
}

void BlockTypeSelect::initUI()
{
    QGridLayout *pgridLayout = new QGridLayout(this);
    pgridLayout->setMargin(0);
    pgridLayout->setVerticalSpacing(8);
    pgridLayout->setHorizontalSpacing(10);

    QLabel *namelabel = new QLabel(this);
    namelabel->setText(QObject::tr("Name")); // 名称
    pgridLayout->addWidget(namelabel, 0, 0, 1, 1);

    m_lineEdit = new KLineEdit(this);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_lineEdit->sizePolicy().hasHeightForWidth());
    m_lineEdit->setSizePolicy(sizePolicy);
    m_lineEdit->setValidator(new QRegExpValidator(QRegExp(NPS::REG_CONTROL_NAME), this));
    if (NPS::isCombineBlockSubModel(m_pModel)) {
        m_lineEdit->setDisabled(true);
    }
    pgridLayout->addWidget(m_lineEdit, 0, 1, 1, 9);
    if (m_canChangeType) {
        QLabel *typelabel = new QLabel(this);
        typelabel->setText(QObject::tr("Type")); // 类型
        pgridLayout->addWidget(typelabel, 1, 0, 1, 1);

        m_typebutton = new QPushButton(this);
        m_typebutton->setMinimumSize(QSize(30, 0));
        m_typebutton->setMaximumSize(QSize(30, 16777215));
        pgridLayout->addWidget(m_typebutton, 1, 1, 1, 1);
        creatTypeMenu();

        // m_editButton = new QPushButton(this);
        // m_editButton->setMinimumSize(QSize(30, 0));
        // m_editButton->setMaximumSize(QSize(30, 16777215));
        // m_editButton->setText(QObject::tr("edit")); // 编辑
        // m_editButton->setEnabled(false);
        // m_editButton->hide();

        // pgridLayout->addWidget(m_editButton, 1, 2, 1, 1);

        m_typepath = new ElideLineText(this);
        pgridLayout->addWidget(m_typepath, 1, 2, 1, 1);
    }
    m_pBusPortNumLabel = new QLabel(this);
    m_pBusPortNumLabel->hide();
    m_pBusPortNumLabel->setText(tr("Ports Count")); // 端口数量
    pgridLayout->addWidget(m_pBusPortNumLabel, 2, 0, 1, 1);

    m_pBusPortNumEdit = new KLineEdit(this);
    m_pBusPortNumEdit->hide();
    m_pBusPortNumEdit->setValidator(new QRegExpValidator(REG_BUS_PORTS_COUNT, this));
    m_pBusPortNumEdit->setObjectName("m_pBusPortNumEdit");
    QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(m_pBusPortNumEdit->sizePolicy().hasHeightForWidth());
    m_pBusPortNumEdit->setSizePolicy(policy);
    m_pBusPortNumEdit->setFixedWidth(50);
    pgridLayout->addWidget(m_pBusPortNumEdit, 2, 1, 1, 1);

    m_priorityLabel = new QLabel(this);
    m_priorityLabel->setText(QObject::tr("Priority")); // 优先级
    m_priorityLabel->hide();
    pgridLayout->addWidget(m_priorityLabel, 3, 0, 1, 1);

    m_LineEditPriority = new KLineEdit(this);
    m_LineEditPriority->setValidator(new QRegExpValidator(REG_BUS_PRIORITY, this));
    m_LineEditPriority->hide();
    m_LineEditPriority->setFixedWidth(50);
    pgridLayout->addWidget(m_LineEditPriority, 3, 1, 1, 1);

    QSpacerItem *vspacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);

#ifdef COMPILER_PRODUCT_DESIGNER
    m_StepSizeLabel = new QLabel(this);
    m_StepSizeLabel->setText(QObject::tr("stepsize")); // 步长
    m_StepSizeLabel->hide();
    pgridLayout->addWidget(m_StepSizeLabel, 4, 0, 1, 1);

    m_LineEditStepSize = new KLineEdit(this);
    m_LineEditStepSize->setValidator(new QRegExpValidator(REG_DOUBLE, this));
    m_LineEditStepSize->hide();
    m_LineEditStepSize->setFixedWidth(50);
    pgridLayout->addWidget(m_LineEditStepSize, 4, 1, 1, 1);

    pgridLayout->addItem(vspacer, 5, 0, 1, 4);
#else
    pgridLayout->addItem(vspacer, 4, 0, 1, 4);
#endif

    this->setLayout(pgridLayout);
}

void BlockTypeSelect::refreshUIData()
{
    if (m_lineEdit != nullptr) {
        if (m_pModel != nullptr) {
            m_lineEdit->setText(m_pModel->getName());
        } else {
            m_lineEdit->setText("");
        }
    }
    setTypeInfo();
#ifdef COMPILER_PRODUCT_DESIGNER
    setUIStepSize();
#endif
    setUIPriority(getInitPriority());
    setUIPortCount(getInitPortCout());
}
void BlockTypeSelect::setUIStepSize()
{
    if (CombineBoardModel::Type == m_pModel->getModelType()) {
        m_StepSizeLabel->show();
        m_LineEditStepSize->show();
        m_LineEditStepSize->setText(m_pModel->getChildSysStepSize());
    }
}
void BlockTypeSelect::creatTypeMenu()
{
    if (m_typebutton == nullptr) {
        return;
    }

    if (!m_canChangeType) {
        m_typebutton->setEnabled(false);
        m_typebutton->hide();
        return;
    }
    m_typebutton->setEnabled(true);
    // create menu
    QMenu *pmenu = new QMenu(this);
    QAction *pActSelectType = new QAction(CMA::SELECT_TYPE, this);
    QAction *pActRemoveType = new QAction(CMA::REMOVE_TYPE, this);
    pmenu->addAction(pActSelectType);
    pmenu->addAction(pActRemoveType);
    m_typebutton->setMenu(pmenu);
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_TitleBarUnshadeButton);
    QPixmap pixmap = icon.pixmap(QSize(20, 20));
    m_typebutton->setIcon(QIcon(pixmap.scaled(QSize(100, 100))));
    m_typebutton->setStyleSheet("QPushButton::menu-indicator{image:none;}");

    // connect(ui.editButton, SIGNAL(clicked(bool)), this, SLOT(onEditClicked(bool)));
    connect(pActSelectType, SIGNAL(triggered()), this, SLOT(onSelectTypeClicked()));
    connect(pActRemoveType, SIGNAL(triggered()), this, SLOT(onRemoveTypeClicked()));
}

void BlockTypeSelect::setTypeInfo()
{
    if (m_typepath == nullptr) {
        return;
    }
    if (m_pModel == nullptr) {
        m_typepath->setElideText("");
    } else {
        m_typepath->setElideText(getPath(m_pModel->getPrototypeName()));
    }
}

QString BlockTypeSelect::getPath(const QString &strName)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || strName.isEmpty()) {
        return QString("");
    }
    QString strProjpath = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
    return QString("%1/%2").arg(strProjpath).arg(strName);
}

QString BlockTypeSelect::getBlockName() const
{
    if (m_lineEdit != nullptr) {
        return m_lineEdit->text().trimmed();
    }
    return QString();
}

void BlockTypeSelect::setUIPortCount(int count)
{
    if (m_pBusPortNumLabel == nullptr || m_pBusPortNumEdit == nullptr) {
        return;
    }

    m_pBusPortNumEdit->setText(QString::number(count));
    if (count < 0) {
        m_pBusPortNumLabel->hide();
        m_pBusPortNumEdit->hide();
    } else {
        m_pBusPortNumEdit->setDisabled(!canChangePort());
        m_pBusPortNumLabel->show();
        m_pBusPortNumEdit->show();
    }
}

int BlockTypeSelect::getNewPortCount()
{
    if (m_pBusPortNumEdit != nullptr && !m_pBusPortNumEdit->isHidden()) {
        return m_pBusPortNumEdit->text().toInt();
    }
    return 0;
}

int BlockTypeSelect::getInitPortCout()
{
    if (m_pModel == nullptr) {
        return INVALID_PORT_PRIORITY;
    }

    if (ControlBlock::Type != m_pModel->getModelType()) {
        return INVALID_PORT_PRIORITY;
    }
    PControlBlock pcblock = m_pModel.dynamicCast<ControlBlock>();
    if (pcblock == nullptr) {
        return INVALID_PORT_PRIORITY;
    }

    if (NPS::VARIABLE_INPUT_PORTS_PROTOTYPENAMES.contains(pcblock->getPrototypeName())) {
        if (pcblock->getPrototypeName() == NPS::PROTOTYPENAME_MULTIPORTSWITCH) {
            int strDataPortDefaultCase =
                    pcblock->getControlVariableValue(NPS::KEYWORDS_MULTIPORTSWITCH_DEFAULTCASE).toInt();

            if (1 == strDataPortDefaultCase) { // "Additional data port"
                // MultiportSwitch模块有一个固定输入端口和一个附加端口
                return pcblock->getInputVariableGroup()->getVariableMap().size() - 2;
            }

            // MultiportSwitch模块有一个固定输入端口
            return pcblock->getInputVariableGroup()->getVariableMap().size() - 1;
        }
        return pcblock->getInputVariableGroup()->getVariableMap().size();
    } else if (NPS::VARIABLE_OUTPUT_PORTS_PROTOTYPENAMES.contains(pcblock->getPrototypeName())) {
        return pcblock->getOutputVariableGroup()->getVariableMap().size();
    } else if (pcblock->getPrototypeName() == NPS::PROTOTYPENAME_SYSTRANSFER) {
        auto var = pcblock->getControlPVariable(NPS::KEYWORDS_SYSTRANSFER_BSEND);
        if (var != nullptr) {
            if (pcblock->getVariableData(var, RoleDataDefinition::ValueRole).toBool()) {
                if (pcblock->getInputVariableGroup()) {
                    return pcblock->getInputVariableGroup()->getVariableMap().size();
                }
                return 0;
            } else {
                if (pcblock->getOutputVariableGroup()) {
                    return pcblock->getOutputVariableGroup()->getVariableMap().size();
                }
                return 0;
            }
        }
    }
    return INVALID_PORT_PRIORITY;
}

bool BlockTypeSelect::canChangePriority()
{
    if (m_pModel == nullptr || m_pModel->isInstance() || NPS::isCombineBlockSubModel(m_pModel)) {
        return false;
    }
    return true;
}

bool BlockTypeSelect::canChangePort()
{
    if (m_pModel == nullptr || m_pModel->isInstance() || NPS::isCombineBlockSubModel(m_pModel)) {
        return false;
    }
    return true;
}

void BlockTypeSelect::setUIPriority(int count)
{
    if (m_priorityLabel == nullptr || m_LineEditPriority == nullptr) {
        return;
    }

    m_LineEditPriority->setText(QString::number(count));
    if (count < 0) {
        m_LineEditPriority->hide();
        m_priorityLabel->hide();
    } else {
        m_LineEditPriority->setDisabled(!canChangePriority());
        m_LineEditPriority->show();
        m_priorityLabel->show();
    }
}

int BlockTypeSelect::getNewPriority() const
{
    if (m_LineEditPriority != nullptr && !m_LineEditPriority->isHidden()) {
        return m_LineEditPriority->text().toInt();
    }

    return INVALID_PORT_PRIORITY;
}

int BlockTypeSelect::getInitPriority()
{
    if (m_pModel == nullptr) {
        return INVALID_PORT_PRIORITY;
    }
    return m_pModel->getPriority();
}

void BlockTypeSelect::changeInstanceModel(QSharedPointer<Kcc::BlockDefinition::Model> protoModel)
{
    if (protoModel == nullptr) {
        return;
    }
    CMA::destoryInstance(m_pModel);
    m_pModel = protoModel->createInstance();
    QString tempname = QObject::tr("%1(1)").arg(protoModel->getName());
    QString errorinfo;
    int j = 2;
    while (!CMA::checkNameValid(tempname, m_pModel, errorinfo)) {
        tempname = QObject::tr("%1(%2)").arg(protoModel->getName()).arg(QString::number(j++));
    }
    m_pModel->setName(tempname);
}

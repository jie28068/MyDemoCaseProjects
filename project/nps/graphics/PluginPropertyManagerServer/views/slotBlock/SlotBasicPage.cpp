#include "SlotBasicPage.h"

#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLWidgets/KCustomDialog.h"
#include "PropertyServerMng.h"
#include "controlblocktypeselectview.h"
#include <QMenu>
#include <QUuid>

USE_LOGOUT_("SlotBasicPage")

static const int HORIZONTAL_HEADER_HEIGHT = 26;
static const int ELEC_PHASE_INVALID = 0;

SlotBasicPage::SlotBasicPage(QSharedPointer<Kcc::BlockDefinition::SlotBlock> pSlotBlock, QWidget *parent)
    : CWidget(parent),
      m_SelectedBlockName(""),
      m_selectModel(nullptr),
      m_SlotBlock(pSlotBlock),
      m_SelectBlockDlg(nullptr)
{
    ui.setupUi(this);
    initUI();
    if (pSlotBlock == nullptr) {
        return;
    }

    ui.lineEditName->setText(pSlotBlock->getName());
    QString blockSelectProto = pSlotBlock->getFilterInfo(RoleDataDefinition::FilerProtoTypeRole).toString();
    if (!blockSelectProto.isEmpty()) {
        ui.blockPath->setElideText(getPath(blockSelectProto));
        ui.groupBoxVar->setVisible(false); // 变量
        m_SelectedBlockName = blockSelectProto;
        m_selectModel = CMA::getModelByPrototype(m_SelectedBlockName);
    } else {
        ui.groupBoxVar->setVisible(true); // 变量
        ui.lineEditInputVar->setText(getVariableNameList(pSlotBlock, RoleDataDefinition::InputSignal).join(","));
        ui.lineEditOutputVar->setText(getVariableNameList(pSlotBlock, RoleDataDefinition::OutputSignal).join(","));
    }
    updatePhaseWidgetSts(m_SlotBlock);
}

SlotBasicPage::~SlotBasicPage()
{
    if (m_SelectBlockDlg != nullptr) {
        delete m_SelectBlockDlg;
        m_SelectBlockDlg = nullptr;
    }
}

bool SlotBasicPage::saveData(const QStringList &inputCheckedList, const QStringList &outputCheckedList)
{
    if (m_SlotBlock == nullptr || ui.groupBoxVar == nullptr || ui.lineEditInputVar == nullptr
        || ui.lineEditOutputVar == nullptr) {
        return false;
    }

    bool bchange = false;
    QString oldname = m_SlotBlock->getName();
    if (getName() != m_SlotBlock->getName()) {
        m_SlotBlock->setName(getName());
        LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, oldname, QObject::tr("Name"), oldname, getName()));
        bchange = true;
    }
    if (m_SlotBlock->isInstance()) {
        m_SlotBlock->setTargetModel(m_selectModel);
        bchange = true;
    } else {
        // 端口锁定信号
        PVariableGroup inputGroup = beginResetGroup(RoleDataDefinition::InputSignal);
        PVariableGroup outputGroup = beginResetGroup(RoleDataDefinition::OutputSignal);
        PVariableGroup pPortGroup = beginResetGroup(RoleDataDefinition::PortGroup);
        if (getPhase() != m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerPhaseNoRole).toInt()
            && m_selectModel != nullptr && ElectricalBlock::Type == m_selectModel->getModelType()) {
            if (ELEC_PHASE_INVALID != m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerPhaseNoRole).toInt()) {
                bchange = true;
            }
            m_SlotBlock->setFilterInfo(RoleDataDefinition::FilerPhaseNoRole, getPhase());
        }
        if (m_SelectedBlockName != m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerProtoTypeRole).toString()) {
            m_SlotBlock->setFilterInfo(RoleDataDefinition::FilerProtoTypeRole, m_SelectedBlockName);
            savePicToSlot(m_selectModel);
            bchange = true;
            if (pPortGroup != nullptr) {
                pPortGroup->clearVariableMap();
            }
            if (inputGroup != nullptr) {
                inputGroup->clearVariableMap();
            }
            if (outputGroup != nullptr) {
                outputGroup->clearVariableMap();
            }
        }
        if (ui.groupBoxVar->isVisible()) {
            if (!m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerBelongTypeRole).toString().isEmpty()) {
                bchange = true;
                m_SlotBlock->setFilterInfo(RoleDataDefinition::FilerBelongTypeRole, "");
            }
            if (saveVariables(getUserDefineList(RoleDataDefinition::InputSignal), RoleDataDefinition::InputSignal)) {
                bchange = true;
            }
            if (saveVariables(getUserDefineList(RoleDataDefinition::OutputSignal), RoleDataDefinition::OutputSignal)) {
                bchange = true;
            }
            sortPortOrder(pPortGroup,
                          QStringList() << getUserDefineList(RoleDataDefinition::InputSignal)
                                        << getUserDefineList(RoleDataDefinition::OutputSignal));
        } else {
            if (saveGroupVariables(RoleDataDefinition::InputSignal, inputCheckedList)) {
                bchange = true;
            }
            if (saveGroupVariables(RoleDataDefinition::OutputSignal, outputCheckedList)) {
                bchange = true;
            }
            sortPortOrder(pPortGroup, QStringList() << inputCheckedList << outputCheckedList);
        }
        // 恢复端口信号
        if (pPortGroup != nullptr) {
            pPortGroup->endResetGroup();
        }
        if (inputGroup != nullptr) {
            inputGroup->endResetGroup();
        }
        if (outputGroup != nullptr) {
            outputGroup->endResetGroup();
        }
    }
    return bchange;
}

QString SlotBasicPage::getSelectBlockDef()
{
    return m_SelectedBlockName;
}

const int SlotBasicPage::getPhase()
{
    if (ui.phasecomboBox != nullptr) {
        return ui.phasecomboBox->currentText().toInt();
    }
    return 0;
}

QSharedPointer<Kcc::BlockDefinition::Model> SlotBasicPage::getSelectModel()
{
    return m_selectModel;
}

bool SlotBasicPage::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (getName() != m_SlotBlock->getName() && !CMA::checkNameValid(getName(), m_SlotBlock, errorinfo)) {
        return false;
    }
    if (ui.groupBoxVar != nullptr && ui.groupBoxVar->isVisible() && ui.lineEditInputVar != nullptr
        && ui.lineEditOutputVar != nullptr) {
        QRegExp variablereg(NPS::REG_VARIABLES);
        if (!ui.lineEditInputVar->text().isEmpty()) {
            bool bmatch = variablereg.exactMatch(ui.lineEditInputVar->text());
            if (!bmatch) {
                // 输入变量[%1]的格式错误，仅数字字母下划线可作为名称，且名称不能以数字开头，多个名称用','隔开！
                errorinfo = tr("Input variables [%1] format is incorrect, only alphanumeric and underline "
                               "can be entered as names, and names cannot start with a number, and "
                               "multiple names are separated by ','!")
                                    .arg(ui.lineEditInputVar->text());
                return false;
            }
        }
        if (!ui.lineEditOutputVar->text().isEmpty()) {
            bool bmatch = variablereg.exactMatch(ui.lineEditOutputVar->text());
            if (!bmatch) {
                // 输出变量[%1]的格式错误，仅数字字母下划线可作为名称，且名称不能以数字开头，多个名称用','隔开！
                errorinfo = tr("Output variables [%1] format is incorrect, only alphanumeric and underline "
                               "can be entered as names, and names cannot start with a number, and "
                               "multiple names are separated by ','!")
                                    .arg(ui.lineEditOutputVar->text());
                return false;
            }
        }
    }

    return true;
}
void SlotBasicPage::setCWidgetReadOnly(bool bReadOnly)
{
    this->setDisabled(bReadOnly);
}

void SlotBasicPage::onEditClicked(bool checked) { }

void SlotBasicPage::onRemoveTypeClicked()
{
    ui.blockPath->setElideText("");
    ui.groupBoxVar->setVisible(true);
    ui.lineEditName->clear();
    m_selectModel = nullptr;
    m_SelectedBlockName = "";
    updatePhaseWidgetSts(m_selectModel);
    emit slotTypeChanged(m_selectModel);
}

void SlotBasicPage::onSelectTypeClicked()
{
    if (PropertyServerMng::getInstance().propertyServer != nullptr) {
        PModel model = PropertyServerMng::getInstance().propertyServer->selectBlock(
                CMA::SELECT_TYPE, ControlBlockTypeSelectView::SelectType_AllPrototoType);
        if (model != nullptr) {
            m_SelectedBlockName = model->getPrototypeName();
            m_selectModel = model;
            ui.blockPath->setElideText(getPath(m_SelectedBlockName));
            ui.groupBoxVar->setVisible(false);
            updatePhaseWidgetSts(m_selectModel);
            QString tempname = QObject::tr("%1(SLOT)").arg(m_SelectedBlockName);
            QString tempname2 = tempname;
            QString errorinfo;
            int j = 1;
            while (!CMA::checkNameValid(tempname2, m_SlotBlock, errorinfo)) {
                tempname2 = tempname + QString::number(j++);
            }

            ui.lineEditName->setText(tempname2);
            emit slotTypeChanged(m_selectModel);
        }
    }
}

void SlotBasicPage::onCurrentTextChanged(const QString &textstr)
{
    emit slotTypeChanged(m_selectModel);
}

void SlotBasicPage::initUI()
{
    ui.gridLayout->setMargin(0);
    ui.lineEditName->setText("");
    ui.lineEditName->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME), this));
    ui.lineEditName->setDisabled(false);
    ui.editButton->setDisabled(true);

    // create menu
    QMenu *pmenu = new QMenu(this);
    QAction *pActSelectType = new QAction(CMA::SELECT_TYPE, this);
    QAction *pActRemoveType = new QAction(CMA::REMOVE_TYPE, this);
    pmenu->addAction(pActSelectType);
    pmenu->addAction(pActRemoveType);
    ui.typebutton->setMenu(pmenu);
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_TitleBarUnshadeButton);
    QPixmap pixmap = icon.pixmap(QSize(20, 20));
    ui.typebutton->setIcon(QIcon(pixmap.scaled(QSize(100, 100))));
    ui.typebutton->setStyleSheet("QPushButton::menu-indicator{image:none;}");

    connect(ui.editButton, SIGNAL(clicked(bool)), this, SLOT(onEditClicked(bool)));
    connect(pActSelectType, SIGNAL(triggered()), this, SLOT(onSelectTypeClicked()));
    connect(pActRemoveType, SIGNAL(triggered()), this, SLOT(onRemoveTypeClicked()));
    connect(ui.phasecomboBox, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(onCurrentTextChanged(const QString &)));
    ui.groupBoxVar->setVisible(false);        // 变量
    ui.groupBoxLowerLimit->setVisible(false); // 下限
    ui.groupBoxUpperLimit->setVisible(false); // 上限
    ui.groupBox_6->setVisible(false);         // 过滤器
    ui.groupBox_2->setVisible(false);         // 分类
    ui.lineEditInputVar->setValidator(new QRegExpValidator(QRegExp(NPS::REG_VARIABLES), ui.lineEditInputVar));
    ui.lineEditOutputVar->setValidator(new QRegExpValidator(QRegExp(NPS::REG_VARIABLES), ui.lineEditOutputVar));
}

QString SlotBasicPage::getPath(const QString &strName)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || strName.isEmpty()) {
        return QString("");
    }
    QString strProjpath = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
    return QObject::tr("%1/%2").arg(strProjpath).arg(strName);
}

QString SlotBasicPage::getName()
{
    if (ui.lineEditName != nullptr) {
        return ui.lineEditName->text().trimmed();
    }
    return QString();
}

void SlotBasicPage::renameName(const QString &selectPrototype)
{
    // Model *pParentModel = peblock->getParentModel();
    // if (pParentModel == nullptr) {
    //     continue;
    // }
}

bool SlotBasicPage::saveVariables(const QStringList &varlist, const QString &groupName)
{
    if (m_SlotBlock == nullptr || groupName.isEmpty()) {
        return false;
    }
    bool bchange = false;
    PVariableGroup pgroup = m_SlotBlock->getVariableGroup(groupName);
    if (pgroup == nullptr) {
        return false;
    }
    PVariableGroup portGroup = m_SlotBlock->getVariableGroup(RoleDataDefinition::PortGroup);
    if (portGroup == nullptr) {
        return false;
    }
    // add
    for (int i = 0; i < varlist.size(); ++i) {
        if (varlist[i].isEmpty()) {
            continue;
        }
        if (pgroup->getVariableByName(varlist[i]) == nullptr) {
            bchange = true;
        } else {
            PVariable tmppavr = pgroup->getVariableByName(varlist[i]);
            if (tmppavr != nullptr && tmppavr->getOrder() != i) {
                m_SlotBlock->setVariableData(tmppavr, RoleDataDefinition::OrderRole, i);
                bchange = true;
                updatePort(m_SlotBlock, portGroup, groupName, tmppavr, SlotBasicPage::PortOpe_UpdateOrder);
            }
            continue;
        }
        // addvariable
        PVariable pvar = pgroup->createVariable();
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::NameRole, varlist[i]);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::DataTypeRole, NPS::DataType_Double);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableControlType,
                                     RoleDataDefinition::ControlTypeDouble);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableControlValue, "(,)");
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::ValueRole, QVariant());
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::OrderRole, i);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedRole, true);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedEnableRole, true);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::ShowModeRole, Variable::VisiableWritable);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedTimeRole, QDateTime::currentDateTime());
        updatePort(m_SlotBlock, portGroup, groupName, pvar, SlotBasicPage::PortOpe_Add);
    }
    // delete
    QList<PVariable> groupvarlist = pgroup->getVariableMap().values();
    for (PVariable pvar : groupvarlist) {
        if (pvar != nullptr && !varlist.contains(pvar->getName())) {
            updatePort(m_SlotBlock, portGroup, groupName, pvar, SlotBasicPage::PortOpe_Delete);
            pgroup->removeVariable(pvar);
            bchange = true;
        }
    }
    return bchange;
}

QStringList SlotBasicPage::getVariableNameList(QSharedPointer<Kcc::BlockDefinition::SlotBlock> pSlotBlock,
                                               const QString &groupname)
{
    if (pSlotBlock == nullptr || groupname.isEmpty()) {
        return QStringList();
    }
    QStringList listdata;
    QList<PVariable> varlist = CMA::getVarGroupList(pSlotBlock, groupname);
    for (PVariable pvar : varlist) {
        if (pvar != nullptr) {
            listdata.append(pvar->getName());
        }
    }
    return listdata;
}

bool SlotBasicPage::saveGroupVariables(const QString &groupname, const QStringList &checkedList)
{
    if (m_SlotBlock == nullptr || groupname.isEmpty()) {
        return false;
    }
    PVariableGroup portGroup = m_SlotBlock->getVariableGroup(RoleDataDefinition::PortGroup);
    if (portGroup == nullptr) {
        return false;
    }
    bool bchange = false;
    PVariableGroup pvargroup = m_SlotBlock->getVariableGroup(groupname);
    if (pvargroup == nullptr) {
        return false;
    }
    if (m_selectModel == nullptr) {
        return bchange;
    }
    if ((ControlBlock::Type == m_selectModel->getModelType()
         || CombineBoardModel::Type == m_selectModel->getModelType())) {
        if (m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerBelongTypeRole).toString()
            != NPS::FilterBelongType_Control) {
            m_SlotBlock->setFilterInfo(RoleDataDefinition::FilerBelongTypeRole, NPS::FilterBelongType_Control);
            bchange = true;
        }
        if (saveControlGroupVariables(getVarMap(CMA::getVarGroupList(m_selectModel, groupname), checkedList), groupname,
                                      checkedList)) {
            bchange = true;
        }
    } else if (ElectricalBlock::Type == m_selectModel->getModelType()) {
        if (m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerBelongTypeRole).toString()
            != NPS::FilterBelongType_Electrical) {
            m_SlotBlock->setFilterInfo(RoleDataDefinition::FilerBelongTypeRole, NPS::FilterBelongType_Electrical);
            bchange = true;
        }
        if (groupname == RoleDataDefinition::InputSignal
            && saveElectricalGroupVariables(PropertyServerMng::getInstance().m_pIElecSysServer->GetInputParamMap(
                                                    m_selectModel->getPrototypeName()),
                                            groupname, checkedList)) {
            bchange = true;
        } else if (groupname == RoleDataDefinition::OutputSignal) {
            PElectricalBlock peblock = m_selectModel.dynamicCast<ElectricalBlock>();
            if (peblock == nullptr) {
                return bchange;
            }
            if (saveElectricalGroupVariables(PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
                                                     peblock->getPrototypeName(), peblock->getPhaseNumber()),
                                             groupname, checkedList)) {
                bchange = true;
            }
        }
    }
    return bchange;
}

bool SlotBasicPage::saveElectricalGroupVariables(const QMap<QString, Kcc::ElecSys::ElecParamDef> &elecvarmap,
                                                 const QString &groupname, const QStringList &checkList)
{
    if (m_SlotBlock == nullptr || groupname.isEmpty()) {
        return false;
    }
    QStringList sortlist = elecvarmap.keys();
    qSort(sortlist.begin(), sortlist.end(),
          [&sortlist](const QString &lhs, const QString &rhs) -> bool { return lhs < rhs; });
    PVariableGroup pvargroup = m_SlotBlock->getVariableGroup(groupname);
    PVariableGroup portGroup = m_SlotBlock->getVariableGroup(RoleDataDefinition::PortGroup);
    if (pvargroup == nullptr) {
        pvargroup = m_SlotBlock->createVariableGroup(groupname);
    }
    if (portGroup == nullptr) {
        return false;
    }
    bool bchange = false;
    // add
    for (int i = 0; i < checkList.size(); ++i) {
        if (checkList[i].isEmpty()) {
            continue;
        }
        if (pvargroup->getVariableByName(checkList[i]) == nullptr) {
            bchange = true;
        } else {
            PVariable tmppavr = pvargroup->getVariableByName(checkList[i]);
            if (tmppavr != nullptr && tmppavr->getOrder() != i) {
                m_SlotBlock->setVariableData(tmppavr, RoleDataDefinition::OrderRole, i);
                bchange = true;
                updatePort(m_SlotBlock, portGroup, groupname, tmppavr, SlotBasicPage::PortOpe_UpdateOrder);
            }
            continue;
        }
        // addvariable
        PVariable pvar = pvargroup->createVariable();
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::NameRole, checkList[i]);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedEnableRole, true);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::DataTypeRole, NPS::DataType_Double);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableControlType,
                                     RoleDataDefinition::ControlTypeDouble);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableControlValue, "(,)");
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::DescriptionRole,
                                     elecvarmap.value(sortlist[i], ElecParamDef()).description);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::ShowModeRole, Variable::VisiableWritable);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::OrderRole, i);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedRole, true);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedTimeRole, QDateTime::currentDateTime());
        updatePort(m_SlotBlock, portGroup, groupname, pvar, SlotBasicPage::PortOpe_Add);
    }
    // delete
    QList<PVariable> groupvarlist = pvargroup->getVariableMap().values();
    for (PVariable pvar : groupvarlist) {
        if (pvar != nullptr && !checkList.contains(pvar->getName())) {
            updatePort(m_SlotBlock, portGroup, groupname, pvar, SlotBasicPage::PortOpe_Delete);
            pvargroup->removeVariable(pvar);
            bchange = true;
        }
    }
    return bchange;
}

bool SlotBasicPage::saveControlGroupVariables(
        const QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>> &checkVarMap, const QString &groupname,
        const QStringList &checkList)
{
    if (m_SlotBlock == nullptr || groupname.isEmpty()) {
        return false;
    }
    PVariableGroup pvargroup = m_SlotBlock->getVariableGroup(groupname);
    PVariableGroup portGroup = m_SlotBlock->getVariableGroup(RoleDataDefinition::PortGroup);
    if (portGroup == nullptr || pvargroup == nullptr) {
        return false;
    }
    bool bchange = false;
    // add
    for (int i = 0; i < checkList.size(); ++i) {
        if (checkList[i].isEmpty()) {
            continue;
        }
        if (pvargroup->getVariableByName(checkList[i]) == nullptr) {
            bchange = true;
        } else {
            PVariable tmppavr = pvargroup->getVariableByName(checkList[i]);
            if (tmppavr != nullptr && tmppavr->getOrder() != i) {
                m_SlotBlock->setVariableData(tmppavr, RoleDataDefinition::OrderRole, i);
                bchange = true;
                updatePort(m_SlotBlock, portGroup, groupname, tmppavr, SlotBasicPage::PortOpe_UpdateOrder);
            }
            continue;
        }
        // addvariable
        PVariable pvar = pvargroup->createVariable();
        PVariable checkVar = checkVarMap[checkList[i]];
        if (checkVar != nullptr) {
            pvar->copyFromVariable(checkVar);
        } else {
            m_SlotBlock->setVariableData(pvar, RoleDataDefinition::DataTypeRole, NPS::DataType_Double);
            m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableControlType,
                                         RoleDataDefinition::ControlTypeDouble);
            m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableControlValue, "(,)");
            m_SlotBlock->setVariableData(pvar, RoleDataDefinition::ValueRole, QVariant());
        }
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::NameRole, checkList[i]);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedEnableRole, false);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::OrderRole, i);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::ShowModeRole, Variable::VisiableWritable);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedRole, true);
        m_SlotBlock->setVariableData(pvar, RoleDataDefinition::VariableCheckedTimeRole, QDateTime::currentDateTime());
        updatePort(m_SlotBlock, portGroup, groupname, pvar, SlotBasicPage::PortOpe_Add);
    }
    // delete
    QList<PVariable> groupvarlist = pvargroup->getVariableMap().values();
    for (PVariable pvar : groupvarlist) {
        if (pvar != nullptr && !checkList.contains(pvar->getName())) {
            updatePort(m_SlotBlock, portGroup, groupname, pvar, SlotBasicPage::PortOpe_Delete);
            pvargroup->removeVariable(pvar);
            bchange = true;
        }
    }
    return bchange;
}

bool SlotBasicPage::updatePort(QSharedPointer<Kcc::BlockDefinition::SlotBlock> slotBlock,
                               QSharedPointer<Kcc::BlockDefinition::VariableGroup> portGroup, const QString &groupname,
                               QSharedPointer<Kcc::BlockDefinition::Variable> pvar, SlotBasicPage::PortOpe ope)
{
    if (slotBlock == nullptr || portGroup == nullptr || pvar == nullptr || groupname.isEmpty()
        || SlotBasicPage::PortOpe_Invalid == ope) {
        return false;
    }

    if (SlotBasicPage::PortOpe_Add == ope) {
        PVariable pPortVar = portGroup->createVariable();
        slotBlock->setVariableData(pPortVar, RoleDataDefinition::NameRole, pvar->getName());
        slotBlock->setVariableData(pPortVar, RoleDataDefinition::OrderRole, pvar->getOrder());
        slotBlock->setVariableData(pPortVar, RoleDataDefinition::DataTypeRole, pvar->getDataType());
        slotBlock->setVariableData(pPortVar, RoleDataDefinition::ShowModeRole, Variable::VisiableReadOnly);
        if (RoleDataDefinition::InputSignal == groupname) {
            slotBlock->setVariableData(pPortVar, RoleDataDefinition::PortType, Variable::ControlIn);
            slotBlock->setVariableData(pPortVar, RoleDataDefinition::PortPresetPosition, QPointF(0, 5));
        } else {
            slotBlock->setVariableData(pPortVar, RoleDataDefinition::PortType, Variable::ControlOut);
            slotBlock->setVariableData(pPortVar, RoleDataDefinition::PortPresetPosition, QPointF(10, 5));
        }
    } else if (SlotBasicPage::PortOpe_Delete == ope) {
        QList<PVariable> varlist = slotBlock->findVariableInGroup(RoleDataDefinition::PortGroup,
                                                                  RoleDataDefinition::NameRole, pvar->getName());
        for (PVariable portVar : varlist) {
            portGroup->removeVariable(portVar);
        }
    } else if (SlotBasicPage::PortOpe_UpdateOrder == ope) {
        QList<PVariable> varlist = slotBlock->findVariableInGroup(RoleDataDefinition::PortGroup,
                                                                  RoleDataDefinition::NameRole, pvar->getName());
        for (PVariable portVar : varlist) {
            portVar->setOrder(pvar->getOrder());
        }
    }

    return true;
}

QSharedPointer<Kcc::BlockDefinition::VariableGroup> SlotBasicPage::beginResetGroup(const QString &groupName)
{
    if (groupName.isEmpty()) {
        return PVariableGroup(nullptr);
    }
    PVariableGroup pPortGroup = m_SlotBlock->getVariableGroup(groupName);
    if (pPortGroup == nullptr) {
        pPortGroup = m_SlotBlock->createVariableGroup(groupName);
    }
    pPortGroup->setAutoSortVariable(false);
    pPortGroup->beginResetGroup();
    return pPortGroup;
}

QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>>
SlotBasicPage::getVarMap(const QList<QSharedPointer<Kcc::BlockDefinition::Variable>> &sortList,
                         const QStringList &checkedList)
{
    QMap<QString, PVariable> checkVarMap;
    for (QString namestr : checkedList) {
        for (PVariable pvar : sortList) {
            if (pvar != nullptr && namestr == pvar->getName()) {
                checkVarMap[namestr] = pvar;
                break;
            }
        }
    }
    return checkVarMap;
}

QStringList SlotBasicPage::getUserDefineList(const QString &groupName)
{
    QStringList listdata;
    if (RoleDataDefinition::InputSignal == groupName) {
        if (ui.lineEditInputVar != nullptr) {
            listdata = ui.lineEditInputVar->text().split(",");
        }
    } else if (RoleDataDefinition::OutputSignal == groupName) {
        if (ui.lineEditOutputVar != nullptr) {
            listdata = ui.lineEditOutputVar->text().split(",");
        }
    }
    QMutableListIterator<QString> iter(listdata);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value().isEmpty()) {
            iter.remove();
            continue;
        }
    }
    return listdata;
}

void SlotBasicPage::sortPortOrder(QSharedPointer<Kcc::BlockDefinition::VariableGroup> portGroup,
                                  const QStringList &portList)
{
    if (portGroup == nullptr || portList.isEmpty()) {
        return;
    }
    for (int i = 0; i < portList.size(); ++i) {
        PVariable pvar = portGroup->getVariableByName(portList[i]);
        if (pvar != nullptr && pvar->getOrder() != i) {
            pvar->setData(RoleDataDefinition::OrderRole, i);
        }
    }
}

bool SlotBasicPage::savePicToSlot(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (m_SlotBlock == nullptr) {
        return false;
    }
    if (model == nullptr || model->getResource().isEmpty()) {
        // 使用插槽的图
        model = CMA::getModelByPrototype(m_SlotBlock->getPrototypeName());
        if (model == nullptr) {
            return false;
        }
    }
    m_SlotBlock->setResource(model->getResource());
    return true;
}

bool SlotBasicPage::canChangePhase(QSharedPointer<Kcc::BlockDefinition::Model> model, QList<int> &phaseList)
{
    phaseList.clear();
    if (model == nullptr || ElectricalBlock::Type != model->getModelType()) {
        return false;
    }
    phaseList =
            PropertyServerMng::getInstance().m_pElecModelCheckServer->getBlockPhaseNumber(model->getPrototypeName());
    qSort(phaseList.begin(), phaseList.end(),
          [&phaseList](const int &lhs, const int &rhs) -> bool { return lhs < rhs; });
    if (phaseList.size() > 1) {
        return true;
    } else {
        return false;
    }
}

void SlotBasicPage::updatePhaseWidgetSts(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    int intPhaseNo = getModelInitPhase(model);
    QList<int> phaseList;
    if (canChangePhase(m_selectModel, phaseList)) {
        ui.phasecomboBox->clear();
        for (int phaseNo : phaseList) {
            ui.phasecomboBox->addItem(QString::number(phaseNo));
        }
        ui.phasecomboBox->setCurrentText(QString::number(intPhaseNo));
        ui.phaseLabel->setVisible(true);
        ui.phasecomboBox->setVisible(true);
    } else {
        ui.phasecomboBox->clear();
        ui.phasecomboBox->addItem(QString::number(intPhaseNo));
        ui.phasecomboBox->setCurrentText(QString::number(intPhaseNo));
        ui.phaseLabel->setVisible(false);
        ui.phasecomboBox->setVisible(false);
    }
}

const int SlotBasicPage::getModelInitPhase(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return ELEC_PHASE_INVALID;
    }
    if (model == m_SlotBlock) {
        QString blockSelectProto = m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerProtoTypeRole).toString();
        if (PropertyServerMng::getInstance().m_ModelManagerServer == nullptr || blockSelectProto.isEmpty()) {
            return ELEC_PHASE_INVALID;
        }
        PModel selectModel = PropertyServerMng::getInstance().m_ModelManagerServer->GetToolkitModel(
                blockSelectProto, KL_TOOLKIT::ELECTRICAL_TOOLKIT);
        if (selectModel == nullptr) {
            return ELEC_PHASE_INVALID;
        }
        int phaseInit = m_SlotBlock->getFilterInfo(RoleDataDefinition::FilerPhaseNoRole).toInt();
        if (phaseInit == ELEC_PHASE_INVALID) {
            return getModelInitPhase(selectModel);
        } else {
            return phaseInit;
        }
    }
    if (ElectricalBlock::Type != model->getModelType()) {
        return ELEC_PHASE_INVALID;
    }
    return model
            ->getValueByRoleData(RoleDataDefinition::ElectricalGroup, RoleDataDefinition::VariableTypeRole,
                                 RoleDataDefinition::PhaseNumber)
            .toUInt();
}

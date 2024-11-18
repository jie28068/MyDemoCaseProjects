#include "simusetwidget.h"

#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "Variable.h"
using namespace Kcc::BlockDefinition;
USE_LOGOUT_("SimuSetWidget")

const QString ERROR_MODEL_NULL = QObject::tr("model is null");
const QString ERROR_VALUE_EXCEED =
        QObject::tr("The WF input value exceeds the range. Please enter a value in the range of (0-1)");

SimuSetWidget::SimuSetWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, QWidget *parent /*= nullptr*/)
    : CWidget(parent), m_pElectricalBlock(block), m_pTableWidget(nullptr), m_pDispersed(nullptr)
{
    initUI();
}

SimuSetWidget::~SimuSetWidget() { }

QList<CustomModelItem> SimuSetWidget::getTableList()
{
    if (m_pTableWidget != nullptr) {
        return m_pTableWidget->getNewListData();
    }
    return QList<CustomModelItem>();
}

DispersedSelect *SimuSetWidget::getDispersedSelect()
{
    return m_pDispersed;
}

bool SimuSetWidget::checkLegitimacy(QString &errorinfo)
{
    if (m_pElectricalBlock == nullptr) {
        errorinfo = ERROR_MODEL_NULL;
        return false;
    }
    errorinfo = "";
    if (m_pDispersed != nullptr && m_pDispersed->getCurrentLineEdit() != nullptr
        && m_pDispersed->getComboBox() != nullptr) {
        if (m_pDispersed->getComboBox()->currentText() == METHOD_WEIGHT_INTEGRAL) {
            QString tmpstr = m_pDispersed->getCurrentLineEdit()->text();
            if (tmpstr.toDouble() <= 0 || tmpstr.toDouble() >= 1) {
                errorinfo = ERROR_VALUE_EXCEED;
                return false;
            }
        }
    }
    if (m_pTableWidget != nullptr && !m_pTableWidget->checkLegitimacy(errorinfo)) {
        return false;
    }
    if (NPS::PROTOTYPENAME_SYNCMACHINE == m_pElectricalBlock->getPrototypeName() && m_pTableWidget != nullptr) {
        CustomModelItem te2f = getItemByKeyWords("T_e2f"); // 磁链方程开始求解时刻
        CustomModelItem tf2m = getItemByKeyWords("T_f2m"); // 转子运动方程开始求解时刻
        CustomModelItem tm2m = getItemByKeyWords("T_m2m"); // 多质量块方程开始求解时刻
        if (tf2m.value.toDouble() < te2f.value.toDouble()) {
            // 仿真设置中参数[%1]的值需要比[%2]的值大，请重新设置！
            errorinfo = QObject::tr("The value of parameter [%1] in simulation settings needs to be greater than or "
                                    "equal to the value of [%2], please reset!")
                                .arg(tf2m.name)
                                .arg(te2f.name);
            return false;
        }
        if (tm2m.value.toDouble() < tf2m.value.toDouble()) {
            // 仿真设置中参数[%1]的值需要比[%2]的值大，请重新设置！
            errorinfo = QObject::tr("The value of parameter [%1] in simulation settings needs to be greater than or "
                                    "equal to the value of [%2], please reset!")
                                .arg(tm2m.name)
                                .arg(tf2m.name);
            return false;
        }
    }
    return true;
}

void SimuSetWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_pDispersed != nullptr) {
        m_pDispersed->setCWidgetReadOnly(bReadOnly);
    }

    if (m_pTableWidget != nullptr) {
        m_pTableWidget->setCWidgetReadOnly(bReadOnly);
    }
}

bool SimuSetWidget::saveData()
{
    if (m_pElectricalBlock == nullptr) {
        return false;
    }

    bool bchange = false;
    QString oldname = m_pElectricalBlock->getName();
    QVariant oldvalue = QVariant();
    if (CMA::saveCustomModeListDataToModel(m_pElectricalBlock, m_pTableWidget->getNewListData(),
                                           RoleDataDefinition::SimulationParameter)) {
        bchange = true;
    }
    PVariable pwfvar = m_pElectricalBlock->getSimulationPVariable(NPS::KEYWORDS_SIMUPARAM_WF);
    if (pwfvar != nullptr
        && m_pElectricalBlock->getSimulationVariableValue(NPS::KEYWORDS_SIMUPARAM_WF) != getWFValue()) {
        bchange = true;
        LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, oldname, pwfvar->getDisplayName(),
                              m_pElectricalBlock->getSimulationVariableValue(NPS::KEYWORDS_SIMUPARAM_WF), getWFValue(),
                              pwfvar->getName(), pwfvar->getControlType(), pwfvar->getControlValue()));
        m_pElectricalBlock->setVariableData(pwfvar, RoleDataDefinition::ValueRole, getWFValue());
    }
    return bchange;
}

void SimuSetWidget::initUI()
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);
    PVariable pvar = m_pElectricalBlock->getSimulationPVariable(NPS::KEYWORDS_SIMUPARAM_WF);
    if (pvar != nullptr) {
        m_pDispersed = new DispersedSelect(this);
        m_pDispersed->setSelectValue(
                m_pElectricalBlock->getVariableData(pvar, RoleDataDefinition::ValueRole).toString());
        m_pDispersed->setMethodLabelName(pvar->getDisplayName());
        gridLayout->addWidget(m_pDispersed, 0, 0, 1, 4);
    }
    m_pTableWidget = new CustomTableWidget(this);
    m_pTableWidget->setListData(CMA::getPropertyModelItemList(m_pElectricalBlock,
                                                              RoleDataDefinition::SimulationParameter, false, "",
                                                              QStringList() << NPS::KEYWORDS_SIMUPARAM_WF));
    gridLayout->addWidget(m_pTableWidget, 1, 0, 1, 4);
}

QVariant SimuSetWidget::getWFValue()
{
    if (m_pDispersed != nullptr && m_pDispersed->getCurrentLineEdit() != nullptr) {
        return m_pDispersed->getCurrentLineEdit()->text();
    }
    return "";
}

CustomModelItem SimuSetWidget::getItemByKeyWords(const QString &keywords)
{
    if (m_pTableWidget != nullptr) {
        QList<CustomModelItem> listdata = m_pTableWidget->getNewListData();
        foreach (CustomModelItem item, listdata) {
            if (keywords == item.keywords) {
                return item;
            }
        }
    }
    return CustomModelItem("", "", "", "", false);
}

InitSetWidget::InitSetWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, QWidget *parent)
    : CustomTableWidget(parent), m_pElectricalBlock(block)
{
    setListData(CMA::getPropertyModelItemList(m_pElectricalBlock, RoleDataDefinition::InitializationParameter));
}

InitSetWidget::~InitSetWidget() { }

bool InitSetWidget::saveData()
{
    return CMA::saveCustomModeListDataToModel(m_pElectricalBlock, getNewListData(),
                                              RoleDataDefinition::InitializationParameter);
    return false;
}

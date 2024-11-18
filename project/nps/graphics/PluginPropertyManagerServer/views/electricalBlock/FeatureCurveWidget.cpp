#include "FeatureCurveWidget.h"
#include "CoreLib/ServerManager.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "PropertyServerMng.h"

using namespace Kcc::BlockDefinition;
USE_LOGOUT_("FeatureCurveWidget")
FeatureCurveWidget::FeatureCurveWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> elecblock, QWidget *parent)
    : CWidget(parent),
      m_electricalBlock(elecblock),
      m_currentDevicetype(nullptr),
      m_tableWidget(nullptr),
      m_tipsLabel(nullptr),
      m_UIGraphWidget(nullptr),
      m_UPGraphWidget(nullptr)
{
    if (elecblock != nullptr) {
        m_currentDevicetype = elecblock->getDeviceModel();
    }
    InitUI();
}

FeatureCurveWidget::~FeatureCurveWidget() { }

bool FeatureCurveWidget::saveData()
{
    bool change = false;
    if (saveVariableData(FCKEY::KEYWORD_T, RoleDataDefinition::Tempeture)) {
        change = true;
    }
    if (saveVariableData(FCKEY::KEYWORD_S, RoleDataDefinition::Illumination)) {
        change = true;
    }
    return change;
}

void FeatureCurveWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_tableWidget != nullptr) {
        m_tableWidget->setCWidgetReadOnly(bReadOnly);
    }
}

bool FeatureCurveWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (m_tableWidget != nullptr) {
        return m_tableWidget->checkLegitimacy(errorinfo);
    }
    return true;
}

void FeatureCurveWidget::updateGraphWidget(QSharedPointer<Kcc::BlockDefinition::DeviceModel> deviceModel)
{
    if (m_UIGraphWidget == nullptr || m_UPGraphWidget == nullptr
        || PropertyServerMng::getInstance().m_pIElecSysServer == nullptr || m_tableWidget == nullptr) {
        return;
    }
    if (deviceModel == nullptr) {
        m_currentDevicetype = nullptr;
        m_UIGraphWidget->setXYDoubleData(QList<double>(), QList<double>(), PoleXY(0, 0));
        m_UPGraphWidget->setXYDoubleData(QList<double>(), QList<double>(), PoleXY(0, 0));
        if (m_tipsLabel != nullptr) {
            m_tipsLabel->show();
        }
        return;
    }
    if (m_tipsLabel != nullptr) {
        m_tipsLabel->hide();
    }
    if (m_currentDevicetype != deviceModel) {
        m_currentDevicetype = deviceModel;
    }
    QList<double> uList;
    QList<double> iList;
    QList<double> pList;
    QMap<QString, QVariant> paraInMap;
    QMap<QString, QList<double>> paraOutMap;
    PVariableGroup group = deviceModel->getVariableGroup(RoleDataDefinition::DeviceTypeParameter);
    if (group != nullptr) {
        for (PVariable var : group->getVariableMap().values()) {
            paraInMap.insert(var->getName(), var->getData(RoleDataDefinition::ValueRole));
        }
    }
    paraInMap.insert(KEYUIP::Keyword_S, m_tableWidget->getItemData(KEYUIP::Keyword_S).value);
    paraInMap.insert(KEYUIP::Keyword_T, m_tableWidget->getItemData(KEYUIP::Keyword_T).value);
    int pptvalue = PropertyServerMng::getInstance().m_pIElecSysServer->MPPT(paraInMap, paraOutMap);
    uList = paraOutMap[KEYUIP::Keyword_U];
    iList = paraOutMap[KEYUIP::Keyword_I];
    pList = paraOutMap[KEYUIP::Keyword_P];
    if (uList.size() <= 0 || iList.size() <= 0 || pList.size() <= 0) {
        m_UIGraphWidget->setXYDoubleData(QList<double>(), QList<double>(), PoleXY(0, 0));
        m_UPGraphWidget->setXYDoubleData(QList<double>(), QList<double>(), PoleXY(0, 0));
        return;
    }

    PoleXY poleUI(uList[uList.size() - 1], iList[iList.size() - 1]);
    PoleXY poleUP(uList[uList.size() - 1], pList[pList.size() - 1]);
    uList.removeLast();
    iList.removeLast();
    pList.removeLast();
    m_UIGraphWidget->setXYDoubleData(uList, iList, poleUI, tr("Voltage(V)"), tr("Current(A)"));
    m_UPGraphWidget->setXYDoubleData(uList, pList, poleUP, tr("Voltage(V)"), tr("Power(W)"));
}

void FeatureCurveWidget::onTableDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem)
{
    updateGraphWidget(m_currentDevicetype);
}

void FeatureCurveWidget::InitUI()
{
    if (m_electricalBlock == nullptr) {
        return;
    }
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);

    m_tableWidget = new CustomTableWidget(this);
    m_tableWidget->setFixedHeight(82);
    QList<CustomModelItem> listdata;
    QVariant tValue = 25;
    QVariant sValue = 1000;
    PVariable tvar = m_electricalBlock->getVariableByRoleData(
            RoleDataDefinition::ElectricalGroup, RoleDataDefinition::VariableTypeRole, RoleDataDefinition::Tempeture);
    PVariable svar = m_electricalBlock->getVariableByRoleData(RoleDataDefinition::ElectricalGroup,
                                                              RoleDataDefinition::VariableTypeRole,
                                                              RoleDataDefinition::Illumination);
    if (tvar != nullptr) {
        tValue = m_electricalBlock->getVariableData(tvar, RoleDataDefinition::ValueRole).toDouble();
    }
    if (svar != nullptr) {
        sValue = m_electricalBlock->getVariableData(svar, RoleDataDefinition::ValueRole).toDouble();
    }
    listdata.append(CustomModelItem(FCKEY::KEYWORD_T, FCKEY::DISPLAYNAME_T, tValue,
                                    RoleDataDefinition::ControlTypeDouble, true, "[-20,80]"));
    listdata.append(CustomModelItem(FCKEY::KEYWORD_S, FCKEY::DISPLAYNAME_S, sValue,
                                    RoleDataDefinition::ControlTypeDouble, true, "[0,2000]"));
    m_tableWidget->setListData(listdata);
    m_tipsLabel = new QLabel(this);
    // 您需要选择一个设备类型来显示特性曲线
    m_tipsLabel->setText(tr("You need to select a device type to display the feature curves"));
    m_UIGraphWidget = new GraphWidget();
    m_UPGraphWidget = new GraphWidget();
    updateGraphWidget(m_currentDevicetype);
    gridLayout->addWidget(m_tableWidget, 0, 0, 1, 1);
    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addWidget(m_UIGraphWidget);
    hlayout->addWidget(m_UPGraphWidget);
    gridLayout->addWidget(m_tipsLabel, 1, 0, 1, 1);
    gridLayout->addLayout(hlayout, 2, 0, 1, 1);
    QSpacerItem *vSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(vSpacer, 3, 0, 1, 1);
    connect(m_tableWidget, &CustomTableWidget::tableDataItemChanged, this, &FeatureCurveWidget::onTableDataItemChanged);
}

bool FeatureCurveWidget::saveVariableData(const QString &keyword, const QString &variableType)
{
    if (m_electricalBlock == nullptr || m_tableWidget == nullptr) {
        return false;
    }
    bool change = false;
    PVariable tvar = m_electricalBlock->getVariableByRoleData(RoleDataDefinition::ElectricalGroup,
                                                              RoleDataDefinition::VariableTypeRole, variableType);
    PVariable svar = m_electricalBlock->getVariableByRoleData(RoleDataDefinition::ElectricalGroup,
                                                              RoleDataDefinition::VariableTypeRole,
                                                              RoleDataDefinition::Illumination);
    QVariant oldValue = m_tableWidget->getItemData(keyword, false).value;
    CustomModelItem item = m_tableWidget->getItemData(keyword, true);
    if (tvar == nullptr) {
        PVariableGroup vargroup = m_electricalBlock->getVariableGroup(RoleDataDefinition::ElectricalGroup);
        if (vargroup == nullptr) {
            vargroup = m_electricalBlock->createVariableGroup(RoleDataDefinition::ElectricalGroup);
        }
        if (vargroup == nullptr) {
            return false;
        }
        tvar = vargroup->createVariable();
        if (tvar == nullptr) {
            return false;
        }
        tvar->setName(keyword);
        tvar->setData(RoleDataDefinition::VariableTypeRole, variableType);
        tvar->setData(RoleDataDefinition::ValueRole, item.value);
        if (oldValue.toDouble() != item.value.toDouble()) {
            LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_electricalBlock->getName(), item.name, oldValue,
                                  item.value, item.keywords, item.valueType, item.controlLimit));
        }
        change = true;
    } else {
        if (oldValue.toDouble() != item.value.toDouble()) {
            tvar->setData(RoleDataDefinition::ValueRole, item.value);
            LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_electricalBlock->getName(), item.name, oldValue,
                                  item.value, item.keywords, item.valueType, item.controlLimit));
            change = true;
        }
    }
    return change;
}

#include "powerflowparamwidget.h"

#include "CommonModelAssistant.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include <cmath>
#include <complex>

static const double INVALID_VOLTAGE = -1.0;
static const int INVALID_BASEMVA = 0;
static const int INVALID_FREQUENCY = 0;

using std::complex;
using std::conj;
// 注册及请求服务所需要的头文件
#include "CoreLib/ServerManager.h"
USE_LOGOUT_("PowerFlowParamWidget")

PowerFlowParamWidget::PowerFlowParamWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock,
                                           const QList<CustomModelItem> &paramlist, QWidget *parent /*= nullptr*/)
    : CustomTableWidget(parent), m_pElecBlock(peblock), m_tmpDeviceModel(nullptr), m_curParamList(paramlist)
{
    m_oldList.clear();
    m_oldList = getTideList();
    if (m_pElecBlock != nullptr) {
        m_tmpDeviceModel = m_pElecBlock->getDeviceModel();
    }
    setListData(m_oldList);
}

PowerFlowParamWidget::~PowerFlowParamWidget() { }

void PowerFlowParamWidget::updatePFList(const QList<CustomModelItem> &paramlist,
                                        QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype)
{
    if (m_pElecBlock != nullptr
        && (NPS::PROTOTYPENAME_TWTRANSFORMER == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_NITWTRANSFORMER == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_LINE == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_PI_LINE == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_PI_LINKLINE == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_BUSBAR == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_DOTBUSBAR == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_SHUNTFILTER == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_INDUCTOR == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_CAPACITOR == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_RESISTOR == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_LOAD == m_pElecBlock->getPrototypeName()
            || NPS::PROTOTYPENAME_CPLOAD == m_pElecBlock->getPrototypeName())) {
        setListData(getTideList(paramlist, true, devicetype));
    }
}

bool PowerFlowParamWidget::saveData()
{
    if (m_pElecBlock == nullptr) {
        return false;
    }
    QList<CustomModelItem> pflist = getNewListData();
    QMutableListIterator<CustomModelItem> iter(pflist);
    while (iter.hasNext()) {
        iter.next();
        if (!iter.value().isValid()) {
            iter.remove();
            continue;
        }
        if (iter.value().otherPropertyMap[KEY_GROUPNAME].toString() != RoleDataDefinition::LoadFlowParameter) {
            iter.remove();
            continue;
        }
    }
    return CMA::saveCustomModeListDataToModel(m_pElecBlock, pflist, RoleDataDefinition::LoadFlowParameter);
}

void PowerFlowParamWidget::onDeviceTypeChanged(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype)
{
    m_tmpDeviceModel = devicetype;
    updatePFList(m_curParamList, m_tmpDeviceModel);
}

void PowerFlowParamWidget::onParamChanged(const QList<CustomModelItem> &paramlist)
{
    m_curParamList = paramlist;
    updatePFList(m_curParamList, m_tmpDeviceModel);
}

QList<CustomModelItem> PowerFlowParamWidget::getTideList(const QList<CustomModelItem> &paramlist, bool bfromout,
                                                         QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype)
{
    if (m_pElecBlock == nullptr || m_pElecBlock->getLoadFlowVariableGroup() == nullptr) {
        return QList<CustomModelItem>();
    }
    QList<CustomModelItem> listdata =
            CMA::getPropertyModelItemList(m_pElecBlock, RoleDataDefinition::LoadFlowParameter);
    QSharedPointer<DeviceModel> tmpdevicetype = m_pElecBlock->getDeviceModel();
    if (bfromout) {
        tmpdevicetype = devicetype;
    }
    // 线路的潮流计算结果需要计算
    if (NPS::PROTOTYPENAME_LINE == m_pElecBlock->getPrototypeName()
        || NPS::PROTOTYPENAME_PI_LINE == m_pElecBlock->getPrototypeName()
        || NPS::PROTOTYPENAME_PI_LINKLINE == m_pElecBlock->getPrototypeName()) {
        QString r, x, b;
        getLineRXB(m_pElecBlock, tmpdevicetype,
                   getKeyValue(paramlist,
                               NPS::PROTOTYPENAME_LINE == m_pElecBlock->getPrototypeName() ? NPS::KEYWORDS_LENTH
                                                                                           : NPS::KEYWORDS_PILINE_LEN,
                               bfromout, RoleDataDefinition::ElectricalParameter)
                           .toDouble(),
                   r, x, b);
        setItemValue(listdata, NPS::KEYWORDS_R, r);
        setItemValue(listdata, NPS::KEYWORDS_X, x);
        setItemValue(listdata, NPS::KEYWORDS_B, b);
    } else if (NPS::PROTOTYPENAME_INDUCTOR == m_pElecBlock->getPrototypeName()) {
        // 电感元件（电抗）x=l/Z，
        double Zb = getICRZBValue(m_pElecBlock);
        if (!CMA::isEqualO(Zb)) {
            double val = getKeyValue(paramlist, NPS::KEYWORDS_INDUCTOR_VAL, bfromout,
                                     RoleDataDefinition::ElectricalParameter)
                                 .toDouble();

            setItemValue(listdata, NPS::KEYWORDS_X, NPS::doubleToString15(val / Zb));
        }

    } else if (NPS::PROTOTYPENAME_CAPACITOR == m_pElecBlock->getPrototypeName()) {
        // 电容元件(电纳)b=-c*Z
        double Zb = getICRZBValue(m_pElecBlock);
        if (!CMA::isEqualO(Zb)) {
            double val = getKeyValue(paramlist, NPS::KEYWORDS_CAPACITOR_VAL, bfromout,
                                     RoleDataDefinition::ElectricalParameter)
                                 .toDouble();
            setItemValue(listdata, NPS::KEYWORDS_B, NPS::doubleToString15(-val * Zb));
        }
    } else if (NPS::PROTOTYPENAME_RESISTOR == m_pElecBlock->getPrototypeName()) {
        ////电阻计算：r = R/Z
        double Zb = getICRZBValue(m_pElecBlock);
        if (!CMA::isEqualO(Zb)) {
            double val = getKeyValue(paramlist, NPS::KEYWORDS_RESISTOR_VAL, bfromout,
                                     RoleDataDefinition::ElectricalParameter)
                                 .toDouble();
            setItemValue(listdata, NPS::KEYWORDS_R, NPS::doubleToString15(val / Zb));
        }
    } else if (NPS::PROTOTYPENAME_BUSBAR == m_pElecBlock->getPrototypeName()
               || NPS::PROTOTYPENAME_DOTBUSBAR == m_pElecBlock->getPrototypeName()) {
        // 母线的参数
        QVariant voltage = m_pElecBlock->getElectricalVariableValue(NPS::KEYWORDS_BUSBAR_VOLTAGE);
        for (CustomModelItem item : paramlist) {
            if (NPS::KEYWORDS_BUSBAR_VOLTAGE == item.keywords) {
                voltage = item.value;
                break;
            }
        }
        setItemValue(listdata, NPS::KEYWORDS_BUSBAR_BASEKV, voltage);
    } else if (NPS::PROTOTYPENAME_TWTRANSFORMER == m_pElecBlock->getPrototypeName()) {
        // 相移角angle计算YY：0度 YD（高压侧Y，低压侧D）：30度 DY（高压侧D，低压侧Y）： - 30度 DD：0度
        if (tmpdevicetype != nullptr) {
            setItemValue(
                    listdata, NPS::KEYWORDS_TWT_X,
                    NPS::doubleToString15(getPFX(
                            m_pElecBlock, tmpdevicetype->getDeviceTypeVariableValue(NPS::KEYWORDS_TWT_LS).toDouble())));
            setItemValue(listdata, NPS::KEYWORDS_TWT_ANGLE,
                         getAngleValue(m_pElecBlock->getPrototypeName(), tmpdevicetype));
            // setItemValue(listdata, NPS::KEYWORDS_TWT_RATIO,
            //              NPS::doubleToString15(getRatio(m_pElecBlock, tmpdevicetype)));
        } else {
            setItemValue(listdata, NPS::KEYWORDS_TWT_X, 0.1);
            // setItemValue(listdata, NPS::KEYWORDS_TWT_RATIO, 0.1);
        }
    } else if (NPS::PROTOTYPENAME_NITWTRANSFORMER == m_pElecBlock->getPrototypeName()) {
        // 相移角angle计算YY：0度 YD（高压侧Y，低压侧D）：30度 DY（高压侧D，低压侧Y）： - 30度 DD：0度
        if (tmpdevicetype != nullptr) {
            setItemValue(listdata, NPS::KEYWORDS_NITWT_ANGLE,
                         getAngleValue(m_pElecBlock->getPrototypeName(), tmpdevicetype));
            // setItemValue(listdata, NPS::KEYWORDS_NITWT_RATIO,
            //              NPS::doubleToString15(getRatio(m_pElecBlock, tmpdevicetype)));
        }
    } else if (NPS::PROTOTYPENAME_SHUNTFILTER == m_pElecBlock->getPrototypeName()) {
        // R-L-C=1,R-L=2,R-C=3,R-L-C|R=4,R-L-C-C|R=5
        double spvalue = 0.0;
        double sqvalue = 0.0;
        getShuntFilter(paramlist, spvalue, sqvalue, bfromout);
        setItemValue(listdata, NPS::KEYWORDS_SHUNTFILTER_P, NPS::doubleToString15(spvalue));
        setItemValue(listdata, NPS::KEYWORDS_SHUNTFILTER_Q, NPS::doubleToString15(sqvalue));
    } else if (NPS::PROTOTYPENAME_LOAD == m_pElecBlock->getPrototypeName()) {
        double spvalue = 0;
        double sqvalue = 0;
        getLoadPQ(tmpdevicetype, spvalue, sqvalue);
        setItemValue(listdata, NPS::KEYWORDS_LOAD_PD, NPS::doubleToString15(spvalue));
        setItemValue(listdata, NPS::KEYWORDS_LOAD_QD, NPS::doubleToString15(sqvalue));
    } else if (NPS::PROTOTYPENAME_CPLOAD == m_pElecBlock->getPrototypeName()) {
        if (tmpdevicetype != nullptr) {
            setItemValue(listdata, NPS::KEYWORDS_LOAD_PD,
                         tmpdevicetype->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter,
                                                           RoleDataDefinition::NameRole, NPS::KEYWORDS_CPLOAD_P));
            setItemValue(listdata, NPS::KEYWORDS_LOAD_QD,
                         tmpdevicetype->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter,
                                                           RoleDataDefinition::NameRole, NPS::KEYWORDS_CPLOAD_Q));
        } else {
            setItemValue(listdata, NPS::KEYWORDS_LOAD_PD, 0);
            setItemValue(listdata, NPS::KEYWORDS_LOAD_QD, 0);
        }
    }
    // 添加潮流计算结果显示
    listdata.append(CMA::getPropertyModelItemList(m_pElecBlock, RoleDataDefinition::LoadFlowResultParameter, true));
    return listdata;
}

int PowerFlowParamWidget::getCurrentProjBaseVMA()
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return INVALID_BASEMVA;
    }
    return PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::SB).toInt();
}

int PowerFlowParamWidget::getCurrentProjFrequency()
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return INVALID_FREQUENCY;
    }
    return PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::FREQUENCY).toInt();
}

int PowerFlowParamWidget::getAngleValue(const QString &prototypename,
                                        QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype)
{
    if ((NPS::PROTOTYPENAME_TWTRANSFORMER != prototypename && NPS::PROTOTYPENAME_NITWTRANSFORMER != prototypename)
        || devicetype == nullptr) {
        return 0;
    }
    QString hconstr = getConnectionStr(devicetype, NPS::KEYWORDS_TRANSFORMER_HCONNECTION);
    QString lconstr = getConnectionStr(devicetype, NPS::KEYWORDS_TRANSFORMER_LCONNECTION);
    if ((NPS::CONNECTIONTYPE_D == hconstr && NPS::CONNECTIONTYPE_D == lconstr)
        || (NPS::CONNECTIONTYPE_Y == hconstr && NPS::CONNECTIONTYPE_Y == lconstr)) {
        return 0;
    } else if (NPS::CONNECTIONTYPE_Y == hconstr && NPS::CONNECTIONTYPE_D == lconstr) {
        return 30;
    } else if (NPS::CONNECTIONTYPE_D == hconstr && NPS::CONNECTIONTYPE_Y == lconstr) {
        return -30;
    }
    return 0;
}

QString PowerFlowParamWidget::getConnectionStr(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype,
                                               const QString &keyword)
{
    if (devicetype == nullptr) {
        return QString();
    }
    PVariable pvar = devicetype->getDeviceTypePVariable(keyword);
    if (pvar == nullptr) {
        return QString();
    }
    return NPS::RealValueMaptoControlValue(pvar->getDefaultValue(), pvar->getControlType(), pvar->getControlValue())
            .toString();
}

void PowerFlowParamWidget::setItemValue(QList<CustomModelItem> &listdata, const QString &keyword, const QVariant &value)
{
    for (CustomModelItem &item : listdata) {
        if (keyword == item.keywords) {
            item.value = value;
            break;
        }
    }
}

void PowerFlowParamWidget::getShuntFilter(const QList<CustomModelItem> paramlist, double &pvalue, double &qvalue,
                                          bool bfromout)
{
    // R-L-C=1,R-L=2,R-C=3,R-L-C|R=4,R-L-C-C|R=5
    pvalue = 0.0;
    qvalue = 0.0;
    if (m_pElecBlock == nullptr || m_pElecBlock->getParentModel() == nullptr) {
        return;
    }
    // 基准容量
    QList<PModel> adjModels = m_pElecBlock->getParentModel()->getAdjacentChild(m_pElecBlock->getUUID());
    if (adjModels.size() <= 0 || adjModels[0] == nullptr) {
        return;
    }
    int bustype = adjModels[0]
                          ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                               NPS::KEYWORDS_BUSBAR_AC)
                          .toInt();
    int Uvalue = adjModels[0]
                         ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                              NPS::KEYWORDS_BUSBAR_VOLTAGE)
                         .toDouble()
            / sqrt(3);
    if (bustype == 1) {
        Uvalue = 3 * Uvalue;
    }
    int frequency = getCurrentProjFrequency();
    double Rvalue =
            getKeyValue(paramlist, NPS::KEYWORDS_SHUNTFILTER_R, bfromout, RoleDataDefinition::ElectricalParameter)
                    .toDouble();
    double Lvalue =
            getKeyValue(paramlist, NPS::KEYWORDS_SHUNTFILTER_L, bfromout, RoleDataDefinition::ElectricalParameter)
                    .toDouble();
    double Cvalue =
            getKeyValue(paramlist, NPS::KEYWORDS_SHUNTFILTER_C, bfromout, RoleDataDefinition::ElectricalParameter)
                    .toDouble();
    double Rpvalue =
            getKeyValue(paramlist, NPS::KEYWORDS_SHUNTFILTER_RP, bfromout, RoleDataDefinition::ElectricalParameter)
                    .toDouble();
    double Csvalue =
            getKeyValue(paramlist, NPS::KEYWORDS_SHUNTFILTER_CS, bfromout, RoleDataDefinition::ElectricalParameter)
                    .toDouble();
    int ShuntTypevalue = getKeyValue(paramlist, NPS::KEYWORDS_SHUNTFILTER_SHUNTTYPE, bfromout,
                                     RoleDataDefinition::ElectricalParameter)
                                 .toInt();
    double ClcR = 0.0;
    double clcX = 0.0;
    double ClcW = 2 * M_PI * frequency;
    switch (ShuntTypevalue) {
    case NPS::SHUNTTYPE_RLC: {
        if (!CMA::isEqualO(ClcW) && !CMA::isEqualO(Cvalue)) {
            ClcR = Rvalue;
            clcX = ClcW * Lvalue - 1 / (ClcW * Cvalue);
        }
        break;
    }
    case NPS::SHUNTTYPE_RL: {
        ClcR = Rvalue;
        clcX = ClcW * Lvalue;
        break;
    }
    case NPS::SHUNTTYPE_RC: {
        if (!CMA::isEqualO(ClcW) && !CMA::isEqualO(Cvalue)) {
            ClcR = Rvalue;
            clcX = -1 / (ClcW * Cvalue);
        }
        break;
    }
    case NPS::SHUNTTYPE_RLC_R: {
        if (!CMA::isEqualO(ClcW) && !CMA::isEqualO(Cvalue) && !CMA::isEqualO(Rvalue + Rpvalue)
            && !CMA::isEqualO(ClcW * Lvalue)) {
            ClcR = (pow(Rvalue, 2) * Rpvalue + Rvalue * pow(Rpvalue, 2) + pow(ClcW, 2) * pow(Lvalue, 2) * Rpvalue)
                    / (pow(Rvalue + Rpvalue, 2) + pow(ClcW * Lvalue, 2));
            clcX = (ClcW * Lvalue * pow(Rpvalue, 2)) / (pow(Rvalue + Rpvalue, 2) + pow(ClcW * Lvalue, 2))
                    - 1 / (ClcW * Cvalue);
        }
        break;
    }
    case NPS::SHUNTTYPE_RLCC_R: {
        if (!CMA::isEqualO(ClcW) && !CMA::isEqualO(Cvalue) && !CMA::isEqualO(Rvalue + Rpvalue)
            && !CMA::isEqualO(ClcW * Lvalue)) {
            ClcR = (pow(Rvalue, 2) * Rpvalue + Rvalue * pow(Rpvalue, 2) + pow(ClcW, 2) * pow(Lvalue, 2) * Rpvalue
                    - 2 * Rpvalue * Lvalue / Cvalue + Rpvalue / (pow(ClcW * Cvalue, 2)))
                    / (pow(Rvalue + Rpvalue, 2) + pow((ClcW * Lvalue) - 1 / (ClcW * Cvalue), 2));
            clcX = (ClcW * Lvalue * pow(Rpvalue, 2) - pow(Rpvalue, 2) / (ClcW * Cvalue))
                            / (pow(Rvalue + Rpvalue, 2) + pow(ClcW * Lvalue - 1 / (ClcW * Cvalue), 2))
                    - 1 / (ClcW * Csvalue);
        }
        break;
    }
    default:
        break;
    }
    if (!CMA::isEqualO(ClcR) && !CMA::isEqualO(clcX)) {
        pvalue = pow(Uvalue, 2) * ClcR / (pow(ClcR, 2) + pow(clcX, 2));
        qvalue = -pow(Uvalue, 2) * clcX / (pow(ClcR, 2) + pow(clcX, 2));
    }
}

QVariant PowerFlowParamWidget::getKeyValue(const QList<CustomModelItem> &paramlist, const QString &keyword,
                                           bool bfromout, const QString &groupName)
{
    if (keyword.isEmpty()) {
        return QVariant();
    }
    if (bfromout) {
        for (CustomModelItem item : paramlist) {
            if (keyword == item.keywords) {
                return item.value;
            }
        }
        return QVariant();
    }
    if (m_pElecBlock == nullptr) {
        return QVariant();
    }
    return m_pElecBlock->getValueByRoleData(groupName, RoleDataDefinition::NameRole, keyword);
}

void PowerFlowParamWidget::getLoadPQ(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype, double &pvalue,
                                     double &qvalue)
{
    if (devicetype == nullptr) {
        pvalue = 0;
        qvalue = 0;
        return;
    }
    double loadR = devicetype
                           ->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter, RoleDataDefinition::NameRole,
                                                NPS::KEYWORDS_LOAD_R)
                           .toDouble();
    double LoadL = devicetype
                           ->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter, RoleDataDefinition::NameRole,
                                                NPS::KEYWORDS_LOAD_L)
                           .toDouble();
    double Vrated = devicetype
                            ->getValueByRoleData(RoleDataDefinition::DeviceTypeParameter, RoleDataDefinition::NameRole,
                                                 NPS::KEYWORDS_LOAD_VRATED)
                            .toDouble();
    int frequency = getCurrentProjFrequency();
    double U = Vrated / sqrt(3);
    double w = 2 * M_PI * frequency;
    auto Z = complex<double>(0.0, w * loadR * LoadL) / complex<double>(loadR, w * LoadL);
    double R = Z.real();
    double X = Z.imag();
    auto PQ = U * conj<double>(U / complex<double>(R, X));
    pvalue = PQ.real() * 3;
    qvalue = PQ.imag() * 3;
}

double PowerFlowParamWidget::getICRZBValue(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model)
{
    if (model == nullptr || model->getParentModel() == nullptr) {
        return 0.0;
    }
    // 基准容量
    int baseMVA = getCurrentProjBaseVMA();
    if (baseMVA == INVALID_BASEMVA) {
        return 0.0;
    }
    QList<PModel> adjModels = model->getParentModel()->getAdjacentChild(model->getUUID());
    for (PModel model : adjModels) {
        if (model == nullptr) {
            continue;
        }
        double vol = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                               NPS::KEYWORDS_BUSBAR_VOLTAGE)
                             .toDouble();
        if (CMA::isEqualO(vol)) {
            return 0.0;
        }
        return pow(vol, 2) / baseMVA;
    }
    return 0.0;
}

double PowerFlowParamWidget::getPFX(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model, const double &LS)
{
    if (model == nullptr) {
        return 0.0;
    }

    int baseMVA = getCurrentProjBaseVMA();
    if (baseMVA == 0) {
        return 0.0;
    }

    int frequency = getCurrentProjFrequency();
    double Vb = getHFBUSVoltage(m_pElecBlock);
    double Zb = pow(Vb, 2) / baseMVA;
    if (CMA::isEqualO(Zb)) {
        return 0.0;
    }
    return 2 * M_PI * frequency * LS / Zb;
}

double PowerFlowParamWidget::getHFBUSVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model)
{
    if (model == nullptr || model->getVariableGroup(RoleDataDefinition::PortGroup) == nullptr) {
        return 0.0;
    }

    PModel pParentModel = model->getParentModel();
    if (pParentModel.isNull()) {
        return 0.0;
    }
    QList<PBlockConnector> adjConnectors = pParentModel->getAdjacentConnector(model->getUUID());
    PVariable HFVar = model->getVariableByRoleData(RoleDataDefinition::PortGroup, RoleDataDefinition::NameRole,
                                                   NPS::KEYWORDS_TWT_HFBUS);
    if (HFVar == nullptr) {
        return 0.0;
    }

    for (PBlockConnector pcon : adjConnectors) {
        if (pcon->getDstModel() != nullptr && (pcon->getDstModel()->getUUID() == model->getUUID())
            && HFVar->getUUID() == pcon->getDstPortUuid()) {
            // dstblock就是连着当前端口，另一端就是连接的模块
            if (pcon->getSrcModel() != nullptr) {
                return pcon->getSrcModel()
                        ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                             NPS::KEYWORDS_BUSBAR_VOLTAGE)
                        .toDouble();
            }
        } else if (pcon->getSrcModel() != nullptr && (pcon->getSrcModel()->getUUID() == model->getUUID())
                   && HFVar->getUUID() == pcon->getSrcPortUuid()) {
            // srcBlock就是连着当前端口，另一端就是连接的模块
            if (pcon->getDstModel() != nullptr) {
                return pcon->getDstModel()
                        ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                             NPS::KEYWORDS_BUSBAR_VOLTAGE)
                        .toDouble();
            }
        }
    }

    return 0.0;
}

void PowerFlowParamWidget::getLineRXB(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> elecModel,
                                      QSharedPointer<Kcc::BlockDefinition::DeviceModel> devmodel,
                                      const double &lineLength, QString &r, QString &x, QString &b)
{
    r = "0";
    x = "0";
    b = "0";
    if (elecModel == nullptr || devmodel == nullptr) {
        return;
    }
    double rp = 0;
    double lp = 0;
    double cp = 0;
    if (NPS::PROTOTYPENAME_LINE == elecModel->getPrototypeName()) {
        rp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_RP).toDouble();
        lp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_LP).toDouble();
        cp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_CP).toDouble();
    } else {
        rp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_PILINE_R1).toDouble();
        lp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_PILINE_L1).toDouble();
        cp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_PILINE_C1).toDouble();
    }
    // 基准容量
    int baseMVA = getCurrentProjBaseVMA();
    int frequency = getCurrentProjFrequency();
    if (baseMVA == INVALID_BASEMVA || frequency == INVALID_FREQUENCY) {
        return;
    }
    PModel pParentModel = elecModel->getParentModel();
    if (pParentModel.isNull()) {
        return;
    }
    QList<PModel> adjModels = pParentModel->getAdjacentChild(elecModel->getObjectUuid());
    if (adjModels.size() <= 0 || adjModels[0] == nullptr) {
        return;
    }
    double vol = adjModels[0]
                         ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                              NPS::KEYWORDS_BUSBAR_VOLTAGE)
                         .toDouble();
    if (vol == 0) {
        return;
    }
    // 标幺值Zb
    double Zb = pow(vol, 2) / baseMVA;
    // 电阻 r = r1*长度/Z标幺值
    r = NPS::doubleToString15(rp * lineLength / Zb);
    // 电抗 x = x1*长度/Z标幺值
    x = NPS::doubleToString15(2 * M_PI * frequency * lp * lineLength / Zb);
    // 电纳 b = b1*长度*Z标幺值
    b = NPS::doubleToString15(2 * M_PI * frequency * cp * lineLength * Zb);
}

// double PowerFlowParamWidget::getRatio(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock,
//                                       QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype)
// {
//     if (peblock == nullptr || peblock->getVariableGroup(RoleDataDefinition::PortGroup) == nullptr
//         || devicetype == nullptr) {
//         return 0.0;
//     }

//     PModel pParentModel = peblock->getParentModel();
//     if (pParentModel.isNull()) {
//         return 0.0;
//     }
//     QMap<QString, PElectricalBlock> busMap;
//     auto connectlist = pParentModel->getConnectors();
//     QList<PVariable> portlist = CMA::getVarGroupList(peblock, RoleDataDefinition::PortGroup);
//     for (PVariable portVar : portlist) {
//         if (portVar == nullptr) {
//             continue;
//         }
//         for (PBlockConnector pcon : connectlist) {
//             if (pcon->getDstModel() != nullptr && (pcon->getDstModel()->getUUID() == peblock->getUUID())
//                 && portVar->getUUID() == pcon->getDstPortUuid()) {
//                 // dstblock就是连着当前端口，另一端就是连接的模块
//                 if (pcon->getSrcModel() != nullptr && !busMap.contains(portVar->getName())) {
//                     busMap.insert(portVar->getName(), (pcon->getSrcModel()).dynamicCast<ElectricalBlock>());
//                     break;
//                 }
//             } else if (pcon->getSrcModel() != nullptr && (pcon->getSrcModel()->getUUID() == peblock->getUUID())
//                        && portVar->getUUID() == pcon->getSrcPortUuid()) {
//                 // srcBlock就是连着当前端口，另一端就是连接的模块
//                 if (pcon->getDstModel() != nullptr && !busMap.contains(portVar->getName())) {
//                     busMap.insert(portVar->getName(), (pcon->getDstModel()).dynamicCast<ElectricalBlock>());
//                     break;
//                 }
//             }
//         }
//     }
//     double highVol = getBusBarVoltage(busMap[NPS::KEYWORDS_TWT_HFBUS]);
//     double lowVol = getBusBarVoltage(busMap[NPS::KEYWORDS_TWT_LFBUS]);
//     if (highVol == 0.0 || lowVol == 0.0) {
//         return 0.0;
//     }
//     QString hconstr = getConnectionStr(devicetype, NPS::KEYWORDS_TRANSFORMER_HCONNECTION);
//     QString lconstr = getConnectionStr(devicetype, NPS::KEYWORDS_TRANSFORMER_LCONNECTION);
//     if ((NPS::CONNECTIONTYPE_D == hconstr && NPS::CONNECTIONTYPE_D == lconstr)
//         || (NPS::CONNECTIONTYPE_Y == hconstr && NPS::CONNECTIONTYPE_Y == lconstr)) {
//         return highVol / lowVol;
//     } else if (NPS::CONNECTIONTYPE_Y == hconstr && NPS::CONNECTIONTYPE_D == lconstr) {
//         return (highVol / lowVol) / sqrt(3);
//     } else if (NPS::CONNECTIONTYPE_D == hconstr && NPS::CONNECTIONTYPE_Y == lconstr) {
//         return (highVol / lowVol) * sqrt(3);
//     }
//     return 0.0;
// }

// double PowerFlowParamWidget::getBusBarVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock)
// {
//     if (peblock == nullptr) {
//         return 0.0;
//     }

//     return peblock
//             ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
//                                  NPS::KEYWORDS_BUSBAR_VOLTAGE)
//             .toDouble();
// }

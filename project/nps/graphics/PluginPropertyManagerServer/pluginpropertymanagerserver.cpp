#include "pluginpropertymanagerserver.h"
#include "CommonModelAssistant.h"
#include "CoreLib/DocHelper.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PropertyManagerServer.h"
#include "PropertyServerMng.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <cmath>
#include <complex>

using std::complex;
using std::conj;
using namespace Kcc::DocHelper;

static const int DLGLIST_WIDTH = 640;
static const int DLGLIST_HEIGHT = 480;

REG_MODULE_BEGIN(PluginPropertyManagerServer, "", "PluginPropertyManagerServer")
REG_MODULE_END(PluginPropertyManagerServer)

USE_LOGOUT_("PluginDataManagerServer")

PluginPropertyManagerServer::PluginPropertyManagerServer(QString strName)
    : Module(Module_Type_Normal, strName),
      m_openCurrentVariables(nullptr),
      m_powerFlowCalcSet(nullptr),
      m_faultSimuAction(nullptr),
      m_pPowerFlowCalcSetDialog(nullptr),
      m_pPowerFlowCalcSetWidget(nullptr),
      m_projectName("")
{
    m_openCurrentVariables = new QAction(this);
    QObject::connect(m_openCurrentVariables, SIGNAL(triggered()), SLOT(onOpenCurrentVariables()));
    m_powerFlowCalcSet = new QAction(this);
    QObject::connect(m_powerFlowCalcSet, SIGNAL(triggered()), SLOT(onPowerFlowCalcSet()));
    m_powerFlowCalcSet->setEnabled(true);
    m_faultSimuAction = new QAction(this);
    QObject::connect(m_faultSimuAction, SIGNAL(triggered()), SLOT(onFaultSimuSet()));
    propertyManagerServer = new PropertyManagerServer();
    RegServer<IPropertyManagerServer>(propertyManagerServer);
}

PluginPropertyManagerServer::~PluginPropertyManagerServer()
{
    if (m_pPowerFlowCalcSetWidget != nullptr) {
        delete m_pPowerFlowCalcSetWidget;
        m_pPowerFlowCalcSetWidget = nullptr;
    }
    if (m_pPowerFlowCalcSetDialog) {
        delete m_pPowerFlowCalcSetDialog;
        m_pPowerFlowCalcSetDialog = nullptr;
    }
}

void PluginPropertyManagerServer::init(KeyValueMap &params)
{
    if (params.contains("ProjectName")) {
        m_projectName = params["ProjectName"];
    }

    if (!PropertyServerMng::getInstance().init()) {
        LOGOUT(QObject::tr("PropertyServerMng init false"), LOG_ERROR);
    }

    if (PropertyServerMng::getInstance().m_pUIServer && PropertyServerMng::getInstance().m_pUIServer->GetMainUI()) {
        PropertyServerMng::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginPropertyManagerServer", "openCurrentSimuVariables", m_openCurrentVariables);
        PropertyServerMng::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginPropertyManagerServer", "PowerFlowCalcSet", m_powerFlowCalcSet);
        PropertyServerMng::getInstance().m_pUIServer->GetMainUI()->RegisterAction("PluginPropertyManagerServer",
                                                                                  "faultSimulation", m_faultSimuAction);
    }

    if (PropertyServerMng::getInstance().m_pGraphicsModelingServerIF != nullptr) {
        PropertyServerMng::getInstance().m_pGraphicsModelingServerIF->connectNotify(
                Notify_DrawingBoardActived, this,
                SLOT(onRecieveGraphiceServerManagerMsg(unsigned int, const NotifyStruct &)));
    }

    if (PropertyServerMng::getInstance().m_simulationManagerServerIF != nullptr) {
        PropertyServerMng::getInstance().m_simulationManagerServerIF->connectNotify(
                Notify_DrawingBoardRunning, this,
                SLOT(onRecieveSimulationServerManagerMsg(unsigned int, const NotifyStruct &)));
        PropertyServerMng::getInstance().m_simulationManagerServerIF->connectNotify(
                Notify_DrawingBoardStopped, this,
                SLOT(onRecieveSimulationServerManagerMsg(unsigned int, const NotifyStruct &)));
    }
}

void PluginPropertyManagerServer::unInit(KeyValueMap &saveParams)
{
    PropertyServerMng::getInstance().unInit();
}

void PluginPropertyManagerServer::onPowerFlowCalcSet()
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        // 未获取到项目配置！
        LOGOUT(tr("Project configuration not obtained!"), LOG_ERROR);
        return;
    }
    if (PropertyServerMng::getInstance().m_pUIServer == nullptr
        || PropertyServerMng::getInstance().m_pUIServer->GetMainUI() == nullptr) {
        return;
    }
    int basemva = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::SB).toInt();
    if (m_pPowerFlowCalcSetWidget == nullptr) {
        m_pPowerFlowCalcSetWidget = new PowerFlowSetWidget();
    } else {
        m_pPowerFlowCalcSetWidget->updatePFCFG();
    }
    if (m_pPowerFlowCalcSetDialog == nullptr) {
        m_pPowerFlowCalcSetDialog = new KCustomDialog(
                TITLE_POWERFLOW_SET, m_pPowerFlowCalcSetWidget, KBaseDlgBox::Cancel, KBaseDlgBox::NoButton,
                dynamic_cast<QMainWindow *>(PropertyServerMng::getInstance().m_pUIServer->GetMainUI()));
        QPushButton *pbtnOK = m_pPowerFlowCalcSetDialog->addCustomButton(QObject::tr("Execute")); // 执行
        m_pPowerFlowCalcSetDialog->setDefaultButton(pbtnOK);
        m_pPowerFlowCalcSetDialog->resize(DLGLIST_WIDTH, DLGLIST_HEIGHT);
        m_pPowerFlowCalcSetDialog->memoryGeometry(NPS::DLGMEMORY_SETTING);
        m_pPowerFlowCalcSetDialog->setObjectName(CMA::USERMANUAL_POWERFLOWCLC);
        // 按钮信号槽
        QObject::connect(pbtnOK, SIGNAL(released()), this, SLOT(onImplementclicked()));
    }
    m_pPowerFlowCalcSetDialog->exec();
}

void PluginPropertyManagerServer::onImplementclicked()
{
    if (m_pPowerFlowCalcSetDialog != nullptr && m_pPowerFlowCalcSetWidget != nullptr
        && PropertyServerMng::getInstance().m_pGraphicsModelingServer != nullptr
        && PropertyServerMng::getInstance().m_simulationManagerServer != nullptr
        && PropertyServerMng::getInstance().m_projectManagerServer != nullptr) {
        PDrawingBoardClass pdrawboard =
                PropertyServerMng::getInstance().m_pGraphicsModelingServer->getDrawingBoardInfoById(
                        PropertyServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID());
        QString errorinfo = "";
        QVariantMap boardactivemap;
        if (PropertyServerMng::getInstance()
                    .m_projectManagerServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE)
                    .canConvert<QVariantMap>()) {
            boardactivemap = PropertyServerMng::getInstance()
                                     .m_projectManagerServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE)
                                     .value<QVariantMap>();
        }
        if (pdrawboard == nullptr || ElecBoardModel::Type != pdrawboard->getModelType()) {
            // 请切换到电气画板再设置！
            LOGOUT(tr("Please switch to the electrical drawing board before setting up!"), LOG_ERROR);
        } else if (!boardactivemap[pdrawboard->getUUID()].toBool()) {
            // 请激活当前画板再设置！
            LOGOUT(tr("Please activate the current drawing board before setting it up!"), LOG_ERROR);
        } else if (!m_pPowerFlowCalcSetWidget->checkLegitimacy(errorinfo)) {
            LOGOUT(errorinfo, LOG_ERROR);
        } else {
            QVariantMap pfParamMap = m_pPowerFlowCalcSetWidget->getPFParamMap();
            m_pPowerFlowCalcSetWidget->saveData();
            int basemv = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::SB).toInt();
            int frequency = PropertyServerMng::getInstance()
                                    .m_projectManagerServer->GetProjectConfig(KL_PRO::FREQUENCY)
                                    .toInt();
            if (frequency <= 0 || basemv <= 0) {
                LOGOUT(tr("Project configuration not obtained!"), LOG_ERROR);
            }
            QString errorinfo = "";
            updatePowerFlowParam(pfParamMap[PFSET_KWD_BASEMVA].toInt(), frequency, pdrawboard);
            PropertyServerMng::getInstance().m_projectManagerServer->SaveBoardModel(pdrawboard);
            PropertyServerMng::getInstance().m_simulationManagerServer->RunPowerFlow(pdrawboard, POWERFLOW_AC,
                                                                                     pfParamMap);
        }
    }
    m_pPowerFlowCalcSetDialog->hide();
}

static inline bool checkCurBoardPowerflowCalcEnable()
{
    QString strboarduuid = PropertyServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
    PDrawingBoardClass pdrawboard =
            PropertyServerMng::getInstance().m_pGraphicsModelingServer->getDrawingBoardInfoById(strboarduuid);
    if (pdrawboard != nullptr && ElecBoardModel::Type == pdrawboard->getModelType()) {
        return true;
    } else {
        return false;
    }
}

void PluginPropertyManagerServer::onFaultSimuSet()
{
    if (PropertyServerMng::getInstance().m_pUIServer == nullptr
        || PropertyServerMng::getInstance().m_pUIServer->GetMainUI() == nullptr) {
        return;
    }
    FaultSimuWidget *pFaultSimuWidget = new FaultSimuWidget();
    KCustomDialog faultsimudlg(TITLE_FAULT_SIMU, pFaultSimuWidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
                               KBaseDlgBox::Ok,
                               dynamic_cast<QMainWindow *>(PropertyServerMng::getInstance().m_pUIServer->GetMainUI()));
    faultsimudlg.resize(pFaultSimuWidget->width(), pFaultSimuWidget->height());
    faultsimudlg.memoryGeometry(NPS::DLGMEMORY_CURRSIMU);
    faultsimudlg.setMargin(1);
    faultsimudlg.setHiddenButtonGroup(true);
    if (KBaseDlgBox::Ok == faultsimudlg.exec()) {
        pFaultSimuWidget->saveData();
    }
}

void PluginPropertyManagerServer::onRecieveGraphiceServerManagerMsg(unsigned int code, const NotifyStruct &param)
{
    if (PropertyServerMng::getInstance().m_pGraphicsModelingServer == nullptr || m_powerFlowCalcSet == nullptr)
        return;
    switch (code) {
    case Notify_DrawingBoardActived: {
        m_powerFlowCalcSet->setEnabled(checkCurBoardPowerflowCalcEnable());
    } break;
    default:
        break;
    }
}

void PluginPropertyManagerServer::onRecieveSimulationServerManagerMsg(unsigned int code, const NotifyStruct &param)
{
    if (code == Notify_DrawingBoardRunning) {
        m_powerFlowCalcSet->setEnabled(false);
    } else {
        m_powerFlowCalcSet->setEnabled(checkCurBoardPowerflowCalcEnable());
    }
}

void PluginPropertyManagerServer::updatePowerFlowParam(
        const int &basemva, const int &frequency, QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pdrawboard)
{
    if (pdrawboard == nullptr || ElecBoardModel::Type != pdrawboard->getModelType() || basemva == 0) {
        return;
    }
    for (PModel cmodel : pdrawboard->getChildModels().values()) {
        // 线路的潮流计算结果需要计算
        if (cmodel == nullptr) {
            continue;
        }
        PElectricalBlock peblock = cmodel.dynamicCast<ElectricalBlock>();
        if (peblock == nullptr) {
            continue;
        }
        QSharedPointer<DeviceModel> tmpdevicetype = peblock->getDeviceModel();
        if (NPS::PROTOTYPENAME_LINE == peblock->getPrototypeName()
            || NPS::PROTOTYPENAME_PI_LINE == peblock->getPrototypeName()
            || NPS::PROTOTYPENAME_PI_LINKLINE == peblock->getPrototypeName()) {
            QString r, x, b;
            getLineRXB(peblock, tmpdevicetype, basemva, frequency, r, x, b);
            peblock->setVariableData(peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter,
                                                                    RoleDataDefinition::NameRole, NPS::KEYWORDS_R),
                                     RoleDataDefinition::ValueRole, r);
            peblock->setVariableData(peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter,
                                                                    RoleDataDefinition::NameRole, NPS::KEYWORDS_X),
                                     RoleDataDefinition::ValueRole, x);
            peblock->setVariableData(peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter,
                                                                    RoleDataDefinition::NameRole, NPS::KEYWORDS_B),
                                     RoleDataDefinition::ValueRole, b);
        } else if (NPS::PROTOTYPENAME_BUSBAR == peblock->getPrototypeName()
                   || NPS::PROTOTYPENAME_DOTBUSBAR == peblock->getPrototypeName()) {
            // 母线的参数
            peblock->setVariableData(
                    peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter, RoleDataDefinition::NameRole,
                                                   NPS::KEYWORDS_BUSBAR_BASEKV),
                    RoleDataDefinition::ValueRole, peblock->getElectricalVariableValue(NPS::KEYWORDS_BUSBAR_VOLTAGE));
        } else if (NPS::PROTOTYPENAME_TWTRANSFORMER == peblock->getPrototypeName()) {
            // 相移角angle计算YY：0度 YD（高压侧Y，低压侧D）：30度 DY（高压侧D，低压侧Y）： - 30度 DD：0度
            PVariable pvarTwtX = peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter,
                                                                RoleDataDefinition::NameRole, NPS::KEYWORDS_TWT_X);
            PVariable pvarAngle = peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter,
                                                                 RoleDataDefinition::NameRole, NPS::KEYWORDS_TWT_ANGLE);
            PVariable pvarRatio = peblock->getVariableByRoleData(RoleDataDefinition::LoadFlowParameter,
                                                                 RoleDataDefinition::NameRole, NPS::KEYWORDS_TWT_RATIO);
            if (pvarTwtX == nullptr || pvarAngle == nullptr || pvarRatio == nullptr) {
                continue;
            }

            if (tmpdevicetype != nullptr) {
                peblock->setVariableData(
                        pvarTwtX, RoleDataDefinition::ValueRole,
                        NPS::doubleToString15(
                                getPFX(peblock, basemva, frequency,
                                       tmpdevicetype->getDeviceTypeVariableValue(NPS::KEYWORDS_TWT_LS).toDouble())));
                peblock->setVariableData(pvarAngle, RoleDataDefinition::ValueRole,
                                         getAngleValue(peblock->getPrototypeName(), tmpdevicetype));
                // peblock->setVariableData(pvarRatio, RoleDataDefinition::ValueRole,
                //                          NPS::doubleToString15(getRatio(peblock, tmpdevicetype)));
            } else {
                peblock->setVariableData(pvarTwtX, RoleDataDefinition::ValueRole, 0.1);
                // [%1]设备类型为空，潮流参数[%2]无法计算！
                LOGOUT(QString("[%1] device type is empty, power flow parameter [%2] cannot be "
                               "calculated!")
                               .arg(peblock->getName())
                               .arg(pvarTwtX->getDisplayName()),
                       LOG_ERROR);
            }
        } else if (NPS::PROTOTYPENAME_NITWTRANSFORMER == peblock->getPrototypeName()) {
            // 相移角angle计算YY：0度 YD（高压侧Y，低压侧D）：30度 DY（高压侧D，低压侧Y）： - 30度 DD：0度
            PVariable pvarAngle = peblock->getVariableByRoleData(
                    RoleDataDefinition::LoadFlowParameter, RoleDataDefinition::NameRole, NPS::KEYWORDS_NITWT_ANGLE);
            PVariable pvarRatio = peblock->getVariableByRoleData(
                    RoleDataDefinition::LoadFlowParameter, RoleDataDefinition::NameRole, NPS::KEYWORDS_NITWT_RATIO);
            if (pvarAngle == nullptr || pvarRatio == nullptr) {
                continue;
            }
            if (tmpdevicetype != nullptr) {
                peblock->setVariableData(pvarAngle, RoleDataDefinition::ValueRole,
                                         getAngleValue(peblock->getPrototypeName(), tmpdevicetype));
                // peblock->setVariableData(pvarRatio, RoleDataDefinition::ValueRole,
                //                          NPS::doubleToString15(getRatio(peblock, tmpdevicetype)));
            } else {
                // [%1]设备类型为空，潮流参数[%2]无法计算！
                LOGOUT(QObject::tr("[%1] Device type is empty, power flow parameter [%2] cannot be calculated!")
                               .arg(peblock->getName())
                               .arg(pvarAngle->getDisplayName()),
                       LOG_ERROR);
            }
        } else if (NPS::PROTOTYPENAME_SHUNTFILTER == peblock->getPrototypeName()) {
            // R-L-C=1,R-L=2,R-C=3,R-L-C|R=4,R-L-C-C|R=5
            double spvalue = 0.0;
            double sqvalue = 0.0;
            getShuntFilter(peblock, frequency, spvalue, sqvalue);
            peblock->setLoadFlowVariableValue(NPS::KEYWORDS_SHUNTFILTER_P, NPS::doubleToString15(spvalue));
            peblock->setLoadFlowVariableValue(NPS::KEYWORDS_SHUNTFILTER_Q, NPS::doubleToString15(sqvalue));
        } else if (NPS::PROTOTYPENAME_INDUCTOR == peblock->getPrototypeName()) {
            // 电感元件（电抗）x=l/Z，
            double Zb = getICRZBValue(peblock, basemva);
            if (!CMA::isEqualO(Zb)) {
                double val = peblock->getValueByRoleData(RoleDataDefinition::ElectricalParameter,
                                                         RoleDataDefinition::NameRole, NPS::KEYWORDS_INDUCTOR_VAL)
                                     .toDouble();
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_X, NPS::doubleToString15(val / Zb));
            }
        } else if (NPS::PROTOTYPENAME_CAPACITOR == peblock->getPrototypeName()) {
            // 电容元件(电纳)b=-c*Z
            double Zb = getICRZBValue(peblock, basemva);
            if (!CMA::isEqualO(Zb)) {
                double val = peblock->getValueByRoleData(RoleDataDefinition::ElectricalParameter,
                                                         RoleDataDefinition::NameRole, NPS::KEYWORDS_CAPACITOR_VAL)
                                     .toDouble();
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_B, NPS::doubleToString15(val / Zb));
            }
        } else if (NPS::PROTOTYPENAME_RESISTOR == peblock->getPrototypeName()) {
            ////电阻计算：r = R/Z
            double Zb = getICRZBValue(peblock, basemva);
            if (!CMA::isEqualO(Zb)) {
                double val = peblock->getValueByRoleData(RoleDataDefinition::ElectricalParameter,
                                                         RoleDataDefinition::NameRole, NPS::KEYWORDS_RESISTOR_VAL)
                                     .toDouble();
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_R, NPS::doubleToString15(val / Zb));
            }
        } else if (NPS::PROTOTYPENAME_LOAD == peblock->getPrototypeName()) {
            double spvalue = 0;
            double sqvalue = 0;
            getLoadPQ(peblock->getDeviceModel(), frequency, spvalue, sqvalue);
            peblock->setLoadFlowVariableValue(NPS::KEYWORDS_LOAD_PD, NPS::doubleToString15(spvalue));
            peblock->setLoadFlowVariableValue(NPS::KEYWORDS_LOAD_QD, NPS::doubleToString15(sqvalue));
        } else if (NPS::PROTOTYPENAME_CPLOAD == peblock->getPrototypeName()) {
            if (peblock->getDeviceModel() != nullptr) {
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_LOAD_PD,
                                                  peblock->getDeviceModel()->getValueByRoleData(
                                                          RoleDataDefinition::DeviceTypeParameter,
                                                          RoleDataDefinition::NameRole, NPS::KEYWORDS_CPLOAD_P));
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_LOAD_QD,
                                                  peblock->getDeviceModel()->getValueByRoleData(
                                                          RoleDataDefinition::DeviceTypeParameter,
                                                          RoleDataDefinition::NameRole, NPS::KEYWORDS_CPLOAD_Q));
            } else {
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_LOAD_PD, 0);
                peblock->setLoadFlowVariableValue(NPS::KEYWORDS_LOAD_QD, 0);
            }
        }
    }
}

int PluginPropertyManagerServer::getAngleValue(const QString &prototypename,
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

QString PluginPropertyManagerServer::getConnectionStr(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype,
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
    return QString();
}

double PluginPropertyManagerServer::getPFX(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model,
                                           const int &baseMVA, const int &frequency, const double &LS)
{
    if (model == nullptr || baseMVA == 0) {
        return 0.0;
    }
    double Vb = getHFBUSVoltage(model);
    double Zb = pow(Vb, 2) / baseMVA;
    if (CMA::isEqualO(Zb)) {
        return 0.0;
    }
    return 2 * M_PI * frequency * LS / Zb;
}

double PluginPropertyManagerServer::getHFBUSVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model)
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

// double PluginPropertyManagerServer::getRatio(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock,
//                                              QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype)
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
//         LOGOUT(QString("[%1] one side voltage is 0, ratio calculation failed").arg(peblock->getName()), LOG_ERROR);
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

// double PluginPropertyManagerServer::getBusBarVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock)
// {
//     if (peblock == nullptr) {
//         return 0.0;
//     }
//     return peblock
//             ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
//                                  NPS::KEYWORDS_BUSBAR_VOLTAGE)
//             .toDouble();
// }

void PluginPropertyManagerServer::getShuntFilter(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                 const int &frequency, double &pvalue, double &qvalue)
{
    // R-L-C=1,R-L=2,R-C=3,R-L-C|R=4,R-L-C-C|R=5
    pvalue = 0.0;
    qvalue = 0.0;
    if (model == nullptr || model->getParentModel() == nullptr) {
        return;
    }
    // 基准容量
    QList<PModel> adjModels = model->getParentModel()->getAdjacentChild(model->getUUID());
    if (adjModels.size() <= 0 || adjModels[0] == nullptr) {
        return;
    }

    int Uvalue = adjModels[0]
                         ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                              NPS::KEYWORDS_BUSBAR_VOLTAGE)
                         .toDouble()
            / sqrt(3);
    int bustype = adjModels[0]
                          ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                               NPS::KEYWORDS_BUSBAR_AC)
                          .toInt();
    if (bustype == 1) {
        Uvalue = 3 * Uvalue;
    }
    double Rvalue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                              NPS::KEYWORDS_SHUNTFILTER_R)
                            .toDouble();
    double Lvalue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                              NPS::KEYWORDS_SHUNTFILTER_L)
                            .toDouble();
    double Cvalue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                              NPS::KEYWORDS_SHUNTFILTER_C)
                            .toDouble();
    double Rpvalue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                               NPS::KEYWORDS_SHUNTFILTER_RP)
                             .toDouble();
    double Csvalue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                               NPS::KEYWORDS_SHUNTFILTER_CS)
                             .toDouble();
    int ShuntTypevalue = model->getValueByRoleData(RoleDataDefinition::ElectricalParameter,
                                                   RoleDataDefinition::NameRole, NPS::KEYWORDS_SHUNTFILTER_SHUNTTYPE)
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

void PluginPropertyManagerServer::getLoadPQ(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype,
                                            const int &frequency, double &pvalue, double &qvalue)
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
    double U = Vrated / sqrt(3);
    double w = 2 * M_PI * frequency;
    auto Z = complex<double>(0.0, w * loadR * LoadL) / complex<double>(loadR, w * LoadL);
    double R = Z.real();
    double X = Z.imag();
    auto PQ = U * conj<double>(U / complex<double>(R, X));
    pvalue = PQ.real() * 3;
    qvalue = PQ.imag() * 3;
}

double PluginPropertyManagerServer::getICRZBValue(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model,
                                                  const int &baseMVA)
{
    if (model == nullptr || model->getParentModel() == nullptr) {
        return 0.0;
    }

    if (baseMVA == 0) {
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

void PluginPropertyManagerServer::getLineRXB(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> elecModel,
                                             QSharedPointer<Kcc::BlockDefinition::DeviceModel> devmodel,
                                             const int &basemva, const int &frequency, QString &r, QString &x,
                                             QString &b)
{
    r = "0";
    x = "0";
    b = "0";
    if (elecModel == nullptr || devmodel == nullptr || basemva == 0 || frequency == 0) {
        return;
    }
    double rp = 0;
    double lp = 0;
    double cp = 0;
    double lineLength = 0;
    if (NPS::PROTOTYPENAME_LINE == elecModel->getPrototypeName()) {
        lineLength = elecModel
                             ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                                  NPS::KEYWORDS_LENTH)
                             .toDouble();
        rp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_RP).toDouble();
        lp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_LP).toDouble();
        cp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_CP).toDouble();
    } else {
        lineLength = elecModel
                             ->getValueByRoleData(RoleDataDefinition::ElectricalParameter, RoleDataDefinition::NameRole,
                                                  NPS::KEYWORDS_PILINE_LEN)
                             .toDouble();
        rp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_PILINE_R1).toDouble();
        lp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_PILINE_L1).toDouble();
        cp = devmodel->getDeviceTypeVariableValue(NPS::KEYWORDS_PILINE_C1).toDouble();
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
    double Zb = pow(vol, 2) / basemva;
    // 电阻 r = r1*长度/Z标幺值
    r = NPS::doubleToString15(rp * lineLength / Zb);
    // 电抗 x = x1*长度/Z标幺值
    x = NPS::doubleToString15(2 * M_PI * frequency * lp * lineLength / Zb);
    // 电纳 b = b1*长度*Z标幺值
    b = NPS::doubleToString15(2 * M_PI * frequency * cp * lineLength * Zb);
}

void PluginPropertyManagerServer::onOpenCurrentVariables()
{
    if (PropertyServerMng::getInstance().m_pUIServer == nullptr
        || PropertyServerMng::getInstance().m_pUIServer->GetMainUI() == nullptr) {
        return;
    }
    CurrentSimuWidget *pAllVariables = new CurrentSimuWidget(m_projectName);
    pAllVariables->initData();
    KCustomDialog cursimudlg(tr("Current Simulation"), pAllVariables, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
                             KBaseDlgBox::Ok,
                             dynamic_cast<QMainWindow *>(PropertyServerMng::getInstance().m_pUIServer->GetMainUI()));
    cursimudlg.resize(pAllVariables->width(), pAllVariables->height());
    cursimudlg.memoryGeometry(NPS::DLGMEMORY_CURRSIMU);
    cursimudlg.setObjectName(CMA::USERMANUAL_CURSIMU);
    if (KBaseDlgBox::Ok == cursimudlg.exec()) {
        pAllVariables->saveVariableData();
    }
}

// void PluginPropertyManagerServer::onVarOKclicked()
//{
//	m_customDlg->hide();
//	delete m_customDlg;
//	m_customDlg = nullptr;
// }

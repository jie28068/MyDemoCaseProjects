#include "SimuParamsProperty.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "PropertyManagerServer.h"
#include "PropertyServerMng.h"

using namespace Kcc::ProjectManager;

USE_LOGOUT_("PropertyManagerServer")

static const QString SIMUPARAMS_TITLE = QObject::tr("Simulation Parameter Settings");   // 仿真参数设置
static const QString SIMUPARAMS_BASE = QObject::tr("Basic Simulation Parameters");      // 基本仿真参数
static const QString SIMUPARAMS_ELEC = QObject::tr("Electrical Simulation Parameters"); // 电气仿真参数
static const QString SIMUPARAMS_CTRL = QObject::tr("Control Simulation Parameters");    // 控制仿真参数
static const QString SIMUPARAMS_DEBUG = QObject::tr("Run Step Config");                 // 单步仿真设置

SimuParamsProperty::SimuParamsProperty(SimuParams &params, RunStepConfigParams &stepParams, int simuState, int NpsOrCad)
    : CommonWrapper(),
      m_simuState(simuState),
      m_simuParams(params),
      m_stepParams(stepParams),
      m_pSimuParamWidget(nullptr),
      m_pElecParamsWidget(nullptr),
      m_pRunStepConfigWidget(nullptr)
{
    m_NpsOrCad = (SimuParamWidget::SimulationType)NpsOrCad;
    init();
}

SimuParamsProperty::~SimuParamsProperty()
{
    delete m_pSimuParamWidget;
    delete m_pElecParamsWidget;
    delete m_pRunStepConfigWidget;
}

QString SimuParamsProperty::getTitle()
{
    return SIMUPARAMS_TITLE;
}

void SimuParamsProperty::init()
{
    CommonWrapper::init();
    m_pSimuParamWidget = new SimuParamWidget(m_NpsOrCad);
    m_pSimuParamWidget->setParams(m_simuParams, (SimulationState)m_simuState);
    addWidget(m_pSimuParamWidget, SIMUPARAMS_BASE);
    if (m_NpsOrCad == SimuParamWidget::SimuNPS) {
        m_pElecParamsWidget = new ElecParamsWidget();
        m_pElecParamsWidget->setParams(m_simuParams, (SimulationState)m_simuState);
        addWidget(m_pElecParamsWidget, SIMUPARAMS_ELEC);
    }
    m_pRunStepConfigWidget = new RunStepConfigWidget(nullptr);
    m_pRunStepConfigWidget->setParams(m_stepParams, (SimulationState)m_simuState);
    addWidget(m_pRunStepConfigWidget, SIMUPARAMS_DEBUG);
}

void SimuParamsProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::DialogCode::Accepted) {
        if (m_pElecParamsWidget) {
            m_pElecParamsWidget->getParams(m_simuParams);
        }
        m_pSimuParamWidget->getParams(m_simuParams);
        m_pRunStepConfigWidget->getParams(m_stepParams);
        PIProjectManagerServer pProjServer = RequestServer<IProjectManagerServer>();
        auto pCurProj = pProjServer->GetCurProject();
        if (!pCurProj.isNull()) {
            pCurProj->setProjectConfig(KL_PRO::FREQUENCY, m_simuParams.F);
            pCurProj->setProjectConfig(KL_PRO::ISINITIALIZED, (bool)m_simuParams.INIT);
            pCurProj->setProjectConfig(KL_PRO::RUN_TIME, m_simuParams.MAXTIME);
            pCurProj->setProjectConfig(KL_PRO::SIMU_START_TIME, m_simuParams.StartTime);
            pCurProj->setProjectConfig(KL_PRO::STEP_SIZE, m_simuParams.TAU);
            pCurProj->setProjectConfig(KL_PRO::ODE_SOLVER, m_simuParams.algtype);
            pCurProj->setProjectConfig(KL_PRO::ODE_EXTORDER, m_simuParams.extOrder);
            pCurProj->setProjectConfig(KL_PRO::ODE_NEWTONITER, m_simuParams.newtonIter);
        } else {
            //"设置仿真参数失败"
            LOGOUT(QObject::tr("Failed to set simulation parameters."), LOG_ERROR);
        }
    }
}

bool SimuParamsProperty::checkValue(QString &errorinfo)
{
    if (!m_pSimuParamWidget->checkValue(errorinfo)) {
        return false;
    }
    if (m_pElecParamsWidget && !m_pElecParamsWidget->checkValue(errorinfo)) {
        return false;
    }
    if (m_pRunStepConfigWidget && !m_pRunStepConfigWidget->checkValue(errorinfo)) {
        return false;
    }
    return true;
}

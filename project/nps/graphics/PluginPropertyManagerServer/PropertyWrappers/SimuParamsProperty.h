#pragma once

#include "CommonWrapper.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "SimuParamWidget.h"
#include "server/SimulationCore/ISimuInstance.h"

// using namespace Kcc::DataManager;
using namespace Kcc::BlockDefinition;
using namespace Kcc::SimulationManager;

class SimuParamsProperty : public CommonWrapper
{
public:
    SimuParamsProperty(SimuParams &params, RunStepConfigParams &stepParams, int simuState, int NpsOrCad);
    virtual ~SimuParamsProperty();

    // 名称，展示在标题栏
    virtual QString getTitle() override;

    // 弹出Dialog并得到交互结果时调用此接口
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;

    // 在窗口点击OK时，判断弹窗中的值是否符合要求，不符合返回具体信息errorinfo
    virtual bool checkValue(QString &errorinfo = QString()) override;

private:
    void init();
    SimuParamWidget::SimulationType m_NpsOrCad;
    int m_simuState;
    SimuParams &m_simuParams;
    RunStepConfigParams &m_stepParams;
    SimuParamWidget *m_pSimuParamWidget;
    ElecParamsWidget *m_pElecParamsWidget;
    RunStepConfigWidget *m_pRunStepConfigWidget;
};
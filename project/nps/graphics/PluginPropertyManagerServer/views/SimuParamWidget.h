#pragma once

#include <QCheckBox>
#include <QCombobox>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>

#include "KLineEdit.h"
#include "server/SimulationCore/ISimuInstance.h"
#include "server/SimulationManagerServer/ISimulationManagerServer.h"
// 仿真设置widget，被SimuParamsProperty使用

using Kcc::SimuParams;
using namespace Kcc::SimulationManager;

enum SimulationState { SimuStop, SimuRun };

class SimuParamWidget : public QWidget
{
    Q_OBJECT

public:
    enum SimulationType { SimuNPS, SimuCAD };

    SimuParamWidget(SimulationType type, QWidget *parent = 0);
    ~SimuParamWidget();

    void getParams(SimuParams &simuParam);
    void setParams(const SimuParams &simuparams, SimulationState simuState);

    bool checkValue(QString &errorinfo);

private slots:
    void onComboBoxAlgChanged(int index);

private:
    void initUI();

    SimulationType m_NpsOrCad;
    KLineEdit *m_lineEdit_tao;
    KLineEdit *m_lineEdit_maxtime;
    KLineEdit *m_lineEdit_startTime;
    QComboBox *m_comboBox_algtype;
    KLineEdit *m_pNewtonIter_edit;
    QSpinBox *m_pExtOrder_spin;
    QLabel *m_pNewtonIter_label;
    QLabel *m_pExtOrder_label;
};
class ElecParamsWidget : public QWidget
{
    Q_OBJECT

public:
    ElecParamsWidget(QWidget *parent = 0);
    ~ElecParamsWidget();

    void getParams(SimuParams &simuParam);
    void setParams(const SimuParams &simuparams, SimulationState simuState);

    bool checkValue(QString &errorinfo);

private:
    void initUI();

    QComboBox *m_comboBox_f;
    QComboBox *m_comboBox_init;
};

class RunStepConfigWidget : public QWidget
{
    Q_OBJECT
public:
    RunStepConfigWidget(QWidget *parent = 0) : QWidget(parent) { initUI(); }
    ~RunStepConfigWidget() { }

    void getParams(RunStepConfigParams &stepParams);
    void setParams(const RunStepConfigParams &stepParams, SimulationState simuState);

    bool checkValue(QString &errorinfo);

private slots:
    void onPauseTimeCheck(int state);

private:
    void initUI();

    KLineEdit *m_pRunSteps_edit;
    KLineEdit *m_pPauseTime_edit;
    QCheckBox *m_pPauseTime_che;
};

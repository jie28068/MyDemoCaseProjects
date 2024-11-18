#include <QGridLayout>
#include <QListView>
#include <QRegExpValidator>

#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "SimuParamWidget.h"

USE_LOGOUT_("SimuParamWidget")

static const QRegExp REG_INT("^(\\d+)([eE](\\d+))?$");
static const QRegExp REG_UDOUBLE("^((0\\.\\d+)|(([1-9]{1}\\d*)(\\.\\d+)?))([eE](-?\\d+))?$");
static const QRegExp REG_DOUBLE("^((-?0\\.\\d+)|((-?[1-9]{1}\\d*)(\\.\\d+)?))([eE](-?\\d+))?$");

SimuParamWidget::SimuParamWidget(SimulationType type, QWidget *parent) : m_NpsOrCad(type), QWidget(parent)
{
    initUI();
}

SimuParamWidget::~SimuParamWidget() { }

void SimuParamWidget::initUI()
{
    QGridLayout *gridLayout = new QGridLayout(this);
    int rowNum = -1;

    QLabel *label_tao = new QLabel(NPS::SIMUPARAMS_STEP_SIZE, this);
    QLabel *label_maxtime = new QLabel(NPS::SIMUPARAMS_TIME, this);
    QLabel *label_startTime = new QLabel(NPS::SIMUPARAMS_START_TIME, this);
    m_lineEdit_tao = new KLineEdit(this);
    m_lineEdit_maxtime = new KLineEdit(this);
    m_lineEdit_startTime = new KLineEdit(this);
    m_lineEdit_tao->setValidator(new QRegExpValidator(REG_UDOUBLE, this));
    m_lineEdit_tao->setMaxLength(10);
    m_lineEdit_maxtime->setValidator(new QRegExpValidator(REG_UDOUBLE, this));
    m_lineEdit_maxtime->setMaxLength(10);
    m_lineEdit_startTime->setValidator(new QRegExpValidator(REG_DOUBLE, this));
    m_lineEdit_startTime->setMaxLength(10);
    gridLayout->addWidget(label_tao, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_lineEdit_tao, rowNum, 1, 1, 1);
    gridLayout->addWidget(label_startTime, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_lineEdit_startTime, rowNum, 1, 1, 1);
    gridLayout->addWidget(label_maxtime, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_lineEdit_maxtime, rowNum, 1, 1, 1);

    // #ifdef COMPILER_PRODUCT_DESIGNER
    m_pExtOrder_label = new QLabel(NPS::SIMUPARAMS_EXTORDER, this);
    m_pNewtonIter_label = new QLabel(NPS::SIMUPARAMS_NEWTONITER, this);
    m_pExtOrder_spin = new QSpinBox(this);
    m_pNewtonIter_edit = new KLineEdit(this);
    m_pExtOrder_spin->setRange(1, 4);
    m_pExtOrder_spin->setValue(4);
    m_pNewtonIter_edit->setValidator(new QRegExpValidator(REG_INT, this));
    m_pNewtonIter_edit->setText("5");
    m_pNewtonIter_edit->setMaxLength(10);

    QLabel *label_algtype = new QLabel(NPS::SIMUPARAMS_ALGTYPE, this);
    m_comboBox_algtype = new QComboBox(this);
    connect(m_comboBox_algtype, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxAlgChanged(int)));
    m_comboBox_algtype->setView(new QListView());
    m_comboBox_algtype->addItem(ODE_SOLVER_DISCRETE);
    m_comboBox_algtype->addItem(ODE_SOLVER_1);
    m_comboBox_algtype->addItem(ODE_SOLVER_2);
    m_comboBox_algtype->addItem(ODE_SOLVER_3);
    m_comboBox_algtype->addItem(ODE_SOLVER_4);
    m_comboBox_algtype->addItem(ODE_SOLVER_5);
    m_comboBox_algtype->addItem(ODE_SOLVER_8);
    m_comboBox_algtype->addItem(ODE_SOLVER_14X);
    m_comboBox_algtype->addItem(ODE_SOLVER_1BE);

    gridLayout->addWidget(label_algtype, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_comboBox_algtype, rowNum, 1, 1, 1);
    gridLayout->addWidget(m_pExtOrder_label, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_pExtOrder_spin, rowNum, 1, 1, 1);
    gridLayout->addWidget(m_pNewtonIter_label, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_pNewtonIter_edit, rowNum, 1, 1, 1);
    // #endif
    QVBoxLayout *spacer = new QVBoxLayout(this);
    spacer->addStretch();
    gridLayout->addLayout(spacer, ++rowNum, 0);
    this->setLayout(gridLayout);
}

bool SimuParamWidget::checkValue(QString &errorinfo)
{
    SimuParams temp;
    getParams(temp);

    if (temp.TAU == 0) {
        errorinfo = tr("simulation step size cannot be 0"); // 仿真步长不能为0
        return false;
    }
    if (temp.MAXTIME == 0) {
        errorinfo = tr("simulation duration cannot be 0"); // 仿真时长不能为0
        return false;
    }
    if (temp.MAXTIME < temp.TAU) {
        errorinfo =
                tr("the simulation duration cannot be less than the simulation step size"); // 仿真时长不能小于仿真步长
        return false;
    }
    int64_t startStepCount = temp.StartTime / temp.TAU;
    double simuStartTime = startStepCount * temp.TAU;
    if (std::abs(temp.StartTime - simuStartTime) > (std::abs(simuStartTime) * DBL_EPSILON)) {
        if (simuStartTime < temp.StartTime) {
            simuStartTime = (startStepCount + 1) * temp.TAU;
        }
        // 仿真开始时间必须是仿真步长的倍数，自动修改为%1
        LOGOUT(tr("The simulation start time must be a multiple of the simulation step size, automatically modified to "
                  "%1")
                       .arg(simuStartTime),
               LOG_WARNING);
        temp.StartTime = simuStartTime;
        m_lineEdit_startTime->setText(QString::number(simuStartTime));
    }
    if (temp.StartTime > temp.MAXTIME) {
        errorinfo = tr(
                "The end time of simulation cannot be less than the start time of simulation"); // 仿真结束时间不能小于仿真开始时间
        return false;
    }

    return true;
}

void SimuParamWidget::setParams(const SimuParams &simuparams, SimulationState simuState)
{
    m_lineEdit_maxtime->setText(QString::number(simuparams.MAXTIME));
    m_lineEdit_tao->setText(QString::number(simuparams.TAU));
    m_lineEdit_startTime->setText(QString::number(simuparams.StartTime));

    if (simuState == SimuRun) {
        m_lineEdit_maxtime->setEnabled(false);
        m_lineEdit_tao->setEnabled(false);
        m_lineEdit_startTime->setEnabled(false);
    }

    // #ifdef COMPILER_PRODUCT_DESIGNER
    m_pExtOrder_spin->setValue(simuparams.extOrder);
    m_pNewtonIter_edit->setText(QString::number(simuparams.newtonIter));
    m_comboBox_algtype->setCurrentIndex(simuparams.algtype);

    if (simuState == SimuRun) {
        m_pExtOrder_spin->setEnabled(false);
        m_pNewtonIter_edit->setEnabled(false);
        m_comboBox_algtype->setEnabled(false);
    }
    // #endif
}

void SimuParamWidget::getParams(SimuParams &simuParam)
{
    simuParam.MAXTIME = m_lineEdit_maxtime->text().toDouble();
    simuParam.TAU = m_lineEdit_tao->text().toDouble();
    simuParam.StartTime = m_lineEdit_startTime->text().toDouble();

    // #ifdef COMPILER_PRODUCT_DESIGNER
    simuParam.extOrder = m_pExtOrder_spin->value();
    simuParam.newtonIter = m_pNewtonIter_edit->text().toUInt();
    simuParam.algtype = m_comboBox_algtype->currentIndex();
    // #endif
}

void SimuParamWidget::onComboBoxAlgChanged(int index)
{
    QString curItemStr = m_comboBox_algtype->itemText(index);
    if (curItemStr == ODE_SOLVER_14X) {
        m_pExtOrder_spin->setVisible(true);
        m_pExtOrder_label->setVisible(true);
        m_pNewtonIter_edit->setVisible(true);
        m_pNewtonIter_label->setVisible(true);
    } else if (curItemStr == ODE_SOLVER_1BE) {
        m_pExtOrder_spin->setVisible(false);
        m_pExtOrder_label->setVisible(false);
        m_pNewtonIter_edit->setVisible(true);
        m_pNewtonIter_label->setVisible(true);
    } else {
        m_pExtOrder_spin->setVisible(false);
        m_pExtOrder_label->setVisible(false);
        m_pNewtonIter_edit->setVisible(false);
        m_pNewtonIter_label->setVisible(false);
    }
}

ElecParamsWidget::ElecParamsWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
}

ElecParamsWidget::~ElecParamsWidget() { }

void ElecParamsWidget::initUI()
{
    QGridLayout *gridLayout = new QGridLayout(this);
    int rowNum = -1;
    QLabel *label_f = new QLabel(NPS::SIMUPARAMS_FRE, this);
    QLabel *label_init = new QLabel(NPS::SIMUPARAMS_INIT, this);
    m_comboBox_f = new QComboBox(this);
    m_comboBox_f->setView(new QListView());
    m_comboBox_f->addItems(QStringList() << QString("50") << QString("60"));
    m_comboBox_init = new QComboBox(this);
    m_comboBox_init->setView(new QListView());
    m_comboBox_init->addItem(NPS::SIMUPARAMS_INIT_USER_DEF);  // 用户自定义初始化
    m_comboBox_init->addItem(NPS::SIMUPARAMS_INIT_SYS_DEF); // 系统自动初始化
    m_comboBox_init->setEnabled(true);
    gridLayout->addWidget(label_f, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_comboBox_f, rowNum, 1, 1, 1);
    gridLayout->addWidget(label_init, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_comboBox_init, rowNum, 1, 1, 1);
    gridLayout->setColumnStretch(1, 1);
    QVBoxLayout *spacer = new QVBoxLayout(this);
    spacer->addStretch();
    gridLayout->addLayout(spacer, ++rowNum, 0);
    this->setLayout(gridLayout);
}

void ElecParamsWidget::setParams(const SimuParams &params, SimulationState simuState)
{
    m_comboBox_f->setCurrentIndex(m_comboBox_f->findText(QString::number(params.F)));
    m_comboBox_init->setCurrentIndex(params.INIT);
    if (simuState == SimuRun) {
        m_comboBox_f->setEnabled(false);
        m_comboBox_init->setEnabled(false);
    } else {
        m_comboBox_f->setEnabled(true);
        m_comboBox_init->setEnabled(true);
    }
}

void ElecParamsWidget::getParams(SimuParams &params)
{
    params.F = m_comboBox_f->currentText().toInt();
    params.INIT = m_comboBox_init->currentIndex();
}

bool ElecParamsWidget::checkValue(QString &errorinfo)
{
    SimuParams temp;
    getParams(temp);
    if (temp.F == 0) {
        errorinfo = QObject::tr("power frequency cannot be 0"); // 工频不能为0
        return false;
    }
    return true;
}

void RunStepConfigWidget::initUI()
{
    QGridLayout *gridLayout = new QGridLayout(this);
    int rowNum = -1;

    QLabel *pLabel = new QLabel(tr("Single Simulation Steps"), this); // 单次仿真步数
    m_pRunSteps_edit = new KLineEdit(this);
    m_pRunSteps_edit->setValidator(new QRegExpValidator(REG_INT, this));
    m_pRunSteps_edit->setMaxLength(10);
    m_pPauseTime_edit = new KLineEdit(this);
    m_pPauseTime_edit->setValidator(new QRegExpValidator(REG_UDOUBLE, this));
    m_pPauseTime_edit->setMaxLength(10);
    m_pPauseTime_che = new QCheckBox(tr("Simulation Pause Time"), this); // 仿真暂停时间
    m_pPauseTime_edit->setEnabled(false);
    connect(m_pPauseTime_che, SIGNAL(stateChanged(int)), this, SLOT(onPauseTimeCheck(int)));
    gridLayout->addWidget(pLabel, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_pRunSteps_edit, rowNum, 1, 1, 1);
    gridLayout->addWidget(m_pPauseTime_che, ++rowNum, 0, 1, 1);
    gridLayout->addWidget(m_pPauseTime_edit, rowNum, 1, 1, 1);

    QVBoxLayout *spacer = new QVBoxLayout(this);
    spacer->addStretch();
    gridLayout->addLayout(spacer, ++rowNum, 0);
    this->setLayout(gridLayout);
}

bool RunStepConfigWidget::checkValue(QString &errorinfo)
{
    return true;
}

void RunStepConfigWidget::setParams(const RunStepConfigParams &stepParams, SimulationState simuState)
{
    m_pRunSteps_edit->setText(QString::number(stepParams.runSteps));
    m_pPauseTime_edit->setText(QString::number(stepParams.pauseTime));
    m_pPauseTime_che->setChecked(stepParams.pauseTimeFlg);
}

void RunStepConfigWidget::getParams(RunStepConfigParams &stepParams)
{
    stepParams.runSteps = m_pRunSteps_edit->text().toDouble();
    stepParams.pauseTime = m_pPauseTime_edit->text().toDouble();
    stepParams.pauseTimeFlg = m_pPauseTime_che->isChecked();
}

void RunStepConfigWidget::onPauseTimeCheck(int state)
{
    if (state == 0) {
        m_pPauseTime_edit->setEnabled(false);
    } else {
        m_pPauseTime_edit->setEnabled(true);
    }
}

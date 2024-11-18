#include "ProjectSetProperty.h"
#include "GraphicsModelingKernel/CanvasContext.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "PropertyManagerServer.h"
#include "PropertyServerMng.h"
#include <QFileDialog>

using namespace Kcc::ProjectManager;

USE_LOGOUT_("PropertyManagerServer")

static const QString PRJSET_TITLE = QObject::tr("Project Configuration"); // 项目配置
static const QString PRJSET_FILE = QObject::tr("File Saving Settings");   // 文件保存设置
static const QString PRJSET_BASE = QObject::tr("Basic Settings");         // 基本信息

static const int BASE_KB = 1024;
static const int MAX_FILE_SIZE_KB = 1024 * 100;
static const int MIN_FILE_SIZE_KB = 100;
static const QString SIMUPARAMS_PRECISION = QObject::tr("Data Retention Accuracy"); // 数据保存精度
static const QString SIMUPARAMS_SUFFIX = QObject::tr("File Suffix");                // 文件后缀类型
static const QString SIMUPARAMS_MAX_FILE_SIZE = QObject::tr("Max File Size(KB)");   // 保存文件大小(KB)
static const QString SIMUPARAMS_ISUSESYSPATH = QObject::tr("Use Built-in Python");  // 使用内置Python
static const QString SIMUPARAMS_PYPATH = QObject::tr("External Python Path");       // 外部Python

static const QString PrjDescrition = QObject::tr("Project Description");    // 项目描述
static const QString ShowSimuSort = QObject::tr("Show Simulation Sorting"); // 显示仿真排序

ProjectSetProperty::ProjectSetProperty(int NpsOrCad) : CommonWrapper(), m_pFileSaveConfigWidget(nullptr)
{
    m_NpsOrCad = (SimuParamWidget::SimulationType)NpsOrCad;
    init();
}

ProjectSetProperty::~ProjectSetProperty() { }

QString ProjectSetProperty::getTitle()
{
    return PRJSET_TITLE;
}

void ProjectSetProperty::init()
{
    CommonWrapper::init();
    FileSaveConfigWidget::SimuCommonConfig simuComConfig;
    PIProjectManagerServer pProMngServer = RequestServer<IProjectManagerServer>();
    auto pCurProj = pProMngServer->GetCurProject();
    m_baseinfoWidget = new BaseInfoWidget(nullptr);
    if (pCurProj) {
        simuComConfig.m_dataPrecision = pCurProj->getProjectConfig(KL_PRO::DATA_RETENTION_ACCURACY).toInt();
        simuComConfig.m_fileSuffix = pCurProj->getProjectConfig(KL_PRO::FILE_SAVE_SUFFIX).toString();
        simuComConfig.m_fileSize = pCurProj->getProjectConfig(KL_PRO::FILE_SAVE_SIZE).toInt();
        m_baseinfoWidget->setShowSimuSort(pCurProj->getProjectConfig(KL_PRO::SHOW_SIMULATION_SORT).toBool());
        m_baseinfoWidget->setDescription(pCurProj->getProjectProperty(KL_PRO::DESCRIBE));
    }

    if (m_NpsOrCad == SimuParamWidget::SimuNPS) { }

    m_pFileSaveConfigWidget = new FileSaveConfigWidget(nullptr);
    m_pFileSaveConfigWidget->setParams(simuComConfig);
    QString strpath = pCurProj->getProjectConfig(KL_PRO::FILE_PYTHONPATH).toString();
    m_pFileSaveConfigWidget->setPythPath(strpath);
    addWidget(m_pFileSaveConfigWidget, PRJSET_FILE);
    addWidget(m_baseinfoWidget, PRJSET_BASE);
}
void setModelShowsimusort(QList<PModel> boardModelList, bool isShowsimusort)
{
    foreach (PModel pmodel, boardModelList) {
        if (Model::Board_Type < pmodel->getModelType()) { // 画板类型
            pmodel->getCanvasContext()->enableShowModelRunningSort(isShowsimusort);
        }
        setModelShowsimusort(pmodel->getChildModels().values(), isShowsimusort);
    }
}
void ProjectSetProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::DialogCode::Accepted) {
        FileSaveConfigWidget::SimuCommonConfig simuComConfig;
        m_pFileSaveConfigWidget->getParams(simuComConfig);
        PIProjectManagerServer pProjServer = RequestServer<IProjectManagerServer>();
        auto pCurProj = pProjServer->GetCurProject();
        if (!pCurProj.isNull()) {
            pCurProj->setProjectConfig(KL_PRO::DATA_RETENTION_ACCURACY, simuComConfig.m_dataPrecision);
            pCurProj->setProjectConfig(KL_PRO::FILE_SAVE_SUFFIX, simuComConfig.m_fileSuffix);
            pCurProj->setProjectConfig(KL_PRO::FILE_SAVE_SIZE, simuComConfig.m_fileSize);
            pCurProj->setProjectConfig(KL_PRO::FILE_PYTHONPATH, m_pFileSaveConfigWidget->getPythonPath());
            pCurProj->setProjectConfig(KL_PRO::SHOW_SIMULATION_SORT, m_baseinfoWidget->showSimuSort());
            pCurProj->setProjectProperty(KL_PRO::DESCRIBE, m_baseinfoWidget->getDescription());

            bool isShowsimusort = m_baseinfoWidget->showSimuSort();
            QList<PModel> boardModelList = pCurProj->getAllBoardModel();
            setModelShowsimusort(boardModelList, isShowsimusort);

        } else {
            LOGOUT(QObject::tr("Failed to set simulation parameters."), LOG_ERROR);
        }
    }
}

bool ProjectSetProperty::checkValue(QString &errorinfo)
{

    if (m_pFileSaveConfigWidget && !m_pFileSaveConfigWidget->checkValue(errorinfo)) {
        return false;
    }
    return true;
}

void FileSaveConfigWidget::initUI()
{
    QGridLayout *gridLayout = new QGridLayout(this);
    int rowNum = -1;

    QLabel *pPrecitionLab = new QLabel(SIMUPARAMS_PRECISION, this);
    m_spin_precision = new QSpinBox(this);
    m_spin_precision->setRange(1, 21);
    QLabel *pFileSizeLab = new QLabel(SIMUPARAMS_MAX_FILE_SIZE, this);
    m_spin_fileSize = new QSpinBox(this);
    m_spin_fileSize->setRange(MIN_FILE_SIZE_KB, MAX_FILE_SIZE_KB);
    QLabel *pSuffix = new QLabel(SIMUPARAMS_SUFFIX, this);
    m_combo_suffix = new QComboBox(this);
    m_combo_suffix->setView(new QListView(this));
    m_combo_suffix->addItem("csv");
    m_combo_suffix->addItem("txt");
    QLabel *pischeck = new QLabel(SIMUPARAMS_ISUSESYSPATH, this);
    m_checkbox = new QCheckBox(this);
    QLabel *pPath = new QLabel(SIMUPARAMS_PYPATH, this);
    m_pathedit = new KLineEdit(this);
    m_selectpath = new QPushButton("...", this);
    gridLayout->addWidget(pFileSizeLab, ++rowNum, 0);
    gridLayout->addWidget(m_spin_fileSize, rowNum, 1);
    gridLayout->addWidget(pPrecitionLab, ++rowNum, 0);
    gridLayout->addWidget(m_spin_precision, rowNum, 1);
    gridLayout->addWidget(pSuffix, ++rowNum, 0);
    gridLayout->addWidget(m_combo_suffix, rowNum, 1);
    gridLayout->addWidget(pischeck, ++rowNum, 0);
    gridLayout->addWidget(m_checkbox, rowNum, 1);
    gridLayout->addWidget(pPath, ++rowNum, 0);
    gridLayout->addWidget(m_pathedit, rowNum, 1);
    gridLayout->addWidget(m_selectpath, rowNum, 2);
    gridLayout->setColumnStretch(1, 1);
    QVBoxLayout *spacer = new QVBoxLayout(this);
    spacer->addStretch();
    gridLayout->addLayout(spacer, ++rowNum, 0);
    this->setLayout(gridLayout);
    connect(m_selectpath, &QPushButton::clicked, this, &FileSaveConfigWidget::onSelectButtonClicked);
    connect(m_checkbox, &QCheckBox::stateChanged, this, &FileSaveConfigWidget::onCheckBox);
}

void FileSaveConfigWidget::onCheckBox(int checked)
{
    m_pathedit->setEnabled(!checked);
    m_selectpath->setEnabled(!checked);
}

void FileSaveConfigWidget::onSelectButtonClicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "", "", "*.exe");
    if (!filepath.isEmpty()) {
        m_pathedit->setText(filepath);
    }
}
bool FileSaveConfigWidget::checkValue(QString &errorinfo)
{
    return true;
}

void FileSaveConfigWidget::setParams(const SimuCommonConfig &simuCom)
{
    m_combo_suffix->setCurrentText(simuCom.m_fileSuffix);
    m_spin_precision->setValue(simuCom.m_dataPrecision);
    m_spin_fileSize->setValue(simuCom.m_fileSize / BASE_KB);
}

void FileSaveConfigWidget::getParams(SimuCommonConfig &simuCom)
{
    simuCom.m_dataPrecision = m_spin_precision->value();
    simuCom.m_fileSuffix = m_combo_suffix->currentText();
    simuCom.m_fileSize = m_spin_fileSize->value() * BASE_KB;
}

void BaseInfoWidget::initUI()
{
    QGridLayout *layout = new QGridLayout(this);
    QLabel *label = new QLabel(PrjDescrition, this);
    m_textEdit = new QTextEdit(this);
    int row = 0;
    QLabel *simulabel = new QLabel(ShowSimuSort, this);
    m_checkbox = new QCheckBox(this);
    layout->addWidget(label, row, 0);
    layout->addWidget(m_textEdit, row, 1);
    layout->addWidget(simulabel, ++row, 0);
    layout->addWidget(m_checkbox, row, 1);
    QVBoxLayout *spacer = new QVBoxLayout(this);
    spacer->addStretch();
    layout->addLayout(spacer, ++row, 0);
}
void FileSaveConfigWidget::setPythPath(const QString &path)
{
    if (path.isEmpty()) {
        m_checkbox->setChecked(true);
    } else {
        m_checkbox->setChecked(false);
        m_pathedit->setText(path);
    }
}

QString FileSaveConfigWidget::getPythonPath()
{
    if (m_checkbox->isChecked()) {
        return QString();
    }
    return m_pathedit->text();
}
#include "ImportBlockProperty.h"
#include "ContextMenu.h"
#include "CoreLib/ServerManager.h"
#include "GlobalDefinition.h"
#include <QFileDialog>
#include <QGridLayout>
#include <QRegExpValidator>

USE_LOGOUT_("ImportBlockProperty")

ImportBlockProperty::ImportBlockProperty(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.pImportblockButton, SIGNAL(clicked()), this, SLOT(onLoadFile())); // textEdited
    connect(ui.m_pBlockNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));

    ui.m_pBlockNameEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
    ui.m_pFileNameEdit->setReadOnly(true);
}

ImportBlockProperty::~ImportBlockProperty() { }

void ImportBlockProperty::onLoadFile()
{
    // 文件夹路径
    m_strFilePath = QFileDialog::getOpenFileName(this, tr("select file"), "/",
                                                 tr("file(*slx *mdl);;")); // 选择文件  文件(*slx *mdl);;

    QFileInfo fileInfo = QFileInfo(m_strFilePath);
    // 文件名
    m_fileName = fileInfo.baseName();
    if (!(m_strFilePath.isEmpty())) {
        ui.m_pFileNameEdit->setText(m_strFilePath);
    }
}

void ImportBlockProperty::onTextChanged(const QString &str)
{
    if (str.isEmpty()) {
        LOGOUT(tr("module name cannot be empty"), LOG_OPERATION_WARNING); // 模块名称不能为空
        return;
    }
    if (str.length() > 32) {
        LOGOUT(tr("module names can only be in english and numbers, with a maximum length of 32 characters"),
               LOG_OPERATION_WARNING); // 模块名称只能是英文和数字，长度最多32个字符
        ui.m_pBlockNameEdit->setText("");
        return;
    }
    if (!(str.mid(0, 1) >= "a" && str.mid(0, 1) <= "z" || str.mid(0, 1) >= "A" && str.mid(0, 1) <= "Z")) {
        LOGOUT(tr("module name can only start in english"), LOG_OPERATION_WARNING); // 模块名称只能是英文开头
        ui.m_pBlockNameEdit->setText("");
        return;
    }
    m_blockName = str;
}

ImportSimulinkBlock::ImportSimulinkBlock(PIProjectManagerServer pProjectServer, QString blockName)
    : m_pProjectServer(pProjectServer), m_blockName(blockName)
{
}

void ImportSimulinkBlock::createBlockByCallback(const QString &strInfo)
{
    PControlBlock pBlock;
    pBlock->setPrototypeName(m_blockName);
    pBlock->setPrototypeName_CHS(pBlock->getPrototypeName());
    pBlock->setPrototypeName_Readable(pBlock->getPrototypeName());
    // pBlock->setIsBuildIn(false);
    // pBlock->setCanDelete(true);
    pBlock->setVerID("0.0.0");
    // pBlock->setClassification(ControlBlock::Other);
    QStringList str_ioput = strInfo.split(";", QString::SkipEmptyParts);
    QStringList str_iput = str_ioput[0].split("=", QString::SkipEmptyParts);
    QStringList str_oput = str_ioput[1].split("=", QString::SkipEmptyParts);
    QStringList inParam;
    if (str_iput.size() > 1) {
        inParam = str_iput[1].split(",", QString::SkipEmptyParts);
    }
    QStringList outParam;
    if (str_oput.size() > 1) {
        outParam = str_oput[1].split(",", QString::SkipEmptyParts);
    }
    foreach (const QString &key, inParam) {
        PVariable variable = pBlock->getInputVariableGroup()->createVariable();
        variable->setDataType(Global::DataType_Double);
        variable->setDefaultValue(0.0);
    }
    foreach (const QString &key, outParam) {
        PVariable variable = pBlock->getOutputVariableGroup()->createVariable();
        variable->setDataType(Global::DataType_Double);
        variable->setDefaultValue(0.0);
    }
    // 生成端口信息
    initPortInfo(pBlock);
    auto project = m_pProjectServer->GetCurProject();
    if (project) {
        project->saveModel(PModel(pBlock));
    }
    emit refreshView();
}

void ImportSimulinkBlock::initPortInfo(PControlBlock pBlock)
{
    // 生成端口信息
    int index = 0;
    // 获取模块端口的group
    PVariableGroup portVaribleGroup = pBlock->getPortManager();
    // 处理输入端口
    if (pBlock->getInputVariableGroup() != nullptr) {
        QList<QString> portNames = pBlock->getInputVariableGroup()->getVariableMapNames();
        int inPortCount = pBlock->getInputVariableGroup()->getVariableMap().size();
        std::sort(portNames.begin(), portNames.end(), [](const QString &str1, const QString &str2) -> bool {
            return str1.right(str1.size() - 2).toInt() < str2.right(str2.size() - 2).toInt();
        });

        for (int inIndex = 0; inIndex < inPortCount; inIndex++) {
            PVariable inputPortVariable = portVaribleGroup->createVariable();
            PVariable variable = pBlock->getInputVariableGroup()
                                         ->findVariable(RoleDataDefinition::NameRole, portNames[inIndex])
                                         .first();
            inputPortVariable->setDataType(variable->getDataType());
            inputPortVariable->setPresetPosition(QPointF(0, 2 * (inIndex + 1)));
            inputPortVariable->setPortType(Variable::ControlIn);
            inputPortVariable->setName(portNames[inIndex]);
            inputPortVariable->setDisplayName(portNames[inIndex]);
            inputPortVariable->setShowMode(variable->getShowMode());
            index++;
        }
        index = 0;
    }
    // 处理输出端口
    if (pBlock->getOutputVariableGroup() != nullptr) {
        QList<QString> portNames = pBlock->getOutputVariableGroup()->getVariableMapNames();
        int outPortCount = pBlock->getOutputVariableGroup()->getVariableMap().size();
        std::sort(portNames.begin(), portNames.end(), [](const QString &str1, const QString &str2) -> bool {
            return str1.right(str1.size() - 2).toInt() < str2.right(str2.size() - 2).toInt();
        });
        for (int outIndex = 0; outIndex < outPortCount; outIndex++) {
            PVariable outPortVariable = portVaribleGroup->createVariable();
            PVariable variable = pBlock->getOutputVariableGroup()
                                         ->findVariable(RoleDataDefinition::NameRole, portNames[outIndex])
                                         .first();
            outPortVariable->setDataType(variable->getDataType());
            outPortVariable->setPresetPosition(QPointF(0, 2 * (outIndex + 1)));
            outPortVariable->setPortType(Variable::ControlIn);
            outPortVariable->setName(portNames[outIndex]);
            outPortVariable->setDisplayName(portNames[outIndex]);
            outPortVariable->setShowMode(variable->getShowMode());
            index++;
        }
    }
}

bool ImportSimulinkBlock::importCallback(void *pParam, const QString &strInfo, Kcc::CodeManager::CompileCode code)
{
    switch (code) {
    case Kcc::CodeManager::Succeed: {
        ImportSimulinkBlock *pThis = static_cast<ImportSimulinkBlock *>(pParam);
        pThis->createBlockByCallback(strInfo);
        delete pThis;
        LOGOUT(tr("import simulink model completed"), LOG_NORMAL); // 导入Simulink模型完成
    } break;
    case Kcc::CodeManager::Failed:
        LOGOUT(tr("importing simulink model failed,%1").arg(strInfo), LOG_ERROR); // 导入Simulink模型失败，%1
        break;
    case Kcc::CodeManager::Compiling:
        LOGOUT(tr("simulink model compilation in progress"), LOG_NORMAL); // Simulink模型编译中
        break;
    case Kcc::CodeManager::SimulinMdlOpening:
        LOGOUT(tr("open simulink model"), LOG_NORMAL); // 打开Simulink模型中
        break;
    case Kcc::CodeManager::SimulinkMdlBuilding:
        LOGOUT(tr("simulink model under construction"), LOG_NORMAL); // Simulink模型构建中
        break;
    case Kcc::CodeManager::SimulinkFilesGenerating:
        LOGOUT(tr("simulink model generation in progress"), LOG_NORMAL); // Simulink模型生成中
        break;
    default:
        break;
    }
    return true;
}
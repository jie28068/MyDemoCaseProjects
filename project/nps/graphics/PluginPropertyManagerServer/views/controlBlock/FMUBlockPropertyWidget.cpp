#include "FMUBlockPropertyWidget.h"
#include "CodeManagerServer/ICodeManagerServer.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"

#include <QFileDialog>
#include <qglobal.h>

USE_LOGOUT_("FMUBlockPropertyWidget")

static const QString FilePath = QObject::tr("FMU Path");
static const QString DescriptionSTR = QObject::tr("Descriptions");
static const int LabelFixedWidth = 65;
static const int DescLabelFixedWidth = 80;
static const int TabelFixedHeight = 180;

using namespace Kcc::BlockDefinition;
using namespace Kcc::CodeManager;

FMUBlockPropertyWidget::FMUBlockPropertyWidget(QSharedPointer<Kcc::BlockDefinition::ControlBlock> pControlBlock,
                                               QWidget *parent)
    : CWidget(parent), m_pControlBlock(pControlBlock), m_bValueChanged(false)
{
    Q_ASSERT(m_pControlBlock);
    initUI();
}

FMUBlockPropertyWidget::~FMUBlockPropertyWidget() { }

void FMUBlockPropertyWidget::initUI()
{
    PVariableGroup fmuParaGroup = m_pControlBlock->createVariableGroup(RoleDataDefinition::FmuParameter);
    if (!fmuParaGroup) {
        LOGOUT(tr("FMU module initialization failed!"), LOG_ERROR);
        return;
    }

    PVariable pathVariable = fmuParaGroup->getVariableByName(FMUPATH);
    if (pathVariable) {
        m_strFilePathFMUOld = pathVariable->getDefaultValue().toString();
    }

    QString strFMUDescription;
    PVariable descVariable = fmuParaGroup->getVariableByName(FMUDESCRIPTION);
    if (descVariable) {
        strFMUDescription = descVariable->getDefaultValue().toString();
    }

    QGridLayout *pGridLayout = new QGridLayout(this);
    m_pLabelFilePath = new QLabel(this);
    m_pLabelFilePath->setFixedWidth(LabelFixedWidth);
    m_pLabelFilePath->setText(FilePath);

    m_pLineEditFilePath = new CustomLineEdit(this);
    m_pLineEditFilePath->setReadOnly(true);
    m_pLineEditFilePath->setDisabled(true);
    m_pLineEditFilePath->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME)));
    if (!isFileExist(m_strFilePathFMUOld)) {
        m_pLineEditFilePath->setStyleSheet("color:red");
    }
    m_pLineEditFilePath->setText(m_strFilePathFMUOld);

    m_pathPushButton = new QPushButton(this);
    m_pathPushButton->setText(tr("Load File"));
    m_pathPushButton->setFixedWidth(LabelFixedWidth);
    connect(m_pathPushButton, SIGNAL(clicked()), this, SLOT(onBlockFileLoading()));

    m_pFMUInfoWidget = new CustomTableWidget(this);
    QList<CustomModelItem> listdata = getModelItem(fmuParaGroup, QStringList(), false, true);
    m_pFMUInfoWidget->setListData(listdata, QStringList() << tr("Attribute Name") << tr("Attribute Value"));
    m_pFMUInfoWidget->setFixedHeight(TabelFixedHeight);

    m_pLabelDescription = new QLabel(this);
    m_pLabelDescription->setFixedWidth(DescLabelFixedWidth);
    m_pLabelDescription->setText(DescriptionSTR);

    m_pTextEditDescription = new QTextEdit(this);
    m_pTextEditDescription->setReadOnly(true);

    m_pTextEditDescription->setText(strFMUDescription);

    pGridLayout->addWidget(m_pLabelFilePath, 0, 0, 1, 1);
    pGridLayout->addWidget(m_pLineEditFilePath, 0, 1, 1, 3);
    pGridLayout->addWidget(m_pathPushButton, 0, 4, 1, 1);
    pGridLayout->addWidget(m_pFMUInfoWidget, 1, 0, 5, 5);
    pGridLayout->addWidget(m_pLabelDescription, 7, 0, 1, 1);
    pGridLayout->addWidget(m_pTextEditDescription, 8, 0, 1, 5);
}
QList<CustomModelItem> FMUBlockPropertyWidget::getModelItem(PVariableGroup &fmuParaGroup, QStringList &listheader,
                                                            bool bparam, bool bReadOnly)
{
    QString strDataType;
    QList<CustomModelItem> listdata;
    for (QString key : listFMUInfoKeys) {
        PVariable variable = fmuParaGroup->getVariableByName(key);
        if (!variable) {
            continue;
        }
        if ("modelName" == key) {
            strDataType = tr("Model Name");
        } else if ("fmiType" == key) {
            strDataType = tr("FMI Type");
        } else if ("fmiVersion" == key) {
            strDataType = tr("FMI Version");
        } else if ("fmuPath" == key) {
            strDataType = tr("File Path");
        } else if ("numberOfContinuousStates" == key) {
            strDataType = tr("Number Of Continuous States");
        } else if ("numberOfEventIndicators" == key) {
            strDataType = tr("Number Of Event Indicators");
        } else if ("generationDateAndTime" == key) {
            strDataType = tr("Generation Date And Time");
        } else if ("generationTool" == key) {
            strDataType = tr("Generation Tool");
        }
        if (!strDataType.isEmpty()) {
            listdata.append(CustomModelItem(key, strDataType, variable->getDefaultValue().toString(),
                                            RoleDataDefinition::ControlTypeTextbox, !bReadOnly));
        }
        strDataType.clear();
    }
    return listdata;
}

void FMUBlockPropertyWidget::onBlockFileLoading()
{
    static QString lastpath = "";
    auto curProject = PropertyServerMng::getInstance().m_projectManagerServer->GetCurProject();
    if (curProject != nullptr) {
        lastpath = curProject->getProjectDir();
    }
    // 文件路径
    QString strFMUFilePath = QFileDialog::getOpenFileName(this, tr("Select FMU file"), lastpath, "FMU file(*fmu);;");

    if (strFMUFilePath.isEmpty()) {
        return;
    }

    PVariableGroup fmuParaGroup = m_pControlBlock->getVariableGroup(RoleDataDefinition::FmuParameter);
    if (PropertyServerMng::getInstance().m_pCodeManagerSvr == nullptr || !fmuParaGroup) {
        LOGOUT(tr("Failed to load FMU file!"), LOG_ERROR);
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool bImportFlag = PropertyServerMng::getInstance().m_pCodeManagerSvr->ImportFMU(strFMUFilePath, m_pControlBlock);
    if (!bImportFlag) {
        LOGOUT(tr("Failed to load FMU file!"), LOG_ERROR);
        return;
    }

    // 刷新新fmu模块信息
    m_pLineEditFilePath->setStyleSheet("color:black");
    m_pLineEditFilePath->setText(strFMUFilePath);

    QList<CustomModelItem> listdata = getModelItem(fmuParaGroup, QStringList(), false, true);
    m_pFMUInfoWidget->setListData(listdata, QStringList() << tr("Attribute Name") << tr("Attribute Value"));

    PVariable descVariable = fmuParaGroup->getVariableByName(FMUDESCRIPTION);
    if (descVariable) {
        m_pTextEditDescription->setText(descVariable->getDefaultValue().toString());
    }

    m_bValueChanged = true;
    emit onFMUBlockChanged(m_pControlBlock);
    QApplication::restoreOverrideCursor();
}

bool FMUBlockPropertyWidget::isFileExist(QString fullFileName)
{
    QFileInfo fileInfo(fullFileName);
    if (fileInfo.isFile()) {
        return true;
    }
    return false;
}

void FMUBlockPropertyWidget::setCWidgetReadOnly(bool bReadOnly)
{
    // if (m_pLabelFilePath != nullptr) {
    //     m_pLabelFilePath->setDisabled(bReadOnly);
    // }
    // if (m_pLineEditFilePath != nullptr) {
    //     m_pLineEditFilePath->setDisabled(bReadOnly);
    // }
    // if (m_pathPushButton != nullptr) {
    //     m_pathPushButton->setDisabled(bReadOnly);
    // }
    // if (m_pFMUInfoWidget != nullptr) {
    //     m_pFMUInfoWidget->setCWidgetReadOnly(bReadOnly);
    // }
    // if (m_pLabelDescription != nullptr) {
    //     m_pLabelDescription->setDisabled(bReadOnly);
    // }
    // if (m_pTextEditDescription != nullptr) {
    //     m_pTextEditDescription->setDisabled(bReadOnly);
    // }
}

bool FMUBlockPropertyWidget::checkValueChanged()
{
    return m_bValueChanged;
}

bool FMUBlockPropertyWidget::saveData(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    bool bachange = false;
    QString str = m_pLineEditFilePath->text();
    if (m_strFilePathFMUOld == str) {
        return bachange;
    }

    if (model == nullptr) {
        return bachange;
    }

    PControlBlock pcblock = model.dynamicCast<ControlBlock>();
    if (pcblock == nullptr) {
        return bachange;
    }

    pcblock->parsePortsAfterBlockTypeChanged();

    bachange = true;
    return bachange;
}

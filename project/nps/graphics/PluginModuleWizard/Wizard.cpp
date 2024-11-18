
#include "Wizard.h"
#include "WizardIamgeSourceWidget.h"
#include "WizardPageBase.h"
#include "WizardPageCodeChange.h"
#include "WizardPageCodeNew.h"
#include "WizardPageInfoChange.h"
#include "WizardPageInfoNew.h"
#include "WizardPageParamChange.h"
#include "WizardPageParamNew.h"
#include <QDebug>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>

Wizard::Wizard(const QString &path, QWidget *parent /*= 0*/)
    : QWizard(parent), m_pBlock(new ControlBlock), m_pOldBlock(nullptr), controlBlock(new ControlBlock)
{
    init(tr("Added Custom Module Wizard"));

    m_pBlock->setVerID(QString("0.0.0"));
    addPage(m_pInfoPage = new ComponentInfoPage(m_pBlock, controlBlock, this));
    addPage(m_imagePage = new IamgeSourceInfoPage(m_pBlock, controlBlock, this));
    addPage(m_pParamPage = new ComponentParamPage(m_pBlock, controlBlock, this));
    addPage(m_pCodePage = new ComponentCodePage(m_pBlock, controlBlock, path, this));
    connect(this, SIGNAL(customButtonClicked(int)), this, SLOT(onCustomButtonClicked(int)));
}

Wizard::Wizard(PModel model, const QString &path, QWidget *parent)
    : QWizard(parent),
      m_pBlock(qSharedPointerDynamicCast<ControlBlock>(model)),
      m_pOldBlock(qSharedPointerDynamicCast<ControlBlock>(model)),
      controlBlock(new ControlBlock)
{
    init(tr("Edit The Custom Module Wizard"));

    addPage(m_pInfoPage = new ComponentInfoPageOnModify(m_pBlock, controlBlock, this));
    addPage(m_imagePage = new IamgeSourceInfoPageOnModify(m_pBlock, controlBlock, this));
    addPage(m_pParamPage = new ComponentParamPageOnModify(m_pBlock, controlBlock, this));
    ComponentCodePageOnModify *modifypage = new ComponentCodePageOnModify(m_pBlock, controlBlock, path, this);
    addPage(m_pCodePage = modifypage);
    connect(this, SIGNAL(customButtonClicked(int)), this, SLOT(onCustomButtonClicked(int)));
    modifypage->setOldBlock(m_pOldBlock);
}

Wizard::Wizard(ModelType modelType, QWidget *parent) : QWizard(parent)
{
    if (modelType == ElectricalMd) {
        m_pElecBlock = PElectricalBlock(new ElectricalBlock);
        elecBlock = PElectricalBlock(new ElectricalBlock);
        elecInit(tr("Added Electrical Module Wizard"));
        addPage(m_pInfoPage = new ComponentInfoPage(m_pElecBlock, elecBlock, this));
        addPage(m_imagePage = new IamgeSourceInfoPage(m_pElecBlock, elecBlock, this));
        addPage(m_pParamPage = new ComponentParamPage(m_pElecBlock, elecBlock, this));
        m_pInfoPage->setElecParameterPage(m_pParamPage);
    } else if (modelType == DeviceModelMd) {
        m_pDeviceBlock = PDeviceModel(new DeviceModel);
        deviceBlock = PDeviceModel(new DeviceModel);
        elecInit(tr("Added electrical equipment type wizard"));
        addPage(m_pInfoPage = new ComponentInfoPage(m_pDeviceBlock, deviceBlock, this));
        addPage(m_pParamPage = new ComponentParamPage(m_pDeviceBlock, deviceBlock, this));
        m_pInfoPage->setElecParameterPage(m_pParamPage);
    }
}

Wizard::Wizard(ModelType modelType, PModel model, QWidget *parent) : QWizard(parent)
{
    if (modelType == ElectricalMd) {
        m_pElecBlock = qSharedPointerDynamicCast<ElectricalBlock>(model);
        elecBlock = PElectricalBlock(new ElectricalBlock);
        elecInit(tr("Modify The Electrical Module Wizard"));
        addPage(m_pInfoPage = new ComponentInfoPageOnModify(m_pElecBlock, elecBlock, this));
        addPage(m_imagePage = new IamgeSourceInfoPageOnModify(m_pElecBlock, elecBlock, this));
        addPage(m_pParamPage = new ComponentParamPageOnModify(m_pElecBlock, elecBlock, this));
    } else if (modelType == DeviceModelMd) {
        m_pDeviceBlock = qSharedPointerDynamicCast<DeviceModel>(model);
        deviceBlock = PDeviceModel(new DeviceModel);
        elecInit(tr("Modify the Electrical Equipment Type wizard"));
        addPage(m_pInfoPage = new ComponentInfoPageOnModify(m_pDeviceBlock, deviceBlock, this));
        addPage(m_pParamPage = new ComponentParamPageOnModify(m_pDeviceBlock, deviceBlock, this));
    }
}

void Wizard::elecInit(const QString &str)
{
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
    setWizardStyle(ModernStyle); // 向导的风格
    setOption(NoCancelButton, true);
    setButtonText(BackButton, tr("Return"));
    setButtonText(NextButton, tr("Next steps"));
    setButtonText(FinishButton, tr("Done"));
    button(BackButton)->setFixedWidth(80);
    button(NextButton)->setFixedWidth(80);
    button(FinishButton)->setFixedWidth(80);
    button(CustomButton1)->setFixedWidth(80);
    setWindowTitle(str);
    resize(640, 480);
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
}

Wizard::~Wizard() { }

void Wizard::initializePage(int id)
{
    QWizard::initializePage(id);
}

void Wizard::cleanupPage(int id)
{
    QWizard::cleanupPage(id);
}

void Wizard::onCurrentIdChanged(int nPageId)
{
    if (page(nPageId) == m_pCodePage) {
        setButtonLayout(QList<WizardButton>()
                        << Stretch << BackButton << /*CommitButton*/ CustomButton1 << FinishButton /*<< Stretch*/);
        button(FinishButton)->setEnabled(false);
        button(CustomButton1)->setFixedWidth(80);
    } else {
        setButtonLayout(QList<WizardButton>() << Stretch << BackButton << NextButton << FinishButton /*<< Stretch*/);
    }
}

void Wizard::onCustomButtonClicked(int which)
{
    if (which == CustomButton1) {
        m_pCodePage->compile();
    }
}

void Wizard::init(const QString &strTitle)
{
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
    setWizardStyle(ModernStyle); // 向导的风格
    setOption(NoCancelButton, true);
    setButtonText(BackButton, tr("Return"));
    setButtonText(NextButton, tr("Next steps"));
    setButtonText(FinishButton, tr("Done"));
    setButtonText(CustomButton1, tr("Compilation"));
    button(BackButton)->setFixedWidth(80);
    button(NextButton)->setFixedWidth(80);
    button(FinishButton)->setFixedWidth(80);
    button(CustomButton1)->setFixedWidth(80);
    setWindowTitle(strTitle);
    resize(640, 480);
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
}

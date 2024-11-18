#ifndef WIZARD_H
#define WIZARD_H

#include "DialogVariableInput.h"
#include "GlobalDefinition.h"

#include "KLModelDefinitionCore.h"
#include <QRegExpValidator>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QWizard>
#include <QWizardPage>

class ComponentInfoPage;
class ComponentInfoPageOnModify;
class ComponentParamPage;
class ComponentParamPageOnModify;
class ComponentCodePage;
class IamgeSourceInfoPage;

using namespace Kcc::BlockDefinition;

class Wizard : public QWizard
{
    Q_OBJECT
public:
    enum ModelType { ElectricalMd, DeviceModelMd };
    // 构造新增模块向导
    Wizard(const QString &path, QWidget *parent = 0);
    // 构造修改模块向导
    Wizard(PModel model, const QString &path, QWidget *parent = 0);
    ~Wizard();

    PControlBlock getControlBlock() { return m_pBlock; }

    PElectricalBlock getElecBlock() { return elecBlock; }

    PDeviceModel getDeviceBlock() { return deviceBlock; }

    /// @brief 新增电气模块或设备类型向导
    /// @param modelType ElectricalMd:电气,DeviceModelMd:设备类型
    /// @param parent
    Wizard(ModelType modelType, QWidget *parent = 0);
    /// @brief 修改电气模块或设备类型向导
    /// @param model
    /// @param parent
    Wizard(ModelType modelType, PModel model, QWidget *parent = 0);

protected:
    virtual void initializePage(int id) override;
    virtual void cleanupPage(int id) override;

private slots:
    void onCurrentIdChanged(int nPageId);
    void onCustomButtonClicked(int which);

private:
    void init(const QString &strTitle);
    void elecInit(const QString &str);

private:
    // 原始的block
    PControlBlock m_pBlock;
    PControlBlock m_pOldBlock;
    PElectricalBlock m_pElecBlock;
    PDeviceModel m_pDeviceBlock;

    // 临时的block
    PControlBlock controlBlock;
    PElectricalBlock elecBlock;
    PDeviceModel deviceBlock;
    // 页面
    ComponentInfoPage *m_pInfoPage;
    ComponentParamPage *m_pParamPage;
    ComponentCodePage *m_pCodePage;
    IamgeSourceInfoPage *m_imagePage;
};

#endif // WIZARD_H

#include "DeviceModelProperty.h"

#include "CommonModelAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"

const QString TAB_NAME_DEV_TYPE = QObject::tr("Device Type");             // 设备类型
const QString PROPERTY_NEW_DEVICETYPE = QObject::tr("New Device Type");   // 新建设备类型
const QString PROPERTY_EDIT_DEVICETYPE = QObject::tr("Edit Device Type"); // 编辑设备类型
using namespace Kcc::BlockDefinition;

DeviceModelProperty::DeviceModelProperty(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devmodel, bool bnew,
                                         const QString &prototypename, bool readOnly)
    : CommonWrapper(readOnly),
      m_devModel(devmodel),
      m_new(bnew),
      m_deviceWidget(nullptr),
      m_prototypeName(prototypename)
{
    m_oldName = "";
    if (m_devModel != nullptr && !bnew) {
        m_oldName = m_devModel->getName();
    }
    if (bnew) {
        m_oldName = getInitDeviceName(prototypename);
    }
}

DeviceModelProperty::~DeviceModelProperty() { }

void DeviceModelProperty::init()
{
    CommonWrapper::init();

    if (m_new) {
        PDeviceModel psysmodel =
                PropertyServerMng::getInstance().m_projectManagerServer->GetSystemDeviceModel(m_prototypeName);
        if (psysmodel != nullptr) {
            m_devModel = psysmodel->copy().dynamicCast<DeviceModel>();
        } else {
            m_devModel = PDeviceModel();
        }
    }
    QList<CustomModelItem> listdata;
    if (m_devModel != nullptr) {
        listdata = CMA::getPropertyModelItemList(m_devModel, RoleDataDefinition::DeviceTypeParameter);
    }
    m_deviceWidget = new NewEditDeviceType(m_oldName, m_prototypeName, listdata, m_new);
    addWidget(m_deviceWidget, CMA::BLOCK_TAB_NAME_DEV_TYPE);
    setPropertyEditableStatus(m_readOnly);
}

QString DeviceModelProperty::getTitle()
{
    if (m_new) {
        return PROPERTY_NEW_DEVICETYPE;
    }
    return PROPERTY_EDIT_DEVICETYPE;
}

QSharedPointer<Kcc::BlockDefinition::DeviceModel> DeviceModelProperty::getDeviceModel()
{
    return m_devModel;
}

void DeviceModelProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code != QDialog::Accepted) {
        m_bPropertyIsChanged = false;
        m_devModel = nullptr;
        return;
    }
    if (m_deviceWidget != nullptr && m_deviceWidget->saveDeviceModel(m_devModel)) {
        m_bPropertyIsChanged = true;
        m_devModel->setModifyTime(QDateTime::currentDateTime());
        if (m_new) {
            PropertyServerMng::getInstance().m_projectManagerServer->AddDeviceModel(m_devModel);
        } else {
            PropertyServerMng::getInstance().m_projectManagerServer->ModifyDeviceModel(m_oldName, m_devModel);
        }
    }
}

const QString DeviceModelProperty::getInitDeviceName(const QString &prototypeName)
{
    QString initName = QString("%1_Type_1").arg(prototypeName);
    int number = 1;
    while (PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(initName)) {
        initName = QString("%1_Type_%2").arg(prototypeName).arg(QString::number(++number));
    }
    return initName;
}

#include "NewEditDeviceType.h"
#include "CommonModelAssistant.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include <QRegExpValidator>

#include "CoreLib/ServerManager.h"

USE_LOGOUT_("NewEditDeviceType")

const QString STR_TYPENAME = QObject::tr("Device Type Name"); //
NewEditDeviceType::NewEditDeviceType(const QString &devicename, const QString &devicePrototype,
                                     const QList<CustomModelItem> &listdata, bool bnew, QWidget *parent)
    : CWidget(parent),
      m_OldName(devicename),
      m_devicePrototype(devicePrototype),
      m_pNameLabel(nullptr),
      m_tableWidget(nullptr),
      m_devicetypeName(nullptr),
      m_newDeviceType(bnew)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(10, 10, 10, 0);

    m_pNameLabel = new QLabel(this);
    QSizePolicy labelSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    labelSizePolicy.setHorizontalStretch(0);
    labelSizePolicy.setVerticalStretch(0);
    labelSizePolicy.setHeightForWidth(m_pNameLabel->sizePolicy().hasHeightForWidth());
    m_pNameLabel->setSizePolicy(labelSizePolicy);
    m_pNameLabel->setText(STR_TYPENAME);
    gridLayout->addWidget(m_pNameLabel, 0, 0, 1, 1);

    m_devicetypeName = new KLineEdit;
    m_devicetypeName->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME), m_devicetypeName));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_devicetypeName->sizePolicy().hasHeightForWidth());
    m_devicetypeName->setSizePolicy(sizePolicy);
    m_devicetypeName->setMinimumSize(QSize(0, 0));
    m_devicetypeName->setText(m_OldName);
    gridLayout->addWidget(m_devicetypeName, 0, 1, 1, 2);
    QList<CustomModelItem> realListData = listdata;
    updateListData(realListData);
    m_tableWidget = new CustomTableWidget(realListData, this);
    m_tableWidget->setVerticalHeaderVisible(false);
    gridLayout->addWidget(m_tableWidget, 1, 0, 1, 3);

    this->setLayout(gridLayout);
    connect(m_tableWidget, &CustomTableWidget::tableDataItemChanged, this, &NewEditDeviceType::onModelDataChanged);
}

NewEditDeviceType::~NewEditDeviceType() { }

bool NewEditDeviceType::saveDeviceModel(QSharedPointer<Kcc::BlockDefinition::DeviceModel> pmodel)
{
    if (pmodel == nullptr || m_tableWidget == nullptr) {
        return false;
    }
    bool bchange = false;
    QString oldname = pmodel->getName();
    QVariant oldvalue = QVariant();
    if (m_newDeviceType) {
        bchange = true;
        pmodel->setName(getNewDeviceTypeName());
        for (CustomModelItem item : m_tableWidget->getNewListData()) {
            if (!item.isValid()) {
                continue;
            }
            pmodel->setVariableData(pmodel->getDeviceTypePVariable(item.keywords), RoleDataDefinition::ValueRole,
                                    item.value);
        }
    } else {
        if (pmodel->getName() != getNewDeviceTypeName()) {
            pmodel->setName(getNewDeviceTypeName());
            bchange = true;
        }
        for (CustomModelItem item : m_tableWidget->getNewListData()) {
            if (!item.isValid()) {
                continue;
            }
            oldvalue = QVariant();
            oldvalue = pmodel->getDeviceTypeVariableValue(item.keywords);
            if (item.value != oldvalue) {
                pmodel->setVariableData(pmodel->getDeviceTypePVariable(item.keywords), RoleDataDefinition::ValueRole,
                                        item.value);
                LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, oldname, item.name, oldvalue, item.value,
                                      item.keywords, item.valueType, item.controlLimit));
                bchange = true;
            }
        }
    }
    return bchange;
}

QString NewEditDeviceType::getNewDeviceTypeName()
{
    if (m_devicetypeName != nullptr) {
        return m_devicetypeName->text().trimmed();
    }
    return QString();
}

void NewEditDeviceType::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_pNameLabel != nullptr) {
        m_pNameLabel->setDisabled(bReadOnly);
    }
    if (m_devicetypeName != nullptr) {
        m_devicetypeName->setDisabled(bReadOnly);
    }
    if (m_tableWidget != nullptr) {
        m_tableWidget->setCWidgetReadOnly(bReadOnly);
    }
}

bool NewEditDeviceType::checkLegitimacy(QString &errorinfo)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return false;
    }
    QString newName = getNewDeviceTypeName();
    if (newName.isEmpty()) {
        errorinfo = NPS::ERRORINFO_DEVICETYPENAME_EMPTY;
        return false;
    }
    if (newName.length() > BLOCK_NAME_MAX_LENGHT) {
        errorinfo = NPS::ERRORINFO_NAME_EXCEEDLENGTH;
        return false;
    }
    if (m_newDeviceType || (!m_newDeviceType && newName != m_OldName)) {
        if (PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(newName)) {
            errorinfo = NPS::ERRORINFO_REPEAT;
            return false;
        }
    }

    if (m_tableWidget != nullptr && !m_tableWidget->checkLegitimacy(errorinfo)) {
        return false;
    }
    return true;
}

const QMap<QString, bool> NewEditDeviceType::getLoadEditMap(const int &loadTypeValue)
{
    QMap<QString, bool> editMap;
    switch (loadTypeValue) {
    case NPS::LOADTYPE_RLC: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_RL: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    case NPS::LOADTYPE_RC: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_R: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, true);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    case NPS::LOADTYPE_LC: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_L: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, true);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    case NPS::LOADTYPE_C: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, true);
        break;
    }
    case NPS::LOADTYPE_OPEN: {
        editMap.insert(NPS::KEYWORDS_LOAD_R, false);
        editMap.insert(NPS::KEYWORDS_LOAD_L, false);
        editMap.insert(NPS::KEYWORDS_LOAD_C, false);
        break;
    }
    default:
        break;
    }
    return editMap;
}

const QMap<QString, bool> NewEditDeviceType::getBatteryEditMap(const int &chargeDynamics)
{
    QMap<QString, bool> editMap;
    editMap.insert(NPS::KEYWORDS_BATTERY_R1, chargeDynamics >= NPS::CHARGEDYNAMICS_1 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_T1, chargeDynamics >= NPS::CHARGEDYNAMICS_1 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_R2, chargeDynamics >= NPS::CHARGEDYNAMICS_2 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_T2, chargeDynamics >= NPS::CHARGEDYNAMICS_2 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_R3, chargeDynamics >= NPS::CHARGEDYNAMICS_3 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_T3, chargeDynamics >= NPS::CHARGEDYNAMICS_3 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_R4, chargeDynamics >= NPS::CHARGEDYNAMICS_4 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_T4, chargeDynamics >= NPS::CHARGEDYNAMICS_4 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_R5, chargeDynamics >= NPS::CHARGEDYNAMICS_5 ? true : false);
    editMap.insert(NPS::KEYWORDS_BATTERY_T5, chargeDynamics >= NPS::CHARGEDYNAMICS_5 ? true : false);
    return editMap;
}

const CustomModelItem &NewEditDeviceType::getCustomModelItem(const QString &keyword,
                                                             const QList<CustomModelItem> &listData)
{
    for (const CustomModelItem &item : listData) {
        if (keyword == item.keywords) {
            return item;
        }
    }
    return CustomModelItem();
}

void NewEditDeviceType::updateListData(QList<CustomModelItem> &listData)
{
    if (NPS::PROTOTYPENAME_LOAD == m_devicePrototype) {
        updateLoadItemList(listData);
    } else if (NPS::PROTOTYPENAME_BATTERY == m_devicePrototype) {
        updateBatteryItemList(listData);
    }
}

void NewEditDeviceType::updateLoadItemList(QList<CustomModelItem> &listData)
{
    QMap<QString, bool> editMap;
    CustomModelItem loadTypeItem = getCustomModelItem(NPS::KEYWORDS_LOAD_TYPE, listData);
    if (loadTypeItem.isValid()) {
        editMap = getLoadEditMap(loadTypeItem.value.toInt());
    }
    for (CustomModelItem &item : listData) {
        if (editMap.contains(item.keywords)) {
            if (!editMap[item.keywords]) {
                item.value = 0;
                item.bcanEdit = false;
            }
        }
    }
}

void NewEditDeviceType::updateBatteryItemList(QList<CustomModelItem> &listData)
{
    QMap<QString, bool> editMap;
    CustomModelItem chargeDynamicsItem = getCustomModelItem(NPS::KEYWORDS_BATTERY_CHARGEDYNAMICS, listData);
    if (chargeDynamicsItem.isValid()) {
        editMap = getBatteryEditMap(chargeDynamicsItem.value.toInt());
    }
    for (CustomModelItem &item : listData) {
        if (editMap.contains(item.keywords)) {
            if (!editMap[item.keywords]) {
                item.bcanEdit = false;
            }
        }
    }
}

void NewEditDeviceType::onModelDataChanged(const CustomModelItem &oldItem, const CustomModelItem &newItem)
{
    if (m_tableWidget == nullptr) {
        return;
    }
    if (NPS::PROTOTYPENAME_LOAD == m_devicePrototype && NPS::KEYWORDS_LOAD_TYPE == newItem.keywords) {
        QMap<QString, bool> editMap = getLoadEditMap(newItem.value.toInt());
        for (QString keyword : editMap.keys()) {
            CustomModelItem tmpItem = m_tableWidget->getItemData(keyword, editMap[keyword]);
            if (!editMap[keyword]) {
                tmpItem.value = 0;
            }
            tmpItem.bcanEdit = editMap[keyword];
            m_tableWidget->setItemData(keyword, tmpItem);
        }
    } else if (NPS::PROTOTYPENAME_BATTERY == m_devicePrototype
               && NPS::KEYWORDS_BATTERY_CHARGEDYNAMICS == newItem.keywords) {
        QMap<QString, bool> editMap = getBatteryEditMap(newItem.value.toInt());
        for (QString keyword : editMap.keys()) {
            CustomModelItem tmpItem = m_tableWidget->getItemData(keyword);
            tmpItem.bcanEdit = editMap[keyword];
            m_tableWidget->setItemData(keyword, tmpItem);
        }
    }
}

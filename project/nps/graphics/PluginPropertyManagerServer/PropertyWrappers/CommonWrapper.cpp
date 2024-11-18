#include "CommonWrapper.h"

CommonWrapper::CommonWrapper(bool readOnly) : QObject(nullptr), m_bPropertyIsChanged(false), m_readOnly(readOnly)
{
    m_TabItemList.clear();
}

CommonWrapper::~CommonWrapper() { }

void CommonWrapper::init()
{
    m_TabItemList.clear();
    m_bPropertyIsChanged = false;
}

QString CommonWrapper::getTitle()
{
    return QString();
}

QStringList CommonWrapper::getTreeTabNameList()
{
    QStringList treeTabNameList;
    for (TabDetailItem item : m_TabItemList) {
        treeTabNameList.append(item.treetabName);
    }
    return treeTabNameList;
}

QStringList CommonWrapper::getTabWidgetNameList(const QString &treeTabName)
{
    for (TabDetailItem item : m_TabItemList) {
        if (treeTabName == item.treetabName) {
            return item.tabWidgetNameList;
        }
    }
    return QStringList();
}

QWidget *CommonWrapper::getCustomPropertyWidget(const QString &treeTabName, const QString &tabWidgetName)
{
    if (treeTabName.isEmpty()) {
        return nullptr;
    }
    for (TabDetailItem item : m_TabItemList) {
        if (treeTabName == item.treetabName) {
            if (item.nameMapWidgetMap.contains(tabWidgetName)) {
                return item.nameMapWidgetMap[tabWidgetName];
            }
            return nullptr;
        }
    }
    return nullptr;
}

void CommonWrapper::setPropertyEditableStatus(bool bReadOnly)
{
    for (TabDetailItem item : m_TabItemList) {
        for (QString tabWidgetName : item.nameMapWidgetMap.keys()) {
            if (item.nameMapWidgetMap[tabWidgetName] == nullptr) {
                continue;
            }
            CWidget *tmpwidget = dynamic_cast<CWidget *>(item.nameMapWidgetMap[tabWidgetName]);
            if (tmpwidget != nullptr) {
                tmpwidget->setCWidgetReadOnly(bReadOnly);
            }
        }
    }
}

void CommonWrapper::onDialogExecuteResult(QDialog::DialogCode code) { }

bool CommonWrapper::checkValue(QString &errorinfo)
{
    errorinfo = "";
    for (TabDetailItem item : m_TabItemList) {
        for (QString tabWidgetName : item.nameMapWidgetMap.keys()) {
            if (item.nameMapWidgetMap[tabWidgetName] == nullptr) {
                continue;
            }
            CWidget *tmpwidget = dynamic_cast<CWidget *>(item.nameMapWidgetMap[tabWidgetName]);
            if (tmpwidget != nullptr && !tmpwidget->checkLegitimacy(errorinfo)) {
                return false;
            }
        }
    }
    return true;
}

bool CommonWrapper::isPropertyChanged()
{
    return m_bPropertyIsChanged;
}

CommonWrapper::LeftWidgetType CommonWrapper::getLeftWidgetType()
{
    return CommonWrapper::LeftWidgetType_Other;
}

QPixmap CommonWrapper::getBlockPixmap()
{
    return QPixmap();
}

QString CommonWrapper::getHelpUrl()
{
    return QString();
}

void CommonWrapper::addWidget(QWidget *widget, const QString &treeTabName, bool pushback, const QString &tabWidgetName)
{
    bool existTreeTab = false;
    for (TabDetailItem &item : m_TabItemList) {
        if (treeTabName == item.treetabName) {
            if (widget == nullptr) {
                return;
            }
            item.nameMapWidgetMap.insert(tabWidgetName.isEmpty() ? treeTabName : tabWidgetName, widget);
            if (pushback) {
                item.tabWidgetNameList.push_back(tabWidgetName.isEmpty() ? treeTabName : tabWidgetName);
            } else {
                item.tabWidgetNameList.push_front(tabWidgetName.isEmpty() ? treeTabName : tabWidgetName);
            }
            existTreeTab = true;
            return;
        }
    }
    if (!existTreeTab) {
        TabDetailItem item = TabDetailItem(treeTabName);
        if (widget != nullptr) {
            item.nameMapWidgetMap.insert(tabWidgetName.isEmpty() ? treeTabName : tabWidgetName, widget);
            item.tabWidgetNameList.append(tabWidgetName.isEmpty() ? treeTabName : tabWidgetName);
        }
        if (pushback) {
            m_TabItemList.push_back(item);
        } else {
            m_TabItemList.push_front(item);
        }
    }
}

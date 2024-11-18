#ifndef NEWEDITDEVICETYPE_H
#define NEWEDITDEVICETYPE_H

#pragma once
#include "KLineEdit.h"
#include "customtablewidget.h"
#include <QGridLayout>
#include <QLabel>

namespace Kcc {
namespace BlockDefinition {
class DeviceModel;
}
}
class NewEditDeviceType : public CWidget
{
public:
    NewEditDeviceType(const QString &devicename, const QString &devicePrototype, const QList<CustomModelItem> &listdata,
                      bool bnew, QWidget *parent = nullptr);
    ~NewEditDeviceType();
    bool saveDeviceModel(QSharedPointer<Kcc::BlockDefinition::DeviceModel> pmodel);
    QString getNewDeviceTypeName();
    virtual void setCWidgetReadOnly(bool bReadOnly);
    virtual bool checkLegitimacy(QString &errorinfo);
public slots:
    void onModelDataChanged(const CustomModelItem &oldItem, const CustomModelItem &newItem);

private:
    const QMap<QString, bool> getLoadEditMap(const int &loadTypeValue);
    const QMap<QString, bool> getBatteryEditMap(const int &chargeDynamics);
    /// @brief 获取指定item
    /// @param keyword 关键字
    /// @param listData
    /// @return
    const CustomModelItem &getCustomModelItem(const QString &keyword, const QList<CustomModelItem> &listData);
    void updateListData(QList<CustomModelItem> &listData);
    /// @brief 更新负载的数据
    /// @param listData
    void updateLoadItemList(QList<CustomModelItem> &listData);
    /// @brief 更新电池的数据
    /// @param listData
    void updateBatteryItemList(QList<CustomModelItem> &listData);

private:
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_devmodel;
    QLabel *m_pNameLabel;
    KLineEdit *m_devicetypeName;
    CustomTableWidget *m_tableWidget;
    bool m_newDeviceType;
    QString m_OldName;
    QString m_devicePrototype;
};

#endif
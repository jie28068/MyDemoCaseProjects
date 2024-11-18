#ifndef DEVICEMODELPROPERTY_H
#define DEVICEMODELPROPERTY_H

#pragma once
#include "CommonWrapper.h"
#include "NewEditDeviceType.h"
#include <QSharedPointer>

namespace Kcc {
namespace BlockDefinition {
class DeviceModel;
}
}

class DeviceModelProperty : public CommonWrapper
{
    Q_OBJECT
public:
    DeviceModelProperty(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devmodel, bool bnew = false,
                        const QString &prototypename = QString(), bool readOnly = false);
    ~DeviceModelProperty();
    virtual void init();
    virtual QString getTitle();
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> getDeviceModel();

    virtual void onDialogExecuteResult(QDialog::DialogCode code);

private:
    const QString getInitDeviceName(const QString &prototypeName);

private:
    bool m_new;
    NewEditDeviceType *m_deviceWidget;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_devModel;
    QString m_prototypeName;
    QString m_oldName;
};

#endif
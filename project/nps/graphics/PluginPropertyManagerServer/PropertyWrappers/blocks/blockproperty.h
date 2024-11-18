#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "CommonModelAssistant.h"
#include "CommonWrapper.h"

#include "PropertyTableModel.h"
#include <QDialog>
#include <QSharedPointer>
#include <QTextEdit>
namespace Kcc {
namespace BlockDefinition {
class SlotBlock;
class Model;
class ElectricalBlock;
class ControlBlock;
class BlockPorts;
}
}
// Block基类属性管理
class BlockProperty : public CommonWrapper
{
    Q_OBJECT
public:
    BlockProperty(QSharedPointer<Kcc::BlockDefinition::Model> model, bool isReadOnly);
    virtual ~BlockProperty();

    virtual void init() override;
    virtual QString getTitle() override;

    // common wrapper
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;
    virtual QStringList getPropertyKeyChangedList();
    virtual CommonWrapper::LeftWidgetType getLeftWidgetType() override;

protected:
    QPixmap drawTextIcon(const QString &textstr, bool isInstance, bool bcombinedBlock = false);

private:
    QString transToWFString(const double &value);

    QString transEnumMapValueToString(const QString &datarange, QVariant value);

    QString getAssociatedBoardsFromIDs(QStringList listString);

protected:
    // 属性值变更的key 列表
    QStringList propertyKeyChangedList;
    QSharedPointer<Kcc::BlockDefinition::Model> m_pModel;
};

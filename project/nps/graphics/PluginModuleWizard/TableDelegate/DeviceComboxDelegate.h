#pragma once
#include "TableBaseDelegate.h"

/// @brief 电气设备类型委托模型
class DeviceComboxDelegate : public TableBaseDelegate
{
    Q_OBJECT
public:
    DeviceComboxDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private:
    /// @brief 变量类型（下拉框）
    mutable QStringList typeList;
};
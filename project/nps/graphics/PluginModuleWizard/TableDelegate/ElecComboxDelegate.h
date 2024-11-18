#pragma once
#include "TableBaseDelegate.h"

/// @brief 电气委托模型
class ElecComboxDelegate : public TableBaseDelegate
{
    Q_OBJECT
public:
    ElecComboxDelegate(QObject *parent);
    ~ElecComboxDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private:
    /// @brief 端口类型（下拉框）
    QStringList portList;
    /// @brief 端口类型-porttype
    QStringList portTypeList;
};
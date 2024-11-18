#pragma once

#include "GlobalDefinition.h"
#include "TableModel/DeviceWizardTableModel.h"
#include "TableModel/ElecWizardTableModel.h"
#include "TableModel/WizardTableModel.h"
#include <QComboBox>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QWidget>

class TableBaseDelegate : public QStyledItemDelegate
{
protected:
    TableBaseDelegate(QObject *parent) : QStyledItemDelegate(parent) { }
    /// @brief 变量类型（下拉框）
    mutable QStringList typeList;
};
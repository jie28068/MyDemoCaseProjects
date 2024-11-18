#pragma once

#include "../KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "GlobalDefinition.h"
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
using namespace Kcc::BlockDefinition;

template<class T>
class WizardTableAbstractModel : public QAbstractTableModel
{
public:
    WizardTableAbstractModel(QObject *parent) : QAbstractTableModel(parent) { }

    /// @brief 清除数据
    void clear()
    {
        beginResetModel();
        m_data.clear();
        endResetModel();
    }

    QStringList getHeaderNames() { return m_listHeader; }

protected:
    QList<T> m_data;          // 数据表
    QStringList m_listHeader; // 表头名称
    QString m_prototypeName;  // 原型名
};

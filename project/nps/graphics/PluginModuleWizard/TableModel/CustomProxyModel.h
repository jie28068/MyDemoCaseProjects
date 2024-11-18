#pragma once
#include "WizardTableAbstractModel.h"

class CustomProxyModel : public QSortFilterProxyModel
{
public:
    CustomProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent),
          m_strFilterString(""),
          m_FilterColumnStr(""),
          m_FilterCol(0),
          filteredRowCount(0)
    {
    }

    void setFilterString(const QString &strFilter)
    {
        m_strFilterString = strFilter;
        filteredRowCount = 0;
        invalidateFilter();
    }

    void setFilterColumn(const QString &colname, const int &colno)
    {
        m_FilterColumnStr = colname;
        m_FilterCol = colno;
        filteredRowCount = 0;
        invalidateFilter();
    }

public:
    /// @brief 对源模型进行排序和过滤操作
    /// @param source_row 源模型中的行号
    /// @param source_parent 源模型中行的父索引。
    /// @return
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

        QString strValue = index.sibling(index.row(), m_FilterCol).data().toString();
        bool falg = strValue.contains(m_strFilterString, Qt::CaseInsensitive);
        if (falg) {
            ++filteredRowCount;
        }
        return falg;
    }

    mutable int filteredRowCount; // 计算当前返回的行数

private:
    QString m_strFilterString; // 输入框文本
    QString m_FilterColumnStr; // 下拉框文本
    int m_FilterCol;           // 下拉框序号
};
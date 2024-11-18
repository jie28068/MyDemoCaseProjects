#ifndef CUSTOMSORTFILTERPROXYMODEL_H
#define CUSTOMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

// 排序和筛选 Model
class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit CustomSortFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent), m_FilterColumn(0), m_strFilterString(QString(""))
    {
    }

    void setFilterString(const QString &strFilter);

    void setFilterColumn(const int &col);

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_strFilterString;
    int m_FilterColumn;
};

#endif // CUSTOMSORTFILTERPROXYMODEL_H

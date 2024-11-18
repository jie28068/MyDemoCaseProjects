#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit SortFilterProxyModel(QObject *parent = nullptr);
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void setRxCol0(const QString rx);
    void setRxCol1(const QString rx);
    void setRxCol2(const QString rx);
    void setRxCol3(const QString rx);
private:
   QString m_rxCol0;
   QString m_rxCol1;
   QString m_rxCol2;
   QString m_rxCol3;

};

#endif // SORTFILTERPROXYMODEL_H

#ifndef MTTABLEMODEL_H
#define MTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include <QSortFilterProxyModel>
#include <QString>

class MTTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MTTableModel(QObject *parent = nullptr);
    MTTableModel(MTTableModel &m);
    void setData(const QList<QVariantMap> &mapList);
    void setHeaderList(const QStringList &list);
    void setHeaderMap(QMap<QString, QString>map);
    void setDefaultData(int section, const QVariant &data);
    QList<QVariantMap>& getTableData(){return tableData;}
    void setTableData(int section, QString key, QVariant value);
    QVariantMap getCurTableData(int idx);
    int headerSection(QString headerName);

//    template<typename T>
    QList<QString> getColumn(QString colName, QSortFilterProxyModel* filter=nullptr);
//    void operator = (MTTableModel m){
//        tableData = m.getTableData();
//    }


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    int columnCount(const QModelIndex &parent=QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    const QVariant defaultData(int section) const;

private:
    QList<QVariantMap> tableData;
    QStringList headerList;//表头
    QMap<QString, QString> headerTranslator;//表头翻译映射
    QMap<int, QVariant> defaultDataMap;


    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // MTTABLEMODEL_H

#ifndef DATAANALYSETABLEVIEW_H
#define DATAANALYSETABLEVIEW_H

#include "def.h"
#include <QAbstractTableModel>
#include <QTableView>

class DataAnalyseTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    DataAnalyseTableModel(QObject *parent);
    ~DataAnalyseTableModel();

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public:
    void setDatas(const QList<DataAnalyseInfo> &datas);

protected:
private:
    QList<QString> m_headerList;
    QList<DataAnalyseInfo> m_datas;
};

class DataAnalyseTableView : public QTableView, public IDataAnalyseObserver
{
    Q_OBJECT

public:
    DataAnalyseTableView(QWidget *parent = nullptr);
    ~DataAnalyseTableView();

    virtual void update(const QList<DataAnalyseInfo> &datas) override;
public slots:
    void onRefreshData(const QList<DataAnalyseInfo> &datas);

private:
    DataAnalyseTableModel *m_model;
};

#endif // DATAANALYSETABLEVIEW_H

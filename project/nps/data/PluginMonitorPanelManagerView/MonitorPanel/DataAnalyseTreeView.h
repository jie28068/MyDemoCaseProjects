#ifndef DataAnalyseTreeVIEW_H
#define DataAnalyseTreeVIEW_H

#include <QAbstractItemModel>
#include <QTreeView>
#include <def.h>

class DataAnalyseTreeItem
{
public:
    explicit DataAnalyseTreeItem(DataAnalyseTreeItem *parent = nullptr);
    ~DataAnalyseTreeItem();

    QVariant data(int col, int role);
    Qt::ItemFlags flags(int col);

    DataAnalyseTreeItem *child(int i);
    DataAnalyseTreeItem *parent(void);

    QList<DataAnalyseTreeItem *> childs(void) { return m_childItems; }
    int childCount(void) const;
    int pos(void) const;
    bool setData(int col, const QVariant &val, int role);
    int columnCount(void) const;
    bool append(DataAnalyseTreeItem *item);
    bool remove(int i);
    bool remove(DataAnalyseTreeItem *item);
    int level(void) const;
    void setLevel(int n);
    void setIcon(QString path);

    void setItemData(QList<QVariant> &itemData) { m_itemData = itemData; }

private:
    QList<DataAnalyseTreeItem *> m_childItems;
    DataAnalyseTreeItem *m_parentItem;
    int m_level;
    QIcon m_icon;
    QList<QVariant> m_itemData;
};

/////////////////////////////////////////////////////////////////////////////////
class DataAnalyseTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    DataAnalyseTreeModel(QObject *parent);
    ~DataAnalyseTreeModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole) override;

    DataAnalyseTreeItem *root(void) const;
    DataAnalyseTreeItem *itemFromIndex(const QModelIndex &index) const;
    void refresh(void);

    void addDateAnalyseInfo(QString &curveName, DataAnalyseInfo &info);
    void clear();

private:
    QStringList m_headerList;
    DataAnalyseTreeItem *m_rootItem;
};

////////////////////////////////////////////////////////////
class DataAnalyseTreeView : public QTreeView
{
    Q_OBJECT

public:
    DataAnalyseTreeView(QWidget *parent);
    ~DataAnalyseTreeView();

    DataAnalyseTreeModel *model(void) { return m_model; }

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    void focusOutEvent(QFocusEvent *event) override;

private:
    DataAnalyseTreeModel *m_model;
};

#endif // DataAnalyseTreeVIEW_H

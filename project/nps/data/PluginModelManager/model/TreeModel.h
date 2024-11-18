#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "TreeItem.h"
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel() override;

    // 增删改查
    TreeItem *rootItem();
    TreeItem *itemFromIndex(const QModelIndex &index) const;

    void addChildItem(TreeItem *childItem);
    void removeChildItem(TreeItem *childItem);
    void removeChildrenItems(TreeItem *parenetItem);

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    // 拖拽相关
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;

public:
    TreeItem *m_codeItem;       // 项目代码型
    TreeItem *m_combineItem;    // 项目构造型
    TreeItem *m_eleCombineItem; // 项目电气构造型

private:
    TreeItem *m_rootItem;
};

//////////////////////////////////////////////////////////////////////////
// 排序和筛选 Model
class TreeViewProxyModel : public QSortFilterProxyModel
{
public:
    explicit TreeViewProxyModel(bool isShowLeaf = true, QObject *parent = nullptr);
    void setFilterString(const QString &strFilter = QString());

    void begin() { beginResetModel(); }
    void end() { endResetModel(); }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    bool m_isShowLeaf;
};

#endif // TREEMODEL_H

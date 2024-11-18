#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>

#include "TreeItem.h"

class ListModel : public QAbstractListModel
{
public:
    ListModel(QObject *parent = nullptr);
    ~ListModel();

    TreeItem *getParentItem() { return m_parentItem; }
    TreeItem *getItemByRow(int row);
    void refreshModelData(TreeItem *parentItem);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                         const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                      const QModelIndex &parent) override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    int rowCount(const QModelIndex &parent) const override;
    Qt::DropActions supportedDropActions() const override;

private:
    TreeItem *m_parentItem;
    QList<TreeItem *> m_lstItem;
};

#endif
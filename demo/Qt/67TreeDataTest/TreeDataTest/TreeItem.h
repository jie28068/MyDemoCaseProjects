#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>

class TreeItem
{
public:
    explicit TreeItem(QString name, TreeItem *parent = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *item);
    void clearChildren();

    TreeItem *child(int row) { return _children.value(row); }
    QList<TreeItem*> children() { return _children; }
    static QList<TreeItem*> grandchildren(TreeItem* parent);
    QList<TreeItem*> grandchildren();

    TreeItem *parent() { return _parent; }
    int childCount() const { return _children.count(); }
    int row() const { return _row; }
    void setRow(int row){ _row = row; }
    QString name() { return _name; }
    void setName(QString name) { _name = name; }

private:
    TreeItem *_parent;            ///<父节点
    QList<TreeItem*> _children;   ///<子节点列表
    int _row;                     ///<记录本item位于父节点下第几个节点
    QString _name;
};

#endif // TREEITEM_H

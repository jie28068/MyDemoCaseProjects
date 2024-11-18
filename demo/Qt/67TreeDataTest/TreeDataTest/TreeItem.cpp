#include "TreeItem.h"

TreeItem::TreeItem(QString name, TreeItem *parent)
    : _parent(parent),
      _row(0),
      _name(name)
{

}

TreeItem::~TreeItem()
{
    clearChildren();
}

/**
 * @brief TreeItem::appendChild
 * 在此节点下增加子节点
 * @param child 节点
 */
void TreeItem::appendChild(TreeItem *item)
{
    item->setRow(_children.size());   // item存自己是第几个，可以优化效率
    _children.append(item);
}

/**
 * @brief TreeItem::clearChildren
 * 清空所有子节点
 */
void TreeItem::clearChildren()
{
    qDeleteAll(_children);
    _children.clear();
}

/**
 * @brief TreeItem::grandchildren
 * 获取指定父节点parent下的所有子节点(递归包括孙子、重孙子节点)
 * @param parent 父节点
 * @return 子节点列表
 */
QList<TreeItem*> TreeItem::grandchildren(TreeItem* parent)
{
    if (parent->childCount() == 0)
    {
        return QList<TreeItem*>();
    }
    QList<TreeItem*> listItems;
    listItems << parent->children();
    foreach (auto child, parent->children())
    {
        listItems << child->grandchildren(child);
    }
    return listItems;
}

/**
 * @brief TreeItem::grandchildren
 * 获取当前节点下的所有子节点(递归包括孙子、重孙子节点)
 * @return 子节点列表
 */
QList<TreeItem*> TreeItem::grandchildren()
{
    return grandchildren(this);
}

#include "TreeItem.h"
#include "ModelManagerConst.h"
#include "ModelViewDefine.h"

TreeItem::TreeItem(QString name, Type type, TreeItem *parent, qint32 order)
    : m_name(name), m_type(type), m_parent(parent),m_order(order)
{
    m_isShow = true;
    m_isAuthorized = true;
    m_modelType = -1;
}

TreeItem::~TreeItem() { }

// 在本节点下添加子节点
void TreeItem::addChild(TreeItem *item)
{
    item->setRow(m_children.size());
    item->setOrder(m_children.size()+1);
    m_children.append(item);

    if (this->parent()) { // 不是rootItem
        item->m_toolkitName = this->m_toolkitName;
        item->m_toolkitType = this->m_toolkitType;
    }
}

void TreeItem::removeChild(TreeItem *item)
{
    // 删除子item
    m_children.removeOne(item);

    // 重新设置item的Row
    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->setRow(i);
        m_children[i]->setOrder(i+1);
    }
}

// 清空所有子节点
void TreeItem::removeChildren()
{
    m_children.clear();
}

int TreeItem::grandSonCount()
{
    int grandSonCnt = 0;
    for (auto item : this->children()) {
        if (item->childCount() > 0) {
            grandSonCnt++;
        }
    }
    return grandSonCnt;
}

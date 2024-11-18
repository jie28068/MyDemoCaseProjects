#ifndef TREEITEM_H
#define TREEITEM_H

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "ModelManagerConst.h"
#include <QModelIndex>
#include <QVariant>

using namespace Kcc::BlockDefinition;
using namespace KL_TOOLKIT;
class TreeItem
{
public:
    // 节点类型
    enum Type {
        Unknown,
        HeadNode,   // 头节点（一级节点
        BranchNode, // 分支节点
        LeafNode,   // 叶子节点
    };

    explicit TreeItem(QString name = "", Type type = Unknown, TreeItem *parent = nullptr, qint32 order = 0);
    ~TreeItem();

    TreeItem *parent() { return m_parent; }

    void addChild(TreeItem *item);
    void removeChild(TreeItem *item);
    void removeChildren();

    TreeItem *child(int row) { return m_children.value(row); }
    QList<TreeItem *> children() { return m_children; }
    void setChildren(QList<TreeItem *> children) { m_children = children; }

    int childCount() const { return m_children.count(); }
    int grandSonCount(); // 获取具有孙子item的子item个数

    void setRow(int row) { m_row = row; } // 保存该节点是其父节点的第几个子节点，查询优化所用
    int row() const { return m_row; }     // 返回本节点位于父节点下第几个子节点
    void setOrder(int order) { m_order = order; };// 设置节点顺序值

    // 自定义数据(公开)
public:
    QString m_name;            // 原型名称
    Type m_type;               // 类型
    bool m_isShow;             // 是否显示
    bool m_isAuthorized;       // 是否被授权
    QVariant m_image;          // 图片数据
    int m_modelType;           // 模型类型
    ToolkitType m_toolkitType; // 工具箱类型 0-项目 1-系统 2-用户
    QString m_toolkitName;     // 所属工具箱名称
    bool m_Expand;             // 分组是否展开
    QModelIndex m_index;       // 树节点Index
    qint32 m_order;        // 分组顺序\模块顺序


    PModel m_model;

private:
    QList<TreeItem *> m_children; // 子节点
    TreeItem *m_parent;           // 父节点
    int m_row;                    // 此item位于父节点中第几个
};

#endif // TREEITEM_H

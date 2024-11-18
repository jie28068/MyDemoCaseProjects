#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "CoreLib/ServerManager.h"
#include "KCustomDialog.h"
#include "KLWidgets/KItemView.h"
#include "TreeModel.h"
#include <QLineEdit>

class ModelTreeView : public KTreeView
{
    Q_OBJECT
public:
    ModelTreeView(QWidget *parent, bool isShowLeaf = false);
    ~ModelTreeView();

    void initTreeView(); // 初始化

    TreeItem *getItemByIndex(const QModelIndex &index);

    TreeModel *getTreeModel() { return m_treeModel; }

    void treeFilter(const QString &strFilter);
    TreeViewProxyModel *getSortModel() { return m_proxyModel; }

    // static接口（简化代码
    static PModel addProjectItem(TreeItem *parentItem);
    static PModel addToolkitItem(TreeItem *parentItem);

    static void editProjectItem(TreeItem *item, const QString &modelName); // 编辑Item
    static void editToolkitItem(TreeItem *item, const QString &modelName); // 编辑Item

    static bool deleteItem(TreeItem *item, QModelIndex parentIndex = QModelIndex(),
                           TreeModel *treeModel = nullptr); // 删除 Item

private slots:
    void onRecevieProjectManagerServerMsg(uint code, const NotifyStruct &param); // 接受 项目管理器服务通知
    void onTreeContextMenu(const QPoint &pos);                                   // 右键菜单

    void onFilter(const QString &strFilter); // 搜索过滤

    void onAddToolkit();     // 添加工具箱
    void onAddGroupItem();   // 添加组
    void onAddProjectItem(); // 添加项目的item
    void onAddToolkitItem(); // 添加工具箱的item

    void onEditItem(); // 编辑

    void onDeleteItem(); // 删除item

    void onCloseAddToolKit(int ret); // 关闭添加工具箱弹窗

private:
    void fillMenuByTreeItem(TreeItem *item); // 根据命中树节点，填充右键菜单

    void recordExpand(QModelIndex parentIndex); // 记录树展开情况
    void restoreExpand();                       // 还原树展开情况

private:
    TreeViewProxyModel *m_proxyModel;
    TreeModel *m_treeModel;

    QList<QModelIndex> m_expandNode; // 已展开的树节点
    bool m_isShowLeaf;

    // UI
    QMenu *m_menu;
    KCustomDialog *m_addToolkitDlg; // 添加工具箱弹窗
    QLineEdit *m_leAddToolkit;      // 添加工具箱的输入框
};

#endif // TREEVIEW_H

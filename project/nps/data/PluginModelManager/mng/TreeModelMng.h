#ifndef TREEMODELMNG_H
#define TREEMODELMNG_H

#include "KLWidgets/KItemView.h"
#include "ModelManagerViewServerMng.h"
#include "ModelTreeView.h"
#include "TreeModel.h"
#include <QJsonObject>

// 树视图model管理类
class TreeModelMng
{
public:
    friend class ModelTreeView;
    friend class ModelListView;
    static TreeModelMng &getInstance();
    ~TreeModelMng();

    void initTreeModel();
    void saveTreeModel();

    TreeModel *getTreeModel() { return m_treeModel; }

private:
    void initProjectModels();               // 项目Model
    void initUserModels();                  // 用户Model，可修改的，项目共享的
    void initSystemModels();                // 系统Model，不可修改，项目共享
    void initAuthorization(TreeItem *item); // 初始化工具箱授权

    void refreshProjectItem();
    void addItemByModelList(QList<PModel> modelList, TreeItem *parentItem);
    void authorizeItem(TreeItem *item, bool isAuthorised);

    void parseJsonObj(QJsonObject obj, TreeItem *parentItem);  // 解析Json
    void fillTreeItem(TreeItem *item, bool isProject = false); // 填充树节点

    bool saveJson(TreeItem *item);
    void fillJsObj(QJsonObject &groupObj, TreeItem *item);

    QVariant getModelImageData(PModel model);

private:
    TreeModelMng();

    QList<ModelTreeView *> m_treeViewList;
    TreeModel *m_treeModel;
};

#endif
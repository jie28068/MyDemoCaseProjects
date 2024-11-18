#ifndef CUSTOMTREEMODEL_H
#define CUSTOMTREEMODEL_H

#include "AssistantDefine.h"
#include "KLModelDefinitionCore/BlockDefinition.h"
#include <QScrollBar>
#include <QSharedPointer>
#include <QStandardItemModel>
#include <QTreeView>

class TreeView;
struct WorkSpaceInfo;
class KLProject;
class ProjDataManager;
namespace Kcc {
namespace BlockDefinition {
class Model;
class DeviceModel;
}
}

namespace Kcc {
namespace PluginComponent {
class IPluginComponentServer;
}
}

struct TreeItemState {

    TreeItemState(const QString _keywords = "", bool _bExpand = false, const QVariantMap otherproperty = QVariantMap())
        : keywords(_keywords), bExpand(_bExpand), otherPropertyMap(otherproperty)
    {
    }
    inline bool equal(const QString &itemstr) const { return (itemstr == this->keywords); }
    inline bool isValid() const { return !keywords.trimmed().isEmpty(); }
    inline bool operator==(const TreeItemState &other) const
    {
        return (keywords == other.keywords) && (bExpand == other.bExpand)
                && (otherPropertyMap == other.otherPropertyMap);
    }
    inline bool operator!=(const TreeItemState &other) const { return !(*this == other); }
    void setProperty(const QString &key, const QVariant &value)
    {
        if (otherPropertyMap.contains(key)) {
            otherPropertyMap[key] = value;
        } else {
            otherPropertyMap.insert(key, value);
        }
    }
    QVariant getProperty(const QString &key) { return otherPropertyMap.value(key, QVariant()); }

    QVariantMap getPropertyMap() { return otherPropertyMap; }
    QString keywords;             // 作为key
    bool bExpand;                 // 是否展开
    QVariantMap otherPropertyMap; // 其他属性
};
class CustomTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum TableType { TableType_DataManager, TableType_NetworkModel };
    enum NodeType {
        NodeType_Invalid,          //""
        NodeType_DataRoot,         //"数据库"
        NodeType_GlobalLib,        //"全局库"
        NodeType_GlobalDeviceType, //"全局库-设备类型"
        NodeType_GlobalCtrBlock,   //"全局库-控制模块"
        NodeType_Workspace,        //"工作空间"
        NodeType_Project,          //"项目"
        NodeType_LocalLib,         //"项目-本地库"
        NodeType_DeviceType,       //"项目-本地库-设备类型"
        NodeType_DeviceTypeClass, //"项目-本地库-设备类型-分类prototypename"/网络模型管理器-设备类型-分类
        NodeType_CodeBlock,                 //"项目-本地库-代码型模板"
        NodeType_CodeBlockClass,            //"项目-本地库-代码型class"
        NodeType_CombinedModel,             //"项目-本地库-构造型模板"
        NodeType_ComplexModel,              //"项目-本地库-复合型模板"
        NodeType_Network,                   //"项目-网络模型"
        NodeType_ElecElementClass,          //"项目-网络模型-元件"/网络模型管理器-元件
        NodeType_Model,                     // model
        NodeType_ModelInstance,             // model instance
        NodeType_SimuResult,                //"项目-仿真结果"
        NodeType_SimuResultSubItem,         //"项目-仿真结果-当前仿真结果item"
        NodeType_NetworkDeviceType,         // 网络模型管理器-设备类型
        NodeType_NetworkElecElement,        // 网络模型管理器-电气元件
        NodeType_NetworkBlockTemplate,      // 网络模型管理器-模块模板
        NodeType_NetworkBlockInstance,      // 网络模型管理器-模块实例
        NodeType_NetworkBlockInstanceClass, // 网络模型管理器-模块实例分类
        NodeType_NetworkComplexTemplate,    // 网络模型管理器-复合模型模板
        NodeType_NetworkComplexInstance,    // 网络模型管理器-复合模型实例
        NodeType_NetworkControlSystem,      // 网络模型管理器-控制系统
        NodeType_NetworkControlBlock,       // 网络模型管理器-控制系统-代码型模块
    };
    enum DeviceTypeClassType {
        DeviceTypeClassType_Other,
        DeviceTypeClassType_System,
        DeviceTypeClassType_Project,
    };
    enum CustomRole {
        CustomRole_NodeType = Qt::UserRole + 300,      // 节点类型，决定菜单类型
        CustomRole_ActiveStatus = Qt::UserRole + 301,  // activestatus
        CustomRole_PrototypeName = Qt::UserRole + 303, // prototypename
        CustomRole_ParentUUID = Qt::UserRole + 304,    // ParentUUID
    };
    // 项目排序模式
    enum ProjectSortType { ProjectSortType_Invalid = -1, ProjectSortType_Name, ProjectSortType_Time };
    CustomTreeModel(QSharedPointer<ProjDataManager> projDataManager, QObject *parent = nullptr);
    ~CustomTreeModel();
    /// @brief 设置widget目标类型
    /// @param wobj
    void setWidgetObject(AssistantDefine::WidgetObject wobj) { m_widgetObject = wobj; }
    /// @brief 获取widget目标类型
    /// @return
    AssistantDefine::WidgetObject getWidgetObject() { return m_widgetObject; }
    /// @brief 更新数据管理器的treeview数据
    /// @param pTreeView treeview
    /// @param currentWorkSpace 当前工作空间名
    /// @param curProject
    void updateTree(TreeView *pTreeView);
    /// @brief 更新网络模型treeview数据
    /// @param pTreeView
    void updateNetWorkTree(TreeView *pTreeView);
    /// @brief 设置初值tree展开状态
    /// @param treeStsMap
    void setInitTreeState(const QVariantMap &treeStsMap);
    /// @brief 获取当前tree展开状态
    /// @return
    QVariantMap getTreeCurState(TreeView *pTreeView);
    /// @brief 获取项目排序模式
    /// @return
    CustomTreeModel::ProjectSortType getSortType();
    /// @brief 初始化项目排序模式
    /// @param initSorttype
    void setInitSortType(const CustomTreeModel::ProjectSortType &initSorttype);
    /// @brief 保存tree的展开状态
    /// @param pTreeView TreeView
    void saveTreeState(TreeView *pTreeView);
    /// @brief 加载tree的展开状态
    /// @param pTreeView TreeView
    void loadTreeState(TreeView *pTreeView);
    /// @brief 获取treeindex
    /// @param pTreeView TreeView
    /// @param indexNames 从top节点到最后节点的列表
    QModelIndex getTreeIndex(const QStringList &indexNames);
    /// @brief 获取从top到当前节点的节点列表
    /// @param index 当前节点的index
    /// @return
    QStringList getIndexNames(const QModelIndex &index);
    /// @brief 获取当前index的相邻indexnames
    /// @param index 当前index
    /// @return 如果index有兄弟，返回最近的兄弟，否则返回父亲
    QStringList getTreeIndexSiblingIndexNames(const QModelIndex &index);
    /// @brief 获取当前节点所属的项目
    /// @param index 当前节点的index
    /// @return
    QSharedPointer<KLProject> getProjectByIndex(const QModelIndex &index);
    /// @brief 获取当前节点所属的工作空间名
    /// @param index 当前节点的index
    /// @return
    QString getWorkSpaceNameByIndex(const QModelIndex &index);
    /// @brief 获取当前节点所属的工作空间名和项目名
    /// @param index 当前节点的index
    /// @param wsname 返回工作空间名
    /// @param projname 返回项目名
    void getWSProjByIndex(const QModelIndex &index, QString &wsname, QString &projname);
    /// @brief 获取当前仿真item信息
    /// @param index 当前节点index
    /// @return
    Kcc::BlockDefinition::SimulationResultInfo getSimuResultInfo(const QModelIndex &index);
    /// @brief 获取项目当前仿真item列表
    /// @param index 当前节点index
    /// @return
    QList<Kcc::BlockDefinition::SimulationResultInfo> getSimuResultInfoList(const QModelIndex &index);
    /// @brief 查看当前result是否原始数据
    /// @param index
    /// @return
    bool isOriginResultInfo(const QModelIndex &index);
    /// @brief 获取当前节点的model
    /// @param index 当前节点的index
    /// @return
    QSharedPointer<Kcc::BlockDefinition::Model> getModel(const QModelIndex &index);
    /// @brief 获取当前节点的model list
    /// @param index 当前节点的index
    /// @return
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> getModels(const QModelIndex &index);
    /// @brief 获取当前节点所在项目的激活状态
    /// @param index 当前节点的index
    /// @return true为激活否则为false
    bool getProjectActiveSts(const QModelIndex &index);
    /// @brief 设置项目排序类型
    /// @param sorttype
    /// @param ptreeView
    void setProjectSortType(const CustomTreeModel::ProjectSortType &sorttype, TreeView *ptreeView);

private:
    void saveTreeState(TreeView *pTreeView, QStandardItem *item_, const QString keyword);
    void loadTreeState(TreeView *pTreeView, QStandardItem *item_, const QString keyword);
    /// @brief 获取treeitem
    /// @param parentItem 父item
    /// @param indexNames
    /// @return
    QStandardItem *getTreeItem(QStandardItem *parentItem, const QStringList &indexNames);
    /// @brief 创建全局库treeview
    /// @param itemRoot treeview根节点
    void CreateGlobalLibrary(QStandardItem *&itemRoot);
    /// @brief 创建项目节点下的treeview
    /// @param pProject 项目
    /// @param projitem 项目节点
    /// @param projactive 项目激活状态，true为激活否则为false
    void createProjectSubTree(QSharedPointer<KLProject> pProject, QStandardItem *&projitem, bool projactive = false);
    /// @brief 创建项目节点下本地库的treeview
    /// @param pProject 项目
    /// @param localLibItem 本地库节点
    void createProjectLocalLib(QSharedPointer<KLProject> pProject, QStandardItem *&localLibItem);
    /// @brief 获取对应model该创建的节点列表数据
    /// @param modellist model数据
    /// @param modeltype model类型
    /// @param icon 节点图标
    /// @param drawboardactivemap 画板激活状态map
    /// @param isInstanceClass 是否实例分类
    /// @return
    QList<QStandardItem *> getModelItems(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &modellist,
                                         const int &modeltype, const QIcon &icon = QIcon(),
                                         const QVariantMap &drawboardactivemap = QVariantMap(),
                                         bool isInstanceClass = false);
    /// @brief 获取控制系统节点列表数据
    /// @param modellist model数据
    /// @param modeltype model类型
    /// @param icon 节点图标
    /// @return
    QList<QStandardItem *>
    CustomTreeModel::getControlSysItems(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &modellist,
                                        const int &modeltype, const QIcon &icon);
    /// @brief 获取设备类型的实际类型
    /// @param index 当前节点index
    /// @return 包括全局设备类型，本地(项目)设备类型
    CustomTreeModel::DeviceTypeClassType getDeviceType(const QModelIndex &index);
    /// @brief 获取项目下对应modelType的实例
    /// @param pProject 项目
    /// @param modelType 模型类型
    /// @param prototypeName 指定模型原型名，为空则不指定
    /// @return
    QList<QSharedPointer<Kcc::BlockDefinition::Model>>
    getAllInstance(QSharedPointer<KLProject> pProject, const int &modelType, const QString &prototypeName = "");
    /// @brief 获取项目电气元件
    /// @param project 项目
    /// @param prototype 元件原型名，原型名为空则返回所有的电气元件
    /// @return 返回指定原型名电气元件
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> getElecElements(QSharedPointer<KLProject> project,
                                                                       const QString &prototype = "");
    /// @brief 获取画板电气元件
    /// @param elecBoardModel 电气画板
    /// @param prototype 元件原型名，为空则返回画板所有元件
    /// @return
    QList<QSharedPointer<Kcc::BlockDefinition::Model>>
    getElecElements(QSharedPointer<Kcc::BlockDefinition::Model> elecBoardModel, const QString &prototype = "");
    /// @brief 排序项目
    /// @param projectMap
    /// @return
    const QStringList sortProjectList(const QMap<QString, QSharedPointer<KLProject>> &projectMap);

private:
    QMap<QString, TreeItemState> m_treeStateMap;
    QString m_curSelectedItemText;
    int m_scrollIndex;
    QSharedPointer<ProjDataManager> m_pProjDataManager;
    AssistantDefine::WidgetObject m_widgetObject;
    CustomTreeModel::ProjectSortType m_projSortType; // 仅数据管理器使用
};

#endif // CUSTOMTREEMODEL_H

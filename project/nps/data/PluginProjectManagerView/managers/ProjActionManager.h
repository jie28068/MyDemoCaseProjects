#ifndef PROJACTIONMANAGER_H
#define PROJACTIONMANAGER_H

#pragma once

#include "AssistantDefine.h"
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMap>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QScopedPointer>

/// @brief 菜单和快捷键管理
class ProjActionManagerPrivate;
class ProjActionInfo;
class ManagerCommonWidget;

class ProjActionManager : public QObject
{
    Q_OBJECT
public:
    enum ActionType {
        Act_startType = 0,
        Act_Up,         // 上一步
        Act_Down,       // 下一步
        Act_Refresh,    // 刷新
        Act_open,       // 打开
        Act_active,     // 激活
        Act_record,     // 记录
        Act_cut,        // 剪切Ctrl+V
        Act_copy,       // 复制Ctrl+C
        Act_paste,      // 粘贴Ctrl+V
        Act_edit,       // 编辑
        Act_delete,     // 删除Del
        Act_rename,     // 重命名
        Act_toolbarNew, // 数据管理器的new按钮，和menu可点击同步
        Act_new, // 新建构造型，新建代码型，新建复合模型，新建电气系统, 新建项目,新建工作空间
        Act_newControlBoard,    // 新建控制系统
        Act_newElecCombine,     // 新建电气构造型模板
        Act_newComplexInstance, // 新建复合模型实例
        Act_newBlockInstance,   // 新建模块型实例
        Act_import,             // 导入
        Act_export,             // 导出
        Act_Undo,               // 撤销
        Act_Redo,               // 重做
        Act_BatchModify,        // 批量修改
        Act_settings,           // 项目设置
        Act_SortProjByName,     // 给项目排序按名称顺序
        Act_SortProjByTime,     // 给项目排序按时间顺序
        Act_EndType,            // 标记菜单值结尾
    };
    enum MenuType {
        MenuType_Invalid, // 无menu类型
        MenuType_Tree,    // tree menu包括newmenu
        MenuType_Table,   // 表格menu
        MenuType_New,     // 新建menu
        MenuType_Sort     // 排序menu
    };

    static ProjActionManager &getInstance();
    AssistantDefine::WidgetObject getActTriggerObj();
    void setActTriggerObj(AssistantDefine::WidgetObject actTriggerObj);
    QAction *getAction(ProjActionManager::ActionType type);

    QList<QAction *> getActionList(const QList<ProjActionManager::ActionType> &types);

    void setActionEnable(ActionType type, bool enable, const QString acttext = "");
    void setActionCheckable(ActionType type, bool checkable);
    /// @brief 获取menu指针
    /// @param menuType menu类型
    /// @return
    QMenu *getMenu(const ProjActionManager::MenuType &menuType);
    /// @brief 获取排序按钮
    /// @return
    QPushButton *getSortButton();
    /// @brief 获取menu action列表
    /// @param menuType menu类型
    /// @return
    const QList<ProjActionManager::ActionType> getMenuActionList(const ProjActionManager::MenuType &menuType);
    /// @brief 设置menu action列表
    /// @param menuType menu类型
    /// @param actEnablelist action列表
    void setMenuActionList(const ProjActionManager::MenuType &menuType,
                           const QList<ProjActionManager::ActionType> &actEnablelist);
    /// @brief 更新tree menu状态
    /// @param actEnablelist 可用Action列表
    /// @param activetext 激活action的文字
    /// @param newtext 新建acttion的文字
    void setTreeMenuStatus(const QList<ProjActionManager::ActionType> &actEnablelist,
                           const QString &activetext = AssistantDefine::STR_ACT_ACTIVE,
                           const QString &newtext = AssistantDefine::STR_ACT_NEW);
    /// @brief 更新tablemenu状态
    /// @param actEnablelist menu可以使用的列表
    /// @param newtext 新建action的文字
    void setTableMenuStatus(const QList<ProjActionManager::ActionType> &actEnablelist,
                            const QString &newtext = AssistantDefine::STR_ACT_NEW);
    /// @brief 更新表格和tree一起的action
    /// @param treeEnablelist tree可操作的action
    /// @param tableEnablelist 表格可操作的action
    /// @param activetext 激活的text
    /// @param newtext 新建的text
    void setAllActionStatus(const QList<ProjActionManager::ActionType> &treeEnablelist,
                            const QList<ProjActionManager::ActionType> &tableEnablelist,
                            const QString &activetext = AssistantDefine::STR_ACT_ACTIVE,
                            const QString &newtext = AssistantDefine::STR_ACT_NEW);
    void connectSignals(ManagerCommonWidget *commonWidget);
    void disconnectSignals(ManagerCommonWidget *commonWidget);

public slots:
    void onNewMenuActionTriggered();
    void onProjectSortByName();
    void onProjectSortByTime();

private:
    ProjActionManager(QObject *parent = 0);
    ~ProjActionManager();

    QAction *createAction(ActionType type, ProjActionInfo *info);
    /// @brief treemenu添加action
    /// @param allList //menu列表
    /// @param newmenuPosBehindAct //新建menu所在action后面的位置
    /// @param newMenuList 新建menu action列表
    /// @param separatorList 分割线，在action之后
    void addTreeMenuAction(const QList<ProjActionManager::ActionType> &allList,
                           const ProjActionManager::ActionType &newmenuPosBehindAct,
                           const QList<ProjActionManager::ActionType> &newMenuList,
                           const QList<ProjActionManager::ActionType> &separatorList);
    /// @brief 为menu添加action
    /// @param menuType menu类型
    /// @param allList 添加列表
    /// @param separatorList 分割线
    void addMenuAction(const ProjActionManager::MenuType &menuType, const QList<ProjActionManager::ActionType> &allList,
                       const QList<ProjActionManager::ActionType> &separatorList);

private:
    QScopedPointer<ProjActionManagerPrivate> dataPtr;
    QMenu *m_treeMenu;
    QMenu *m_newMenu;
    QMenu *m_tableMenu;
    QMenu *m_projSortMenu;
    QPushButton *m_sortBtn;
};

Q_DECLARE_METATYPE(ProjActionManager::ActionType);

#endif
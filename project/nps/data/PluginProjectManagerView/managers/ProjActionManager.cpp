#include "ProjActionManager.h"

#include "ManagerCommonWidget.h"
#include "ProjActionManagerPrivate.h"
#include "ProjectViewServers.h"

ProjActionManager::ProjActionManager(QObject *parent)
    : QObject(parent), m_treeMenu(nullptr), m_newMenu(nullptr), m_tableMenu(nullptr)
{
    dataPtr.reset(new ProjActionManagerPrivate());
    for (int i = int(ProjActionManager::Act_startType) + 1; i < int(ProjActionManager::Act_EndType); i++) {
        ActionType type = ActionType(i);
        if (dataPtr->actionsInfoMap.contains(type)) {
            createAction(type, &dataPtr->actionsInfoMap[type]);
        }
    }
    if (ProjectViewServers::getInstance().m_pUIServer != nullptr
        && ProjectViewServers::getInstance().m_pUIServer->GetMainUI() != nullptr) {
        m_treeMenu = new QMenu(dynamic_cast<QMainWindow *>(ProjectViewServers::getInstance().m_pUIServer->GetMainUI()));
        m_tableMenu =
                new QMenu(dynamic_cast<QMainWindow *>(ProjectViewServers::getInstance().m_pUIServer->GetMainUI()));
        m_projSortMenu =
                new QMenu(AssistantDefine::STR_ACT_SORT,
                          dynamic_cast<QMainWindow *>(ProjectViewServers::getInstance().m_pUIServer->GetMainUI()));
    } else {
        m_treeMenu = new QMenu();
        m_tableMenu = new QMenu();
        m_projSortMenu = new QMenu(AssistantDefine::STR_ACT_SORT);
    }
    QList<ProjActionManager::ActionType> allList;
    allList << ProjActionManager::Act_open << ProjActionManager::Act_active << ProjActionManager::Act_record
            << ProjActionManager::Act_cut << ProjActionManager::Act_copy << ProjActionManager::Act_paste
            << ProjActionManager::Act_edit << ProjActionManager::Act_delete << ProjActionManager::Act_rename
            << ProjActionManager::Act_import << ProjActionManager::Act_export << ProjActionManager::Act_settings;
    QList<ProjActionManager::ActionType> newMenuList;
    newMenuList << ProjActionManager::Act_new << ProjActionManager::Act_newControlBoard
                << ProjActionManager::Act_newElecCombine << ProjActionManager::Act_newComplexInstance
                << ProjActionManager::Act_newBlockInstance;
    // 在对应action后面加分割线
    QList<ProjActionManager::ActionType> separatorList;
    separatorList << ProjActionManager::Act_record << ProjActionManager::Act_paste << ProjActionManager::Act_rename
                  << ProjActionManager::Act_export;
    addTreeMenuAction(allList, ProjActionManager::Act_paste, newMenuList, separatorList);
    QList<ProjActionManager::ActionType> tableList;
    tableList << ProjActionManager::Act_copy << ProjActionManager::Act_paste << ProjActionManager::Act_open
              << ProjActionManager::Act_new << ProjActionManager::Act_delete << ProjActionManager::Act_edit
              << ProjActionManager::Act_BatchModify;
    QList<ProjActionManager::ActionType> tableSeparatorList;
    tableSeparatorList << ProjActionManager::Act_paste;
    addMenuAction(ProjActionManager::MenuType_Table, tableList, tableSeparatorList);
    m_treeMenu->setProperty("isHideIcon", true);
    m_tableMenu->setProperty("isHideIcon", true);
    m_treeMenu->setAccessibleName("treeMenu");
    m_tableMenu->setAccessibleName("tableMenu");
    QList<ProjActionManager::ActionType> sortList;
    sortList << ProjActionManager::Act_SortProjByName << ProjActionManager::Act_SortProjByTime;
    m_projSortMenu->setIcon(QIcon(":/PluginProjectManagerView/resource/ActionsIcon/sort-normal.png"));
    addMenuAction(ProjActionManager::MenuType_Sort, sortList, QList<ProjActionManager::ActionType>());
    m_tableMenu->setAccessibleName("sortMenu");
    connect(getAction(ProjActionManager::Act_SortProjByName), SIGNAL(triggered()), this, SLOT(onProjectSortByName()));
    connect(getAction(ProjActionManager::Act_SortProjByTime), SIGNAL(triggered()), this, SLOT(onProjectSortByTime()));

    m_sortBtn = new QPushButton();
    m_sortBtn->setObjectName("projSortBtn");
    m_sortBtn->setToolTip(AssistantDefine::STR_ACT_SORT);
    m_sortBtn->setFixedSize(QSize(20, 20));
    m_sortBtn->setMenu(getMenu(ProjActionManager::MenuType_Sort));
}

ProjActionManager::~ProjActionManager() { }

QAction *ProjActionManager::createAction(ActionType type, ProjActionInfo *info)
{
    if (nullptr == info) {
        return nullptr;
    }
    QAction *action = nullptr;
    if (info->icon.isNull()) {
        action = new QAction(info->text, this);
    } else {
        action = new QAction(QIcon(info->icon), info->text, this);
    }
    action->setData(QVariant::fromValue(type));
    action->setCheckable(info->checkable);

    if (!info->shortCut.isEmpty()) {
        action->setShortcut(info->shortCut);
    }
    if (!info->tooltips.isEmpty()) {
        action->setToolTip(info->tooltips);
    }

    dataPtr->actionsMap[type] = action;
    action->setIconVisibleInMenu(false); // 设置action是否显示图标
    return action;
}

void ProjActionManager::addTreeMenuAction(const QList<ProjActionManager::ActionType> &allList,
                                          const ProjActionManager::ActionType &newmenuPosBehindAct,
                                          const QList<ProjActionManager::ActionType> &newMenuList,
                                          const QList<ProjActionManager::ActionType> &separatorList)
{
    if (m_treeMenu == nullptr || dataPtr == nullptr) {
        return;
    }
    dataPtr->treeMenuList = allList;
    dataPtr->newMenuList = newMenuList;
    for (ProjActionManager::ActionType actType : allList) {
        m_treeMenu->addAction(getAction(actType));
        if (separatorList.contains(actType)) {
            m_treeMenu->addSeparator();
        }
        if (newmenuPosBehindAct == actType) {
            m_newMenu = new QMenu(AssistantDefine::STR_ACT_NEW, m_treeMenu);
            m_newMenu->setAccessibleName("newMenu");
            for (ProjActionManager::ActionType actType2 : newMenuList) {
                m_newMenu->addAction(getAction(actType2));
            }
            if (m_newMenu->menuAction() != nullptr) {
                m_treeMenu->addAction(m_newMenu->menuAction());
            }
            QAction *tooBarAct = getAction(ProjActionManager::Act_toolbarNew);
            if (tooBarAct != nullptr) {
                connect(tooBarAct, SIGNAL(triggered()), this, SLOT(onNewMenuActionTriggered()));
            }
        }
    }
}

void ProjActionManager::addMenuAction(const ProjActionManager::MenuType &menuType,
                                      const QList<ProjActionManager::ActionType> &allList,
                                      const QList<ProjActionManager::ActionType> &separatorList)
{
    if (getMenu(menuType) == nullptr || dataPtr == nullptr) {
        return;
    }
    setMenuActionList(menuType, allList);
    for (ProjActionManager::ActionType actType : allList) {
        getMenu(menuType)->addAction(getAction(actType));
        if (separatorList.contains(actType)) {
            getMenu(menuType)->addSeparator();
        }
    }
}

ProjActionManager &ProjActionManager::getInstance()
{
    static ProjActionManager instance;
    return instance;
}

AssistantDefine::WidgetObject ProjActionManager::getActTriggerObj()
{
    return dataPtr->actTrigObj;
}

void ProjActionManager::setActTriggerObj(AssistantDefine::WidgetObject actTriggerObj)
{
    dataPtr->actTrigObj = actTriggerObj;
}

QAction *ProjActionManager::getAction(ProjActionManager::ActionType type)
{
    if (type <= Act_startType || type >= Act_EndType) {
        return nullptr;
    }
    return dataPtr->actionsMap.value(type, nullptr);
}

QList<QAction *> ProjActionManager::getActionList(const QList<ProjActionManager::ActionType> &types)
{
    QList<QAction *> actions;
    if (types.isEmpty()) {
        return actions;
    }
    for (ProjActionManager::ActionType type : types) {
        QAction *action = getAction(type);
        if (action != nullptr) {
            actions.append(action);
        }
    }
    return actions;
}

void ProjActionManager::setActionEnable(ActionType type, bool enable, const QString acttext)
{
    QAction *act = getAction(type);
    if (act != nullptr) {
        act->setEnabled(enable);
        if (!acttext.isEmpty()) {
            act->setText(acttext);
        }
        if (act->icon().isNull()) {
            return;
        }
        QString filePath = dataPtr->actionsInfoMap[type].icon;
        int index = filePath.lastIndexOf("/");
        QString path = filePath.left(index + 1);
        QStringList acticonnamelist = filePath.replace(path, "").split("-");
        if (acticonnamelist.size() <= 0) {
            return;
        }

        if (enable) {
            act->setIcon(QIcon(path + QString("%1-normal.png").arg(acticonnamelist[0])));
        } else {
            act->setIcon(QIcon(path + QString("%1-disable.png").arg(acticonnamelist[0])));
        }
    }
}

void ProjActionManager::setActionCheckable(ActionType type, bool checkable)
{
    QAction *actn = getAction(type);
    if (actn == nullptr) {
        return;
    }
    actn->setChecked(checkable);
}

QMenu *ProjActionManager::getMenu(const ProjActionManager::MenuType &menuType)
{
    switch (menuType) {
    case ProjActionManager::MenuType_Tree:
        return m_treeMenu;
    case ProjActionManager::MenuType_Table:
        return m_tableMenu;
    case ProjActionManager::MenuType_New:
        return m_newMenu;
    case ProjActionManager::MenuType_Sort:
        return m_projSortMenu;
    default:
        break;
    }
    return nullptr;
}

QPushButton *ProjActionManager::getSortButton()
{
    return m_sortBtn;
}

const QList<ProjActionManager::ActionType>
ProjActionManager::getMenuActionList(const ProjActionManager::MenuType &menuType)
{
    if (dataPtr == nullptr) {
        return QList<ProjActionManager::ActionType>();
    }
    switch (menuType) {
    case ProjActionManager::MenuType_Tree:
        return dataPtr->treeMenuList + dataPtr->newMenuList;
    case ProjActionManager::MenuType_Table:
        return dataPtr->tableMenuList;
    case ProjActionManager::MenuType_New:
        return dataPtr->newMenuList;
    case ProjActionManager::MenuType_Sort:
        return dataPtr->sortMenuList;
    default:
        break;
    }
    return QList<ProjActionManager::ActionType>();
}

void ProjActionManager::setMenuActionList(const ProjActionManager::MenuType &menuType,
                                          const QList<ProjActionManager::ActionType> &actEnablelist)
{
    if (dataPtr == nullptr) {
        return;
    }
    switch (menuType) {
    case ProjActionManager::MenuType_Tree:
        dataPtr->treeMenuList = actEnablelist;
        break;
    case ProjActionManager::MenuType_Table:
        dataPtr->tableMenuList = actEnablelist;
        break;
    case ProjActionManager::MenuType_New:
        dataPtr->newMenuList = actEnablelist;
        break;
    case ProjActionManager::MenuType_Sort:
        dataPtr->sortMenuList = actEnablelist;
        break;
    default:
        break;
    }
}

void ProjActionManager::setTreeMenuStatus(const QList<ProjActionManager::ActionType> &actEnablelist,
                                          const QString &activetext, const QString &newtext)
{
    if (dataPtr == nullptr) {
        return;
    }
    setActionEnable(ProjActionManager::Act_Up, actEnablelist.contains(ProjActionManager::Act_Up));
    setActionEnable(ProjActionManager::Act_Down, actEnablelist.contains(ProjActionManager::Act_Down));
    setActionEnable(ProjActionManager::Act_Refresh, actEnablelist.contains(ProjActionManager::Act_Refresh));
    for (ProjActionManager::ActionType treeActType : dataPtr->treeMenuList) {
        setActionEnable(treeActType, actEnablelist.contains(treeActType),
                        ProjActionManager::Act_active == treeActType ? activetext : "");
    }
    bool newEnable = false;
    for (ProjActionManager::ActionType newAct : dataPtr->newMenuList) {
        setActionEnable(newAct, actEnablelist.contains(newAct), ProjActionManager::Act_new == newAct ? newtext : "");
        if (actEnablelist.contains(newAct)) {
            newEnable = true;
        }
    }
    if (m_newMenu != nullptr && m_newMenu->menuAction() != nullptr) {
        if (newEnable) {
            m_newMenu->menuAction()->setEnabled(true);
            setActionEnable(ProjActionManager::Act_toolbarNew, true);
        } else {
            m_newMenu->menuAction()->setEnabled(false);
            setActionEnable(ProjActionManager::Act_toolbarNew, false);
        }
    }

    bool sortEnable = false;
    for (ProjActionManager::ActionType sortAct : dataPtr->sortMenuList) {
        if (actEnablelist.contains(sortAct)) {
            sortEnable = true;
            break;
        }
    }
    if (m_sortBtn != nullptr) {
        m_sortBtn->setDisabled(!sortEnable);
    }
}

void ProjActionManager::setTableMenuStatus(const QList<ProjActionManager::ActionType> &actEnablelist,
                                           const QString &newtext)
{
    if (m_tableMenu == nullptr || dataPtr == nullptr) {
        return;
    }
    for (ProjActionManager::ActionType actType : dataPtr->tableMenuList) {
        setActionEnable(actType, actEnablelist.contains(actType), ProjActionManager::Act_new == actType ? newtext : "");
    }
}

void ProjActionManager::setAllActionStatus(const QList<ProjActionManager::ActionType> &treeEnablelist,
                                           const QList<ProjActionManager::ActionType> &tableEnablelist,
                                           const QString &activetext, const QString &newtext)
{
    setTreeMenuStatus(treeEnablelist);
    for (ProjActionManager::ActionType actType : tableEnablelist) {
        setActionEnable(actType, true, ProjActionManager::Act_new == actType ? newtext : "");
    }
}

void ProjActionManager::connectSignals(ManagerCommonWidget *commonWidget)
{
    if (nullptr == commonWidget) {
        return;
    }
    for (int i = int(ProjActionManager::Act_startType) + 1; i < int(ProjActionManager::Act_EndType); i++) {
        ActionType type = ActionType(i);
        QAction *action = getAction(type);
        if (action != nullptr) {
            QObject::connect(action, SIGNAL(triggered()), commonWidget, SLOT(onActionTriggered()),
                             Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
        }
    }
}

void ProjActionManager::disconnectSignals(ManagerCommonWidget *commonWidget)
{
    if (nullptr == commonWidget) {
        return;
    }
    for (int i = int(ProjActionManager::Act_startType) + 1; i < int(ProjActionManager::Act_EndType); i++) {
        ActionType type = ActionType(i);
        QAction *action = getAction(type);
        if (action != nullptr) {
            QObject::disconnect(action, SIGNAL(triggered()), commonWidget, SLOT(onActionTriggered()));
        }
    }
}

void ProjActionManager::onProjectSortByName()
{
    setActionCheckable(ProjActionManager::Act_SortProjByName, true);
    setActionCheckable(ProjActionManager::Act_SortProjByTime, false);
}

void ProjActionManager::onProjectSortByTime()
{
    setActionCheckable(ProjActionManager::Act_SortProjByName, false);
    setActionCheckable(ProjActionManager::Act_SortProjByTime, true);
}

void ProjActionManager::onNewMenuActionTriggered()
{
    if (dataPtr == nullptr) {
        return;
    }
    for (ProjActionManager::ActionType actType : dataPtr->newMenuList) {
        QAction *act = getAction(actType);
        if (act != nullptr && act->isEnabled()) {
            act->trigger();
            break;
        }
    }
}
#ifndef PROJACTIONMANAGERPRIVATE_H
#define PROJACTIONMANAGERPRIVATE_H

#pragma once

#include "ProjActionManager.h"

#include <QAction>
#include <QIcon>
class ProjActionInfo
{
public:
    ProjActionInfo();
    ProjActionInfo(QString text, QKeySequence shortKey = QKeySequence(), const QString icon = "",
                   bool checkable = false, const QString tooltipsstr = "");
    /// @brief 菜单文本描述
    QString text;
    /// @brief 菜单图标路径
    QString icon;
    /// @brief 菜单快捷键
    QKeySequence shortCut;
    /// @brief 菜单是否checkable
    bool checkable;
    QString tooltips;
};

ProjActionInfo::ProjActionInfo() { }

ProjActionInfo::ProjActionInfo(QString text, QKeySequence shortKey, QString icon, bool checkable,
                               const QString tooltipsstr)
{
    this->text = text;
    this->shortCut = shortKey;
    this->icon = icon;
    this->checkable = checkable;
    this->tooltips = tooltipsstr;
}

class ProjActionManagerPrivate
{
public:
    ProjActionManagerPrivate();
    QMap<ProjActionManager::ActionType, QAction *> actionsMap;
    QMap<ProjActionManager::ActionType, ProjActionInfo> actionsInfoMap;
    AssistantDefine::WidgetObject actTrigObj;
    QList<ProjActionManager::ActionType> tableMenuList; // 表格meunu action列表
    QList<ProjActionManager::ActionType> treeMenuList;  // tree meunu action列表
    QList<ProjActionManager::ActionType> newMenuList;   // 新建meenu action列表
    QList<ProjActionManager::ActionType> sortMenuList;  // 排序meenu action列表
};

ProjActionManagerPrivate::ProjActionManagerPrivate()
{
    actTrigObj = AssistantDefine::WidgetObject_Invalid;
    actionsInfoMap[ProjActionManager::Act_Up] =
            ProjActionInfo(AssistantDefine::STR_ACT_UP, QKeySequence(),
                           ":/PluginProjectManagerView/resource/ActionsIcon/up-normal.png");
    actionsInfoMap[ProjActionManager::Act_Down] =
            ProjActionInfo(AssistantDefine::STR_ACT_DOWN, QKeySequence(),
                           ":/PluginProjectManagerView/resource/ActionsIcon/down-normal.png");
    actionsInfoMap[ProjActionManager::Act_Refresh] =
            ProjActionInfo(AssistantDefine::STR_ACT_REFRESH, QKeySequence(),
                           ":/PluginProjectManagerView/resource/ActionsIcon/refresh-normal.png");
    actionsInfoMap[ProjActionManager::Act_open] = ProjActionInfo(AssistantDefine::STR_ACT_OPEN);
    actionsInfoMap[ProjActionManager::Act_active] = ProjActionInfo(AssistantDefine::STR_ACT_ACTIVE);
    actionsInfoMap[ProjActionManager::Act_record] = ProjActionInfo(AssistantDefine::STR_ACT_RECORD);
    // 分割线
    actionsInfoMap[ProjActionManager::Act_cut] =
            ProjActionInfo(AssistantDefine::STR_ACT_CUT, QKeySequence::Cut,
                           ":/PluginProjectManagerView/resource/ActionsIcon/cut-normal.png");
    actionsInfoMap[ProjActionManager::Act_copy] =
            ProjActionInfo(AssistantDefine::STR_ACT_COPY, QKeySequence::Copy,
                           ":/PluginProjectManagerView/resource/ActionsIcon/copy-normal.png");
    actionsInfoMap[ProjActionManager::Act_paste] =
            ProjActionInfo(AssistantDefine::STR_ACT_PASTE, QKeySequence::Paste,
                           ":/PluginProjectManagerView/resource/ActionsIcon/paste-normal.png");
    // 分割线
    actionsInfoMap[ProjActionManager::Act_new] = ProjActionInfo(AssistantDefine::STR_ACT_NEW);
    actionsInfoMap[ProjActionManager::Act_toolbarNew] =
            ProjActionInfo(AssistantDefine::STR_ACT_NEW, QKeySequence::New,
                           ":/PluginProjectManagerView/resource/ActionsIcon/new-normal.png");
    actionsInfoMap[ProjActionManager::Act_newControlBoard] = ProjActionInfo(
            QObject::tr("%1 %2").arg(AssistantDefine::STR_ACT_NEW).arg(AssistantDefine::STR_BOARDTYPE_CONTROLSYSTEM));
    actionsInfoMap[ProjActionManager::Act_newElecCombine] =
            ProjActionInfo(QObject::tr("%1 %2")
                                   .arg(AssistantDefine::STR_ACT_NEW)
                                   .arg(AssistantDefine::STR_BOARDTYPE_ELECCOMBINEMODEL));
    actionsInfoMap[ProjActionManager::Act_newComplexInstance] =
            ProjActionInfo(AssistantDefine::STR_ACT_NEWCOMPLEXINSTANCE);
    actionsInfoMap[ProjActionManager::Act_newBlockInstance] = ProjActionInfo(AssistantDefine::STR_ACT_NEWBLOCKINSTANCE);
    actionsInfoMap[ProjActionManager::Act_edit] = ProjActionInfo(AssistantDefine::STR_ACT_EDIT);
    actionsInfoMap[ProjActionManager::Act_delete] =
            ProjActionInfo(AssistantDefine::STR_ACT_DELETE, QKeySequence::Delete,
                           ":/PluginProjectManagerView/resource/ActionsIcon/delete-normal.png");
    actionsInfoMap[ProjActionManager::Act_rename] = ProjActionInfo(AssistantDefine::STR_ACT_RENAME);
    // 分割线
    actionsInfoMap[ProjActionManager::Act_import] =
            ProjActionInfo(AssistantDefine::STR_ACT_IMPORT, QKeySequence(),
                           ":/PluginProjectManagerView/resource/ActionsIcon/import-normal.png");
    actionsInfoMap[ProjActionManager::Act_export] =
            ProjActionInfo(AssistantDefine::STR_ACT_EXPORT, QKeySequence(),
                           ":/PluginProjectManagerView/resource/ActionsIcon/export-normal.png");
    actionsInfoMap[ProjActionManager::Act_Undo] = ProjActionInfo(AssistantDefine::STR_ACT_UNDO, QKeySequence::Undo);
    actionsInfoMap[ProjActionManager::Act_Redo] = ProjActionInfo(AssistantDefine::STR_ACT_REDO, QKeySequence::Redo);
    actionsInfoMap[ProjActionManager::Act_BatchModify] =
            ProjActionInfo(AssistantDefine::STR_ACT_BATCHMODIFY, QKeySequence());
    actionsInfoMap[ProjActionManager::Act_settings] = ProjActionInfo(AssistantDefine::STR_ACT_SETTINGS, QKeySequence());
    // 项目排序
    actionsInfoMap[ProjActionManager::Act_SortProjByName] =
            ProjActionInfo(AssistantDefine::STR_ACT_SORTBYNAME, QKeySequence(), "", true);
    actionsInfoMap[ProjActionManager::Act_SortProjByTime] =
            ProjActionInfo(AssistantDefine::STR_ACT_SORTBYTIME, QKeySequence(), "", true);
    tableMenuList.clear();
    treeMenuList.clear();
    newMenuList.clear();
    sortMenuList.clear();
}

#endif
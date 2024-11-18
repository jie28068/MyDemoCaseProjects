#ifndef PLUGINPROJECTMANAGERVIEW_H
#define PLUGINPROJECTMANAGERVIEW_H

#pragma once
#include "CoreLib/Module.h"
#include "KLWidgets/KCustomDialog.h"
#include "NetworkModelManagerWidget.h"
#include "ProjDataManager.h"
#include "ProjectManagerWidget.h"
#include <QAction>
#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QThread>
namespace Kcc {
namespace BlockDefinition {
class Model;
}
}
class PluginProjectManagerView : public QObject, public Module
{
    Q_OBJECT

    DEFINE_MODULE

public:
    PluginProjectManagerView(QString strName);
    ~PluginProjectManagerView();

    void init(KeyValueMap &params) override;
    void unInit(KeyValueMap &saveParams) override;
    /// @brief 跳转到model所在画板的tree item位置
    /// @param model
    void JumpToReferenceModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 外部编辑项目，如：simulab调用simuDesinger软件编辑项目
    /// @param projectAbsPath 项目绝对路径
    virtual void editExternalProject(const QString &projectAbsPath);
private slots:
    void onOpenProjManagerAction();
    void onOpenNetworkManagerAction();
    void onOpenDrawboardAction();
    void onOpenQuikisFiles();
    void onAddDrawboardAction();
    void onCloseDataManager(unsigned int, const NotifyStruct &);
    void onProjectManagetHide();
    void onNetworkManagetHide();
    void onTimerOut();

private:
    void showDialog(KCustomDialog *dlg);
    void initDialog(KCustomDialog *dlg);
    void updateFileActions(bool actEnable);

private:
    QAction *m_projManagerOpenAction;
    QAction *m_projNetWorkManagerOpenAction;
    QAction *m_openDrawboardAction;
    QAction *m_addDrawboardAction;
    QAction *m_openQuikisFilesAction;
    QSharedPointer<ProjDataManager> m_ProjDataManager;
    KCustomDialog *m_projDialg;
    ProjectManagerWidget *m_pProjManagerWidget;
    KCustomDialog *m_pNetworkDialg;
    NetworkModelManagerWidget *m_pNetworkManagerWidget;
    QAction *m_actNewWorkspace;      // 新建工作空间
    QAction *m_actNewProject;        // 新建项目空间
    QAction *m_actImportWorkspace;   // 导入工作空间
    QAction *m_actImportProject;     // 导入项目
    QAction *m_actImportDemoProject; // 导入示例项目
    QAction *m_actExportWorkspace;   // 导出工作空间（默认是当前工作空间）
    QAction *m_actExportProject;     // 导出项目（默认是当前项目）
    QTimer *m_pTimer;
};
#endif
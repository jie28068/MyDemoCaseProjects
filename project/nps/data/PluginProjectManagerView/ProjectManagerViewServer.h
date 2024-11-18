#ifndef PROJECTMANAGERVIEWSERVER_H
#define PROJECTMANAGERVIEWSERVER_H

#pragma once

#include "CoreLib/ServerBase.h"
#include "ProjectManagerView/IProjectManagerViewServer.h"
class PluginProjectManagerView;
using namespace Kcc;
using namespace Kcc::ProjectManagerView;
class ProjectManagerViewServer : public QObject, public ServerBase, public IProjectManagerViewServer
{
    Q_OBJECT
public:
    ProjectManagerViewServer(PluginProjectManagerView *projview);
    ~ProjectManagerViewServer();
    /// @brief 跳转到控制模型所在画板
    /// @param model 引用电气元件的控制模型
    virtual void JumpToReferenceModel(const QSharedPointer<Kcc::BlockDefinition::Model> model);

    /// @brief 外部编辑项目，如：simulab调用simuDesinger软件编辑项目
    /// @param projectAbsPath 项目绝对路径
    virtual void editExternalProject(const QString &projectAbsPath);

private:
    PluginProjectManagerView *m_projectManagerView;
};

#endif
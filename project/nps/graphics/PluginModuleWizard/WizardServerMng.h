#ifndef WIZARDSERVERMNG_H
#define WIZARDSERVERMNG_H

#pragma once

#include <QObject>

#include "CodeManagerServer/ICodeManagerServer.h"
using namespace Kcc::CodeManager;

#include "ModelManagerServer/IModelManagerServer.h"
using namespace Kcc::ModelManager;

#include "ProjectManagerServer/IProjectManagerServer.h"
using namespace Kcc::ProjectManager;

#include <server/UILayout/ICoreMainUIServer.h>
using namespace Kcc::CoreManUI_Layout;

class WizardServerMng : public QObject
{
    Q_OBJECT
public:
    static WizardServerMng &getInstance();
    bool init();
    void unInit();

private:
    WizardServerMng();

public:
    PICodeManagerServer m_pCodeManagerServer;     // 代码编辑编译
    PIServerInterfaceBase m_pCodeManagerServerIF; // 代码编辑服务消息响应
    PIProjectManagerServer m_pProjectServer;      // 项目管理服务接口
    PIModelManagerServer m_pModelServer;          // 模型管理服务接口
    PICoreManUILayoutServer m_pUIServerIF;        // UI服务
};

#endif
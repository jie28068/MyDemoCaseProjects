#ifndef PROJECTSERVERMNG_H
#define PROJECTSERVERMNG_H
#pragma once

#include "SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
using namespace Kcc::SimuNPSAdapter;

#include "ModelManagerServer/IModelManagerServer.h"
using namespace Kcc::ModelManager;

#include "PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
using namespace Kcc::PluginGraphicModeling;
class ProjectServerMng : public QObject
{
    Q_OBJECT
public:
    static ProjectServerMng &getInstance();
    bool init();
    void unInit();

private:
    ProjectServerMng();

public:
    PIModelManagerServer m_pModelManagerServer;         // 原型管理（工具箱服务
    PISimuNPSDataDictionaryServer m_pDataDicServer;     // 数据字典服务
    PIPluginGraphicModelingServer m_GraphicModelServer; // 画板服务
};

#endif
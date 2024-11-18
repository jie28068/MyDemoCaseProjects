#ifndef CONFIGINFOMANAGER_H
#define CONFIGINFOMANAGER_H

#include "CoreLib/ServerManager.h"
#include "GlobalDefinition.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "ModelManagerServer/IModelManagerServer.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

using namespace Kcc::ModelManager;
using namespace Kcc::BlockDefinition;
using namespace Kcc::ProjectManager;
using namespace Global;

// 程序产生的所有文件，默认存放位置
#define APP_CONFIG_DIR (QCoreApplication::applicationDirPath() + "/ToolKitModel/")

class ConfigInfoManager
{
public:
    ConfigInfoManager(PIModelManagerServer m_pModelManagerServer,PIProjectManagerServer m_pProjectMngServer);
    ~ConfigInfoManager();
    bool initGroup(Category category);
    PGroupConfigClass getConfigInfo(Category category);
    bool readFile(const QString filePath,Category category);
    bool saveFile(PGroupConfigClass configclass);

 
private:
    QJsonArray parseClass(PGroupConfigClass pluginclass);
    bool parseJson(Category category, const QJsonArray &pluginvalue);
private:
    PGroupConfigClass m_pConfigClass;
    QMap<QString,PGroupConfigClass> m_mapAllConfigFileData;
    PIModelManagerServer m_pModelManagerServer;
    PIProjectManagerServer m_pProjectMngServer;
};
typedef QSharedPointer<ConfigInfoManager> PConfigInfoManager;
#endif
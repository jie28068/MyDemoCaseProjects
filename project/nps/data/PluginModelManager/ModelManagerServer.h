#ifndef PLUGINMODELMANAGERSERVER_H
#define PLUGINMODELMANAGERSERVER_H

#include "qsharedpointer.h"

#include "CoreLib/ServerBase.h"
#include "CoreLib/ServerManager.h"
#include "IModelManagerServer.h"
#include "ModelToolkit.h"

using namespace Kcc;
using namespace Kcc::ModelManager;

class QLibrary;

class ModelManagerServer : public ServerBase, public IModelManagerServer
{
public:
    ModelManagerServer();
    virtual ~ModelManagerServer();

    /// @ 框架相关
    virtual void Init();
    virtual void UnInit();

    /// @ 外部接口 ****************
    virtual QList<Kcc::BlockDefinition::PModel> GetToolkitModels(const QString &toolkitName);

    virtual bool ContainToolkitModel(const QString &modelName, QString &toolkitName);

    virtual Kcc::BlockDefinition::PModel GetToolkitModel(const QString &modelName,
                                                         const QString &toolkitName = QString());

    virtual void SaveToolkitModel(const QString &toolkitName, Kcc::BlockDefinition::PModel model);

    virtual void AddToolkitModel(const QString &toolkitName, Kcc::BlockDefinition::PModel model);

    virtual void RemoveToolkitModel(const QString &toolkitName, const QString &modelName);

    virtual void ClearToolkitModel(const QString &toolkitName);

    virtual QStringList GetToolkitNames() { return m_toolkits.keys(); };

    virtual bool IsFuctionValid(KL_TOOLKIT::DSN_FUCTION fucCode);

    virtual QString GetDllPathByToolkitName(const QString &toolkitName);

public:
    bool addToolkit(const QString &addToolkitName);

    bool removeToolkit(const QString &delToolkitName);

private:
    ModelToolkit *getToolkit(const QString &toolkitName); // 根据工具箱名称获取对应工具箱

private:
    QHash<QString, ModelToolkit *> m_toolkits; // key为工具箱名称
};

typedef QSharedPointer<ModelManagerServer> PModelManagerServer;

#endif // PLUGINMODELMANAGERSERVER_H

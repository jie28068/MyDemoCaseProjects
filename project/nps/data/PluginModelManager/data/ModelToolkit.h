#ifndef CONTROLMODELTOOLKIT_H
#define CONTROLMODELTOOLKIT_H

#include "ModelManagerConst.h"
#include "ProjectManagerServer/IProjectManagerServer.h"

// extern "C" __declspec(dllexport) void *getInstance(); // 获取工具箱符号

class ModelToolkit
{
public:
    ModelToolkit(QString toolkitName);
    ~ModelToolkit();

    QList<Kcc::BlockDefinition::PModel> getModels() const;

    Kcc::BlockDefinition::PModel getModel(const QString &modelName);

    bool containModel(const QString &modelName);

    void saveModel(const Kcc::BlockDefinition::PModel model);

    void addModel(const Kcc::BlockDefinition::PModel addModel);

    void removeModel(const Kcc::BlockDefinition::PModel removeModel);

    void clearModel();

    QString getToolkitName() const { return m_toolkitName; };

    QString getToolkitVersion() const { return "1.0"; }

private:
    void loadSystemModels(); // 加载系统模型

    bool isNeedLoadModel(const QString &toolkitName);
    bool updateModel(PModel model, const QString &fileBaseName);

private:
    QMap<QString, Kcc::BlockDefinition::PModel> m_systemModels; // key-原型名称

    const QString m_toolkitName; // 工具箱名称

    QString m_modelDir;    // 模型文件目录
    QString m_modelSuffix; // 模型文件后缀

    bool m_isLicensed; // 是否授权
};

#endif // CONTROLMODELTOOLKIT_H

#include "ModelManagerServer.h"
#include "CoreLib/ServerManager.h"
#include "DSNFuctionMng.h"
#include "ModelToolkit.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>

USE_LOGOUT_("ModelManagerServer")

ModelManagerServer::ModelManagerServer() { }

ModelManagerServer::~ModelManagerServer() { }

void ModelManagerServer::Init()
{
    // 初始化软件授权信息
    DSNFuctionMng::getInstance().initFuctionInfo();

    // 根据工具箱目录json文件加载工具箱
    QDir dir(KL_TOOLKIT::MODEL_DIR);
    if (!dir.exists()) {
        return;
    }
    auto fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (auto fileInfo : fileInfoList) {
        auto toolkitName = fileInfo.baseName();

        addToolkit(toolkitName);
    }

    // 所有工具箱加载后，调用一下此接口，否则工具箱中构造型模块没有同步
    ModelSystem::getInstance()->loadSystemData();
}

void ModelManagerServer::UnInit() { }

bool ModelManagerServer::IsFuctionValid(KL_TOOLKIT::DSN_FUCTION fucCode)
{
    return DSNFuctionMng::getInstance().isFucLicensed(fucCode);
}

QString ModelManagerServer::GetDllPathByToolkitName(const QString &toolkitName)
{
    return DSNFuctionMng::getInstance().getDllPathByToolkitName(toolkitName);
}

bool ModelManagerServer::addToolkit(const QString &addToolkitName)
{
    if (m_toolkits.contains(addToolkitName)) {
        return false;
    }

    ModelToolkit *toolkit = new ModelToolkit(addToolkitName);

    m_toolkits.insert(toolkit->getToolkitName(), toolkit);

    return true;
}

bool ModelManagerServer::removeToolkit(const QString &delToolkitName)
{
    bool ret = true;

    if (!m_toolkits.contains(delToolkitName)) {
        return false;
    }

    m_toolkits.remove(delToolkitName);

    // 删除json配置文件
    QFileInfo jsonFile(KL_TOOLKIT::MODEL_DIR + delToolkitName + ".json");
    if (jsonFile.isFile()) {
        auto filePath = jsonFile.absoluteFilePath();
        ret &= QFile::remove(filePath);
    }

    // 删除工具箱文件夹
    QDir dir(KL_TOOLKIT::MODEL_DIR + delToolkitName);
    if (dir.exists()) {
        ret &= dir.removeRecursively();
    }

    return ret;
}

QList<Kcc::BlockDefinition::PModel> ModelManagerServer::GetToolkitModels(const QString &toolkitName)
{
    ModelToolkit *toolkit = getToolkit(toolkitName);
    if (toolkit) {
        return toolkit->getModels();
    }

    return QList<Kcc::BlockDefinition::PModel>();
}

bool ModelManagerServer::ContainToolkitModel(const QString &modelName, QString &toolkitName)
{
    foreach (auto toolkit, m_toolkits) {
        if (toolkit->containModel(modelName)) {
            toolkitName = toolkit->getToolkitName();
            return true;
        }
    }

    return false;
}

Kcc::BlockDefinition::PModel ModelManagerServer::GetToolkitModel(const QString &modelName, const QString &toolkitName)
{
    if (toolkitName == QString()) {
        auto keys = m_toolkits.keys();
        for (auto name : keys) {
            ModelToolkit *toolkit = getToolkit(name);
            if (toolkit && toolkit->containModel(modelName)) {
                return toolkit->getModel(modelName);
            }
        }
    } else {
        ModelToolkit *toolkit = getToolkit(toolkitName);
        if (toolkit) {
            return toolkit->getModel(modelName);
        }
    }

    return Kcc::BlockDefinition::PModel();
}

void ModelManagerServer::SaveToolkitModel(const QString &toolkitName, Kcc::BlockDefinition::PModel model)
{
    if (!model) {
        return;
    }

    ModelToolkit *toolkit = getToolkit(toolkitName);
    if (toolkit) {
        return toolkit->saveModel(model);
    }
}

void ModelManagerServer::AddToolkitModel(const QString &toolkitName, Kcc::BlockDefinition::PModel model)
{
    if (!model) {
        return;
    }

    ModelToolkit *toolkit = getToolkit(toolkitName);
    if (toolkit) {
        return toolkit->addModel(model);
    }
}

void ModelManagerServer::RemoveToolkitModel(const QString &toolkitName, const QString &modelName)
{
    ModelToolkit *toolkit = getToolkit(toolkitName);
    if (toolkit) {
        Kcc::BlockDefinition::PModel removeModel = toolkit->getModel(modelName);
        if (removeModel) {
            toolkit->removeModel(removeModel);
        }
    }
}

void ModelManagerServer::ClearToolkitModel(const QString &toolkitName)
{
    ModelToolkit *toolkit = getToolkit(toolkitName);
    if (toolkit) {
        toolkit->clearModel();
    }
}

ModelToolkit *ModelManagerServer::getToolkit(const QString &toolkitName)
{
    if (m_toolkits.contains(toolkitName)) {
        return m_toolkits.value(toolkitName);
    }

    LOGOUT(QString("%1 is not found.").arg(toolkitName));
    return nullptr;
}

#include "ModelToolkit.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "DSNFuctionMng.h"
#include <QDir>

using namespace Kcc::ProjectManager;
using namespace Kcc::BlockDefinition;

USE_LOGOUT_("ModelToolkit")

// void *getInstance()
// {
//     static ModelToolkit cmToolkit;
//     return (void *)&cmToolkit;
// }

ModelToolkit::ModelToolkit(QString toolkitName) : m_toolkitName(toolkitName)
{
    m_modelDir = KL_TOOLKIT::MODEL_DIR + m_toolkitName + "/";
    m_modelSuffix = KL_TOOLKIT::MODEL_FILE_SUFFIX;
    m_isLicensed = DSNFuctionMng::getInstance().isToolKitsLicensed(m_toolkitName);

    loadSystemModels();
}

ModelToolkit::~ModelToolkit() { }

QList<PModel> ModelToolkit::getModels() const
{
    return m_systemModels.values();
}

PModel ModelToolkit::getModel(const QString &modelName)
{
    return m_systemModels.value(modelName, PModel());
}

bool ModelToolkit::containModel(const QString &modelName)
{
    for (auto model : m_systemModels) {
        if (model->getPrototypeName().compare(modelName, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }

    return false;
}

void ModelToolkit::saveModel(const PModel model)
{
    QString modelName = model->getPrototypeName();
    QString filePath = m_modelDir + modelName + m_modelSuffix;

    ModelHelp::writeModelToFile(filePath, model);
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    // 开发者模式时，将Model的信息生成json，方便调试
    ModelHelp::dumpModelFile(filePath + ".json", model);
#endif

    // 工具箱的构造型模块在保存时，根据关联关系同步到画板中的模块
    if (model->getModelType() == Model::Combine_Board_Type || model->getModelType() == Model::Ele_CombineBoard_Type) {
        auto modelList = model->getConnectedModel(ModelConnOriginCombine);
        for (auto wmodel : modelList) {
            auto target = wmodel.toStrongRef();
            if (target.isNull()) {
                continue;
            }

            ModelSystem::getInstance()->syncSystemCombineModel(target, model);
        }
    }
}

void ModelToolkit::addModel(const PModel addModel)
{
    QString modelName = addModel->getPrototypeName();

    if (m_systemModels.contains(modelName)) {
        return;
    }
    m_systemModels.insert(modelName, addModel);

    QString filePath = m_modelDir + modelName + m_modelSuffix;

    addModel->setModelToolkitName(m_toolkitName);
    addModel->setModelAuthorize(true); // fixme 授权
    ModelHelp::writeModelToFile(filePath, addModel);

#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    // 开发者模式时，将Model的信息生成json，方便调试
    ModelHelp::dumpModelFile(filePath + ".json", addModel);
#endif
}

void ModelToolkit::removeModel(const PModel removeModel)
{
    QString modelName = removeModel->getPrototypeName();

    if (!m_systemModels.contains(modelName)) {
        return;
    }
    m_systemModels.remove(modelName);

    QString filePath = m_modelDir + modelName + m_modelSuffix;

    QFile::remove(filePath);

#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    // 开发者模式时，删除调试json
    QFile::remove(filePath + ".json");
#endif
}

void ModelToolkit::clearModel()
{
    foreach (auto model, m_systemModels) {
        QString modelName = model->getPrototypeName();
        QString filePath = m_modelDir + modelName + m_modelSuffix;

        QFile::remove(filePath);
    }
    m_systemModels.clear();
}

void ModelToolkit::loadSystemModels()
{
    QDir dir(m_modelDir);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList(QStringList({ "*.tkmodel", "*.klmodel" }), QDir::Files);
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString newName = fileInfo.absoluteFilePath();

        if (fileInfo.suffix() == "tkmodel") {
            // 重命名tkmodel为klmodel，如果已存在对应的klmodel则删除tkmodel
            newName.replace(".tkmodel", ".klmodel");
            if (QFile::exists(newName)) {
                QFile::remove(fileInfo.absoluteFilePath());
                continue;
            }
            QFile::rename(fileInfo.absoluteFilePath(), newName);
        }

        if (!isNeedLoadModel(m_toolkitName)) { // 是否需要加载Model
            continue;
        }

        PModel model = ModelHelp::readModelFromFile(newName); // 反序列化
        if (model.isNull()) {
            continue;
        }

        // #ifdef _DEBUG
        // updateModel(model, fileInfo.baseName()); // Debug 手动修改model文件名称后，调整原型名称
        // #endif

        QString modelName = model->getPrototypeName();
        if (m_systemModels.contains(modelName)) {
            continue;
        }

        m_systemModels.insert(modelName, model);
        model->setModelToolkitName(m_toolkitName);
        model->setModelAuthorize(m_isLicensed);
        ModelSystem::getInstance()->registerSystemModel(model);
    }
}

bool ModelToolkit::isNeedLoadModel(const QString &toolkitName)
{
    // 外部启动Designer @TODO 后期这种服务单独提供exe服务进程，去除此处理
    if (gConfGet("serviceMode").toBool()) {
        // 不是默认控制和电气工具箱，不加载
        if (toolkitName != KL_TOOLKIT::CONTROL_TOOLKIT && toolkitName != KL_TOOLKIT::ELECTRICAL_TOOLKIT) {
            return false;
        }
    }

    return true;
}

QStringList g_updateModelNames = QStringList() << "SCL_mod"
                                               << "VCT_mod"
                                               << "EXC_mod"
                                               << "GTE_input"
                                               << "OEL_input"
                                               << "SCL_PowP"
                                               << "OEL_PowP"
                                               << "EXC_SCL1C_Kp_Ki";
bool ModelToolkit::updateModel(PModel model, const QString &fileBaseName)
{
    if (g_updateModelNames.contains(fileBaseName)) {
        auto oldPrototypeName = model->getPrototypeName();
        model->setPrototypeName(fileBaseName);
        model->setPrototypeName_CHS(fileBaseName);
        model->setPrototypeName_Readable(fileBaseName);
        model->setName(fileBaseName);
        this->saveModel(model);
    }

    return true;
}

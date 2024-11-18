#include "ConfigInfoManager.h"
#include "QPHelper.h"

USE_LOGOUT_("ConfigInfoManager")

ConfigInfoManager::ConfigInfoManager(PIModelManagerServer m_pModelManagerServer,
                                     PIProjectManagerServer m_pProjectMngServer)
    : m_pModelManagerServer(m_pModelManagerServer), m_pProjectMngServer(m_pProjectMngServer)
{
}

ConfigInfoManager::~ConfigInfoManager() { 
}

bool ConfigInfoManager::initGroup(Category category)
{
    if (category == Device) {
        return readFile(APP_CONFIG_DIR + deviceConfigFileName + ".json", category);
    } else if (category == Controller) {
        return readFile(APP_CONFIG_DIR + controlConfigFileName + ".json", category);
    }
    return false;
}

bool ConfigInfoManager::readFile(const QString filePath, Category category)
{
    // 检查config文件
    if (!QFile::exists(filePath)) {
        return false;
    }
    QString fileContent;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QJsonDocument jsonData = QJsonDocument::fromJson(file.readAll());
    file.close();
    // 反序列化插件
    bool isSuccess = parseJson(category, jsonData["GroupList"].toArray());
    m_mapAllConfigFileData[QString(category)] = m_pConfigClass;
    return isSuccess;
}

bool ConfigInfoManager::saveFile(PGroupConfigClass configclass)
{
    QJsonObject root;
    root["GroupList"] = parseClass(m_pConfigClass);
    QJsonDocument rootDoc(root);

    QFile file(APP_CONFIG_DIR + configclass->cfgFileName + ".json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream text(&file);
    text.setCodec("UTF-8");
    text << rootDoc.toJson();
    file.close();

    return true;
}

PGroupConfigClass ConfigInfoManager::getConfigInfo(Category category)
{
    return m_mapAllConfigFileData[QString(category)];
}

QJsonArray ConfigInfoManager::parseClass(PGroupConfigClass configclass)
{
    QJsonArray pluginvalue;
    if (configclass == nullptr) {
        return pluginvalue;
    }
    // 新建map存储分组名称和顺序
    QMap<int, QString> groupNames;
    for (QString groupname : configclass->mapGroups.keys()) {
        groupNames.insert(qint32(configclass->mapGroups[groupname]->groupOrder), groupname);
    }
    QMap<int, QString>::iterator groupNameItor;
    for (groupNameItor = groupNames.begin(); groupNameItor != groupNames.end(); groupNameItor++) {
        QJsonArray block;
        QJsonObject groupitem;
        if (configclass->mapGroups[groupNameItor.value()] == nullptr) {
            continue;
        }
        groupitem["groupName"] = groupNameItor.value();
        groupitem["groupOrder"] = groupNameItor.key();
        groupitem["groupIsExpand"] = configclass->mapGroups[groupNameItor.value()].data()->groupIsExpand;
        // 序列化block参数
        QMap<int, QString> blockNames;
        for (QString blockName : configclass->mapGroups[groupNameItor.value()]->groupBlocksMap.keys()) {
            blockNames.insert(
                    qint32(configclass->mapGroups[groupNameItor.value()]->groupBlocksMap[blockName].displayorder),
                    blockName);
        }
        // 新建map存储模块名称和顺序
        QMap<int, QString>::iterator blockNameItor;
        for (blockNameItor = blockNames.begin(); blockNameItor != blockNames.end(); blockNameItor++) {
            QJsonObject blockitem;
            blockitem["prototypeName"] = blockNameItor.value();
            blockitem["displayorder"] = blockNameItor.key();
            blockitem["display"] =
                    configclass->mapGroups[groupNameItor.value()]->groupBlocksMap[blockNameItor.value()].display;
            block.append(blockitem);
        }
        groupitem["ModelList"] = block;
        pluginvalue.append(groupitem);
    }
    return pluginvalue;
}

bool ConfigInfoManager::parseJson(Category category, const QJsonArray &pluginvalue)
{
    if (pluginvalue.isEmpty()) {
        LOGOUT(QObject::tr("the configuration file is empty!"), LOG_ERROR); // 配置文件为空！
        return false;
    }
    m_pConfigClass = PGroupConfigClass(new GroupConfigClass);
    if (category == Category::Controller) {
        m_pConfigClass->cfgFileName = controlConfigFileName;
    } else if (category == Category::Device) {
        m_pConfigClass->cfgFileName = deviceConfigFileName;
    }
    for (unsigned int i = 0; i < pluginvalue.size(); ++i) {
        // const Json::Value &groupvalue = pluginvalue[i];
        PGroupClass groupclass(new GroupClass);
        groupclass->groupName = pluginvalue[i]["groupName"].toString();
        groupclass->groupOrder = pluginvalue[i]["groupOrder"].toInt();
        groupclass->groupIsExpand = pluginvalue[i]["groupIsExpand"].toBool();
        for (unsigned int j = 0; j < pluginvalue[i]["ModelList"].toArray().size(); ++j) {
            GroupClass::GroupBlockParamater param;
            PModel block;
            QString prototypeName = pluginvalue[i]["ModelList"][j]["prototypeName"].toString();
            if (category == Category::Device) {
                block = m_pModelManagerServer->GetToolkitModel(prototypeName, KL_TOOLKIT::ELECTRICAL_TOOLKIT);
            } else if (category == Category::Controller) {
                block = m_pModelManagerServer->GetToolkitModel(prototypeName, KL_TOOLKIT::CONTROL_TOOLKIT);
            }
            if (block.isNull()) {
                continue;
            }
            param.prototypeName = prototypeName;
            param.viewName = block->getPrototypeName_CHS();
            param.viewIcon = block->getResource().value((Kcc::BlockDefinition::PNG_NORMAL_PIC));
            param.displayorder = pluginvalue[i]["ModelList"][j]["displayorder"].toInt();
            param.display = pluginvalue[i]["ModelList"][j]["display"].toBool();
            groupclass->groupBlocksMap[param.prototypeName] = param;
        }
        // 处理本地代码型、构造型、本地构造元件
        if (QObject::tr(groupclass->groupName.toUtf8()) == ControllerGroupCustomCode
            || QObject::tr(groupclass->groupName.toUtf8()) == ControllerGroupCustomBoard
            || QObject::tr(groupclass->groupName.toUtf8()) == DeviceGroupLocalCombine) {
            QList<PModel> blockList;
            qint16 order = 1;
            if (QObject::tr(groupclass->groupName.toUtf8()) == ControllerGroupCustomCode) {
                blockList = m_pProjectMngServer->GetAllBoardModel(ControlBlock::Type);
            }
            if (QObject::tr(groupclass->groupName.toUtf8()) == ControllerGroupCustomBoard) {
                blockList = m_pProjectMngServer->GetAllBoardModel(CombineBoardModel::Type);
            }
            if (QObject::tr(groupclass->groupName.toUtf8()) == DeviceGroupLocalCombine) {
                blockList = m_pProjectMngServer->GetAllBoardModel(ElecCombineBoardModel::Type);
            }
            QString language = QPHelper::getLanguage();
            for (PModel block : blockList) {
                GroupClass::GroupBlockParamater param;
                param.prototypeName = block->getPrototypeName();
                param.viewName = block->getPrototypeName_CHS();
                param.viewIcon = block->getResource().value((Kcc::BlockDefinition::PNG_NORMAL_PIC));
                param.displayorder = order++;
                param.display = true;
                groupclass->groupBlocksMap[param.prototypeName] = param;
            }
            order = 1;
        }
        m_pConfigClass->mapGroups[pluginvalue[i]["groupName"].toString()] = groupclass;
    }
    return true;
}

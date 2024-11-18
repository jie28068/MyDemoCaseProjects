#include "GraphicsConfig.h"
#include "nlohmann/json.hpp"

#include <QDebug>
#include <QFile>

class GraphicsConfigPrivate
{
public:
    QStringList transparentBackgroundModuleList;
};

GraphicsConfig::GraphicsConfig()
{
    dataPtr.reset(new GraphicsConfigPrivate());
    parseConfig();
}

GraphicsConfig::~GraphicsConfig() { }

void GraphicsConfig::parseConfig()
{
    QFile file(":/config/graphicsConfig");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray contents = file.readAll();
    // qDebug() << contents.data();

    nlohmann::json j = nlohmann::json::parse(contents.data());
    if (!j.is_null()) {
        auto transparentBackGroundModuleNode = j["TransparentBackgroundModule"];
        if (transparentBackGroundModuleNode.is_array()) {
            for (int i = 0; i < transparentBackGroundModuleNode.size(); i++) {
                std::string moduleName = transparentBackGroundModuleNode[i];
                dataPtr->transparentBackgroundModuleList.append(moduleName.c_str());
            }
        }
    }
}

GraphicsConfig &GraphicsConfig::getInstance()
{
    static GraphicsConfig config;
    return config;
}

QStringList GraphicsConfig::getTransparentBackgroundModules()
{
    return dataPtr->transparentBackgroundModuleList;
}

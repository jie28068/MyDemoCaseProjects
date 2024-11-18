#include "BuriedData.h"

#include <QDir>
#include <QStandardPaths>
#include <fstream>

BuriedData &BuriedData::getInstance()
{
    static BuriedData instance;
    return instance;
}

void BuriedData::increaseSourceUsage(std::string &type, std::string &sourcePrototype)
{
    try {
        if (type == "Control") {
            auto &node = root["UsageStatistics"]["Control"][sourcePrototype];
            if (node.is_number()) {
                int counts = node.get<int>();
                node = ++counts;
            } else {
                node = 1;
            }
        } else if (type == "Electrical") {
            auto &node = root["UsageStatistics"]["Electrical"][sourcePrototype];
            if (node.is_number()) {
                int counts = node.get<int>();
                node = ++counts;
            } else {
                node = 1;
            }
        }

    } catch (...) {
    }
}

int BuriedData::getSourceUsageCount(std::string &type, std::string &sourcePrototype)
{
    try {
        if (type == "Control") {
            auto &node = root["UsageStatistics"]["Control"][sourcePrototype];
            if (node.is_number()) {
                int counts = node.get<int>();
                return counts;
            } else {
                return 0;
            }
        } else if (type == "Electrical") {
            auto &node = root["UsageStatistics"]["Electrical"][sourcePrototype];
            if (node.is_number()) {
                int counts = node.get<int>();
                return counts;
            } else {
                return 0;
            }
        }

    } catch (...) {
        return 0;
    }
    return 0;
}

void BuriedData::save()
{
    std::ofstream o(buriedDataFilePath.toStdString());
    o << std::setw(4) << root << std::endl;
}

BuriedData::BuriedData()
{
    QString fileName = "graphics_buried_data.json";
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    buriedDataFilePath = filePath + QDir::separator() + fileName;

    try {
        if (QFile::exists(buriedDataFilePath)) {
            std::ifstream f(buriedDataFilePath.toStdString());
            root = json::parse(f);
        }

    } catch (...) {
        root = json();
    }
}

BuriedData::~BuriedData() { }

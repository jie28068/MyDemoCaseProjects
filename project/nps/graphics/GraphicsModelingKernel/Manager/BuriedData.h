#ifndef GRAPHICS_BURIED_DATA_H
#define GRAPHICS_BURIED_DATA_H

#include <QString>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class BuriedData
{
public:
    static BuriedData &getInstance();

public:
    void increaseSourceUsage(std::string &type, std::string &sourcePrototype);

    int getSourceUsageCount(std::string &type, std::string &sourcePrototype);

    void save();

private:
    BuriedData();
    ~BuriedData();

private:
    json root;
    QString buriedDataFilePath;
};

#endif
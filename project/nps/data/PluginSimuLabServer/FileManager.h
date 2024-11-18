#pragma once

#include "GenerateMapSchema.h"
#include "KLModelDefinitionCore/BlockDefinition.h"
#include "ToolsLib/Json/json.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <string>

using namespace Kcc::BlockDefinition;
using namespace std;

class FileManager
{
public:
    FileManager() {};
    virtual ~FileManager() {};

    /// @brief 写文件
    /// @param filePath 路径
    /// @param fileContent 内容
    /// @return
    static bool writeToFile(const QString &filePath, const QString &fileContent);
    /// @brief 写map文件
    /// @param filePath 路径
    /// @param mapSchema map信息
    /// @return
    static bool writeToMap(const QString &filePath, const SimuLabSchema::MapSchema &mapSchema);
    /// @brief 删除文件
    /// @param dirName 文件路径
    /// @return
    static bool deleteDirfile(const QString &dirName);
    /// @brief 从srcPath复制目录/文件到dstPath
    /// @param srcPath 源路径
    /// @param dstPath 目标路径
    static void copyDir(const QString &srcPath, const QString &dstPath);
};

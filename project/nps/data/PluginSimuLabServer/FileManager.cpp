#include "FileManager.h"
#include "CoreLib/ServerManager.h"
#include <QDataStream>
#include <QDir>
#include <QTextCodec>

USE_LOGOUT_("SimuLabServer")

bool FileManager::writeToFile(const QString &filePath, const QString &fileContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOGOUT(QObject::tr("打开文件[%1]失败! 原因：%2").arg(filePath).arg(file.errorString()), LOG_ERROR);
        return false;
    }

    QTextStream text(&file);
    text << fileContent;
    file.close();
    return true;
}

bool FileManager::writeToMap(const QString &filePath, const SimuLabSchema::MapSchema &mapSchema)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        LOGOUT(QObject::tr("打开文件[%1]失败! 原因：%2").arg(filePath).arg(file.errorString()), LOG_ERROR);
        return false;
    }

    QTextStream text(&file);
    // text.setCodec(QTextCodec::codecForName("GBK"));
    text << "[Parameter]\n";
    size_t paramidx = 0;
    for each (SimuLabSchema::ParameterUnit var in mapSchema.parameterList) {
        text << QString("%1=%2|%3|%4|%5|%6|%7|%8|%9|%10|\n")
                        .arg(paramidx)
                        .arg(var.blkName)
                        .arg(var.paramName)
                        .arg(var.varName)
                        .arg(var.typeStr)
                        .arg(var.valueType)
                        .arg(var.nRows)
                        .arg(var.nCols)
                        .arg(var.valuesStr)
                        .arg(var.blockType);

        paramidx++;
    }

    text << "\n[Signal]\n";
    for each (SimuLabSchema::SignalUnit var in mapSchema.signalList) {
        text << QString("%1=%2|%3|%4|%5|%6|%7|%8|%9|\n")
                        .arg(var.boIdx)
                        .arg(var.blkName)
                        .arg(var.sigName)
                        .arg(var.boIdx)
                        .arg(var.sigWidth)
                        .arg(var.row)
                        .arg(var.col)
                        .arg(var.typeStr)
                        .arg(var.blockType);
    }

    file.close();
    return true;
}

bool FileManager::deleteDirfile(const QString &dirName)
{
    QDir directory(dirName);
    if (!directory.exists()) {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i) {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink()) {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath)) {
                error = true;
            }
        } else if (fileInfo.isDir()) {
            if (!deleteDirfile(fileInfo.absoluteFilePath())) {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path()))) {
        error = true;
    }
    return !error;
}

void FileManager::copyDir(const QString &srcPath, const QString &dstPath)
{
    QDir srcDir(srcPath);
    QDir dstDir(dstPath);
    if (!dstDir.exists()) {
        dstDir.mkdir(dstPath);
    }
    QFileInfoList list = srcDir.entryInfoList();
    foreach (QFileInfo info, list) {
        if (info.fileName() == "." || info.fileName() == "..") {
            continue;
        }
        if (info.isDir()) {
            copyDir(info.filePath(), dstPath + "/" + info.fileName());
            continue;
        }

        QFile file(info.filePath());
        file.copy(dstPath + "/" + info.fileName());
    }
}

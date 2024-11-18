#ifndef PATHTOOL_H
#define PATHTOOL_H
#include <QStandardPaths>
#include <QDir>
#include <QString>

/**
 * @brief The PathTool class
 * 用于获取各种目录路径
 */
class PathTool{
public:
    PathTool(){};

    //客户端缓存路径
    static QString clientCache(){
        QString path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                        + QString("/ManageTool/.cache");
        createDir(path);
        return path;
    }

    //根据签名查找文件
    static QString findArchiveFile(QString sign, QString path=PathTool::clientCache()){
        QDir dir(path);

        QStringList filters;
        filters<<QString("%1.*").arg(sign);
        QFileInfoList file_list = dir.entryInfoList(filters);
        if(file_list.isEmpty())
            return QString();
        else
            return file_list.first().absoluteFilePath();
    }

    //数据库路径
    static QString databasePath(){
        QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + QString("/ManageTool/database");
        createDir(path);
        return path;
    };
    //缓存路径
    static QString cachePath(){
        QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + QString("/ManageTool/.cache");
        createDir(path);
        return path;
    }
    //档案路径
    static QString archivePath(QString archiveName=QString()){
        QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + QString("/ManageTool/archive");
        createDir(path);
        if(archiveName.isEmpty())
            return path;
        else
            return path+"/"+archiveName;
    }
    //软件运行日志路径
    static QString logPath(QString fileName=QString()){
        QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + QString("/ManageTool/log");
        createDir(path);
        if(fileName.isEmpty())
            return path;
        else
            return path+"/"+fileName;
    }
    static void createDir(QString path){
        QDir dir;
        dir.mkpath(path);
    }

};


#endif // PATHTOOL_H

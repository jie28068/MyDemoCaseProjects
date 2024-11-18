#ifndef PROJDATAMANAGER_H
#define PROJDATAMANAGER_H

#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "KLProject/BuiltinModelManager.h"
#include "KLProject/KLProject.h"
#include <QDir>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QVariantMap>
namespace Kcc {
namespace ProjectManager {
class IProjectManagerServer;
}
}

namespace Kcc {
namespace BlockDefinition {

class Model;
}
}
namespace PROJDATAMNGKEY {
#ifdef COMPILER_PRODUCT_DESIGNER
static const QString SIMUNPS_WORKSPACE_PATH = (QDir::homePath() + "/AppData/Local/SimuLab.Designer/workspace1.0");
#else
static const QString SIMUNPS_WORKSPACE_PATH = (QDir::homePath() + "/AppData/Local/SimuNPS/workspace1.0");
#endif

static const QString REG_WORKSPACENAME = "^[)(a-zA-Z0-9\u4E00-\u9FA5_-]{1,32}$";
static const QString KEY_PROJECT_ACTIVESTAUS = "KEY_PROJECT_ACTIVESTAUS";
static const QString ZIP_WORKSPACE_SUFFIX = ".npsdw";
static const QString DESINGERWS = "WorkSpace";

// 工作空间信息
static const QString KEY_ALLWORKSPACE = "All workspace";
static const QString KEY_WORKSPACEPATH = "Workspace path";
// 数据管理器tree展开状态
static const QString KEY_TREETYPE_DATAMNG = "KEY_TREETYPE_DATAMNG";
// 数据管理器项目排序状态
static const QString KEY_PROJ_SORTTYPE = "KEY_PROJ_SORTTYPE";
// 网络模型管理器tree展开状态
static const QString KEY_TREETYPE_NETWORKMNG = "KEY_TREETYPE_NETWORKMNG";
// 项目信息
static const QString KEY_ALLPROJECT = "All project";
static const QString KEY_PROJECTPATH = "Project path";
static const QString KEY_ENGLISHNAME = "English Name";
static const QString NPSFILEBOOT = "npsboot";
static const QString KEY_PROJECTICON = "Project icon";
static const QString KEY_CLASSENGLISHNAME = "Class English Name";

static const QString BOOS_FILE_PATH = SIMUNPS_WORKSPACE_PATH + "/." + NPSFILEBOOT;
}

class FileData
{
public:
    virtual ~FileData() { }

    // virtual QString suffix() = 0; //后缀名不用加.
    virtual QString serialized() = 0;
    virtual void deserialized(const QString &content) = 0;
};

// 所有工作空间和当前当作工作空间存储
class File_npsboot : public FileData
{
public:
    static QString suffix() { return PROJDATAMNGKEY::NPSFILEBOOT; }

    QString serialized() override;
    void deserialized(const QString &content) override;

    inline QStringList getWorkspaceList() const { return m_allWorkspace; }
    inline void addWorkspace(const QString &workspacePath)
    {
        if (!m_allWorkspace.contains(workspacePath)) {
            m_allWorkspace.append(workspacePath);
        }
    }
    inline void removeWorkSpace(const QString &workspacePath) { m_allWorkspace.removeOne(workspacePath); }
    inline void setTreeState(const QMap<QString, QVariant> &treeSts, const QString &treetype)
    {
        if (PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG == treetype) {
            m_DataMngTreeSts = treeSts;
        } else if (PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG == treetype) {
            m_NetWorkMngTreeSts = treeSts;
        }
    }
    inline QVariantMap getTreeState(const QString &treetype)
    {
        if (PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG == treetype) {
            return m_DataMngTreeSts;
        } else if (PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG == treetype) {
            return m_NetWorkMngTreeSts;
        } else {
            return QVariantMap();
        }
    }
    inline void setSortType(const int &sorttype) { m_sortProjectType = sorttype; }
    inline const int getSortType() { return m_sortProjectType; }

public:
    QStringList m_allWorkspace;                  // 所有的工作空间文件（相对路径)
    QMap<QString, QVariant> m_DataMngTreeSts;    // 数据管理器tree展开状态
    QMap<QString, QVariant> m_NetWorkMngTreeSts; // 网络模型管理器tree展开状态
    int m_sortProjectType;                       // 排序项目类型
};

// 工作空间文件.npsws//当前工作空间下的所有项目
class File_npsws : public FileData
{
public:
    static QString suffix() { return "npsws"; }

    QString serialized() override;
    void deserialized(const QString &content) override;

    inline QStringList getAllProject() const { return m_allProject; }
    inline void addProject(const QString &projectPath)
    {
        if (!m_allProject.contains(projectPath)) {
            m_allProject.append(projectPath);
        }
    }
    inline void removeProject(const QString &projectPath) { m_allProject.removeOne(projectPath); }

public:
    QStringList m_allProject; // 该工作空间所有的项目（相对路径)
};

struct WorkSpaceInfo {
    WorkSpaceInfo(const QString _name = "") : name(_name) { }
    inline bool isValid() { return !name.isEmpty(); }
    QString getWorkspaceAbsolutePath()
    {
        return PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + name + "/" + name + "." + File_npsws::suffix();
    }
    QString getProjectAbsolutePath(const QString &projname)
    {
        if (allprojects.contains(projname) && allprojects[projname] != nullptr) {
            return allprojects[projname]->getProjectPath();
        }
        return QString();
    }
    QString name; // 工作空间名
    QString wsabsPath;
    QMap<QString, QSharedPointer<KLProject>> allprojects; // key是项目名，value是项目指针;
};
class ProjDataManager : public QObject
{
    Q_OBJECT
public:
    ProjDataManager(QObject *parent = 0);
    ~ProjDataManager();

    void setValue(const QString &keyword, const QVariant &value);
    QVariant getValue(const QString &keyword);
    /// @brief 创建工作空间
    /// @param name 工作空间名
    /// @return 成功返回true，否则返回false
    bool createWorkspace(const QString &name);
    /// @brief 删除工作空间
    /// @param workspaceName 工作空间名
    /// @return 成功返回true，否则返回false
    bool deleteWorkspace(const QString &workspaceName);
    /// @brief 导入工作空间
    /// @param importfilepath 导入的文件完整路径
    /// @param codec 编码
    /// @return 导入成功返回true，否则返回false
    WorkSpaceInfo importWorkSpace(const QString &importfilepath, const QString codec = "system");
    /// @brief 导入工作空间
    /// @param importfilepath 导入的文件完整路径
    /// @return 导入成功返回true，否则返回false
    bool exportWorkSpace(const QString &exportwsName, const QString targetDir);
    /// @brief 重命名工作空间
    /// @param oldName 旧的工作空间名
    /// @param newName 新的工作空间名
    /// @return 成功返回true，否则返回false
    bool renameWorkspace(const QString &oldName, const QString &newName);
    /// @brief 新建项目
    /// @param workspacename 工作目录
    /// @param projname 项目名
    /// @return 成功返回指针，否则返回nullptr
    QSharedPointer<KLProject> createProject(const QString &workspacename, const QString &projname);
    /// @brief 删除项目
    /// @param workspacename 项目所属工作空间
    /// @param projname 项目名
    /// @return 成功返回true，否则返回false
    bool deleteProject(const QString &workspacename, const QString &projname);
    /// @brief 重命名项目
    /// @param workspacename 工作空间名
    /// @param projnameold 项目旧名称
    /// @param projnamenew 项目新名称
    /// @return 成功返回true，否则返回false
    bool renameProject(const QString &workspacename, const QString &projnameold, const QString &projnamenew);
    /// @brief 导出项目
    /// @param pProject 待导出的项目
    /// @param targetdir 导出的目标路径
    /// @return 导出成功返回true，否则返回false
    bool exportProject(QSharedPointer<KLProject> pProject, const QString &targetdir);
    /// @brief 导入项目
    /// @param importWSName 导入的目标工作空间
    /// @param srcfilepath 待导入的项目文件绝对路径
    /// @return 导入成功返回项目指针，否则返回nullptr
    QSharedPointer<KLProject> importProject(const QString &importWSName, const QString &srcfilepath);
    /// @brief 设置项目激活状态
    /// @param workspacename 工作空间名
    /// @param projname 项目名
    /// @param bactive ture是激活状态，false是非激活状态
    void setProjectActiveStatus(const QString &workspacename, const QString &projname, bool bactive);
    /// @brief 获取项目
    /// @param workspacename 项目所属工作空间
    /// @param projname 项目名
    /// @return
    QSharedPointer<KLProject> getProject(const QString &workspacename, const QString &projname);
    /// @brief 获取所有工作空间及项目信息
    /// @return
    QMap<QString, WorkSpaceInfo> getAllWorkSpace();
    // /// @brief 导入示例项目
    // /// @param projectName 导入的工作空间名
    // /// @return
    // bool improtDemoProject(const QString &WorkspaceName);
    QStringList getAllDemoProjects();
    /// @brief
    /// @return 分类与分类下的示例项目
    QMap<QString, QStringList> getCateDemoProjects();
    /// @brief 导入示例项目到指定工作空间
    /// @param sourceProjectPath 拷贝的示例项目相对路径
    /// @param targetWorkspaceName 拷贝的目标工作空间
    /// @param targetProjectName 拷贝后的项目名
    /// @return
    QSharedPointer<KLProject> copyDemoProject(const QString &sourceProjectPath, const QString &targetWorkspaceName,
                                              const QString &targetProjectName);
    /// @brief 拷贝项目
    /// @param srcProject 待拷贝的项目
    /// @param targetWSName 拷贝到的工作空间名
    /// @return
    QSharedPointer<KLProject> copyProject(QSharedPointer<KLProject> srcProject, const QString &targetWSName);
    /// @brief 移动项目
    /// @param srcProject 待移动的项目
    /// @param targetWSName 移动到的工作空间名
    /// @return
    QSharedPointer<KLProject> moveProject(QSharedPointer<KLProject> srcProject, const QString &targetWSName);
    /// @brief 获取当前项目
    /// @return
    QSharedPointer<KLProject> getCurrentProject();
    /// @brief 获取当前项目工作空间
    /// @return
    const WorkSpaceInfo getCurrentWorkSpace();
    /// @brief 获取系统项目，比如全局库里面的设备类型
    /// @return
    QSharedPointer<KLProject> getSystemProject();
    /// @brief 数据升级之后刷新画面
    ///  @param projectPath 如果是外部模式，传递外部项目路径
    void reloadData(const QString &projectPath = "");
    /// @brief 获取tree初始展开状态
    /// @param treeType 树分类
    /// @return
    QMap<QString, QVariant> getTreeStateMap(const QString &treeType);
    /// @brief 保存tree展开状态
    /// @param treeType 树分类
    /// @param stsMap 展开状态map
    /// @return
    bool saveTreeStateMap(const QString &treeType, const QVariantMap &stsMap);
    /// @brief 设置项目排序模式
    /// @param sorttype 排序模式
    void setSortType(const int &sorttype);
    /// @brief 获取项目排序模式
    /// @return
    const int getSortType();
    /// @brief 项目启动模式
    /// @return
    KLProject::ProjectMode projectMode();
    /// @brief 编辑外部项目
    /// @param projectAbsPath 项目绝对路径
    void editExternalProject(const QString &projectAbsPath);

private:
    /// @brief 获取根文件信息
    /// @return
    File_npsboot *getBootFile();
    /// @brief 获取工作空间
    /// @param wsname 工作空间名
    /// @param wsFilePath 工作空间文件完整路径
    /// @return
    File_npsws *getWorkSpace(const QString &wsname, QString &wsFilePath);
    /// @brief 获取相对路径
    /// @param dirPath 要去除的前面部分的路径
    /// @param filePath 要转换的目标绝对路径
    /// @return 相对路径
    QString getRelativeFilePath(QString dirPath, QString filePath);
    /// @brief 写入文件
    /// @param filePath 写入的文件绝对路径
    /// @param fileContent 写入的内容
    /// @return 写入成功返回true，否则返回false
    bool writeToFile(const QString filePath, const QString fileContent);
    /// @brief 读取文件内容
    /// @param filePath 读取文件绝对路径
    /// @param fileContent 读出内容
    /// @return 读出成功返回true，否则返回false
    bool readFromFile(const QString filePath, QString &fileContent);
    /// @brief 保存文件数据
    /// @param filePath 文件绝对路径
    /// @param data 数据
    /// @return 保存成功返回true，否则返回false
    bool saveFileData(const QString filePath, FileData *data);
    /// @brief 读取文件数据
    /// @param filePath 文件绝对路径
    /// @param data 数据
    /// @return 读取成功返回true，否则返回false
    bool loadFileData(const QString filePath, FileData *data);
    /// @brief 检查文件名是否符合规范
    /// @param name 待检测文件名
    /// @return 可以命名返回true，否则返回false
    bool checkFileName(QString name);
    /// @brief 检查工作空间名是否重名。
    /// @param wsName 工作空间名
    /// @return 不重名返回true，否则返回false
    bool checkWSNameValid(const QString &wsName);
    /// @brief 检查项目名是否重名
    /// @param wsName 工作空间名
    /// @param projName 项目名
    /// @return 不重名返回true，否则返回false
    bool checkProjectNameValid(const QString &wsName, const QString &projName);
    /// @brief 注册文件后缀
    /// @param suffix 文件后缀
    inline void registerSuffix(const QString suffix) { m_allFiles[suffix]; }
    /// @brief 文件后缀是否注册了
    /// @param filePath 文件绝对路径
    /// @return 注册了返回true，否则返回false
    bool isRegisteredSuffix(const QString filePath);
    /// @brief 删除工作空间下的所有目录和文件
    /// @param path 工作空间的绝对路径
    /// @return 删除成功返回true，否则返回false
    bool deleteDirectory(const QString &path);
    /// @brief 初始化根文件
    void initBootFile();
    /// @brief 初始化
    void initWorkspace();
    /// @brief 加载所有工程
    /// @param projectPath 外部模式项目文件绝对路径
    void loadAllProjects(const QString &projectPath = "");
    // 读取buildin/ExampleNew中的workspace中项目的名称
    void loadDemoProjects();
    /// @brief 获取工作空间文件绝对路径
    /// @param projFileAbsolutePath 项目文件绝对路径
    /// @return 工作空间文件绝对路径
    QString getWorkSpaceFilePath(const QString &projFileAbsolutePath);
    /// @brief 更新根节点下的工作空间配置
    /// @param workspacePath 工作空间路径
    /// @param bAdd true为追加，否则为删除
    /// @return
    bool updateBootFileWSCfg(const QString &workspacePath, bool bAdd);
    /// @brief 更新工作空间下的项目配置
    /// @param projFileAbsolutePath 项目文件绝对路径
    /// @param bAdd true为追加，false为删除
    /// @return
    bool updateWSProjectCfg(const QString &projFileAbsolutePath, bool bAdd);
    /// @brief 拷贝文件夹
    /// @param srcPath 源路径
    /// @param dstPath 目标路径
    /// @param coverFileIfExist
    /// @param topLevel 顶层文件夹
    /// @return
    bool copyDirectory(const QString &srcPath, const QString &dstPath, bool coverFileIfExist, bool topLevel);
    /// @brief 检测工作空间文件是否为旧模型数据
    /// @param projectList 工作空间中项目路径列表
    /// @return
    bool checkeWorkSpaceFileIsOld(const QString &workspaceDir, QStringList projectList);
    /// @brief 获取外部项目工作空间名
    /// @param projectPath 项目路径
    /// @return
    QString getExternalWorkSpace(const QString &projectPath);

private:
    QMap<QString, WorkSpaceInfo> m_allWorkSpaceMap;
    // 第一个key是文件后缀名，第二个key是完整路径+文件名
    QMap<QString, QMap<QString, FileData *>> m_allFiles;
    // 保存程序内建的所有demo项目的名称
    QStringList m_demoProjects;
    QMap<QString, QStringList> m_demoPrjMap;
};

typedef QSharedPointer<ProjDataManager> PProjDataManager;

#endif
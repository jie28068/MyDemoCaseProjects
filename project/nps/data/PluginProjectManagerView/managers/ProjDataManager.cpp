#include "ProjDataManager.h"

#include "AssistantDefine.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "JlCompress.h"
#include "KLWidgets/KMessageBox.h"
#include "ProjectViewServers.h"
#include <QApplication>
#include <QPHelper.h>

USE_LOGOUT_("ProjDataManager")

// 升级后示例项目demo的保存路径
#define BUILDI_DEMO_PATH (QFileInfo(QCoreApplication::applicationDirPath()).canonicalFilePath() + "/Example")

ProjDataManager::ProjDataManager(QObject *parent) : QObject(parent)
{
    registerSuffix(File_npsboot::suffix());
    registerSuffix(File_npsws::suffix());

    reloadData();
}

ProjDataManager::~ProjDataManager() { }

void ProjDataManager::setValue(const QString &keyword, const QVariant &value)
{
    gConfSet(keyword, value);
}

QVariant ProjDataManager::getValue(const QString &keyword)
{
    return gConfGet(keyword);
}

bool ProjDataManager::createWorkspace(const QString &name)
{
    if (!checkFileName(name)) {
        return false;
    }

    // 检查是否重名
    if (!checkWSNameValid(name)) {
        LOGOUT(QObject::tr("Workspace [%1] is Already Exist,Please Rename!").arg(name), LOG_WARNING);
        return false;
    }

    QString workspaceDir = PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + name;
    if (!QDir().mkpath(workspaceDir)) { // 创建工作空间文件夹"无法创建目录\"" + workspaceDir + "\"！"
        LOGOUT(QObject::tr("Unable to create directory [%1] !").arg(workspaceDir), LOG_ERROR);
        return false;
    }
    QString workspacePath = workspaceDir + "/" + name + "." + File_npsws::suffix();
    updateBootFileWSCfg(workspacePath, true);
    return true;
}

bool ProjDataManager::deleteWorkspace(const QString &workspaceName)
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return false;
    }
    QString workspaceFilePath;
    File_npsws *wsFileData = getWorkSpace(workspaceName, workspaceFilePath);
    if (workspaceFilePath.isEmpty()) {
        return false;
    }
    for (PKLProject project : m_allWorkSpaceMap[workspaceName].allprojects.values()) {
        if (project == nullptr) {
            continue;
        }
        ProjectViewServers::getInstance().m_projectManagerServer->DestoryProject(project);
    }
    updateBootFileWSCfg(workspaceFilePath, false);
    deleteDirectory(QFileInfo(PROJDATAMNGKEY::BOOS_FILE_PATH).dir().absolutePath() + "/" + workspaceName);
    return true;
}

WorkSpaceInfo ProjDataManager::importWorkSpace(const QString &importfilepath, const QString codec)
{
    if (importfilepath.isEmpty() || ProjectViewServers::getInstance().m_projectManagerServer == nullptr)
        return WorkSpaceInfo();

    QString workspaceName = QFileInfo(importfilepath).completeBaseName();
    if (!checkFileName(workspaceName)) {
        return WorkSpaceInfo();
    }

    QStringList bootHasPath;
    for (QString wsPath : m_allFiles[File_npsws::suffix()].keys())
        bootHasPath << QFileInfo(wsPath).baseName();

    // 解压
    QString importwsname = QFileInfo(importfilepath).baseName();
    QFile tmpFile(importfilepath);
    QString newName = QFileInfo(importfilepath).dir().absolutePath() + "/" + importwsname + ".zip";
    QString targetDirPath = PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + importwsname;
    // 如果存在，重命名
    if (QDir(targetDirPath).exists()) {
        // 得到当前文件夹下所有子文件夹的名称
        QStringList allDirs;
        QFileInfoList &allFileList = QDir(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH).entryInfoList();
        for (QFileInfo file : allFileList) {
            QString &tmpUse = QFileInfo(file).baseName();
            if (tmpUse.isEmpty() || !file.isDir() || tmpUse == "." || tmpUse == "..")
                continue;
            allDirs << tmpUse;
        }

        int i = 0;
        QString tmpName = importwsname;
        while (allDirs.contains(tmpName, Qt::CaseInsensitive)) {
            tmpName = importwsname + QString("(%1)").arg(++i);
        }
        targetDirPath = PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + tmpName;
    }
    tmpFile.rename(newName);
    QStringList &unpackedFileNames = JlCompress::extractDir(newName, targetDirPath);
    tmpFile.rename(importfilepath);
    if (unpackedFileNames.empty()) {
        //"工作空间文件为空或者无法读取，工作空间文件导入失败"
        LOGOUT(QObject::tr("The workspace file is empty or unreadable, and importing the workspace file failed!"),
               LOG_ERROR);
        deleteDirectory(targetDirPath);
        return WorkSpaceInfo();
    }

    QString wsFilePath;
    for (QString tmpFileName : unpackedFileNames) {
        if (QFileInfo(tmpFileName).suffix() == File_npsws::suffix()) {
            // 如果npsws文件的名称在boot文件中已经有了，则重命名
            QString &tmpStr = QFileInfo(tmpFileName).baseName();
            if (bootHasPath.contains(tmpStr, Qt::CaseInsensitive)) {
                QString tmpBase;
                int i = 0;
                do {
                    ++i;
                    tmpBase = tmpStr + QString("(%1)").arg(i);
                } while (bootHasPath.contains(tmpBase, Qt::CaseInsensitive));

                wsFilePath = QFileInfo(tmpFileName).dir().absolutePath() + "/" + tmpBase + "." + File_npsws::suffix();
                QFile(tmpFileName).rename(wsFilePath);
                break;
            } else {
                wsFilePath = tmpFileName;
                break;
            }
        }
    }

    if (wsFilePath.isEmpty()) {
        //"工作空间文件导入失败"
        LOGOUT(QObject::tr("Import workspace file failed!"), LOG_ERROR);
        deleteDirectory(targetDirPath);
        return WorkSpaceInfo();
    }

    // 更改父文件夹的名称
    QString newParentDir = QFileInfo(targetDirPath).dir().absolutePath() + "/" + QFileInfo(wsFilePath).baseName();
    if (QFile(targetDirPath).rename(newParentDir)) {
        targetDirPath = newParentDir;
    }
    wsFilePath = newParentDir + "/" + QFileInfo(wsFilePath).baseName() + "." + File_npsws::suffix();

    // 判断工作空间名称是否已经存在
    File_npsboot *bootFile = getBootFile();
    if (!bootFile) {
        if (targetDirPath != PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH) {
            deleteDirectory(targetDirPath);
        }
        return WorkSpaceInfo();
    }

    // 加载项目文件
    QString &workspaceDir = targetDirPath;
    auto *workspace = new File_npsws;
    if (!loadFileData(wsFilePath, workspace)) {
        if (workspaceDir != PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH) {
            deleteDirectory(workspaceDir);
        }
        return WorkSpaceInfo();
    }

    // 检测导入的工作空间是否为旧数据工作空间
    if (checkeWorkSpaceFileIsOld(workspaceDir, workspace->m_allProject)) {
        if (workspaceDir != PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH) {
            delete workspace;
            deleteDirectory(workspaceDir);
            m_allFiles[QFileInfo(wsFilePath).suffix()].remove(wsFilePath);
            LOGOUT(tr("The workspace file has expired and cannot be imported"),
                   LOG_WARNING); // 工作空间文件已过期,无法导入
            return WorkSpaceInfo();
        }
    }

    bool isSuccess = true; // 任意一个有问题则是false
    WorkSpaceInfo wsinfo(QFileInfo(wsFilePath).baseName());
    for (QString relativePath : workspace->m_allProject) {
        QString projectPath = workspaceDir + "/" + relativePath;
        PKLProject pProject = ProjectViewServers::getInstance().m_projectManagerServer->LoadProject(projectPath);
        if (pProject == nullptr) {
            isSuccess = false;
        } else {
            wsinfo.allprojects.insert(QFileInfo(relativePath).baseName(), pProject);
        }
    }

    if (!isSuccess) {
        if (workspaceDir != PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH) {
            delete workspace;
            deleteDirectory(workspaceDir);
            m_allFiles[QFileInfo(wsFilePath).suffix()].remove(wsFilePath);
        }
        return WorkSpaceInfo();
    }

    // 加入boot文件
    QString workspaceRelativePath = getRelativeFilePath(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH, wsFilePath);
    bootFile->addWorkspace(workspaceRelativePath);
    saveFileData(PROJDATAMNGKEY::BOOS_FILE_PATH, bootFile);
    m_allWorkSpaceMap.insert(wsinfo.name, wsinfo);
    // 导入工作空间完成

    LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Import, AssistantDefine::LOG_TYPE_WORKSPACE,
                                        wsinfo.name, true),
           LOG_NORMAL);
    return wsinfo;
}

bool ProjDataManager::exportWorkSpace(const QString &exportwsName, const QString targetDir)
{
    QString workspaceFilePath = "";

    File_npsboot *bootFile = getBootFile();
    for (QString path : bootFile->m_allWorkspace) {
        QString workspaceName = QFileInfo(path).baseName();
        if (exportwsName.compare(workspaceName, Qt::CaseInsensitive) == 0) {
            workspaceFilePath = PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + path;
            break;
        }
    }

    if (workspaceFilePath.isEmpty()) { // 创建工作空间文件夹
                                       // 未找到工作空间[]文件,导出失败！
        LOGOUT(QObject::tr("Unable to find workspace [%1] file, export failed!"), LOG_ERROR);
        return false;
    }

    QString srcPath = QFileInfo(workspaceFilePath).dir().absolutePath();
    QString dstFileName =
            targetDir + "\\" + QFileInfo(workspaceFilePath).baseName() + PROJDATAMNGKEY::ZIP_WORKSPACE_SUFFIX;
    if (QFile::exists(dstFileName)) {
        //"目标文件已存在，是否确认替换"
        if (KMessageBox::question(AssistantDefine::getMainWindow(),
                                  QObject::tr("The target file already exists. Are you sure to replace it!"),
                                  KMessageBox::Yes | KMessageBox::No)
            != KMessageBox::Yes) {
            return false;
        }
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // 在这里添加压缩
    bool flg = JlCompress::compressDir(dstFileName, srcPath, true);
    QApplication::restoreOverrideCursor();

    if (flg) {
        //"导出工作空间\"" + exportwsName + "\"完成"
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Export, AssistantDefine::LOG_TYPE_WORKSPACE,
                                            exportwsName, true),
               LOG_NORMAL);
        return true;
    } else {
        // 导出工作空间\"" + exportwsName + "\"失败"
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Export, AssistantDefine::LOG_TYPE_WORKSPACE,
                                            exportwsName, false),
               LOG_WARNING);
        return false;
    }
}

bool ProjDataManager::renameWorkspace(const QString &oldName, const QString &newName)
{
    if (oldName.isEmpty() || newName.isEmpty() || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return false;
    }

    if (oldName == newName) {
        return false;
    }

    if (!checkFileName(newName)) {
        return false;
    }
    // check name repeat
    if (!checkWSNameValid(newName)) {
        LOGOUT(QObject::tr("Workspace [%1] is Already Exist,Please Rename!").arg(newName), LOG_WARNING);
        return false;
    }

    // 查找旧wsfile
    QString workspaceOldPath;
    File_npsws *workspaceFile = getWorkSpace(oldName, workspaceOldPath);
    if (workspaceFile == nullptr) {
        LOGOUT(QObject::tr("Not Found Workspace [%1] !").arg(oldName), LOG_ERROR);
        return false;
    }
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr) {
        return false;
    }

    // 修改文件夹的名字
    QString newDirPath = QFileInfo(workspaceOldPath).absolutePath() + "/../" + newName;
    newDirPath = QFileInfo(newDirPath).absoluteFilePath(); // 去掉路径中的..
    if (!QDir().rename(QFileInfo(workspaceOldPath).absolutePath(), newDirPath)) {
        return false;
    }

    // 修改文件名
    QFile file(newDirPath + "/" + oldName + "." + File_npsws::suffix());
    QString newFilePath = newDirPath + "/" + newName + "." + File_npsws::suffix();
    if (!file.rename(newFilePath)) {
        QDir().rename(newDirPath, QFileInfo(workspaceOldPath).absolutePath()); // 还原文件夹的名字
        return false;
    }

    m_allFiles[File_npsws::suffix()][newFilePath] = workspaceFile;
    m_allFiles[File_npsws::suffix()].remove(workspaceOldPath);

    // 将修改同步到bootFile
    QString workspaceRelativePath = getRelativeFilePath(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH, newFilePath);
    for (int i = 0; i < bootFile->m_allWorkspace.size(); ++i) {
        if (QFileInfo(bootFile->m_allWorkspace[i]).baseName() == oldName) {
            bootFile->m_allWorkspace[i] = workspaceRelativePath;
            break;
        }
    }
    saveFileData(PROJDATAMNGKEY::BOOS_FILE_PATH, bootFile);
    WorkSpaceInfo tmpinfo = m_allWorkSpaceMap[oldName];
    tmpinfo.name = newName;
    m_allWorkSpaceMap.remove(oldName);
    m_allWorkSpaceMap.insert(tmpinfo.name, tmpinfo);
    ProjectViewServers::getInstance().m_projectManagerServer->UpdataProjectPrefix(
            QFileInfo(workspaceOldPath).absolutePath(), QFileInfo(newFilePath).absolutePath());
    return true;
}
QSharedPointer<KLProject> ProjDataManager::createProject(const QString &workspacename, const QString &projname)
{
    if (workspacename.isEmpty() || projname.isEmpty()
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return nullptr;
    }
    QString projppath = "";
    if (!checkProjectNameValid(workspacename, projname)) {
        LOGOUT(QObject::tr("Project [%1] is Already Exist,Please Rename!").arg(projname), LOG_WARNING);
        return nullptr;
    }
    QSharedPointer<KLProject> pProject = ProjectViewServers::getInstance().m_projectManagerServer->CreatProject(
            PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + workspacename, projname, projppath);
    if (pProject != nullptr && updateWSProjectCfg(projppath, true)) {
        return pProject;
    }
    return nullptr;
}

bool ProjDataManager::deleteProject(const QString &workspacename, const QString &projname)
{
    if (workspacename.isEmpty() || projname.isEmpty()
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        //"项目信息为空或项目服务为空，无法删除项目！"
        LOGOUT(QObject::tr("The project information is empty or the project service is empty, unable to delete the "
                           "project!"),
               LOG_ERROR);
        return false;
    }
    PKLProject project = getProject(workspacename, projname);
    QString projpath = project->getProjectPath();
    if (ProjectViewServers::getInstance().m_projectManagerServer->DestoryProject(project)
        && updateWSProjectCfg(projpath, false)) {
        return true;
    }
    return false;
}

bool ProjDataManager::renameProject(const QString &workspacename, const QString &projnameold,
                                    const QString &projnamenew)
{
    if (workspacename.isEmpty() || projnamenew.isEmpty() || projnameold.isEmpty()
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return false;
    }
    if (!checkProjectNameValid(workspacename, projnamenew)) {
        LOGOUT(QObject::tr("Project [%1] is Already Exist,Please Rename!").arg(projnamenew), LOG_WARNING);
        return false;
    }
    PKLProject project = getProject(workspacename, projnameold);
    if (project == nullptr) {
        return false;
    }
    QString oldProjectPath = project->getProjectPath();
    if (ProjectViewServers::getInstance().m_projectManagerServer->RenamProjectName(project, projnamenew)) {
        updateWSProjectCfg(oldProjectPath, false);
        updateWSProjectCfg(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + workspacename + "/" + projnamenew + "/"
                                   + projnamenew + "." + QFileInfo(oldProjectPath).suffix(),
                           true);
        return true;
    }
    return false;
}

bool ProjDataManager::exportProject(QSharedPointer<KLProject> pProject, const QString &targetdir)
{
    if (pProject == nullptr || targetdir.isEmpty()
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return false;
    }

    bool ret = ProjectViewServers::getInstance().m_projectManagerServer->ExportProject(targetdir,
                                                                                       pProject->getProjectPath());
    return ret;
}

QSharedPointer<KLProject> ProjDataManager::importProject(const QString &importWSName, const QString &srcfilepath)
{
    KLProject::ProjectMode prjModel = projectMode();
    if (importWSName.isEmpty() || srcfilepath.isEmpty() || !m_allWorkSpaceMap.contains(importWSName)
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return nullptr;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    PKLProject pImportProj = ProjectViewServers::getInstance().m_projectManagerServer->ImportProject(
            PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + importWSName, srcfilepath, prjModel);
    QApplication::restoreOverrideCursor();
    if (pImportProj == nullptr) {
        // 导入项目失败
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Import, AssistantDefine::LOG_TYPE_PROJECT, "",
                                            false),
               LOG_WARNING);
        return nullptr;
    } else {
        // 导入项目[%1]成功
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Import, AssistantDefine::LOG_TYPE_PROJECT,
                                            pImportProj->getProjectName(), true),
               LOG_NORMAL);
    }
    if (KLProject::ProjectMode_DesignerExterrnal == prjModel) {
        m_allWorkSpaceMap[importWSName].allprojects.clear();
        m_allWorkSpaceMap[importWSName].allprojects.insert(pImportProj->getProjectName(), pImportProj);
        if (pImportProj) {
            // // 关闭打开画板
            ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->DeleteWorkareaWidgets();
            QTimer::singleShot(0, [this, pImportProj]() {
                QList<PModel> models = pImportProj->getAllModel(ControlBoardModel::Type);
                for (auto model : models) {
                    if (model) {
                        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(model);
                    }
                }
            });
        }
        return pImportProj;
    }
    if (updateWSProjectCfg(pImportProj->getProjectPath(), true)) {
        return pImportProj;
    }
    return nullptr;
}

void ProjDataManager::setProjectActiveStatus(const QString &workspacename, const QString &projname, bool bactive)
{
    PKLProject pproject = getProject(workspacename, projname);
    if (pproject != nullptr) {
        if (bactive) {
            ProjectViewServers::getInstance().m_projectManagerServer->SetCurProject(pproject);
        } else {
            ProjectViewServers::getInstance().m_projectManagerServer->SetCurProject(PKLProject(nullptr));
        }
    }
}

QSharedPointer<KLProject> ProjDataManager::getProject(const QString &workspacename, const QString &projname)
{
    if (m_allWorkSpaceMap.contains(workspacename) && m_allWorkSpaceMap[workspacename].allprojects.contains(projname)) {
        return m_allWorkSpaceMap[workspacename].allprojects[projname];
    }

    return QSharedPointer<KLProject>(nullptr);
}

QMap<QString, WorkSpaceInfo> ProjDataManager::getAllWorkSpace()
{
    return m_allWorkSpaceMap;
}

QStringList ProjDataManager::getAllDemoProjects()
{
    return m_demoProjects;
}
QMap<QString, QStringList> ProjDataManager::getCateDemoProjects()
{
    return m_demoPrjMap;
}

QSharedPointer<KLProject> ProjDataManager::copyDemoProject(const QString &sourceProjectPath,
                                                           const QString &targetWorkspaceName,
                                                           const QString &targetProjectName)
{
    if (KLProject::ProjectMode_DesignerExterrnal != projectMode()) {
        // 名称检查
        if (!checkFileName(targetProjectName)) {
            return nullptr;
        }

        if (!m_demoProjects.contains(sourceProjectPath)) {
            // "无此s示例项目！"
            LOGOUT(QObject::tr("There is no such example project!"), LOG_ERROR);
            return nullptr;
        }

        if (!checkProjectNameValid(targetWorkspaceName, targetProjectName)) {
            // 已存在项目 + targetProjectName
            LOGOUT(QObject::tr("Project [%1] already exists").arg(targetProjectName), LOG_WARNING);
            return nullptr;
        }

        // 得到目标位置的工作空间目录等参数
        QString targetWorkspacePath;
        File_npsws *targetWorkspace = getWorkSpace(targetWorkspaceName, targetWorkspacePath);
        if (targetWorkspace == nullptr) {
            return nullptr;
        }
    }
    KLProject::ProjectMode prjModel = projectMode();
    QSharedPointer<KLProject> pProject = ProjectViewServers::getInstance().m_projectManagerServer->ImportDemoProject(
            PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + targetWorkspaceName,
            BUILDI_DEMO_PATH + "/" + sourceProjectPath, targetProjectName, prjModel);

    if (KLProject::ProjectMode_DesignerExterrnal == prjModel) {
        m_allWorkSpaceMap[targetWorkspaceName].allprojects.clear();
        m_allWorkSpaceMap[targetWorkspaceName].allprojects.insert(pProject->getProjectName(), pProject);
        if (pProject) {
            // // 关闭打开画板
            ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->DeleteWorkareaWidgets();
            QTimer::singleShot(0, [this, pProject]() {
                QList<PModel> models = pProject->getAllModel(ControlBoardModel::Type);
                for (auto model : models) {
                    if (model) {
                        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(model);
                    }
                }
            });
        }
    }

    if (pProject != nullptr && updateWSProjectCfg(pProject->getProjectPath(), true)) {
        return pProject;
    }
    return nullptr;
}

QSharedPointer<KLProject> ProjDataManager::copyProject(QSharedPointer<KLProject> srcProject,
                                                       const QString &targetWSName)
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr || srcProject == nullptr
        || targetWSName.isEmpty()) {
        return nullptr;
    }
    QString targetProjName = srcProject->getProjectName();
    int index = 1;
    while (!checkProjectNameValid(targetWSName, targetProjName)) {
        targetProjName = srcProject->getProjectName() + QString("(%1)").arg(QString::number(index++));
    }
    PKLProject cpProject = ProjectViewServers::getInstance().m_projectManagerServer->CopyProject(
            srcProject, PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + targetWSName, targetProjName);
    if (cpProject != nullptr) {
        updateWSProjectCfg(cpProject->getProjectPath(), true);
        return cpProject;
    }
    return nullptr;
}

QSharedPointer<KLProject> ProjDataManager::moveProject(QSharedPointer<KLProject> srcProject,
                                                       const QString &targetWSName)
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr || srcProject == nullptr
        || targetWSName.isEmpty()) {
        return nullptr;
    }
    QString oldProjectFilePath = srcProject->getProjectPath();
    QString targetProjName = srcProject->getProjectName();
    int index = 1;
    while (!checkProjectNameValid(targetWSName, targetProjName)) {
        targetProjName = srcProject->getProjectName() + QString("(%1)").arg(QString::number(index++));
    }
    PKLProject mvProject = ProjectViewServers::getInstance().m_projectManagerServer->MoveProject(
            srcProject, PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + targetWSName, targetProjName);
    if (mvProject != nullptr) {
        updateWSProjectCfg(oldProjectFilePath, false);
        updateWSProjectCfg(mvProject->getProjectPath(), true);
        return mvProject;
    }
    return nullptr;
}

QSharedPointer<KLProject> ProjDataManager::getCurrentProject()
{
    if (ProjectViewServers::getInstance().m_projectManagerServer != nullptr) {
        return ProjectViewServers::getInstance().m_projectManagerServer->GetCurProject();
    }

    return QSharedPointer<KLProject>(nullptr);
}

const WorkSpaceInfo ProjDataManager::getCurrentWorkSpace()
{
    PKLProject curProject = getCurrentProject();
    if (curProject == nullptr) {
        return WorkSpaceInfo();
    }

    for (WorkSpaceInfo wsinfo : m_allWorkSpaceMap.values()) {
        for (PKLProject project : wsinfo.allprojects.values()) {
            if (project == curProject) {
                return wsinfo;
            }
        }
    }
    return WorkSpaceInfo();
}

QSharedPointer<KLProject> ProjDataManager::getSystemProject()
{
    return BuiltinModelManager::getInstance().getBuiltinProject();
}

void ProjDataManager::reloadData(const QString &projectPath)
{
    initBootFile();
    initWorkspace();
    loadAllProjects(projectPath);
    loadDemoProjects();
    // 项目启动时，加载变量到数据字典
    AssistantDefine::addProjectVarToDictionary(getCurrentProject());
}

QMap<QString, QVariant> ProjDataManager::getTreeStateMap(const QString &treeType)
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr || treeType.isEmpty()) {
        return QVariantMap();
    }
    return bootFile->getTreeState(treeType);
}

bool ProjDataManager::saveTreeStateMap(const QString &treeType, const QVariantMap &stsMap)
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr) {
        return false;
    }
    bootFile->setTreeState(stsMap, treeType);
    if (!saveFileData(PROJDATAMNGKEY::BOOS_FILE_PATH, bootFile)) {
        return false;
    }
    return true;
}

void ProjDataManager::setSortType(const int &sorttype)
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr) {
        return;
    }
    return bootFile->setSortType(sorttype);
}

const int ProjDataManager::getSortType()
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr) {
        return -1;
    }
    return bootFile->getSortType();
}

// bool ProjDataManager::improtDemoProject(const QString &WorkspaceName)
// {

//     // QString workspaceName = getWorkSpaceName("导入示例项目");
//     // if (workspaceName.isEmpty())
//     //     return;

//     // DemoProjectView *projectView = new DemoProjectView();
//     // projectView->setMessage("请选择示例项目：");
//     // projectView->setNameList(pDataManagerServer->GetDemoProjectNames());
//     // KCustomDialog *dialog = new KCustomDialog("请选择示例项目", projectView, KBaseDlgBox::Ok |
//     KBaseDlgBox::Cancel);
//     // dialog->resize(400, 500);
//     // if (dialog->exec() != KBaseDlgBox::Ok)
//     //     return;

//     QStringList projectNameList; /*= projectView->getSelectNames();*/
//     if (projectNameList.isEmpty())
//         return;

//     for each (QString projectName in projectNameList) {
//         // check name
//         int n = 1;
//         QString nameTemp = projectName;
//         while (m_allWorkSpaceMap[importWorkspaceName].allprojects.contains(nameTemp)) {
//             nameTemp = projectName + QString("(%1)").arg(n++);
//         }

//         if (1 /*CopyDemoProject(projectName, importWorkspaceName, nameTemp)*/)
//             LOGOUT("导入项目\"" + nameTemp + "\"完成"   , LOG_NORMAL);
//         else
//             LOGOUT("导入项目\"" + nameTemp + "\"失败", LOG_ERROR);
//     }
// }

File_npsboot *ProjDataManager::getBootFile()
{
    return dynamic_cast<File_npsboot *>(m_allFiles[File_npsboot::suffix()][PROJDATAMNGKEY::BOOS_FILE_PATH]);
}

File_npsws *ProjDataManager::getWorkSpace(const QString &wsname, QString &wsFilePath)
{
    wsFilePath = "";
    for (QString path : m_allFiles[File_npsws::suffix()].keys()) {
        if (QFileInfo(path).baseName() == wsname) {
            wsFilePath = path;
            return dynamic_cast<File_npsws *>(m_allFiles[File_npsws::suffix()][path]);
            break;
        }
    }
    return nullptr;
}

QString ProjDataManager::getRelativeFilePath(QString dirPath, QString filePath)
{
    return QDir(dirPath).relativeFilePath(filePath);
}

bool ProjDataManager::writeToFile(const QString filePath, const QString fileContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 打开文件\"" + filePath + "\"失败！原因：" + file.errorString()
        LOGOUT(QObject::tr("Failed to open file [%1]! Reason: %2").arg(filePath).arg(file.errorString()), LOG_ERROR);
        return false;
    }

    QTextStream text(&file);
    text << fileContent;

    file.close();
    return true;
}

bool ProjDataManager::readFromFile(const QString filePath, QString &fileContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 打开文件\"" + filePath + "\"失败！原因：" + file.errorString()
        LOGOUT(QObject::tr("Failed to open file [%1]! Reason: %2").arg(filePath).arg(file.errorString()), LOG_ERROR);
        return false;
    }

    QTextStream text(&file);
    fileContent = text.readAll();

    file.close();
    return true;
}

bool ProjDataManager::saveFileData(const QString filePath, FileData *data)
{
    if (!isRegisteredSuffix(filePath)) {
        delete data;
        return false;
    }

    if (!writeToFile(filePath, data->serialized())) {
        delete data;
        return false;
    }

    m_allFiles[QFileInfo(filePath).suffix()][filePath] = data;
    return true;
}

bool ProjDataManager::loadFileData(const QString filePath, FileData *data)
{
    if (!isRegisteredSuffix(filePath)) {
        delete data;
        return false;
    }

    QString fileContent;
    if (!readFromFile(filePath, fileContent)) {
        delete data;
        return false;
    }

    data->deserialized(fileContent);
    m_allFiles[QFileInfo(filePath).suffix()][filePath] = data;

    return true;
}

bool ProjDataManager::checkFileName(QString name)
{
    QRegExp regworkspace(PROJDATAMNGKEY::REG_WORKSPACENAME);
    bool bmatch = regworkspace.exactMatch(name);
    if (!bmatch) {
        //"名称只能是英文、数字、中文、下划线、横杠，长度最多32个字符。"
        LOGOUT(QObject::tr("The name can only be in English, numbers, Chinese, underscores, and horizontal bars, with "
                           "a maximum length of 32 characters."),
               LOG_WARNING);
        return false;
    }
    return true;
}

bool ProjDataManager::checkWSNameValid(const QString &wsName)
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr) {
        return true;
    }

    for (QString workspace : bootFile->getWorkspaceList()) {
        if (wsName.compare(QFileInfo(workspace).baseName(), Qt::CaseInsensitive) == 0) {
            return false;
        }
    }
    return true;
}

bool ProjDataManager::checkProjectNameValid(const QString &wsName, const QString &projName)
{
    QString wspath;
    File_npsws *wsFileData = getWorkSpace(wsName, wspath);
    if (wsFileData == nullptr) {
        return true;
    }
    for (QString projRelativePath : wsFileData->getAllProject()) {
        if (projName.compare(QFileInfo(projRelativePath).baseName(), Qt::CaseInsensitive) == 0) {
            return false;
        }
    }
    return true;
}

bool ProjDataManager::isRegisteredSuffix(const QString filePath)
{
    QString suffix = QFileInfo(filePath).suffix();

    if (m_allFiles.contains(suffix)) {
        return true;
    } else {
        // 无法处理后缀名为\"" + suffix + "\"的文件！
        LOGOUT(QObject::tr("Unable to process files with suffix name [%1]!").arg(suffix), LOG_ERROR);
        return false;
    }
}

bool ProjDataManager::deleteDirectory(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }

    QDir dir(path);
    if (!dir.exists()) {
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); // 设置过滤
    QFileInfoList fileList = dir.entryInfoList();           // 获取所有的文件信息
    for (QFileInfo file : fileList) {                       // 遍历文件信息
        if (file.isFile()) {                                // 是文件，删除
            file.dir().remove(file.fileName());
        } else { // 递归删除
            deleteDirectory(file.absoluteFilePath());
        }
    }

    return dir.rmdir(dir.absolutePath()); // 删除文件夹
}

void ProjDataManager::initBootFile()
{
    if (KLProject::ProjectMode_DesignerExterrnal == projectMode()) {
        return;
    }
    if (!QDir(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH).exists()) {
        if (!QDir().mkpath(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH)) {
            // 程序内部异常：无法创建目录[%1]!
            LOGOUT(QObject::tr("Program internal exception: Unable to create directory [%1]!")
                           .arg(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH),
                   LOG_ERROR);
            return;
        }
    }

    // 检查启动文件
    if (!QFile::exists(PROJDATAMNGKEY::BOOS_FILE_PATH)) {
        saveFileData(PROJDATAMNGKEY::BOOS_FILE_PATH, new File_npsboot);
        return;
    }
    for (QString suffixstr : m_allFiles.keys()) {
        m_allFiles[suffixstr].clear();
    }
    loadFileData(PROJDATAMNGKEY::BOOS_FILE_PATH, new File_npsboot);
}

void ProjDataManager::initWorkspace()
{
    if (KLProject::ProjectMode_DesignerExterrnal == projectMode()) {
        return;
    }
    // npsboot按运行逻辑是必然存在的，但是考虑到文件创建可能失败（例如没有权限），所以还是检查一下
    if (!m_allFiles[File_npsboot::suffix()].contains(PROJDATAMNGKEY::BOOS_FILE_PATH)) {
        // 程序内部异常：未找到启动文件[%1]\""
        LOGOUT(QObject::tr("Program internal exception: startup file [%1] not found !")
                       .arg(PROJDATAMNGKEY::BOOS_FILE_PATH),
               LOG_ERROR);
        return;
    }

    auto bootFile = dynamic_cast<File_npsboot *>(m_allFiles[File_npsboot::suffix()][PROJDATAMNGKEY::BOOS_FILE_PATH]);
    if (bootFile == nullptr) {
        LOGOUT(QObject::tr("Program internal exception: startup file [%1] not found !")
                       .arg(PROJDATAMNGKEY::BOOS_FILE_PATH),
               LOG_ERROR);
        return;
    }

    QStringList workspaceList = bootFile->getWorkspaceList();

    for (QString workspaceFilePath : workspaceList) {
        loadFileData(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + workspaceFilePath, new File_npsws);
    }
}

void ProjDataManager::loadAllProjects(const QString &projectPath)
{
    m_allWorkSpaceMap.clear();
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    if (KLProject::ProjectMode_DesignerExterrnal == projectMode()) {
        if (projectPath.isEmpty()) {
            LOGOUT(tr("Project path is empty!"), LOG_WARNING);
            return;
        }
        QString tempProjectPath = projectPath;
        tempProjectPath = tempProjectPath.replace("\\", "/");
        QSharedPointer<KLProject> proj = nullptr;
        if (QFileInfo(tempProjectPath).isDir()) {
            QStringList pathList = tempProjectPath.split("/");
            QString projName = "";
            tempProjectPath = "";
            if (pathList.size() > 0) {
                projName = pathList.takeLast();
            }
            proj = ProjectViewServers::getInstance().m_projectManagerServer->CreatProject(pathList.join("/"), projName,
                                                                                          tempProjectPath);
            if (proj == nullptr) {
                LOGOUT(tr("Project is empty!"), LOG_WARNING);
                return;
            }
            ProjectViewServers::getInstance().m_projectManagerServer->SetCurProject(proj);
            if (ProjectViewServers::getInstance().m_pGraphicModelingServer != nullptr
                && ProjectViewServers::getInstance().m_pPropertyManagerServer != nullptr) {
                PDrawingBoardClass pdrawboard =
                        ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDrawBoard(
                                PMKEY::BoardTypeElectrical, false);
                PDrawingBoardClass ctrdrawboard =
                        ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDrawBoard(
                                PMKEY::BoardTypeControl, false);
                ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(pdrawboard);
                ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(ctrdrawboard);
            }
        } else {
            proj = ProjectViewServers::getInstance().m_projectManagerServer->LoadProject(tempProjectPath);
            if (proj == nullptr) {
                LOGOUT(tr("Project is empty!"), LOG_WARNING);
                return;
            }
            ProjectViewServers::getInstance().m_projectManagerServer->SetCurProject(proj);
        }
        WorkSpaceInfo wsinfo(getExternalWorkSpace(tempProjectPath));
        wsinfo.allprojects.insert(QFileInfo(tempProjectPath).baseName(), proj);
        m_allWorkSpaceMap.insert(wsinfo.name, wsinfo);
        return;
    } else {
        // 初始化所有workspace
        for (QString npswspath : m_allFiles[File_npsws::suffix()].keys()) {
            WorkSpaceInfo wsinfo(QFileInfo(npswspath).baseName());
            File_npsws *wsfile = dynamic_cast<File_npsws *>(m_allFiles[File_npsws::suffix()][npswspath]);
            if (wsfile == nullptr) {
                continue;
            }
            for (QString projrelativepath : wsfile->getAllProject()) {
                QSharedPointer<KLProject> proj = ProjectViewServers::getInstance().m_projectManagerServer->LoadProject(
                        QFileInfo(npswspath).absolutePath() + "/" + projrelativepath);
                if (proj != nullptr) {
                    wsinfo.allprojects.insert(QFileInfo(projrelativepath).baseName(), proj);
                }
            }
            m_allWorkSpaceMap.insert(wsinfo.name, wsinfo);
        }
        // 初始化所有项目加载完成后,调用一下项目管理器LoadAllProjectFinished
        ProjectViewServers::getInstance().m_projectManagerServer->LoadAllProjectFinished();
    }
}
// 解析示例项目分类文件
QMap<QString, QStringList> analyfile(const QString &fileclassName, const QString &filepath)
{
    QMap<QString, QStringList> demomap;
    QStringList demolist;
    QFile tmpFile(filepath);
    QString className = fileclassName;
    bool isEnglish = "en_US" == QPHelper::getLanguage();
    if (!tmpFile.open(QIODevice::ReadOnly)) {
        return demomap;
    }

    QTextStream text(&tmpFile);
    text.setCodec("UTF-8"); // 固定使用UTF-8编码
    QString content = text.readAll();
    tmpFile.close();

    if (content.isEmpty()) {
        return demomap;
    }

    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (!doc.isObject()) {
        LOGOUT(QObject::tr("Parsing json failed"), LOG_ERROR);
        return demomap;
    }

    QJsonObject root = doc.object();
    QJsonArray projarray = root.value(PROJDATAMNGKEY::KEY_ALLPROJECT).toArray();
    for (auto const &file : projarray) {
        demolist.append(file.toObject().value(PROJDATAMNGKEY::KEY_PROJECTPATH).toString() + "~"
                        + file.toObject().value(PROJDATAMNGKEY::KEY_ENGLISHNAME).toString());
    }
    // 最后一个是图标
    demolist.append(root.value(PROJDATAMNGKEY::KEY_PROJECTICON).toString());
    if (isEnglish) {
        className = root.value(PROJDATAMNGKEY::KEY_CLASSENGLISHNAME).toString();
    }
    demomap.insert(className, demolist);
    return demomap;
}

void ProjDataManager::loadDemoProjects()
{
    m_demoProjects.clear();
    QString demoPath = BUILDI_DEMO_PATH + "/演示空间." + File_npsws::suffix();
    QFile tmpFile(demoPath);
    if (!tmpFile.open(QIODevice::ReadOnly)) {
        //"示例项目不存在！"
        LOGOUT(QObject::tr("Example project does not exist!"), LOG_ERROR);
        return;
    }
    QTextStream text(&tmpFile);
    text.setCodec("UTF-8"); // 固定使用UTF-8编码
    QString content = text.readAll();
    tmpFile.close();

    if (content.isEmpty()) {
        //"示例项目中无内容！"
        LOGOUT(QObject::tr("There is no content in the Example project!"), LOG_ERROR);
        return;
    }

    QScopedPointer<File_npsws> tmpWs(new File_npsws);
    tmpWs->deserialized(content);
    m_demoProjects = tmpWs->getAllProject();

    QDir dir(BUILDI_DEMO_PATH);
    QFileInfoList files = dir.entryInfoList(QStringList() << "*.json", QDir::Files, QDir::Name);
    foreach (auto file, files) {
        m_demoPrjMap.insert(analyfile(file.baseName(), file.absoluteFilePath()));
    }
}

QString ProjDataManager::getWorkSpaceFilePath(const QString &projFileAbsolutePath)
{
    QString relativepath = getRelativeFilePath(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH, projFileAbsolutePath);
    QStringList tmplist = relativepath.split("/");
    if (tmplist.size() <= 0) {
        return QString();
    }

    return PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH + "/" + tmplist[0] + "/" + tmplist[0] + "." + File_npsws::suffix();
}

bool ProjDataManager::updateBootFileWSCfg(const QString &workspacePath, bool bAdd)
{
    File_npsboot *bootFile = getBootFile();
    if (bootFile == nullptr) {
        return false;
    }

    QString workspaceRelativePath = getRelativeFilePath(PROJDATAMNGKEY::SIMUNPS_WORKSPACE_PATH, workspacePath);
    if (bAdd) {
        if (!saveFileData(workspacePath, new File_npsws)) {
            //"无法创建工作空间文件：\"" + workspacePath + "\"！"
            LOGOUT(QObject::tr("Unable to create workspace file:%1 !").arg(workspacePath), LOG_ERROR);
            return false;
        }
        bootFile->addWorkspace(workspaceRelativePath);
        WorkSpaceInfo info(QFileInfo(workspacePath).baseName());
        m_allWorkSpaceMap.insert(info.name, info);
    } else {
        bootFile->removeWorkSpace(workspaceRelativePath);
        delete m_allFiles[File_npsws::suffix()][workspacePath];
        m_allFiles[File_npsws::suffix()][workspacePath] = nullptr;
        m_allFiles[File_npsws::suffix()].remove(workspacePath);
        m_allWorkSpaceMap.remove(QFileInfo(workspacePath).baseName());
    }

    if (!saveFileData(PROJDATAMNGKEY::BOOS_FILE_PATH, bootFile)) {
        return false;
    }
    return true;
}

bool ProjDataManager::updateWSProjectCfg(const QString &projFileAbsolutePath, bool bAdd)
{
    if (KLProject::ProjectMode_DesignerExterrnal == projectMode()) {
        return true;
    }
    if (projFileAbsolutePath.isEmpty()) {
        return false;
    }
    QString workspacefilepath = getWorkSpaceFilePath(projFileAbsolutePath);
    QString workspacepath = QFileInfo(workspacefilepath).path();
    QString projrelativefilepath = getRelativeFilePath(workspacepath, projFileAbsolutePath);
    File_npsws *workspaceFile = nullptr;
    for (QString pathstr : m_allFiles[File_npsws::suffix()].keys()) {
        if (pathstr == workspacefilepath) {
            workspaceFile = dynamic_cast<File_npsws *>(m_allFiles[File_npsws::suffix()][pathstr]);
            if (bAdd) {
                workspaceFile->addProject(projrelativefilepath);
                m_allWorkSpaceMap[QFileInfo(pathstr).baseName()]
                        .allprojects[QFileInfo(projFileAbsolutePath).baseName()] =
                        ProjectViewServers::getInstance().m_projectManagerServer->GetProject(projFileAbsolutePath);
            } else {
                workspaceFile->removeProject(projrelativefilepath);
                m_allWorkSpaceMap[QFileInfo(pathstr).baseName()].allprojects.remove(
                        QFileInfo(projFileAbsolutePath).baseName());
            }
            return saveFileData(workspacefilepath, workspaceFile);
        }
    }
    return false;
}

bool ProjDataManager::copyDirectory(const QString &srcPath, const QString &dstPath, bool coverFileIfExist,
                                    bool topLevel = true)
{
    QDir srcDir(srcPath);
    QDir dstDir(dstPath);
    if (!dstDir.exists()) { // 目的文件目录不存在则创建文件目录
        if (!dstDir.mkdir(dstDir.absolutePath()))
            return false;
    } else if (topLevel) {
        // 目标文件夹已存在，是否确认替换
        if (KMessageBox::question(AssistantDefine::getMainWindow(),
                                  QObject::tr("The target folder already exists. Are you sure to replace it!"),
                                  KMessageBox::Yes | KMessageBox::No)
            != KMessageBox::Yes)
            return false;
    }

    QFileInfoList fileInfoList = srcDir.entryInfoList();
    for (QFileInfo fileInfo : fileInfoList) {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if (fileInfo.isDir()) { // 当为目录时，递归的进行copy
            if (!copyDirectory(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()), coverFileIfExist, false))
                return false;
        } else { // 当允许覆盖操作时，将旧文件进行删除操作
            if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
                dstDir.remove(fileInfo.fileName());
            }
            /// 进行文件copy
            if (!QFile::copy(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()))) {
                return false;
            }
        }
    }
    return true;
}

bool ProjDataManager::checkeWorkSpaceFileIsOld(const QString &workspaceDir, QStringList projectList)
{
    if (projectList.isEmpty() || workspaceDir.isEmpty()) {
        return false;
    }

    QString projectPath = workspaceDir + "/" + projectList.first();
    QStringList files = QDir(DIR_PATH(projectPath)).entryList();
    if (!files.contains(KL_PRO::OLD_DRAWBOARD_FILE) && !files.contains(KL_PRO::OLD_BLOCK_FILE)) {
        return false;
    }
    return true;
}

QString ProjDataManager::getExternalWorkSpace(const QString &projectPath)
{
    if (projectPath.isEmpty()) {
        return PROJDATAMNGKEY::DESINGERWS;
    }
    QStringList folderList = projectPath.split("/");
    if (folderList.size() < 5) {
        return PROJDATAMNGKEY::DESINGERWS;
    } else {
        return folderList[folderList.size() - 5];
    }
}

KLProject::ProjectMode ProjDataManager::projectMode()
{
#ifdef COMPILER_PRODUCT_DESIGNER
    if (gConfGet("tempMode").toBool()) {
        return KLProject::ProjectMode_DesignerExterrnal;
    } else {
        return KLProject::ProjectMode_DesignerGeneral;
    }
#else
    return KLProject::ProjectMode_SimuNPS;
#endif
}

void ProjDataManager::editExternalProject(const QString &projectAbsPath)
{
    reloadData(projectAbsPath);
}

// 所有工作空间和当前当作工作空间存储
QString File_npsboot::serialized()
{
    QJsonObject root;
    QJsonArray wsarray;
    for (QString workspacePath : m_allWorkspace) {
        QJsonObject obj;
        obj.insert(PROJDATAMNGKEY::KEY_WORKSPACEPATH, workspacePath);
        wsarray.append(obj);
    }
    root.insert(PROJDATAMNGKEY::KEY_ALLWORKSPACE, wsarray);
    QJsonArray datamngArray;
    for (QString keystr : m_DataMngTreeSts.keys()) {
        QJsonObject objDM;
        objDM.insert(keystr, m_DataMngTreeSts[keystr].toJsonValue());
        datamngArray.append(objDM);
    }
    root.insert(PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG, datamngArray);
    QJsonArray networkArray;
    for (QString keystr : m_NetWorkMngTreeSts.keys()) {
        QJsonObject objNW;
        objNW.insert(keystr, m_NetWorkMngTreeSts[keystr].toJsonValue());
        networkArray.append(objNW);
    }
    root.insert(PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG, networkArray);
    root.insert(PROJDATAMNGKEY::KEY_PROJ_SORTTYPE, m_sortProjectType);
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

void File_npsboot::deserialized(const QString &content)
{
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (!doc.isObject()) {
        LOGOUT(QObject::tr("Parsing json failed"), LOG_ERROR);
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray wsarray = root.value(PROJDATAMNGKEY::KEY_ALLWORKSPACE).toArray();
    for (auto const &file : wsarray) {
        m_allWorkspace.append(file.toObject().value(PROJDATAMNGKEY::KEY_WORKSPACEPATH).toString());
    }
    if (root.contains(PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG)) {
        QJsonArray DMArray = root.value(PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG).toArray();
        for (auto const &file : DMArray) {
            QVariantMap tmpMap = file.toObject().toVariantMap();
            for (const QString keystr : tmpMap.keys()) {
                m_DataMngTreeSts[keystr] = tmpMap[keystr];
            }
        }
    }
    if (root.contains(PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG)) {
        QJsonArray NWArray = root.value(PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG).toArray();
        for (auto const &file : NWArray) {
            QVariantMap tmpMap = file.toObject().toVariantMap();
            for (const QString keystr : tmpMap.keys()) {
                m_NetWorkMngTreeSts[keystr] = tmpMap[keystr];
            }
        }
    }
    if (root.contains(PROJDATAMNGKEY::KEY_PROJ_SORTTYPE)) {
        m_sortProjectType = root.value(PROJDATAMNGKEY::KEY_PROJ_SORTTYPE).toInt();
    } else {
        m_sortProjectType = 0;
    }
}

// 当前工作空间下的所有项目
QString File_npsws::serialized()
{
    QJsonObject root;
    QJsonArray projarray;
    for (QString projectPath : m_allProject) {
        QJsonObject obj;
        obj.insert(PROJDATAMNGKEY::KEY_PROJECTPATH, projectPath);
        projarray.append(obj);
    }
    root.insert(PROJDATAMNGKEY::KEY_ALLPROJECT, projarray);
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

void File_npsws::deserialized(const QString &content)
{
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (!doc.isObject()) {
        LOGOUT(QObject::tr("Parsing json failed"), LOG_ERROR);
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray projarray = root.value(PROJDATAMNGKEY::KEY_ALLPROJECT).toArray();
    for (auto const &file : projarray) {
        m_allProject.append(file.toObject().value(PROJDATAMNGKEY::KEY_PROJECTPATH).toString());
    }
}

#include "ProjectManagerServer.h"
#include "Associate.h"
#include "CoreLib/ServerManager.h"
#include "GraphicsModelingKernel/CanvasContext.h"
#include "JlCompress.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLWidgets/KMessageBox.h"
#include "ProjectServerMng.h"
#include <CoreLib/GlobalConfigs.h>
#include <QLabel>
#include <QMovie>

#include <QClipboard>

USE_LOGOUT_("ProjectManagerServer")

using namespace Kcc::SimuNPSAdapter;
#define LOAD_GIF_DIR                                                                                                   \
    (QFileInfo(QCoreApplication::applicationDirPath()).canonicalFilePath() + "/configs/splashScreen/launch")

ProjectManagerServer::ProjectManagerServer()
{
    fileManager = new KLFileManager();
    curProjectPath = QString("");
    // // 非外部模式才去读取当前激活项目路径
    // if (tempProjectPath.isEmpty()) {
    //     readProjectPath();
    // }

    projectManager = ModelSystem::getInstance()->getProjectManager();
}

ProjectManagerServer::~ProjectManagerServer()
{
    // 平台插件 软件运行一直存在 不需要析构
}

void ProjectManagerServer::Init()
{
    if (!ProjectServerMng::getInstance().init()) {
        LOGOUT(tr("ProjectServerMng initialization failed"), LOG_ERROR); // ProjectServerMng初始化失败
        return;
    }

    BuiltinModelManager::getInstance(); // 触发一下单例的构造
}

void ProjectManagerServer::UnInit()
{
    ProjectServerMng::getInstance().unInit();
}

PKLProject ProjectManagerServer::LoadProject(const QString &projPath)
{
    if (FindProject(projPath)) {
        return projects.value(projPath);
    }

    QString suffix = KLProject::getProjectFileSuffix();
    if (QFileInfo(projPath).suffix() != suffix) {
        LOGOUT(tr("Incorrect project format. Failed to load"), LOG_ERROR); // 项目格式有误,加载失败
        return PKLProject();
    }

    PKLProject newProject = PKLProject(new KLProject(projPath));

    // // 注册项目
    // projectManager->addProject(projPath);

    // 这个switch必须放在loadProject之前,内部Model在反序列化连接关系时是往当前项目里面加的
    projectManager->switchProject(projPath);

    bool ret = newProject->loadProject(); // 创建项目、项目配置信息文件

    projectManager->loadProjectData();

    // 注册完成后,清除选择的项目
    projectManager->switchProject(curProjectPath);

    if (!ret) {
        newProject.clear();
        projectManager->removeProject(projPath);
        return PKLProject();
    }
    projects[projPath] = newProject;

    bindSignal(newProject);
    return newProject;
}

PKLProject ProjectManagerServer::CreatProject(const QString &dir, const QString &projName, QString &projPath)
{
    projPath = setProjectPath(dir, projName);
    if (FindProject(projPath)) {
        LOGOUT(QString(tr("Project %1 already exists")).arg(projPath), LOG_WARNING); // 项目%1已存在
        return PKLProject();
    }

    PKLProject newProject = PKLProject(new KLProject(projPath));
    newProject->init(); // 创建项目、项目配置信息文件
    projects[projPath] = newProject;
    // projectManager->addProject(projPath);
    auto modelPool = projectManager->getProject(projPath);
    if (modelPool) {
        modelPool->load();
    }

    bindSignal(newProject);

    NotifyStruct notify;
    notify.code = IPM_Notify_CreateProjectFinished;
    notify.paramMap["name"] = newProject->getProjectName();
    emitNotify(notify);

    return newProject;
}

bool ProjectManagerServer::DestoryProject(PKLProject project)
{
    QString path = project->getProjectPath();
    if (!FindProject(path)) {
        LOGOUT(QString(tr("Project %1 does not exist and failed to be deleted")).arg(path),
               LOG_WARNING); // 项目%1不存在,删除失败
        return false;
    }

    if (curProjectPath == project->getProjectPath()) {
        curProjectPath = "";
        saveProjectPath();
    }

    projectManager->removeProject(project->getProjectPath());

    // 删除项目所属文件目录所有文件
    QString dir = DIR_PATH(path);
    fileManager->deleteDirectory(dir);

    // 删除文件映射,释放内存
    projects[path].clear();
    projects.remove(path);
    // project = nullptr;

    return true;
}

bool ProjectManagerServer::FindProject(const QString &path)
{
    return QStringList(projects.keys()).contains(path, Qt::CaseInsensitive);
}

PKLProject ProjectManagerServer::CopyProject(PKLProject sourceProject, const QString &targetDir,
                                             const QString &targetProject)
{
    if (!sourceProject || sourceProject->getProjectPath().isEmpty() || targetDir.isEmpty() || targetProject.isEmpty())
        return PKLProject();

    QString sourceProjectDir, sourcePorijectPath, targetProjectDir, targetPorjectPath;
    sourcePorijectPath = sourceProject->getProjectPath();
    targetPorjectPath = setProjectPath(targetDir, targetProject);
    sourceProjectDir = DIR_PATH(sourcePorijectPath);
    targetProjectDir = DIR_PATH(targetPorjectPath);

    QString sourceProjName = BASE_NAME(sourcePorijectPath);
    // 查询原始项目是否存在
    {
        bool isExist = false;
        foreach (QString path, projects.keys()) {
            if (path == sourcePorijectPath) {
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            LOGOUT(QString(tr("Project %1 does not exist")).arg(sourcePorijectPath), LOG_WARNING); // 项目%1不存在
            return PKLProject();
        }
    }
    // 查询目标项目是否存在
    {
        bool isExist = false;
        foreach (QString path, projects.keys()) {
            if (path == targetPorjectPath) {
                isExist = true;
                break;
            }
        }
        if (isExist) {
            LOGOUT(QString(tr("Project %1 already exists")).arg(targetPorjectPath), LOG_WARNING); // 项目%1已存在
            return PKLProject();
        }
    }

    // 拷贝原始项目目录下文件夹
    bool copied = fileManager->copyDirectory(sourceProjectDir, targetProjectDir, true);
    if (!copied) {
        LOGOUT(tr("Error copying file"), LOG_ERROR); // 复制文件出错
        fileManager->deleteDirectory(targetProjectDir);
        return PKLProject();
    }

    // 若项目名不一样,修改项目文件名字
    if (sourceProjName != targetProject) {

        QString name = targetProjectDir + "/" + sourceProjName + "." + KLProject::getProjectFileSuffix();
        if (!fileManager->renameFile(name, targetPorjectPath)) {
            fileManager->deleteDirectory(targetProjectDir);
            return PKLProject();
        }
    }

    // 加载复制后的项目
    PKLProject loadProject = LoadProject(targetPorjectPath);
    if (loadProject) {
        loadProject->setProjectProperty(KL_PRO::CREATE_TIME,
                                        QDateTime::currentDateTime().toString(KL_PRO::DATE_FORMAT));
    }
    return loadProject;
}

PKLProject ProjectManagerServer::MoveProject(PKLProject sourceProject, const QString &targetDir,
                                             const QString &targetProjectName)
{
    if (!sourceProject || sourceProject->getProjectPath().isEmpty()) {
        return PKLProject();
    }

    QString sourceDir = getProjectPrefix(sourceProject->getProjectPath());
    QString sourceProjectName = BASE_NAME(sourceProject->getProjectPath());
    if (sourceDir == targetDir && sourceProjectName == targetProjectName) {
        // 本项目移动不做任何操作
        return sourceProject;
    }
    PKLProject copyProj = CopyProject(sourceProject, targetDir, targetProjectName);
    if (copyProj) {
        DestoryProject(sourceProject);
    }
    return copyProj;
}

PKLProject ProjectManagerServer::ImportProject(const QString &targetDir, const QString &filePath,
                                               KLProject::ProjectMode prjModel)
{
    QString tarDir = targetDir;

#ifdef COMPILER_PRODUCT_DESIGNER
    PKLProject curProject;
    QString curProjectDir, oldName, importName;
    if (KLProject::ProjectMode::ProjectMode_DesignerExterrnal == prjModel) {
        curProject = GetCurProject();
        if (!curProject) {
            return PKLProject();
        }
        curProjectDir = curProject->getProjectDir();
        tarDir = DIR_PATH(curProjectDir);
        oldName = BASE_NAME(curProject->getProjectPath());
    }
#endif

    if (tarDir.isEmpty() || filePath.isEmpty()) {
        return PKLProject();
    }
    // 开始重命名、解压
    QString Path = setProjectPathExt(tarDir, BASE_NAME(filePath));
    QString projectDir = DIR_PATH(Path);
    if (QDir(projectDir).exists()) {
        // 得到当前文件夹下所有子文件夹的名称
        QStringList allDirs;
        QFileInfoList allFileList = QDir(tarDir).entryInfoList();
        foreach (QFileInfo file, allFileList) {
            const QString &tmpUse = BASE_NAME(file);
            if (tmpUse.isEmpty() || !file.isDir() || tmpUse == "." || tmpUse == "..")
                continue;
            allDirs << file.baseName();
        }

        int i = 0;
        QString tmpName;
        do {
            ++i;
            tmpName = BASE_NAME(filePath) + QString("(%1)").arg(i);
        } while (allDirs.contains(tmpName, Qt::CaseInsensitive));
        projectDir += QString("(%1)").arg(i);
    }

    QFile tmpFile(filePath);
    QString newName = DIR_PATH(filePath) + "/" + BASE_NAME(filePath) + ".zip";
    tmpFile.rename(newName);

    QString code;
#ifdef WIN32
    code = "system";
#else
    code = "GBK";
#endif

    const QStringList &unpackedFiles = JlCompress::extractDir(newName, projectDir);
    tmpFile.rename(filePath);

    if (unpackedFiles.empty()) {
        LOGOUT(tr("Import project failed"), LOG_ERROR); // 导入项目失败
        fileManager->deleteDirectory(projectDir);
        return PKLProject();
    }

    QString projectPath;
    foreach (QString prjName, unpackedFiles) {
        if (QFileInfo(prjName).suffix() == KLProject::getProjectFileSuffix()) {
            // 如果当前工作空间已经有了这个名字的项目，则重命名
            const QString &tmpStr = BASE_NAME(prjName);
            projectPath = DIR_PATH(prjName) + "/" + BASE_NAME(projectDir) + "." + KLProject::getProjectFileSuffix();
            QFile(prjName).rename(projectPath);
            break;
        }
    }

#ifdef COMPILER_PRODUCT_DESIGNER
    if (KLProject::ProjectMode::ProjectMode_DesignerExterrnal == prjModel) {
        // 删除原项目
        fileManager->deleteDirectory(curProjectDir);
        DestoryProject(curProject);
        // 重命名为原项目
        QString tempPath = DIR_PATH(projectPath) + "/" + oldName + "." + KLProject::getProjectFileSuffix();
        QFile(projectPath).rename(tempPath);
        QString tempDir = tarDir + "/" + oldName;
        importName = BASE_NAME(projectDir);
        QDir().rename(tarDir + "/" + importName, tempDir);

        projectPath = tempDir + "/" + oldName + "." + KLProject::getProjectFileSuffix();
    }
#endif

    // 若导入的项目为 model之前的旧数据项目,则导入失败
    if (checkProjectIsStale(projectPath)) {
        return PKLProject();
    }

    // 加载导入的项目
    auto loadProj = LoadProject(projectPath);

    if (loadProj) {
        saveProject(loadProj);
        // // 导入的项目版本若是低于1.0.5,则保存项目内所有模型
        // SaveProjectAllModel(QList<PKLProject>() << loadProj);
        loadProj->setProjectProperty(KL_PRO::CREATE_TIME, QDateTime::currentDateTime().toString(KL_PRO::DATE_FORMAT));
        // 导入项目处理完成,发送项目导入完成信号
        NotifyStruct notify;
        notify.code = IPM_Notify_ImportProjectFinished;
        notify.paramMap["name"] = loadProj->getProjectName();
        emitNotify(notify);
    }
    return loadProj;
}

bool ProjectManagerServer::ExportProject(const QString &targetDir, const QString &filePath)
{
    if (targetDir.isEmpty() || filePath.isEmpty())
        return false;

    // 判断filePath项目是否存在
    bool isExist = false;
    foreach (QString path, projects.keys()) {
        if (path == filePath) {
            isExist = true;
            break;
        }
    }

    if (!isExist) {
        LOGOUT(QString(tr("Project %1 does not exist")).arg(filePath), LOG_WARNING); // 项目%1不存在
        return false;
    }

    QString srcPath = DIR_PATH(filePath);
    // modify By 20231019 @txy 新增导出项目时不包含仿真结果
    QString tempSrcPath = srcPath + "-ExPortTemp";
    fileManager->copyDirectory(srcPath, tempSrcPath, true);
    if (QDir(tempSrcPath + "/" + KLProject::getSimulationDataDirName()).exists()) {
        fileManager->deleteDirectory(tempSrcPath + "/" + KLProject::getSimulationDataDirName());
    }

    QString dstFileName = setExportProjectPath(targetDir, BASE_NAME(filePath));
    if (QFile::exists(dstFileName)) {
        //"目标文件已存在，是否确认替换"
        if (KMessageBox::question(QObject::tr("The target file already exists. Are you sure to replace it!"),
                                  KMessageBox::Yes | KMessageBox::No)
            != KMessageBox::Yes) {
            return false;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    // 在这里添加压缩
    QString code;
#ifdef WIN32
    code = "system";
#else
    code = "GBK";
#endif
    bool flg = JlCompress::compressDir(dstFileName, tempSrcPath, true);

    fileManager->deleteDirectory(tempSrcPath);

    QApplication::restoreOverrideCursor();
    return flg;
}

QStringList ProjectManagerServer::GetAllProjectPath()
{
    QStringList projNameList;
    foreach (QString path, projects.keys()) {
        projNameList.append(path);
    }
    return projNameList;
}

void ProjectManagerServer::SetCurProject(PKLProject project)
{
    // 若传入的指针为空,视为取消激活项目
    if (project == nullptr) {
        // 取消激活,记录画板打开画板状态
        recordBoardOpenStatus();

        curProjectPath.clear();
        saveProjectPath();

        projectManager->switchProject("");

        NotifyStruct notify;
        notify.code = IPM_Notify_CurrentProjectDeactivate;
        emitNotify(notify);
        return;
    }

    if (!FindProject(project->getProjectPath())) {
        return;
    }

    if (curProjectPath != project->getProjectPath()) {
        // 记录原画板打开画板状态
        recordBoardOpenStatus();

        curProjectPath = project->getProjectPath();
        saveProjectPath();
        projectManager->switchProject(curProjectPath);

        // 恢复当前画板打开状态
        QTimer::singleShot(0, [this]() {
            // 这里需要等平台先关闭所有工作区画板后再打开记录的画板 所有将打开操作放在当前事件循环最后面
            recoverBoardOpenStatus();
            // 激活画板打开监控面板
            emit openMonitor();
        });

        // 切换项目若剪贴板有剪切标识则清除剪贴板,目的:只允许跨项目复制,不允许跨项目剪切
        auto clipBoard = QApplication::clipboard();
        if (clipBoard) {
            QByteArray mimaData = clipBoard->mimeData()->data("IS_CUT_OPERATION");
            bool isCut = false;
            if (!mimaData.isEmpty()) {
                QDataStream dataStream(&mimaData, QIODevice::ReadOnly);
                dataStream >> isCut;
            }
            if (isCut) {
                clipBoard->clear();
            }
        }

        NotifyStruct notify;
        notify.code = IPM_Notify_CurrentProjectChanged;
        notify.paramMap["projPath"] = project->getProjectPath();
        emitNotify(notify);
        LOGOUT(QString(tr("Project [%1] is activated")).arg(BASE_NAME(project->getProjectPath())),
               LOG_NORMAL); // 项目[%1]已激活
    }
}

PKLProject ProjectManagerServer::GetCurProject()
{
    if (curProjectPath.isEmpty() || !projects.contains(curProjectPath)) {
        return PKLProject();
    }

    return projects.value(curProjectPath);
}

PKLProject ProjectManagerServer::GetProject(const QString &projectPath)
{
    if (!FindProject(projectPath))
        return PKLProject();

    return projects.value(projectPath);
}

void ProjectManagerServer::UpdataProjectPrefix(const QString &sourcePrefixfDir, const QString &newPrefixfDir)
{
    // QString soureceName = BASE_NAME(sourcePrefixfDir);
    foreach (QString projPath, projects.keys()) {
        if (projPath.contains(sourcePrefixfDir)) {
            QString oldPath = projPath;
            QString newProjPath = projPath.replace(0, sourcePrefixfDir.length(), newPrefixfDir);
            projects[newProjPath] = projects[oldPath];
            projects[oldPath].clear();
            projects.remove(oldPath);

            projects[newProjPath]->updataProjectPrefix(sourcePrefixfDir, newPrefixfDir);
            // 修改 项目模型管理中的映射关系
            projectManager->modifyProject(oldPath, newProjPath);
        }
    }
    if (curProjectPath.contains(sourcePrefixfDir)) {
        curProjectPath.replace(sourcePrefixfDir, newPrefixfDir);
        saveProjectPath();
    }
}

bool ProjectManagerServer::RenamProjectName(PKLProject sourceProject, const QString &newProjectName)
{
    if (!sourceProject || sourceProject->getProjectPath().isEmpty()) {
        return false;
    }

    QString oldProjectPath = sourceProject->getProjectPath();
    QString oldProjectDir = DIR_PATH(oldProjectPath);
    QString sourcePrefixfDir = getProjectPrefix(oldProjectPath);
    QString newProjectPath = setProjectPathExt(sourcePrefixfDir, newProjectName);
    QString newProjectDir = DIR_PATH(newProjectPath);

    if (!projects.contains(oldProjectPath)) {
        LOGOUT(QString(tr("No project :%1, rename failed")).arg(oldProjectPath),
               LOG_WARNING); // 不存在项目:%1,重命名失败
        return false;
    }

    if (projects.contains(newProjectPath)) {
        LOGOUT(QString(tr("Existing project :%1, rename failed")).arg(newProjectPath),
               LOG_WARNING); // 已存在项目:%1,重命名失败
        return false;
    }

    // 重命名项目文件名
    QString newPath = oldProjectDir + "/" + newProjectName + "." + KLProject::getProjectFileSuffix();
    if (!fileManager->renameFile(oldProjectPath, newPath)) {
        return false;
    }
    // 重命名项目目录名
    QDir oldDir(DIR_PATH(oldProjectPath));
    if (!oldDir.rename(oldProjectDir, newProjectDir)) {
        return false;
    }

    projects[newProjectPath] = projects[oldProjectPath];
    projects[oldProjectPath].clear();
    projects.remove(oldProjectPath);

    projects[newProjectPath]->renameProject(newProjectPath);

    if (curProjectPath == oldProjectPath) {
        curProjectPath = newProjectPath;
        saveProjectPath();
    }
    // 修改 项目模型管理中的映射关系
    projectManager->modifyProject(oldProjectPath, newProjectPath);
    return true;
}

void ProjectManagerServer::LoadAllProjectFinished()
{
    QString activeProjectPath = readProjectPath();
    auto project = GetProject(activeProjectPath);
    SetCurProject(project);
}

void ProjectManagerServer::SaveProjectAllModel(QList<PKLProject> projectList)
{
    if (projectList.isEmpty()) {
        return;
    }

    QList<PKLProject> saveProjects;
    int iCurVersion = QString("1.0.5.0").remove(".").toInt();

    for (auto project : projectList) {
        if (!project) {
            continue;
        }
        int iGetVersion = 0;
        QString getVersion = project->getProjectProperty(KL_PRO::VERSION);
        if (getVersion.contains(KL_PRO::MAINVERSION_PREFIX)) {
            iGetVersion = QString(getVersion.remove(KL_PRO::MAINVERSION_PREFIX).remove(".") + "0").toInt();
        } else if (getVersion.contains(KL_PRO::RELEASEVERSION_PREFIX)) {
            iGetVersion = QString(getVersion.remove(KL_PRO::RELEASEVERSION_PREFIX).remove(".")).toInt();
        } else {
            // 适配 0.0.0这种格式
            iGetVersion = QString(getVersion.remove(".") + "0").toInt();
        }

        if (iGetVersion < iCurVersion) {
            saveProjects.append(project);
        }
    }

    if (saveProjects.size() > 0) {
        QWidget *widget = new QWidget();
        QLabel *labelStr = new QLabel(tr("Project model data saving...")); // 项目模型数据保存中...
        QLabel *labelMovie = new QLabel();
        QMovie *movie = new QMovie(LOAD_GIF_DIR + "/load.gif");
        labelMovie->setMovie(movie);
        labelMovie->setAlignment(Qt::AlignCenter);
        movie->start();
        QVBoxLayout *lyout = new QVBoxLayout(widget);
        lyout->addWidget(labelStr);
        lyout->addWidget(labelMovie);

        lyout->setAlignment(Qt::AlignCenter);
        KCustomDialog *dialogAboutView =
                new KCustomDialog(tr("Update project model data"), widget, KBaseDlgBox::NoButton,
                                  KBaseDlgBox::NoButton); // 更新项目模型数据
        dialogAboutView->setHiddenButtonGroup(true);
        dialogAboutView->setResizeAble(false);
        dialogAboutView->setMargin(1);
        dialogAboutView->resize(200, 150);

        auto saveThread = QThread::create([saveProjects]() {
            for (auto proj : saveProjects) {
                LOGOUT(QString(tr("%1 Version:%2"))
                               .arg(proj->getProjectName())
                               .arg(proj->getProjectProperty(KL_PRO::VERSION)),
                       LOG_NORMAL);
                for (auto model : proj->getAllModel()) {
                    proj->saveModel(model);
                }
                proj->setProjectProperty(KL_PRO::VERSION, gConfGet("SimuNPS_Version").toString());
            }
        });

        if (saveThread) {
            connect(saveThread, &QThread::finished, this, [dialogAboutView]() {
                dialogAboutView->close();
                QApplication::restoreOverrideCursor();
            });
            connect(saveThread, &QThread::finished, saveThread, &QThread::deleteLater);
            saveThread->start();
            QApplication::setOverrideCursor(Qt::WaitCursor);
            dialogAboutView->exec();
        }
    }
}

bool ProjectManagerServer::RenameBoardModel(PModel boardModel, const QString oldName, const QString newName)
{
    return false;
}

bool ProjectManagerServer::CheckNameIsvalid(const QString &name)
{
    if (Kcc::BlockDefinition::NPS_FileNameRule.indexIn(name) != 0) {
        return false;
    }
    return true;
}

QList<PModel> ProjectManagerServer::GetAllBoardModel(int modelType)
{
    auto curProject = GetCurProject();
    if (curProject) {
        if (modelType == -1) {
            QList<PModel> boardModelList;
            boardModelList = curProject->getAllBoardModel();
            // boardModelList.append(curProject->getAllModel(DrawingBoardClass::Type));
            // boardModelList.append(curProject->getAllModel(ElecBoardModel::Type));
            // boardModelList.append(curProject->getAllModel(ControlBoardModel::Type));
            // boardModelList.append(curProject->getAllModel(CombineBoardModel::Type));
            // boardModelList.append(curProject->getAllModel(ComplexBoardModel::Type));
            return boardModelList;
        } else { // 获取指定类型Model集合
            return curProject->getAllModel(modelType);
        }
    }
    return QList<PModel>();
}

PModel ProjectManagerServer::CreateBoardModel(const QString &boardName, int modelType)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return PModel();
    }

    if (!CheckNameIsvalid(boardName) || IsExistModel(boardName)) {
        return PModel();
    }

    PModel model = curProject->creatModel(boardName, modelType);
    if (model) {
        // 新建的画板默认是激活状态
        QVariantMap map;
        map[model->getUUID()] = true;
        curProject->setProjectConfig(KL_PRO::BOARD_ISACTIVATE, map);
    } else {
        return PModel();
    }

    // 发送创建本地构造型信号
    // if (modelType == CombineBoardModel::Type) {
    NotifyStruct notify;
    notify.code = IPM_Notify_DrawingBoardCreate;
    notify.paramMap["name"] = boardName;
    notify.paramMap["type"] = modelType;
    emitNotify(notify);
    //}

    return model;
}

bool ProjectManagerServer::DestroyBoardModel(PModel boardModel)
{
    if (!boardModel) {
        return false;
    }
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    bool ret = curProject->deleteModel(boardModel);
    // 销毁画板后,清除数据字典字典元素
    if (ret) {
        QString name = boardModel->getName();
        if (ProjectServerMng::getInstance().m_pDataDicServer) {
            ProjectServerMng::getInstance().m_pDataDicServer->DelDataDictionary(GetRealTimeSimulationName() + "."
                                                                                + name);
        }
    } else {
        return ret;
    }

    // 发送删除画板模型通知
    if (curProject->isBoardType(boardModel->getModelType())) {
        NotifyStruct notify;
        notify.code = IPM_Notify_DrawingBoardDestroyed;
        notify.paramMap["name"] = boardModel->getName();
        notify.paramMap["type"] = boardModel->getModelType();
        emitNotify(notify);
    }

    // 项目中自定义控制型
    if (boardModel->getModelType() == ControlBlock::Type) {
        NotifyStruct notify;
        notify.code = IPM_Notify_CodePrototypeDestroyed;
        notify.paramMap["name"] = boardModel->getName();
        emitNotify(notify);
    }

    // 项目配置移除画板激活信息
    QVariantMap map;
    map[boardModel->getUUID()];
    curProject->setProjectConfig(KL_PRO::BOARD_ISACTIVATE, map, true);
    return ret;
}

void ProjectManagerServer::ConfirmUserDefinedBoardModel(const QString &boardName)
{
    // emitNotify(NotifyStruct(IMD_Notify_CombinedPrototypeChanged));
}

bool ProjectManagerServer::SaveBoardModel(PModel boardModel, bool isLog)
{
    if (!boardModel) {
        return false;
    }
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    bool ret = curProject->saveModel(boardModel, isLog);

    if (!ret) {
        return ret;
    }
    NotifyStruct notify;
    notify.code = IPM_Notify_DrawingBoardSaved;
    notify.paramMap["name"] = boardModel->getName();
    notify.paramMap["type"] = boardModel->getModelType();
    emitNotify(notify);

    if (boardModel->getModelType() == ElecCombineBoardModel::Type) {
        NotifyStruct notify;
        notify.code = IPM_Notify_CreateElecCombineBoardSave;
        notify.paramMap["name"] = boardModel->getName();
        notify.paramMap["type"] = boardModel->getModelType();
        emitNotify(notify);
    }

    return ret;
}

bool ProjectManagerServer::ReloadBoardModel(PModel boardModel)
{
    if (!boardModel) {
        return false;
    }
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    return curProject->reloadModel(boardModel);
}

PModel ProjectManagerServer::GetBoardModelByName(const QString &boardName)
{
    auto boardList = GetAllBoardModel();
    foreach (auto board, boardList) {
        if (board->getName() == boardName) {
            return board;
        }
    }
    return PModel();
}

PModel ProjectManagerServer::GetBoardModelByUUID(const QString &uuid)
{
    auto boardList = GetAllBoardModel();
    foreach (auto board, boardList) {
        if (board->getUUID() == uuid) {
            return board;
        } else if (board->findChildModelRecursive(uuid)) {
            return board->findChildModelRecursive(uuid);
        }
    }
    return PModel();
}

bool ProjectManagerServer::IsExistModel(const QString &modelName, DuplicateChecks checkType)
{
    if (ProjectServerMng::getInstance().m_pModelManagerServer) {
        // 检测是否为原始模型（如sin、电机等，不包含构造原型）
        if (checkType.testFlag(DuplicateCheck_ToolBox)) {
            QString toolkitName;
            if (ProjectServerMng::getInstance().m_pModelManagerServer->ContainToolkitModel(modelName, toolkitName)) {
                return true;
            }
        }
    }

    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    // 检测是否为项目内模型
    if (checkType.testFlag(DuplicateCheck_CurProject)) {
        if (curProject->checkModelNameIsExist(modelName)) {
            return true;
        }
    }

    // 检测是否为系统内建模型
    if (checkType.testFlag(DuplicateCheck_SystemBuildIn)) {
        if (getBuiltinProject()->checkModelNameIsExist(modelName)) {
            return true;
        }
    }

    return false;
}

QStringList ProjectManagerServer::GetActiveBoardModelNames()
{
    auto curProject = GetCurProject();
    if (curProject) {
        return curProject->getActiveBoardModelNameList();
    }
    return QStringList();
}

PModel ProjectManagerServer::CreateBlockModel(const QString &prototypeName)
{
    if (ProjectServerMng::getInstance().m_pModelManagerServer) {
        // 检测是否为原始模型（如sin、电机等，不包含构造原型）
        PModel model = ProjectServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(prototypeName);
        if (model) {
            PModel blockModel = model->copy(); // 创建原始模型的副本模型
            return blockModel;
        }
    }

    auto curProject = GetCurProject();
    if (!curProject) {
        return PModel();
    }

    // 检测是否为项目内模型
    auto proModel = curProject->getAllModel(ControlBlock::Type);
    proModel.append(curProject->getAllModel(CombineBoardModel::Type));
    proModel.append(curProject->getAllModel(ElecCombineBoardModel::Type));
    foreach (auto model, proModel) {
        if (model->getPrototypeName() == prototypeName) {
            auto connectFunc = [](ModelCopyStrategy *strategy, const QList<PAssociate> &associates) -> void {
                if (strategy->getNewModel()->getModelType() == ElectricalBlock::Type) {
                    for (auto ass : associates) {
                        // 只处理 设备类型的关联关系
                        if (ass->type == ModelConnDeviceType) {
                            ModelSystem::getInstance()->getProjectManager()->addAssociate(
                                    strategy->getNewModel(), ass->target, ass->type, ass->mirrored);
                        }
                    }
                }
            };

            PModel blockModel = model->copy(connectFunc); // 创建原始模型的副本模型  ,!!!不分离项目内模型和副本
            return blockModel;
        }
    }

    return PModel();
}

bool ProjectManagerServer::AddSystemCombineBoardModel(PCombineBoardModel combineModel)
{
    return getBuiltinProject()->saveModel(combineModel);
}

bool ProjectManagerServer::DeleteSystemCombineBoardModel(PCombineBoardModel combineModel)
{
    return getBuiltinProject()->deleteModel(combineModel);
}

QList<PCombineBoardModel> ProjectManagerServer::GetSystemCombineBoardModels()
{
    QList<PCombineBoardModel> retCombineBoardModel;
    auto allSysCombineModel = getBuiltinProject()->getAllModel(CombineBoardModel::Type);

    foreach (auto model, allSysCombineModel) {
        auto combineModel = model.dynamicCast<CombineBoardModel>();
        if (combineModel) {
            retCombineBoardModel.append(combineModel);
        }
    }

    return retCombineBoardModel;
}

bool ProjectManagerServer::AddControlModel(PModel model)
{
    if (!model) {
        return false;
    }
    auto curProject = GetCurProject();
    if (curProject) {
        return curProject->saveModel(model);
    }
    return false;
}

bool ProjectManagerServer::DeleteControlModel(PModel model)
{
    if (!model) {
        return false;
    }
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    // 项目中自定义控制型
    if (model->getModelType() == ControlBlock::Type) {
        NotifyStruct notify;
        notify.code = IPM_Notify_CodePrototypeDestroyed;
        notify.paramMap["name"] = model->getName();
        emitNotify(notify);
    }

    return curProject->deleteModel(model);
}

bool ProjectManagerServer::SaveControlModel(PModel model)
{
    if (!model) {
        return false;
    }
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    return curProject->saveModel(model);
}

PModel ProjectManagerServer::GetControlModel(const QString &modelName)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return PModel();
    }

    return curProject->getModel(modelName);
}

QList<PModel> ProjectManagerServer::GetAllControlModel()
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return QList<PModel>();
    }

    return curProject->getAllModel(ControlBlock::Type);
}

bool ProjectManagerServer::AddDeviceModel(PDeviceModel deviceModel)
{
    if (!deviceModel) {
        return false;
    }
    auto curProject = GetCurProject();
    if (curProject) {
        return curProject->saveModel(deviceModel);
    }
    return false;
}

bool ProjectManagerServer::DeleteDeviceModel(PDeviceModel deviceModel)
{
    if (!deviceModel) {
        return false;
    }
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }

    NotifyStruct notify;
    notify.code = IPM_Notify_DeviceTypeDestroyed;
    notify.paramMap["name"] = deviceModel->getName();
    emitNotify(notify);

    return curProject->deleteModel(deviceModel);
}

bool ProjectManagerServer::ModifyDeviceModel(const QString &oldName, PDeviceModel deviceModel)
{
    if (!deviceModel) {
        return false;
    }
    auto curProject = GetCurProject();
    if (curProject) {
        if (oldName != deviceModel->getName()) { // 重命名
            curProject->renameModel(deviceModel, oldName, deviceModel->getName());
        }

        return curProject->saveModel(deviceModel);
    }
    return false;
}

QList<PDeviceModel> ProjectManagerServer::GetDeviceModels(const QString &prototypeName)
{
    QList<PDeviceModel> retDeviceModels;

    auto curProject = GetCurProject();
    if (!curProject) {
        return retDeviceModels;
    }
    auto allDeviceModels = curProject->getAllModel(DeviceModel::Type);
    foreach (auto model, allDeviceModels) {
        auto deviceModel = model.dynamicCast<DeviceModel>();
        if (!deviceModel) {
            continue;
        }
        if (deviceModel->getDeviceTypePrototypeName() == prototypeName) {
            retDeviceModels.append(deviceModel);
        }
    }

    return retDeviceModels;
}

QMap<QString, QList<PDeviceModel>> ProjectManagerServer::GetDeviceModels()
{
    QMap<QString, QList<PDeviceModel>> retDeviceModelMap;

    auto curProject = GetCurProject();
    if (!curProject) {
        return retDeviceModelMap;
    }
    auto allDeviceModels = curProject->getAllModel(DeviceModel::Type);
    foreach (auto model, allDeviceModels) {
        auto deviceModel = model.dynamicCast<DeviceModel>();
        if (!deviceModel) {
            continue;
        }
        QString prototypeName = deviceModel->getDeviceTypePrototypeName();
        if (!retDeviceModelMap.contains(prototypeName)) {
            QList<PDeviceModel> listDeviceModel;
            listDeviceModel.append(deviceModel);
            retDeviceModelMap[prototypeName] = listDeviceModel;
        } else {
            retDeviceModelMap[prototypeName].append(deviceModel);
        }
    }

    return retDeviceModelMap;
}

bool ProjectManagerServer::HaveDeviceModel(const QString &prototypeName)
{
    QString strPN = prototypeName;
    if (strPN == "PiTypeTransmissionLinkLine") {
        strPN = "PiTypeTransmissionLine";
    }
    if (GetSystemDeviceModel(strPN) != nullptr) {
        return true;
    }
    return false;
}

PDeviceModel ProjectManagerServer::GetSystemDeviceModel(const QString &prototypename)
{
    if (prototypename.isEmpty()) {
        return PDeviceModel(nullptr);
    }
    QList<PDeviceModel> sysdevices = GetSystemDeviceModels();
    for (PDeviceModel pdevmod : sysdevices) {
        if (pdevmod != nullptr && prototypename == pdevmod->getDeviceTypePrototypeName()) {
            return pdevmod;
        }
    }
    return PDeviceModel(nullptr);
}

QList<PDeviceModel> ProjectManagerServer::GetSystemDeviceModels()
{
    QList<PDeviceModel> retDevModel;
    auto allSysDevModel = ModelSystem::getInstance()->getSystemDeviceModels();

    foreach (auto model, allSysDevModel) {
        auto devModel = model.dynamicCast<DeviceModel>();
        if (devModel) {
            retDevModel.append(devModel);
        }
    }

    return retDevModel;
}

QColor ProjectManagerServer::GetColorOfVolLevel(const double &voltageLevel)
{
    QColor color;
    if (voltageLevel <= 1.0) {
        QVariantList listdata = GetCurProject()->getProjectConfig(KL_PRO::PFSET_LOWERLIMIT).toList();
        if (listdata.isEmpty())
            return color;
        if (voltageLevel <= listdata.at(4).toDouble()) {
            color = listdata.at(5).value<QColor>();
        } else if (voltageLevel <= listdata.at(2).toDouble()) {
            color = listdata.at(3).value<QColor>();
        } else {
            color = listdata.at(1).value<QColor>();
        }
    } else {
        QVariantList listdata = GetCurProject()->getProjectConfig(KL_PRO::PFSET_UPPERLIMIT).toList();
        if (listdata.isEmpty())
            return color;
        if (voltageLevel >= listdata.at(4).toDouble()) {
            color = listdata.at(5).value<QColor>();
        } else if (voltageLevel >= listdata.at(2).toDouble()) {
            color = listdata.at(3).value<QColor>();
        } else {
            color = listdata.at(1).value<QColor>();
        }
    }
    return color;
}

QVariant ProjectManagerServer::GetProjectConfig(const QString &key)
{
    auto curProject = GetCurProject();
    if (curProject) {
        return curProject->getProjectConfig(key);
    }

    return QVariant();
}

void ProjectManagerServer::SetProjectConfig(const QString &key, const QVariant &value, bool isDelete)
{
    auto curProject = GetCurProject();
    if (curProject) {
        return curProject->setProjectConfig(key, value, isDelete);
    }
}

QString ProjectManagerServer::GetCurrentProjectAbsoluteDir()
{
    auto curProject = GetCurProject();
    if (curProject) {
        return curProject->getProjectDir();
    }
    return QString();
}

QString ProjectManagerServer::GetCurrentProjectRelativeDir()
{
    auto curProject = GetCurProject();
    if (curProject) {
        QStringList pathlist = QFileInfo(curProject->getProjectPath()).path().split("/");
        if (pathlist.size() >= 2) {
            return pathlist[pathlist.size() - 2] + "/" + pathlist[pathlist.size() - 1];
        }

        return QFileInfo(curProject->getProjectPath()).path();
    }

    return QString();
}

QString ProjectManagerServer::GetModelPrototypeNameCHS(const QString &prototypeName)
{
    if (ProjectServerMng::getInstance().m_pModelManagerServer) {
        PModel model = ProjectServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(prototypeName);
        if (model) {
            return model->getPrototypeName_CHS();
        }
    }

    return QString();
}

QString ProjectManagerServer::GetRealTimeSimulationName()
{
    return NPS_DataDictionaryNodeName;
}

QString ProjectManagerServer::GetSimulationStorageName()
{
    return CURRENTSIMULATION;
}

QString ProjectManagerServer::GetCodeModelDlldir()
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return QString();
    }
    return curProject->getDlldir();
}

void ProjectManagerServer::onBoardActiveStatusChange(QVariant activeStatus)
{
    NotifyStruct notify;
    notify.code = IPM_Notify_ActivationStateChanged;
    notify.paramMap = activeStatus.toMap();
    emitNotify(notify);
}

void ProjectManagerServer::onConfigInfoChange(const QString &key, const QVariant &value)
{
    NotifyStruct notify;
    notify.code = IPM_Notify_ConfigInfoChange;
    notify.paramMap["key"] = key;
    notify.paramMap["value"] = value;
    emitNotify(notify);
}

void ProjectManagerServer::onBoardLabelChange(const QVariant &value)
{
    NotifyStruct notify;
    notify.code = IPM_Notify_BoardLabelChange;
    notify.paramMap = value.toMap();
    emitNotify(notify);
}

void ProjectManagerServer::onRecordResult(const QString &recordName, QVector<ParamIdentifier> params)
{
    if (!ProjectServerMng::getInstance().m_pDataDicServer) {
        return;
    }
    for (auto param : params) {
        ProjectServerMng::getInstance().m_pDataDicServer->AddDataDictionary(recordName + "." + param.getParamNameStr());
    }
}

void ProjectManagerServer::onRenameResult(const QString &sourceName, const QString &newName,
                                          QVector<ParamIdentifier> params)
{
    if (!ProjectServerMng::getInstance().m_pDataDicServer) {
        return;
    }

    ProjectServerMng::getInstance().m_pDataDicServer->RenameDictionaryElement(sourceName, newName);
}

void ProjectManagerServer::onDeleteResult(const QString &sourceName, QVector<ParamIdentifier> params)
{
    if (!ProjectServerMng::getInstance().m_pDataDicServer) {
        return;
    }

    ProjectServerMng::getInstance().m_pDataDicServer->DelDataDictionary(sourceName);
}

void ProjectManagerServer::onLocalCodeBlockChange(const QString &ModelName)
{
    // 自定义代码型
    NotifyStruct notify;
    notify.code = IPM_Notify_CodePrototypeSaved;
    notify.paramMap["name"] = ModelName;
    emitNotify(notify);
}

void ProjectManagerServer::onCopyModelFinished()
{
    // 复制Model已经完成
    NotifyStruct notify;
    notify.code = IPM_Notify_CopyModelFinished;
    emitNotify(notify);
}

void ProjectManagerServer::onBoardModelRename(QVariantMap info)
{
    QString oldName = info.value("oldName").toString();
    QString newName = info.value("newName").toString();
    QString uuid = info.value("modelUUID").toString();
    if (oldName.isEmpty() || newName.isEmpty()) {
        return;
    }

    // 发送画板模型名称重命名通知
    NotifyStruct notify;
    notify.code = IPM_Notify_DrawingBoardFileRenamed;
    notify.paramMap["oldName"] = oldName;
    notify.paramMap["newName"] = newName;
    notify.paramMap["uuid"] = uuid;
    emitNotify(notify);
}

void ProjectManagerServer::onCombineModelSync(QList<QWeakPointer<Model>> models)
{
    QList<PModel> boardModels;
    for (auto model : models) {
        if (model) {
            auto topModel = model.toStrongRef()->getTopParentModel();
            if (!boardModels.contains(topModel)) {
                boardModels << topModel;
            }
        }
    }
    if (boardModels.size() > 0) {
        if (ProjectServerMng::getInstance().m_GraphicModelServer) {
            ProjectServerMng::getInstance().m_GraphicModelServer->VerifyStatus(boardModels);
        }
    }
}

QString ProjectManagerServer::setProjectPath(QString prefixDir, QString projName)
{
    QString projectPath = setProjectPathExt(prefixDir, projName);
    QString projectDir = DIR_PATH(projectPath);
    if (!QDir(projectDir).exists()) {
        KLFileManager *fileManager = new KLFileManager();
        fileManager->creatDirectory(projectDir);
        delete fileManager;
    }
    return projectPath;
}

QString ProjectManagerServer::setProjectPathExt(QString prefixDir, QString projName)
{
    QString projectDir = prefixDir + "/" + projName;
    QString projectPath = projectDir + "/" + projName + "." + KLProject::getProjectFileSuffix();
    return projectPath;
}

QString ProjectManagerServer::getProjectPrefix(const QString &projectPath)
{
    QString relativePath = QString("/%1/%1.%2").arg(BASE_NAME(projectPath)).arg(KLProject::getProjectFileSuffix());
    QString tempPath = projectPath;
    return tempPath.remove(relativePath);
}

QString ProjectManagerServer::setExportProjectPath(QString prefixDir, QString name)
{
    QString path = prefixDir + "/" + name + "." + EXPORTPROJECT_SUFFIX;
    return path;
}
void ProjectManagerServer::saveProjectPath()
{
    QString path = CURRENT_PROJECT_PATH + "/当前项目.npspj";
    QString relativePath = fileManager->getRelativeFilePath(SIMUNPS_WORKSPACE_PATH, curProjectPath);
    fileManager->writeToFile(path, relativePath);
}

QString ProjectManagerServer::readProjectPath()
{
    QString path = CURRENT_PROJECT_PATH + "/当前项目.npspj";
    if (!QDir(CURRENT_PROJECT_PATH).exists()) {
        if (!QDir().mkdir(CURRENT_PROJECT_PATH)) {
            LOGOUT(QString(tr("Internal program exception, unable to create directory :%1")).arg(CURRENT_PROJECT_PATH),
                   LOG_ERROR); // 程序内部异常,无法创建目录:%1
            return QString();
        }
    }

    QFile tmpFile(path);
    if (!tmpFile.open(QIODevice::ReadWrite)) {
        return QString();
    }

    QString getFilePath;
    fileManager->readFromFile(path, getFilePath);

    // 旧的文件中存的是绝对路径,若包含绝对路径前缀则清空curProjectPath
    if (getFilePath.contains(SIMUNPS_WORKSPACE_PATH)) {
        return QString();
    } else {
        return SIMUNPS_WORKSPACE_PATH + "/" + getFilePath;
    }
}

void ProjectManagerServer::bindSignal(PKLProject project)
{
    QObject::connect(project.data(), SIGNAL(boardActiveStatusChange(QVariant)), this,
                     SLOT(onBoardActiveStatusChange(QVariant)));
    QObject::connect(project.data(), SIGNAL(configInfoChange(const QString &, const QVariant &)), this,
                     SLOT(onConfigInfoChange(const QString &, const QVariant &)));
    QObject::connect(project.data(), SIGNAL(boardLabelChange(const QVariant &)), this,
                     SLOT(onBoardLabelChange(const QVariant &)));
    QObject::connect(project.data(), SIGNAL(recordResult(const QString &, QVector<ParamIdentifier>)), this,
                     SLOT(onRecordResult(const QString &, QVector<ParamIdentifier>)));
    QObject::connect(project.data(), SIGNAL(renameResult(const QString &, const QString &, QVector<ParamIdentifier>)),
                     this, SLOT(onRenameResult(const QString &, const QString &, QVector<ParamIdentifier>)));
    QObject::connect(project.data(), SIGNAL(deleteResult(const QString &, QVector<ParamIdentifier>)), this,
                     SLOT(onDeleteResult(const QString &, QVector<ParamIdentifier>)));
    QObject::connect(project.data(), SIGNAL(createLocalCodeBlock(const QString &)), this,
                     SLOT(onLocalCodeBlockChange(const QString &)));
    QObject::connect(project.data(), SIGNAL(copyModelFinished()), this, SLOT(onCopyModelFinished()));
    QObject::connect(project.data(), SIGNAL(boardModelRename(QVariantMap)), this,
                     SLOT(onBoardModelRename(QVariantMap)));
    QObject::connect(project.data(), SIGNAL(combineModelSync(QList<QWeakPointer<Model>>)), this,
                     SLOT(onCombineModelSync(QList<QWeakPointer<Model>>)));
}

void ProjectManagerServer::initBoardStatus(PModel model)
{
    PCanvasContext canvasContext = model->getCanvasContext();
    if (canvasContext) {
        // 画板默认冻结状态
        canvasContext->setLocked(true);
        // 默认显示导航窗
        canvasContext->setShowNavigationPreview(true);
        // 画板背景默认点阵网格
        canvasContext->setGridFlag(CanvasContext::kGridShowDotGrid);
        // 缩放比例默认100%
        canvasContext->setScale(1);
        // 中心位置坐标默认左上
        canvasContext->setCenterPos(QPointF(0, 0));
        // 设置连接线颜色默认黑色
        canvasContext->setLineColor(QColor(GKD::WIRE_NORMAL_COLOR));
    }
    // 遍历子Model，若为构造画板，递归调用
    for (PModel childModel : model->getChildModels()) {
        if (childModel->getModelType() == CombineBoardModel::Type) {
            initBoardStatus(childModel);
        }
    }
}

PKLProject ProjectManagerServer::getBuiltinProject()
{
    return BuiltinModelManager::getInstance().getBuiltinProject();
}
void ProjectManagerServer::buildDDSstructure()
{
    if (!ProjectServerMng::getInstance().m_pDataDicServer) {
        return;
    }

    PKLProject curProject = GetCurProject();
    if (!curProject) {
        return;
    }

    // 只构建当前项目的仿真结果
    QList<SimulationResultInfo> resultList = curProject->getAllSimulationDataInfo();

    bool isEmpty = true;
    // 添加记录过的仿真结果变量至数据字典
    for (SimulationResultInfo result : resultList) {
        if (result.name == CURRENTSIMULATION) {
            continue;
        }
        for (ParamIdentifier param : result.params) {
            ProjectServerMng::getInstance().m_pDataDicServer->AddDataDictionary(
                    result.name + "." + param.board_name + "." + param.block_name + "." + param.port_name);
            isEmpty = false;
        }
    }

    // 如果当前没有任何保存的仿真结果，需要一个临时结果以初始化数据字典
    // 除非数据字典确实不需要这样做了，否则不要轻易移除此代码
    if (isEmpty) {
        ProjectServerMng::getInstance().m_pDataDicServer->AddDataDictionary("仿真结果2.画板1.测试模块1.端口xxxx");
        ProjectServerMng::getInstance().m_pDataDicServer->DelDataDictionary("仿真结果2");
    }
}

bool ProjectManagerServer::checkProjectIsStale(const QString &projePath)
{
    if (projePath.isEmpty()) {
        return true;
    }

    QString projDir = DIR_PATH(projePath);
    QDir dir(projDir);
    if (!dir.exists()) {
        return true;
    }
    // 判断条件,是否包含有DrawingBoard、UserDefined
    QStringList files = dir.entryList();
    if (files.contains(KL_PRO::OLD_DRAWBOARD_FILE) || files.contains(KL_PRO::OLD_BLOCK_FILE)) {
        // 删除文件夹
        fileManager->deleteDirectory(projDir);
        LOGOUT(tr("The project file has expired and cannot be imported"),
               LOG_WARNING); // 项目文件已过期,无法导入
        return true;
    }
    return false;
}

void ProjectManagerServer::saveProject(PKLProject project)
{
    if (!project) {
        return;
    }

    // LOGOUT(QString(tr("%1 Version:%2"))
    //                .arg(project->getProjectName())
    //                .arg(project->getProjectProperty(KL_PRO::VERSION)),
    //        LOG_NORMAL);
    for (auto model : project->getAllModel()) {
        project->saveModel(model, false);
    }

    project->setProjectProperty(KL_PRO::VERSION, gConfGet("SimuNPS_Version").toString());
}

void ProjectManagerServer::recordBoardOpenStatus()
{
    if (!GetCurProject() || !ProjectServerMng::getInstance().m_GraphicModelServer) {
        return;
    }
    auto openBoards = ProjectServerMng::getInstance().m_GraphicModelServer->getAllOpenBoardUUID();
    // 不属于该项目的画板不记录(例如打开了系统画板)
    for (auto uuid : openBoards) {
        if (!GetCurProject()->findModel(uuid)) {
            openBoards.removeAll(uuid);
        }
    }

    if (GetCurProject()->getProjectConfig(KL_PRO::BOARD_OPEN_STATUS).toStringList() != openBoards) {
        GetCurProject()->setProjectConfig(KL_PRO::BOARD_OPEN_STATUS, openBoards);
    }
}

void ProjectManagerServer::recoverBoardOpenStatus()
{
    if (!GetCurProject() || !ProjectServerMng::getInstance().m_GraphicModelServer) {
        return;
    }
    auto uuids = GetCurProject()->getProjectConfig(KL_PRO::BOARD_OPEN_STATUS).toStringList();
    for (auto uuid : uuids) {
        ProjectServerMng::getInstance().m_GraphicModelServer->openDrawingBoard(GetCurProject()->findModel(uuid));
    }
}
PKLProject ProjectManagerServer::ImportDemoProject(const QString &targetDir, const QString &filePath,
                                                   const QString &targetProjectName, KLProject::ProjectMode prjModel)
{
    QString tarDir = targetDir;

#ifdef COMPILER_PRODUCT_DESIGNER
    PKLProject curProject;
    QString curProjectDir, oldName, importName;
    if (KLProject::ProjectMode::ProjectMode_DesignerExterrnal == prjModel) {
        curProject = GetCurProject();
        if (!curProject) {
            return PKLProject();
        }
        curProjectDir = curProject->getProjectDir();
        tarDir = DIR_PATH(curProjectDir);
        oldName = BASE_NAME(curProject->getProjectPath());
        importName = targetProjectName;
    }
#endif

    QString targetProjectPath = setProjectPath(tarDir, targetProjectName);
    if (FindProject(targetProjectPath)) {
        LOGOUT(QString(tr("Failed to import example project, already exists :%1")).arg(targetProjectPath),
               LOG_WARNING); // 导入示例项目失败,已经存在项目:%1
        return PKLProject();
    }

    // 拷贝
    int i = 0;
    QString demoProject = DIR_PATH(filePath);
    QString targetProjectDir = DIR_PATH(targetProjectPath);
    bool copied = fileManager->copyDirectory(demoProject, targetProjectDir, true);

    if (!copied) {
        LOGOUT(tr("Error copying file"), LOG_ERROR); // 复制文件出错
        fileManager->deleteDirectory(targetProjectDir);
        return PKLProject();
    }

    QString sourceProjectBaseName = BASE_NAME(filePath);
    if (sourceProjectBaseName != targetProjectName) {
        QFile file(targetProjectDir + "/" + sourceProjectBaseName + "." + KLProject::getProjectFileSuffix());
        if (!file.rename(targetProjectPath)) {
            fileManager->deleteDirectory(targetProjectDir);
            return PKLProject();
        }
    }

#ifdef COMPILER_PRODUCT_DESIGNER
    if (KLProject::ProjectMode::ProjectMode_DesignerExterrnal == prjModel) {
        // 删除原项目
        fileManager->deleteDirectory(curProjectDir);
        DestoryProject(curProject);
        // 重命名为原项目
        QString tempPath = DIR_PATH(targetProjectPath) + "/" + oldName + "." + KLProject::getProjectFileSuffix();
        QFile(targetProjectPath).rename(tempPath);
        QString tempDir = tarDir + "/" + oldName;
        QDir().rename(tarDir + "/" + importName, tempDir);

        targetProjectPath = tempDir + "/" + oldName + "." + KLProject::getProjectFileSuffix();
    }
#endif

    // 加载导入的项目
    auto loadProj = LoadProject(targetProjectPath);
    if (loadProj) {
        saveProject(loadProj);
        // // 导入的项目版本若是低于1.0.5,则保存项目内所有模型
        // SaveProjectAllModel(QList<PKLProject>() << loadProj);
        // 初始化示例项目中画板的实时数据
        loadProj->setProjectProperty(KL_PRO::CREATE_TIME, QDateTime::currentDateTime().toString(KL_PRO::DATE_FORMAT));
        initProjectBoardStatus(loadProj);
    }

    if (loadProj) {
        // 导入项目处理完成,发送项目导入完成信号
        NotifyStruct notify;
        notify.code = IPM_Notify_ImportProjectFinished;
        notify.paramMap["name"] = loadProj->getProjectName();
        emitNotify(notify);
    }
    return loadProj;
}

void ProjectManagerServer::initProjectBoardStatus(PKLProject project)
{
    if (project) {
        for (auto board : project->getAllBoardModel()) {
            initBoardStatus(board);
            project->saveModel(board, false);
        }
    }
}

bool ProjectManagerServer::CreateAndRunDataDictionary()
{
    if (!ProjectServerMng::getInstance().m_pDataDicServer) {
        return false;
    }
    buildDDSstructure();
    ProjectServerMng::getInstance().m_pDataDicServer->RunDataDictionary();
    //  startVariableOutputThread();

    return true;
}

void ProjectManagerServer::UpdateBoardDataDictionary(PModel boardModel)
{
    if (!boardModel) {
        return;
    }

    NotifyStruct notify;
    notify.code = IPM_Notify_RefreshDatadictionary;
    notify.paramMap["boardUuid"] = boardModel->getObjectUuid();
    emitNotify(notify);
}

bool ProjectManagerServer::CreateSimulationVariableList(const QString &storageName, const QString &simulationIdentifier,
                                                        const QVector<ParamIdentifier> &allParamIds)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }
    return curProject->createSimulationResultStorage(storageName, simulationIdentifier, allParamIds);
}

bool ProjectManagerServer::ClearLastSimulationData(const QString &storageName)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }
    return curProject->clearLastSimulationData(storageName);
}

QSharedPointer<QMap<QString, QMap<QString, QMap<quint64, double>>>>
ProjectManagerServer::GetAllSimulationResultData(const QString &storageName)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return nullptr;
    }
    return curProject->readAllSimulationData(storageName);
}

bool ProjectManagerServer::AppendSimulationVariableList(const QVector<ParamIdentifier> &appendParamIds)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }
    return curProject->appendSimulationVariableList(appendParamIds);
}

bool ProjectManagerServer::WriteSimulationVariableBlockValue(const QString &simulationIdentifier,
                                                             const QVector<ParamIdentifier> &allParamIds,
                                                             const QVector<quint64> &timeList,
                                                             const QVector<QVector<double>> &variableBlock)
{
    auto curProject = GetCurProject();
    if (!curProject) {
        return false;
    }
    if (timeList.size() != variableBlock.size() || timeList.isEmpty() || variableBlock.isEmpty()) {
        return false;
    }
    if (!ProjectServerMng::getInstance().m_pDataDicServer) {
        LOGOUT(tr("Failed to open the data dictionary"), LOG_ERROR); // 数据字典打开失败
        return false;
    }
    int colorder = 0;
    // 发送至数据字典
    foreach (ParamIdentifier paramId, allParamIds) {
        QString variableName = paramId.getParamNameStr();
        QString name = NPS_DataDictionaryNodeName + "." + variableName;
        QString nameDic = "Dictionary." + name;
        if (!ProjectServerMng::getInstance().m_pDataDicServer->isHaveDictionaryElement(name))
            return false;

        const int count = timeList.size();
        double *alldata = new double[count];
        quint64 firstTime = timeList[0];
        quint64 space = 1.0;
        int roworder = 0;
        foreach (quint64 time, timeList) {
            if (roworder == 0)
                firstTime = time;
            if (roworder == 1)
                space = time - firstTime;
            const double &vardata = variableBlock[roworder][colorder];
            alldata[roworder] = vardata;
            ++roworder;
        }
        if (ProjectServerMng::getInstance().m_pDataDicServer->isHaveDictionaryElement(name)) {
            ProjectServerMng::getInstance().m_pDataDicServer->InputChannelData(name, alldata, count, firstTime, space);
        }
        delete[] alldata;

        colorder++;
    }
    return curProject->writeSimulationData(simulationIdentifier, allParamIds, timeList, variableBlock);
}

void ProjectManagerServer::WriteSimulationVariableFinished(const QString &simulationIdentifier)
{
    auto curProject = GetCurProject();
    if (curProject) {
        curProject->writeSimulationDataFinished(simulationIdentifier);
        NotifyStruct notify;
        notify.code = IPM_Notify_SimulationResultDataWriteFinished;
        emitNotify(notify);
    }
}

void ProjectManagerServer::OpenQUIKIS(const QString filePath)
{
    QString registryPath = "HKEY_CURRENT_USER\\SOFTWARE\\Keliang\\QuiKIS2.0";
    QString quickisPath = QSettings(registryPath, QSettings::NativeFormat).value("applicationPath").toString();
    QString exePath = quickisPath + "QUiKIS.exe";
    QFile file(exePath);

    if (quickisPath.isEmpty() || !file.exists()) {
        LOGOUT(tr("QuiKIS not installed"), LOG_ERROR);
        return;
    }

    if (!filePath.isEmpty()) {
        QStringList arguments;
        arguments << "load" << filePath;
        QProcess::startDetached(exePath, arguments);
    }
}

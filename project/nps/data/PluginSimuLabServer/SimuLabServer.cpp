#include "SimuLabServer.h"
#include "CoreLib/ServerManager.h"
#include "GenerateMapSchema.h"
#include "SplitModelTools.h"
#include "ToolsLib/Json/json.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLibrary>

using namespace Kcc::CoreManUI_Layout;
USE_LOGOUT_("SimuLabServer")

SimuLabServer::SimuLabServer()
{
    pPrjMgr = RequestServer<IProjectManagerServer>();
    subStepMap.clear();
}

SimuLabServer::~SimuLabServer()
{
    pPrjMgr.clear();
}

void SimuLabServer::Init()
{
    pCtrlSysCodeMgr = RequestServer<ICtrlSysCodeMgr>();
    if (!pCtrlSysCodeMgr) {
        LOGOUT(QObject::tr("ICtrlSysCodeMgr未注册"), LOG_ERROR);
    }
}

void SimuLabServer::UnInit()
{
    pCtrlSysCodeMgr.clear();
}

QString SimuLabServer::getConfig()
{
    Json::Value root;
    QMapIterator<QString, double> iter(subStepMap);
    while (iter.hasNext()) {
        iter.next();
        QString key = iter.key();
        double step = iter.value();
        Json::Value item;
        item[key.toStdString()] = step;
        root[SL_CONST::StepJsonKeyName].append(item);
    }
    return QString::fromStdString(root.toStyledString());
}

bool SimuLabServer::splitModel(const QString &dirPath, QString &errMsg)
{
    PKLProject proj = nullptr;
    QList<PDrawingBoardClass> drawingBoardList;
    if (!getProjectInfo(dirPath, proj, drawingBoardList)) {
        errMsg = QObject::tr("解析项目文件失败");
        return false;
    }

    QMap<QString, int> modelList;
    PDrawingBoardClass topBoard = drawingBoardList[0];
    if (!SplitModelTools::getInstance().isInitiativeSplitModel(topBoard)) {
        // 如果控制画板上一个标准命名的模型都没有，则自动给套入一个SM里。
        PModel copyCombineModel = CopyBoard2Combine::getCopy(topBoard.dynamicCast<Model>());
        QString mainBoardName = topBoard->getName();
        QString splitModelName = QString("%1_%2").arg(mainBoardName).arg(SL_CONST::NormalSMName);
        copyCombineModel->setName(splitModelName);
        copyCombineModel->setPrototypeName(splitModelName);
        copyCombineModel->getPortManager()->clearVariableMap();
        copyCombineModel->removeVariableGroup(RoleDataDefinition::InputSignal);
        copyCombineModel->removeVariableGroup(RoleDataDefinition::OutputSignal);
        pPrjMgr->SaveBoardModel(copyCombineModel);
        // 设置模型Id
        QString modelName = QString("%1_%2").arg(SL_CONST::NormalSMName).arg(mainBoardName);
        modelList[modelName] = 1;
    } else {
        // 标准化建模
        if (!SplitModelTools::getInstance().checkModelRule(topBoard)) {
            errMsg = QObject::tr("模型不符合拆模型规范");
            return false;
        }

        // 获取模型Id列表和拆模型
        for (auto model : topBoard->getChildModels()) {
            // 设置模型Id
            QString modelName = QString("%1_%2").arg(model->getName()).arg(topBoard->getName());
            int id = SplitModelTools::getInstance().getModelIndex(topBoard, model);
            modelList[modelName] = id;
            // 拆模型
            PDrawingBoardClass board = model.dynamicCast<DrawingBoardClass>();
            SplitModelTools::getInstance().genSplitModel(board, topBoard, pPrjMgr);
        }
    }

    // 将模型列表转换为json字符串
    Json::Value root;
    QMapIterator<QString, int> iter(modelList);
    while (iter.hasNext()) {
        iter.next();
        QString key = iter.key();
        int id = iter.value();
        Json::Value item;
        item[key.toStdString()] = id;
        root[SL_CONST::ModelIdJsonKeyName].append(item);
    }
    errMsg = QString::fromStdString(root.toStyledString());

    proj.clear();
    return true;
}

bool SimuLabServer::generateCode(const QString &dirPath, QString &errMsg)
{
    PKLProject proj = nullptr;
    QList<PDrawingBoardClass> drawingBoardList;
    if (!getProjectInfo(dirPath, proj, drawingBoardList, false)) {
        errMsg = QObject::tr("解析项目文件失败");
        return false;
    }

    PDrawingBoardClass topBoard = drawingBoardList[0];
    SimuLabSchema mapSchema;
    bool isRule = true;
    if (!SplitModelTools::getInstance().isInitiativeSplitModel(topBoard)) {
        // 如果控制画板上一个标准命名的模型都没有，则自动给套入一个SM里
        isRule = false;
        bool result = GenerateMapSchema::getInstance().generateOnceModel(SL_CONST::NormalSMName, topBoard->getName(),
                                                                         drawingBoardList, mapSchema, errMsg);
        if (!result) {
            LOGOUT(errMsg, LOG_ERROR);
            return false;
        }
    } else {
        // 标准化建模
        bool result = GenerateMapSchema::getInstance().generateMap(mapSchema, drawingBoardList, errMsg);
        if (!result) {
            LOGOUT(errMsg, LOG_ERROR);
            return false;
        }
    }

    // 创建生成代码目录结构
    QDir dir(dirPath);
    dir.cdUp();
    QString workPath = dir.path();
    if (!generateDir(workPath, topBoard, isRule)) {
        errMsg = QObject::tr("创建代码生成目录结构失败");
        LOGOUT(errMsg, LOG_ERROR);
        return false;
    }

    // 写map文件和生成控制引擎代码
    if (!generateSubSysCode(workPath, mapSchema, proj, errMsg)) {
        LOGOUT(errMsg, LOG_ERROR);
        return false;
    }

    // 如果生成代码成功把仿真配置信息填充到errMsg中
    errMsg = getConfig();

    proj.clear();
    return true;
}

bool SimuLabServer::getProjectInfo(const QString &dirPath, PKLProject &project,
                                   QList<PDrawingBoardClass> &drawingBoardList, bool isUpdate)
{
    // 获取npsproj文件路径
    QDir dir(dirPath);
    QStringList filters;
    filters << "*.npsproj";
    QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files);
    if (fileInfoList.size() < 1) {
        LOGOUT(QObject::tr("没有找到工程文件[.npsproj]"), LOG_ERROR);
        return false;
    }
    QString filePath = fileInfoList[0].absoluteFilePath();

    // 是否要重新加载
    if (!isUpdate) {
        project = pPrjMgr->GetProject(filePath);
        isUpdate = !(project && project->getAllBoardModel().size() > 0);
    }
    if (isUpdate) {
        project = loadProject(filePath);
        if (project == nullptr) {
            LOGOUT(QObject::tr("解析工程文件失败, 路径: %1").arg(filePath), LOG_ERROR);
            return false;
        }
        pPrjMgr->SetCurProject(project); // 加载项目之后设置一下当前激活项目，代码生成接口会使用CurProject
    }

    drawingBoardList.clear();
    for (PModel model : project->getAllBoardModel()) {
        PDrawingBoardClass boardModel = model.dynamicCast<DrawingBoardClass>();
        drawingBoardList << boardModel;
    }

    if (!swapMainDrawingBoard(drawingBoardList)) {
        LOGOUT(QObject::tr("控制画板不存在"), LOG_ERROR);
        return false;
    }

    // 设置子系统步长信息
    dir.cdUp();
    QString workPath = dir.path();
    setSubModelSteps(workPath, drawingBoardList[0], project);

    return true;
}

void SimuLabServer::setSubModelSteps(const QString &workPath, const PDrawingBoardClass &mainBoard,
                                     const PKLProject &project)
{
    subStepMap.clear();
    QFileInfo tmpInfo(workPath);
    const QString strMainModelName = tmpInfo.completeBaseName();
    if (GenerateMapSchema::getInstance().checkStandard(mainBoard)) {
        for (auto subModel : mainBoard->getChildModels()) {
            std::tuple<double, double> simuInfo = SplitModelTools::getInstance().getSimulationInfo(project, subModel);
            QString key = QString("%1_%2_%3")
                                  .arg(strMainModelName)
                                  .arg(subModel->getName())
                                  .arg(mainBoard->getName())
                                  .toLower(); // 根目录名_模型名_画板名
            subStepMap[key] = std::get<1>(simuInfo);
        }
    } else {
        QString key = QString("%1_sm_%2").arg(strMainModelName).arg(mainBoard->getName()).toLower();
        QVariantMap projcfgMap = project->getProjectConfig();
        subStepMap[key] = projcfgMap[KL_PRO::STEP_SIZE].toDouble();
    }
}

bool SimuLabServer::generateSubSysCode(const QString &workPath, const SimuLabSchema &mapSchema,
                                       const PKLProject &pProject, QString &errMsg)
{
    QFileInfo tmpInfo(workPath);
    const QString strMainModelName = tmpInfo.completeBaseName().toLower();
    foreach (SimuLabSchema::MapSchema mapUnit, mapSchema.mapList) {
        QString tmpSubModelName = QString("%1_%2").arg(strMainModelName).arg(mapUnit.mapFileName).toLower();
        QString sourceCodePath = QString("%1/%2/KLRTLINUXtarget").arg(workPath).arg(tmpSubModelName);

        // 设置步长
        if (!subStepMap.contains(tmpSubModelName)) {
            errMsg = "获取子系统步长失败";
            return false;
        }
        QVariantMap config = pProject->getProjectConfig();
        config[KL_PRO::STEP_SIZE] = subStepMap[tmpSubModelName];
        QStringList dirList;
        dirList << SL_CONST::EngineDllPath;
        config[KL_PRO::LIB_DIRS] = dirList;

        // 生成源码
        bool ret = pCtrlSysCodeMgr->ExportAllSource(mapUnit.drawingBoard, config, sourceCodePath);
        if (!ret) {
            errMsg = "调用控制引擎接口[ExportAllSource]返回失败";
            return false;
        }

        // 写子系统目录标志文件
        QString subModelFilePath = QString("%1/%2/%3.%4")
                                           .arg(workPath)
                                           .arg(tmpSubModelName)
                                           .arg(tmpSubModelName)
                                           .arg(SL_CONST::SubModelSuffix);
        FileManager::writeToFile(subModelFilePath, "SubModel Flag");

        // 写map文件
        QString mapFilePath = QString("%1/%2.map").arg(sourceCodePath).arg(tmpSubModelName);
        FileManager::writeToMap(mapFilePath, mapUnit);

        // 拷贝固定代码
        QString srcPath = QString(QFileInfo(QCoreApplication::applicationDirPath()).canonicalFilePath()) + "/"
                + SL_CONST::GenCodeDirName;
        FileManager::copyDir(srcPath, sourceCodePath);

        // 将makefile移动到model下
        QString srcModelCompile = QString("%1/makefile").arg(sourceCodePath);
        QString dstModelCompile = QString("%1/model/makefile").arg(sourceCodePath);
        QFile::rename(srcModelCompile, dstModelCompile);

        // 修改编译脚本名称
        QString srcCompileName = QString("%1/compileall.mk").arg(sourceCodePath);
        QString dstCompileName = QString("%1/%4.mk").arg(sourceCodePath).arg(tmpSubModelName);
        QFile::rename(srcCompileName, dstCompileName);

        // 拷贝用户源码
        if (!splitCopySourceCode(sourceCodePath, mapUnit.minBlockList)) {
            errMsg = "拷贝用户源码失败";
            return false;
        }

        // 拷贝文件模块配置文件
        QString dstFileDir = QString("%1/datafiles").arg(sourceCodePath);
        if (!copyFileBlockCnf(mapUnit, dstFileDir, errMsg)) {
            return false;
        }
    }
    return true;
}

bool SimuLabServer::generateDir(const QString &rootPath, const PDrawingBoardClass &drawingBoard, bool isRule)
{
    // 删除带xx.subsys文件的目录(之所以要遍历是因为模型改名后不知道删哪个)
    QString subModelPath = QString("%1").arg(rootPath);
    QDir dir(subModelPath);
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList fiSubModels = dir.entryInfoList();
    foreach (QFileInfo fiSubModel, fiSubModels) {
        if (!fiSubModel.isDir()) {
            continue;
        }

        bool bIsSubModel = false;
        QDir dirSubModel(fiSubModel.absoluteFilePath());
        dirSubModel.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        QFileInfoList fiSubModelFiles = dirSubModel.entryInfoList();
        foreach (QFileInfo fiSubModelFile, fiSubModelFiles) {
            if (fiSubModelFile.baseName().toLower() != fiSubModel.fileName().toLower()
                || (fiSubModelFile.suffix().toLower() != SL_CONST::SubModelSuffix)) {
                continue;
            }
            bIsSubModel = true;
            break;
        }

        if (bIsSubModel) {
            if (!FileManager::deleteDirfile(fiSubModel.absoluteFilePath())) {
                LOGOUT(QObject::tr("删除生成代码目录[%1]失败!").arg(fiSubModel.absoluteFilePath()), LOG_ERROR);
                return false;
            }
        }
    }

    // 创建生成代码目录结构
    QFileInfo rootInfo(rootPath);
    QString rootDirName = rootInfo.completeBaseName().toLower();
    QString boardName = drawingBoard->getName().toLower();
    if (!isRule) {
        // 没有标准化
        QString genCodeRootDir =
                QString("%1_%2_%3").arg(rootDirName).arg(SL_CONST::NormalSMName).arg(boardName).toLower();
        QString modelPath = QString("%1/%2/KLRTLINUXtarget").arg(rootPath).arg(genCodeRootDir);
        QDir dir(rootPath);
        if (!dir.mkpath(modelPath)) {
            LOGOUT(QObject::tr("创建生成代码目录失败! path=%1").arg(modelPath), LOG_ERROR);
            return false;
        }

    } else {
        // 已经标准化(SM/SS)
        QMapIterator<QString, PModel> iter(drawingBoard->getChildModels());
        while (iter.hasNext()) {
            iter.next();
            PModel block = iter.value();
            QString genCodeRootDir =
                    QString("%1_%2_%3").arg(rootDirName).arg(block->getName()).arg(boardName).toLower();
            QString modelPath = QString("%1/%2/KLRTLINUXtarget").arg(rootPath).arg(genCodeRootDir);
            QDir dir(rootPath);
            if (!dir.mkpath(modelPath)) {
                LOGOUT(QObject::tr("创建生成代码目录失败! path=%1").arg(modelPath), LOG_ERROR);
                return false;
            }
        }
    }
    return true;
}

bool SimuLabServer::swapMainDrawingBoard(QList<PDrawingBoardClass> &drawingBoardList)
{
    int index = 0;
    for each (PDrawingBoardClass var in drawingBoardList) {
        int boardType = var->getModelType();
        if (boardType == DrawingBoardClass::Control_Board_Type) {
            drawingBoardList.swap(index, 0);
            return true;
        }
        index++;
    }
    return false;
}

bool SimuLabServer::splitCopySourceCode(const QString &dstCodePath, const QList<PModel> &minBlockList)
{
    // 要求用户动态库名称等于源码目录名称
    // for each (PModel block in minBlockList) {
    //    PControlBlock ctrBlock = block.dynamicCast<ControlBlock>();
    //    if (false) {//ctrBlock->classification == ControlBlock::CodeModel) {
    //        QString prototypeName = ctrBlock->getPrototypeName();
    //        QString dllName = prototypeName.split("_")[0];

    //        QDir srcDir(m_prjCfg.sourceCodePath);
    //        if (m_prjCfg.sourceCodePath.isEmpty() || !srcDir.exists()) {
    //            LOGOUT(QString("请配置仿真参数源码路径!"), LOG_ERROR);
    //            return false;
    //        }

    //        QString userSrcPath = QString("%1/%2").arg(m_prjCfg.sourceCodePath).arg(dllName);
    //        QDir userPath(userSrcPath);
    //        if (!userPath.exists()) {
    //            LOGOUT(QString("用户源码[%1]不存在.").arg(userSrcPath), LOG_ERROR);
    //            return false;
    //        }

    //        QString userDstPath = QString("%1/userblock/%2").arg(dstCodePath).arg(dllName);
    //        copyDir(userSrcPath, userDstPath);
    //    }
    //}
    return true;
}

PKLProject SimuLabServer::loadProject(const QString &projPath)
{
    return pPrjMgr->LoadProject(projPath);
}

bool SimuLabServer::copyFileBlockCnf(const SimuLabSchema::MapSchema &mapUnit, const QString &filesDir, QString &errMsg)
{
    QString boardName = mapUnit.drawingBoard->getName();
    for (auto block : mapUnit.minBlockList) {
        // 获取文件路径
        QString filePath;
        if (block->getPrototypeName() == SL_CONST::ProtoName_FromFile) {
            PVariableGroup paramGroup = block->getVariableGroup(RoleDataDefinition::Parameter);
            auto pv = paramGroup->getVariableByName(SL_CONST::FromFilePATH);
            filePath = pv ? pv->getDefaultValue().toString() : filePath;
        } else if (block->getPrototypeName() == SL_CONST::ProtoName_FMU) {
            PVariableGroup paramGroup = block->getVariableGroup(RoleDataDefinition::FmuParameter);
            auto pv = paramGroup->getVariableByName(SL_CONST::FMUPATH);
            filePath = pv ? pv->getDefaultValue().toString() : filePath;
        }

        if (filePath.isEmpty()) {
            continue;
        }

        // 判断配置文件路径是否存在
        QFile file(filePath);
        if (!file.exists()) {
            QString context =
                    QString("<log><p>[%1]</p><a code=\"100\" uuid=\"%2\" boardname=\"%1\">%3</a><p>%4</p></log>")
                            .arg(boardName)
                            .arg(block->getObjectUuid())
                            .arg(block->getName())
                            .arg("路径不存在");
            LOGOUT(context, LOG_ERROR);
            errMsg = QString("[%1]画板[%2]模块的参数[fileName=%3]路径不存在!")
                                .arg(boardName)
                                .arg(block->getName())
                                .arg(filePath);
            return false;
        }

        // 拷贝到datafiles目录下
        QDir dir(filesDir);
        if (!dir.exists()) {
            bool ismkdir = dir.mkdir(filesDir);
            if (!ismkdir) {
                errMsg = QString("创建目录[%1]失败").arg(filesDir);
                return false;
            }
        }

        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        QString dstPathFile = QString("%1/%2").arg(filesDir).arg(fileName);
        QFile dstFile(dstPathFile);
        if (dstFile.exists()) {
            QString context =
                    QString("<log><p>[%1]</p><a code=\"100\" uuid=\"%2\" boardname=\"%1\">%3</a><p>%4</p></log>")
                            .arg(boardName)
                            .arg(block->getObjectUuid())
                            .arg(block->getName())
                            .arg("配置文件重名");
            LOGOUT(context, LOG_ERROR);
            errMsg = QString("[%1]画板[%2]模块的参数[fileName=%3]文件重名!")
                                .arg(boardName)
                                .arg(block->getName())
                                .arg(filePath);
            return false;
        }
        if (!file.copy(dstPathFile)) {
            QString context =
                    QString("<log><p>[%1]</p><a code=\"100\" uuid=\"%2\" boardname=\"%1\">%3</a><p>%4</p></log>")
                            .arg(boardName)
                            .arg(block->getObjectUuid())
                            .arg(block->getName())
                            .arg("文件拷贝失败");
            LOGOUT(context, LOG_ERROR);
            errMsg = QString("[%1]画板[%2]模块的参数[fileName=%3]文件拷贝失败!")
                                .arg(boardName)
                                .arg(block->getName())
                                .arg(filePath);
            return false;
        }
    }
    return true;
}
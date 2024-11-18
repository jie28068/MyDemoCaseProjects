#include "SimuNPSBusinessHooksServer.h"
#include "CanvasWidget.h"
#include "CoreLib/ServerManager.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingTool.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "Manager/ClipboardManager.h"
#include "Manager/ModelingServerMng.h"
#include "ModelWidget.h"

using namespace Kcc::BlockDefinition;

USE_LOGOUT_("PIPluginGraphicModelingServer")
SimuNPSBusinessHooksServer::SimuNPSBusinessHooksServer(QSharedPointer<CanvasContext> pcanvascontext,
                                                       QSharedPointer<Kcc::BlockDefinition::Model> model)
    : BusinessHooksServer(pcanvascontext)
{
    m_pBoardClass = model;
}

void SimuNPSBusinessHooksServer::createImageAnnotationDialog(PSourceProxy sourceProxy)
{
    if (!sourceProxy) {
        return;
    }

    if (sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION
        && sourceProxy->prototypeName() == ImageAnnotationStr) {
        if (ModelingServerMng::getInstance().m_pPropertyManagerServer) {
            ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowCommentProperty(sourceProxy,
                                                                                           m_canvascontext);
        }
    }
}

QString SimuNPSBusinessHooksServer::createNewBlock(QString prototypeName, QPointF centerPoint, bool needUndo,
                                                   QString defaultName, QString blockUUID)
{
    CanvasWidget *canvasWidget = findCanvasWidget();
    if (canvasWidget) {
        auto block = canvasWidget->addBlockToView(prototypeName, centerPoint, needUndo, defaultName, blockUUID);
        if (block) {
            return block->getUUID();
        }
        return "";
    }

    ModelWidget *modelWidget = findModelWidget();
    if (modelWidget) {
        auto block = modelWidget->addBlockToView(prototypeName, centerPoint, needUndo, defaultName);
        if (block) {
            return block->getUUID();
        }
    }

    return QString();
}

bool SimuNPSBusinessHooksServer::isFullScreenWorkArea()
{
    if (ModelingServerMng::getInstance().m_pUIServer != nullptr
        && ModelingServerMng::getInstance().m_pUIServer->GetMainUI() != nullptr) {
        return ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->WorkAreaIsInFullScreen();
    }

    return false;
}

void SimuNPSBusinessHooksServer::setNewSourceProxy(QSharedPointer<SourceProxy> orignalSouce,
                                                   QSharedPointer<SourceProxy> newSource,
                                                   QVector<QString> &allSourceName)
{
    QMap<QString, QSharedPointer<Model>> activeBlock; // 画板所有模块

    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }

    QVector<QString>::iterator allIter = allSourceName.begin(); // 框选的所有模块名
    auto curBoardModel =
            ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(m_canvascontext->uuid());
    if (!curBoardModel) {
        return;
    }

    activeBlock = curBoardModel->getChildModels();

    QString oldName = newSource->name(); // 原名 aa1d 1
    QRegExp reg("\\d*$");
    int pos = reg.indexIn(oldName);
    QString oldNamechange = oldName.left(pos); // aa1d
    QString name;
    int index = 0;

    bool isTrue = true;
    for each (auto source in m_canvascontext->getAllSource()) {
        QString plockNmae = source->name();
        if (oldName == plockNmae) {
            isTrue = false;
            break;
        }
    }

    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer.isNull()) {
        LOGOUT("IPluginGraphicModelingServer未注册", LOG_ERROR);
    } else if (isTrue) {
        index = oldName.right(oldName.size() - pos).toInt();
    } else if (newSource->prototypeName() != ImageAnnotationStr) {
        index = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getBlockPrototypeNextIndex(
                oldNamechange, curBoardModel, "", false);
    } else { // 图片特殊处理
        for (int i = 0;; ++i) {
            auto blockMap = m_canvascontext->getAllSource();
            auto iter = blockMap.constBegin();
            QString resultName;
            if (i == 0) {
                resultName = QString("%1").arg((oldNamechange.size() == 0) ? "1" : oldNamechange);
                if (resultName.endsWith(' ')) { // 特殊处理：空格结尾
                    i = i + 1;
                    resultName = QString("%1%2").arg(oldNamechange).arg(i);
                }
            } else {
                resultName = QString("%1%2").arg(oldNamechange).arg(i);
            }
            bool isTrue = true;
            while (iter != blockMap.constEnd()) {
                QString plockNmae = iter.value()->name();
                if (resultName == plockNmae) {
                    isTrue = false;
                    break;
                }
                iter++;
            }
            if (isTrue) {
                index = i;
                break;
            }
        }
    }

    if (index == 0) {
        name = QString("%1").arg(oldNamechange.size() == 0 ? "1" : oldNamechange);
        if (pos == oldName.size() - 1) { // 跨画板复制名字末尾是0需要加上0
            name.append("0");
        }
    } else {
        name = QString("%1%2").arg(oldNamechange).arg(index);
    }
    // 图元组模块名 + 画板模块名
    QMap<QString, QSharedPointer<Model>>::iterator activeIter = activeBlock.begin();
    while (activeIter != activeBlock.end()) {
        QString str = activeIter->data()->getName();
        if (!allSourceName.contains(str)) {
            allSourceName.append(str);
        }
        activeIter++;
    }
    if (!isTrue) {
        allIter = allSourceName.begin();
        // 检测同一模块是否重名
        while (allIter != allSourceName.end()) {
            if (name == allIter) {
                name = QString("%1%2").arg(oldNamechange).arg(++index);
                allIter = allSourceName.begin();
            } else {
                allIter++;
            }
        }
    }
    allSourceName.push_back(name);
    newSource->setName(name);
}

PDrawingBoardClass SimuNPSBusinessHooksServer::findBoardModel()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer
        || !ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        return nullptr;
    }

    PModel pmodel = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(m_canvascontext->uuid());
    if (!pmodel) {
        return nullptr;
    }

    PModel topModel = pmodel->getTopParentModel();

    return topModel.dynamicCast<DrawingBoardClass>();
}

CanvasWidget *SimuNPSBusinessHooksServer::findCanvasWidget()
{
    // if (ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
    //     return ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(
    //             m_canvascontext->uuid());
    // }
    if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        return nullptr;
    }
    /// 2023.08.23 by txy
    /// 目前电气画板和构造画板都存在子系统的概念,根据子系统的uuid在m_pGraphicsModelingServer是找不到对应画板的,只能拿到顶层画板
    PModel topModel = findBoardModel();
    if (!topModel) {
        return nullptr;
    }
    CanvasWidget *widget =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(topModel->getUUID());

    return widget;
}

ModelWidget *SimuNPSBusinessHooksServer::findModelWidget()
{
    CanvasWidget *widget = findCanvasWidget();
    if (widget) {
        return widget->getCurModelWidget();
    }
    return nullptr;
}

QString SimuNPSBusinessHooksServer::getRandomString(int length)
{
    QString res;
    qsrand(QDateTime::currentMSecsSinceEpoch());

    const char ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int size = sizeof(ch);

    int num = 0;
    for (int i = 0; i < length; ++i) {
        num = rand() % (size - 1);
        res.append(ch[num]);
    }

    return res;
}

QList<BlockStruct> SimuNPSBusinessHooksServer::getCanCreateBlockList()
{
    CanvasWidget *canvasWidget = findCanvasWidget();
    if (canvasWidget) {
        return canvasWidget->getCanCreateBlockList();
    }

    ModelWidget *modelWidget = findModelWidget();
    if (modelWidget) {
        return modelWidget->getCanCreateBlockList();
    }
    return QList<BlockStruct>();
}

bool SimuNPSBusinessHooksServer::isModelDataAlreadyExist(PSourceProxy source)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext || !source) {
        return false;
    }
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return false;
    }
    auto canvasModel = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(canvasContext->uuid());
    if (canvasModel) {
        return canvasModel->hasChild(source->uuid());
    }
    return false;
}

void SimuNPSBusinessHooksServer::workAreaInFullScreenChange(const QString &boradName)
{
    if (ModelingServerMng::getInstance().m_pUIServer != nullptr
        && ModelingServerMng::getInstance().m_pUIServer->GetMainUI() != nullptr
        && ModelingServerMng::getInstance().m_pGraphicsModelingServer != nullptr) {
        auto m_widget = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardName(boradName);
        if (m_widget) {
            auto isFull = m_widget->fullScreenMode();
            if (!isFull) {
                ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->FullscreenWorkareaWidget(boradName);
            } else {
                ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->ExitFullscreenWorkareaWidget(boradName,
                                                                                                        boradName);
            }
        }
    }
}

void SimuNPSBusinessHooksServer::openShowHelpWidget(const QString &name)
{
    if (ModelingServerMng::getInstance().m_pPluginElectricComponentServer != nullptr && !name.isEmpty()) {
        ModelingServerMng::getInstance().m_pPluginElectricComponentServer->openShowHelp(name);
    }
}

QString SimuNPSBusinessHooksServer::getCurProjectSign()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return QString();
    }
    PKLProject curProject = ModelingServerMng::getInstance().m_pProjectMngServer->GetCurProject();
    if (!curProject) {
        return QString();
    }
    return curProject->getProjectPath();
}

QString SimuNPSBusinessHooksServer::getTopParentModelUUID()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return QString();
    }
    PModel pmodel = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(m_canvascontext->uuid());
    if (!pmodel) {
        return QString();
    }
    return pmodel->getTopParentModel()->getUUID();
}

QString SimuNPSBusinessHooksServer::getCurrentProjectDir()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return QString();
    }
    PKLProject curProject = ModelingServerMng::getInstance().m_pProjectMngServer->GetCurProject();
    if (!curProject) {
        return QString();
    }
    return curProject->getProjectDir();
}

void SimuNPSBusinessHooksServer::PrintErrorInfos()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    PDrawingBoardClass drawBoard = ModelingServerMng::getInstance()
                                           .m_pProjectMngServer->GetBoardModelByUUID(m_canvascontext->uuid())
                                           .dynamicCast<DrawingBoardClass>();
    QMap<QString, PBlockConnector> allConnectors = drawBoard->getConnectors();
    QMap<QString, PBlockConnector>::iterator iter = allConnectors.begin();
    for (iter; iter != allConnectors.end(); iter++) {
        QString connectorUUID = iter.key();
        if (!m_canvascontext->getConnectorWireContext(connectorUUID)) {
            // 说明当前连接线在画板中不存在
            PBlockConnector connector = iter.value();
            if (!connector) {
                return;
            }
            QString srcModelName = connector->getSrcModel()->getName();
            QString dstModelName = connector->getDstModel()->getName();
            QString srcPortUUID = connector->getSrcPortUuid();
            QString dstPortUUID = connector->getDstPortUuid();
            QMap<QString, PVariable> srcModelPortVariables =
                    connector->getSrcModel()->getPortManager()->getVariableMap();
            QMap<QString, PVariable> dstModelPortVariables =
                    connector->getDstModel()->getPortManager()->getVariableMap();
            PVariable srcPortVariable = srcModelPortVariables.value(srcPortUUID, PVariable());
            PVariable dstPortVariable = dstModelPortVariables.value(dstPortUUID, PVariable());

            QString debugMsg = QString("不存在模块%1:%2->模块%3:%4的连接")
                                       .arg(srcModelName)
                                       .arg(srcPortVariable->getName())
                                       .arg(dstModelName)
                                       .arg(dstPortVariable->getName());
            qDebug() << debugMsg;
        }
    }

    QMap<QString, PModel> allChilds = drawBoard->getChildModels();
    QMap<QString, PModel>::iterator iterChild = allChilds.begin();
    for (iterChild; iterChild != allChilds.end(); iterChild++) {
        QString childUUID = iterChild.key();
        if (!m_canvascontext->getSource(childUUID)) {
            // 当前画板不存在此模型
            PModel childModel = iterChild.value();
            qDebug() << QString("不存在模型:%1").arg(childModel->getName());
        }
    }
}

void SimuNPSBusinessHooksServer::createEmptySubsystem(const QRectF &rc, QString &newModelUUID)
{
    QString subSystemPrototypeName = SUBSYSTEM_PROTO_PREFIX + getRandomString(6);
    PIProjectManagerServer projectManagerServer = ModelingServerMng::getInstance().m_pProjectMngServer;
    if (!projectManagerServer) {
        return;
    }

    int modelType = m_pBoardClass->getModelType();
    int createBoardType = CombineBoardModel::Type;
    if (modelType == ElecBoardModel::Type || modelType == ElecCombineBoardModel::Type) {
        createBoardType = ElecCombineBoardModel::Type;
    }

    PDrawingBoardClass drawBoardClass = ModelHelp::createModel(createBoardType).dynamicCast<DrawingBoardClass>();
    if (!drawBoardClass) {
        return;
    }
    QString subSystemName = m_pBoardClass->calculateNextChildName(SUBSYSTEM_NAME_PREFIX);
    drawBoardClass->setName(subSystemName);
    drawBoardClass->setPrototypeName(subSystemPrototypeName);
    drawBoardClass->setPrototypeName_Readable(SUBSYSTEM_NAME_PREFIX);

    PCanvasContext currentCtx = getCanvasContext();
    if (!currentCtx) {
        return;
    }
    PCanvasContext ctx = drawBoardClass->createCanvasContext();
    ctx->setSize(currentCtx->size());
    QSize size = ctx->size();
    if (createBoardType == CombineBoardModel::Type) {
        // 创建输入模块
        PModel pInBlock = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                GraphicsModelingConst::SPECIAL_BLOCK_CTRLIN);
        // 创建输出模块
        PModel pOutBlock = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                GraphicsModelingConst::SPECIAL_BLOCK_CTRLOUT);
        if (!pInBlock || !pOutBlock) {
            return;
        }
        QString inputBlockName, outputBlockName;
        QString inputBlockUUID, outputBlockUUID;
        double canvasWidth = size.width() * 3.77;
        double canvasHeight = size.height() * 3.77;
        pInBlock->setName("In");
        pOutBlock->setName("Out");

        pInBlock->setPostion(QPointF(canvasWidth / 2 - 100, canvasHeight / 2));
        pOutBlock->setPostion(QPointF(canvasWidth / 2 + 100, canvasHeight / 2));

        PBlockConnector pConnector = PBlockConnector(new BlockConnector());
        if (!pConnector) {
            return;
        }
        pConnector->setSrcModel(pInBlock);
        pConnector->setDstModel(pOutBlock);
        pConnector->setIsCubicOn(true);

        ctx->setCenterPos(QPointF(canvasWidth / 2, canvasHeight / 2));
        auto inputPortManager = pInBlock->getPortManager();
        if (inputPortManager) {
            auto variableMap = inputPortManager->getVariableMap();
            if (variableMap.size() <= 0) {
                return;
            }
            QString inputPortUUID = variableMap.first()->getUUID();
            pConnector->setSrcPortUuid(inputPortUUID);
        }

        auto outputPortManager = pOutBlock->getPortManager();
        if (outputPortManager) {
            auto variableMap = outputPortManager->getVariableMap();
            if (variableMap.size() <= 0) {
                return;
            }
            QString outputPortUUID = variableMap.first()->getUUID();
            pConnector->setDstPortUuid(outputPortUUID);
        }

        drawBoardClass->addChildModel(pInBlock);
        drawBoardClass->addChildModel(pOutBlock);
        drawBoardClass->addConnector(pConnector);
    }

    auto modelWidget = findModelWidget();
    if (m_pBoardClass && modelWidget) {
        QString blockName, blockUUID;
        GraphicsModelingTool::calculateBlockUuid(blockUUID, blockName, drawBoardClass, m_pBoardClass);
        drawBoardClass->setName(blockName);
        drawBoardClass->setPostion(rc.center());
        drawBoardClass->setSize(QSize(rc.width(), rc.height()));
        modelWidget->addBlockToView(drawBoardClass, false);
        newModelUUID = drawBoardClass->getUUID();
    }
}

void SimuNPSBusinessHooksServer::createContructorSubSystem(const QRectF &rc, QList<PSourceProxy> selectedSources,
                                                           QList<PConnectorWireContext> doubleSelectedWireContext,
                                                           QList<PConnectorWireContext> singleSideSelectedWireContext,
                                                           QString &newModelUUID, QMap<QString, QString> &newLinkMap)
{
    if (selectedSources.isEmpty() || !m_pBoardClass) {
        return;
    }

    int modelType = m_pBoardClass->getModelType();
    int createBoardType = CombineBoardModel::Type;
    if (modelType == ElecBoardModel::Type || modelType == ElecCombineBoardModel::Type) {
        createBoardType = ElecCombineBoardModel::Type;
    }

    QString subSystemPrototypeName = SUBSYSTEM_PROTO_PREFIX + getRandomString(6);
    PIProjectManagerServer projectManagerServer = ModelingServerMng::getInstance().m_pProjectMngServer;
    if (!projectManagerServer) {
        return;
    }

    PDrawingBoardClass drawBoardClass = ModelHelp::createModel(createBoardType).dynamicCast<DrawingBoardClass>();
    if (!drawBoardClass) {
        return;
    }
    QString subSystemName = m_pBoardClass->calculateNextChildName(SUBSYSTEM_NAME_PREFIX);
    drawBoardClass->setName(subSystemName);
    drawBoardClass->setPrototypeName(subSystemPrototypeName);
    drawBoardClass->setPrototypeName_Readable(SUBSYSTEM_NAME_PREFIX);

    if (!newModelUUID.isEmpty() && !drawBoardClass->getChildModel(newModelUUID)) {
        drawBoardClass->setUUID(newModelUUID);
    }

    PCanvasContext currentCtx = getCanvasContext();
    if (!currentCtx) {
        return;
    }
    PCanvasContext ctx = drawBoardClass->createCanvasContext();
    if (!ctx) {
        return;
    }
    ctx->setSubsystemInitialized(false);
    ctx->setSize(currentCtx->size());
    QSize size = ctx->size();

    QList<PModel> selectedModels;

    foreach (auto source, selectedSources) {
        auto model = m_pBoardClass->getChildModel(source->uuid());
        if (model) {
            QString modelPrototypeName = model->getPrototypeName();
            if (modelPrototypeName == "In" || modelPrototypeName == "Out") {
                continue;
            }
            ClipboardManager::getInstance().addBlock(model);
            m_pBoardClass->removeChildModel(model->getUUID());
            selectedModels.append(model);
            drawBoardClass->addChildModel(model);
            PSourceProxy tmpSource = ctx->getSource(model->getUUID());
            if (tmpSource) {
                // 同步模块的大小以及矩阵变换
                tmpSource->setSize(source->size());
                tmpSource->setScaleTransform(source->scaleTransform());
                tmpSource->setRotateTransform(source->rotateTransform());
                tmpSource->setTranslateTransform(source->translateTransform());
            }
        }
    }

    foreach (auto wireCtx, doubleSelectedWireContext) {
        PSourceProxy srcSource = wireCtx->srcSource();
        PSourceProxy dstSource = wireCtx->dstSource();
        if (srcSource && dstSource) {
            PModel srcModel, dstModel;
            QString srcPortUUID, dstPortUUID;
            if (srcSource->prototypeName() == "In" || srcSource->prototypeName() == "Out") {
                auto model = m_pBoardClass->getChildModel(srcSource->uuid());
                srcModel = model->copy();
                QString blockName, blockUUID;
                GraphicsModelingTool::calculateBlockUuid(blockUUID, blockName, srcModel, drawBoardClass);
                srcModel->setName(blockName);
                auto srcPortManager = srcModel->getPortManager();
                if (srcPortManager) {
                    auto variableMap = srcPortManager->getVariableMap();
                    if (variableMap.size() <= 0) {
                        return;
                    }
                    srcPortUUID = variableMap.first()->getUUID();
                }
                drawBoardClass->addChildModel(srcModel);
                newLinkMap[wireCtx->uuid()] = blockName;
            }
            if (dstSource->prototypeName() == "In" || dstSource->prototypeName() == "Out") {
                auto model = m_pBoardClass->getChildModel(dstSource->uuid());
                dstModel = model->copy();
                QString blockName, blockUUID;
                GraphicsModelingTool::calculateBlockUuid(blockUUID, blockName, dstModel, drawBoardClass);
                dstModel->setName(blockName);
                auto dstPortManager = dstModel->getPortManager();
                if (dstPortManager) {
                    auto variableMap = dstPortManager->getVariableMap();
                    if (variableMap.size() <= 0) {
                        return;
                    }
                    dstPortUUID = variableMap.first()->getUUID();
                }
                drawBoardClass->addChildModel(dstModel);
                newLinkMap[wireCtx->uuid()] = blockName;
            }
            if (!srcModel) {
                srcModel = drawBoardClass->getChildModel(srcSource->uuid());
                srcPortUUID = wireCtx->srcPortID();
            }
            if (!dstModel) {
                dstModel = drawBoardClass->getChildModel(dstSource->uuid());
                dstPortUUID = wireCtx->dstPortID();
            }
            if (srcModel && dstModel) {
                PBlockConnector pConnector = PBlockConnector(new BlockConnector());
                pConnector->setSrcModel(srcModel);
                pConnector->setDstModel(dstModel);
                pConnector->setSrcPortUuid(srcPortUUID);
                pConnector->setDstPortUuid(dstPortUUID);
                pConnector->setIsCubicOn(true);
                drawBoardClass->addConnector(pConnector);
                PConnectorWireContext tmpWireContext = ctx->getConnectorWireContext(pConnector->getUUID());
                if (tmpWireContext) {
                    // 同步连接线的坐标，保证线型一致
                    tmpWireContext->setPoints(wireCtx->points());
                }
            }
        }
    }

    // 把连接线只有一端在子系统的，另一端替换成输入或者输出
    foreach (auto wireCtx, singleSideSelectedWireContext) {
        PSourceProxy srcSource = wireCtx->srcSource();
        PSourceProxy dstSource = wireCtx->dstSource();
        if (srcSource && dstSource) {
            auto srcModel = drawBoardClass->getChildModel(srcSource->uuid());
            auto dstModel = drawBoardClass->getChildModel(dstSource->uuid());

            // 此处srcModel和dstModel有一个为nullptr
            PModel inOutModel;
            if (dstModel) {
                // 控制系统输入端口
                // 创建输入模块
                QPointF modelPos = dstModel->getPosition();
                QSizeF modelSize = dstModel->getSize();
                inOutModel = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                        GraphicsModelingConst::SPECIAL_BLOCK_CTRLIN);
                QPointF inPos = modelPos;
                inPos.setX(modelPos.x() - 200);
                inPos.setY(modelPos.y() + 10);
                inOutModel->setPostion(inPos);

            } else if (srcModel) {
                // 创建输出模块
                QPointF modelPos = srcModel->getPosition();
                QSizeF modelSize = srcModel->getSize();
                inOutModel = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                        GraphicsModelingConst::SPECIAL_BLOCK_CTRLOUT);
                QPointF inPos = modelPos;
                inPos.setX(modelPos.x() + modelSize.width() + 200);
                inPos.setY(modelPos.y() + 10);
                inOutModel->setPostion(inPos);
            }
            if (!inOutModel) {

                continue;
            }
            QString blockName, blockUUID;
            GraphicsModelingTool::calculateBlockUuid(blockUUID, blockName, inOutModel, drawBoardClass);
            inOutModel->setName(blockName);

            drawBoardClass->addChildModel(inOutModel);

            QString inoutPortUUID;

            auto inoutPortManager = inOutModel->getPortManager();
            if (inoutPortManager) {
                auto variableMap = inoutPortManager->getVariableMap();
                if (variableMap.size() <= 0) {
                    return;
                }
                inoutPortUUID = variableMap.first()->getUUID();
            }

            PBlockConnector pConnector = PBlockConnector(new BlockConnector());

            pConnector->setIsCubicOn(true);

            if (srcModel) {
                pConnector->setSrcModel(srcModel);
                pConnector->setSrcPortUuid(wireCtx->srcPortID());
                pConnector->setDstModel(inOutModel);
                pConnector->setDstPortUuid(inoutPortUUID);

            } else if (dstModel) {
                pConnector->setDstModel(dstModel);
                pConnector->setDstPortUuid(wireCtx->dstPortID());
                pConnector->setSrcModel(inOutModel);
                pConnector->setSrcPortUuid(inoutPortUUID);
            }
            drawBoardClass->addConnector(pConnector);
            newLinkMap[wireCtx->uuid()] = blockName;
        }
    }

    auto modelWidget = findModelWidget();
    if (m_pBoardClass && modelWidget) {
        drawBoardClass->setPostion(rc.center());
        drawBoardClass->setSize(QSize(160, rc.height()));
        modelWidget->addBlockToView(drawBoardClass, false);
        newModelUUID = drawBoardClass->getUUID();

        if (createBoardType == CombineBoardModel::Type) {
            // 把未连接的端口连上输出或者输出
            foreach (auto source, selectedSources) {
                auto model = drawBoardClass->getChildModel(source->uuid());
                if (model) {
                    QList<PBlockConnector> connectors = drawBoardClass->getAdjacentConnector(model->getUUID());
                    auto portVarMap = model->getPortManager()->getVariableMap();
                    QList<QString> connectedPortIDS;
                    foreach (auto co, connectors) {
                        if (co->getSrcModel() == model) {
                            connectedPortIDS.append(co->getSrcPortUuid());

                        } else if (co->getDstModel() == model) {
                            connectedPortIDS.append(co->getDstPortUuid());
                        }
                    }

                    QList<PVariable> sortedPorts = model->getPortManager()->getVariableSortByOrder();

                    QPointF modelPos = model->getPosition();
                    QSizeF modelSize = model->getSize();

                    int inIndex = 0;
                    int outIndex = 0;

                    for (auto portVar : sortedPorts) {
                        QString portUUID = portVar->getUUID();
                        if (connectedPortIDS.contains(portUUID)) {
                            continue;
                        }
                        Variable::PortType portType = portVar->getPortType();
                        PModel inOutModel;
                        if (portType == Variable::ControlIn) {
                            // 控制系统输入端口
                            // 创建输入模块
                            inOutModel = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                                    GraphicsModelingConst::SPECIAL_BLOCK_CTRLIN);
                            QPointF inPos = modelPos;
                            inPos.setX(modelPos.x() - 100);
                            inPos.setY(modelPos.y() + inIndex * 40 + 10);
                            inOutModel->setPostion(inPos);
                            inIndex++;

                        } else if (portType == Variable::ControlOut) {
                            // 控制系统输出端口
                            // 创建输出模块
                            inOutModel = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                                    GraphicsModelingConst::SPECIAL_BLOCK_CTRLOUT);
                            QPointF inPos = modelPos;
                            inPos.setX(modelPos.x() + modelSize.width() + 160);
                            inPos.setY(modelPos.y() + outIndex * 40 + 10);
                            inOutModel->setPostion(inPos);
                            outIndex++;
                        } else {
                            continue;
                        }
                        QString blockName, blockUUID;
                        GraphicsModelingTool::calculateBlockUuid(blockUUID, blockName, inOutModel, drawBoardClass);
                        inOutModel->setName(blockName);

                        drawBoardClass->addChildModel(inOutModel);

                        QString inoutPortUUID;

                        auto inoutPortManager = inOutModel->getPortManager();
                        if (inoutPortManager) {
                            auto variableMap = inoutPortManager->getVariableMap();
                            if (variableMap.size() <= 0) {
                                return;
                            }
                            inoutPortUUID = variableMap.first()->getUUID();
                        }

                        PBlockConnector pConnector = PBlockConnector(new BlockConnector());
                        pConnector->setIsCubicOn(true);
                        if (portType == Variable::ControlIn) {
                            pConnector->setSrcModel(inOutModel);
                            pConnector->setDstModel(model);
                            pConnector->setSrcPortUuid(inoutPortUUID);
                            pConnector->setDstPortUuid(portUUID);
                            drawBoardClass->addConnector(pConnector);
                        } else if (portType == Variable::ControlOut) {
                            pConnector->setSrcModel(model);
                            pConnector->setDstModel(inOutModel);
                            pConnector->setSrcPortUuid(portUUID);
                            pConnector->setDstPortUuid(inoutPortUUID);
                            drawBoardClass->addConnector(pConnector);
                        }
                    }
                }
            }
        }
    }
}

void SimuNPSBusinessHooksServer::destroyContructorSubSystem(QString subSystemModelUUID,
                                                            QList<PSourceProxy> selectedSources)
{
    if (subSystemModelUUID.isEmpty() || selectedSources.isEmpty() || !m_pBoardClass) {
        return;
    }
    auto canvasCtx = m_pBoardClass->getCanvasContext();
    if (!canvasCtx) {
        return;
    }
    PDrawingBoardClass subsystemDrawBoard =
            m_pBoardClass->getChildModel(subSystemModelUUID).dynamicCast<DrawingBoardClass>();
    if (!subsystemDrawBoard) {
        return;
    }
    foreach (auto source, selectedSources) {
        if (!source) {
            continue;
        }
        PModel model = subsystemDrawBoard->getChildModel(source->uuid());
        if (model) {

            m_pBoardClass->addChildModel(model);
            PSourceProxy tmpSource = canvasCtx->getSource(model->getUUID());
            if (tmpSource) {
                // 同步模块的大小以及矩阵变换
                tmpSource->setSize(source->size());
                tmpSource->setScaleTransform(source->scaleTransform());
                tmpSource->setRotateTransform(source->rotateTransform());
                tmpSource->setTranslateTransform(source->translateTransform());
            }
            subsystemDrawBoard->removeChildModel(model->getUUID());
            model->setParentModel(m_pBoardClass);
        }
    }
}

void SimuNPSBusinessHooksServer::renameSubsystem(const QString &sourceUUID)
{
    if (!m_pBoardClass) {
        return;
    }
    PModel subSystemModel = m_pBoardClass->getChildModel(sourceUUID);
    if (subSystemModel) {
        QString subSystemName = m_pBoardClass->calculateNextChildName(SUBSYSTEM_NAME_PREFIX);
        subSystemModel->setName(subSystemName);
    }
}

QString SimuNPSBusinessHooksServer::isContainCanvasContext(PSourceProxy source)
{
    if (!source) {
        return "";
    }
    PModel childModel = m_pBoardClass->getChildModel(source->uuid());
    if (!childModel) {
        return "";
    }
    auto canvasCtx = childModel->getCanvasContext();
    if (canvasCtx) {
        return canvasCtx->uuid();
    }
    return QString();
}

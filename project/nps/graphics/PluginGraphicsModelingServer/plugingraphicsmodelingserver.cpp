#include "PluginGraphicsModelingServer.h"
#include "CanvasWidget.h"
#include "CombineBoardModel.h"
#include "ComplexBoardModel.h"
#include "ControlBoardModel.h"
#include "CoreLib/ServerManager.h"
#include "ElecBoardModel.h"
#include "Manager/ModelingServerMng.h"
#include "ModelWidget.h"
#include <GraphicsModelingConst.h>
#include <GraphicsModelingTool.h>
#include <KLWidgets/KCustomDialog.h>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <quuid.h>

USE_LOGOUT_("PluginGraphicsModelingServer")

Q_DECLARE_METATYPE(Simu_Var::SimuVarBoard)

PluginGraphicsModelingServer::PluginGraphicsModelingServer()
{
    m_activeDrawingBoardUUID = "";
    m_bRuningStatus = false;
    m_mapRunValue.clear();
}

PluginGraphicsModelingServer::~PluginGraphicsModelingServer() { }

void PluginGraphicsModelingServer::Init()
{
    m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!m_pUIServer) {
        LOGOUT("ICoreManUILayoutServer未注册", LOG_ERROR);
        return;
    }

    m_m_pUIServerIF = RequestServerInterface<ICoreManUILayoutServer>();
    if (!m_m_pUIServerIF) {
        LOGOUT("PICoreManUILayoutServer  InterfaceBase未注册", LOG_ERROR);
        return;
    }

    m_pDataDictionaryServer = RequestServer<ISimuNPSDataDictionaryServer>();
    if (!m_pDataDictionaryServer) {
        LOGOUT("ISimuNPSDataDictionaryServer未注册", LOG_ERROR);
        return;
    }

    PIServerInterfaceBase logserver = RequestServerInterface<ILogServer>();
    if (logserver != nullptr) {
        logserver->connectNotify(100, this, SLOT(onReceiveHyperLink(unsigned int, const NotifyStruct &)));
    }

    PIServerInterfaceBase pSimulationManagerServer =
            RequestServerInterface<ISimulationManagerServer>(); // 仿真调度服务，消息响应用

    // 平台画板切换通知
    m_m_pUIServerIF->connectNotify(Notify_CurrIndex_TableWidget_Change, this,
                                   SLOT(onWorkTabIndexChange(unsigned int, const NotifyStruct &)));

    if (pSimulationManagerServer != nullptr) {
        pSimulationManagerServer->connectNotify(Notify_DrawingBoardRunning, this,
                                                SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSimulationManagerServer->connectNotify(Notify_DrawingBoardStopped, this,
                                                SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSimulationManagerServer->connectNotify(Notify_ProgressInit, this,
                                                SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSimulationManagerServer->connectNotify(Notify_ProgressRunning, this,
                                                SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSimulationManagerServer->connectNotify(Notify_RunStepVariables, this,
                                                SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    }

    // 添加长仿宋字体
    if (QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() + "/font/ChangFangSong.ttf") == -1) {
        LOGOUT("字体加载失败！", LOG_ERROR);
    }
}

void PluginGraphicsModelingServer::UnInit()
{
    saveBuriedData();
    if (m_pUIServer != nullptr) {
        m_pUIServer.clear();
    }
    if (m_m_pUIServerIF != nullptr) {
        m_m_pUIServerIF.clear();
    }
    if (m_pDataDictionaryServer != nullptr) {
        m_pDataDictionaryServer.clear();
    }
}

QStringList PluginGraphicsModelingServer::getAllDrawingBoardNames() const
{
    QStringList openedBoardNames;
    for (auto boardModel : m_modelMap) {
        openedBoardNames.append(boardModel->getName());
    }
    return openedBoardNames;
}

PDrawingBoardClass PluginGraphicsModelingServer::getDrawingBoardInfoById(const QString &drawingBoardUUID)
{
    if (m_modelMap.contains(drawingBoardUUID)) {
        return m_modelMap[drawingBoardUUID].dynamicCast<DrawingBoardClass>();
    }
    return nullptr;
}

bool PluginGraphicsModelingServer::hasDrawingBoard(const QString &boardName)
{
    auto boardList = m_modelMap.values();
    for (auto board : boardList) {
        if (board->getName() == boardName) {
            return true;
        }
    }
    return false;
}

bool PluginGraphicsModelingServer::hasDrawingBoardById(const QString &boardUUID)
{
    if (m_canvasWidgetMap.contains(boardUUID)) {
        return true;
    }
    return false;
}

QString PluginGraphicsModelingServer::getUUIDByBoardName(const QString &boardName)
{
    for (auto boardModel : m_modelMap) {
        if (boardModel->getName() == boardName) {
            return boardModel->getObjectUuid();
        }
    }
    return QString();
}

void PluginGraphicsModelingServer::getSubSystemAllParentModels(PModel subSystem, QList<PModel> &modelList)
{
    PModel model = subSystem->getParentModel();
    if (model) {
        modelList.push_front(model);
        getSubSystemAllParentModels(model, modelList);
    }
}

CanvasWidget *PluginGraphicsModelingServer::getCanvasByBoardUUID(const QString &boardUUID)
{
    return m_canvasWidgetMap.value(boardUUID, nullptr);
}

CanvasWidget *PluginGraphicsModelingServer::getCanvasByBoardName(const QString &name)
{
    QString uuid = getUUIDByBoardName(name);
    return getCanvasByBoardUUID(uuid);
}

ModelWidget *PluginGraphicsModelingServer::getModelWidgetByuuid(const QString &rootUuid, QString &uuid)
{
    CanvasWidget *widget = m_canvasWidgetMap.value(rootUuid);
    if (!widget)
        return nullptr;
    ModelWidget *modelWidget = widget->getModelWidgetByUUid(uuid);
    if (modelWidget)
        return modelWidget;
    return nullptr;
}

QString PluginGraphicsModelingServer::getBoardNameByUUID(const QString &boardUUID)
{
    if (!m_modelMap.contains(boardUUID)) {
        return QString();
    }
    return m_modelMap[boardUUID]->getName();
}

QString PluginGraphicsModelingServer::getActiveDrawingBoardUUID()
{
    return m_activeDrawingBoardUUID;
}

void PluginGraphicsModelingServer::onReceiveHyperLink(unsigned int i, const NotifyStruct &param)
{
    Kcc::ProjectManager::PIProjectManagerServer pProjMngServer = ModelingServerMng::getInstance().m_pProjectMngServer;
    Kcc::PropertyManager::PIPropertyManagerServer pPropMngServer =
            ModelingServerMng::getInstance().m_pPropertyManagerServer;
    if (!pProjMngServer || !pPropMngServer) {
        return;
    }
    const QString boardNameStack = param.paramMap["boardname"].toString();
    if (boardNameStack.isEmpty()) {
        return;
    }
    const QStringList boardNames = boardNameStack.split('/');
    const QString &topBoardName = boardNames.first();
    PModel pCurBoardModel = pProjMngServer->GetBoardModelByName(topBoardName);
    if (pCurBoardModel) {
        openDrawingBoard(pCurBoardModel, QString());
        const QString boardUUID = pCurBoardModel->getUUID();
        if (!boardUUID.isEmpty() && m_canvasWidgetMap.contains(boardUUID)) {
            CanvasWidget *activeDrawingBoard = qobject_cast<CanvasWidget *>(m_canvasWidgetMap[boardUUID]);
            for (int i = 1; i < boardNames.size(); ++i) {
                const QString cbduuid = pCurBoardModel->getUuidByChildName(boardNames[i]);
                pCurBoardModel = pCurBoardModel->getChildModel(cbduuid);
                if (!pCurBoardModel) {
                    return;
                }
                activeDrawingBoard->openCurrentModel(pCurBoardModel);
            }
            const QString blockuuid = param.paramMap["uuid"].toString();
            activeDrawingBoard->scrollToBlock(blockuuid);
            PModel pBlockModel = pCurBoardModel->getChildModel(blockuuid);
            pPropMngServer->ShowBlockProperty(pBlockModel);
        }
    }
}

void PluginGraphicsModelingServer::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    // 运行状态下画板不可操作
    if (code == Notify_DrawingBoardRunning) {
        setRuningStatus(true);
    } else if (code == Notify_DrawingBoardStopped) {
        setRuningStatus(false);
    } else if (code == Notify_ProgressInit) { // 如果是初始值
        int minimun = param.paramMap.value("minimun").toInt();
        int maximun = param.paramMap.value("maximun").toInt();
    } else if (code == Notify_ProgressRunning) {
        float value = param.paramMap.value("value").toFloat();
    } else if (code == Notify_RunStepVariables) {
        if (!m_bRuningStatus)
            return;
        m_mapRunValue.clear();
        QVariant value = param.paramMap["Variables"];
        if (!value.canConvert<Simu_Var::SimuVarBoard>())
            return;
        Simu_Var::SimuVarBoard boardValue = value.value<Simu_Var::SimuVarBoard>();
        setRunStepVariablesData(boardValue);
    }
}

void PluginGraphicsModelingServer::onWorkTabIndexChange(unsigned int code, const NotifyStruct &param)
{
    if (code != Notify_CurrIndex_TableWidget_Change)
        return;
    int curIndex = param.paramMap["CurrentIndex"].toInt();
    QString curTitle = param.paramMap["CurrentTitle"].toString();
    QString classType = param.paramMap["CurrentClass"].toString();

    // 切换为监控画板处理
    if (classType == "MonitorPanel") { }
}

bool PluginGraphicsModelingServer::openDrawingBoard(PModel pBoardModel, const QString &activeBlockID)
{
    if (createDrawingBoard(pBoardModel)) {
        QString boardUUID = pBoardModel->getUUID();

        if (boardUUID != "" && m_canvasWidgetMap.contains(boardUUID)) {
            CanvasWidget *activeDrawingBoard = qobject_cast<CanvasWidget *>(m_canvasWidgetMap[boardUUID]);
            if (activeDrawingBoard) {
                activeDrawingBoard->scrollToBlock(activeBlockID);
            }
        }
        return true;
    } else {
        return false;
    }
}

bool PluginGraphicsModelingServer::createDrawingBoard(PModel pBoardModel)
{
    if (!pBoardModel) {
        return false;
    }

    QString strTitle = pBoardModel->getName();
    QString strUUID = pBoardModel->getUUID();
    QMap<QString, QVariant> paramMap;
    paramMap[WORKAREA_TITLE] = strTitle;
    paramMap[WORKAREA_TAB_DO_NOT_RESTORE] = true;

    // 如果页面已经存在则激活
    if (m_canvasWidgetMap.contains(strUUID) && m_canvasWidgetMap.value(strUUID)) {
        if (m_pUIServer && m_pUIServer->GetMainUI()->WorkAreaContainsCurrWidget(strTitle)) {
            m_pUIServer->GetMainUI()->SwitchPanel(strTitle);
        }
        return true;
    }

    m_pUIServer->GetMainUI()->CreateWorkarea("CanvasWidget", paramMap);

    return true;
}

bool PluginGraphicsModelingServer::deleteDrawingBoard(const QString &drawingBoardName)
{
    // 关闭窗体
    if (m_pUIServer && m_pUIServer->GetMainUI()) {
        if (m_pUIServer->GetMainUI()->WorkAreaContainsCurrWidget(drawingBoardName)
            && getCanvasByBoardName(drawingBoardName)) {
            // 需要清除修改标志，否则会弹出是否保存窗口，框架接口修改后不需要此操作
            clearBoardWidgetModifyFlag(drawingBoardName);

            m_pUIServer->GetMainUI()->DeleteWorkareaWidget(getCanvasByBoardName(drawingBoardName));

            PIServerInterfaceBase pserver = RequestServerInterface<IPluginGraphicModelingServer>();
            NotifyStruct notifyStruct;
            notifyStruct.code = Notify_DrawingBoardClosed;
            notifyStruct.paramMap["BoardName"] = drawingBoardName;
            pserver->emitNotify(notifyStruct);
        }

        // 移除最近打开项目
        m_pUIServer->GetMainUI()->RemoveRecentProject(drawingBoardName);
    }
    if (ModelingServerMng::getInstance().m_pProjectMngServer) {
        return false;
    }
    auto board = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByName(drawingBoardName);
    if (board) {
        // 删除画板时，删除该画板的所有书签
        BookMark::getInstance().deleteBookMarkAction(board->getObjectUuid());
        m_modelMap.remove(board->getObjectUuid());
        m_canvasWidgetMap.remove(board->getObjectUuid());
    }

    return true;
}

// 修改构造画板
void PluginGraphicsModelingServer::modifyUserDefineBlock(const QString &blockName)
{
    // 如果页面已经存在则激活
    if (hasDrawingBoard(blockName)) {
        // 通过框架设置该页面为活动页
        if (m_pUIServer) {
            if (m_pUIServer->GetMainUI()->WorkAreaContainsCurrWidget(blockName)) {
                m_pUIServer->GetMainUI()->SwitchPanel(blockName);
            }
        }
        return;
    }

    if (!ModelingServerMng::getInstance().m_pProjectMngServer
        || !ModelingServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    auto boardModel = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByName(blockName); // 项目内
    if (!boardModel) {
        boardModel = ModelingServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(blockName); // 工具箱
    }
    if (boardModel) {
        // 依据boardModel结构创建自定义图层并打开
        this->createDrawingBoard(boardModel);
    }
}

void PluginGraphicsModelingServer::drawingBoardLoaded(CanvasWidget *canvasWidget)
{
    if (canvasWidget) {
        PModel pBoardModel = canvasWidget->getBoardModel();
        QString uuid = pBoardModel->getUUID();
        m_canvasWidgetMap[uuid] = canvasWidget;
        m_modelMap[uuid] = pBoardModel;
    }
}

void PluginGraphicsModelingServer::drawingBoardClosed(const QString &blockUUID)
{
    QString uuid = blockUUID;
    if (m_canvasWidgetMap.contains(uuid)) {
        m_canvasWidgetMap.remove(uuid);
    }
    if (m_modelMap.contains(uuid)) {
        m_modelMap.remove(uuid);
    }
}

void PluginGraphicsModelingServer::drawingBoardActived(const QString &activeDrawingBoardUUID)
{
    updateActiveDrawingBoardUUID(activeDrawingBoardUUID);

    // 1.发送通知：先请求对应的服务，这个服务对象类型为必须是PIServerInterfaceBase
    PIServerInterfaceBase pserver = RequestServerInterface<IPluginGraphicModelingServer>();

    if (pserver && m_activeDrawingBoardUUID != "" && m_canvasWidgetMap.contains(activeDrawingBoardUUID)) {
        CanvasWidget *activeDrawingBoard = qobject_cast<CanvasWidget *>(m_canvasWidgetMap[m_activeDrawingBoardUUID]);
        // 2.确定发送参数
        NotifyStruct notifyStruct;
        notifyStruct.code = Notify_DrawingBoardActived;
        notifyStruct.paramMap["BoardType"] = activeDrawingBoard->getBoardModel()->getModelType();
        notifyStruct.paramMap["BoardUUID"] = m_activeDrawingBoardUUID;
        // 3.发送通知
        pserver->emitNotify(notifyStruct);
    }
}

void PluginGraphicsModelingServer::drawingBoardNotActived(const QString &blockUUID)
{
    if (m_activeDrawingBoardUUID == blockUUID) {
        updateActiveDrawingBoardUUID("");
    }
}

void PluginGraphicsModelingServer::clearBoardWidgetModifyFlag(const QString &blockName)
{
    QString uuid = getUUIDByBoardName(blockName);
    if (m_canvasWidgetMap.contains(uuid)) {
        CanvasWidget *boardWidget = qobject_cast<CanvasWidget *>(m_canvasWidgetMap[uuid]);
        boardWidget->removeModifiedLabel();
    }
}

void PluginGraphicsModelingServer::setDrawBoardModified(const QString &uuid)
{
    if (m_canvasWidgetMap.contains(uuid)) {
        CanvasWidget *boardWidget = qobject_cast<CanvasWidget *>(m_canvasWidgetMap[uuid]);
        if (boardWidget) {
            boardWidget->addModifiedLabel();
        }
    }
}

QRectF PluginGraphicsModelingServer::getMinRect(QString &boardUUID)
{
    CanvasWidget *canvas = getCanvasByBoardUUID(boardUUID);
    if (canvas != nullptr) {
        return canvas->getMinRect();
    }
    return QRectF();
}

QStringList PluginGraphicsModelingServer::getAllOpenBoardUUID()
{
    return m_canvasWidgetMap.keys();
}

Simu_Var::SimuVarBoard PluginGraphicsModelingServer::getRunVariable(QString &uuid)
{
    if (m_mapRunValue.contains(uuid)) {
        return m_mapRunValue[uuid];
    }
    return Simu_Var::SimuVarBoard();
}

bool PluginGraphicsModelingServer::setDisplayAndHighlightModel(PModel boardModel, PModel blockModel,
                                                               PModel subSystemModel)
{
    if (boardModel == nullptr) {
        return false;
    }

    QList<PModel> modelList;
    auto topCanvasWidget = getCanvasByBoardUUID(boardModel->getUUID());
    if (!topCanvasWidget) {
        return false;
    }
    if (subSystemModel == nullptr) {
        if (blockModel == nullptr) {
            return false;
        }

        auto topModelWidget = topCanvasWidget->getTopModelWidget();
        if (topModelWidget == nullptr) {
            return false;
        }

        auto topCanvasContext = topModelWidget->getCanvasContext();
        if (topCanvasContext == nullptr) {
            return false;
        }
        topCanvasContext->getCanvasProperty().setGotoFromHighLightSourcceList(QStringList());

        modelList.push_front(boardModel);
        topCanvasWidget->m_navWidget->addItemList(modelList);
        topCanvasWidget->openTopBoardModel(boardModel);
        topCanvasWidget->scrollToBlock(blockModel->getUUID());
        return true;
    } else {
        modelList.push_front(subSystemModel);
        getSubSystemAllParentModels(subSystemModel, modelList);
        topCanvasWidget->m_navWidget->addItemList(modelList);
        topCanvasWidget->openCurrentModel(subSystemModel);
        auto pModelWidget = topCanvasWidget->getModelWidgetByUUid(subSystemModel->getUUID());
        if (pModelWidget) {
            pModelWidget->getCanvasView()->scrollToBlock(blockModel->getUUID());
            return true;
        }
    }
    return false;
}

void PluginGraphicsModelingServer::VerifyStatus(QList<PModel> boardModels)
{
    for (auto model : boardModels) {
        auto canvasWidget = getCanvasByBoardUUID(model->getUUID());
        if (canvasWidget) {
            canvasWidget->verifyStatus();
        }
    }
}

bool PluginGraphicsModelingServer::getRunningStatus()
{
    return m_bRuningStatus;
}

QString PluginGraphicsModelingServer::getValidUserDefinedDrawBoardName(QString prefix, QString suffix)
{
    QString validBoardName;
    const QString constName = prefix + "构造模块";
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return QString();
    }
    int i = 0;
    while (i < 1000) {
        bool isValid = true;
        i++;

        // 检查画板是否重名
        auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
        foreach (auto board, boardList) {
            auto pDrawingBoardClass = board.dynamicCast<DrawingBoardClass>();
            if (!pDrawingBoardClass) {
                continue;
            }
            QString boardName = pDrawingBoardClass->getName();
            if (boardName.compare(validBoardName, Qt::CaseInsensitive) == 0) {
                isValid = false;
            }
        }
        // 控制自定义模块需要检查名称是否存在
        if (ModelingServerMng::getInstance().m_pProjectMngServer->IsExistModel(validBoardName)) {
            isValid = false;
        }
        // 校验画板名称是否合法
        if (!validBoardName.contains(NPS_FileNameRule)) {
            isValid = false;
        }
        // 校验新建画板名称是否为监控面板
        if (!QString::compare(validBoardName, "监控面板")) {
            isValid = false;
        }

        if (isValid) {
            break;
        }

        validBoardName = QString("%1%2").arg(constName).arg(i);
    }
    validBoardName += suffix;

    return validBoardName;
}

// 模块原型下一次创建时使用的索引
int PluginGraphicsModelingServer::getBlockPrototypeNextIndex(const QString &prototypeName, PModel pBoardModel,
                                                             const QString &blockUUID, bool useIndexZero)
{
    if (!pBoardModel.isNull()) {
        for (int i = useIndexZero ? 0 : 1;; ++i) {
            auto blockMap = pBoardModel->getChildModels();
            auto iter = blockMap.constBegin();
            QString resultName;
            if (i == 0) {
                resultName = QString("%1").arg((prototypeName.size() == 0) ? "1" : prototypeName);
                if (resultName.endsWith(' ')) { // 特殊处理：空格结尾
                    i = i + 1;
                    resultName = QString("%1%2").arg(prototypeName).arg(i);
                }
            } else {
                resultName = QString("%1%2").arg(prototypeName).arg(i);
            }

            bool isTrue = true;
            while (iter != blockMap.constEnd()) {
                QString plockNmae = iter.value()->getName();
                if (resultName == plockNmae && blockUUID != iter.value()->getUUID()) {
                    isTrue = false;
                    break;
                }
                iter++;
            }
            if (isTrue) {
                return i;
            }
        }
    }
    return 0;
}

void PluginGraphicsModelingServer::updateActiveDrawingBoardUUID(QString uuid)
{
    m_activeDrawingBoardUUID = uuid;
    NotifyStruct notify;
    notify.code = Notify_DrawBoardChanged;
    notify.paramMap["drawboarduuid"] = uuid;
    if (m_canvasWidgetMap.contains(uuid)) {
        notify.paramMap["CanvasWidget"] = QVariant::fromValue((void *)m_canvasWidgetMap[uuid]);
    } else {
        notify.paramMap["CanvasWidget"] = QVariant::fromValue((void *)nullptr);
    }
    emitNotify(notify);
}

void PluginGraphicsModelingServer::setRuningStatus(bool bRunning)
{
    m_bRuningStatus = bRunning;
    for (auto canvasWidget : m_canvasWidgetMap) {
        if (canvasWidget) {
            canvasWidget->setRunningStatus(bRunning);
        }
    }
}

void PluginGraphicsModelingServer::setBoardPowerFlowState(const QString &blockUUID, bool status)
{
    if (!m_canvasWidgetMap.contains(blockUUID))
        return;
    CanvasWidget *boardWidget = qobject_cast<CanvasWidget *>(m_canvasWidgetMap[blockUUID]);
    if (!boardWidget)
        return;
    PModel pBoardModel = boardWidget->getBoardModel();
    if (pBoardModel == nullptr) {
        return;
    }
    PCanvasContext canvasContext = pBoardModel->getCanvasContext();
    if (canvasContext == nullptr) {
        return;
    }
    bool signalsBlocked = canvasContext->signalsBlocked();
    canvasContext->blockSignals(false);
    canvasContext->setIsShowPowerFlowData(status);
    canvasContext->blockSignals(signalsBlocked);
}

void PluginGraphicsModelingServer::setRunStepVariablesData(Simu_Var::SimuVarBoard &boardValue)
{
    QString uuid = QString::fromStdString(boardValue.uuid);
    m_mapRunValue[uuid] = boardValue;
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    PModel model = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(uuid);
    if (model && model->getTopParentModel()) {
        ModelWidget *widget = getModelWidgetByuuid(model->getTopParentModel()->getUUID(), uuid);
        if (widget) {
            widget->setRunStepVariablesData(boardValue);
        }
    }
    map<string, Simu_Var::SimuVarBlock> blockMap = boardValue.blockMap;
    for (auto iter : blockMap) {
        Simu_Var::SimuVarBlock block = iter.second;
        for (auto paramVecIter : block.defBoard) {
            setRunStepVariablesData(paramVecIter);
        }
    }
}

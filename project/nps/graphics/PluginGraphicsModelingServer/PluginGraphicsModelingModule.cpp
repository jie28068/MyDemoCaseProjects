#include "PluginGraphicsModelingModule.h"
#include "CanvasWidget.h"
#include "CombineBoardModel.h"
#include "ComplexBoardModel.h"
#include "ControlBoardModel.h"
#include "CoreLib/ServerManager.h"
#include "ElecBoardModel.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingKernel/ActionManager.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "GraphicsModelingTool.h"
#include "Manager/ModelingServerMng.h"
#include "ModelWidget.h"
#include "QTimer"
#include <KLWidgets/KMessageBox.h>
#include <QDesktopWidget>
#include <QPushButton>
#include <QUuid>

/**
静态注册（当dll被加载时会调用），每个模块都必须使用此宏（第一个参数是当前类的类型；第二个是组的名称，
一般为空；第三个是显示名称，一般是类型名称）
**/
REG_MODULE_BEGIN(PluginGraphicsModelingModule, "", "PluginGraphicsModelingModule")
REG_MODULE_END(PluginGraphicsModelingModule)

USE_LOGOUT_("PluginGraphicsModelingModule")

static const QString SM_PROTOTYPE_NAME = "SynchronousMachine";
static const QString EI_PROTOTYPE_NAME = "ElectricalInterface";

PluginGraphicsModelingModule::PluginGraphicsModelingModule(QString moduleName)
    : Module(Module_Type_Normal, moduleName), m_actRefreshTimer(nullptr)
{
    // 注册GrapicModeling服务
    PluginGraphicsModelingServer *GraphicsModelingServer = new PluginGraphicsModelingServer();
    RegServer<IPluginGraphicModelingServer>(GraphicsModelingServer);

    m_bOnRunning = false;

    // 书签序列化
    connect(&BookMark::getInstance(), SIGNAL(bookMarkSerialize(QVariantMap)), this,
            SLOT(onBookMarkSerialize(QVariantMap)));
}

PluginGraphicsModelingModule::~PluginGraphicsModelingModule() { }

void PluginGraphicsModelingModule::init(KeyValueMap &params)
{
    if (!ModelingServerMng::getInstance().init()) {
        LOGOUT("ModelingServerMng init false", LOG_ERROR);
        return;
    }

    initActions();
    initTimer();
    initServerNotify();

    setBaseLogHandler(graphicsKernelLogHandler);

    // FMU
    m_pExportDrawingboardToFMU = new ExportDrawingboardToFMU();
    m_pPopExportToFMUDlg = new KCustomDialog();
    m_pPopExportToFMUDlg->resize(550, 150);
    m_pPopExportToFMUDlg->setTitle(tr("board is derived for the fmu"));        // 画板导出为FMU
    m_pExportPbtnOK = m_pPopExportToFMUDlg->addCustomButton(tr("ok"));         // 确定
    m_pExportPbtnCancel = m_pPopExportToFMUDlg->addCustomButton(tr("cancel")); // 取消
    m_pPopExportToFMUDlg->setCentralWidget(m_pExportDrawingboardToFMU);
    m_pPopExportToFMUDlg->setDefaultButton(m_pExportPbtnOK);

    QObject::connect(m_pExportPbtnOK, SIGNAL(released()), this, SLOT(onExportToFmuPbtnOKclicked()));
    QObject::connect(m_pExportPbtnCancel, SIGNAL(released()), this, SLOT(onExportToFmuPbtnCancelclicked()));

    // 初始化时书签反序列化
    onBookMarkDeserialize();
}

void PluginGraphicsModelingModule::unInit(KeyValueMap &saveParams)
{
    // 关闭定时器
    if (m_actRefreshTimer) {
        m_actRefreshTimer->stop();
    }

    ModelingServerMng::getInstance().unInit();
}

void PluginGraphicsModelingModule::graphicsKernelLogHandler(QString msg, int level)
{
    Kcc::LOG_LEVEL kccLogLevel;
    switch (level) {
    case -2:
        kccLogLevel = Kcc::LOG_ERROR;
        break;
    case -1:
        kccLogLevel = Kcc::LOG_WARNING;
        break;
    case 0:
        kccLogLevel = Kcc::LOG_NORMAL;
        break;
    case 1:
        kccLogLevel = Kcc::LOG_DEBUG;
        break;
    case 2:
        kccLogLevel = Kcc::LOG_OPERATION_FAILURE;
        break;
    case 3:
        kccLogLevel = Kcc::LOG_OPERATION_WARNING;
        break;
    case 4:
        kccLogLevel = Kcc::LOG_OPERATION_SUCCESS;
        break;
    case 100:
        kccLogLevel = Kcc::LOG_SYSTEM_LEVEL;
        break;
    default:;
    }
    if (ModelingServerMng::getInstance().m_pLogSeverIF) {
        LOGOUT(msg, kccLogLevel);
    }
}

void PluginGraphicsModelingModule::initActions()
{
    mainWigetAddAction();

    m_pModifyDrawingBoardAction = new QAction(this);
    m_pModifyDrawingBoardAction->setText(tr("modify the drawing board")); // 修改画板
    QObject::connect(m_pModifyDrawingBoardAction, SIGNAL(triggered()), this, SLOT(onModifyDrawingBoard()));

    m_pSaveDrawingBoardAction = new QAction(this);
    m_pSaveDrawingBoardAction->setText(tr("save the drawing board")); // 保存画板
    m_pSaveDrawingBoardAction->setShortcuts(QKeySequence::Save);
    QObject::connect(m_pSaveDrawingBoardAction, SIGNAL(triggered()), this, SLOT(onSaveDrawingBoard()));

    auto &pUIServer = ModelingServerMng::getInstance().m_pUIServer;
    // 注册自己的菜单按钮（参数和Menu.xml对应）
    if (pUIServer->GetMainUI()) {
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "saveDrawingBoardAction",
                                               m_pSaveDrawingBoardAction);
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "copy",
                                               ActionManager::getInstance().getAction(ActionManager::Copy));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "cut",
                                               ActionManager::getInstance().getAction(ActionManager::Cut));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "delete",
                                               ActionManager::getInstance().getAction(ActionManager::Delete));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "paste",
                                               ActionManager::getInstance().getAction(ActionManager::Paste));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "undo",
                                               ActionManager::getInstance().getAction(ActionManager::Undo));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "redo",
                                               ActionManager::getInstance().getAction(ActionManager::Redo));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "zoomIn",
                                               ActionManager::getInstance().getAction(ActionManager::ZoomIn));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "zoomOut",
                                               ActionManager::getInstance().getAction(ActionManager::ZoomOut));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "rotateClockwise",
                                               ActionManager::getInstance().getAction(ActionManager::RotateClockwise));
        pUIServer->GetMainUI()->RegisterAction("PluginGraphicsModelingModule", "rotate180",
                                               ActionManager::getInstance().getAction(ActionManager::Rotate180));
        pUIServer->GetMainUI()->RegisterAction(
                "PluginGraphicsModelingModule", "rotateAntiClockwise",
                ActionManager::getInstance().getAction(ActionManager::RotateAntiClockwise));
        pUIServer->GetMainUI()->RegisterAction(
                "PluginGraphicsModelingModule", "drawBoardProperty",
                ActionManager::getInstance().getAction(ActionManager::DrawBoardProperty));
    }
    // 未打开画板时,Action全部置灰
    if (ModelingServerMng::getInstance().m_pUIServer) {
        QList<QWidget *> widgetList = ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->WorkAreaWidgets();
        if (widgetList.isEmpty()) {
            // todo liwenyu 这里没有激活的窗口
        }
    }

    connect(ActionManager::getInstance().getAction(ActionManager::DrawBoardProperty), SIGNAL(triggered()), this,
            SLOT(onModifyDrawingBoard()));

    // 画板右键action绑定
    connect(ActionManager::getInstance().getAction(ActionManager::Set), SIGNAL(triggered()), this,
            SLOT(onModifyDrawingBoard()));
    connect(ActionManager::getInstance().getAction(ActionManager::SetReplica), SIGNAL(triggered()), this,
            SLOT(onModifyDrawingBoard()));
    connect(ActionManager::getInstance().getAction(ActionManager::EditAndBrowseData), SIGNAL(triggered()), this,
            SLOT(onOpenDataManagerView()));
    connect(ActionManager::getInstance().getAction(ActionManager::ExportBorad), SIGNAL(triggered()), this,
            SLOT(onExportDrawingBoard()));
    connect(ActionManager::getInstance().getAction(ActionManager::ExportFMU), SIGNAL(triggered()), this,
            SLOT(onExportDrawingBoardToFMU()));
}

void PluginGraphicsModelingModule::initTimer()
{
    // fixme 画板区域当前激活页面为监控页面时没有消息发出，需要定时查询当前激活页面，更新菜单状态
    m_actRefreshTimer = new QTimer(this);
    QObject::connect(m_actRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshActionState()));
    m_actRefreshTimer->start(200);
}

void PluginGraphicsModelingModule::initServerNotify()
{
    // 接收项目管理服务通知
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardDestroyed, this,
            SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardCreate, this,
            SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardSaved, this,
            SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardFileRenamed, this,
            SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_CurrentProjectChanged, this,
            SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));

    // 接收画板运行状态
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_DrawingBoardRunning, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_DrawingBoardStopped, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    // 接收潮流计算状态
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_PowerFlowFinished, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    // 接收单步调试运行状态
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_RunStepVariables, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));

    // 画板服务
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServerIF != nullptr) {
        ModelingServerMng::getInstance().m_pGraphicsModelingServerIF->connectNotify(
                Notify_DrawBoardChanged, this,
                SLOT(onRecieveGraphiceServerManagerMsg(unsigned int, const NotifyStruct &)));
    }

    // 属性管理服务
    ModelingServerMng::getInstance().m_pPropertyManagerServerIF->connectNotify(
            Notify_DrawingBoardCreate, this,
            SLOT(onRecievePropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pPropertyManagerServerIF->connectNotify(
            Notify_DrawingBoardModified, this,
            SLOT(onRecievePropertyManagerServerMsg(unsigned int, const NotifyStruct &)));

    // 网络管理服务
    // ModelingServerMng::getInstance().m_pNetworkManagerServerIF->connectNotify(
    //         Notify_BlockChanged, this, SLOT(onRecieveNetworkManagerMsg(unsigned int, const NotifyStruct &)));
}

void PluginGraphicsModelingModule::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    if (code == Notify_DrawingBoardRunning) {
        m_bOnRunning = true;
    } else if (code == Notify_DrawingBoardStopped) {
        m_bOnRunning = false;

    } else if (code == Notify_PowerFlowFinished) {
        if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
            return;
        }
        if (param.paramMap["ErrInfo"].toString().isEmpty()) {
            QString boardUUID = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->setBoardPowerFlowState(boardUUID, true);
        }
    }
}

void PluginGraphicsModelingModule::onRecievePropertyManagerServerMsg(unsigned int code, const NotifyStruct &param)
{
    QString boardName = param.paramMap[PMKEY::MODEL_NAME].toString();
    int boardType = param.paramMap[PMKEY::MODEL_TYPE].toInt();
    QString boardUUID = param.paramMap[PMKEY::MODEL_UUID].toString();
    QString oldBoardName = param.paramMap[PMKEY::MODEL_OLD_NAME].toString();
    bool isTriggetModif = param.paramMap[PMKEY::BOARD_TRIGGER_MODIFY].toBool();

    if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer
        || !ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }

    auto boardModel = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(boardUUID);
    if (!boardModel) {
        return;
    }

    if (Notify_DrawingBoardCreate == code) {
        if (ModelingServerMng::getInstance().m_pGraphicsModelingServer->createDrawingBoard(boardModel)) {
            int boardType = boardModel->getModelType();
            // 如果为自定义类型画板，则由数据管理器通知元器件窗口
            if (boardType == CombineBoardModel::Type) {
                ModelingServerMng::getInstance().m_pProjectMngServer->ConfirmUserDefinedBoardModel(
                        boardModel->getName());
            }
        }
    } else if (Notify_DrawingBoardModified == code) {
        CanvasWidget *canvasWidget =
                ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(boardUUID);
        if (!canvasWidget) {
            return;
        }
        canvasWidget->triggerContensChangedSlot(isTriggetModif);

        // 画板名称修改
        if (boardName != oldBoardName) {
            // 画板名称变化在setName时已经对画板进行重命名保存操作
            // 若当前画板有*号则移除
            if (canvasWidget->isModified()) {
                canvasWidget->removeModifiedLabel();
            }
        }

        // // 同步数据字典
        // QString simulationName = NPS_RealTimeSimulationName;
        // if (ModelingServerMng::getInstance().m_pDataDictionaryServer->isHaveDictionaryElement(simulationName + "."
        //                                                                                       + oldBoardName)) {
        //     bool rename = ModelingServerMng::getInstance().m_pDataDictionaryServer->RenameDictionaryElement(
        //             simulationName + "." + oldBoardName, boardName);
        //     if (!rename) {
        //         LOGOUT("数据字典同步画板名失败", LOG_WARNING);
        //     }
        // }
    }
}

void PluginGraphicsModelingModule::onRecieveNetworkManagerMsg(unsigned int code, const NotifyStruct &param)
{
    // fixme 网络模型管理器服务没有了
    // switch (code) {
    // case Notify_BlockChanged: {
    //     QString blockUUID = param.paramMap["blockUUID"].toString();
    //     QString boardUUID = param.paramMap["boardUUID"].toString();
    //     if (blockUUID.isEmpty() || boardUUID.isEmpty()) {
    //         return;
    //     }
    //     if (!ModelingServerMng::getInstance().m_pModelDataMngServer) {
    //         return;
    //     }
    //     auto boardList = ModelingServerMng::getInstance().m_pModelDataMngServer->GetAllBoardModel();
    //     auto pDrawBoard = GraphicsModelingTool::getBoradByUUID(boardUUID, boardList);
    //     if (pDrawBoard != nullptr) {
    //         if (pDrawBoard->getModelType() == ElecBoardModel::Type) {
    //             ModelingServerMng::getInstance().m_pDrawingBoardHelperServer->VerifyDrawingBoardStatus(pDrawBoard);
    //         }
    //         QWidget *wiget =
    //                 ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(boardUUID);
    //         if (wiget != nullptr) {
    //             CanvasWidget *canvasWiget = dynamic_cast<CanvasWidget *>(wiget);
    //             if (canvasWiget != nullptr) {
    //                 canvasWiget->onContensChanged();
    //             }
    //         }
    //         break;
    //     }
    // } break;
    // default:
    //     break;
    // }
}

void PluginGraphicsModelingModule::onRecevieProjectManagerServerMsg(uint code, const NotifyStruct &param)
{
    if (!ModelingServerMng::getInstance().m_pUIServer || !ModelingServerMng::getInstance().m_pGraphicsModelingServer
        || !ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    if (code == ProjectManager::IPM_Notify_CurrentProjectChanged) {
        onBookMarkDeserialize();
    } else if (code == ProjectManager::IPM_Notify_DrawingBoardDestroyed) {
        QString boardName = param.paramMap["name"].toString();

        // 关闭窗体
        if (ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->WorkAreaContainsCurrWidget(boardName)) {
            // 需要清除修改标志，否则会弹出是否保存窗口，框架接口修改后不需要此操作
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->clearBoardWidgetModifyFlag(boardName);
            auto findCanvas =
                    ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardName(boardName);
            if (findCanvas) {
                ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->DeleteWorkareaWidget(findCanvas);
            }
        }
    } else if (code == ProjectManager::IPM_Notify_DrawingBoardSaved) {

        return; // 自定义构造型原型变更后模块实例暂时不标红或者标黄

        int type = param.paramMap["type"].toInt();
        if (type != Model::Combine_Board_Type)
            return;
        QString blockPrototypeName = param.paramMap["name"].toString();
        auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
        QStringList openedBoardNames =
                ModelingServerMng::getInstance().m_pGraphicsModelingServer->getAllDrawingBoardNames();
        for (auto boardModel : boardList) {
            QString boardName = boardModel->getName();
            if (boardName.size() == 0 || openedBoardNames.contains(boardName)) {
                continue;
            }
            // 不相等需要判断画板内部block是否包含当前变化的构造型模块，如果有提示当前block。
            bool isChange = false;
            auto childModels = boardModel->getChildModels();
            for (auto model : childModels) {
                if (model->getModelType() == CombineBoardModel::Type
                    && model->getPrototypeName() == blockPrototypeName) {
                    model->setState(Block::StateWarring);
                    model->setStateInfo(ERROR_INFO_PROTOTYPE_MOD_NOUSE);
                    isChange = true;
                }
            }
            if (isChange) {
                ModelingServerMng::getInstance().m_pProjectMngServer->SaveBoardModel(boardModel);
            }
        }
    } else if (code == ProjectManager::IPM_Notify_CodePrototypeSaved) { // 项目自定义代码修改
        QString blockPrototypeName = param.paramMap["name"].toString();
        auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
        QStringList openedBoardNames =
                ModelingServerMng::getInstance().m_pGraphicsModelingServer->getAllDrawingBoardNames();
        for (auto boardModel : boardList) {
            QString boardName = boardModel->getName();
            if (boardName.size() == 0 || openedBoardNames.contains(boardName)) {
                continue;
            }
            bool isChange = false;
            auto childModels = boardModel->getChildModels();
            for (auto model : childModels) {
                if (model && model->getPrototypeName() == blockPrototypeName
                    && ModelingServerMng::getInstance().m_pProjectMngServer) {
                    auto curProj = ModelingServerMng::getInstance().m_pProjectMngServer->GetCurProject();
                    if (curProj) {
                        auto controlBlockModel = curProj->getModel(blockPrototypeName); // 项目中自定义代码型模块
                        if (controlBlockModel) {
                            auto oldBlockVerID = controlBlockModel->getVerID();
                            auto curBlockVerID = model->getVerID();
                            QStringList oldveridlist = oldBlockVerID.split(".", QString::SkipEmptyParts);
                            QStringList curveridlist = curBlockVerID.split(".", QString::SkipEmptyParts);
                            if (oldveridlist.size() < 3 || curveridlist.size() < 3) {
                                continue;
                            }
                            if (oldveridlist[0] != curveridlist[0]) { // 大版本不一致
                                model->setState(Block::StateError);
                                model->setStateInfo(ERROR_INFO_PROTOTYPE_MOD_NOUSE);
                                isChange = true;
                            } else if (oldveridlist[1] != curveridlist[1]) { // 中版本不一致
                                model->setState(Block::StateWarring);
                                model->setStateInfo(ERROR_INFO_PROTOTYPE_MOD_MAYEFFECT);
                                isChange = true;
                            }
                        }
                    }
                }
            }
            if (isChange) {
                ModelingServerMng::getInstance().m_pProjectMngServer->SaveBoardModel(boardModel);
            }
        }
    } else if (code == ProjectManager::IPM_Notify_DrawingBoardFileRenamed) {
        QString oldBoardName = param.paramMap["oldName"].toString();
        QString newName = param.paramMap["newName"].toString();
        QString uuid = param.paramMap["uuid"].toString();
        // 同步书签
        BookMark::getInstance().changeBoardName(oldBoardName, newName);
        // 移除最近打开的画板
        // ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->RemoveRecentProject(oldBoardName);
        // 若画板打开,修改工作区标题
        CanvasWidget *canvasWidget =
                ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(uuid);
        if (canvasWidget) {
            canvasWidget->boardNameChanged(); // 画板修改，工作区变更等
        }
    }
}

void PluginGraphicsModelingModule::onBookMarkSerialize(QVariantMap map)
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer || !ModelingServerMng::getInstance().m_pUIServer) {
        return;
    }
    // 序列化
    QVariantMap drawingBoardLabel =
            ModelingServerMng::getInstance().m_pProjectMngServer->GetProjectConfig(KL_PRO::BOARD_LABAL).toMap();
    // 找到了便插入数据
    if (drawingBoardLabel.find("bookMark") != drawingBoardLabel.end()) {
        drawingBoardLabel["bookMark"] = map["bookMark"];
    } else { // 没找到就新插入该字段
        drawingBoardLabel.insert("bookMark", map["bookMark"]);
    }
    // 无任何打开画板时快捷键打开画板
    QWidget *mainWiget = ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->GetMainWidget();
    // 删除重复的书签action，防止快捷键冲突
    QList<QAction *> actions = mainWiget->actions();
    for (QAction *act : actions) {
        // 平台原有的快捷键跳过
        if (act->shortcut().toString() == "Ctrl+P" || act->shortcut().toString() == "Ctrl+A") {
            continue;
        }
        mainWiget->removeAction(act);
    }
    auto bookmarks = *BookMark::getInstance().getBookMarks();
    for (auto value : bookmarks) {
        if (!value->shortcut().isEmpty()) {
            if (!isActionExist(mainWiget, value)) {
                connect(value, &QAction::triggered, this, &PluginGraphicsModelingModule::onBookMarkOpen);
                mainWiget->addAction(value);
            }
        }
    }
    ModelingServerMng::getInstance().m_pProjectMngServer->SetProjectConfig(KL_PRO::BOARD_LABAL, drawingBoardLabel);
}

bool PluginGraphicsModelingModule::isActionExist(QWidget *widget, QAction *action)
{
    QList<QAction *> actions = widget->actions();
    for (QAction *act : actions) {
        if (act == action) {
            return true;
        }
    }
    return false;
}

void PluginGraphicsModelingModule::onBookMarkOpen()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    QAction *ac = (QAction *)(sender());
    QVariantMap map = ac->data().toMap();
    QString uuid = map.find("uuid").value().toString();
    auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
    foreach (auto board, boardList) {
        if (board->getUUID() == uuid) {
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->createDrawingBoard(
                    board.dynamicCast<DrawingBoardClass>());
            ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->SwitchWorkareaWidget(board->getName());
        }
    }
}

void PluginGraphicsModelingModule::onBookMarkDeserialize()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    // 返序列化
    auto drawingBoardLabel =
            ModelingServerMng::getInstance().m_pProjectMngServer->GetProjectConfig(KL_PRO::BOARD_LABAL).toMap();
    BookMark::getInstance().deserialize(drawingBoardLabel);
}

void PluginGraphicsModelingModule::onModifyDrawingBoard()
{
    // 获取当前活动页面标题
    QString activeDrawingDoardUUID =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
    if (activeDrawingDoardUUID.isEmpty()) {
        return;
    }
    auto canvasWidget =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(activeDrawingDoardUUID);
    if (!canvasWidget)
        return;
    auto modelWidget = canvasWidget->getCurModelWidget();
    if (!modelWidget)
        return;
    auto model = modelWidget->getModel();
    // 获取活动页面数据结构
    PDrawingBoardClass pDrawingBoardInfo = model.dynamicCast<DrawingBoardClass>();
    if (pDrawingBoardInfo == nullptr) {
        LOGOUT(QString(tr("query page%1 information failure")).arg(modelWidget->getCanvasContext()->uuid()),
               LOG_ERROR); // 查询页面%1 信息失败
        return;
    }
    if (ModelingServerMng::getInstance().m_pPropertyManagerServer) {
        ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowDrawBoardProperty(pDrawingBoardInfo);
    }
}

void PluginGraphicsModelingModule::onOpenDataManagerView()
{
    if (ModelingServerMng::getInstance().m_pUIServer == nullptr) {
        return;
    }
    // 打开数据管理器界面
    QAction *mActionNew = ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->GetRegisterAction(
            "PluginDataManagerViewopenDataManager");
    if (mActionNew)
        mActionNew->activate(QAction::Trigger);
}

void PluginGraphicsModelingModule::onSaveDrawingBoard()
{
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer == nullptr) {
        return;
    }
    QString uuid = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
    CanvasWidget *canvasWidget = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(uuid);
    if (canvasWidget) {
        canvasWidget->Save();
    }
}

static bool PFNCompileCallback_func(void *pParam, const QString &strInfo, CompileCode code)
{
    if (code == CompileCode::Succeed) {
        LOGOUT(strInfo + QString("，导出画板完成"));
    } else if (code == CompileCode::Failed) {
        LOGOUT(strInfo + QString("，导出画板失败"), LOG_ERROR);
    }
    return true;
}

void PluginGraphicsModelingModule::onExportDrawingBoard()
{
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer == nullptr
        || ModelingServerMng::getInstance().m_pICodeManagerServer == nullptr) {
        return;
    }
    QString boardUUID = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
    PDrawingBoardClass pDrawingBoardClasses =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getDrawingBoardInfoById(boardUUID);
    ModelingServerMng::getInstance().m_pICodeManagerServer->ExportCtrlSys(pDrawingBoardClasses, PFNCompileCallback_func,
                                                                          nullptr);
}

void PluginGraphicsModelingModule::onExportDrawingBoardToFMU()
{
    m_pPopExportToFMUDlg->show();
}

static bool PFNExportFMUCallback_func(void *pParam, const QString &strInfo, CompileCode code)
{
    if (code == CompileCode::Succeed) {
        LOGOUT(strInfo + QString("，导出FMU完成"));
    } else if (code == CompileCode::Failed) {
        LOGOUT(strInfo + QString("，导出FMU失败"), LOG_ERROR);
    }
    return true;
}

void PluginGraphicsModelingModule::onExportToFmuPbtnOKclicked()
{
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer == nullptr
        || ModelingServerMng::getInstance().m_pICodeManagerServer == nullptr) {
        return;
    }
    FmiParam params;
    if (m_pExportDrawingboardToFMU->getFmiParams(params)) {
        QString boardUUID = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
        PDrawingBoardClass pDrawingBoardClasses =
                ModelingServerMng::getInstance().m_pGraphicsModelingServer->getDrawingBoardInfoById(boardUUID);
        ModelingServerMng::getInstance().m_pICodeManagerServer->ExportFMU(pDrawingBoardClasses, params,
                                                                          PFNExportFMUCallback_func, nullptr);
        m_pPopExportToFMUDlg->hide();
    }
    return;
}

void PluginGraphicsModelingModule::onExportToFmuPbtnCancelclicked()
{
    m_pPopExportToFMUDlg->hide();
    return;
}

void PluginGraphicsModelingModule::refreshActionState()
{
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer == nullptr) {
        return;
    }
    ActionManager &actionMgr = ActionManager::getInstance();
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID() == "") {
        // m_pModifyDrawingBoardAction->setEnabled(false);
        m_pSaveDrawingBoardAction->setEnabled(false);
        actionMgr.getAction(ActionManager::DrawBoardProperty)->setEnabled(false);
    } else {
        // m_pModifyDrawingBoardAction->setEnabled(true);
        m_pSaveDrawingBoardAction->setEnabled(true);
        actionMgr.getAction(ActionManager::DrawBoardProperty)->setEnabled(true);
    }
}

void PluginGraphicsModelingModule::mainWigetAddAction()
{
    if (!ModelingServerMng::getInstance().m_pUIServer)
        return;
    QWidget *mainWiget = ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->GetMainWidget();
    if (mainWiget == nullptr)
        return;
    // 添加Action
    mainWiget->addAction(ActionManager::getInstance().getAction(ActionManager::Prints));
    mainWiget->addAction(ActionManager::getInstance().getAction(ActionManager::SelectAll));
}

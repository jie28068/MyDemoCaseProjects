#include "ModelWidget.h"
#include "BlockDelegate/BlockDelegateMng.h"
#include "CanvasWidget.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingTool.h"
#include "KLWidgets/KMessageBox.h"
#include "KernnelInterface/KernelInterfaceFactory.h"
#include "ModelManagerServer/ModelManagerConst.h"
#include "NavWidget.h"

// 元件窗口服务
#include "PluginComponentServer/IPluginComponentServer.h"
using namespace Kcc::PluginComponent;

USE_LOGOUT_("ModelWidget")

ModelWidget::ModelWidget(QWidget *parent) : QWidget(parent)
{
    this->setAcceptDrops(true);

    m_nullWidget = new NavWidget(this);
}

ModelWidget::~ModelWidget() { }

void ModelWidget::initModelWidget(Kcc::BlockDefinition::PModel pModel, CanvasWidget *canvasWidget)
{
    if (pModel == nullptr || canvasWidget == nullptr) {
        return;
    }
    m_canvasWidget = canvasWidget;
    m_model = pModel;

    m_pKernelInterface = KernelInterfaceFactory::createKernelInterface(pModel);
    connect(m_pKernelInterface.data(), &BaseKernelInterface::contentsChanged, canvasWidget,
            &CanvasWidget::onContensChanged);

    m_canvasContext = m_pKernelInterface->loadCanvas(pModel);

    m_canvasView = QSharedPointer<ICanvasView>(createCanvasView(m_canvasContext));
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        m_canvasView->setRunningStatus(ModelingServerMng::getInstance().m_pGraphicsModelingServer->getRunningStatus());
    }
    m_canvasView->setCanCreateBlockList();

    // 页面布局
    m_vLoyout = new QVBoxLayout(this);
    m_vLoyout->setContentsMargins(0, 0, 0, 0);
    m_vLoyout->setSpacing(0);
    m_toolBar = m_canvasView->getToolBar();
    if (m_toolBar) {
        m_vLoyout->addWidget(m_toolBar);
    }
    m_vLoyout->addWidget(m_canvasWidget->m_navWidget);
    m_vLoyout->addWidget(m_canvasView.data());

    this->setLayout(m_vLoyout);

    // 初始化服务通知
    initServerNotify();
}

void ModelWidget::refreshUI()
{
    m_vLoyout->insertWidget(1, m_canvasWidget->m_navWidget);
}

PModel ModelWidget::addBlockToView(QString prototypeName, QPointF centerPoint, bool needUndo, QString defaultName,
                                   QString modelUUID)
{
    if (!m_pKernelInterface || !ModelingServerMng::getInstance().m_pProjectMngServer) {
        return PModel();
    }

    bool canModify = m_canvasContext->canModify();
    if (!canModify) {
        return PModel();
    }

    QString tipinfo = "";

    bool isSourceVisible = m_canvasContext->getCanvasProperty().isBaseLayerVisible();

    if (!isSourceVisible) {
        // If source is not visible
        tipinfo = tr("Source is not visible,Modify source visibility to true");
        // 提示用户是否修改资源为可见
        if (KMessageBox::question(tipinfo, KMessageBox::Yes | KMessageBox::No) == KMessageBox::Yes) {
            m_canvasContext->getCanvasProperty().setBaseLayerVisible(true);
        } else {
            return PBlock();
        }
        return PModel();
    }

    auto flags = m_canvasContext->getStatusFlags();
    if (!flags.testFlag(CanvasContext::kProjectActivate)) {
        // 画板非激活状态
        return PModel();
    }

    // add by liwenyu
    int modelType = m_model->getModelType();
    if (modelType == Model::Complex_Board_Type) {
        // 复合模型,只允许添加插槽模型
        if (prototypeName != "Slot") {
            return PModel();
        }
    } else if (modelType == Model::Combine_Board_Type) {
        // 构造型不能添加slot
        if (prototypeName == "Slot") {
            return PModel();
        }
    } else if (modelType == Model::Control_Board_Type) {
        // 控制系统,不能添加插槽
        if (prototypeName == "Slot") {
            return PModel();
        }
    }
    // end

    // 清空选中模块
    m_canvasView->scene()->clearSelection();

    QString uuid, name;
    PModel pModel = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(prototypeName);
    if (!pModel) {
        LOGOUT(QString("%1,创建模块%2失败").arg(__FUNCTION__).arg(prototypeName), LOG_ERROR);
        return PModel();
    }

    GraphicsModelingTool::calculateBlockUuid(uuid, name, pModel, m_model);

    if (!modelUUID.isEmpty() && !m_model->getChildModel(modelUUID)) {
        pModel->setUUID(modelUUID);
    }

    if (!GraphicsModelingTool::enableBlockAdd(pModel, m_model)) {
        return PModel();
    }

    if (defaultName != QString()) {
        name = defaultName;
    }

    pModel->setName(name);
    pModel->setModifyTime(QDateTime::currentDateTime());
    pModel->setPostion(QPointF(centerPoint.rx(), centerPoint.ry()));

    if (pModel->getModelType() == CombineBoardModel::Type || pModel->getModelType() == ElecBoardModel::Type) {
        // 构造型模块端口都可见
        auto portVaribleMap = pModel->getPortManager()->getVariableMap();
        for (PVariable portInfo : portVaribleMap) {
            portInfo->setShowMode(Variable::VisiableReadOnly);
        }
    }

    //  需求为多路开关模块默认应该有三个输入变量（端口），实际新建该模块时只有一个固定输入变量（端口）
    //  由于多路开关模块的输入端口名称会动态变化（除了默认第一个固定输入端口），
    //  如果在向导新建模块时加了输入变量（即端口）的话，后续运行时会报错找不到模型编译时加的端口，
    //  所以在多路开关模块拖到画板中时默认新增2个端口
    if (pModel->getPrototypeName() == "MultiportSwitch") {
        auto cb = pModel.dynamicCast<ControlBlock>();
        if (cb) {
            cb->updataVariableInfo(1, 4);
            cb->parseDynamicBlockPorts();
        }
    }

    this->addBlockToView(pModel, needUndo);

    if (NPS::PROTOTYPENAME_SCOPE == pModel->getPrototypeName()) {
        defaultAddDataDictionary(pModel);
    }

    return pModel;
}

void ModelWidget::addBlockToView(PModel pModel, bool needUndo)
{
    if (!m_pKernelInterface)
        return;

    m_pKernelInterface->addBlockToView(pModel, m_canvasView, needUndo);
}

void ModelWidget::deleteBlockFromView(PModel pModel)
{
    if (!m_pKernelInterface)
        return;

    m_pKernelInterface->deleteBlockFromView(pModel, m_canvasView);
}

void ModelWidget::addConnectorToView(PBlockConnector pConnector)
{
    if (!m_pKernelInterface)
        return;

    m_pKernelInterface->addConnector(pConnector, m_canvasView);
}

void ModelWidget::deleteConnector(QString connectorUUID)
{
    if (!m_pKernelInterface)
        return;

    m_pKernelInterface->deleteConnector(connectorUUID, m_canvasView);
}

void ModelWidget::scrollToBlock(const QString &strActiveBlockID)
{
    if (!m_canvasView)
        return;

    m_canvasView->scrollToBlock(strActiveBlockID);
}

QList<BlockStruct> ModelWidget::getCanCreateBlockList()
{
    if (!ModelingServerMng::getInstance().m_pModelManagerServer
        || !ModelingServerMng::getInstance().m_pPluginElectricComponentServer
        || !ModelingServerMng::getInstance().m_pPluginControlComponentServer) {
        return QList<BlockStruct>();
    }

    QList<BlockStruct> returnList;
    int curBoardType = m_model->getModelType();
    ComponentInfo result;
    QStringList componentsList;
    if (curBoardType == ElecBoardModel::Type || curBoardType == Model::Ele_CombineBoard_Type) {
        result = ModelingServerMng::getInstance().m_pPluginElectricComponentServer->GetComponentInfo();
    } else {
        result = ModelingServerMng::getInstance().m_pPluginControlComponentServer->GetComponentInfo();
    }
    for each (auto stringList in result.mapComponents) {
        componentsList.append(stringList);
    }
    if (curBoardType == ElecBoardModel::Type || curBoardType == Model::Ele_CombineBoard_Type) {
        auto listElect = ModelingServerMng::getInstance().m_pModelManagerServer->GetToolkitModels(
                KL_TOOLKIT::ELECTRICAL_TOOLKIT);
        for (auto electBlock : listElect) {
            if (!componentsList.isEmpty() && !componentsList.contains(electBlock->getPrototypeName()))
                continue;
            if (curBoardType != Model::Ele_CombineBoard_Type // 只有电气构造画板能创建 外部电气节点和单线变换器
                && (electBlock->getPrototypeName() == "ExternalElectricalNode"
                    || electBlock->getPrototypeName() == "SingleLineConverter"))
                continue;
            BlockStruct tmpBlcok;
            tmpBlcok.prototypeName = electBlock->getPrototypeName();
            tmpBlcok.prototypeChsName = electBlock->getPrototypeName_CHS();
            returnList.push_back(tmpBlcok);
        }
    } else {
        auto listControl =
                ModelingServerMng::getInstance().m_pModelManagerServer->GetToolkitModels(KL_TOOLKIT::CONTROL_TOOLKIT);
        for (auto controlBlock : listControl) {
            if (!componentsList.isEmpty() && !componentsList.contains(controlBlock->getPrototypeName()))
                continue;
            BlockStruct tmpBlcok;
            bool isAccept = false;
            if (curBoardType == ControlBoardModel::Type
                && controlBlock->getPrototypeName() != GraphicsModelingConst::SPECIAL_BLOCK_SLOT
                && controlBlock->getPrototypeName() != GraphicsModelingConst::SPECIAL_BLOCK_CTRLIN
                && controlBlock->getPrototypeName() != GraphicsModelingConst::SPECIAL_BLOCK_CTRLOUT) {
                isAccept = true;
            } else if (curBoardType == CombineBoardModel::Type
                       && controlBlock->getPrototypeName() != GraphicsModelingConst::SPECIAL_BLOCK_ELECINTERFACE
                       && controlBlock->getPrototypeName() != GraphicsModelingConst::SPECIAL_BLOCK_SLOT
                       && controlBlock->getPrototypeName() != GraphicsModelingConst::SPECIAL_BLOCK_FMU
                       && controlBlock->getPrototypeName() != m_model->getName()) {
                isAccept = true;
            } else if (curBoardType == ComplexBoardModel::Type
                       && controlBlock->getPrototypeName() == GraphicsModelingConst::SPECIAL_BLOCK_SLOT) {
                isAccept = true;
            }
            if (isAccept) {
                tmpBlcok.prototypeName = controlBlock->getPrototypeName();
                tmpBlcok.prototypeChsName = controlBlock->getPrototypeName_CHS();
                tmpBlcok.prototypeReadableName = controlBlock->getPrototypeName_Readable();
                returnList.push_back(tmpBlcok);
            }
        }
    }
    return returnList;
}

void ModelWidget::setRunStepVariablesData(Simu_Var::SimuVarBoard &boardValue)
{
    map<string, Simu_Var::SimuVarBlock> blockMap = boardValue.blockMap;
    for (auto iter : blockMap) {
        QMap<QString, QMap<QString, QVariant>> dataMap;
        Simu_Var::SimuVarBlock block = iter.second;

        for (auto paramVecIter : block.paramMap) {
            QMap<QString, QVariant> memberMap;
            for (auto param : paramVecIter.second) {
                memberMap[QString::fromStdString(param.name)] = QString::fromStdString(param.value);
            }
            dataMap[QString::fromStdString(paramVecIter.first)] = memberMap;
        }

        auto source = m_canvasContext->getSource(QString::fromStdString(iter.first));
        if (source) {
            source->setRunVariableDataMap(dataMap);
        }
    }
}

void ModelWidget::showEvent(QShowEvent *event)
{
    if (!m_model || !m_canvasWidget) {
        return;
    }

    m_vLoyout->removeWidget(m_nullWidget);
    m_vLoyout->insertWidget(1, m_canvasWidget->m_navWidget);

    if (m_canvasView) {
        m_canvasView->widgetShow(m_model->getName(), m_model->getUUID());
    } else {
        ActionManager::getInstance().setActivateCanvas(nullptr);
    }
}

void ModelWidget::hideEvent(QHideEvent *event)
{
    if (!m_model || !m_canvasWidget) {
        return;
    }

    m_vLoyout->removeWidget(m_canvasWidget->m_navWidget);
    m_vLoyout->insertWidget(1, m_nullWidget);

    if (m_canvasView) {
        m_canvasView->widgetHide(m_model->getName(), m_model->getUUID());
    }
}

void ModelWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasFormat(KL_TOOLKIT::DRAG_MIME_TYPE_STRING)) {
        event->ignore();
        return;
    }

    QString itemName;
    int boardtype = -1;
    int curBoardType = m_model->getModelType();

    QByteArray itemData = event->mimeData()->data(KL_TOOLKIT::DRAG_MIME_TYPE_STRING);
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
    dataStream >> itemName >> boardtype;

    // 判断元件是否可以拖拽
    bool isAccept = false;

    switch (curBoardType) {
    case ElecBoardModel::Type:
        // 电气画板只能拖放电气元件
        isAccept = (boardtype == ElecBoardModel::Type);
        break;
    case ControlBoardModel::Type:
        // 控制系统可拖放控制模块，但In/Out除外
        isAccept = (boardtype == ControlBoardModel::Type);
        break;
    case CombineBoardModel::Type:
        // 自定义构造型可拖放控制模块，但不能放电气接口
        isAccept = ((boardtype == CombineBoardModel::Type || boardtype == ControlBoardModel::Type)
                    && itemName != "ElectricalInterface");
        break;
    case ComplexBoardModel::Type:
        // 电气画板只能拖放电气元件
        isAccept = boardtype == ComplexBoardModel::Type;
        break;
    case ElecCombineBoardModel::Type:
        // 电气构造画板可拖放电气模块和External模块
        isAccept = (boardtype == ElecBoardModel::Type || boardtype == ElecCombineBoardModel::Type);
        break;
    default:
        break;
    }

    if (isAccept) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        LOGOUT(tr("%1 artboard cannot create %2 modules [%3]!") //%1画板不能创建%2模块[%3]！
                       .arg(getBroadType(curBoardType))
                       .arg(getBroadType(boardtype))
                       .arg(itemName),
               LOG_WARNING);
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void ModelWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QPointF dropPos = event->pos();
    if (m_canvasView) {
        QPointF viewPos = m_canvasView->pos();
        QPointF scenePos = m_canvasView->mapToScene((dropPos - viewPos).toPoint());
        QRectF rcScene = m_canvasView->getCanvasScene()->sceneRect();
        QRectF copyRightRect = m_canvasView->getCanvasScene()->getCopyRightRect();
        bool isShowlegend = getCanvasContext()->getCanvasProperty().isLegendVisible();
        if (!rcScene.contains(scenePos)
            || (isShowlegend && !copyRightRect.isEmpty() && copyRightRect.contains(scenePos))) {
            // 如果鼠标坐标不在场景内，不允许拖拽创建
            event->ignore();
            return;
        }
    }

    if (event->mimeData()->hasFormat(KL_TOOLKIT::DRAG_MIME_TYPE_STRING)) {
        QString itemName;
        QIcon icon;
        QByteArray itemData = event->mimeData()->data(KL_TOOLKIT::DRAG_MIME_TYPE_STRING);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        dataStream >> itemName;

        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void ModelWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(KL_TOOLKIT::DRAG_MIME_TYPE_STRING)) {
        int boardtype = -1;
        QString itemName;
        QByteArray itemData = event->mimeData()->data(KL_TOOLKIT::DRAG_MIME_TYPE_STRING);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        dataStream >> itemName >> boardtype;

        // 拖放并创建模块
        QPointF dropPos = event->pos();
        QPointF viewPos = m_canvasView->pos();
        // 减去viewpos在widget中的偏移
        QPointF adjustPos = dropPos - viewPos;
        addBlockToView(itemName, m_canvasView->mapToScene(adjustPos.toPoint()));

        this->setCursor(Qt::ArrowCursor);
    }
}

void ModelWidget::resizeEvent(QResizeEvent *event) { }

void ModelWidget::initServerNotify()
{
    if (!ModelingServerMng::getInstance().m_pSimulationManagerServer
        || !ModelingServerMng::getInstance().m_pPropertyManagerServerIF
        // || !ModelingServerMng::getInstance().m_pNetworkManagerServerIF
        || !ModelingServerMng::getInstance().m_pDrawingBoardHelperServerIF
        || !ModelingServerMng::getInstance().m_pProjectMngServerIF || !ModelingServerMng::getInstance().m_pLogSeverIF) {
        return;
    }

    // 服务通知
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_DrawingBoardRunning, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_DrawingBoardStopped, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_ProgressInit, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_ProgressRunning, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    // 编译完成通知
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_CompileFinished, this, SLOT(onRecieveCtrSysMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pSimulationManagerServer->connectNotify(
            Notify_UpdateBlockOrder, this, SLOT(onRecieveCtrSysMsg(unsigned int, const NotifyStruct &)));

    ModelingServerMng::getInstance().m_pPropertyManagerServerIF->connectNotify(
            Notify_BlockPropertyChanged, this,
            SLOT(onReceivePropertyServerMessage(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pPropertyManagerServerIF->connectNotify(
            Notify_BlockSimuOutParamAliasChanged, this,
            SLOT(onReceivePropertyServerMessage(unsigned int, const NotifyStruct &)));

    // Model数据管理服务通知
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_CodePrototypeDestroyed, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_CodePrototypeSaved, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardDestroyed, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardCreate, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DeviceTypeDestroyed, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_ActivationStateChanged, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardSaved, this, SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));
    ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
            IPM_Notify_DrawingBoardFileRenamed, this,
            SLOT(onReceiveProjectMngServerMsg(unsigned int, const NotifyStruct &)));

    if (ModelingServerMng::getInstance().m_pProjectMngServerIF) {
        ModelingServerMng::getInstance().m_pProjectMngServerIF->connectNotify(
                IPM_Notify_CodePrototypeSaved, this,
                SLOT(onReceiveModelDataManagerServerMsg(unsigned int, const NotifyStruct &)));
    }

    // 网络模型管理器通知 fixme
    // ModelingServerMng::getInstance().m_pNetworkManagerServerIF->connectNotify(
    //         Notify_BlockChanged, this, SLOT(onRecieveNetworkManagerMsg(unsigned int, const NotifyStruct &)));
    // ModelingServerMng::getInstance().m_pNetworkManagerServerIF->connectNotify(
    //         Notify_DeviceTypeModifiedFinished, this,
    //         SLOT(onRecieveNetworkManagerMsg(unsigned int, const NotifyStruct &)));

    // 订阅画板规则校验结果
    ModelingServerMng::getInstance().m_pDrawingBoardHelperServerIF->connectNotify(
            Kcc::ElectricalModelCheck::Notify_Electrical_Verify_Result, this,
            SLOT(onRecieveDrawingBoardHelperServerMsg(unsigned int, const NotifyStruct &)));

    // 元件窗口服务
    if (m_model != nullptr) {
        QVariantMap mapinfo;
        if (ElecBoardModel::Type == m_model->getModelType() || ElecCombineBoardModel::Type == m_model->getModelType()) {
            mapinfo.insert("name", PLUGIN_COMPONENT_VIEW_DEVICE);
        } else {
            mapinfo.insert("name", PLUGIN_COMPONENT_VIEW_CONTROL);
        }
        PIServerInterfaceBase componentserverif = RequestServerInterface<IPluginComponentServer>(mapinfo);
        if (componentserverif == nullptr) {
            LOGOUT("IPluginComponentServer注册失败！", LOG_ERROR);
        } else {
            componentserverif->connectNotify(Notify_AddComponentToBoard, this,
                                             SLOT(onRecieveComponentServerMsg(unsigned int, const NotifyStruct &)));
        }
    }
}

// 遍历画板所有模块，查找代码型模块，原型名称匹配的进行状态修改并显示
void ModelWidget::setBlockState(QString prototypeName, Block::ModelState blockState, QString stateInfo)
{
    QList<PModel> modelList = GraphicsModelingTool::findBlockByPrototypeName(prototypeName, m_model);
    bool isNeedVerifyStatus = false;
    if (modelList.isEmpty())
        return;

    bool isNeedSave = false;
    for (auto pModel : modelList) {
        QString uuid = pModel->getUUID();
        QSharedPointer<SourceProxy> source = m_canvasContext->getSource(uuid);
        if (!source)
            return;
        source->setState(GraphicsModelingTool::getBlockStateString(blockState));
        source->setStatetips(stateInfo);
        QString blockname = pModel->getName();
        if (blockState == Block::StateError) {
            LOGOUT(blockname + ":" + stateInfo, LOG_ERROR);
        } else if (blockState == Block::StateWarring) {
            LOGOUT(blockname + ":" + stateInfo, LOG_WARNING);
        }
        isNeedSave = true;
    }
    // 强制保存模块状态和状态提示信息
    if (isNeedSave) {
        m_canvasWidget->Save();
    }
}

void ModelWidget::setSingleBlockState(QString uuid, Block::ModelState blockState, QString stateInfo)
{
    QSharedPointer<SourceProxy> source = m_canvasContext->getSource(uuid);
    if (source == nullptr)
        return;

    source->setState(GraphicsModelingTool::getBlockStateString(blockState));
    source->setStatetips(stateInfo);

    QString blockname = source->name();
    if (blockState == Block::StateError || blockState == Block::StateDisable) {
        LOGOUT(blockname + ":" + stateInfo, LOG_ERROR);
    } else if (blockState == Block::StateWarring) {
        LOGOUT(blockname + ":" + stateInfo, LOG_WARNING);
    } else if (blockState == Block::StateNormal) {
        LOGOUT(blockname + ":" + stateInfo, LOG_NORMAL);
    }
}

void ModelWidget::changeBlockState(QString blockUUID, PCanvasContext canvasCtx,
                                   QMap<QString, QVariant>::const_iterator iter)
{
    if (canvasCtx == nullptr) {
        return;
    }
    PSourceProxy psourcce = canvasCtx->getSource(blockUUID);
    if (psourcce != nullptr) {
        std::vector<Kcc::SimulationManager::CompileResult> blockStates =
                iter.value().value<std::vector<Kcc::SimulationManager::CompileResult>>();
        Block::ModelState blockState = Block::ModelState::StateNormal;
        QString stateInfo;
        for (int i = 0; i < blockStates.size(); i++) {
            // 需要判断下日志级别，不能让警告 覆盖了 错误
            if (blockState < blockStates[i].level)
                blockState = static_cast<Block::ModelState>(blockStates[i].level);
            if (stateInfo.isEmpty()) {
                stateInfo = QString::fromStdString(blockStates[i].msg);
            } else {
                stateInfo = stateInfo + QString("\n%1").arg(QString::fromStdString(blockStates[i].msg));
            }
        }
        if (m_model->getModelType() == ControlBoardModel::Type || m_model->getModelType() == CombineBoardModel::Type) {
            psourcce->setState(GraphicsModelingTool::getBlockStateString(blockState));
            psourcce->setStatetips(stateInfo);
        }
    }
}

void ModelWidget::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    // 画板运行没有遮罩层，暂不处理
    QString strBoardUuid = m_model->getUUID();
    switch (code) {
    case Notify_DrawingBoardRunning: {
        QStringList listUuid = param.paramMap.value("Boarduuids").toStringList();
        /*         if (listUuid.contains(strBoardUuid)) {
                    m_canvasView->setInteractive(false);
                } */
    } break;
    case Notify_DrawingBoardStopped: {
        QStringList listUuid = param.paramMap.value("Boarduuids").toStringList();
        /*         if (listUuid.contains(strBoardUuid)) {
                    m_canvasView->setInteractive(true);
                } */
        // 运行结束后将变量显示设为空隐藏起来
        for each (auto source in m_canvasContext->getAllSource()) {
            source->setRunVariableDataMap(QMap<QString, QMap<QString, QVariant>>());
        }
    } break;
    case Notify_ProgressInit: {
        int minimun = param.paramMap.value("minimun").toInt();
        int maximun = param.paramMap.value("maximun").toInt();
    } break;
    case Notify_ProgressRunning: {
        float value = param.paramMap.value("value").toFloat();
    } break;
    case Notify_RunStepVariables: {
        int i = 1;
    } break;
    default: {
    }
    }
}

void ModelWidget::onReceivePropertyServerMessage(unsigned int code, const NotifyStruct &param)
{
    switch (code) {
    case Notify_BlockPropertyChanged: {
        QString boardName = param.paramMap[PMKEY::MODEL_NAME].toString();
        QString blockUUID = param.paramMap[PMKEY::MODEL_UUID].toString();

        if (blockUUID.isEmpty() || boardName != m_model->getName()) {
            return;
        }
        PModel model = m_model->getChildModel(blockUUID);
        if (m_canvasWidget && model) {
            m_canvasWidget->verifyStatus();
        }

    } break;
    case Notify_BlockSimuOutParamAliasChanged: {
        QString boardUUID = param.paramMap[PMKEY::MODEL_UUID].toString();
        if (boardUUID.isEmpty() || m_model->getUUID() != boardUUID) {
            return;
        }
        if (m_pKernelInterface != nullptr) {
            m_pKernelInterface->emitContentsChanged();
        }
    } break;
    default: {
    }
    }
}

void ModelWidget::onRecieveCtrSysMsg(unsigned int code, const NotifyStruct &param)
{
    QString blockPrototypeName = param.paramMap["name"].toString();
    if (!m_model) {
        return;
    }
    switch (code) {
    case SimulationManager::Notify_CompileFinished:
        if (param.paramMap.contains("board_name") && param.paramMap["board_name"].toString() == m_model->getName()
            && m_canvasContext != nullptr) {
            // 遍历画板所有模块，查找模块，ID匹配的进行状态修改并显示
            QMap<QString, QVariant>::const_iterator iter = param.paramMap.begin();
            while (iter != param.paramMap.end()) {
                QStringList stateIDS = iter.key().split("/");
                // 名称格式为  /模块ID/子模块ID/***
                if (stateIDS.at(0) == "board_name") {
                    iter++;
                    continue;
                }
                QString uuid;
                if (stateIDS.size() == 1) {
                    uuid = m_model->getUuidByChildName(stateIDS.at(0));
                    changeBlockState(uuid, m_canvasContext, iter); // 改变模块的状态
                } else {
                    PModel model = m_model;

                    for (int i = 0; i < stateIDS.size(); i++) {
                        uuid = model->getUuidByChildName(stateIDS.at(i));
                        model = model->getChildModel(uuid);
                    }

                    PModel parentModel = model->getParentModel();
                    if (model && parentModel && model != m_model) {
                        PCanvasContext canvasCtx = parentModel->getCanvasContext();
                        if (canvasCtx) {
                            changeBlockState(uuid, canvasCtx, iter); // 改变子模块的状态（构造模块)
                        }
                    }
                }

                iter++;
            }
        }
        break;
    case SimulationManager::Notify_UpdateBlockOrder: {
        qDebug() << param.paramMap;
        if (m_model && m_model->getTopParentModel() == m_model) {
            QString topUUID = m_model->getUUID();
            int startIndex = 1;
            updateModelOrder(topUUID, param.paramMap, startIndex);
        }

    } break;
    default:
        break;
    }
}

void ModelWidget::updateModelOrder(QString boardUUID, QMap<QString, QVariant> orderParams, int &startIndex)
{

    PDrawingBoardClass drawboardClass = ModelingServerMng::getInstance()
                                                .m_pProjectMngServer->GetBoardModelByUUID(boardUUID)
                                                .dynamicCast<DrawingBoardClass>();
    if (drawboardClass) {
        QStringList orderList = orderParams.value(boardUUID, QVariant(QStringList())).toStringList();
        PCanvasContext canvasContext = drawboardClass->getCanvasContext();
        if (!orderList.isEmpty() && canvasContext) {
            foreach (auto order, orderList) {
                if (orderParams.contains(order)) {
                    // 如果当前模块是子系统
                    int tempStart = startIndex;
                    updateModelOrder(order, orderParams, startIndex);
                    auto sourceProxy = canvasContext->getSource(order);
                    if (sourceProxy) {
                        QString badgeMsg = QString("%1-%2").arg(tempStart).arg(startIndex - 1);
                        sourceProxy->setBadgeMsg(badgeMsg);
                    }

                } else {
                    auto sourceProxy = canvasContext->getSource(order);
                    if (sourceProxy) {
                        if (sourceProxy->getCommentState() != "disable") {
                            sourceProxy->setBadgeMsg(QString::number(startIndex++));
                        }
                    }
                }
            }
        }
    }
}

void ModelWidget::defaultAddDataDictionary(PModel model)
{
    auto curDict = ModelingServerMng::getInstance().m_pProjectMngServer->GetRealTimeSimulationName();
    QString dictvar = NPS::getDictBoardBlockVarStr(curDict, model, "", "yi");
    if (!dictvar.isEmpty()) {
        ModelingServerMng::getInstance().m_pDataDictionaryServer->AddDataDictionary(dictvar);
    }
}

QString ModelWidget::getBroadType(int type)
{
    QString nameType;
    switch (type) {
    case ElecBoardModel::Type:
        nameType = GMS::BoardTypeElectrical;
        break;
    case ControlBoardModel::Type:
        nameType = GMS::BoardTypeControl;
        break;
    case CombineBoardModel::Type:
        nameType = GMS::BoardTypeCombine;
        break;
    case ComplexBoardModel::Type:
        nameType = GMS::BoardTypeComplex;
        break;
    case ElecCombineBoardModel::Type:
        nameType = GMS::BoardTypeElecCombine;
        break;
    default:
        nameType = QString();
        break;
    }
    return nameType;
}

void ModelWidget::onRecieveComponentServerMsg(unsigned int code, const NotifyStruct &param)
{
    if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        return;
    }
    QString aa = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
    QString bb = m_model->getUUID();
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID() != m_model->getUUID()) {
        return;
    }
    if (code == Notify_AddComponentToBoard && m_canvasView != nullptr && m_canvasView->getToolBar() != nullptr) {
        QString prototypename = param.paramMap.value("prototypename").toString();
        // 拖放并创建模块
        QPointF centerpoint = QPointF(m_canvasView->width() / 2.0, m_canvasView->height() / 2.0);
        int toolHeight = m_canvasView->getToolBar()->height();
        // 因为当前widget包含了一个toolbar，所有需要减去一个toolbar的高度
        QPointF adjustPos = centerpoint - QPointF(0, toolHeight);
        addBlockToView(prototypename, m_canvasView->mapToScene(adjustPos.toPoint()));
    }
}

void ModelWidget::onRecieveNetworkManagerMsg(unsigned int code, const NotifyStruct &param)
{
    // fixme 没有网络模型管理器服务了
    // if (!m_pKernelInterface) {
    //     return;
    // }
    // switch (code) {
    // case Notify_DeviceTypeModifiedFinished: {
    //     QString devicename = param.paramMap["devicename"].toString();
    //     QString prototypename = param.paramMap["deviceprototype"].toString();
    //     int boardType = m_pDrawingBoardInfo->getModelType();
    //     if (ElecBoardModel::Type == boardType) {
    //         m_pKernelInterface->modifyDeviceType(prototypename, devicename);
    //     }
    // } break;
    // case Notify_BlockChanged: {
    //     QString blockUUID = param.paramMap["blockUUID"].toString();
    //     QString boardUUID = param.paramMap["boardUUID"].toString();

    //     if (blockUUID.isEmpty() || m_pDrawingBoardInfo->getUUID() != boardUUID) {
    //         return;
    //     }
    //     PModel model = m_pDrawingBoardInfo->getChildModel(blockUUID);
    //     if (m_pKernelInterface && model) {
    //         m_pKernelInterface->reloadBlock(model, m_canvasContext);
    //     }
    // } break;
    // default:
    //     break;
    // }
}

void ModelWidget::onRecieveDrawingBoardHelperServerMsg(unsigned int code, const NotifyStruct &param)
{
    if (m_canvasView.isNull()) {
        return;
    }
    PCanvasScene p_scene = m_canvasView->getCanvasScene();
    if (p_scene.isNull())
        return;

    switch (code) {
    case Kcc::ElectricalModelCheck::Notify_Electrical_Verify_Result: {
        QString boardUUID = param.paramMap["boardUUID"].toString();
        if (m_model->getModelType() == ElecBoardModel::Type && boardUUID == m_model->getUUID()) {
            // 更新block关联的画板图元相关的状态 ,包括文字和tooltip
            auto modelMap = m_model->getChildModels();
            for (auto model : modelMap) {
                QString id = model->getUUID();
                PSourceProxy source = m_canvasContext->getSource(id);
                if (!source) {
                    continue;
                }
                Block::ModelState state = (Block::ModelState)model->getState();
                if ((model->getPrototypeName() == "Busbar" || model->getPrototypeName() == "DotBusbar")
                    && state == Block::StateDisable) {
                    source->setState(GraphicsModelingTool::getBlockStateString(state));
                } else {
                    QString stateInfo = state == Block::StateNormal ? "" : model->getStateInfo();
                    source->setState(GraphicsModelingTool::getBlockStateString(state));
                }
            }

            p_scene->update();
        }
    } break;
    default: {
    }
    }
}

void ModelWidget::onReceiveProjectMngServerMsg(unsigned int code, const NotifyStruct &param)
{
    QString blockPrototypeName = param.paramMap["name"].toString();
    if (blockPrototypeName.isEmpty() && code != IPM_Notify_ActivationStateChanged) {
        return;
    }
    switch (code) {
    case IPM_Notify_CodePrototypeDestroyed: {
        setBlockState(blockPrototypeName, Block::StateError, ERROR_INFO_PROTOTYPE_DEL);
    } break;
    case IPM_Notify_DrawingBoardDestroyed: {
        int boardType = param.paramMap["type"].toInt();
        if (boardType == ElecCombineBoardModel::Type || boardType == CombineBoardModel::Type) {
            setBlockState(blockPrototypeName, Block::StateError, ERROR_INFO_PROTOTYPE_DEL);
        }
    } break;
    case IPM_Notify_CodePrototypeSaved: {
        auto modelMap = m_model->getChildModels();
        for (auto model : modelMap) {
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
                            setSingleBlockState(model->getUUID(), Model::StateError, ERROR_INFO_PROTOTYPE_MOD_NOUSE);
                        } else if (oldveridlist[1] != curveridlist[1]) { // 中版本不一致
                            setSingleBlockState(model->getUUID(), Model::StateWarring,
                                                ERROR_INFO_PROTOTYPE_MOD_MAYEFFECT);
                        }
                    }
                }
            }
        }
    } break;
    case IPM_Notify_DrawingBoardSaved: {
        int type = param.paramMap["type"].toInt();
        if (type == Model::Combine_Board_Type) {
            // 原型有变更后 更新索引列表
            m_canvasView->setCanCreateBlockList();

            return; // 自定义构造型原型变更后模块实例暂时不标红或者标黄

            // 相等说明是修改的画板本身verid
            if (blockPrototypeName != m_model->getName()) {
                // 不相等需要判断画板内部block是否包含当前变化的构造型模块，如果有提示当前block。
                auto modelMap = m_model->getChildModels();
                for (auto model : modelMap) {
                    if (model->getModelType() == CombineBoardModel::Type
                        && model->getPrototypeName() == blockPrototypeName) {
                        setSingleBlockState(model->getUUID(), Model::StateWarring, ERROR_INFO_PROTOTYPE_MOD_NOUSE);
                        m_canvasWidget->Save();
                    }
                }
            }
        }
    } break;
    case IPM_Notify_DeviceTypeDestroyed: {
        // 如果画板类型属于电气
        int curtype = m_model->getModelType();
        bool isNeedVerifyStatus = false;
        if (curtype == ElecBoardModel::Type) {
            auto modelMap = m_model->getChildModels();
            for (auto model : modelMap) {
                auto pElecBlock = model.dynamicCast<ElectricalBlock>();
                if (!pElecBlock || !ModelingServerMng::getInstance().m_pProjectMngServer) {
                    return;
                }
                // 判断该电器是不是含有设备类型的器件
                if (ModelingServerMng::getInstance().m_pProjectMngServer->HaveDeviceModel(
                            pElecBlock->getPrototypeName())) {
                    auto deviceModels = ModelingServerMng::getInstance().m_pProjectMngServer->GetDeviceModels(
                            pElecBlock->getPrototypeName());
                    if (!pElecBlock->getDeviceModel()) {
                        isNeedVerifyStatus = true;
                    }
                    foreach (auto deviceModel, deviceModels) {
                        if (deviceModel->getDeviceTypePrototypeName()
                            == pElecBlock->getDeviceModel()->getDeviceTypePrototypeName())
                            isNeedVerifyStatus = true;
                    }
                }
            }
        }
        if (isNeedVerifyStatus) {
            m_canvasWidget->verifyStatus();
        }
    } break;
    case IPM_Notify_ActivationStateChanged: {
        QSharedPointer<CanvasContext> canvasContext = getCanvasContext();
        if (canvasContext == nullptr || m_canvasContext == nullptr) {
            return;
        }
        if (canvasContext->type() != CanvasContext::Type::kElectricalType) {
            return;
        }
        // 如果画板未激活，设置不可编辑
        if (param.paramMap.contains(m_canvasContext->uuid())) {
            bool value = param.paramMap[m_canvasContext->uuid()].toBool();
            m_canvasContext->setStatusFlag(CanvasContext::kProjectActivate, value);
        }
    } break;
    default:
        break;
    }
}
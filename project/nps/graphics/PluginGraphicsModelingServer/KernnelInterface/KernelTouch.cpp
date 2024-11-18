#include "KernelTouch.h"
#include "Associate.h"
#include "CanvasWidget.h"
#include "CoreLib/ServerBase.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingTool.h"
#include "KLModelDefinitionCore/SyncHelper.h"
#include "KernelInterfaceFactory.h"
#include "KernnelInterface/BaseKernelInterface.h"
#include "Manager/ClipboardManager.h"
#include "Manager/ModelingServerMng.h"
#include "ModelCopyStrategy.h"

#include <QApplication>
#include <QFile>
#include <QMimeData>
#include <QSvgRenderer>
#include <qclipboard.h>

USE_LOGOUT_("KernelTouch")

KernelTouch::KernelTouch(BaseKernelInterface *baseKernelInterface)
    : m_pBaseKernelInterface(baseKernelInterface), m_notSyncBlock(false)
{
}

KernelTouch::~KernelTouch() { }

void KernelTouch::initTouch(PModel pBoardModel, PCanvasContext pCanvasContext)
{
    m_boardModel = pBoardModel;
    m_pCanvasContext = pCanvasContext;

    initBlockDelegate(pBoardModel);
    initKernelSignalSlot(pCanvasContext);
}

PSourceProxy KernelTouch::addBlockToView(PModel pModel)
{
    if (m_pCanvasContext == nullptr || pModel == nullptr || m_pBlockDelegateMng == nullptr) {
        PSourceProxy();
    }
    PSourceProxy pSource = m_pCanvasContext->getSource(pModel->getUUID());
    if (pSource.isNull()) {
        return pSource;
    }

    m_pBlockDelegateMng->bindBlockDelegate(pModel); // 绑定模块业务代理

    connectSourceSigSlot(pSource);     // 连接模块信号槽
    connectSourcePortSigSlot(pSource); // 模块端口信号槽

    return pSource;
}

void KernelTouch::deleteBlockFromeView(PModel pModel)
{
    m_pBlockDelegateMng->unbindBlockDelegate(pModel);
}

PConnectorWireContext KernelTouch::addConnectorToView(PBlockConnector pConnector)
{
    if (pConnector == nullptr || m_pCanvasContext == nullptr) {
        nullptr;
    }
    PConnectorWireContext pWire = m_pCanvasContext->getConnectorWireContext(pConnector->getUUID());
    if (!pWire) {
        return nullptr;
    }

    connectWireSigSlot(pWire); // 连接信号槽

    return pWire;
}

void KernelTouch::initBlockDelegate(PModel pBoardModel)
{
    m_pBlockDelegateMng = PBlockDelegateMng(new BlockDelegateMng(pBoardModel));

    if (m_pBlockDelegateMng == nullptr) {
        return;
    }
    auto blockMap = pBoardModel->getChildModels();
    auto iter = blockMap.constBegin();
    while (iter != blockMap.constEnd()) {
        PModel pModel = iter.value();
        if (pModel) {
            m_pBlockDelegateMng->bindBlockDelegate(pModel); // 绑定模块业务代理
        }
        iter++;
    }
}

void KernelTouch::initKernelSignalSlot(PCanvasContext pCanvasContext)
{
    if (pCanvasContext == nullptr) {
        return;
    }
    connectCanvasSigSlot(pCanvasContext);

    QMap<QString, PSourceProxy> sources = pCanvasContext->getAllSource();
    QList<QString> sourceKeys = sources.keys();
    for (int i = 0; i < sourceKeys.size(); i++) {
        PSourceProxy source = sources.value(sourceKeys.at(i));
        if (!source)
            continue;
        connectSourceSigSlot(source);
        connectSourcePortSigSlot(source);
    }

    QMap<QString, PConnectorWireContext> wires = pCanvasContext->getAllConnectorWireContext();
    QList<QString> wireKeys = wires.keys();
    for (int i = 0; i < wireKeys.size(); i++) {
        PConnectorWireContext wire = wires.value(wireKeys.at(i));
        if (!wire)
            continue;
        connectWireSigSlot(wire);
    }
}

void KernelTouch::connectCanvasSigSlot(PCanvasContext canvas)
{
    // 绑定kennel与sever数据变更的通知机制
    connect(canvas.data(), SIGNAL(canvasContextChanged(QString, QVariant)), this,
            SLOT(onCanvasContextChanged(QString, QVariant)));
    connect(canvas.data(), SIGNAL(canvasAddSourceProxy(QSharedPointer<SourceProxy>)), this,
            SLOT(onCanvasAddSourceProxy(QSharedPointer<SourceProxy>)));
    connect(canvas.data(), SIGNAL(canvasDeleteSourceProxy(QSharedPointer<SourceProxy>)), this,
            SLOT(onCanvaDeleteSourceProxy(QSharedPointer<SourceProxy>)));
    connect(canvas.data(), SIGNAL(canvasAddConnectorWire(QSharedPointer<ConnectorWireContext>)), this,
            SLOT(onCanvaAddConnectorWire(QSharedPointer<ConnectorWireContext>)));
    connect(canvas.data(), SIGNAL(canvasDeleteConnectorWire(QSharedPointer<ConnectorWireContext>)), this,
            SLOT(onCanvaDeleteConnectorWire(QSharedPointer<ConnectorWireContext>)));
    connect(canvas.data(), SIGNAL(canvasGroupPositionChange()), this, SLOT(onSourceGroupPositionChanged()));

    connect(canvas.data(), &CanvasContext::sourceProxyEdit, this, &KernelTouch::onCanvasSourceEdit);
}

void KernelTouch::connectSourceSigSlot(PSourceProxy source)
{
    connect(source.data(), SIGNAL(sourceChange(QString, QVariant)), this,
            SLOT(onSourceProxyChanged(QString, QVariant)));
    connect(source.data(), SIGNAL(sigAddPortContext(QSharedPointer<PortContext>)), this,
            SLOT(onAddPortContext(QSharedPointer<PortContext>)));
    connect(source.data(), SIGNAL(sigDelPortContext(QSharedPointer<PortContext>)), this,
            SLOT(onDelPortContext(QSharedPointer<PortContext>)));
}

void KernelTouch::connectSourcePortSigSlot(PSourceProxy source)
{
    if (source == nullptr) {
        return;
    }

    QList<PPortContext> ports = source->portList();
    for (int j = 0; j < ports.size(); j++) {
        PPortContext port = ports.at(j);
        if (!port)
            return;
        connectPortSigSlot(port);
    }
}

void KernelTouch::connectWireSigSlot(PConnectorWireContext wire)
{
    connect(wire.data(), SIGNAL(ConnectorWireChange()), this, SLOT(onConnectorWireChanged()));
    connect(wire.data(), SIGNAL(signalNameChanged(QString)), this, SLOT(onConnectorSignalNameChanged(QString)));
}

void KernelTouch::connectPortSigSlot(PPortContext port)
{
    connect(port.data(), SIGNAL(portChange()), this, SLOT(onBlockPortsChanged()));
}

void KernelTouch::handlePasteBlock(QString originalUUID, PModel &model)
{
    PModel originalBoardClass; // 复制-源画板

    // 从剪切板取出画板uuid
    QString boardUUID;
    QByteArray data = QApplication::clipboard()->mimeData()->data("MIMETYPE_CANVAS_UUID");
    QDataStream dataStream(&data, QIODevice::ReadOnly);
    dataStream >> boardUUID;

    if (!boardUUID.isEmpty()) {
        originalBoardClass = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(boardUUID);
    } else {
        originalBoardClass =
                ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(m_pCanvasContext->uuid());
    }
    if (!originalBoardClass)
        return;

    PModel pOriginalBlock = originalBoardClass->getChildModel(originalUUID); // 源模块
    if (pOriginalBlock) {
        model = pOriginalBlock->copy();
    } else { // 剪切-粘贴
        pOriginalBlock =
                ClipboardManager::getInstance().getChildModel(boardUUID, originalUUID); // 剪切板管理中查找备份模块数据
        if (pOriginalBlock) {
            model = pOriginalBlock->copy();
        }
    }

    if (model == nullptr) {
        return;
    }
    // add by liwenyu 2023.06.08
    // 如果是母线,先清理掉端口信息，后续在syncblock的时候会根据sourceproxy的portlist同步
    // 如果不情况，在母线或者点状母线复制的时候，会导致模型里面出现空端口的情况
    if (model->getPrototypeName() == NPS::PROTOTYPENAME_BUSBAR
        || model->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
        model->getPortManager()->clearVariableMap();
    }
    // end
}

void KernelTouch::removeBusSelectorBlockOutputPorts(PModel model)
{
    if (model != nullptr && model->getPrototypeName() == NPS::PROTOTYPENAME_BUSSELECTOR) {
        auto mainGroup = model->getVariableGroup(RoleDataDefinition::MainGroup);
        if (mainGroup != nullptr && mainGroup->hasRole(RoleDataDefinition::IsClearOutputPortRole)) {
            if (!mainGroup->getGroupData(RoleDataDefinition::IsClearOutputPortRole).toBool()) {
                return;
            }
        }

        auto busSelectorModel = model.dynamicCast<ControlBlock>();
        if (busSelectorModel == nullptr) {
            return;
        }
        auto outputVariablGroup = busSelectorModel->getOutputVariableGroup();
        if (outputVariablGroup == nullptr) {
            return;
        }
        outputVariablGroup->clearVariableMap(); // 清除输出变量信息

        auto portGroup = busSelectorModel->getPortManager();
        if (portGroup == nullptr) {
            return;
        }
        QList<PVariable> outputPortsList = portGroup->findVariable(RoleDataDefinition::PortType, Variable::ControlOut);
        for (auto port : outputPortsList) {
            portGroup->removeVariable(port); // 清除所有输出端口
        }
    }
}

bool KernelTouch::DetermineWhetherStereotypeExist(PModel model)
{
    if (model) {
        if (model == model->getTopParentModel()) {
            return false;
        }
        if (model != model->getTopParentModel()) {
            if (model->getModelType() == PModel::Type::Ele_CombineBoard_Type
                || model->getModelType() == PModel::Type::Combine_Board_Type) {
                //  下一层为电气构造画板时，画板被冻结且不能解锁,非子系统
                if (!model->getPrototypeName().startsWith(SUBSYSTEM_PROTO_PREFIX)) {
                    return true;
                }
            }
        }
    }
    return DetermineWhetherStereotypeExist(model->getParentModel());
}

void KernelTouch::onCanvasContextChanged(QString key, QVariant val)
{
    CanvasContext *ctx = qobject_cast<CanvasContext *>(sender());
    if (!ctx)
        return;

    if (key == GKD::CANVAS_NAME || key == GKD::CANVAS_SIZE || key == GKD::CANVAS_SCALE || key == GKD::CANVAS_DIRECTION
        || key == GKD::CANVAS_CENTER_POS || key == GKD::CANVAS_BACKGROUND_COLOR || key == GKD::CANVAS_DESCRIPTION
        || key == GKD::CANVAS_FREEZE_STATUS || key == GKD::CANVAS_FREEZE_STATUS || key == GKD::CANVAS_AUTHOR
        || key == GKD::CANVAS_DESCRIPTION) //@TXY 先屏蔽画板名,后续接口加了再注意一下
                                           // 画板名、缩放比例、冻结状态、中心位置坐标变化,不触发*号,直接保存至本地
    {
        return; // 不触发画板修改
    }

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onCanvasAddSourceProxy(QSharedPointer<SourceProxy> pSourceProxy)
{
    if (m_pCanvasContext == nullptr) {
        return;
    }
    CanvasContext *ctx = qobject_cast<CanvasContext *>(sender());
    if (!ctx || !pSourceProxy || !ModelingServerMng::getInstance().m_pProjectMngServer)
        return;
    SourceProxy::CreateType createType = pSourceProxy->getSourceCreateType();
    // qDebug() << int(createType);

    if (pSourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION) {
        connectSourceSigSlot(pSourceProxy);
        return;
    }

    // 获取剪切板中的记录的画板uuid
    QString clipboardCanvasUUID;
    QByteArray clipData = QApplication::clipboard()->mimeData()->data("MIMETYPE_CANVAS_UUID");
    if (!clipData.isNull()) {
        QDataStream dataStream(&clipData, QIODevice::ReadOnly);
        dataStream >> clipboardCanvasUUID;
    }

    // 获取剪切板中的记录的项目标识
    QString projectSign;
    QByteArray projectData = QApplication::clipboard()->mimeData()->data("MIMETYPE_PROJECT_SIGN");
    if (!projectData.isNull()) {
        QDataStream dataStream(&projectData, QIODevice::ReadOnly);
        dataStream >> projectSign;
    }

    bool isCrossProject = false;
    PKLProject curProj = ModelingServerMng::getInstance().m_pProjectMngServer->GetCurProject();
    if (curProj && projectSign != "") {
        if (projectSign != curProj->getProjectPath()) {
            // 说明是跨项目复制
            isCrossProject = true;
        }
    }

    QString currentCanvasUUID = ctx->uuid();
    QString targetCanvasUUID = currentCanvasUUID;
    if (SourceProxy::kCreateCopy == createType || SourceProxy::kCreateCut == createType) {
        // 只有复制的时候才需要考虑跨项目/画板的情况
        if (!clipboardCanvasUUID.isEmpty() && clipboardCanvasUUID != currentCanvasUUID) {
            // 说明是跨画板复制
            targetCanvasUUID = clipboardCanvasUUID;
        }
    }

    PModel currentCanvasModel =
            ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(currentCanvasUUID);

    PModel canvasModel = nullptr;
    if (isCrossProject) {
        PKLProject project = ModelingServerMng::getInstance().m_pProjectMngServer->GetProject(projectSign);
        if (project) {
            canvasModel = project->findModel(clipboardCanvasUUID);
        }
    } else {
        projectSign = curProj->getProjectPath(); // 若非跨项目,则设为当前项目路径
        canvasModel = ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByUUID(targetCanvasUUID);
    }

    if (!canvasModel || !currentCanvasModel) {
        // 无法创建model，把图形数据删除
        m_pCanvasContext->deleteSourceProxy(pSourceProxy->uuid());
        return;
    }

    PModel originalModel = nullptr;
    switch (createType) {
    case SourceProxy::kCreateCopy: {
        // 现充画板里面找
        originalModel = canvasModel->getChildModel(pSourceProxy->originalUUID());
        // 若为空,遍历画板内所有子模块查找
        if (!originalModel) {
            originalModel = canvasModel->findChildModelRecursive(pSourceProxy->originalUUID());
        }
        if (!originalModel) {
            // 画板没有找到，说明应该是资源被删除了，从删除的数据中查找
            originalModel =
                    ClipboardManager::getInstance().getChildModel(targetCanvasUUID, pSourceProxy->originalUUID());
        }

    } break;
    case SourceProxy::kCreateCut: {
        originalModel = ClipboardManager::getInstance().getChildModel(targetCanvasUUID, pSourceProxy->originalUUID());

    } break;
    case SourceProxy::kCreateRevert: {
        originalModel = ClipboardManager::getInstance().getChildModel(targetCanvasUUID, pSourceProxy->uuid());

    } break;
    case SourceProxy::kCreateNormal: {
        // 目前只有一种情况，母线类型切换的时候，资源是新增的
        if (pSourceProxy->prototypeName() == NPS::PROTOTYPENAME_BUSBAR
            || pSourceProxy->prototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
            originalModel = ModelingServerMng::getInstance().m_pProjectMngServer->CreateBlockModel(
                    pSourceProxy->prototypeName());
        }
    } break;
    default: {
    }
    }

    if (!originalModel) {
        // 找不到原始数据，那么就不要新增数据了，把图形数据给删除掉
        m_pCanvasContext->deleteSourceProxy(pSourceProxy->uuid());
        return;
    }

    PModel newModel = nullptr;
    if (createType == SourceProxy::kCreateCut || createType == SourceProxy::kCreateRevert
        || createType == SourceProxy::kCreateNormal) {
        newModel = originalModel;
    } else {
        // 本项目复制
        auto loacalCopyConnectFunc = [](ModelCopyStrategy *strategy, const QList<PAssociate> &associates) -> void {
            PModel model = strategy->getNewModel();
            for (auto ass : associates) {
                // 只处理 设备类型的关联关系
                if (ass->type == ModelConnDeviceType) {
                    ModelSystem::getInstance()->getProjectManager()->addAssociate(model, ass->target, ass->type,
                                                                                  ass->mirrored);
                }
            }
        };

        // 跨项目复制
        QMap<QString, PModel> deviceTypeMap; // 设备类型映射,key为旧的设备类型uuid,value为复制的新的设备类型
        PKLProject srcProject = ModelingServerMng::getInstance().m_pProjectMngServer->GetProject(projectSign);
        auto crossCopyConnectFunc = [curProj, srcProject, &deviceTypeMap](ModelCopyStrategy *strategy,
                                                                          const QList<PAssociate> &associates) -> void {
            PModel model = strategy->getNewModel();
            // 跨项目复制,若model为电气模块且原model绑定设备类型,则还要复制出新的设备类型并建立连接关系
            if (model->getModelType() == ElectricalBlock::Type) {
                for (auto ass : associates) {
                    if (ass->type == ModelConnDeviceType && !ass->targetUuid.isEmpty() && ass->target) {
                        if (srcProject->findModel(ass->targetUuid)) {
                            // 查找到说明是项目内设备类型
                            // 复制该设备类型
                            if (!deviceTypeMap.contains(ass->targetUuid)) {
                                auto newDevice = curProj->copyModel(srcProject, ass->target);
                                deviceTypeMap.insert(ass->targetUuid, newDevice);
                                model->connectModel(newDevice, ModelConnDeviceType, ass->target);
                            } else {
                                model->connectModel(deviceTypeMap.value(ass->targetUuid), ModelConnDeviceType,
                                                    ass->mirrored);
                            }
                        } else {
                            // 项目内未查询到,但是存在 说明是系统设备类型
                            // 直接连接
                            model->connectModel(ass->target, ModelConnDeviceType, ass->mirrored);
                        }
                    }
                }
            }
        };

        ModelCopyStrategy *strategy = nullptr;
        auto sourceModelPool = ModelSystem::getInstance()->getProjectManager()->getProject(projectSign);
        auto destModelPool = ModelSystem::getInstance()->getProjectManager()->getProject(curProj->getProjectPath());
        if (isCrossProject) {
            strategy = new CopyStrategyCrossModelPool(originalModel, sourceModelPool);
            strategy->setAssociateListener(crossCopyConnectFunc);
        } else {
            strategy = new ModelCopyStrategy(originalModel, sourceModelPool);
            strategy->setAssociateListener(loacalCopyConnectFunc);
        }
        strategy->setDestModelPool(destModelPool);
        strategy->setCopyAsOriginModel(false);
        if (ModelHelp::isCombineModel(originalModel) && !ModelHelp::isSubsystem(originalModel)) {
            strategy->setRecordChildMap(true);
        } else {
            strategy->setRecordChildMap(false);
        }
        newModel = strategy->doCopy();

        if (isCrossProject) {
            // 跨项目拷贝画板中代码型模块,构造型模块(控制、电气),子系统在拷贝的过程中会拷贝新的原型model,这些原型model需要save一下存储到本地
            for (auto newPrototype : strategy->getOldPrototype2NewMap()) {
                if (newPrototype) {
                    QString oldPrototypeName;
                    if (newPrototype->getModelType() == ControlBlock::Type) {
                        auto map = strategy->getNewModel2OldNameMap();
                        if (!map.contains(newPrototype)) {
                            LOGOUT("Error");
                            continue;
                        }
                        oldPrototypeName = map.value(newPrototype);
                    }
                    curProj->copySourceBlockHandle(newPrototype, oldPrototypeName, srcProject);
                }
            }
        }

        // CopyListener connectModelFunc = isCrossProject ? crossCopyConnectFunc : loacalCopyConnectFunc;
        //  newModel = originalModel->copy(connectModelFunc);
        //  if (isCrossProject) {
        //      PKLProject sourceProject =
        //      ModelingServerMng::getInstance().m_pProjectMngServer->GetProject(projectSign);
        //      // 若originalModel属于本地代码型模块则还需要复制本地代码型至本地
        //      if (originalModel->getModelType() == ControlBlock::Type) {
        //          auto prototypeModel = sourceProject->getModel(originalModel->getPrototypeName());
        //          if (prototypeModel) {
        //              // 找到了说明是本地代码型模块,复制原型Model
        //              curProj->copyModel(sourceProject, prototypeModel);
        //          }
        //      }
        //      // 若originalModel属于本地构造型子系统,其内部若有本地代码型模块也要复制本地代码型原型至本地
        //      if (originalModel->getModelType() == CombineBoardModel::Type) {
        //          curProj->copyCombineModelInsideLocalCodeModel(sourceProject, originalModel);
        //      }
        //      // 其他逻辑如电气接口、电气构造型等暂不考虑
        //  }
    }
    // add by liwenyu 2023.06.10
    // 如果是母线,先清理掉端口信息，后续在syncblock的时候会根据sourceproxy的portlist同步
    // 如果不情况，在母线或者点状母线复制的时候，会导致模型里面出现空端口的情况
    if (newModel->getPrototypeName() == NPS::PROTOTYPENAME_BUSBAR
        || newModel->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
        newModel->getPortManager()->clearVariableMap();

        auto portList = pSourceProxy->portList();
        if (!portList.isEmpty()) {
            auto portMgr = newModel->getPortManager();
            foreach (auto port, portList) {
                PVariable portVariable = portMgr->getVariable(port->uuid());
                if (!portVariable) {
                    // 如果模型数据没有该端口，则创建
                    portVariable = portMgr->createVariable();
                    portVariable->setUUID(port->uuid());
                    SyncHelper::syncBlockPort(port.data(), newModel);
                }
            }
        }
    }
    // end
    SyncHelper::syncBlock(pSourceProxy.data(), newModel, canvasModel);

    m_pBlockDelegateMng->bindBlockDelegate(newModel); // 绑定模块业务代理
    connectSourceSigSlot(pSourceProxy);               // 绑定模块信号槽
    connectSourcePortSigSlot(pSourceProxy);           // 绑定模块端口信号槽

    if (createType == SourceProxy::kCreateCut || createType == SourceProxy::kCreateRevert) {
        QString oldName = newModel->getName();
        pSourceProxy->setName(oldName);
    }

    currentCanvasModel->addChildModel(newModel);

    // 恢复数据字典
    QString curDict = NPS_DataDictionaryNodeName;
    QString boardName = m_pCanvasContext->name();
    QString blockName = pSourceProxy->name();
    for (auto var : newModel->getResultSaveVariableList()) {
        QString dicName = QString("%1.%2.%3.%4").arg(curDict).arg(boardName).arg(blockName).arg(var->getName());
        ModelingServerMng::getInstance().m_pDataDictionaryServer->AddDataDictionary(dicName);
    }

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onCanvaDeleteSourceProxy(QSharedPointer<SourceProxy> pSourceProxy)
{
    if (m_pCanvasContext == nullptr) {
        return;
    }
    CanvasContext *ctx = qobject_cast<CanvasContext *>(sender());
    if (!ctx || !pSourceProxy || !ModelingServerMng::getInstance().m_pProjectMngServer)
        return;

    disconnect(pSourceProxy.data());

    QString uuid = pSourceProxy->uuid();
    if (m_boardModel->getChildModels().contains(uuid)) {
        auto model = m_boardModel->getChildModel(uuid);
        if (!model)
            return;

        m_backupBlockMap[uuid] = model; // 将模块备份
        ClipboardManager::getInstance().addBlock(model);

        m_pBlockDelegateMng->unbindBlockDelegate(model); // 绑定模块业务代理

        m_boardModel->removeChildModel(uuid);

        // @LY 删除模块级数据字典
        QString curDict = NPS_DataDictionaryNodeName;
        QString boardName = m_pCanvasContext->name();
        QString blockName = pSourceProxy->name();
        QString var = QString("%1.%2.%3").arg(curDict).arg(boardName).arg(blockName);
        if (ModelingServerMng::getInstance().m_pDataDictionaryServer) {
            ModelingServerMng::getInstance().m_pDataDictionaryServer->DelDataDictionary(var);
        }
    }

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onCanvaAddConnectorWire(QSharedPointer<ConnectorWireContext> pCWC)
{
    if (!pCWC || m_boardModel == nullptr || m_pBaseKernelInterface == nullptr)
        return;

    PBlockConnector paddConnector = PBlockConnector(new BlockConnector());

    if (paddConnector == nullptr) {
        return;
    }
    // 复制粘贴
    PBlockConnector pOriginalConnector = m_boardModel->getConnector(pCWC->originalUUID());
    if (pOriginalConnector) {
        paddConnector->copyFrom(pOriginalConnector);
    }

    SyncHelper::syncBlockConnector(pCWC.data(), paddConnector, m_boardModel.data());

    connectWireSigSlot(pCWC);

    m_boardModel->addConnector(paddConnector);

    m_pBaseKernelInterface->emitContentsChanged();

    // 连接线的SrcModel为BusSelector模块时，需显示SrcPort的名称
    auto srcModel = paddConnector->getSrcModel();
    QString strSrcPortID = paddConnector->getSrcPortUuid();
    if (srcModel && srcModel->getPrototypeName() == NPS::PROTOTYPENAME_BUSSELECTOR) {
        auto portManager = srcModel->getPortManager();
        if (portManager) {
            return;
        }
        auto variableMap = portManager->getVariableMap();
        if (variableMap.size() <= 0) {
            return;
        }
        for (auto &v : variableMap) {
            if (!v) {
                continue;
            }
            QString portUUID = v->getUUID();
            if (strSrcPortID == portUUID) {
                pCWC->setSignalName(v->getName());
                break;
            }
        }
    }
}

void KernelTouch::onCanvaDeleteConnectorWire(QSharedPointer<ConnectorWireContext> pCWC)
{
    if (!pCWC || m_boardModel == nullptr) {
        return;
    }
    QString connectorID = pCWC->uuid();
    auto canvasContext = m_boardModel->getCanvasContext();
    if (canvasContext == nullptr) {
        return;
    }
    // 输入端口的连接线被删除时，BusSelector模块需要清除所有输出端口和输出变量信息
    PBlockConnector pBlockConnector = m_boardModel->getConnector(connectorID);
    if (pBlockConnector) {
        auto model = pBlockConnector->getDstModel();
        auto source = canvasContext->getSource(model->getUUID());
        if (model && source) {
            bool b = source->getReadyDeleteFlag();
            if (!b) {
                removeBusSelectorBlockOutputPorts(model);
            }
        }
    }

    if (m_boardModel->getConnectors().contains(connectorID)) {
        m_boardModel->removeConnector(connectorID);
    }

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onCanvasLayerPropertyChanged()
{
    CanvasContext *ctx = qobject_cast<CanvasContext *>(sender());
    if (!ctx)
        return;
}

void KernelTouch::onCanvasSourceEdit(CanvasContext::SourceEditFlag flag, QString sourceUUID)
{
    CanvasContext *ctx = qobject_cast<CanvasContext *>(sender());
    if (!ctx)
        return;
    auto sharedSource = ctx->getSource(sourceUUID);
    if (!sharedSource) {
        return;
    }
    PModel model = m_boardModel->getChildModel(sourceUUID);
    if (model.isNull()) {
        ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowCommentProperty(sharedSource, m_pCanvasContext);
        return;
    }

    int modelType = model->getModelType();

    bool isCombineModel =
            (modelType == CombineBoardModel::Type || modelType == ElecCombineBoardModel::Type) ? true : false;

    m_notSyncBlock = true;
    if (isCombineModel && flag == CanvasContext::kSourceDoubleClick) {
        auto topBoardModel = model->getTopParentModel(); // 顶层画板
        if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer || topBoardModel == nullptr) {
            return;
        }
        auto *canvasWidget = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getCanvasByBoardUUID(
                topBoardModel->getUUID());
        if (canvasWidget) {
            canvasWidget->openCurrentModel(model);
            // 判断当前画板的所在的画板层次是否为存在构造型
            bool flag = DetermineWhetherStereotypeExist(model);
            if (flag) {
                canvasWidget->setNextConstructiveBoard(true);
            }
        }
    } else {
        if (!ModelingServerMng::getInstance().m_pPropertyManagerServer || m_pBaseKernelInterface == nullptr
            || m_pCanvasContext == nullptr) {
            return;
        }
        if (ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowBlockProperty(model)) {
            m_pBaseKernelInterface->emitContentsChanged();
            m_pCanvasContext->getBusinessHooksServer()->verifyCanvasStatus();
            sharedSource->emitOtherChange();
        }
    }
    m_notSyncBlock = false;
}

void KernelTouch::onSourceProxyChanged(QString key, QVariant val)
{
    SourceProxy *source = qobject_cast<SourceProxy *>(sender());
    if (!source || m_notSyncBlock || m_pBaseKernelInterface == nullptr)
        return;

    QString uuid = source->uuid();
    PModel pModel = m_boardModel->getChildModel(uuid);
    if (!pModel) {
        if (key != GKD::SOURCE_DATA_POINT && key != GKD::SOURCE_STATE) {
            m_pBaseKernelInterface->emitContentsChanged();
        }
        return;
    }

    QString strBlockNewName = source->name();
    QString strBlockOldName = pModel->getName();

    SyncHelper::syncBlock(source, pModel,
                          m_boardModel); // 同步模块数据
    if (key == GKD::SOURCE_DATA_POINT) // 展示潮流计算等数据的模块的位置的变化不触发*号 实时保存
    {
        m_pBaseKernelInterface->saveDataPointMap(uuid);
    } else if (key != GKD::SOURCE_STATE) {
        m_pBaseKernelInterface->emitContentsChanged();
    }

    if (key == GKD::SOURCE_NAME) {
        if (m_pBlockDelegateMng) {
            m_pBlockDelegateMng->getBlockDelegate(pModel)->updateOutputVars(strBlockOldName, strBlockNewName);
        }
    }
}

void KernelTouch::onAddPortContext(QSharedPointer<PortContext> portCtx)
{
    SourceProxy *source = qobject_cast<SourceProxy *>(sender());
    if (!source || !portCtx) {
        return;
    }

    PModel model = m_boardModel->getChildModel(source->uuid());
    if (model.isNull()) {
        return;
    }

    if (m_notSyncBlock) {
        return;
    }

    m_notSyncBlock = true;
    source->blockSignals(true);
    model->blockSignals(true);

    auto portVar = model->getPortManager()->createVariable();
    if (portVar) {
        portVar->setUUID(portCtx->uuid());
    }

    SyncHelper::syncBlockPort(portCtx.data(), model);

    source->blockSignals(false);
    model->blockSignals(false);
    m_notSyncBlock = false;

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onDelPortContext(QSharedPointer<PortContext> portCtx)
{
    SourceProxy *source = qobject_cast<SourceProxy *>(sender());
    if (!source || !portCtx) {
        return;
    }

    PModel model = m_boardModel->getChildModel(source->uuid());
    if (model.isNull()) {
        return;
    }

    if (m_notSyncBlock) {
        return;
    }

    m_notSyncBlock = true;
    source->blockSignals(true);
    model->blockSignals(true);

    model->getPortManager()->removeVariable(portCtx->uuid());

    m_notSyncBlock = false;
    source->blockSignals(false);
    model->blockSignals(false);

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onConnectorWireChanged()
{
    ConnectorWireContext *cwc = qobject_cast<ConnectorWireContext *>(sender());
    if (!cwc) {
        return;
    }

    QString uuid = cwc->uuid();
    PBlockConnector pBlockConnector = m_boardModel->getConnector(uuid);
    if (!pBlockConnector) {
        return;
    }

    SyncHelper::syncBlockConnector(cwc, pBlockConnector, m_boardModel.data()); // 同步模块连接线数据

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onConnectorSignalNameChanged(QString name)
{
    ConnectorWireContext *cwc = qobject_cast<ConnectorWireContext *>(sender());
    if (cwc == nullptr) {
        return;
    }

    QString uuid = cwc->uuid();
    PBlockConnector pBlockConnector = m_boardModel->getConnector(uuid);
    if (!pBlockConnector) {
        return;
    }

    auto model = pBlockConnector->getDstModel();
    if (model == nullptr) {
        return;
    }
    if (model->getPrototypeName() == NPS::PROTOTYPENAME_BUSCREATOR) {
        QString portUUID = pBlockConnector->getDstPortUuid();
        auto portVar = model->findVariable(portUUID);
        auto controlBlock = model.dynamicCast<ControlBlock>();
        if (portVar == nullptr || controlBlock == nullptr) {
            return;
        }
        auto inputGroup = controlBlock->getInputVariableGroup();
        if (inputGroup == nullptr) {
            return;
        }
        auto var = inputGroup->getVariableByName(portVar->getName());
        if (var == nullptr) {
            return;
        }

        if (name.isEmpty()) {
            portVar->setName(portVar->getDisplayName());
            var->setName(var->getDisplayName());
        } else {
            portVar->setName(name);
            var->setName(name);
        }
    }
}

void KernelTouch::onBlockPortsChanged()
{
    PortContext *pPortContext = qobject_cast<PortContext *>(sender());
    if (!pPortContext || m_boardModel == nullptr || m_pCanvasContext == nullptr)
        return;

    QString blockUUID = pPortContext->sourceUUID();
    PModel model = m_boardModel->getChildModel(blockUUID);
    auto source = m_pCanvasContext->getSource(blockUUID);
    if (!model || !source) {
        return;
    }

    m_notSyncBlock = true;
    source->blockSignals(true);
    model->blockSignals(true);

    SyncHelper::syncBlockPort(pPortContext, model);

    m_notSyncBlock = false;
    source->blockSignals(false);
    model->blockSignals(false);

    m_pBaseKernelInterface->emitContentsChanged();
}

void KernelTouch::onSourceGroupPositionChanged()
{
    m_pBaseKernelInterface->emitContentsChanged();
}
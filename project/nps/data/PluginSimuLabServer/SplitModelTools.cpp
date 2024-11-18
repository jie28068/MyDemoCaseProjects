#include "SplitModelTools.h"
#include "CoreLib/ServerManager.h"
#include "GenerateMapSchema.h"
USE_LOGOUT_("SplitModelTools")

bool SplitModelTools::genSplitModel(PDrawingBoardClass &curModel, const PDrawingBoardClass &mainBoard,
                                    PIProjectManagerServer &proj)
{
    // 1. 复制当前模型
    QString mainBoardName = mainBoard->getName();
    QString splitModelName = QString("%1_%2").arg(mainBoardName).arg(curModel->getName());
    PDrawingBoardClass copyModel = curModel->copy().dynamicCast<DrawingBoardClass>();
    copyModel->setName(splitModelName);
    copyModel->setPrototypeName(splitModelName);
    copyModel->getPortManager()->clearVariableMap();
    copyModel->removeVariableGroup(RoleDataDefinition::InputSignal);
    copyModel->removeVariableGroup(RoleDataDefinition::OutputSignal);

    // 如果控制画板上只有一个标准化的sm模块，则直接把模型保存。
    if (mainBoard->getChildModels().size() == 1) {
        proj->SaveBoardModel(copyModel);
        return true;
    }

    // 2. mainBoard按子系统分组，分别找出与curBoard相连的输入/输出线
    // {subname: {"In": [], "Out": [], "id": 0, "relId":0}}
    QMap<QString, SplitInfo> splitMap = splitModelGroup(curModel, mainBoard);

    // 3. 根据分组替换当前模型内的In/SimuLabCom和Out模块
    int index = 0;
    for (auto info : splitMap) {
        index++;
        createRecvBlock(copyModel, proj, info, index);
        createSendBlock(copyModel, proj, info, index);
    }

    // 4. 保存拆后模型
    proj->SaveBoardModel(copyModel);
    return true;
}

bool SplitModelTools::isInitiativeSplitModel(const PDrawingBoardClass &mainBoard)
{
    for (auto block : mainBoard->getChildModels()) {
        QString blockName = block->getName();
        QString bfFlag = blockName.split("_")[0].toLower();
        if (bfFlag == SL_CONST::NormalSMName || bfFlag == SL_CONST::NormalSSName) {
            return true;
        }
    }
    return false;
}

bool SplitModelTools::checkModelRule(const PDrawingBoardClass &mainBoard)
{
    // 控制画板上模型命名规则检测
    if (!GenerateMapSchema::getInstance().checkStandard(mainBoard)) {
        LOGOUT(QObject::tr("拆模型失败: 控制画板上模型名称不符合规则."), LOG_ERROR);
        return false;
    }

    // SimuLabCom规则检测
    for (auto curBoard : mainBoard->getChildModels()) {
        int count = 0;
        for (PModel block : curBoard->getChildModels()) {
            if (block->getPrototypeName() == SL_CONST::ProtoName_SimuLabCom) {
                count++;
                if (count > 1) {
                    LOGOUT(QObject::tr("拆模型失败: SimuLabCom模块一个画板最多存在一个."), LOG_ERROR);
                    return false;
                }

                QMap<QString, PBlockConnector> connectList = block->getConnectors();
                for (auto con : connectList) {
                    PModel srcModel = con->getSrcModel();
                    PModel dstModel = con->getDstModel();
                    if (dstModel->getPrototypeName() == SL_CONST::ProtoName_SimuLabCom
                        && srcModel->getPrototypeName() != SL_CONST::ProtoName_In) {
                        LOGOUT(QObject::tr("拆模型失败: SimuLabCom模块只能与In模块相连."), LOG_ERROR);
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

PVariable SplitModelTools::getPortInfoByUuid(const PModel &block, const QString &uuid)
{
    PVariableGroup portGroup = block->getVariableGroup(RoleDataDefinition::PortGroup);
    if (portGroup != nullptr) {
        QList<PVariable> portList = portGroup->getVariableSortByOrder();
        for (auto port : portList) {
            if (port->getUUID() == uuid) {
                return port;
            }
        }
    }
    return nullptr;
}
PBlockConnector SplitModelTools::getConnectorBySrcPortName(const PDrawingBoardClass &curBoard, const PModel &block,
                                                           const QString &portName)
{
    QMap<QString, PBlockConnector> connectList = curBoard->getConnectors();
    for (auto con : connectList) {
        if (con->getSrcModel() == block) {
            PVariable portInfo = getPortInfoByUuid(con->getSrcModel(), con->getSrcPortUuid());
            if (portInfo->getName() == portName) {
                return con;
            }
        }
    }
    return nullptr;
}

int SplitModelTools::getModelIndex(const PDrawingBoardClass &mainBoard, const PModel &model)
{
    QString bfFlag = model->getName().split("_")[0].toLower();
    if (bfFlag == SL_CONST::NormalSMName) {
        return 1;
    }

    int index = 1;
    for (auto curModel : mainBoard->getChildModels()) {
        QString bfFlag = curModel->getName().split("_")[0].toLower();
        if (bfFlag != SL_CONST::NormalSMName) {
            index++;
            if (curModel->getUUID() == model->getUUID()) {
                return index;
            }
        }
    }
    return -1;
}

QMap<QString, SplitModelTools::SplitInfo> SplitModelTools::splitModelGroup(const PDrawingBoardClass &curModel,
                                                                           const PDrawingBoardClass &mainBoard)
{
    QMap<QString, SplitInfo> splitMap;
    QMap<QString, PBlockConnector> connectList = mainBoard->getConnectors();
    for (auto con : connectList) {
        PModel srcBlock = con->getSrcModel();
        PModel dstBlock = con->getDstModel();
        if (srcBlock->getUUID() == curModel->getUUID()) {
            // Out相关
            PVariable portInfo = getPortInfoByUuid(srcBlock, con->getSrcPortUuid());
            if (portInfo == nullptr) {
                LOGOUT(QObject::tr("异常：模块端口不存在!"), LOG_ERROR);
                continue;
            }
            QString dstName = dstBlock->getName();
            if (!splitMap.contains(dstName)) {
                SplitInfo info;
                info.id = getModelIndex(mainBoard, srcBlock);
                info.relId = getModelIndex(mainBoard, dstBlock);
                info.out << portInfo->getName();
                info.name = curModel->getName();
                info.relModel = dstBlock;
                splitMap[dstName] = info;
            } else {
                splitMap[dstName].out << portInfo->getName();
            }
        } else if (dstBlock->getUUID() == curModel->getUUID()) {
            // In相关
            PVariable portInfo = getPortInfoByUuid(dstBlock, con->getDstPortUuid());
            if (portInfo == nullptr) {
                LOGOUT(QObject::tr("异常：模块端口不存在!"), LOG_ERROR);
                continue;
            }
            QString srcName = srcBlock->getName();
            if (!splitMap.contains(srcName)) {
                SplitInfo info;
                info.id = getModelIndex(mainBoard, dstBlock);
                info.relId = getModelIndex(mainBoard, srcBlock);
                info.in << portInfo->getName();
                info.name = curModel->getName();
                info.relModel = srcBlock;
                splitMap[srcName] = info;
            } else {
                splitMap[srcName].in << portInfo->getName();
            }
        }
    }
    return splitMap;
}

void SplitModelTools::createRecvBlock(PDrawingBoardClass &curModel, PIProjectManagerServer &proj, const SplitInfo &info,
                                      int index)
{
    if (info.in.size() <= 0) {
        return;
    }

    // 创建recv模块，并设置参数信息
    PModel recvModel = proj->CreateBlockModel(SL_CONST::ProtoName_SysTransfer);
    recvModel->setName(QString("%1_r%2").arg(SL_CONST::ProtoName_SysTransfer).arg(index));
    curModel->addChildModel(recvModel);

    // 设参数
    PVariableGroup paramGroup = recvModel->getVariableGroup(RoleDataDefinition::Parameter);
    QList<PVariable> paramList = paramGroup->getVariableSortByOrder();
    for (auto param : paramList) {
        QString paramName = param->getName();
        if (paramName == SL_CONST::SysTransferSendParam) {
            param->setDefaultValue(false);
        } else if (paramName == SL_CONST::SysTransferSrcIdParam) {
            param->setDefaultValue(info.relId);
        } else if (paramName == SL_CONST::SysTransferIdParam) {
            param->setDefaultValue(info.id);
        } else if (paramName == SL_CONST::SysTransferSTParam) {
            std::tuple<double, double> simuInfo = getSimulationInfo(proj->GetCurProject(), curModel);
            param->setDefaultValue(std::get<1>(simuInfo));
        }
    }

    // 建立连线关系
    int count = 0;
    PModel simulabcom = nullptr;
    for (auto name : info.in) {
        QMap<QString, PBlockConnector> connectList = curModel->getConnectors();
        for (auto con : connectList) {
            // 找到In关联的最终连线
            PModel srcBlock = con->getSrcModel();
            if (srcBlock->getName() != name) {
                continue;
            }
            recvModel->setPostion(srcBlock->getPosition());
            PBlockConnector relConnector = con;
            PModel dstBlock = con->getDstModel();
            if (dstBlock->getPrototypeName() == SL_CONST::ProtoName_SimuLabCom) {
                simulabcom = dstBlock;
                PVariable portInfo = getPortInfoByUuid(dstBlock, con->getDstPortUuid());
                QString inPortName = portInfo->getName();
                QString outPortName = "yo";
                if (inPortName != "yi") {
                    int portIdx = inPortName.remove("yi").toInt();
                    outPortName = QString("yo%1").arg(portIdx);
                }
                relConnector = getConnectorBySrcPortName(curModel, dstBlock, outPortName);
            }

            // 添加信号
            PVariableGroup outputGroup = recvModel->createVariableGroup(RoleDataDefinition::OutputSignal);
            auto variable = outputGroup->createVariable();
            variable->setName(QString("yo%1").arg(count));
            variable->setDataType("double");
            variable->setShowMode(Variable::VisiableReadOnly);

            // 添加端口
            auto portGroup = recvModel->getPortManager();
            PVariable portVar = portGroup->createVariable([count](PVariable outputPortVariable) {
                outputPortVariable->setDataType("double");
                outputPortVariable->setPortType(Variable::ControlOut);
                outputPortVariable->setPresetPosition(QPointF(10, 2 * (count + 1)));
                outputPortVariable->setName(QString("yo%1").arg(count));
                outputPortVariable->setDisplayName(QString("yo%1").arg(count));
                outputPortVariable->setOrder(count);
                outputPortVariable->setShowMode(Variable::VisiableReadOnly);
            });

            // 将连线关联到recv模块
            if (relConnector != nullptr) {
                PBlockConnector pAddCon = PBlockConnector(new BlockConnector());
                pAddCon->setSrcModel(recvModel);
                pAddCon->setSrcPortUuid(portVar->getUUID());
                pAddCon->setDstModel(relConnector->getDstModel());
                pAddCon->setDstPortUuid(relConnector->getDstPortUuid());
                curModel->addConnector(pAddCon);
            }

            // 删除旧连线
            for (auto con : curModel->getAdjacentConnector(srcBlock->getUUID())) {
                curModel->removeConnector(con->getUUID());
            }
            curModel->removeChildModel(srcBlock->getUUID());
            break;
        }
        count++;
    }
    if (simulabcom != nullptr) {
        for (auto con : curModel->getAdjacentConnector(simulabcom->getUUID())) {
            curModel->removeConnector(con->getUUID());
        }
        curModel->removeChildModel(simulabcom->getUUID());
    }
}

void SplitModelTools::createSendBlock(PDrawingBoardClass &curModel, PIProjectManagerServer &proj, const SplitInfo &info,
                                      int index)
{
    if (info.out.size() <= 0) {
        return;
    }

    // 创建send模块，并设置参数信息
    PModel sendModel = proj->CreateBlockModel(SL_CONST::ProtoName_SysTransfer);
    sendModel->setName(QString("%1_s%2").arg(SL_CONST::ProtoName_SysTransfer).arg(index));
    curModel->addChildModel(sendModel);

    // 设参数
    PVariableGroup paramGroup = sendModel->getVariableGroup(RoleDataDefinition::Parameter);
    QList<PVariable> paramList = paramGroup->getVariableSortByOrder();
    for (auto param : paramList) {
        QString paramName = param->getName();
        if (paramName == SL_CONST::SysTransferSendParam) {
            param->setDefaultValue(true);
        } else if (paramName == SL_CONST::SysTransferSrcIdParam) {
            param->setDefaultValue(info.id);
        } else if (paramName == SL_CONST::SysTransferIdParam) {
            param->setDefaultValue(info.relId);
        } else if (paramName == SL_CONST::SysTransferSTParam) {
            std::tuple<double, double> simuInfo = getSimulationInfo(proj->GetCurProject(), info.relModel);
            param->setDefaultValue(std::get<1>(simuInfo));
        }
    }

    // 建立连线关系
    int count = 0;
    for (auto name : info.out) {
        QMap<QString, PBlockConnector> connectList = curModel->getConnectors();
        for (auto con : connectList) {
            PModel dstBlock = con->getDstModel();
            if (dstBlock->getName() != name) {
                continue;
            }
            sendModel->setPostion(dstBlock->getPosition());
            PBlockConnector pAddCon = PBlockConnector(new BlockConnector());
            pAddCon->setSrcModel(con->getSrcModel());
            pAddCon->setSrcPortUuid(con->getSrcPortUuid());

            // 添加信号
            PVariableGroup inputGroup = sendModel->createVariableGroup(RoleDataDefinition::InputSignal);
            auto variable = inputGroup->createVariable();
            variable->setName(QString("yi%1").arg(count));
            variable->setDataType("double");
            variable->setShowMode(Variable::VisiableReadOnly);

            // 添加端口
            auto portGroup = sendModel->getPortManager();
            PVariable portVar = portGroup->createVariable([count](PVariable inputPortVariable) {
                inputPortVariable->setDataType("double");
                inputPortVariable->setPresetPosition(QPointF(0, 2 * (count + 1)));
                inputPortVariable->setPortType(Variable::ControlIn);
                inputPortVariable->setName(QString("yi%1").arg(count));
                inputPortVariable->setDisplayName(QString("yi%1").arg(count));
                inputPortVariable->setOrder(count);
                inputPortVariable->setShowMode(Variable::VisiableReadOnly);
            });
            pAddCon->setDstModel(sendModel);
            pAddCon->setDstPortUuid(portVar->getUUID());

            // 删除旧连线
            for (auto con : curModel->getAdjacentConnector(dstBlock->getUUID())) {
                curModel->removeConnector(con->getUUID());
            }
            curModel->removeChildModel(dstBlock->getUUID());
            curModel->addConnector(pAddCon);
            break;
        }
        count++;
    }
}

std::tuple<double, double> SplitModelTools::getSimulationInfo(const PKLProject &pProject, const PModel &borad)
{
    // 仿真步长、子系统交互频率
    std::tuple<double, double> simuInfo;
    double step = 0;
    double frequency = 0;
    for (auto block : borad->getChildModels()) {
        if (block->getPrototypeName() == SL_CONST::ProtoName_SimuLabCom) {
            PVariableGroup paramGroup = block->getVariableGroup(RoleDataDefinition::Parameter);
            QList<PVariable> paramList = paramGroup->getVariableSortByOrder();
            for (auto param : paramList) {
                QString paramName = param->getName();
                if (paramName == SL_CONST::ComStepParam) {
                    step = param->getDefaultValue().toDouble();
                } else if (paramName == SL_CONST::ComSTParam) {
                    frequency = param->getDefaultValue().toDouble();
                }
            }
            break;
        }
    }

    if (step == 0) {
        QVariantMap projcfgMap = pProject->getProjectConfig();
        step = projcfgMap[KL_PRO::STEP_SIZE].toDouble();
    }
    frequency = frequency == 0 ? step : frequency;
    simuInfo = std::make_tuple(step, frequency);
    return simuInfo;
}
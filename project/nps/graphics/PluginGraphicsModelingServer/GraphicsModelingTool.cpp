#include "GraphicsModelingTool.h"

#include "CombineBoardModel.h"
#include "ComplexBoardModel.h"
#include "ControlBoardModel.h"
#include "CoreLib/ServerManager.h"
#include "ElecBoardModel.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingConst.h"
#include "Manager/ModelingServerMng.h"
#include <QDateTime>
#include <QFileInfo>
#include <qglobal.h>
#include <quuid.h>

USE_LOGOUT_("GraphicsModelingTool.cpp")
namespace GraphicsModelingTool {

QString getBlockStateString(int blockType)
{
    switch (blockType) {
    case Block::StateNormal:
        return "normal";
    case Block::StateWarring:
        return "warning";
    case Block::StateError:
        return "error";
    case Block::StateDisable:
        return "disable";
    case Block::StateHangup:
        return "hangup";
    default:
        return "default";
    }
}

Block::ModelState getBlockState(QString strBlockType)
{
    if (strBlockType == "normal")
        return Block::StateNormal;
    else if (strBlockType == "warning")
        return Block::StateWarring;
    else if (strBlockType == "error")
        return Block::StateError;
    else if (strBlockType == "disable") {
        return Block::StateDisable;
    } else if (strBlockType == "hangup") {
        return Block::StateHangup;
    } else {
        return Block::StateNormal;
    }
}

void roundEven(QSizeF &size, int round)
{
    int height = size.height();
    int iFactor = height / round;
    int iMod = height % round;
    iFactor = (iMod * 2 > round) ? (iFactor + 1) : iFactor; // 四舍五入 gridSpace
    iFactor = (iFactor % 2 == 0) ? iFactor : iFactor + 1;   // 向上取偶数倍
    height = iFactor * round;

    size.setHeight(height);
}

bool getSourceProxyEnableScale(PModel pModel)
{
    if (!pModel) {
        return true;
    }
    QString prototypeName = pModel->getPrototypeName();
    QStringList disablePrototypeList;
    disablePrototypeList << GraphicsModelingConst::CONTROL_BLOCK_Sum3Gain
                         << GraphicsModelingConst::CONTROL_BLOCK_SumGain << GraphicsModelingConst::CONTROL_BLOCK_Gain
                         << GraphicsModelingConst::CONTROL_BLOCK_InvertedGain
                         << GraphicsModelingConst::CONTROL_BLOCK_Time << GraphicsModelingConst::SPECIAL_BLOCK_CTRLOUT
                         << GraphicsModelingConst::SPECIAL_BLOCK_CTRLIN << GraphicsModelingConst::CONTROL_BLOCK_ADD;
    if (disablePrototypeList.contains(prototypeName)) {
        return false;
    }
    return true;
}

bool getSourceProxyShowInsideBorder(PModel pModel)
{
    if (!pModel) {
        return false;
    }
    if (pModel->getModelType() == CombineBoardModel::Type || pModel->getModelType() == SlotBlock::Type) {
        return true;
    }
    return false;
}

bool isPortLinked(PModel pModel, PVariableGroup portGroup, QList<PBlockConnector> connectors)
{
    if (pModel == nullptr || portGroup == nullptr) {
        return false;
    }
    for (int i = 0; i < connectors.size(); i++) {
        PBlockConnector connector = connectors[i];
        if (connector == nullptr) {
            return false;
        }
        if (connector->getSrcModel()->getUUID() == pModel->getUUID()) {
            auto portVarMap = pModel->getPortManager()->getVariableMap();
            for (auto portVar : portVarMap) {
                if (portVar->getUUID() == connector->getSrcPortUuid()) {
                    return true;
                }
            }
        }
        if (connector->getDstModel()->getUUID() == pModel->getUUID()) {
            auto portVarMap = pModel->getPortManager()->getVariableMap();
            for (auto portVar : portVarMap) {
                if (portVar->getUUID() == connector->getDstPortUuid()) {
                    return true;
                }
            }
        }
    }

    return false;
}

PBlockConnector whichConnectorBlockPort(PModel pModel, PVariableGroup portGroup, QList<PBlockConnector> connectors)
{
    if (pModel == nullptr || portGroup == nullptr) {
        return PBlockConnector();
    }
    for (int i = 0; i < connectors.size(); i++) {
        PBlockConnector connector = connectors[i];
        if (connector == nullptr) {
            return PBlockConnector();
        }
        if (connector->getSrcModel()->getUUID() == pModel->getUUID()) {
            auto portVarMap = pModel->getPortManager()->getVariableMap();
            for (auto portVar : portVarMap) {
                if (portVar->getUUID() == connector->getSrcPortUuid()) {
                    return connector;
                }
            }
        }
        if (connector->getDstModel()->getUUID() == pModel->getUUID()) {
            if (connector->getDstModel()->getUUID() == pModel->getUUID()) {
                auto portVarMap = pModel->getPortManager()->getVariableMap();
                for (auto portVar : portVarMap) {
                    if (portVar->getUUID() == connector->getDstPortUuid()) {
                        return connector;
                    }
                }
            }
        }
    }

    return PBlockConnector();
}

void refreshUserDefinedBlockPorts(PModel pBoradModel)
{
    int inportTotal = 0;
    int outportTotal = 0;
    if (pBoradModel == nullptr) {
        return;
    }
    auto canvasContext = pBoradModel->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto sourceList = canvasContext->getAllSource().values();
    // 按照模块index进行端口排序
    std::sort(sourceList.begin(), sourceList.end(),
              [](const PSourceProxy &source1, const PSourceProxy &source2) -> bool {
                  return source1->getSourceProperty().getInputOutputDisplayIndex()
                          < source2->getSourceProperty().getInputOutputDisplayIndex();
              });

    auto inputGroup = pBoradModel->createVariableGroup(RoleDataDefinition::InputSignal);
    auto outputGroup = pBoradModel->createVariableGroup(RoleDataDefinition::OutputSignal);
    auto portGroup = pBoradModel->getPortManager();
    if (inputGroup == nullptr || outputGroup == nullptr || portGroup == nullptr) {
        return;
    }
    inputGroup->clearVariableMap();
    outputGroup->clearVariableMap();
    portGroup->clearVariableMap();

    for (auto source : sourceList) {
        auto model = pBoradModel->getChildModel(source->uuid());
        if (model.isNull()
            || (model->getPrototypeName() != NPS::PROTOTYPENAME_CTRLIN
                && model->getPrototypeName() != NPS::PROTOTYPENAME_CTRLOUT)) {
            continue;
        }

        auto modelUUID = model->getObjectUuid();
        auto modelName = model->getName();

        if (model->getPrototypeName() == NPS::PROTOTYPENAME_CTRLIN) {
            auto modelOutGroup = model->getVariableGroup(RoleDataDefinition::OutputSignal);
            if (modelOutGroup) {
                auto outputVar = modelOutGroup->getVariableMap().first();
                if (!outputVar) {
                    return;
                }
                if (inputGroup
                            ->findVariable(RoleDataDefinition::BoardInOutSignalModel, modelUUID) // 输入变量
                            .isEmpty()) {
                    auto variable = inputGroup->createVariable();
                    if (variable) {

                        variable->setName(modelName);
                        variable->setDataType(outputVar->getDataType());
                        variable->setShowMode(Variable::VisiableReadOnly);
                        variable->setData(RoleDataDefinition::BoardInOutSignalModel, modelUUID);
                    }
                }
                if (portGroup->findVariable(RoleDataDefinition::BoardInOutSignalModel, modelUUID)
                            .isEmpty()) { // 输入端口
                    portGroup->createVariable([outputVar, modelName, modelUUID](PVariable inputPort) {
                        if (inputPort) {
                            inputPort->setName(modelName);
                            inputPort->setDataType(outputVar->getDataType());
                            inputPort->setShowMode(Variable::VisiableReadOnly);
                            inputPort->setPortType(Variable::ControlIn);
                            inputPort->setData(RoleDataDefinition::BoardInOutSignalModel, modelUUID);
                        }
                    });
                }
            }
            inportTotal++;
        }

        if (model->getPrototypeName() == NPS::PROTOTYPENAME_CTRLOUT) {
            auto modelInputGroup = model->getVariableGroup(RoleDataDefinition::InputSignal);
            if (modelInputGroup) {
                auto inputVar = modelInputGroup->getVariableMap().first();
                if (!inputVar) {
                    return;
                }
                if (outputGroup
                            ->findVariable(RoleDataDefinition::BoardInOutSignalModel,
                                           modelUUID) // 输出变量
                            .isEmpty()) {
                    auto variable = outputGroup->createVariable();
                    variable->setName(modelName);
                    variable->setDataType(inputVar->getDataType());
                    variable->setShowMode(Variable::VisiableReadOnly);
                    variable->setData(RoleDataDefinition::BoardInOutSignalModel, modelUUID);
                }
                if (portGroup->findVariable(RoleDataDefinition::BoardInOutSignalModel, modelUUID)
                            .isEmpty()) { // 输出端口
                    portGroup->createVariable([inputVar, modelName, modelUUID](PVariable outPort) {
                        outPort->setName(modelName);
                        outPort->setDataType(inputVar->getDataType());
                        outPort->setShowMode(Variable::VisiableReadOnly);
                        outPort->setPortType(Variable::ControlOut);
                        outPort->setData(RoleDataDefinition::BoardInOutSignalModel, modelUUID);
                    });
                }
            }
            outportTotal++;
        }
    }

    int portPosDefine[5][5] = {
        { 5, 0, 0, 0, 0 }, { 3, 7, 0, 0, 0 }, { 2, 5, 8, 0, 0 }, { 2, 4, 6, 8, 0 }, { 1, 3, 5, 7, 9 }
    };

    // 完成后分配端口位置
    auto portVarMap = pBoradModel->getPortManager()->getVariableSortByOrder();
    int inputnum = 0;
    int outputnum = 0;
    foreach (auto portVar, portVarMap) {
        if (portVar->getPortType() == Variable::ControlIn) {
            if (inportTotal < 5) {
                int rownum = inportTotal < 5 ? (inportTotal - 1) : 4;
                portVar->setPresetPosition(QPointF(0, portPosDefine[rownum][inputnum]));
            } else {
                portVar->setPresetPosition(QPointF(0, 10.0 / (inportTotal + 1) * (inputnum + 1)));
            }
            inputnum++;
        } else if (portVar->getPortType() == Variable::ControlOut) {
            if (outportTotal < 5) {
                int rownum = outportTotal < 5 ? (outportTotal - 1) : 4;
                portVar->setPresetPosition(QPointF(10, portPosDefine[rownum][outputnum]));
            } else {
                portVar->setPresetPosition(QPointF(10, 10.0 / (outportTotal + 1) * (outputnum + 1)));
            }
            outputnum++;
        }
    }
}

bool calculateBlockUuid(QString &uuid, QString &name, PModel pModel, PModel pBoardModel)
{
    if (pModel == nullptr || pBoardModel == nullptr) {
        return false;
    }
    QString indexKey = pModel->getPrototypeName_Readable().isEmpty() ? pModel->getPrototypeName()
                                                                     : pModel->getPrototypeName_Readable();
    if (indexKey == GraphicsModelingConst::SPECIAL_BLOCK_DOTBUSBAR) {
        // 点状busbar和busbar原型名称不一致，但显示都为busbar，共用递增序号
        indexKey = GraphicsModelingConst::SPECIAL_BLOCK_BUSBAR;
    }
    int index = 1;
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer.isNull()) {
        LOGOUT("IPluginGraphicModelingServer未注册", LOG_ERROR);
    } else {
        index = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getBlockPrototypeNextIndex(indexKey,
                                                                                                       pBoardModel);
    }
    if (index == 0) {
        name = QString("%1").arg(pModel->getPrototypeName_Readable().isEmpty() ? pModel->getPrototypeName()
                                                                               : pModel->getPrototypeName_Readable());
    } else {
        name = QString("%1%2")
                       .arg(pModel->getPrototypeName_Readable().isEmpty() ? pModel->getPrototypeName()
                                                                          : pModel->getPrototypeName_Readable())
                       .arg(index);
    }
    return true;
}

bool enableBlockAdd(PModel pModel, PModel pBoardModel)
{
    if (pModel == nullptr || pBoardModel == nullptr) {
        return false;
    }
    if (CombineBoardModel::Type != pBoardModel->getModelType()
        && ElecCombineBoardModel::Type != pBoardModel->getModelType()) {
        return true;
    }

    if (CombineBoardModel::Type != pModel->getModelType() && ElecCombineBoardModel::Type != pModel->getModelType()) {
        return true;
    }

    // 自定义控制系统画板
    // 1、不能添加自己
    if (pModel->getPrototypeName() == pBoardModel->getPrototypeName()) {
        LOGOUT(QObject::tr("add the stereotype of module to the drawing board."),
               LOG_WARNING); // 添加的构造型模块不能为本画板。
        return false;
    }

    // 2、不能添加包含自己的构造型模块
    // 3、选择不包含自己的构造型模块时，该模块中的容器不能间接的包含自己
    if (containsUserDefinedBlock(pModel, pBoardModel)) {
        LOGOUT(QObject::tr("add the stereotype of module cannot contain the drawing board."),
               LOG_WARNING); // 添加的构造型模块不能包含本画板。
        return false;
    }

    // 4、构造型模块的子系统中不能添加这个构造型模块
    if (pBoardModel->getTopParentModel()->getPrototypeName() == pModel->getPrototypeName()) {
        LOGOUT(QObject::tr("the added stereotype module cannot be the parent module of this drawing board."),
               LOG_WARNING); // 添加的构造型模块不能为本画板的父模块。
        return false;
    }

    return true;
}

bool containsUserDefinedBlock(PModel model, PModel pBoardModel)
{
    if (model == nullptr || pBoardModel == nullptr) {
        return false;
    }

    // 这里需要判断model里面有没有包含pBoardModel
    auto childModelMap = model->getChildModels();
    for (auto childModel : childModelMap) {
        if (childModel->getPrototypeName() == pBoardModel->getPrototypeName()) {
            return true;
        }

        if (containsUserDefinedBlock(childModel, pBoardModel)) {
            return true;
        }
    }

    return false;
}

QList<PModel> findBlockByPrototypeName(const QString &prototypeName, PModel pBoardModel)
{
    QList<PModel> retList;

    QStringList keys = pBoardModel->getChildModels().keys();
    for each (QString key in keys) {
        if (pBoardModel->getChildModels()[key]->getPrototypeName() == prototypeName)
            retList.append(pBoardModel->getChildModel(key));
    }

    return retList;
}

} // end namespace GraphicsModelingTool
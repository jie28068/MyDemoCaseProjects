#include "ElectricalCanvasHooksServer.h"
#include "Associate.h"
#include "CanvasWidget.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "Manager/ModelingServerMng.h"
#include "PublicDefine.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <GraphicsModelingConst.h>
#include <GraphicsModelingTool.h>

using namespace Kcc::BlockDefinition;

USE_LOGOUT_("PIPluginGraphicModelingServer")
ElectricalCanvasHooksServer::ElectricalCanvasHooksServer(QSharedPointer<CanvasContext> pcanvascontext,
                                                         QSharedPointer<Kcc::BlockDefinition::Model> model)
    : SimuNPSBusinessHooksServer(pcanvascontext, model)
{
}

bool ElectricalCanvasHooksServer::enableCreate(int cpsourcetype, QMap<QString, QSharedPointer<SourceProxy>> cpsourcemap)
{
    bool cancreate = false;
    QSharedPointer<CanvasContext> canvasContext = getCanvasContext();
    if (canvasContext == nullptr) {
        return false;
    }
    int cpdrawboardtype = Utility::getModelTypeByCanvasType(CanvasContext::Type(cpsourcetype));
    int currentdrawboardtype = Utility::getModelTypeByCanvasType(canvasContext->type());
    if (ElecBoardModel::Type == currentdrawboardtype) {
        if (ElecBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        } else if (ElecCombineBoardModel::Type == cpdrawboardtype) {
            foreach (PSourceProxy psource, cpsourcemap.values()) {
                // 电气画板不能放node模块和单线变换器模块
                if (NPS::PROTOTYPENAME_EXTERNAL != psource->prototypeName()
                    && NPS::PROTOTYPENAME_SINGAL != psource->prototypeName()) {
                    cancreate = true; // 有可以拷贝的模块
                    break;
                }
            }
        }
    } else if (ElecCombineBoardModel::Type == currentdrawboardtype) {
        cancreate = true;
    }
    return cancreate;
}

bool ElectricalCanvasHooksServer::enableCreate(int cpsourcetype, QSharedPointer<SourceProxy> cpsource)
{
    bool cancreate = false;
    QSharedPointer<CanvasContext> canvasContext = getCanvasContext();
    if (canvasContext == nullptr) {
        return false;
    }
    int cpdrawboardtype = Utility::getModelTypeByCanvasType(CanvasContext::Type(cpsourcetype));
    int currentdrawboardtype = Utility::getModelTypeByCanvasType(canvasContext->type());
    if (ElecBoardModel::Type == currentdrawboardtype) {
        if (ElecBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        } else if (ElecCombineBoardModel::Type == cpdrawboardtype) {
            // 电气画板不能放node模块
            if (NPS::PROTOTYPENAME_EXTERNAL != cpsource->prototypeName()
                && NPS::PROTOTYPENAME_SINGAL != cpsource->prototypeName()) {
                cancreate = true; // 有可以拷贝的模块
            }
        }
    } else if (ElecCombineBoardModel::Type == currentdrawboardtype) {
        cancreate = true;
    }
    return cancreate;
}

void ElectricalCanvasHooksServer::setCreatePortContext(QSharedPointer<PortContext> portCtx)
{
    portCtx->setType("input");
    portCtx->setName("BUS");
}

double ElectricalCanvasHooksServer::getElectricalBlockVoltage(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr) {
        return 0.0;
    }

    PElectricalBlock elecblock = getBlock(source);
    if (elecblock) {
        return elecblock->getElectricalVariableValue("Voltage").toDouble();
    }
    return 0.0;
}

QString ElectricalCanvasHooksServer::getElectricalBlockPhase(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr) {
        return QString();
    }

    PElectricalBlock elecblock = getBlock(source);
    if (elecblock) {
        PVariable bustypeVar = elecblock->getVariableByRoleData(RoleDataDefinition::ElectricalParameter,
                                                                RoleDataDefinition::NameRole, NPS::KEYWORDS_BUSBAR_AC);
        if (bustypeVar == nullptr) {
            return QString();
        }
        return NPS::RealValueMaptoControlValue(elecblock->getVariableData(bustypeVar, RoleDataDefinition::ValueRole),
                                               bustypeVar->getControlType(), bustypeVar->getControlValue())
                .toString();
    }
    return QString();
}

QSharedPointer<ElectricalBlock> ElectricalCanvasHooksServer::getBlock(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr || getCanvasContext() == nullptr) {
        return QSharedPointer<ElectricalBlock>();
    }
    PDrawingBoardClass pboard = qSharedPointerDynamicCast<DrawingBoardClass>(m_pBoardClass);
    if (pboard) {
        return pboard->getChildModel(source->uuid()).dynamicCast<ElectricalBlock>();
    }

    return QSharedPointer<ElectricalBlock>();
}

// QSharedPointer<DrawingBoardClass> ElectricalCanvasHooksServer::getDrawingBoard(const QString &uuid)
// {
//     if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
//         return QSharedPointer<DrawingBoardClass>();
//     }
//     auto boardlist = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
//     foreach (auto pboard, boardlist) {
//         if (pboard != nullptr && pboard->getUUID() == uuid) {
//             return pboard.dynamicCast<DrawingBoardClass>();
//         }
//     }
//     return QSharedPointer<DrawingBoardClass>();
// }

int ElectricalCanvasHooksServer::getElectricalPhaseNumber(QSharedPointer<SourceProxy> source)
{
    int phaseNumber = -1;
    if (source == nullptr) {
        return phaseNumber;
    }

    PElectricalBlock elecblock = getBlock(source);

    if (elecblock) {
        int acType = elecblock->getElectricalVariableValue("AC").toInt();
        if (acType == 1) {
            phaseNumber = 3;
        } else {
            phaseNumber = 1;
        }
    }
    return phaseNumber;
}

bool ElectricalCanvasHooksServer::isBlockCtrlCombined(QSharedPointer<SourceProxy> source)
{
    return false;
}

void ElectricalCanvasHooksServer::verifyCanvasStatus()
{
    PCanvasContext canvasCtx = getCanvasContext();
    if (canvasCtx.isNull()) {
        return;
    }

    CanvasWidget *canvasWidget = findCanvasWidget();

    if (canvasWidget) {
        canvasWidget->verifyStatus();
    }
}

bool ElectricalCanvasHooksServer::checkNameValid(const QString &nameString)
{
    // 如果更改的名字长度超长，提示错误
    if (nameString.length() > BLOCK_NAME_MAX_LENGHT) {
        LOGOUT(QString(QObject::tr("the name exceeds the length limit%1!")).arg(BLOCK_NAME_MAX_LENGHT),
               LOG_ERROR); // 该名称超过长度限制%1!
        return false;
    }
    // 判断当前画板中是否有重名模块
    PCanvasContext canvasCtx = getCanvasContext();
    if (canvasCtx.isNull()) {
        return true;
    }
    QMap<QString, QSharedPointer<SourceProxy>> allsourcemap = canvasCtx->getAllSource();
    QMapIterator<QString, QSharedPointer<SourceProxy>> iter(allsourcemap);
    foreach (QSharedPointer<SourceProxy> psource, allsourcemap.values()) {
        if (psource != nullptr && psource->name() == nameString) {
            blog(QObject::tr(
                         "the name already exists in the current drawing board. please reselect the type and name!"),
                 LOG_ERROR); // 该名称在当前画板中已经存在，请重新选择类型和命名！
            return false;
        }
    }
    return true;
}

void ElectricalCanvasHooksServer::showWaringDialg(const QString &tips)
{
    if (ModelingServerMng::getInstance().m_pPropertyManagerServer != nullptr) {
        ModelingServerMng::getInstance().m_pPropertyManagerServer->openWarningDlg(tips);
    }
}

void ElectricalCanvasHooksServer::showPropertyDialog()
{
    if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        return;
    }
    // 获取当前活动页面标题
    QString activeDrawingDoardUUID =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getActiveDrawingBoardUUID();
    if (activeDrawingDoardUUID.isEmpty()) {
        return;
    }
    // 获取活动页面数据结构
    PDrawingBoardClass pDrawingBoardInfo =
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->getDrawingBoardInfoById(activeDrawingDoardUUID);
    if (pDrawingBoardInfo == nullptr) {
        LOGOUT(QString(QObject::tr("query page %1 information failure")).arg(activeDrawingDoardUUID),
               LOG_ERROR); // 查询页面%1 信息失败
        return;
    }
    if (ModelingServerMng::getInstance().m_pPropertyManagerServer) {
        ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowDrawBoardProperty(pDrawingBoardInfo,
                                                                                         QObject::tr("Legend")); // 图例
    }
}

void ElectricalCanvasHooksServer::changeTextNameHint(const QString &oldname, const QString &newname)
{
    // 如果更改的名字长度超长，提示错误
    if (newname.length() > BLOCK_NAME_MAX_LENGHT) {
        LOGOUT(QString(QObject::tr("the name exceeds the length limit%1!")).arg(BLOCK_NAME_MAX_LENGHT),
               LOG_ERROR); // 该名称超过长度限制%1!
        return;
    }
    QString str = QString(QObject::tr("the name of electrical components[%1],from[%2] modify to [%3]"))
                          .arg(oldname)
                          .arg(oldname)
                          .arg(newname); // 电气元件[%1]的名称，从[%2]修改为[%3]
    blog(str, LOG_NORMAL);
}

QList<showDataStruct> ElectricalCanvasHooksServer::getPowerFlowData(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr) {
        return QList<showDataStruct>();
    }
    PElectricalBlock elecblock = getBlock(source);
    QList<showDataStruct> result;
    if (!elecblock || !elecblock->getLoadFlowResultVariableGroup()) {
        return result;
    }
    auto varMap = elecblock->getLoadFlowResultVariableGroup()->getVariableMap();
    auto keys = varMap.keys();
    for (int i = 0; i < keys.size(); i++) {
        auto var = varMap.value(keys.at(i));
        if (var) {
            showDataStruct showData;
            showData.key = var->getName();
            showData.name = var->getDisplayName();
            showData.value = var->getDefaultValue().toString();
            showData.unit = var->getDataUnit();
            result.push_back(showData);
        }
    }

    return result;
}

int ElectricalCanvasHooksServer::getBusBarPortNumber(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr) {
        return -1;
    }
    PElectricalBlock elecblock = getBlock(source);
    if (elecblock) {
        return elecblock->getElectricalVariableValue("BUS").toInt();
    }
    return -1;
}

void ElectricalCanvasHooksServer::generatePecadCase()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    if (getCanvasContext() != nullptr) {
        ModelingServerMng::getInstance().m_pElecSysServer->CreatePECADCase(
                findBoardModel(), ModelingServerMng::getInstance().m_pProjectMngServer->GetCurrentProjectAbsoluteDir());
    }
}

void ElectricalCanvasHooksServer::copyModelVariableGroups(PSourceProxy srcSource, PSourceProxy dstSource)
{
    if (srcSource == nullptr || dstSource == nullptr) {
        return;
    }
    PElectricalBlock srcElecBlock = getBlock(srcSource);
    PElectricalBlock dstElecBlock = getBlock(dstSource);
    if (!srcElecBlock || !dstElecBlock) {
        return;
    }
    auto variableList = srcElecBlock->getVariableGroupList();
    foreach (auto var, variableList) {
        if (var) {
            QString groupType = var->getGroupType();
            if (groupType == Kcc::BlockDefinition::RoleDataDefinition::PortGroup) {
                continue;
            }
            auto dstVar = dstElecBlock->getVariableGroup(var->getGroupType());
            if (dstVar) {
                dstVar->copyFromGroup(var);
            }
        }
    }
}

QColor ElectricalCanvasHooksServer::GetBusbarVoltageLevelColor(const double &voltageLevel)
{
    if (ModelingServerMng::getInstance().m_pProjectMngServer) {
        return ModelingServerMng::getInstance().m_pProjectMngServer->GetColorOfVolLevel(voltageLevel);
    }

    return QColor();
}

QString ElectricalCanvasHooksServer::getElectricalPortType(QSharedPointer<SourceProxy> source, QString portUUID)
{
    if (source == nullptr) {
        return QString();
    }

    PElectricalBlock electricalBlock = getBlock(source);
    if (electricalBlock) {
        auto pvg = electricalBlock->getPortManager();
        PVariable portVariable = pvg->getVariable(portUUID);
        if (!portVariable) {
            return "";
        }
        auto portType = portVariable->getPortType();
        if (portType == Variable::Electrical_AC) {
            return "AC";
        } else if (portType == Variable::Electrical_DC) {
            return "DC";
        } else if (portType == Variable::Electrical) {
            return "ACDC";
        }
    }
    return QString();
}

void ElectricalCanvasHooksServer::setElectricalACDCType(QSharedPointer<SourceProxy> source, int type)
{
    if (source == nullptr) {
        return;
    }

    PElectricalBlock electricalBlock = getBlock(source);
    if (electricalBlock) {
        electricalBlock->setElectricalVariableValue("AC", type);
        // electricalBlock->setElectricalVariableValue("DC", type + 1);
    }
}
bool ElectricalCanvasHooksServer::isBlockCtrlCodeType(QSharedPointer<SourceProxy> source)
{
    if (!ModelingServerMng::getInstance().m_pModelManagerServer || source == nullptr) {
        return false;
    }
    auto name = source->prototypeName();

    if (ModelingServerMng::getInstance().m_pModelManagerServer->ContainToolkitModel(name, QString())) {
        return true;
    }
    return false;
}

void ElectricalCanvasHooksServer::copyModelAssociationRelation(const QString &boardUUID, const QString &srcBlockUUID,
                                                               const QString &dstBlockUUID)
{
    if (boardUUID.isEmpty() || srcBlockUUID.isEmpty() || dstBlockUUID.isEmpty()) {
        return;
    }

    PModel boardModel = findBoardModel();
    if (!boardModel) {
        return;
    }
    auto srcBlock = boardModel->findChildModelRecursive(srcBlockUUID);
    auto dstBlock = boardModel->findChildModelRecursive(dstBlockUUID);
    if (!srcBlock || !dstBlock) {
        return;
    }

    for (auto ass : srcBlock->getAllAssociate()) {
        if (ass) {
            // 断开原始关联关系
            srcBlock->disconnectModel(ass->target, ass->type);
            // 建立新的关联关系
            dstBlock->connectModel(ass->target, ass->type, ass->mirrored);
        }
    }
}

bool ElectricalCanvasHooksServer::isElecCombineModel(const QString &blockUUID)
{
    auto topBoardModel = findBoardModel();
    if (topBoardModel == nullptr) {
        return false;
    }

    PModel findModel = topBoardModel->findChildModelRecursive(blockUUID);
    if (!findModel) {
        return false;
    }
    return findModel->getModelType() == ElecCombineBoardModel::Type ? true : false;
}

bool ElectricalCanvasHooksServer::checkBlockState(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr) {
        return false;
    }
    if (source->state() == "error"
        && (source->statetips() == ERROR_INFO_PROTOTYPE_MOD_NOUSE || source->statetips() == ERROR_INFO_PROTOTYPE_DEL
            || source->statetips() == ERROR_INFO_PROTOTYPE_MOD_NULL
            || source->statetips() == ERROR_INFO_PROTOTYPE_MOD_NULL_EN)) {
        return false;
    }
    return true;
}
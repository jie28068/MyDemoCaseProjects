#include "ControlCanvasHooksServer.h"
#include "CanvasWidget.h"
#include "CombineBoardModel.h"
#include "ComplexBoardModel.h"
#include "ControlBoardModel.h"
#include "CoreLib/ServerManager.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingKernel/Utility.h"
#include "GraphicsModelingTool.h"
#include "Manager/ModelingServerMng.h"
#include "ModelWidget.h"
#include <QUuid>
#include <qmath.h>

USE_LOGOUT_("PIPluginGraphicModelingServer")
ControlCanvasHooksServer::ControlCanvasHooksServer(QSharedPointer<CanvasContext> pcanvascontext,
                                                   QSharedPointer<Kcc::BlockDefinition::Model> model)
    : SimuNPSBusinessHooksServer(pcanvascontext, model)
{
}

bool ControlCanvasHooksServer::enableCreate(int cpsourcetype, QSharedPointer<SourceProxy> cpsource)
{
    bool cancreate = false;
    if (cpsource == nullptr) {
        return false;
    }
    QSharedPointer<CanvasContext> canvasContext = getCanvasContext();
    if (canvasContext == nullptr) {
        return false;
    }
    int cpdrawboardtype = Utility::getModelTypeByCanvasType(CanvasContext::Type(cpsourcetype));
    int currentdrawboardtype = Utility::getModelTypeByCanvasType(canvasContext->type());
    if (ControlBoardModel::Type == currentdrawboardtype) {
        if (ControlBoardModel::Type == cpdrawboardtype) {

            cancreate = true; // 同类型画板可粘贴
        } else if (CombineBoardModel::Type == cpdrawboardtype) {
            if ("In" != cpsource->prototypeName() && "Out" != cpsource->prototypeName()) {
                cancreate = true; // 有可以拷贝的模块
            }
        }
    } else if (CombineBoardModel::Type == currentdrawboardtype) {
        if (CombineBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        } else if (ControlBoardModel::Type == cpdrawboardtype) {
            // 控制系统不能放ElectricalInterface模块
            if ("ElectricalInterface" != cpsource->prototypeName()) {
                cancreate = true; // 有可以拷贝的模块
            }
        }
    } else if (ComplexBoardModel::Type == currentdrawboardtype) {
        if (ComplexBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        }
    }
    return cancreate;
}

bool ControlCanvasHooksServer::isBlockCtrlCombined(QSharedPointer<SourceProxy> source)
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer || source == nullptr) {
        return false;
    }
    QString blockUUID = source->uuid();
    QString blockDrawName = source->canvasName(); // 模块所在画板名
    auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
    foreach (auto board, boardList) {
        if (board->getName() == blockDrawName) {
            PModel model = board->getChildModel(blockUUID);
            if (model) {
                if (model->getModelType() == CombineBoardModel::Type)
                    return true;
            }
        }
    }
    return false;
}

void ControlCanvasHooksServer::openModelBoard(SourceProxy *source)
{
    if (source == nullptr) {
        return;
    }
    if (!ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        LOGOUT("IPluginGraphicModelingServer未注册", LOG_ERROR);
        return;
    }

    // 打开项目中的构造画板
    ModelingServerMng::getInstance().m_pGraphicsModelingServer->modifyUserDefineBlock(source->prototypeName());

    // 显示下一层时.如果当前画板是fullScreen,则缩小当前画板,放大下一层画板
    // QString drawingBoardName = source->canvasName();
    // bool isfull =
    // ModelingServerMng::getInstance().m_pGraphicsModelingServer->drawBoardIsFullScreen(drawingBoardName); if (isfull)
    // {
    //     // 全屏画板
    //     ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->FullscreenWorkareaWidget(source->name());
    // }
}

void ControlCanvasHooksServer::showWaringDialg(const QString &tips)
{
    if (ModelingServerMng::getInstance().m_pPropertyManagerServer != nullptr) {
        ModelingServerMng::getInstance().m_pPropertyManagerServer->openWarningDlg(tips);
    }
}

bool ControlCanvasHooksServer::enableCreate(int cpsourcetype, QMap<QString, QSharedPointer<SourceProxy>> cpsourcemap)
{
    bool cancreate = false;
    QSharedPointer<CanvasContext> canvasContext = getCanvasContext();
    if (canvasContext == nullptr) {
        return false;
    }
    int cpdrawboardtype = Utility::getModelTypeByCanvasType(CanvasContext::Type(cpsourcetype));
    int currentdrawboardtype = Utility::getModelTypeByCanvasType(canvasContext->type());
    if (ControlBoardModel::Type == currentdrawboardtype) {
        if (ControlBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        } else if (CombineBoardModel::Type == cpdrawboardtype) {
            foreach (PSourceProxy psource, cpsourcemap.values()) {
                if (psource == nullptr) {
                    continue;
                }
                // 控制系统不能放in out模块
                if ("In" != psource->prototypeName() && "Out" != psource->prototypeName()) {
                    cancreate = true; // 有可以拷贝的模块
                    break;
                }
            }
        }
    } else if (CombineBoardModel::Type == currentdrawboardtype) {
        if (CombineBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        } else if (ControlBoardModel::Type == cpdrawboardtype) {
            // 控制系统不能放ElectricalInterface模块
            foreach (PSourceProxy psource, cpsourcemap.values()) {
                if (psource == nullptr) {
                    continue;
                }
                if ("ElectricalInterface" != psource->prototypeName()) {
                    cancreate = true; // 有可以拷贝的模块
                    break;
                }
            }
        }
    } else if (ComplexBoardModel::Type == currentdrawboardtype) {
        if (ComplexBoardModel::Type == cpdrawboardtype) {
            cancreate = true; // 同类型画板可粘贴
        }
    }
    return cancreate;
}

bool ControlCanvasHooksServer::checkNameValid(const QString &nameString)
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

void ControlCanvasHooksServer::showPropertyDialog()
{
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
        auto cWidget = findModelWidget();
        if (cWidget) {
            PModel drawboardModel = cWidget->getModel();
            if (drawboardModel) {
                auto drawingBoardpr = drawboardModel.dynamicCast<DrawingBoardClass>();
                if (drawingBoardpr) {
                    ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowDrawBoardProperty(
                            drawingBoardpr, QObject::tr("Legend")); // 图例
                }
            } else {
                ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowDrawBoardProperty(
                        pDrawingBoardInfo,
                        QObject::tr("Legend")); // 图例
            }
        }
    }
}

bool ControlCanvasHooksServer::isExistBlock(QSharedPointer<SourceProxy> source)
{
    if (source.isNull()) {
        return false;
    }

    if (source->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION) {
        return true;
    }

    auto childModel = m_pBoardClass->getChildModel(source->uuid());
    if (!childModel) {
        return true;
    }
    if (childModel->getModelType() == CombineBoardModel::Type) {
        // 构造型画板即使原型不存在了也不会受影响
        return true;
    }

    return ModelingServerMng::getInstance().m_pProjectMngServer->IsExistModel(source->prototypeName());
}

bool ControlCanvasHooksServer::checkBlockState(QSharedPointer<SourceProxy> source)
{
    if (source == nullptr) {
        return false;
    }
    if (source->state() == "error"
        && (source->statetips() == ERROR_INFO_PROTOTYPE_MOD_NOUSE || source->statetips() == ERROR_INFO_PROTOTYPE_DEL
            || source->statetips() == ERROR_INFO_PROTOTYPE_MOD_NULL)) {
        return false;
    }
    return true;
}

void ControlCanvasHooksServer::changeTextNameHint(const QString &oldname, const QString &newname)
{
    // 如果更改的名字长度超长，提示错误
    if (newname.length() > BLOCK_NAME_MAX_LENGHT) {
        LOGOUT(QString(QObject::tr("the name exceeds the length limit%1!")).arg(BLOCK_NAME_MAX_LENGHT),
               LOG_ERROR); // 该名称超过长度限制%1!
        return;
    }
    QString str = QString(QObject::tr("the name of control elements[%1],from [%2]modify to[%3]"))
                          .arg(oldname)
                          .arg(oldname)
                          .arg(newname); // 控制元件[%1]的名称，从[%2]修改为[%3]
    blog(str, LOG_NORMAL);
}

bool ControlCanvasHooksServer::isBlockCtrlUserCombined(QSharedPointer<SourceProxy> source)
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer || source == nullptr) {
        return false;
    }
    QString blockUUID = source->uuid();
    QString blockDrawName = m_canvascontext->name();
    auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
    foreach (auto board, boardList) {
        if (board->getName() == blockDrawName) {
            PModel model = board->getChildModel(blockUUID);
            if (model) {
                // 插槽模块与本地构造型，电气接口型
                if (model->getModelType() == CombineBoardModel::Type || model->getModelType() == SlotBlock::Type
                    || model->getModelType() == ElectricalContainerBlock::Type) {
                    return true;
                }
                // 用户自定义的代码型
                PControlBlock contrlblock = model.dynamicCast<ControlBlock>();
                if (!contrlblock) {
                    continue;
                }
                // if (contrlblock->getClassification() == ControlBlock::Classification::Other) {
                //     return true;
                // }
            }
        }
    }
    return false;
}

bool ControlCanvasHooksServer::sourceNameIsShowWhenSelectedChanged(bool isSelected)
{
    return true;
    return isSelected;
}

bool ControlCanvasHooksServer::isBlockCtrlCodeType(QSharedPointer<SourceProxy> source)
{
    if (!ModelingServerMng::getInstance().m_pModelManagerServer || source == nullptr) {
        return false;
    }
    auto name = source->prototypeName();

    if (ModelingServerMng::getInstance().m_pModelManagerServer->ContainToolkitModel(name, QString())) {
        auto model = ModelingServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(name);
        if (model) {
            auto type = model->getModelType();
            if (type == Model::Combine_Board_Type || type == Model::Ele_CombineBoard_Type) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool ControlCanvasHooksServer::isShowRunningSort()
{
    PIProjectManagerServer managerServer = ModelingServerMng::getInstance().m_pProjectMngServer;
    if (!managerServer) {
        return false;
    }
    auto currentProject = managerServer->GetCurProject();
    if (!currentProject) {
        return false;
    }
    QVariant flag = currentProject->getProjectConfig(KL_PRO::SHOW_SIMULATION_SORT);
    if (!flag.isValid()) {
        return false;
    }
    return flag.toBool();
}

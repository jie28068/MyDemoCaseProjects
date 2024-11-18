#include "PropertyManagerServer.h"
#include "ComplexInstanceProperty.h"
#include "CoreLib/DocHelper.h"
#include "DeviceModelProperty.h"
#include "FontWidget.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KLWidgets/KCustomDialog.h"
#include "LayerProperty.h"
#include "ProjectSetProperty.h"
#include "PropertyDialog.h"
#include "PropertyServerMng.h"
#include "SimuParamsProperty.h"
#include "blockpropertycontrol.h"
#include "blockpropertyelectrical.h"
#include "controlblocktypeselectview.h"
#include "createdrawingboardwidget.h"
#include "drawboardproperty.h"
#include "eleccontainerproperty.h"
#include "graphcommentproperty.h"
#include "iconblockproperty.h"
#include "slotproperty.h"
#include "textboxproperty.h"
#include <QDebug>
#include <QLabel>
#include <QPushButton>
using namespace Kcc::DocHelper;

USE_LOGOUT_("PropertyManagerServer")

#define QtSharedPointerNullReturnVoid(ptr)                                                                             \
    if (ptr.isNull()) {                                                                                                \
        return false;                                                                                                  \
    }

static const int PROPERTY_WARNING_DLG_WIDTH = 300;
static const int PROPERTY_WARNING_DLG_HEIGHT = 100;

static const int PROPERTY_FONTDLG_WIDTH = 480;
static const int PROPERTY_FONTDLG_HEIGHT = 300;

PropertyManagerServer::PropertyManagerServer()
    : m_bIsDialogPopup(false), m_bIsDrawingBoardRunning(false), m_pCustomDlg(nullptr)
{
}

PropertyManagerServer::~PropertyManagerServer(void) { }

void PropertyManagerServer::Init()
{
    PropertyServerMng::getInstance().init();
    PropertyServerMng::getInstance().propertyServer = this;
    if (PropertyServerMng::getInstance().m_simulationManagerServerIF != nullptr) {
        PropertyServerMng::getInstance().m_simulationManagerServerIF->connectNotify(
                Notify_DrawingBoardRunning, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        PropertyServerMng::getInstance().m_simulationManagerServerIF->connectNotify(
                Notify_DrawingBoardStopped, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    }
}

void PropertyManagerServer::UnInit()
{
    m_pCurrentDrawingBoardInfo.clear();
    PropertyServerMng::getInstance().unInit();
}

bool PropertyManagerServer::ShowBlockProperty(QSharedPointer<Kcc::BlockDefinition::Model> pModel, bool readOnly)
{
    if (pModel == nullptr) {
        return false;
    }
    bool isPropertyChanged = false;
    bool isreadOnly = (readOnly || m_bIsDrawingBoardRunning
                       || (ControlBlock::Type == pModel->getModelType() && pModel->getParentModel() == nullptr
                           && !pModel->isInstance()));
    QString modelProtoType = pModel->getPrototypeName();
    QString shortcutPath = "";
    if (ElectricalBlock::Type == pModel->getModelType() || ElecCombineBoardModel::Type == pModel->getModelType()) {
        // 电气模块
        ElectricalBlockPropertyEditor editor(pModel, isreadOnly);
        editor.init();
        showPropertyDialog(&editor, NPS::DLGMEMORY_BLOCK, "", shortcutPath, modelProtoType);
        isPropertyChanged = editor.isPropertyChanged();
        // propertyKeyChangedList = editor.getPropertyKeyChangedList();
        removePropertyWrapper(&editor);
    } else if (ControlBlock::Type == pModel->getModelType() || CombineBoardModel::Type == pModel->getModelType()) {
        // 控制模块&构造模块
        ControlBlockPropertyEditor ctredit(
                pModel, pModel->isInstance() ? CMA::Operation_EditCtrInstance : CMA::Operation_EditCtrProto,
                isreadOnly);
        ctredit.init();
        if (modelProtoType == NPS::PROTOTYPENAME_FMU) {
            // FMU模块默认初始显示“FMU概览”页面
            showPropertyDialog(&ctredit, NPS::DLGMEMORY_BLOCK, CMA::BLOCK_TAB_NAME_FMU_OVERVIEW, shortcutPath,
                               modelProtoType);
        } else {
            // 控制模块默认初始显示“参数”页面，如果没有参数则显示“类型”页面
            showPropertyDialog(&ctredit, NPS::DLGMEMORY_BLOCK, CMA::BLOCK_TAB_NAME_CTRL_PARAM, shortcutPath,
                               modelProtoType);
        }
        isPropertyChanged = ctredit.isPropertyChanged();
        removePropertyWrapper(&ctredit);
    } else if (ElectricalContainerBlock::Type == pModel->getModelType()) {
        // 电气接口型模块
        ElecContainerProperty editor(pModel, isreadOnly);
        editor.init();
        showPropertyDialog(&editor, NPS::DLGMEMORY_BLOCK, "", shortcutPath, modelProtoType);
        isPropertyChanged = editor.isPropertyChanged();
        removePropertyWrapper(&editor);
        // if (m_pElecBlockOld) {
        //     // 清空电气接口模块中原来的电气元件关联的子画板信息
        //     m_pElecBlockOld->associatedControlSystem.removeOne(block->getParameter(Block::boardUUID).toString());
        // }

        // if (block->m_pElecBlock) {
        //     setElecComponentSubBoardInfo(block);
        // }
    } else if (SlotBlock::Type == pModel->getModelType()) {
        // 槽模块
        PSlotBlock psblock = pModel.dynamicCast<SlotBlock>();
        if (psblock == nullptr) {
            return false;
        }
        if (psblock->isInstance() && psblock->getTargetModel() != nullptr) {
            return ShowBlockProperty(psblock->getTargetModel());
        }

        SlotProperty slotProperty(psblock, isreadOnly);
        slotProperty.init();
        showPropertyDialog(&slotProperty, NPS::DLGMEMORY_BLOCK, "", shortcutPath, modelProtoType);
        isPropertyChanged = slotProperty.isPropertyChanged();
        removePropertyWrapper(&slotProperty);
    }
    if (isPropertyChanged) {
        NotifyStruct notify;
        notify.code = Notify_BlockPropertyChanged;
        notify.paramMap[PMKEY::MODEL_UUID] = pModel->getUUID();
        notify.paramMap[PMKEY::MODEL_TYPE] = pModel->getModelType();
        emitNotify(notify);
    }

    return isPropertyChanged;
}

bool PropertyManagerServer::ShowCommentProperty(QSharedPointer<SourceProxy> pSourceProxy,
                                                QSharedPointer<CanvasContext> canvasContext)
{
    if (pSourceProxy == nullptr) {
        return false;
    }

    QString strPrototypeName = pSourceProxy->prototypeName();
    if (strPrototypeName == "TextToolBlock") {
        TextboxProperty textboxEditor(pSourceProxy);
        textboxEditor.init();
        showPropertyDialog(&textboxEditor, NPS::DLGMEMORY_BLOCK);
        removePropertyWrapper(&textboxEditor);
    } else if (strPrototypeName == RectAnnotationStr || strPrototypeName == CircleAnnotationStr
               || strPrototypeName == PolygonAnnotationStr || strPrototypeName == ArcAnnotationStr) {
        GraphCommentProperty graphEditor(pSourceProxy, GraphCommentProperty::ToolBlockType_Cycle);
        graphEditor.init();
        showPropertyDialog(&graphEditor, NPS::DLGMEMORY_BLOCK);
        removePropertyWrapper(&graphEditor);
    } else if (strPrototypeName == LineSegmentAnnotationStr || strPrototypeName == LineSegmentArrowAnnotationStr
               || strPrototypeName == BrokenLineAnnotationStr || strPrototypeName == BrokenLineArrowAnnotationStr) {
        GraphCommentProperty graphEditor(pSourceProxy, GraphCommentProperty::ToolBlockType_Lines);
        graphEditor.init();
        showPropertyDialog(&graphEditor, NPS::DLGMEMORY_BLOCK);
        removePropertyWrapper(&graphEditor);
    } else if (strPrototypeName == ImageAnnotationStr) {
        IconBlockProperty IconBlockEditor(pSourceProxy, canvasContext);
        IconBlockEditor.init();
        showPropertyDialog(&IconBlockEditor, NPS::DLGMEMORY_BLOCK);
        removePropertyWrapper(&IconBlockEditor);
    } else if (strPrototypeName == TextAnnotationStr) {
        TextboxProperty textBlockEditor(pSourceProxy);
        textBlockEditor.init();
        showPropertyDialog(&textBlockEditor, NPS::DLGMEMORY_BLOCK);
        removePropertyWrapper(&textBlockEditor);
    }

    return false;
}

// bool PropertyManagerServer::ShowBlockProperty(PBlock baseBlock)
// {
//     QtSharedPointerNullReturnVoid(baseBlock);
//     m_pCurrentDrawingBoardInfo.clear();
//     // 属性页是否只读
//     bool isNoShowDialog = true;
//     QString boardUUID = baseBlock->getParameter(Block::boardUUID).toString();
//     // if (PropertyServerMng::getInstance().m_pGraphicsModelingServer && !boardUUID.isEmpty()) {
//     //     m_pCurrentDrawingBoardInfo =
//     //             PropertyServerMng::getInstance().m_pGraphicsModelingServer->getDrawingBoardInfoById(boardUUID);
//     //     if (!m_pCurrentDrawingBoardInfo.isNull()) {
//     //         isNoShowDialog = false;
//     //     } else {
//     //         // 网络模型管理器弹出属性框时，如果没有打开画板画板服务获取不到该画板信息
//     //         // 之所以不直接从数据管理服务查找数据，因为列表遍历比map查找效率低，优先从画板服务查找
//     //         QList<PDrawingBoardClass> allBoards = PropertyServerMng::getInstance()
//     //                                                       .PropertyServerMng::getInstance()
//     //                                                       .m_pDataManagerServer->GetAllBoardModel();
//     //         foreach (PDrawingBoardClass board, allBoards) {
//     //             if (board->getUUID() == boardUUID) {
//     //                 m_pCurrentDrawingBoardInfo = board;
//     //                 isNoShowDialog = false;
//     //                 break;
//     //             }
//     //         }
//     //     }
//     // }
//     bool isDialogDisable = m_bIsDrawingBoardRunning;
//     bool isPropertyChanged = false;
//     QStringList propertyKeyChangedList;

//     Block::BlockType type = baseBlock->getBlockType();
//     switch (type) {
//     case Block::BlockElec: {
//         // 电气模块
//         PElectricalBlock block = qSharedPointerDynamicCast<ElectricalBlock>(baseBlock);
//         QtSharedPointerNullReturnVoid(block);
//         // 电气模块，如果没有关联画板，并且没有电气参数，就不弹窗,否则就是一个空窗口
//         if (isNoShowDialog && !isNeedShowElectricalProperty(block)) {
//             return false;
//         }
//         ElectricalBlockPropertyEditor editor(block, isDialogDisable);
//         editor.init();
//         showPropertyDialog(&editor, NPS::DLGMEMORY_BLOCK, isNoShowDialog);
//         isPropertyChanged = editor.isPropertyChanged();
//         propertyKeyChangedList = editor.getPropertyKeyChangedList();
//         removePropertyWrapper(&editor);

//     } break;
//     case ControlBlock::Type:
//     case Block::BlockCtrlCombined: {

//     } break;
//     case Block::BlockElectricalContainer: {
//         // 电气容器型模块
//         // PElectricalContainerBlock block = qSharedPointerDynamicCast<ElectricalContainerBlock>(baseBlock);
//         // QtSharedPointerNullReturnVoid(block);
//         // PElectricalBlock m_pElecBlockOld = block->m_pElecBlock;
//         // ElecContainerProperty editor(block, isDialogDisable);
//         // editor.init();
//         // showPropertyDialog(&editor, NPS::DLGMEMORY_BLOCK, isNoShowDialog);
//         // isPropertyChanged = editor.isPropertyChanged();
//         // removePropertyWrapper(&editor);
//         // if (m_pElecBlockOld) {
//         //     // 清空电气接口模块中原来的电气元件关联的子画板信息
//         //     QStringList associateList = m_pElecBlockOld->getAssociatedControlSystem();
//         //     associateList.removeOne(block->getParameter(Block::boardUUID).toString());
//         //     m_pElecBlockOld->setAssociatedControlSystem(associateList);
//         // }

//         // if (block->m_pElecBlock) {
//         //     setElecComponentSubBoardInfo(block);
//         // }
//     } break;
//     case Block::BlockSlot: {
//         // 插槽型模块
//         PSlotBlock block = qSharedPointerDynamicCast<SlotBlock>(baseBlock);
//         QtSharedPointerNullReturnVoid(block);
//         SlotProperty slotProperty(block, isDialogDisable);
//         slotProperty.init();
//         showPropertyDialog(&slotProperty, NPS::DLGMEMORY_BLOCK);
//         isPropertyChanged = slotProperty.isPropertyChanged();
//         removePropertyWrapper(&slotProperty);
//     } break;
//     default: {
//         QString msg = tr("unprocessed block attribute interface:%1").arg(type); // 暂未处理的block属性接口:%1
//         LOGOUT(msg, LOG_NORMAL);
//     }
//     }
//     if (isPropertyChanged) {
//         NotifyStruct notify;
//         notify.code = Notify_BlockPropertyChanged;
//         QMap<QString, QVariant> paramas;
//         // QString boardName = PropertyServerMng::getInstance().m_pDataManagerServer->GetBoradNameByUUID(
//         //         baseBlock->getParameter(Block::boardUUID).toString());
//         // paramas["boardName"] = boardName;
//         paramas["blockUUID"] = baseBlock->getParameter(Block::uuid).toString();
//         paramas["blockType"] = baseBlock->getBlockType();
//         paramas["propertyKeyChangedList"] = propertyKeyChangedList;
//         notify.paramMap = paramas;
//         emitNotify(notify);
//     }

//     return isPropertyChanged;
// }

QSharedPointer<Kcc::BlockDefinition::DeviceModel> PropertyManagerServer::CreateDeviceModel(const QString &prototypename)
{
    if (prototypename.isEmpty()) {
        return PDeviceModel(nullptr);
    }
    DeviceModelProperty deveditor(nullptr, true, prototypename);
    deveditor.init();
    showPropertyDialog(&deveditor, NPS::DLGMEMORY_BLOCK, "", CMA::USERMANUAL_NEWDEVICE);
    PDeviceModel pdevmodel = deveditor.getDeviceModel();
    removePropertyWrapper(&deveditor);
    if (pdevmodel != nullptr) {
        NotifyStruct notify;
        notify.code = Notify_CreateDeviceType;
        notify.paramMap[PMKEY::MODEL_UUID] = pdevmodel->getUUID();
        notify.paramMap[PMKEY::MODEL_TYPE] = pdevmodel->getModelType();
        emitNotify(notify);
    }
    return pdevmodel;
}

bool PropertyManagerServer::EditDeviceModel(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicemodel,
                                            bool readOnly)
{
    if (devicemodel == nullptr) {
        return false;
    }
    DeviceModelProperty deveditor(devicemodel, false, devicemodel->getDeviceTypePrototypeName(),
                                  readOnly || m_bIsDrawingBoardRunning);
    deveditor.init();
    showPropertyDialog(&deveditor, NPS::DLGMEMORY_BLOCK, "", CMA::USERMANUAL_EDITDEVICE);
    bool isPropertyChanged = deveditor.isPropertyChanged();
    removePropertyWrapper(&deveditor);
    if (devicemodel != nullptr) {
        NotifyStruct notify;
        notify.code = Notify_EditDeviceType;
        notify.paramMap[PMKEY::MODEL_UUID] = devicemodel->getUUID();
        notify.paramMap[PMKEY::MODEL_TYPE] = devicemodel->getModelType();
        emitNotify(notify);
    }
    return isPropertyChanged;
}

QSharedPointer<Kcc::BlockDefinition::Model> PropertyManagerServer::CreateComplexInstance(const QString &prototype)
{
    ComplexInstanceProperty complexEditor(PModel(nullptr), ComplexInstanceWidget::Operation_NewInstance, prototype);
    complexEditor.init();
    showPropertyDialog(&complexEditor, NPS::DLGMEMORY_SETTING, "", CMA::USERMANUAL_NEWDRAWBOARD);
    PModel model = complexEditor.getComplexModel();
    removePropertyWrapper(&complexEditor);
    if (model != nullptr) {
        NotifyStruct notify;
        notify.code = Notify_CreateComplexInstance;
        notify.paramMap[PMKEY::MODEL_UUID] = model->getUUID();
        notify.paramMap[PMKEY::MODEL_TYPE] = model->getModelType();
        emitNotify(notify);
    }
    return model;
}

bool PropertyManagerServer::EditComplexInstance(QSharedPointer<Kcc::BlockDefinition::Model> model, bool readOnly)
{
    if (model == nullptr) {
        return false;
    }
    ComplexInstanceProperty complexEditor(model, ComplexInstanceWidget::Operation_EditInstance, "",
                                          readOnly || m_bIsDrawingBoardRunning);
    complexEditor.init();
    showPropertyDialog(&complexEditor, NPS::DLGMEMORY_SETTING, "", CMA::USERMANUAL_EDITDRAWBOARD);
    bool isPropertyChanged = complexEditor.isPropertyChanged();
    removePropertyWrapper(&complexEditor);
    return isPropertyChanged;
}

QSharedPointer<Kcc::BlockDefinition::Model> PropertyManagerServer::CreateBlockInstance(const QString &prototype)
{
    PModel model(nullptr);
    if (!prototype.isEmpty()) {
        model = CMA::getModelByPrototype(prototype, CMA::ProtoRange_OnlyProject);
    }
    PModel instanceModel(nullptr);
    if (model != nullptr) {
        instanceModel = model->createInstance();
        QString tempname = QObject::tr("%1(1)").arg(model->getName());
        QString errorinfo;
        int j = 2;
        while (!CMA::checkNameValid(tempname, instanceModel, errorinfo)) {
            tempname = QObject::tr("%1(%2)").arg(model->getName()).arg(QString::number(j++));
        }
        instanceModel->setName(tempname);
    }
    bool canChangeType = true;
    if (instanceModel != nullptr) {
        canChangeType = false;
    }
    ControlBlockPropertyEditor ctredit(instanceModel, CMA::Operation_NewCtrInstance, false, canChangeType);
    ctredit.init();
    showPropertyDialog(&ctredit, NPS::DLGMEMORY_BLOCK, "", CMA::USERMANUAL_NEWDRAWBOARD);
    bool isPropertyChanged = ctredit.isPropertyChanged();
    if (ctredit.isPropertyChanged()) {
        instanceModel = ctredit.getCurrentModel();
    } else {
        instanceModel = PModel(nullptr);
    }
    removePropertyWrapper(&ctredit);
    if (instanceModel != nullptr) {
        NotifyStruct notify;
        notify.code = Notify_CreateBlockInstance;
        notify.paramMap[PMKEY::MODEL_UUID] = instanceModel->getUUID();
        notify.paramMap[PMKEY::MODEL_TYPE] = instanceModel->getModelType();
        emitNotify(notify);
    }
    return instanceModel;
}

QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass>
PropertyManagerServer::CreateDrawBoard(const QString &initBoardType, bool popDlg)
{
    if (popDlg) {
        DrawBoardProperty editor(PDrawingBoardClass(nullptr), true, initBoardType);
        editor.init();
        showPropertyDialog(&editor, NPS::DLGMEMORY_SETTING, "", CMA::USERMANUAL_NEWDRAWBOARD);
        PDrawingBoardClass pdrawboard = editor.getNewDrawingBoard();
        removePropertyWrapper(&editor);
        if (pdrawboard != nullptr) {
            NotifyStruct notify;
            notify.code = Notify_DrawingBoardCreate;
            notify.paramMap[PMKEY::MODEL_NAME] = pdrawboard->getName();
            notify.paramMap[PMKEY::MODEL_UUID] = pdrawboard->getUUID();
            notify.paramMap[PMKEY::MODEL_TYPE] = pdrawboard->getModelType();
            emitNotify(notify);
        }
        return pdrawboard;
    } else {
        if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
            return PDrawingBoardClass(nullptr);
        }

        CreateDrawingboardWidget *pCreateBoardWidget = new CreateDrawingboardWidget(nullptr, true, initBoardType);
        PDrawingBoardClass pDrawingBoardClass =
                PropertyServerMng::getInstance()
                        .m_projectManagerServer
                        ->CreateBoardModel(pCreateBoardWidget->getBoardName(), pCreateBoardWidget->getBoardType())
                        .dynamicCast<DrawingBoardClass>();
        if (pDrawingBoardClass.isNull()) {
            return PDrawingBoardClass(nullptr);
        }
        pDrawingBoardClass->createCanvasContext();

        if (!pCreateBoardWidget->saveDrawingBoardData(pDrawingBoardClass->getCanvasContext())) {
            return PDrawingBoardClass(nullptr);
        }
        // 创建画板后，保存一次画板数据
        PropertyServerMng::getInstance().m_projectManagerServer->SaveBoardModel(pDrawingBoardClass);
        return pDrawingBoardClass;
    }
}

bool PropertyManagerServer::ShowDrawBoardProperty(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pDrawboard,
                                                  const QString tabname, bool readOnly)
{
    if (pDrawboard == nullptr) {
        return false;
    }
    QString oldName = pDrawboard->getName();
    DrawBoardProperty editor(pDrawboard, false, "", readOnly || m_bIsDrawingBoardRunning);
    editor.init();
    showPropertyDialog(&editor, NPS::DLGMEMORY_SETTING, tabname, CMA::USERMANUAL_EDITDRAWBOARD);
    bool bchanged = editor.isPropertyChanged();
    PDrawingBoardClass pdrawboard = editor.getNewDrawingBoard();
    removePropertyWrapper(&editor);
    if (bchanged && pdrawboard != nullptr) {
        NotifyStruct notify;
        notify.code = Notify_DrawingBoardModified;
        notify.paramMap[PMKEY::MODEL_OLD_NAME] = oldName;
        notify.paramMap[PMKEY::BOARD_TRIGGER_MODIFY] = editor.getDataStatus();
        notify.paramMap[PMKEY::MODEL_NAME] = pdrawboard->getName();
        notify.paramMap[PMKEY::MODEL_UUID] = pdrawboard->getUUID();
        notify.paramMap[PMKEY::MODEL_TYPE] = pdrawboard->getModelType();
        emitNotify(notify);
    }
    return bchanged;
}

void PropertyManagerServer::ShowLayerProperty(QSharedPointer<ICanvasScene> pCanvasScene, const QString tabname)
{
    if (pCanvasScene == nullptr)
        return;
    LayerProperty editor(pCanvasScene);
    editor.init();
    showPropertyDialog(&editor, NPS::DLGMEMORY_SETTING, tabname);
    removePropertyWrapper(&editor);
}

void PropertyManagerServer::ShowSimuParamProperty(Kcc::SimuParams &params,
                                                  Kcc::SimulationManager::RunStepConfigParams &stepParams,
                                                  int simuState, int NpsOrCad)
{
    SimuParamsProperty simuParamProperty(params, stepParams, simuState, NpsOrCad);
    showPropertyDialog(&simuParamProperty, NPS::DLGMEMORY_SETTING, "", CMA::USERMANUAL_SETTINGS);
    removePropertyWrapper(&simuParamProperty);
}

void PropertyManagerServer::ShowProjectProperty()
{
    int NpsOrCad = 0;
    ProjectSetProperty prjsetProperty(NpsOrCad);
    showPropertyDialog(&prjsetProperty, NPS::DLGMEMORY_SETTING, "", CMA::USERMANUAL_SETTINGS);
    removePropertyWrapper(&prjsetProperty);
}

void PropertyManagerServer::Clear()
{
    QMapIterator<CommonWrapper *, PropertyWrapperData> iter(m_mapPropertyData);
    while (iter.hasNext()) {
        iter.next();

        delete iter.value().dialog;
        delete iter.value().customdlg;
    }

    m_mapPropertyData.clear();
    m_pCurrentDrawingBoardInfo.clear();
}

bool PropertyManagerServer::checkNameValid(const QString &newname, QSharedPointer<Kcc::BlockDefinition::Model> model,
                                           QString &errorinfo)
{
    return CMA::checkNameValid(newname, model, errorinfo);
}

QSharedPointer<Kcc::BlockDefinition::Model>
PropertyManagerServer::selectBlock(const QString &titleStr, const int &selectType,
                                   QSharedPointer<Kcc::BlockDefinition::Variable> filterVar)
{
    ControlBlockTypeSelectView *BlockSelectWidget =
            new ControlBlockTypeSelectView(ControlBlockTypeSelectView::SelectType(selectType), filterVar);
    KCustomDialog *selectBlockDLG =
            new KCustomDialog(titleStr, BlockSelectWidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::Ok);
    connect(BlockSelectWidget, &ControlBlockTypeSelectView::typeSelect, this,
            [=](QSharedPointer<Kcc::BlockDefinition::Model> model) {
                if (model != nullptr) {
                    selectBlockDLG->done(KBaseDlgBox::Ok);
                }
            });
    selectBlockDLG->resize(BlockSelectWidget->width(), BlockSelectWidget->height());
    PModel model = nullptr;
    if (KBaseDlgBox::Ok == selectBlockDLG->exec()) {
        model = BlockSelectWidget->getSelectedModel();
    }
    if (BlockSelectWidget != nullptr) {
        delete BlockSelectWidget;
        BlockSelectWidget = nullptr;
    }
    if (selectBlockDLG != nullptr) {
        delete selectBlockDLG;
        selectBlockDLG = nullptr;
    }
    return model;
}

void PropertyManagerServer::refreshBlockRule(QMap<QString, QVariant> params)
{
    NotifyStruct notify;
    notify.code = Notify_BlockRuleModify;
    notify.paramMap = params;
    emitNotify(notify);
}

void PropertyManagerServer::notifySceneUpdateState(QString boardName)
{
    NotifyStruct notify;
    notify.code = Notify_SceneUpdateState;
    notify.paramMap["boardName"] = boardName;
    emitNotify(notify);
}

void PropertyManagerServer::createPropertyWrapperData(CommonWrapper *wrapper, const QString &dlgmemorytype,
                                                      const QString &tabname, const QString &shortcutPath,
                                                      const QString &blockProto)
{
    if (wrapper == nullptr) {
        LOGOUT(tr("the CommonWrapper pointer passed in is null"), LOG_ERROR); // 传入的CommonWrapper指针为空
        return;
    }
    if (PropertyServerMng::getInstance().m_pUIServer == nullptr
        || PropertyServerMng::getInstance().m_pUIServer->GetMainUI() == nullptr) {
        return;
    }

    if (m_mapPropertyData.contains(wrapper)) {
        m_mapPropertyData[wrapper].dialog->setPropertyWrapper(wrapper, tabname);
        m_mapPropertyData[wrapper].customdlg->exec();
        return;
    }

    PropertyWrapperData data;
    data.wrapper = wrapper;
    data.dialog = new PropertyDialog();
    data.dialog->setPropertyWrapper(wrapper, tabname);
    if (dlgmemorytype == NPS::DLGMEMORY_BLOCK) {
        data.dialog->setProperty("isDlgmemoryBlock", "true");
    } else {
        data.dialog->setProperty("isDlgmemoryBlock", "false");
    }
    data.customdlg =
            new KCustomDialog(wrapper->getTitle(), data.dialog, KBaseDlgBox::NoButton, KBaseDlgBox::NoButton,
                              dynamic_cast<QMainWindow *>(PropertyServerMng::getInstance().m_pUIServer->GetMainUI()));
    QPushButton *pbtnOK = data.customdlg->addCustomButton(tr("Ok"));     // 确定
    QPushButton *cancel = data.customdlg->addCustomButton(tr("Cancel")); // 取消
    QString toolkitName;
    if ((wrapper->getLeftWidgetType() == CommonWrapper::LeftWidgetType_BlockType
         || wrapper->getLeftWidgetType() == CommonWrapper::LeftWidgetType_BlockInstance)
        && (PropertyServerMng::getInstance().m_ModelManagerServer != nullptr
            && PropertyServerMng::getInstance().m_ModelManagerServer->ContainToolkitModel(blockProto, toolkitName))) {
        QPushButton *helpBtn = data.customdlg->addCustomButton(tr("Help")); // 帮助
        data.customHelpBtn = helpBtn;
        QObject::connect(helpBtn, SIGNAL(released()), this, SLOT(onDialogHelpBtnClicked()));
    }
    data.customdlg->setDefaultButton(pbtnOK);
    QSize size = data.dialog->size();
    data.customdlg->resize(size.width(), size.height());
    data.customdlg->memoryGeometry(dlgmemorytype);
    data.customdlg->setContainerBorderVisible(false);
    data.customOkBtn = pbtnOK;
    QObject::connect(pbtnOK, SIGNAL(released()), this, SLOT(onDialogOkBtnClicked()));
    QObject::connect(cancel, SIGNAL(released()), wrapper, SIGNAL(closeWrapper()));
    QObject::connect(wrapper, SIGNAL(closeWrapper()), this, SLOT(onCloseWrapper()));
    m_mapPropertyData[wrapper] = data;
    data.customdlg->setObjectName(shortcutPath);
    if (0 != data.customdlg->exec()) {
        wrapper->onDialogExecuteResult(QDialog::Rejected);
    }
}

void PropertyManagerServer::removePropertyWrapper(CommonWrapper *wrapper)
{
    if (m_mapPropertyData.contains(wrapper)) {
        PropertyWrapperData &data = m_mapPropertyData[wrapper];
        data.dialog->clear();
        if (data.customdlg != nullptr) {
            delete data.customdlg;
            data.customdlg = nullptr;
        }
        m_mapPropertyData.remove(wrapper);
    }
}

void PropertyManagerServer::showPropertyDialog(CommonWrapper *wrapper, const QString &dlgmemorytype,
                                               const QString &tabname, const QString &shortcutPath,
                                               const QString &blockProto)
{
    if (wrapper == nullptr) {
        LOGOUT(tr("the CommonWrapper pointer passed in is null"), LOG_ERROR); // 传入的CommonWrapper指针为空
        return;
    }
    m_bIsDialogPopup = true;

    createPropertyWrapperData(wrapper, dlgmemorytype, tabname, shortcutPath, blockProto);
}

QFont PropertyManagerServer::ShowFontSetDialog(const QFont font)
{
    FontWidget *fontwidget = new FontWidget(font);
    KCustomDialog fontDlg(tr("Font Settings"), fontwidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
                          KBaseDlgBox::Ok); // 字体设置
    fontDlg.resize(fontwidget->width() + 30, fontwidget->height() + 60);
    fontDlg.setEnableDoubleClicked2FullScreen(false);
    if (KBaseDlgBox::Ok == fontDlg.exec()) {
        if (fontwidget != nullptr) {
            QFont tmpfont = fontwidget->CurrentFont();
            delete fontwidget;
            fontwidget = nullptr;
            return tmpfont;
        }
    }
    if (fontwidget != nullptr) {
        delete fontwidget;
        fontwidget = nullptr;
    }
    return font;
}

void PropertyManagerServer::sendNotify(const unsigned int code, const QMap<QString, QVariant> &parammap)
{
    NotifyStruct notify;
    notify.code = code;
    notify.paramMap = parammap;
    emitNotify(notify);
}

void PropertyManagerServer::openWarningDlg(const QString &tips)
{
    if (!tips.isEmpty()) {
        KMessageBox::warning(tips, KMessageBox::Ok, KMessageBox::Ok);
    }
}

bool PropertyManagerServer::isNeedShowElectricalProperty(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block)
{
    bool isShow = false;
    // QList<ElectricalBlock::ElectricalParameter> paramList;
    // QMap<int, ElectricalBlock::ElectricalParameter> paramMap;
    // // 排序
    // QMapIterator<QString, ElectricalBlock::ElectricalParameter> iter(block->electricalParameterMap);
    // while (iter.hasNext()) {
    //     iter.next();
    //     paramMap.insert(iter.value().order, iter.value());
    // }
    // int paramMapsize = paramMap.size();
    // for (int i = 1; i <= paramMapsize && paramMap.contains(i); ++i) {
    //     paramList.append(paramMap[i]);
    // }

    // // 取值
    // int listsize = paramList.size();
    // for (int j = 0; j < listsize; ++j) {
    //     if (paramList[j].mode == 0
    //         || (paramList[j].keyWords == "NAME") && (block->prototypeName != "Busbar")
    //                 && (block->prototypeName != "DotBusbar")) {
    //         // 不可见
    //         continue;
    //     }
    //     // 带有画板信息的时候，多一个过滤条件，把端口信息过滤
    //     if (!block->getParameter(Block::boardUUID).toString().isEmpty()) {
    //         bool isport = false;
    //         QList<int> portnamelist = block->blockPorts.getPortNames();
    //         foreach (int name, portnamelist) {
    //             BlockPorts::PortInfo portInfo = block->blockPorts.getPortInfo(name);
    //             if (portInfo.viewName == paramList[j].keyWords) {
    //                 isport = true;
    //                 break;
    //             }
    //         }
    //         if (isport) {
    //             continue;
    //         }
    //     }
    //     isShow = true;
    //     break;
    // }
    return isShow;
}

void PropertyManagerServer::onDialogHelpBtnClicked()
{
    QPushButton *btn = dynamic_cast<QPushButton *>(sender());
    QMapIterator<CommonWrapper *, PropertyWrapperData> iter(m_mapPropertyData);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value().customHelpBtn == btn) {
            DocHelper::openObjectDocAssistant(iter.key()->getHelpUrl());
        }
    }
}

void PropertyManagerServer::onDialogOkBtnClicked()
{
    QPushButton *btn = dynamic_cast<QPushButton *>(sender());
    QMapIterator<CommonWrapper *, PropertyWrapperData> iter(m_mapPropertyData);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value().customOkBtn == btn) {
            QString errorinfo = "";
            if (iter.key()->checkValue(errorinfo)) {
                iter.key()->onDialogExecuteResult(QDialog::Accepted);
                iter.value().customdlg->hide();
            } else {
                openWarningDlg(errorinfo);
            }
        }
    }
}

void PropertyManagerServer::onCloseWrapper()
{
    CommonWrapper *wrapper = dynamic_cast<CommonWrapper *>(sender());
    if (wrapper != nullptr && m_mapPropertyData.contains(wrapper) && m_mapPropertyData[wrapper].customdlg != nullptr) {
        wrapper->onDialogExecuteResult(QDialog::Rejected);
        m_mapPropertyData[wrapper].customdlg->hide();
    }
}

void PropertyManagerServer::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    if (code == Notify_DrawingBoardRunning) {
        m_bIsDrawingBoardRunning = true;
    } else if (code == Notify_DrawingBoardStopped) {
        m_bIsDrawingBoardRunning = false;
    }
}

QSet<QString>
PropertyManagerServer::getOtherCtrlBlockVarNames(QSharedPointer<Kcc::BlockDefinition::ControlBlock> curBlock)
{
    QSet<QString> sets;
    // if (m_pCurrentDrawingBoardInfo) {
    //     int boardType = m_pCurrentDrawingBoardInfo->getModelType();
    //     if (boardType == ControlBoardModel::Type || boardType == CombineBoardModel::Type) {
    //         auto iter = m_pCurrentDrawingBoardInfo->blockMap.begin();
    //         for (; iter != m_pCurrentDrawingBoardInfo->blockMap.end(); iter++) {
    //             PControlBlock pCtrlBlock = iter.value().dynamicCast<ControlBlock>();
    //             if (pCtrlBlock && pCtrlBlock != curBlock) {
    //                 auto varKeys = pCtrlBlock->getVarKeys();
    //                 for (auto keyiter = varKeys.begin(); keyiter != varKeys.end(); keyiter++) {
    //                     sets.insert(*keyiter);
    //                 }
    //             }
    //         }
    //     }
    // }

    return sets;
}

void PropertyManagerServer::setElecComponentSubBoardInfo(
        QSharedPointer<Kcc::BlockDefinition::ElectricalContainerBlock> elecInterfaceBlock)
{
    // if (m_pCurrentDrawingBoardInfo) {
    //     QList<PDrawingBoardClass> drawingBoardList;

    //     // 重新设置电气接口模块中电气元件关联的子画板
    //     drawingBoardList =
    //     PropertyServerMng::getInstance().m_pDataManagerServer->GetAllDrawingBoards(); foreach
    //     (PDrawingBoardClass drawBorde, drawingBoardList) {
    //         QString strDrawingBoardUUID = drawBorde->getUUID();
    //         if (strDrawingBoardUUID == m_pCurrentDrawingBoardInfo->getUUID()) {
    //             // 电气元件关联控制系统画板
    //             elecInterfaceBlock->m_pElecBlock->drawingBoard = drawBorde; //
    //             电气元件关联控制系统画板,临时 QString strDrawBordeID = drawBorde->getUUID();
    //             QStringList associateList =
    //             elecInterfaceBlock->m_pElecBlock->getAssociatedControlSystem(); if
    //             (!associateList.contains(strDrawBordeID)) {
    //                 associateList.append(strDrawBordeID); // 电气元件关联控制系统画板
    //                 elecInterfaceBlock->m_pElecBlock->setAssociatedControlSystem(associateList);
    //             }
    //             // 每次关联新的控制系统画板，都触发电气元件所在画板的修改
    //             m_pGrapicModelingServer->setDrawBoardModified(
    //                     elecInterfaceBlock->m_pElecBlock->getParameter(Block::boardUUID).toString());
    //         }
    //     }
    // }
}

#include "CanvasWidget.h"
#include "CoreLib/ServerManager.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "GraphicsModelingTool.h"
#include "KLProject/klproject_global.h"
#include "Manager/ModelingServerMng.h"
#include "ModelManagerServer/ModelManagerConst.h"
#include "ModelWidget.h"
#include "SearchBox.h"
#include "SimulationProcessServer/SimuVarTable.h"

REG_WORKAREA(CanvasWidget)

USE_LOGOUT_("CanvasWidget")

static const int LINEEDIT_WIDTH = 240;            // 搜索框宽度
static const int LINEEDIT_HEIGHT = 21;            // 搜索框高度
static const int LINEEDIT_X_POSITION_OFFSET = 10; // 搜索框横向偏移
static const int LINEEDIT_Y_POSITION_OFFSET = 6;  // 搜索框纵向偏移

CanvasWidget::CanvasWidget(QWidget *parent)
    : BasicWorkareaContentWidget(),
      m_stackedWidget(nullptr),
      m_topModelWidget(nullptr),
      m_curModelWidget(nullptr),
      m_navWidget(nullptr),
      m_serachBox(nullptr),
      m_isRunning(false)
{
    // 初始化UI
    initUI();

    // 初始化信号槽
    initSignalSlot();
}

CanvasWidget::~CanvasWidget()
{
    // 关闭画板后解除锁定状态
    for (int index = 0; index < m_stackedWidget->count(); index++) {
        auto modelWidget = dynamic_cast<ModelWidget *>(m_stackedWidget->widget(index));
        if (modelWidget) {
            auto canvasView = modelWidget->getCanvasView();
            if (canvasView) {
                canvasView->setNextConstructiveBoardStatus(false);
                canvasView->cleanCache();
            }
        }
    }

    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer && m_boardModel) {
        ModelingServerMng::getInstance().m_pGraphicsModelingServer->drawingBoardClosed(m_boardModel->getUUID());

        // 关闭画板后若无打开的画板 相关处理
        QStringList nemeList = ModelingServerMng::getInstance().m_pGraphicsModelingServer->getAllDrawingBoardNames();
        if (nemeList.isEmpty()) {
            // 设置画布未激活
            m_topModelWidget->getCanvasContext()->setStatusFlag(CanvasContext::kActiveWindow, false);
            // 清除当前激活的uuid
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->drawingBoardNotActived(m_boardModel->getUUID());
        }
    }
}

// 保存
void CanvasWidget::Save()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer
        || !ModelingServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    m_topModelWidget->getCanvasView()->transformItemGroupClear();
    m_topModelWidget->getKernelInterface()->saveCanvas();

    // 判断是否是自定义构造型画板
    if (m_boardModel->getModelType() == CombineBoardModel::Type) {
        // 根据输入输出模块，创建构造型模块的端口信息
        GraphicsModelingTool::refreshUserDefinedBlockPorts(m_boardModel);
    }

    // add by liwenyu 2023.07.04 保存当前画板的缩略图数据
    auto view = m_curModelWidget->getCanvasView();
    PModel currentModel = m_curModelWidget->getModel();
    if (view && currentModel) {
        auto sharedPNGBytes = view->saveAsPngData();
        if (sharedPNGBytes && sharedPNGBytes->size() > 0) {
            QByteArray *pngDatas = sharedPNGBytes.get();
            currentModel->setResource(Kcc::BlockDefinition::CANVAS_THUNB_PNG_IMAGE, *pngDatas);
            PModel parentModel = currentModel->getParentModel();
            if (parentModel) {
                auto parentCtx = parentModel->getCanvasContext();
                if (parentCtx) {
                    auto sourceProxy = parentCtx->getSource(currentModel->getUUID());
                    if (sourceProxy) {
                        sourceProxy->getSourceProperty().setBackgroundImage(*pngDatas, false);
                    }
                }
            }
        }
    }
    // end

    QString toolkitName;
    if (ModelingServerMng::getInstance().m_pModelManagerServer->ContainToolkitModel(m_boardModel->getName(),
                                                                                    toolkitName)) {
        ModelingServerMng::getInstance().m_pModelManagerServer->SaveToolkitModel(toolkitName,
                                                                                 m_boardModel); // 保存到工具箱
    } else {
        ModelingServerMng::getInstance().m_pProjectMngServer->SaveBoardModel(m_boardModel); // 项目内
    }

    removeModifiedLabel();
}

void CanvasWidget::UnSave()
{
    // 保存画板的实时数据
    if (ModelingServerMng::getInstance().m_pProjectMngServer) {
        if (m_pProjectMngServer->ReloadBoardModel(m_boardModel)) {
            m_topModelWidget->getKernelInterface()->readRTCanvasContext(m_boardModel);
            m_pProjectMngServer->SaveBoardModel(m_boardModel, false);
            // 还原数据字典
            updataDataDictionary();
        }
    }
}

QIcon CanvasWidget::icon()
{
    QIcon pageIcon;
    if (m_boardModel) {
        int boardType = m_boardModel->getModelType();
        switch (boardType) {
        case ElecBoardModel::Type:
            pageIcon = QIcon(":/drawingboard/electricalBoard-clicked");
            break;
        case ControlBoardModel::Type:
            pageIcon = QIcon(":/drawingboard/controlBoard-clicked");
            break;
        case CombineBoardModel::Type:
            pageIcon = QIcon(":/drawingboard/controlBlockBoard-clicked");
            break;
        case ComplexBoardModel::Type:
            pageIcon = QIcon(":/drawingboard/complexModelBoard-clicked");
            break;
        case ElecCombineBoardModel::Type:
            pageIcon = QIcon(":/drawingboard/electricalBlockBoard-clicked");
            break;
        default:
            break;
        }
    } else {
        LOGOUT(tr("The loaded drawing board failed to find relevant icon information"),
               LOG_WARNING); // 加载的画板未能查询到相关图标信息
    }
    if (pageIcon.isNull()) {
        qDebug() << "icon为空";
    }
    return pageIcon;
}

bool CanvasWidget::init(const QMap<QString, QVariant> &paramMap)
{
    BasicWorkareaContentWidget::init(paramMap);

    // 引用一份数据管理服务智能指针，确保在析构函数中能够获取数据管理服务,用于软件关闭时,调用数据管理器保存实时数据
    m_pProjectMngServer = ModelingServerMng::getInstance().m_pProjectMngServer;

    m_drawingBoardName = paramMap[WORKAREA_TITLE].toString();

    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return false;
    }
    auto boardModel =
            ModelingServerMng::getInstance().m_pProjectMngServer->GetBoardModelByName(m_drawingBoardName); // 项目内
    if (!boardModel) {
        boardModel =
                ModelingServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(m_drawingBoardName); // 工具箱
    }
    if (boardModel) {
        openTopBoardModel(boardModel);
        if (m_serachBox) {
            m_serachBox->setModel(boardModel);
            connect(m_serachBox, SIGNAL(selectedBlock(searchListData &)), this,
                    SLOT(onSelectedBlock(searchListData &)));
        }
        return true;
    }
    return false;
}

void CanvasWidget::openTopBoardModel(PModel pBoardModel)
{
    if (!pBoardModel) {
        LOGOUT(tr("The drawing board object cannot be empty,%1").arg(__FUNCTION__), LOG_ERROR); // 画板对象不能为空,%1
        return;
    }
    m_boardModel = pBoardModel;

    openCurrentModel(pBoardModel);

    m_topModelWidget = m_curModelWidget;
    if (m_navWidget) {
        m_navWidget->setTopModel(pBoardModel);
    }
    if (ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        ModelingServerMng::getInstance().m_pGraphicsModelingServer->drawingBoardLoaded(this); // 加载画板完成
    }

    verifyStatus();
}

void CanvasWidget::openCurrentModel(PModel curModel)
{
    if (m_stackedWidget == nullptr || curModel == nullptr) {
        return;
    }
    // 已存在，则打开
    for (int index = 0; index < m_stackedWidget->count(); index++) {
        auto modelWidget = dynamic_cast<ModelWidget *>(m_stackedWidget->widget(index));
        if (modelWidget && modelWidget->getModel() == curModel) {
            m_curModelWidget = modelWidget;
            m_stackedWidget->setCurrentIndex(index);
            if (curModel->getModelType() != PModel::Type::Ele_CombineBoard_Type
                || (curModel->getModelType() == PModel::Type::Ele_CombineBoard_Type && index == 0)
                || curModel->getModelType() != PModel::Type::Combine_Board_Type
                || (curModel->getModelType() == PModel::Type::Combine_Board_Type && index == 0)) {
                setNextConstructiveBoard(false); // 顶层构造型画板/下一层非构造型画板，解开画板冻结状态
            }
            // modelWidget->refreshUI();

            // 通过日志打开的构造性画板会被锁定
            if (!curModel->getPrototypeName().startsWith(SUBSYSTEM_PROTO_PREFIX)) { // 非子系统
                if (curModel->getModelType() == PModel::Type::Ele_CombineBoard_Type
                    || curModel->getModelType() == PModel::Type::Combine_Board_Type) { // 是电气/控制构造画板
                    if (curModel->getTopParentModel() != curModel)                     // 非顶层画板
                        setNextConstructiveBoard(true);
                }
            }
            return;
        }
    }

    // 新建ModelWidget
    ModelWidget *curModelWidget = new ModelWidget(m_stackedWidget);
    if (curModelWidget == nullptr) {
        return;
    }
    curModelWidget->initModelWidget(curModel, this);
    curModelWidget->getCanvasView()->setRunningStatus(m_isRunning);
    if (m_isRunning
        && ModelingServerMng::getInstance().m_pGraphicsModelingServer) { // 如果在单步调试中，那么设置单步仿真参数
        curModelWidget->setRunStepVariablesData(
                ModelingServerMng::getInstance().m_pGraphicsModelingServer->getRunVariable(curModel->getUUID()));
    }
    m_curModelWidget = curModelWidget;
    m_stackedWidget->addWidget(curModelWidget);
    m_stackedWidget->setCurrentWidget(m_curModelWidget);
    // 通过日志打开的构造性画板会被锁定
    if (m_curModelWidget->getModel()) {
        if (!m_curModelWidget->getModel()->getPrototypeName().startsWith(SUBSYSTEM_PROTO_PREFIX)) {
            if (m_curModelWidget->getModel()->getModelType() == PModel::Type::Ele_CombineBoard_Type
                || m_curModelWidget->getModel()->getModelType() == PModel::Type::Combine_Board_Type) {
                if (m_curModelWidget->getModel()->getTopParentModel() != m_curModelWidget->getModel())
                    setNextConstructiveBoard(true);
            }
        }
    }
}

PModel CanvasWidget::addBlockToView(QString prototypeName, QPointF centerPoint, bool needUndo, QString defaultName,
                                    QString modelUUID)
{
    return m_curModelWidget->addBlockToView(prototypeName, centerPoint, needUndo, defaultName, modelUUID);
}

void CanvasWidget::addBlockToView(PModel pModel, bool needUndo)
{
    m_curModelWidget->addBlockToView(pModel, needUndo);
}

void CanvasWidget::deleteBlockFromView(PModel pModel)
{
    m_curModelWidget->deleteBlockFromView(pModel);
}

void CanvasWidget::addConnectorToView(PBlockConnector pConnector)
{
    m_curModelWidget->addConnectorToView(pConnector);
}

void CanvasWidget::deleteConnector(QString connectorUUID)
{
    m_curModelWidget->deleteConnector(connectorUUID);
}

void CanvasWidget::scrollToBlock(const QString &strActiveBlockID)
{
    if (!m_curModelWidget || !m_curModelWidget->getCanvasView())
        return;

    m_curModelWidget->getCanvasView()->scrollToBlock(strActiveBlockID);
}

void CanvasWidget::triggerContensChangedSlot(bool isTouchModified)
{
    if (isTouchModified) {
        onContensChanged();
    }
}

void CanvasWidget::boardNameChanged()
{
    if (!m_boardModel) {
        return;
    }
    auto boardName = m_boardModel->getName();
    // 该操作会触发画板修改,静止该信号
    this->blockSignals(true);
    this->setWorkAreaTitle(boardName); // 修改工作区标题
    this->blockSignals(false);
    m_navWidget->changeFirstItemName(boardName);
}

void CanvasWidget::setRunningStatus(bool bRunning)
{
    m_isRunning = bRunning;
    for (int index = 0; index < m_stackedWidget->count(); index++) {
        auto modelWidget = dynamic_cast<ModelWidget *>(m_stackedWidget->widget(index));
        if (modelWidget) {
            modelWidget->getCanvasView()->setRunningStatus(bRunning);
        }
    }
}

void CanvasWidget::initUI()
{
    m_stackedWidget = new QStackedWidget(this);
    connect(m_stackedWidget, &QStackedWidget::currentChanged, this, &CanvasWidget::onStackedWidgetChanged);

    // 导航栏
    m_navWidget = new NavWidget(this);

    m_serachBox = new SearchBox(this);
    m_serachBox->move(300, 300);

    m_serachBox->show();
    // 页面布局
    QVBoxLayout *vLoyout = new QVBoxLayout(this);
    vLoyout->setContentsMargins(0, 0, 0, 0);
    vLoyout->setSpacing(0);
    vLoyout->addWidget(m_stackedWidget);

    this->setLayout(vLoyout);
}

void CanvasWidget::initSignalSlot()
{
    connect(&BookMark::getInstance(), SIGNAL(previewUUIDChanged(QString)), this,
            SLOT(onBookMarkArtboardSwitching(QString)));
    connect(this, SIGNAL(fullScreenModeSignal(bool)), this, SLOT(onFullscreenChanged(bool)));
    connect(ActionManager::getInstance().getAction(ActionManager::LayoutManager), SIGNAL(triggered()), this,
            SLOT(onModifyLayerProperty()));
    connect(ActionManager::getInstance().getAction(ActionManager::LayoutManagerNoIcon), SIGNAL(triggered()), this,
            SLOT(onModifyLayerProperty()));
}

void CanvasWidget::updataDataDictionary()
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer || !m_boardModel) {
        return;
    }

    ModelingServerMng::getInstance().m_pProjectMngServer->UpdateBoardDataDictionary(m_boardModel);
}

void CanvasWidget::verifyStatus()
{
    // 若为电气画板，则校验规则
    if (m_boardModel->getModelType() == ElecBoardModel::Type
        || m_boardModel->getModelType() == ElecCombineBoardModel::Type) {
        ModelingServerMng::getInstance().m_pDrawingBoardHelperServer->VerifyDrawingBoardStatus(
                m_boardModel.dynamicCast<DrawingBoardClass>());
    }
}

QRectF CanvasWidget::getMinRect()
{
    if (m_curModelWidget && m_curModelWidget->getCanvasView()) {
        return m_curModelWidget->getCanvasView()->getMinRect();
    }
    return QRectF();
}

ModelWidget *CanvasWidget::getCurModelWidget()
{
    return m_curModelWidget;
}

ModelWidget *CanvasWidget::getModelWidgetByUUid(const QString &uuid)
{
    for (int index = 0; index < m_stackedWidget->count(); index++) {
        auto modelWidget = dynamic_cast<ModelWidget *>(m_stackedWidget->widget(index));
        if (modelWidget && modelWidget->getModel()->getUUID() == uuid) {
            return modelWidget;
        }
    }
    return nullptr;
}

ModelWidget *CanvasWidget::getTopModelWidget()
{
    return m_topModelWidget;
}

void CanvasWidget::setNextConstructiveBoard(bool falg)
{
    for (int index = 0; index < m_stackedWidget->count(); index++) {
        auto modelWidget = dynamic_cast<ModelWidget *>(m_stackedWidget->widget(index));
        if (modelWidget) {
            modelWidget->getCanvasView()->setNextConstructiveBoardStatus(falg);
        }
    }
}

void CanvasWidget::showEvent(QShowEvent *event)
{
    if (m_boardModel && ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        ModelingServerMng::getInstance().m_pGraphicsModelingServer->drawingBoardActived(m_boardModel->getUUID());
    }
}

void CanvasWidget::hideEvent(QHideEvent *event)
{
    if (m_boardModel && ModelingServerMng::getInstance().m_pGraphicsModelingServer) {
        ModelingServerMng::getInstance().m_pGraphicsModelingServer->drawingBoardNotActived(m_boardModel->getUUID());
    }
}

void CanvasWidget::resizeEvent(QResizeEvent *event)
{
    if (m_serachBox) {
        m_serachBox->setFixedWidth(LINEEDIT_WIDTH);
        m_serachBox->setFixedHeight(LINEEDIT_HEIGHT);
        m_serachBox->move(this->rect().topRight()
                          - QPoint((m_serachBox->width() + LINEEDIT_X_POSITION_OFFSET), -(LINEEDIT_Y_POSITION_OFFSET)));
    }
}

void CanvasWidget::onFullscreenChanged(bool bfull)
{
    if (m_curModelWidget && m_curModelWidget->getCanvasView()) {
        m_curModelWidget->getCanvasView()->setFullscreenMode(bfull);
    }
}

void CanvasWidget::onModifyLayerProperty()
{
    if (m_curModelWidget && m_curModelWidget->getCanvasView()
        && ModelingServerMng::getInstance().m_pPropertyManagerServer
        && ActionManager::getInstance().getActivateCanvasUUID() == m_curModelWidget->getCanvasContext()->uuid()) {
        ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowLayerProperty(
                m_curModelWidget->getCanvasView()->getCanvasScene());
    }
}

void CanvasWidget::onBookMarkArtboardSwitching(QString uuid)
{
    if (!ModelingServerMng::getInstance().m_pProjectMngServer) {
        return;
    }
    auto boardList = ModelingServerMng::getInstance().m_pProjectMngServer->GetAllBoardModel();
    foreach (auto board, boardList) {
        if (board->getUUID() == uuid) {
            ModelingServerMng::getInstance().m_pGraphicsModelingServer->createDrawingBoard(
                    board.dynamicCast<DrawingBoardClass>());
            ModelingServerMng::getInstance().m_pUIServer->GetMainUI()->SwitchWorkareaWidget(board->getName());
        }
    }
}

void CanvasWidget::onStackedWidgetChanged(int curIndex)
{
    Q_UNUSED(curIndex);
    if (m_navWidget && m_curModelWidget) {
        m_navWidget->setCurModel(m_curModelWidget->getModel());
    }
}

void CanvasWidget::onSelectedBlock(searchListData &data)
{
    for (int index = 0; index < m_stackedWidget->count(); index++) {
        auto modelWidget = dynamic_cast<ModelWidget *>(m_stackedWidget->widget(index));
        if (modelWidget) {
            modelWidget->getCanvasView()->setSearchHighLight(QStringList());
        }
    }
    if (!data.uuid.isEmpty()) {
        if (!m_boardModel)
            return;

        QStringList uuidList = data.boardUuidPath;
        if (uuidList.isEmpty())
            return;
        QList<PModel> modelList;
        PModel model = m_boardModel;
        modelList.push_back(model);
        for (int i = 1; i < uuidList.size() && model; ++i) {
            model = model->getChildModel(uuidList[i]);
            if (model) {
                modelList.push_back(model);
            }
        }
        if (model) {
            // 如果当前显示的界面对应的model不是要搜索的模块所在的model，那么需要切换显示界面为对应model
            if (!(m_curModelWidget->getModel()) || model->getUUID() != m_curModelWidget->getModel()->getUUID()) {
                modelList.removeLast();
                m_navWidget->addItemList(modelList);
                openCurrentModel(model);
            }
            ModelWidget *widget = getModelWidgetByUUid(model->getUUID());
            if (widget) {
                widget->getCanvasView()->setSearchHighLight(QStringList(data.uuid));
            }
        }
    }
}

QList<BlockStruct> CanvasWidget::getCanCreateBlockList()
{
    if (m_topModelWidget)
        return m_topModelWidget->getCanCreateBlockList();
    return QList<BlockStruct>();
}

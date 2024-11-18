
#include "ScopeViewToolbar.h"
// 添加工作区必须的头文件
#include "SimulationManagerServer/ISimulationManagerServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
// 工作区服务命名空间
using namespace Kcc::CoreManUI_Layout;
using namespace Kcc::SimulationManager;

ScopeViewToolbar::ScopeViewToolbar(QWidget *parent) : QToolBar(parent), mDisplayNum(nullptr)
{
    m_signalMapper = new QSignalMapper(this);
    this->setFixedHeight(36);

    m_addAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    m_addAction->setToolTip(tr("Add(Ctrl+N)"));
    m_addAction->setShortcut(QKeySequence::New);
    //  m_addAction->setIcon(QIcon(":/image/添加.png"));
    m_signalMapper->setMapping(m_addAction, 0);
    QToolButton *addbt = (QToolButton *)this->widgetForAction(m_addAction);
    addbt->setObjectName("add");
    addbt->setFixedSize(20, 20);

    addSeparator();

    mUndoAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    mUndoAction->setToolTip(tr("Undo(Ctrl+Z)")); // 撤销
    mUndoAction->setShortcut(QKeySequence::Undo);
    m_signalMapper->setMapping(mUndoAction, 28);
    QToolButton *undoPt = (QToolButton *)this->widgetForAction(mUndoAction);
    undoPt->setObjectName("undo");
    setActionState(ActionName::undo, false);
    undoPt->setFixedSize(20, 20);

    mRedoAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    mRedoAction->setToolTip(tr("Redo(Ctrl+Y)")); // 重做
    mRedoAction->setShortcut(QKeySequence::Redo);
    m_signalMapper->setMapping(mRedoAction, 29);
    QToolButton *redoPt = (QToolButton *)this->widgetForAction(mRedoAction);
    redoPt->setObjectName("redo");
    setActionState(ActionName::redo, false);
    redoPt->setFixedSize(20, 20);
    addSeparator();

    m_copyAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));

    // m_copyAction->setIcon(QIcon(":/image/复制.png"));
    m_copyAction->setToolTip(tr("Copy(Ctrl+C)"));
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_signalMapper->setMapping(m_copyAction, 1);
    QToolButton *cpbt = (QToolButton *)this->widgetForAction(m_copyAction);
    cpbt->setObjectName("copy");
    setActionState(ActionName::copy, false);
    cpbt->setFixedSize(20, 20);
    m_cutAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    // m_cutAction->setIcon(QIcon(":/image/剪切.png"));
    m_cutAction->setToolTip(tr("Cut(Ctrl+X)"));
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_signalMapper->setMapping(m_cutAction, 2);
    QToolButton *cutbt = (QToolButton *)this->widgetForAction(m_cutAction);
    cutbt->setObjectName("cut");
    setActionState(ActionName::cut, false);
    cutbt->setFixedSize(20, 20);
    m_pasteAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    // m_pasteAction->setIcon(QIcon(":/image/粘贴.png"));
    m_pasteAction->setToolTip(tr("Paste(Ctrl+V)"));
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_signalMapper->setMapping(m_pasteAction, 3);
    QToolButton *pastebt = (QToolButton *)this->widgetForAction(m_pasteAction);
    pastebt->setObjectName("paste");
    setActionState(ActionName::paste, false);
    pastebt->setFixedSize(20, 20);
    m_delAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    // m_delAction->setIcon(QIcon(":/image/删除.png"));
    m_delAction->setToolTip(tr("Delete(Del)"));
    m_delAction->setShortcut(QKeySequence::Delete);
    m_signalMapper->setMapping(m_delAction, 4);
    QToolButton *delbt = (QToolButton *)this->widgetForAction(m_delAction);
    delbt->setObjectName("del");
    setActionState(ActionName::del, false);
    delbt->setFixedSize(20, 20);
    addSeparator();

    m_allSelectAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));

    // m_allSelectAction->setIcon(QIcon(":/image/全选.png"));
    m_allSelectAction->setToolTip(tr("Select All(Ctrl+A)"));
    m_allSelectAction->setShortcut(QKeySequence::SelectAll);
    m_signalMapper->setMapping(m_allSelectAction, 24);
    QToolButton *selectallbt = (QToolButton *)this->widgetForAction(m_allSelectAction);
    selectallbt->setObjectName("selectall");
    selectallbt->setFixedSize(20, 20);
    addSeparator();

    m_showSelectedAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    // m_showSelectedAction->setIcon(QIcon(":/image/显示选中.png"));
    m_showSelectedAction->setToolTip(tr("Show Selected")); // 显示选中
    m_signalMapper->setMapping(m_showSelectedAction, 5);
    QToolButton *displaychosedbt = (QToolButton *)this->widgetForAction(m_showSelectedAction);
    displaychosedbt->setObjectName("displaychosed");
    displaychosedbt->setFixedSize(20, 20);
    m_showAllAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    // m_showAllAction->setIcon(QIcon(":/image/显示所有.png"));
    m_showAllAction->setToolTip(tr("Show All")); // 显示所有
    m_signalMapper->setMapping(m_showAllAction, 6);
    QToolButton *displayallbt = (QToolButton *)this->widgetForAction(m_showAllAction);
    displayallbt->setObjectName("displayall");
    displayallbt->setFixedSize(20, 20);
    addSeparator();

    m_resetAxisAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_resetAxisAction->setIcon(QIcon(":/image/初始坐标.png"));
    m_resetAxisAction->setToolTip(tr("Initial Coordinates")); // 初始坐标
    m_signalMapper->setMapping(m_resetAxisAction, 19);
    QToolButton *initcordbt = (QToolButton *)this->widgetForAction(m_resetAxisAction);
    initcordbt->setObjectName("initcord");
    initcordbt->setFixedSize(20, 20);
    // addSeparator();
    m_xAxisFullAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_xAxisFullAction->setIcon(QIcon(":/image/水平撑满.png"));
    m_xAxisFullAction->setToolTip(tr("Adjust the X-Axis to fully display the waveform")); // X轴撑满
    m_signalMapper->setMapping(m_xAxisFullAction, 20);
    QToolButton *fullxbt = (QToolButton *)this->widgetForAction(m_xAxisFullAction);
    fullxbt->setObjectName("fullx");
    fullxbt->setFixedSize(20, 20);

    m_yAxisFullAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_yAxisFullAction->setIcon(QIcon(":/image/垂直撑满.png"));
    m_yAxisFullAction->setToolTip(tr("Adjust the Y-Axis to fully display the waveform")); // Y轴撑满
    m_signalMapper->setMapping(m_yAxisFullAction, 21);
    QToolButton *fullybt = (QToolButton *)this->widgetForAction(m_yAxisFullAction);
    fullybt->setObjectName("fully");
    fullybt->setFixedSize(20, 20);
    addSeparator();

    m_zoomInXAxisAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_zoomInXAxisAction->setIcon(QIcon(":/image/缩放X轴.png"));
    m_zoomInXAxisAction->setToolTip(tr("Allow waveform to be scaled along the X-Axis"));
    m_zoomInXAxisAction->setCheckable(true);
    m_signalMapper->setMapping(m_zoomInXAxisAction, 11);

    QToolButton *zoomxbt = (QToolButton *)this->widgetForAction(m_zoomInXAxisAction);
    zoomxbt->setObjectName("zoomx");
    zoomxbt->setFixedSize(20, 20);
    m_zoomInYAxisAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_zoomInYAxisAction->setIcon(QIcon(":/image/缩放Y轴.png"));
    m_zoomInYAxisAction->setToolTip(tr("Allow waveform to be scaled along the Y-Axis"));
    m_zoomInYAxisAction->setCheckable(true);
    m_signalMapper->setMapping(m_zoomInYAxisAction, 12);

    QToolButton *zoomybt = (QToolButton *)this->widgetForAction(m_zoomInYAxisAction);
    zoomybt->setObjectName("zoomy");
    zoomybt->setFixedSize(20, 20);
    addSeparator();

    m_layoutRow_1 = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    // m_layoutRow_1->setIcon(QIcon(":/image/布局.png"));
    m_layoutRow_1->setToolTip(tr("Layout")); // 布局
    m_signalMapper->setMapping(m_layoutRow_1, 13);
    QToolButton *layoutbt = (QToolButton *)this->widgetForAction(m_layoutRow_1);
    layoutbt->setObjectName("layout");
    layoutbt->setFixedSize(20, 20);
    addSeparator();

    m_syncCheckboxAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_syncCheckboxAction->setIcon(QIcon(":/image/同步.png"));
    m_syncCheckboxAction->setToolTip(tr("Synchronous Operation")); // 同步鼠标操作
    m_syncCheckboxAction->setCheckable(true);
    m_signalMapper->setMapping(m_syncCheckboxAction, 7);
    QToolButton *sysmousebt = (QToolButton *)this->widgetForAction(m_syncCheckboxAction);
    sysmousebt->setObjectName("syn");
    sysmousebt->setFixedSize(20, 20);
    addSeparator();

    m_mousePointAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_mousePointAction->setIcon(QIcon(":/image/鼠标取点.png"));
    m_mousePointAction->setToolTip(tr("Mouse Pointing")); // 鼠标取点
    m_mousePointAction->setCheckable(true);
    m_signalMapper->setMapping(m_mousePointAction, 23);
    QToolButton *choseptbt = (QToolButton *)this->widgetForAction(m_mousePointAction);
    choseptbt->setObjectName("mousept");
    choseptbt->setFixedSize(20, 20);
    // m_mergeSeparator = addSeparator();
    // m_mergeSeparator->setVisible(false);

    // m_mergeAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_mergeAction->setIcon(QIcon(":/image/合并.png"));
    // m_mergeAction->setToolTip(tr("Merge")); //合并
    // m_signalMapper->setMapping(m_mergeAction, 26);
    // m_mergeAction->setVisible(false);

    // m_unmergeAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_unmergeAction->setIcon(QIcon(":/image/拆分.png"));
    // m_unmergeAction->setToolTip(tr("Split")); //拆分
    // m_signalMapper->setMapping(m_unmergeAction, 27);
    // m_unmergeAction->setVisible(false);
    addSeparator();

    m_dataDictionaryAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_dataDictionaryAction->setIcon(QIcon(":/image/变量表.png"));
    m_dataDictionaryAction->setToolTip(tr("Variable Table")); // 变量表
    m_signalMapper->setMapping(m_dataDictionaryAction, 25);
    QToolButton *dictbt = (QToolButton *)this->widgetForAction(m_dataDictionaryAction);
    dictbt->setObjectName("dict");
    dictbt->setFixedSize(20, 20);
    addSeparator();
    m_exportAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    // m_exportAction->setIcon(QIcon(":/image/导出.png"));
    m_exportAction->setToolTip(tr("Export")); // 导出
    m_signalMapper->setMapping(m_exportAction, 22);
    QToolButton *exportbt = (QToolButton *)this->widgetForAction(m_exportAction);
    exportbt->setObjectName("export");
    exportbt->setFixedSize(20, 20);
    mDisplayNum = new QLabel();
    mDisplayNum->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mDisplayNum->setAlignment(Qt::AlignRight);
    mDisplayNum->setText(QString("<font color=#BFBFBF>%1</font>&nbsp;&nbsp;").arg(tr("Graph Number"))
                         + QString("<font color=#666666>0/15</font>"));
    this->addWidget(mDisplayNum);

    connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(triggered(int)));

    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    this->setIconSize(QSize(20, 20));
}

ScopeViewToolbar::~ScopeViewToolbar() { }

QPoint ScopeViewToolbar::getPlotLayoutChooseDialogShowPos(void)
{
    QRect rc = actionGeometry(m_layoutRow_1);
    return this->mapToGlobal(QPoint(rc.left(), rc.bottom()));
}

bool ScopeViewToolbar::setPlotFunc(const PlotFunc &plotFunc)
{
    if (m_addAction) {
        m_addAction->setVisible(plotFunc.isAddActionExist);
        m_copyAction->setVisible(plotFunc.isCopyActionExist);
        m_cutAction->setVisible(plotFunc.isCutActionExist);
        m_pasteAction->setVisible(plotFunc.isPasteActionExist);
        m_delAction->setVisible(plotFunc.isDelActionExist);
        m_dataDictionaryAction->setVisible(plotFunc.isDataDictionaryExist);
        m_mergeAction->setVisible(plotFunc.isMergeExist);
        m_unmergeAction->setVisible(plotFunc.isMergeExist);

        if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS)
            uiActionUpdate(plotFunc.isMergeExist);

        m_mergeSeparator->setVisible(true);
        return true;
    }
    return false;
}

void ScopeViewToolbar::switchActionChecked(int index)
{
    // if (isExternTrigger == false)
    //     return;
    //  index 7:同步操作, 11:X轴缩放, 12:Y轴缩放, 23:鼠标取点
    switch (index) {
    case 7:
        m_syncCheckboxAction->setChecked(!m_syncCheckboxAction->isChecked());
        break;
    case 11:
        m_zoomInXAxisAction->setChecked(!m_zoomInXAxisAction->isChecked());
        break;
    case 12:
        m_zoomInYAxisAction->setChecked(!m_zoomInYAxisAction->isChecked());
        break;
    case 23:
        m_mousePointAction->setChecked(!m_mousePointAction->isChecked());
        break;
    default:
        break;
    }
}

void ScopeViewToolbar::onStartActionTriggered()
{
    m_startAction->setEnabled(false);
    m_stopAction->setEnabled(true);
}

void ScopeViewToolbar::onStopActionTriggered()
{
    m_startAction->setEnabled(true);
    m_stopAction->setEnabled(false);
}

PlotActionCheckedState ScopeViewToolbar::GetPlotTooBarCheckedState()
{
    PlotActionCheckedState pcs;
    pcs.isMousePointActionChecked = m_mousePointAction->isChecked();
    pcs.isSyncCheckboxActionChecked = m_syncCheckboxAction->isChecked();
    pcs.isXScaleActionChecked = m_zoomInXAxisAction->isChecked();
    pcs.isYScaleActionChecked = m_zoomInYAxisAction->isChecked();

    return pcs;
}

bool ScopeViewToolbar::setPlotMenuVisibleState()
{
    if (m_mergeAction->isVisible() && m_unmergeAction->isVisible())
        uiActionUpdate(true);
    else
        uiActionUpdate(false);
    return true;
}

// 将界面上的action也更新
void ScopeViewToolbar::uiActionUpdate(bool isMergeExist)
{
    PICoreManUILayoutServer pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!pUIServer) {
        // LOGOUT("IWorkareaServer未注册", LOG_ERROR);
        return;
    }
    ICoreManUILayout *ui = pUIServer->GetMainUI();
    if (!ui)
        return;

    QAction *overlapPlotAct = ui->GetRegisterAction("PluginPlotViewoverlapPlot"); // 菜单栏合并绘图按钮
    if (overlapPlotAct)
        overlapPlotAct->setVisible(isMergeExist);
    QAction *splitPlotAct = ui->GetRegisterAction("PluginPlotViewsplitPlot"); // 菜单栏拆分绘图按钮
    if (splitPlotAct)
        splitPlotAct->setVisible(isMergeExist);
}

void ScopeViewToolbar::setPlotNum(int num)
{
    if (!mDisplayNum || num > 15 || num < 0) {
        return;
    }

    // QString tempStr = tr("Plot Number：") + QString::number(num) + "/15";
    mDisplayNum->setText(QString("<font color=#BFBFBF>%1</font>&nbsp;&nbsp;").arg(tr("Graph Number"))
                         + QString("<font color=#666666>%1/15</font>").arg(num));
}

void ScopeViewToolbar::setActionState(ActionName actName, bool isEnable)
{
    switch (actName) {
    case ActionName::add:
        m_addAction->setEnabled(isEnable);
        break;
    case ActionName::undo:
        mUndoAction->setEnabled(isEnable);
        break;
    case ActionName::redo:
        mRedoAction->setEnabled(isEnable);
        break;
    case ActionName::copy:
        m_copyAction->setEnabled(isEnable);
        break;
    case ActionName::cut:
        m_cutAction->setEnabled(isEnable);
        break;
    case ActionName::paste:
        m_pasteAction->setEnabled(isEnable);
        break;
    case ActionName::del:
        m_delAction->setEnabled(isEnable);
        break;
    case ActionName::allSelect:
        m_allSelectAction->setEnabled(isEnable);
        break;
    case ActionName::showSelect:
        m_showSelectedAction->setEnabled(isEnable);
        break;
    case ActionName::showAll:
        m_showAllAction->setEnabled(isEnable);
        break;
    case ActionName::resetAxis:
        m_resetAxisAction->setEnabled(isEnable);
        break;
    case ActionName::fullx:
        m_xAxisFullAction->setEnabled(isEnable);
        break;
    case ActionName::fully:
        m_yAxisFullAction->setEnabled(isEnable);
        break;
    case ActionName::zoomx:
        m_zoomInXAxisAction->setEnabled(isEnable);
        break;
    case ActionName::zoomy:
        m_zoomInYAxisAction->setEnabled(isEnable);
        break;
    case ActionName::layout:
        m_layoutRow_1->setEnabled(isEnable);
        break;
    case ActionName::syc:
        m_syncCheckboxAction->setEnabled(isEnable);
        break;
    case ActionName::mousePt:
        m_mousePointAction->setEnabled(isEnable);
        break;
    case ActionName::varTable:
        m_dataDictionaryAction->setEnabled(isEnable);
        break;
    case ActionName::screenShot:
        m_exportAction->setEnabled(isEnable);
        break;
    }
}

void ScopeViewToolbar::setActionCheckState(ActionName actName, bool can)
{
    switch (actName) {
    case ActionName::zoomx:
        m_zoomInXAxisAction->setChecked(can);
        break;
    case ActionName::zoomy:
        m_zoomInYAxisAction->setChecked(can);
        break;
    case ActionName::syc:
        m_syncCheckboxAction->setChecked(can);
        break;
    case ActionName::mousePt:
        m_mousePointAction->setChecked(can);
        break;
    }
}
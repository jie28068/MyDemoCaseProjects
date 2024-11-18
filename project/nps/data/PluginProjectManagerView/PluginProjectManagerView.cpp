#include "PluginProjectManagerView.h"
#include "AssistantDefine.h"
#include "CoreLib/DocHelper.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "KLWidgets/KCustomDialog.h"
#include "ProjectManagerViewServer.h"
#include "ProjectViewServers.h"
#include "openboardwidget.h"
using namespace Kcc::DocHelper;
#include <QCoreApplication>
#include <QDir>

REG_MODULE_BEGIN(PluginProjectManagerView, "", "PluginProjectManagerView")
REG_MODULE_END(PluginProjectManagerView)

USE_LOGOUT_("PluginProjectManagerView")

PluginProjectManagerView::PluginProjectManagerView(QString strName)
    : Module(Module_Type_Normal, strName),
      m_projManagerOpenAction(nullptr),
      m_projNetWorkManagerOpenAction(nullptr),
      m_projDialg(nullptr),
      m_pProjManagerWidget(nullptr),
      m_pNetworkDialg(nullptr),
      m_pNetworkManagerWidget(nullptr),
      m_actNewWorkspace(nullptr),
      m_actNewProject(nullptr),
      m_actImportWorkspace(nullptr),
      m_actImportProject(nullptr),
      m_actImportDemoProject(nullptr),
      m_actExportWorkspace(nullptr),
      m_actExportProject(nullptr),
      m_pTimer(nullptr)
{
    m_projManagerOpenAction = new QAction(this);
    m_projNetWorkManagerOpenAction = new QAction(this);
    m_openDrawboardAction = new QAction(this);
    m_addDrawboardAction = new QAction(this);
    m_openQuikisFilesAction = new QAction(this);

    m_actNewWorkspace = new QAction(this);
    m_actNewProject = new QAction(this);
    m_actImportWorkspace = new QAction(this);
    m_actImportProject = new QAction(this);
    m_actImportDemoProject = new QAction(this);
    m_actExportWorkspace = new QAction(this);
    m_actExportProject = new QAction(this);

    QObject::connect(m_projNetWorkManagerOpenAction, SIGNAL(triggered()), this, SLOT(onOpenNetworkManagerAction()));
    QObject::connect(m_projManagerOpenAction, SIGNAL(triggered()), this, SLOT(onOpenProjManagerAction()));
    QObject::connect(m_openDrawboardAction, SIGNAL(triggered()), this, SLOT(onOpenDrawboardAction()));
    QObject::connect(m_addDrawboardAction, SIGNAL(triggered()), this, SLOT(onAddDrawboardAction()));
    QObject::connect(m_openQuikisFilesAction, SIGNAL(triggered()), this, SLOT(onOpenQuikisFiles()));

    RegServer<IProjectManagerViewServer>(new ProjectManagerViewServer(this));
}

PluginProjectManagerView::~PluginProjectManagerView()
{
    if (m_projDialg != nullptr) {
        delete m_projDialg;
        m_projDialg = nullptr;
    }
    if (m_pNetworkDialg != nullptr) {
        delete m_pNetworkDialg;
        m_pNetworkDialg = nullptr;
    }
}

void PluginProjectManagerView::init(KeyValueMap &params)
{
    if (!ProjectViewServers::getInstance().init()) {
        LOGOUT("ProjectViewServers init false", LOG_ERROR);
    }
    QMainWindow *parentWidget = nullptr;
    if (ProjectViewServers::getInstance().m_pUIServer != nullptr
        && ProjectViewServers::getInstance().m_pUIServer->GetMainUI() != nullptr) {
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "openProjManager", m_projManagerOpenAction);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "openNetworkManager", m_projNetWorkManagerOpenAction);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "openDrawingBoard", m_openDrawboardAction);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "openQuikisFiles", m_openQuikisFilesAction);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "addDrawingBoard", m_addDrawboardAction);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerView", "onNew",
                                                                                   new QAction(this));
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerView",
                                                                                   "onImport", new QAction(this));
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerView",
                                                                                   "onExport", new QAction(this));

        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerView",
                                                                                   "onNewWorkspace", m_actNewWorkspace);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction("PluginProjectManagerView",
                                                                                   "onNewProject", m_actNewProject);

        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "onImportWorkspace", m_actImportWorkspace);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "onImportProject", m_actImportProject);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "onImportDemoProject", m_actImportDemoProject);

        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "onExportWorkspace", m_actExportWorkspace);
        ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginProjectManagerView", "onExportProject", m_actExportProject);
        parentWidget = dynamic_cast<QMainWindow *>(ProjectViewServers::getInstance().m_pUIServer->GetMainUI());
    }
    m_ProjDataManager = PProjDataManager(new ProjDataManager());
    m_pProjManagerWidget = new ProjectManagerWidget(m_ProjDataManager);
    m_pNetworkManagerWidget = new NetworkModelManagerWidget(m_ProjDataManager);
    m_projDialg = new KCustomDialog(AssistantDefine::TITLE_DATAMANAGER, m_pProjManagerWidget, KBaseDlgBox::NoButton,
                                    KBaseDlgBox::NoButton, parentWidget);
    initDialog(m_projDialg);
    m_pNetworkDialg = new KCustomDialog(AssistantDefine::TITLE_NETWORKMANAGER, m_pNetworkManagerWidget,
                                        KBaseDlgBox::NoButton, KBaseDlgBox::NoButton, parentWidget);
    initDialog(m_pNetworkDialg);
    // /SimuNPS用户手册/6 数据管理器
    // /SimuNPS用户手册/7 网络模型管理器
    m_projDialg->setObjectName("Data Manager");
    m_pNetworkDialg->setObjectName("Network Model Manager");
    //  按钮信号槽
    QObject::connect(m_projDialg, SIGNAL(closed(int)), m_pProjManagerWidget, SLOT(onManagerClosed(int)));
    QObject::connect(m_pNetworkDialg, SIGNAL(closed(int)), m_pNetworkManagerWidget, SLOT(onManagerClosed(int)));
    QObject::connect(m_pProjManagerWidget, SIGNAL(openBlockOrBoard()), this, SLOT(onProjectManagetHide()));
    QObject::connect(m_pProjManagerWidget, SIGNAL(updateNetwork()), m_pNetworkManagerWidget, SLOT(onUpdateNetwork()));
    QObject::connect(m_pNetworkManagerWidget, SIGNAL(modelChanged(QSharedPointer<Kcc::BlockDefinition::Model>)),
                     m_pProjManagerWidget, SLOT(onModelChanged(QSharedPointer<Kcc::BlockDefinition::Model>)));
    QObject::connect(m_pProjManagerWidget, SIGNAL(modelChanged(QSharedPointer<Kcc::BlockDefinition::Model>)),
                     m_pNetworkManagerWidget, SLOT(onModelChanged(QSharedPointer<Kcc::BlockDefinition::Model>)));
    QObject::connect(m_pProjManagerWidget,
                     SIGNAL(projectActiveStsChanged(QSharedPointer<KLProject>, QSharedPointer<KLProject>)),
                     m_pNetworkManagerWidget,
                     SLOT(onProjectActiveStsChanged(QSharedPointer<KLProject>, QSharedPointer<KLProject>)));
    QObject::connect(m_pProjManagerWidget, SIGNAL(deleteModel()), m_pNetworkManagerWidget, SLOT(onDeleteModel()));
    QObject::connect(m_pProjManagerWidget, SIGNAL(addModel(QSharedPointer<Kcc::BlockDefinition::Model>)),
                     m_pNetworkManagerWidget, SLOT(onAddModel(QSharedPointer<Kcc::BlockDefinition::Model>)));
    QObject::connect(m_pNetworkManagerWidget, SIGNAL(openBlockOrBoard()), this, SLOT(onNetworkManagetHide()));

    QObject::connect(m_actNewWorkspace, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileCreateWorkSpace()));
    QObject::connect(m_actNewProject, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileCreateProj()));
    QObject::connect(m_actImportWorkspace, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileImportWorkSpace()));
    QObject::connect(m_actImportProject, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileImportProj()));
    QObject::connect(m_actImportDemoProject, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileImportDemoProj()));
    QObject::connect(m_actExportWorkspace, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileExportWorkSpace()));
    QObject::connect(m_actExportProject, SIGNAL(triggered()), m_pProjManagerWidget, SLOT(onFileExportProj()));

    QSettings setting(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                      QApplication::applicationName());
    if (setting.value("Locale/ShowDemo", true).toBool()
        && KLProject::ProjectMode_DesignerExterrnal != m_ProjDataManager->projectMode()) {
        // 弹出选择示例项目界面
        QTimer::singleShot(2000, [this]() { m_pProjManagerWidget->onFileImportDemoProj(); });
    }

    // 关闭非模态窗口
    PIServerInterfaceBase progressServer = RequestServerInterface<ICoreManUILayoutServer>();
    if (progressServer) {
        progressServer->connectNotify(Notify_Window_Close, this,
                                      SLOT(onCloseDataManager(unsigned int, const NotifyStruct &)));
    }
    if (ProjectViewServers::getInstance().m_projectManagerServerIF) {
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_SwitchLanguage, this, SLOT(onCloseDataManager(unsigned int, const NotifyStruct &)));
    }

    m_pTimer = new QTimer(this);
    m_pTimer->start(1000);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

    if (KLProject::ProjectMode_DesignerExterrnal == m_ProjDataManager->projectMode()) {
        updateFileActions(false);
    }
}

void PluginProjectManagerView::unInit(KeyValueMap &saveParams)
{
    ProjectViewServers::getInstance().unInit();
    Module::unInit(saveParams);
}

void PluginProjectManagerView::JumpToReferenceModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || m_pProjManagerWidget == nullptr || m_projDialg == nullptr) {
        return;
    }
    m_pProjManagerWidget->JumpToReferenceModel(model);
    showDialog(m_projDialg);
}

void PluginProjectManagerView::editExternalProject(const QString &projectAbsPath)
{
    if (m_pProjManagerWidget == nullptr || m_projDialg == nullptr) {
        return;
    }
    m_pProjManagerWidget->editExternalProject(projectAbsPath);
    updateFileActions(false);
}

void PluginProjectManagerView::onOpenProjManagerAction()
{
    if (m_pProjManagerWidget == nullptr || m_projDialg == nullptr) {
        return;
    }
    m_pProjManagerWidget->clickedOpen();
    showDialog(m_projDialg);
}

void PluginProjectManagerView::onOpenNetworkManagerAction()
{
    if (m_pNetworkManagerWidget == nullptr || m_pNetworkDialg == nullptr) {
        return;
    }
    m_pNetworkManagerWidget->clickedOpen();
    showDialog(m_pNetworkDialg);
}

void PluginProjectManagerView::onOpenQuikisFiles()
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    PKLProject curProject = ProjectViewServers::getInstance().m_projectManagerServer->GetCurProject();
    if (curProject == nullptr) {
        LOGOUT(tr("Unable to open QuiKIS because there are no active project"), LOG_ERROR);
        return;
    }
    QList<SimulationResultInfo> simuResultList = curProject->getAllSimulationDataInfo();
    if (simuResultList.isEmpty()) {
        LOGOUT(tr("Unable to open QuiKIS because there are no simulation results for the current project"), LOG_ERROR);
        return;
    }
    for (SimulationResultInfo item : simuResultList) {
        if (!curProject->isFromRecord(item.name)) {
            ProjectViewServers::getInstance().m_projectManagerServer->OpenQUIKIS(
                    curProject->getProjectDir() + "/" + curProject->getSimulationDataDirName() + "/" + item.name);
        }
    }
}

void PluginProjectManagerView::onOpenDrawboardAction()
{
    if (ProjectViewServers::getInstance().m_pUIServer == nullptr
        || ProjectViewServers::getInstance().m_pUIServer->GetMainUI() == nullptr
        || ProjectViewServers::getInstance().m_pGraphicModelingServer == nullptr || m_ProjDataManager == nullptr) {
        return;
    }
    if (m_ProjDataManager->getCurrentProject() == nullptr) {
        LOGOUT(QObject::tr("no active project,cannot open model."), LOG_WARNING);
        return;
    }
    OpenBoardWidget *pOpenBoardWidget = new OpenBoardWidget();
    pOpenBoardWidget->setActionType(OpenBoardWidget::OpenDrawingboard);
    KCustomDialog *Dlg = new KCustomDialog(
            AssistantDefine::TITLE_OPENDRAWBOARD, pOpenBoardWidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
            KBaseDlgBox::Ok, dynamic_cast<QMainWindow *>(ProjectViewServers::getInstance().m_pUIServer->GetMainUI()));
    Dlg->resize(QSize(500, 83));
    Dlg->setEnableDoubleClicked2FullScreen(false);
    Dlg->setResizeAble(false);
    QPoint pos;
    QSize size;
    QWidget *mainWG = ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->GetMainWidget();
    if (mainWG) {
        pos = mainWG->pos();
        size = mainWG->size();
    }
    Dlg->move(pos.x() + ((size.width() - Dlg->width()) / 2), pos.y() + ((size.height() - Dlg->height()) / 2));
    if (KBaseDlgBox::Ok == Dlg->exec()) {
        PDrawingBoardClass model = pOpenBoardWidget->getDrawboard();
        if (model != nullptr) {
            ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(model);
        }
    }
    if (pOpenBoardWidget != nullptr) {
        delete pOpenBoardWidget;
        pOpenBoardWidget = nullptr;
    }
    if (Dlg != nullptr) {
        delete Dlg;
        Dlg = nullptr;
    }
}

void PluginProjectManagerView::onAddDrawboardAction()
{
    if (m_pProjManagerWidget != nullptr) {
        m_pProjManagerWidget->onMenuAddDrawBoard();
    }
}

void PluginProjectManagerView::onCloseDataManager(unsigned int, const NotifyStruct &notify)
{
    if (m_ProjDataManager != nullptr && m_pProjManagerWidget != nullptr) {
        m_ProjDataManager->setSortType(m_pProjManagerWidget->getSortType());
        m_ProjDataManager->saveTreeStateMap(PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG,
                                            m_pProjManagerWidget->getTreeStateMap());
        delete m_pProjManagerWidget;
        m_pProjManagerWidget = nullptr;
    }
    if (m_ProjDataManager != nullptr && m_pNetworkManagerWidget != nullptr) {
        m_ProjDataManager->saveTreeStateMap(PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG,
                                            m_pNetworkManagerWidget->getTreeStateMap());
        delete m_pNetworkManagerWidget;
        m_pNetworkManagerWidget = nullptr;
    }
    if (m_projDialg) {
        delete m_projDialg;
        m_projDialg = nullptr;
    }
    if (m_pNetworkDialg != nullptr) {
        delete m_pNetworkDialg;
        m_pNetworkDialg = nullptr;
    }
}

void PluginProjectManagerView::onProjectManagetHide()
{
    if (m_projDialg != nullptr && !m_projDialg->isHidden()) {
        m_projDialg->hide();
    }
}

void PluginProjectManagerView::onNetworkManagetHide()
{
    if (m_pNetworkDialg != nullptr && !m_pNetworkDialg->isHidden()) {
        m_pNetworkDialg->hide();
    }
}

void PluginProjectManagerView::onTimerOut()
{
    if (m_pNetworkManagerWidget != nullptr) {
        m_pNetworkManagerWidget->initData();
    }
    if (m_pProjManagerWidget != nullptr) {
        m_pProjManagerWidget->initData();
    }
    if (m_pTimer != nullptr) {
        m_pTimer->stop();
    }
}

void PluginProjectManagerView::showDialog(KCustomDialog *dlg)
{
    if (dlg == nullptr) {
        return;
    }
    if (dlg->isHidden()) {
        dlg->show();
    } else {
        if (dlg->isMinimized()) {
            dlg->setWindowState(dlg->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
            dlg->hide();
            dlg->show();
        } else {
            dlg->activateWindow();
            dlg->hide();
            dlg->show();
        }
    }
}

void PluginProjectManagerView::initDialog(KCustomDialog *dlg)
{
    if (dlg == nullptr) {
        return;
    }
    dlg->resize(1202, 759);
    dlg->setMargin(1);
    dlg->setHiddenButtonGroup(true);
    dlg->setModal(false);
}

void PluginProjectManagerView::updateFileActions(bool actEnable)
{
    if (m_actNewWorkspace != nullptr) {
        m_actNewWorkspace->setEnabled(actEnable);
    }
    if (m_actNewProject != nullptr) {
        m_actNewProject->setEnabled(actEnable);
    }
    if (m_actImportWorkspace != nullptr) {
        m_actImportWorkspace->setEnabled(actEnable);
    }
    if (m_actExportWorkspace != nullptr) {
        m_actExportWorkspace->setEnabled(actEnable);
    }
    // if (m_actExportProject != nullptr) {
    //     m_actExportProject->setEnabled(actEnable);
    // }
}

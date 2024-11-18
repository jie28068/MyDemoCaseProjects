#include "ModelManagerView.h"
#include "ModelManagerViewServerMng.h"
#include "ModelViewDefine.h"
#include "TreeModelMng.h"

ModelManagerView::ModelManagerView(QWidget *parent)
    : QMainWindow(parent), m_actModelManagerView(nullptr), m_modelManagerViewDialog(nullptr)
{
    ui.setupUi(this);

    initUI();

    m_actModelManagerView = new QAction();
    connect(m_actModelManagerView, &QAction::triggered, this, &ModelManagerView::onTriggerModelManagerView);

    initPlatformAction();
}

ModelManagerView::~ModelManagerView() { }

void ModelManagerView::showDialog(KCustomDialog *dlg)
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
        }
    }
}

void ModelManagerView::initDialog(KCustomDialog *dlg)
{
    if (dlg == nullptr) {
        return;
    }
    dlg->resize(1000, 800);
    dlg->setMargin(1);
    dlg->setHiddenButtonGroup(true);
    dlg->setModal(false);
}

void ModelManagerView::onTriggerModelManagerView()
{
    showDialog(m_modelManagerViewDialog);
}

void ModelManagerView::onTreeViewClicked(QModelIndex index)
{
    TreeItem *item = ui.treeWidget->m_modelTreeView->getItemByIndex(index);
    if (item) {
        ui.listView->refreshListModel(item);
    }
}

void ModelManagerView::initUI()
{
    ui.splitter->setStretchFactor(0, 3);
    ui.splitter->setStretchFactor(1, 7);

    // 初始化外层包裹
    QWidget *mainWidget = ModelManagerViewServerMng::getInstance().m_pUIServer->GetMainUI()->GetMainWidget();
    m_modelManagerViewDialog = new KCustomDialog(ModelViewDefine::VIEW_TITLE_NAME, this, KBaseDlgBox::NoButton,
                                                 KBaseDlgBox::NoButton, mainWidget);
    initDialog(m_modelManagerViewDialog);

    // 绑定信号
    connect(ui.treeWidget->m_modelTreeView, &ModelTreeView::clicked, this, &ModelManagerView::onTreeViewClicked);
}

void ModelManagerView::initPlatformAction()
{
    if (ModelManagerViewServerMng::getInstance().m_pUIServer
        && ModelManagerViewServerMng::getInstance().m_pUIServer->GetMainUI()) {
        ModelManagerViewServerMng::getInstance().m_pUIServer->GetMainUI()->RegisterAction(
                "PluginModelManagerModule", "triggerModelManagerView", m_actModelManagerView);
    }
}

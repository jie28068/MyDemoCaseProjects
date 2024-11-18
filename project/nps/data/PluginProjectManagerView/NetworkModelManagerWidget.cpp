#include "NetworkModelManagerWidget.h"

#include "ProjActionManager.h"
#include "ProjectViewServers.h"
#include "SyncModelData.h"

NetworkModelManagerWidget::NetworkModelManagerWidget(QSharedPointer<ProjDataManager> pProjDataManager, QWidget *parent)
    : ManagerCommonWidget(pProjDataManager, parent)
{
    initUI();
    connectSignals();
}

NetworkModelManagerWidget::~NetworkModelManagerWidget()
{
    disconnectSignals();
}

void NetworkModelManagerWidget::initData()
{
    if (m_treeModel == nullptr || ui.treeView == nullptr) {
        return;
    }
    m_treeModel->setInitTreeState(m_pProjDataManager->getTreeStateMap(PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG));
    m_treeModel->updateNetWorkTree(ui.treeView);
    m_treeModel->loadTreeState(ui.treeView);
    QModelIndex parentindex = ui.treeView->model()->index(0, 0);
    ui.treeView->setCurrentIndex(parentindex);
    onTreeClicked(parentindex);
    connectProject(getCurrentTreeProject());
}

void NetworkModelManagerWidget::onManagerClosed(int btn) { }

void NetworkModelManagerWidget::onReceiveProjectManagerServerMsg(uint code, const NotifyStruct &param)
{
    if (IPM_Notify_DrawingBoardSaved == code || IPM_Notify_CodePrototypeDestroyed == code
        || IPM_Notify_DrawingBoardDestroyed == code || IPM_Notify_DrawingBoardFileRenamed == code
        || IPM_Notify_DeviceTypeDestroyed == code || ProjectManager::IPM_Notify_CurrentProjectChanged == code
        || ProjectManager::IPM_Notify_CurrentProjectDeactivate == code) {
        updateUI(getTreeCurrIndexNames());
    } else if (IPM_Notify_CodePrototypeSaved == code) {
        connectModel(getProjectModelByName(param.paramMap["name"].toString(), ControlBlock::Type));
        updateUI(getTreeCurrIndexNames());
    } else if (IPM_Notify_DrawingBoardCreate == code) {
        connectModel(getProjectModelByName(param.paramMap["name"].toString(), param.paramMap["type"].toInt()));
        updateUI(getTreeCurrIndexNames());
    }
}

void NetworkModelManagerWidget::onSelectedRowsChanged(const int &totalselectrow)
{
    ManagerCommonWidget::onSelectedRowsChanged(totalselectrow);
    if (ui.tableView != nullptr) {
        ProjActionManager::getInstance().setTableMenuStatus(ui.tableView->getTableMenuEnableList());
    }
}

void NetworkModelManagerWidget::onUpdateNetwork()
{
    updateUI(getTreeCurrIndexNames());
}

void NetworkModelManagerWidget::onProjectActiveStsChanged(QSharedPointer<KLProject> oldActiveProj,
                                                          QSharedPointer<KLProject> newActiveProj)
{
    if (oldActiveProj != nullptr) {
        disconnectProject(oldActiveProj);
    }
    if (newActiveProj != nullptr) {
        connectProject(newActiveProj);
    }
    if (m_treeModel == nullptr || ui.treeView == nullptr) {
        return;
    }
    m_treeModel->setInitTreeState(m_pProjDataManager->getTreeStateMap(PROJDATAMNGKEY::KEY_TREETYPE_NETWORKMNG));
    m_treeModel->updateNetWorkTree(ui.treeView);
    m_treeModel->loadTreeState(ui.treeView);
    QModelIndex parentindex = ui.treeView->model()->index(0, 0);
    ui.treeView->setCurrentIndex(parentindex);
    onTreeClicked(parentindex);
}

void NetworkModelManagerWidget::onDeleteModel()
{
    updateUI(getTreeCurrIndexNames());
}

void NetworkModelManagerWidget::onAddModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    connectModel(model);
    updateUI(getTreeCurrIndexNames());
}

void NetworkModelManagerWidget::onPropertyManagerServerMsg(unsigned int code, const NotifyStruct &structinfo)
{
    QString modelUUID = structinfo.paramMap[PMKEY::MODEL_UUID].toString();
    int modelType = structinfo.paramMap[PMKEY::MODEL_TYPE].toInt();
    if (modelUUID.isEmpty()) {
        return;
    }
    switch (code) {
    case Notify_DrawingBoardCreate:
    case Notify_CreateDeviceType: {
        connectModel(getModelByUUIDTYPE(modelUUID, modelType));
        updateUI(getTreeCurrIndexNames());
        break;
    }
    case Notify_CreateBlockInstance:
    case Notify_CreateComplexInstance:
        updateUI(getTreeCurrIndexNames());
        break;
    default:
        break;
    }
}

void NetworkModelManagerWidget::onActionTriggered()
{
    if (ProjActionManager::getInstance().getActTriggerObj() != AssistantDefine::WidgetObject_NetworkTable
        && ProjActionManager::getInstance().getActTriggerObj() != AssistantDefine::WidgetObject_NetworkTree) {
        return;
    }
    QAction *action = dynamic_cast<QAction *>(sender());
    QVariant var = action->data();
    ProjActionManager::ActionType type = var.value<ProjActionManager::ActionType>();
    QModelIndex curindex = ui.tableView->currentIndex();
    QModelIndexList selectIndexs = ui.tableView->getCurrentSelectedIndexs();
    if (nullptr == action || !curindex.isValid() || m_pTableModel == nullptr) {
        return;
    }
    PKLProject currProject = getCurrentTreeProject();
    if (currProject == nullptr) {
        return;
    }
    switch (type) {
    case ProjActionManager::Act_Undo: {
        if (m_pUndoStack != nullptr) {
            m_pUndoStack->undo();
        }
    } break;
    case ProjActionManager::Act_Redo: {
        if (m_pUndoStack != nullptr) {
            m_pUndoStack->redo();
        }
    } break;
    case ProjActionManager::Act_BatchModify: {
        onTableBatchModify();
    } break;
    case ProjActionManager::Act_copy: {
        onTableCopy();
        if (ui.tableView != nullptr) {
            ProjActionManager::getInstance().setTableMenuStatus(ui.tableView->getTableMenuEnableList());
        }
    } break;
    case ProjActionManager::Act_paste: {
        onTablePaste();
        if (ui.tableView != nullptr) {
            ProjActionManager::getInstance().setTableMenuStatus(ui.tableView->getTableMenuEnableList());
        }
    } break;
    case ProjActionManager::Act_open: {
        onTableOpenBlock();
    } break;
    case ProjActionManager::Act_edit: {
        onTableEdit();
    } break;
    default:
        break;
    }
}

void NetworkModelManagerWidget::onTreeClicked(const QModelIndex &index)
{
    if (!index.isValid() || m_pProxyModel == nullptr || m_pTableModel == nullptr) {
        return;
    }
    QString tablename = m_treeModel->getIndexNames(index).join("|");
    m_pUndoStack = UndoStackManager::instance().getUndoStack(tablename);
    m_pProxyModel->setSourceModel(nullptr);
    int freezecol = 0;
    m_pTableModel->updateTableData(index, freezecol);
    m_pProxyModel->setSourceModel(m_pTableModel);
    m_pProxyModel->sort(0);
    initTableInfo(tablename, getTableHeadList(), freezecol, AssistantDefine::TableCol_Width);
}

void NetworkModelManagerWidget::onModelValueChanged(const QList<OldNewDataStruct> &modifylist)
{
    if (m_pUndoStack != nullptr) {
        m_pUndoStack->push(new TableUndoCommand(m_pTableModel, modifylist, false));
    }
}

void NetworkModelManagerWidget::initUI()
{
    ManagerCommonWidget::initUI();
    addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_copy));
    addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_paste));
    addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_delete));
    if (ui.toolBar != nullptr) {
        ui.toolBar->hide();
    }
    if (m_pTableModel != nullptr) {
        m_pTableModel->setWidgetObject(AssistantDefine::WidgetObject_NetworkTable);
    }
    if (m_treeModel != nullptr) {
        m_treeModel->setWidgetObject(AssistantDefine::WidgetObject_NetworkTree);
    }
    createSyncDataModel();
}

void NetworkModelManagerWidget::connectSignals()
{
    ManagerCommonWidget::connectSignals();
}

void NetworkModelManagerWidget::disconnectSignals()
{
    ManagerCommonWidget::disconnectSignals();
}

void NetworkModelManagerWidget::updateUI(const QStringList &indexNames)
{
    if (m_treeModel == nullptr || ui.treeView == nullptr) {
        return;
    }
    m_treeModel->saveTreeState(ui.treeView);
    m_treeModel->updateNetWorkTree(ui.treeView);
    m_treeModel->loadTreeState(ui.treeView);
    if (indexNames.size() <= 0) {
        QModelIndex parentindex = ui.treeView->model()->index(0, 0);
        ui.treeView->setCurrentIndex(parentindex);
        onTreeClicked(parentindex);
    } else {
        clickedIndex(indexNames);
    }
}

void NetworkModelManagerWidget::enterEvent(QEvent *event)
{
    ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_NetworkTable);
    if (ui.tableView == nullptr) {
        return;
    }
    ProjActionManager::getInstance().setTableMenuStatus(ui.tableView->getTableMenuEnableList());
}

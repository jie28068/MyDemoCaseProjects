#include "ProjectManagerWidget.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "DemoProjectView.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLWidgets/KInputDialog.h"
#include "KLWidgets/KMessageBox.h"
#include "ProjActionManager.h"
#include "ProjectViewServers.h"
#include "SyncModelData.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QPHelper.h>

USE_LOGOUT_("ProjectManagerWidget")

ProjectManagerWidget::ProjectManagerWidget(QSharedPointer<ProjDataManager> pProjDataManager, QWidget *parent)
    : ManagerCommonWidget(pProjDataManager, parent), m_pTimer(nullptr)
{
    initUI();
    connectSignals();
}

ProjectManagerWidget::~ProjectManagerWidget()
{
    disconnectSignals();
}

void ProjectManagerWidget::initData()
{
    if (m_treeModel == nullptr || ui.treeView == nullptr || m_pProjDataManager == nullptr
        || m_syncModelData == nullptr) {
        return;
    }
    m_treeModel->setInitTreeState(m_pProjDataManager->getTreeStateMap(PROJDATAMNGKEY::KEY_TREETYPE_DATAMNG));
    m_treeModel->setInitSortType(CustomTreeModel::ProjectSortType(m_pProjDataManager->getSortType()));
    if (m_pProjDataManager->getSortType() == CustomTreeModel::ProjectSortType_Time) {
        ProjActionManager::getInstance().setActionCheckable(ProjActionManager::Act_SortProjByTime, true);
        ProjActionManager::getInstance().setActionCheckable(ProjActionManager::Act_SortProjByName, false);
    } else {
        ProjActionManager::getInstance().setActionCheckable(ProjActionManager::Act_SortProjByTime, false);
        ProjActionManager::getInstance().setActionCheckable(ProjActionManager::Act_SortProjByName, true);
    }
    m_treeModel->updateTree(ui.treeView);
    m_treeModel->loadTreeState(ui.treeView);
    QModelIndex parentindex = ui.treeView->model()->index(0, 0);
    ui.treeView->setCurrentIndex(parentindex);
    onTreeClicked(parentindex);
    QMap<QString, WorkSpaceInfo> wsinfoMap = m_pProjDataManager->getAllWorkSpace();
    for (WorkSpaceInfo &wsitem : wsinfoMap.values()) {
        for (PKLProject proj : wsitem.allprojects.values()) {
            m_syncModelData->connectProjectAllModels(proj);
        }
    }
}

void ProjectManagerWidget::JumpToReferenceModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    QStringList indexNames;
    indexNames.append(AssistantDefine::DATAROOT_NAME);
    indexNames.append(
            ProjectViewServers::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir().split("/"));

    QString boardName = model->getName();
    PModel parentModel = model;
    if (model->getParentModel() != nullptr) {
        parentModel = model->getParentModel();
        while (parentModel->getParentModel() != nullptr) {
            parentModel = parentModel->getParentModel();
        }
    }
    if (parentModel == nullptr) {
        return;
    }
    if (parentModel->isInstance()) {
        indexNames.append(AssistantDefine::NETMODEL_NAME);
        indexNames.append(parentModel->getName());
    } else {
        if (ElecBoardModel::Type == parentModel->getModelType()
            || ControlBoardModel::Type == parentModel->getModelType()) {
            indexNames.append(AssistantDefine::NETMODEL_NAME);
            indexNames.append(parentModel->getName());
        } else if (CombineBoardModel::Type == parentModel->getModelType()) {
            indexNames.append(AssistantDefine::LOCALLIB_NAME);
            indexNames.append(AssistantDefine::COMBINEDMODEL_NAME);
            indexNames.append(parentModel->getName());
        } else if (ComplexBoardModel::Type == parentModel->getModelType()) {
            indexNames.append(AssistantDefine::LOCALLIB_NAME);
            indexNames.append(AssistantDefine::COMPLEXMODEL_NAME);
            indexNames.append(parentModel->getName());
        }
    }
    JumptoPosition(indexNames);
}

void ProjectManagerWidget::editExternalProject(const QString &projectAbsPath)
{
    if (m_pProjDataManager == nullptr) {
        return;
    }
    QMap<QString, WorkSpaceInfo> allWSMap = m_pProjDataManager->getAllWorkSpace();
    QString wsName;
    QString projectName;
    if (m_pProjDataManager->getCurrentProject() != nullptr) {
        for (WorkSpaceInfo wsinfo : allWSMap.values()) {
            if (wsinfo.allprojects.values().contains(m_pProjDataManager->getCurrentProject())) {
                wsName = wsinfo.name;
                projectName = m_pProjDataManager->getCurrentProject()->getProjectName();
            }
        }
    }
    activeProjectHandle(wsName, projectName, false);
    m_pProjDataManager->editExternalProject(projectAbsPath);
    AssistantDefine::addProjectVarToDictionary(m_pProjDataManager->getCurrentProject());
    connectProject(m_pProjDataManager->getCurrentProject());
    emit projectActiveStsChanged(nullptr, m_pProjDataManager->getCurrentProject());
    updateUI(getTreeCurrIndexNames());
}

const int ProjectManagerWidget::getSortType()
{
    if (m_treeModel != nullptr) {
        return m_treeModel->getSortType();
    }
    return CustomTreeModel::ProjectSortType_Invalid;
}

void ProjectManagerWidget::initUI()
{
    ManagerCommonWidget::initUI();
    if (ui.treeView != nullptr) {
        ui.treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    if (m_pTableModel != nullptr) {
        m_pTableModel->setWidgetObject(AssistantDefine::WidgetObject_DataManagerTable);
    }
    if (m_treeModel != nullptr) {
        m_treeModel->setWidgetObject(AssistantDefine::WidgetObject_DataManagerTree);
    }
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Up));
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Down));
    ui.toolBar->addSeparator();
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Refresh));
    ui.toolBar->addSeparator();
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_toolbarNew));
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_delete));
    ui.toolBar->addSeparator();
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_cut));
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_copy));
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_paste));
    ui.toolBar->addSeparator();
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_import));
    ui.toolBar->addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_export));
    ui.toolBar->addSeparator();
    ui.toolBar->addWidget(ProjActionManager::getInstance().getSortButton());
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    QPushButton *importCsvBtn = new QPushButton();
    importCsvBtn->setFixedSize(QSize(20, 20));
    importCsvBtn->setObjectName("csvimport");
    importCsvBtn->setToolTip("导入csv设备类型");
    connect(importCsvBtn, SIGNAL(released()), this, SLOT(onImportDeviceCSV()), Qt::UniqueConnection);
    ui.toolBar->addWidget(importCsvBtn);
    QPushButton *importElecDevCsvBtn = new QPushButton();
    importElecDevCsvBtn->setFixedSize(QSize(20, 20));
    importElecDevCsvBtn->setObjectName("importElecDevCsvBtn");
    importElecDevCsvBtn->setToolTip("导入电气元件选择设备类型csv");
    connect(importElecDevCsvBtn, SIGNAL(released()), this, SLOT(onElementsConnectDeviceTypeCSV()),
            Qt::UniqueConnection);
    ui.toolBar->addWidget(importElecDevCsvBtn);
#endif
    m_pTimer = new QTimer(this);
    createSyncDataModel();
}

void ProjectManagerWidget::connectSignals()
{
    ManagerCommonWidget::connectSignals();
    if (m_pTimer != nullptr) {
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()), Qt::UniqueConnection);
    }
    if (ui.treeView != nullptr && ui.treeView->selectionModel() != nullptr) {
        connect(ui.treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
                &ProjectManagerWidget::onSelectionChanged, Qt::UniqueConnection);
    }
}

void ProjectManagerWidget::disconnectSignals()
{
    ManagerCommonWidget::disconnectSignals();
    if (m_pTimer != nullptr) {
        disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
    }
    if (ui.treeView != nullptr && ui.treeView->selectionModel() != nullptr) {
        disconnect(ui.treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
                   &ProjectManagerWidget::onSelectionChanged);
    }
}

void ProjectManagerWidget::updateUI(const QStringList &indexNames)
{
    if (m_treeModel == nullptr || ui.treeView == nullptr) {
        return;
    }
    if (indexNames.size() <= 0) {
        m_treeModel->saveTreeState(ui.treeView);
        m_treeModel->updateTree(ui.treeView);
        m_treeModel->loadTreeState(ui.treeView);
        QModelIndex parentindex = ui.treeView->model()->index(0, 0);
        ui.treeView->setCurrentIndex(parentindex);
        onTreeClicked(parentindex);
    } else {
        m_treeModel->saveTreeState(ui.treeView);
        m_treeModel->updateTree(ui.treeView);
        m_treeModel->loadTreeState(ui.treeView);
        clickedIndex(indexNames);
    }
}

void ProjectManagerWidget::enterEvent(QEvent *event)
{
    ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerCommon);
    if (ui.treeView != nullptr) {
        updateAllActionSts(ui.treeView->currentIndex());
    }
}

void ProjectManagerWidget::updateAllActionSts(const QModelIndex &treeIndex)
{
    if (ui.treeView == nullptr || ui.tableView == nullptr
        || AssistantDefine::WidgetObject_DataManagerTable == ProjActionManager::getInstance().getActTriggerObj()
        || AssistantDefine::WidgetObject_DataManagerTree == ProjActionManager::getInstance().getActTriggerObj()) {
        return;
    }
    QList<ProjActionManager::ActionType> tableEnableList = ui.tableView->getTableMenuEnableList();
    QList<ProjActionManager::ActionType> treeList;
    QString activeText;
    QString newText;
    getTreeEnableActionList(ui.treeView->currentIndex(), treeList, activeText, newText);
    ProjActionManager::getInstance().setAllActionStatus(treeList, tableEnableList, activeText, newText);
}

void ProjectManagerWidget::updateTreeMenuSts(const QModelIndex &index)
{
    if (!index.isValid()) { // 右键空白处，不显示菜单
        return;
    }
    if (m_treeModel == nullptr) { // 右键空白处，不显示菜单
        return;
    }
    QList<ProjActionManager::ActionType> enableList;
    QString activeText = "";
    QString newText = "";
    getTreeEnableActionList(index, enableList, activeText, newText);
    ProjActionManager::getInstance().setTreeMenuStatus(enableList, activeText, newText);
}

void ProjectManagerWidget::getTreeEnableActionList(const QModelIndex &index,
                                                   QList<ProjActionManager::ActionType> &menuEnableList,
                                                   QString &activeTxt, QString &newText)
{
    menuEnableList.clear();
    activeTxt = AssistantDefine::STR_ACT_ACTIVE;
    newText = AssistantDefine::STR_ACT_NEW;
    if (!index.isValid()) { // 右键空白处，不显示菜单
        return;
    }
    if (m_treeModel == nullptr) { // 右键空白处，不显示菜单
        return;
    }
    QStandardItem *curitem = m_treeModel->itemFromIndex(index);
    if (curitem != nullptr && curitem->hasChildren()) {
        menuEnableList.append(ProjActionManager::Act_Down);
    }
    if (curitem != nullptr && curitem->parent() != nullptr) {
        menuEnableList.append(ProjActionManager::Act_Up);
    }
    menuEnableList.append(ProjActionManager::Act_Refresh);
    // 运行中禁用所有操作
    if (m_DrawboardIsRuning) {
        return;
    }
    bool isExternalDesinger = false;
    if (m_pProjDataManager != nullptr
        && KLProject::ProjectMode_DesignerExterrnal == m_pProjDataManager->projectMode()) {
        isExternalDesinger = true;
    }
    CustomMineData *cMineData = getCustomMineData();
    CustomTreeModel::NodeType itemnodetype =
            CustomTreeModel::NodeType(index.data(CustomTreeModel::CustomRole_NodeType).toInt());
    bool projectActive = m_treeModel->getProjectActiveSts(index);
    if (getTreeSelectedIndexes().size() > 1) {
        if ((itemnodetype == CustomTreeModel::NodeType_Model || itemnodetype == CustomTreeModel::NodeType_ModelInstance)
            && projectActive) {
            menuEnableList << ProjActionManager::Act_delete;
        }
        return;
    }

    switch (itemnodetype) {
    case CustomTreeModel::NodeType_DataRoot: {
        if (isExternalDesinger) {
            return;
        }
        menuEnableList << ProjActionManager::Act_new << ProjActionManager::Act_import;
        newText = AssistantDefine::STR_ACT_NEWWS;
        break;
    }
    case CustomTreeModel::NodeType_Workspace: {
        menuEnableList << ProjActionManager::Act_SortProjByName << ProjActionManager::Act_SortProjByTime;
        if (isExternalDesinger) {
            menuEnableList << ProjActionManager::Act_import;
            return;
        }
        bool bactive = index.data(CustomTreeModel::CustomRole_ActiveStatus).toBool();
        menuEnableList << ProjActionManager::Act_new << ProjActionManager::Act_import << ProjActionManager::Act_export;
        if (cMineData != nullptr && cMineData->getKLProject() != nullptr && cMineData->getModels().size() <= 0) {
            menuEnableList << ProjActionManager::Act_paste;
        }
        if (!bactive) {
            menuEnableList << ProjActionManager::Act_delete << ProjActionManager::Act_rename;
            activeTxt = AssistantDefine::STR_ACT_ACTIVE;
        } else {
            activeTxt = AssistantDefine::STR_ACT_INACTIVE;
        }
        newText = AssistantDefine::STR_ACT_NEWPROJ;
        break;
    }
    case CustomTreeModel::NodeType_Project: {
        if (isExternalDesinger) {
            menuEnableList << ProjActionManager::Act_settings << ProjActionManager::Act_export;
            return;
        }
        menuEnableList << ProjActionManager::Act_active << ProjActionManager::Act_copy << ProjActionManager::Act_export;
        if (!projectActive) {
            menuEnableList << ProjActionManager::Act_cut << ProjActionManager::Act_delete
                           << ProjActionManager::Act_rename;
            activeTxt = AssistantDefine::STR_ACT_ACTIVE;
        } else {
            activeTxt = AssistantDefine::STR_ACT_INACTIVE;
            menuEnableList << ProjActionManager::Act_settings;
        }
        break;
    }
    case CustomTreeModel::NodeType_DeviceType: {
        if (!projectActive) {
            return;
        }
        if (canPaste(cMineData, DeviceModel::Type)) {
            menuEnableList << ProjActionManager::Act_paste;
        }
        break;
    }
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    case CustomTreeModel::NodeType_GlobalDeviceType: {
        menuEnableList << ProjActionManager::Act_new;
        newText = AssistantDefine::STR_ACT_NEWDEVICETYPE;
        break;
    }
#endif
    case CustomTreeModel::NodeType_CombinedModel: {
        if (!projectActive) {
            return;
        }
        menuEnableList << ProjActionManager::Act_new;
        if (canPaste(cMineData, CombineBoardModel::Type)) {
            menuEnableList << ProjActionManager::Act_paste;
        }
        newText =
                QObject::tr("%1 %2").arg(AssistantDefine::STR_ACT_NEW).arg(AssistantDefine::STR_BOARDTYPE_COMBINEMODEL);
        break;
    }
    case CustomTreeModel::NodeType_ComplexModel: {
        if (!projectActive) {
            return;
        }
        menuEnableList << ProjActionManager::Act_new;
        if (canPaste(cMineData, ComplexBoardModel::Type)) {
            menuEnableList << ProjActionManager::Act_paste;
        }
        newText =
                QObject::tr("%1 %2").arg(AssistantDefine::STR_ACT_NEW).arg(AssistantDefine::STR_BOARDTYPE_COMPLEXMODEL);
        break;
    }
    case CustomTreeModel::NodeType_Network: {
        if (!projectActive) {
            return;
        }
        menuEnableList << ProjActionManager::Act_new << ProjActionManager::Act_newControlBoard
                       << ProjActionManager::Act_newElecCombine;
        if (haveModelProto(CombineBoardModel::Type) || haveModelProto(ControlBlock::Type)) {
            menuEnableList << ProjActionManager::Act_newBlockInstance;
        }
        if (haveModelProto(ComplexBoardModel::Type)) {
            menuEnableList << ProjActionManager::Act_newComplexInstance;
        }

        if (canPaste(cMineData, ElecBoardModel::Type) || canPaste(cMineData, ElecCombineBoardModel::Type)
            || canPaste(cMineData, ControlBoardModel::Type) || canPaste(cMineData, ComplexBoardModel::Type, "", true)
            || canPaste(cMineData, CombineBoardModel::Type, "", true)
            || canPaste(cMineData, ControlBlock::Type, "", true)) {
            menuEnableList << ProjActionManager::Act_paste;
        }
        newText = QObject::tr("%1 %2")
                          .arg(AssistantDefine::STR_ACT_NEW)
                          .arg(AssistantDefine::STR_BOARDTYPE_ELECTRICALSYSTEM);
        break;
    }
    case CustomTreeModel::NodeType_Model:
    case CustomTreeModel::NodeType_ModelInstance: {
        PModel pmodel = m_treeModel->getModel(index);
        if (pmodel == nullptr) {
            break;
        }
        if (pmodel->isInstance() && !projectActive) {
            break;
        }
        menuEnableList << ProjActionManager::Act_copy;
        if (!projectActive || !index.parent().isValid()) {
            break;
        }
        menuEnableList << ProjActionManager::Act_rename;
        bool activests = index.data(CustomTreeModel::CustomRole_ActiveStatus).toBool();
        if (ControlBlock::Type == pmodel->getModelType() && !pmodel->isInstance()) {
            // do nothing
        } else if (ControlBlock::Type == pmodel->getModelType() && pmodel->isInstance()) {
            menuEnableList << ProjActionManager::Act_edit << ProjActionManager::Act_delete;
        } else if (ElecBoardModel::Type == pmodel->getModelType()) {
            menuEnableList << ProjActionManager::Act_open << ProjActionManager::Act_edit
                           << ProjActionManager::Act_delete << ProjActionManager::Act_active;
        } else if (ControlBoardModel::Type == pmodel->getModelType()
                   || ComplexBoardModel::Type == pmodel->getModelType()
                   || CombineBoardModel::Type == pmodel->getModelType()
                   || ElecCombineBoardModel::Type == pmodel->getModelType()) {
            menuEnableList << ProjActionManager::Act_open << ProjActionManager::Act_edit
                           << ProjActionManager::Act_delete;
            if (CombineBoardModel::Type == pmodel->getModelType() && index.parent().isValid()
                && CustomTreeModel::NodeType_CombinedModel
                        == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
                menuEnableList << ProjActionManager::Act_newBlockInstance;
            } else if (ComplexBoardModel::Type == pmodel->getModelType() && index.parent().isValid()
                       && CustomTreeModel::NodeType_ComplexModel
                               == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
                menuEnableList << ProjActionManager::Act_newComplexInstance;
            }
        }
        activeTxt = (activests ? AssistantDefine::STR_ACT_INACTIVE : AssistantDefine::STR_ACT_ACTIVE);
        break;
    }
    case CustomTreeModel::NodeType_DeviceTypeClass: {
        if (!projectActive) {
            menuEnableList << ProjActionManager::Act_copy;
        } else {
            menuEnableList << ProjActionManager::Act_copy << ProjActionManager::Act_new
                           << ProjActionManager::Act_delete;
            QString deviceprototypename = index.data(CustomTreeModel::CustomRole_PrototypeName).toString();
            if (canPaste(cMineData, DeviceModel::Type, deviceprototypename)) {
                menuEnableList << ProjActionManager::Act_paste;
            }
        }
        newText = AssistantDefine::STR_ACT_NEWDEVICETYPE;
        break;
    }
    case CustomTreeModel::NodeType_CodeBlock: {
        if (projectActive) {
            menuEnableList << ProjActionManager::Act_new;
            if (canPaste(cMineData, ControlBlock::Type)) {
                menuEnableList << ProjActionManager::Act_paste;
            }
        }
        newText = QObject::tr("%1 %2").arg(AssistantDefine::STR_ACT_NEW).arg(AssistantDefine::CODEMODEL_NAME);
        break;
    }
    case CustomTreeModel::NodeType_CodeBlockClass: {
        menuEnableList << ProjActionManager::Act_copy;
        if (projectActive) {
            menuEnableList << ProjActionManager::Act_delete << ProjActionManager::Act_edit
                           << ProjActionManager::Act_newBlockInstance;
        }
        break;
    }
    case CustomTreeModel::NodeType_GlobalLib:
    case CustomTreeModel::NodeType_SimuResult: {
        break;
    }
    case CustomTreeModel::NodeType_SimuResultSubItem: {
        if (!projectActive) {
            return;
        }
        if (m_treeModel->isOriginResultInfo(index)) {
            menuEnableList << ProjActionManager::Act_record << ProjActionManager::Act_export
                           << ProjActionManager::Act_open;
        } else {
            menuEnableList << ProjActionManager::Act_rename << ProjActionManager::Act_delete
                           << ProjActionManager::Act_export << ProjActionManager::Act_open;
        }
        break;
    }
    default: {
        break;
    }
    }
}

void ProjectManagerWidget::updateTableData(const QModelIndex &index)
{
    if (!index.isValid() || m_pProxyModel == nullptr || m_pTableModel == nullptr) {
        return;
    }
    QStringList indexnames = m_treeModel->getIndexNames(index);
    QString stackkey = indexnames.join("|");
    m_pUndoStack = UndoStackManager::instance().getUndoStack(stackkey);
    int freezecol = 0;
    m_pTableModel->updateTableData(index, freezecol);
    m_pProxyModel->sort(0);
    initTableInfo(stackkey, getTableHeadList(), freezecol, AssistantDefine::TableCol_Width);
}

QString ProjectManagerWidget::getCurrentWorkSpaceName(const QString &msg)
{
    QString workspaceName;
    QMap<QString, WorkSpaceInfo> allworkspaceMap;
    WorkSpaceInfo curwsInfo;
    if (m_pProjDataManager != nullptr) {
        allworkspaceMap = m_pProjDataManager->getAllWorkSpace();
        curwsInfo = m_pProjDataManager->getCurrentWorkSpace();
    }

    if (curwsInfo.isValid()) {
        workspaceName = curwsInfo.name;
    } else if (allworkspaceMap.size() == 0) {
        LOGOUT(tr("Please create a new workspace"), LOG_WARNING); //"请新建工作空间"
    } else if (allworkspaceMap.size() == 1) {
        workspaceName = allworkspaceMap.keys()[0];
    } else {
        DemoProjectView *projectView = new DemoProjectView();
        projectView->setMessage(tr("Please select a workspace for %1").arg(msg));
        projectView->setNameList(allworkspaceMap.keys(), true);
        projectView->setSelectionMode(QAbstractItemView::SingleSelection);
        // 请选择工作空间
        KCustomDialog *dialog = new KCustomDialog(tr("Please Select A Workspace"), projectView,
                                                  KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::NoButton, this);
        dialog->resize(400, 500);

        if (dialog->exec() == KBaseDlgBox::Ok && projectView->getSelectNames().size() > 0) {
            workspaceName = projectView->getSelectNames()[0];
        }
        delete projectView;
        projectView = nullptr;
        delete dialog;
        dialog = nullptr;
    }
    return workspaceName;
}

bool ProjectManagerWidget::canPaste(CustomMineData *cmineData, const int &modelType, const QString &prototype,
                                    bool isInstance)
{
    if (cmineData == nullptr) {
        return false;
    }
    QList<PModel> models = cmineData->getModels(modelType, prototype);
    QList<PModel> realModels;
    for (PModel model : models) {
        if (model == nullptr) {
            continue;
        }
        if (isInstance && model->isInstance()) {
            realModels.append(model);
        } else if (!isInstance && !model->isInstance()) {
            realModels.append(model);
        }
    }
    if (realModels.size() > 0) {
        return true;
    }
    return false;
}

bool ProjectManagerWidget::copyModelToProject(QSharedPointer<KLProject> curProject,
                                              QSharedPointer<KLProject> srcProject,
                                              QSharedPointer<Kcc::BlockDefinition::Model> srcmodel)
{
    if (curProject == nullptr || srcmodel == nullptr || srcProject == nullptr) {
        return false;
    }

    if (srcmodel->isInstance()) {
        // 实例只能拷贝当前项目
        if (curProject != srcProject) {
            return false;
        }

        PModel protoModel = getProtoModel(srcProject, srcmodel);
        if (protoModel == nullptr) {
            return false;
        }
        PModel copyInstance = srcmodel->copy();
        if (copyInstance == nullptr) {
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_MODEL,
                                                srcmodel->getName(), false),
                   LOG_WARNING);
            return false;
        }
        QString newName = srcmodel->getName() + "(1)";
        QString errorinfo = "";
        int i = 1;
        while (!checkNameValid(curProject, copyInstance, newName, errorinfo)) {
            ++i;
            newName = QString("%1(%2)").arg(srcmodel->getName()).arg(QString::number(i));
        }
        copyInstance->setName(newName);
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_MODEL, newName,
                                            true),
               LOG_NORMAL);
        connectModel(copyInstance);
        if (AssistantDefine::modelNeedAddToDict(copyInstance, curProject)) {
            AssistantDefine::addModelVarToDict(copyInstance);
        }
        emit addModel(copyInstance);
        return true;
    }
    PModel copyOtherModel = curProject->copyModel(srcProject, srcmodel);
    if (copyOtherModel != nullptr) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_MODEL,
                                            copyOtherModel->getName(), true),
               LOG_NORMAL);
        connectModel(copyOtherModel);
        if (AssistantDefine::modelNeedAddToDict(copyOtherModel, curProject)) {
            AssistantDefine::addModelVarToDict(copyOtherModel);
        }
        emit addModel(copyOtherModel);
        return true;
    } else {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_MODEL,
                                            srcmodel->getName(), false),
               LOG_WARNING);
    }
    return false;
}

bool ProjectManagerWidget::deleteProjectModel(QSharedPointer<KLProject> project,
                                              QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (project == nullptr || model == nullptr) {
        return false;
    }
    if (ProjectViewServers::getInstance().m_MonitorPanelPlotServer != nullptr
        && ProjectViewServers::getInstance().m_projectManagerServer != nullptr) {
        ProjectViewServers::getInstance().m_MonitorPanelPlotServer->removeBoardVar(
                QStringList()
                << QString("%1.%2")
                           .arg(ProjectViewServers::getInstance().m_projectManagerServer->GetRealTimeSimulationName())
                           .arg(model->getName()));
    }
    QString modelname = model->getName();
    if (model->isInstance()) {
        PModel protoModel = getProtoModel(project, model);
        if (protoModel != nullptr) {
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_MODEL,
                                                modelname, true),
                   LOG_NORMAL);
            AssistantDefine::deleteModelVarToDict(model);
            protoModel->destroyInstance(model->getObjectUuid());
            project->saveModel(protoModel);
        } else {
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_MODEL,
                                                modelname, false),
                   LOG_WARNING);
            return false;
        }
    } else {
        if (ElecBoardModel::Type == model->getModelType()) {
            QVariantMap boardactivemap;
            if (project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
                boardactivemap = project->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
            }
            if (boardactivemap.contains(model->getUUID())) {
                AssistantDefine::deleteModelVarToDict(model);
            }
        } else if (ControlBoardModel::Type == model->getModelType()) {
            AssistantDefine::deleteModelVarToDict(model);
        }
        ProjectViewServers::getInstance().m_pGraphicModelingServer->deleteDrawingBoard(model->getName());
        if (ProjectViewServers::getInstance().m_projectManagerServer->DestroyBoardModel(model)) {

            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_MODEL,
                                                modelname, true),
                   LOG_NORMAL);
        } else {
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_MODEL,
                                                modelname, false),
                   LOG_WARNING);
        }
    }
    return true;
}

void ProjectManagerWidget::activeProjectHandle(const QString &workspacename, const QString &projname, bool bactive)
{
    QStringList indexNames = getTreeCurrIndexNames();
    // 激活画板时,关闭工作区所有画板
    if (!ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->DeleteWorkareaWidgets()) {
        return;
    }

    // 移除所有最近打开项目
    ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RemoveAllProject();
    PKLProject curActiveProject = m_pProjDataManager->getCurrentProject();
    m_pProjDataManager->setProjectActiveStatus(workspacename, projname, bactive);
    // 激活新项目，清除原来的数据字典变量,并将新变量加载进去
    AssistantDefine::addProjectVarToDictionary(m_pProjDataManager->getCurrentProject());
    updateUI(indexNames);
    emit projectActiveStsChanged(curActiveProject, m_pProjDataManager->getCurrentProject());
}

bool ProjectManagerWidget::haveModelProto(const int &modelType)
{
    if (m_pProjDataManager == nullptr || m_pProjDataManager->getCurrentProject() == nullptr) {
        return false;
    }
    return m_pProjDataManager->getCurrentProject()->getAllModel(modelType).size() > 0;
}

QSharedPointer<Kcc::BlockDefinition::Model>
ProjectManagerWidget::findElecModel(QList<QSharedPointer<Kcc::BlockDefinition::Model>> &allBoards,
                                    const QString &boardName, const QString &prototypeName, const QString &elmentsName)
{
    QList<PModel> models;
    for (PModel board : allBoards) {
        if (board == nullptr || boardName != board->getName()) {
            continue;
        }
        for (PModel cmodel : board->getChildModels().values()) {
            if (cmodel == nullptr) {
                continue;
            }
            if (cmodel->getName() == elmentsName && cmodel->getPrototypeName() == prototypeName) {
                return cmodel;
            } else if (cmodel->getModelType() == ElecCombineBoardModel::Type) {
                return findElecModel(QList<PModel>() << cmodel, boardName, prototypeName, elmentsName);
            }
        }
    }
    return nullptr;
}

QSharedPointer<Kcc::BlockDefinition::DeviceModel>
ProjectManagerWidget::findDeviceModel(QList<QSharedPointer<Kcc::BlockDefinition::Model>> &deviceList,
                                      const QString &deviceName)
{
    for (PModel dev : deviceList) {
        if (dev != nullptr && dev->getName() == deviceName && dev->getModelType() == DeviceModel::Type) {
            return dev.dynamicCast<DeviceModel>();
        }
    }
    return nullptr;
}

void ProjectManagerWidget::setElecDeviceType(QList<QSharedPointer<Kcc::BlockDefinition::Model>> &allBoards,
                                             QList<QSharedPointer<Kcc::BlockDefinition::Model>> &deviceList,
                                             const QString &boardName, const QString &prototypeName,
                                             const QString &elmentsName, const QString &deviceName,
                                             QList<QSharedPointer<Kcc::BlockDefinition::Model>> &needSaveBoards)
{

    PDeviceModel dev = findDeviceModel(deviceList, deviceName);
    if (dev == nullptr) {
        return;
    }
    PModel model = findElecModel(allBoards, boardName, prototypeName, elmentsName);
    if (model == nullptr) {
        return;
    }
    PElectricalBlock elecModel = model.dynamicCast<ElectricalBlock>();
    if (elecModel != nullptr) {
        elecModel->setDeviceModel(dev);
        PModel topModel = elecModel->getTopParentModel();
        if (topModel != nullptr && !needSaveBoards.contains(topModel)) {
            needSaveBoards.append(topModel);
        }
        LOGOUT(QString("电气元件[%1]的设备类型设置为:%2").arg(elecModel->getName()).arg(dev->getName()), LOG_NORMAL);
    }
}

void ProjectManagerWidget::onManagerClosed(int btn) { }

void ProjectManagerWidget::onTreeClicked(const QModelIndex &index)
{
    updateAllActionSts(index);
    updateTableData(index);
}

void ProjectManagerWidget::onTreeContextMenu(const QPoint &point)
{
    QModelIndex curindex = ui.treeView->indexAt(point);
    if (!curindex.isValid()) { // 右键空白处，不显示菜单
        return;
    }
    ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerTree);
    updateTreeMenuSts(curindex);
    if (ProjActionManager::getInstance().getMenu(ProjActionManager::MenuType_Tree) != nullptr) {
        ProjActionManager::getInstance().getMenu(ProjActionManager::MenuType_Tree)->exec(cursor().pos());
    }
    ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerCommon);
}

void ProjectManagerWidget::onActionTriggered()
{
    if (AssistantDefine::WidgetObject_DataManagerTree != ProjActionManager::getInstance().getActTriggerObj()
        && AssistantDefine::WidgetObject_DataManagerTable != ProjActionManager::getInstance().getActTriggerObj()
        && AssistantDefine::WidgetObject_DataManagerCommon != ProjActionManager::getInstance().getActTriggerObj()) {
        return;
    }
    QAction *action = dynamic_cast<QAction *>(sender());
    QModelIndex curindex = ui.treeView->currentIndex();
    if (nullptr == action || !curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    QVariant var = action->data();
    ProjActionManager::ActionType type = var.value<ProjActionManager::ActionType>();
    switch (type) {
    case ProjActionManager::Act_Undo: {
        if (m_pUndoStack != nullptr) {
            m_pUndoStack->undo();
        }
        return;
    }
    case ProjActionManager::Act_Redo: {
        if (m_pUndoStack != nullptr) {
            m_pUndoStack->redo();
        }
        return;
    }
    default:
        break;
    }

    if (AssistantDefine::WidgetObject_DataManagerTree == ProjActionManager::getInstance().getActTriggerObj()) {
        onActionTreeTriggered();
    } else if (AssistantDefine::WidgetObject_DataManagerTable == ProjActionManager::getInstance().getActTriggerObj()) {
        onActionTableTriggered();
    } else if (AssistantDefine::WidgetObject_DataManagerCommon == ProjActionManager::getInstance().getActTriggerObj()) {
        if (ui.tableView->getCurrentSelectedIndexs().size() > 0
            && ui.tableView->getTableMenuEnableList().contains(type)) {
            onActionTableTriggered();
        } else {
            onActionTreeTriggered();
        }
    }
    ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerCommon);
}

void ProjectManagerWidget::onModelValueChanged(const QList<OldNewDataStruct> &modifylist)
{
    m_pUndoStack->push(new TableUndoCommand(m_pTableModel, modifylist, false));
}

void ProjectManagerWidget::onPropertyManagerServerMsg(unsigned int code, const NotifyStruct &structinfo)
{
    QString modelUUID = structinfo.paramMap[PMKEY::MODEL_UUID].toString();
    int modelType = structinfo.paramMap[PMKEY::MODEL_TYPE].toInt();
    if (modelUUID.isEmpty()) {
        return;
    }
    switch (code) {
    case Notify_CreateBlockInstance:
    case Notify_CreateComplexInstance:
    case Notify_CreateDeviceType:
    case Notify_DrawingBoardCreate:
        updateUI(getTreeCurrIndexNames());
        break;
    case Notify_EditDeviceType: {
        onVerifyDrawBoard(modelUUID, modelType);
    }
    case Notify_BlockPropertyChanged: {
        if (ElectricalBlock::Type == modelType) {
            onVerifyDrawBoard(modelUUID, modelType);
        }
    }
    default:
        break;
    }
}

void ProjectManagerWidget::onReceiveProjectManagerServerMsg(uint code, const NotifyStruct &param)
{
    if (IPM_Notify_DrawingBoardCreate == code || IPM_Notify_DrawingBoardDestroyed == code
        || IPM_Notify_DrawingBoardSaved == code || IPM_Notify_DrawingBoardFileRenamed == code
        || IPM_Notify_CodePrototypeDestroyed == code || IPM_Notify_ConfigInfoChange == code) {
        updateUI(getTreeCurrIndexNames());
    } else if (IPM_Notify_CodePrototypeSaved == code) {
        updateUI(getTreeCurrIndexNames());
        connectModel(getProjectModelByName(param.paramMap["name"].toString()));
    }
    if (IPM_Notify_RefreshDatadictionary == code) {
        if (m_pProjDataManager == nullptr) {
            return;
        }
        PKLProject curproject = m_pProjDataManager->getCurrentProject();
        QString boardUuid = param.paramMap["boardUuid"].toString();
        if (boardUuid.isEmpty() || curproject == nullptr) {
            return;
        }
        PModel findModel(nullptr);
        QList<PModel> boardModels = curproject->getAllBoardModel();
        for (PModel model : boardModels) {
            if (model != nullptr && model->getUUID() == boardUuid) {
                findModel = model;
                break;
            }
        }
        if (findModel == nullptr) {
            return;
        }

        if (ElecBoardModel::Type == findModel->getModelType()) {
            QVariantMap boardactivemap;
            if (curproject->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
                boardactivemap = curproject->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
            }
            if (boardactivemap.contains(findModel->getUUID())) {
                AssistantDefine::deleteModelVarToDict(findModel);
                AssistantDefine::addModelVarToDict(findModel);
            }
        } else if (ControlBoardModel::Type == findModel->getModelType()) {
            AssistantDefine::deleteModelVarToDict(findModel);
            AssistantDefine::addModelVarToDict(findModel);
        }
    }
}

void ProjectManagerWidget::onTableMenuEnd()
{
    if (ui.treeView != nullptr) {
        ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerCommon);
        updateAllActionSts(ui.treeView->currentIndex());
    }
}

void ProjectManagerWidget::onSelectedRowsChanged(const int &totalselectrow)
{
    ManagerCommonWidget::onSelectedRowsChanged(totalselectrow);
    if (ui.treeView != nullptr) {
        updateAllActionSts(ui.treeView->currentIndex());
    }
}

void ProjectManagerWidget::onActionTreeTriggered()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    QModelIndex curindex = ui.treeView->currentIndex();
    if (nullptr == action || !curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    CustomTreeModel::NodeType itemnodetype =
            CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());
    QVariant var = action->data();
    ProjActionManager::ActionType type = var.value<ProjActionManager::ActionType>();
    switch (type) {
    case ProjActionManager::Act_Up: {
        onTreeLevelUpDown(true);
    } break;
    case ProjActionManager::Act_Down: {
        onTreeLevelUpDown(false);
    } break;
    case ProjActionManager::Act_Refresh: {
        if (m_pTimer != nullptr) {
            m_treeModel->saveTreeState(ui.treeView);
            m_treeModel->clear();
            m_pTimer->start(25);
        }
    } break;
    case ProjActionManager::Act_open: {
        onOpen();
    } break;
    case ProjActionManager::Act_active: {
        onActiveClicked();
    } break;
    case ProjActionManager::Act_record: {
        onRecord();
    } break;
    case ProjActionManager::Act_cut: {
        onCut();
    } break;
    case ProjActionManager::Act_copy: {
        onCopy();
    } break;
    case ProjActionManager::Act_paste: {
        onPaste();
    } break;
    case ProjActionManager::Act_edit: {
        onEdit();
    } break;
    case ProjActionManager::Act_delete: {
        if (CustomTreeModel::NodeType_Workspace == itemnodetype) {
            onDeleteWorkSpace();
        } else if (CustomTreeModel::NodeType_Project == itemnodetype) {
            onDeleteProj();
        } else if (CustomTreeModel::NodeType_Model == itemnodetype
                   || CustomTreeModel::NodeType_ModelInstance == itemnodetype
                   || CustomTreeModel::NodeType_CodeBlockClass == itemnodetype) {
            onDeleteModel();
        } else if (CustomTreeModel::NodeType_DeviceTypeClass == itemnodetype) {
            onDeleteDeviceModel();
        } else if (CustomTreeModel::NodeType_SimuResultSubItem == itemnodetype) {
            onDeleteSimuResult();
        }
    } break;
    case ProjActionManager::Act_rename: {
        onRename();
    } break;
    case ProjActionManager::Act_new: {
        if (CustomTreeModel::NodeType_DataRoot == itemnodetype) {
            onCreateWorkSpace();
        } else if (CustomTreeModel::NodeType_Workspace == itemnodetype) {
            onCreateProj();
        } else if (CustomTreeModel::NodeType_DeviceTypeClass == itemnodetype) {
            onCreateDeviceType();
        } else if (CustomTreeModel::NodeType_Network == itemnodetype) {
            onCreateModel(PMKEY::BoardTypeElectrical);
        } else if (CustomTreeModel::NodeType_CombinedModel == itemnodetype) {
            onCreateModel(PMKEY::BoardTypeCombine);
        } else if (CustomTreeModel::NodeType_ComplexModel == itemnodetype) {
            onCreateModel(PMKEY::BoardTypeComplex);
        } else if (CustomTreeModel::NodeType_CodeBlock == itemnodetype) {
            onCreateCodeTemplate();
        } else if (CustomTreeModel::NodeType_GlobalDeviceType == itemnodetype) {
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
            if (ProjectViewServers::getInstance().m_wizardServer == nullptr || m_pProjDataManager == nullptr) {
                return;
            }
            QStringList indexNames = getTreeCurrIndexNames();
            PModel devmodel = ProjectViewServers::getInstance().m_wizardServer->NewDeviceWizardDialog();
            PKLProject sysProject = m_pProjDataManager->getSystemProject();
            if (devmodel != nullptr && sysProject != nullptr) {
                sysProject->saveModel(devmodel);
                connectModel(devmodel);
                updateUI(indexNames);
            }
#endif
        }

    } break;
    case ProjActionManager::Act_newControlBoard: {
        onCreateModel(PMKEY::BoardTypeControl);
    } break;
    case ProjActionManager::Act_newElecCombine: {
        onCreateModel(PMKEY::BoardTypeElecCombine);
    } break;
    case ProjActionManager::Act_newComplexInstance: {
        onNewComplexInstance();
    } break;
    case ProjActionManager::Act_newBlockInstance: {
        if (CustomTreeModel::NodeType_CodeBlockClass == itemnodetype
            || (CustomTreeModel::NodeType_Model == itemnodetype && curindex.parent().isValid()
                && CustomTreeModel::NodeType_CombinedModel
                        == curindex.parent().data(CustomTreeModel::CustomRole_NodeType).toInt())) {
            onNewBlockInstance(curindex.data().toString());
        } else {
            onNewBlockInstance();
        }
    } break;
    case ProjActionManager::Act_import: {
        if (CustomTreeModel::NodeType_DataRoot == itemnodetype) {
            onFileImportWorkSpace();
        } else if (CustomTreeModel::NodeType_Workspace == itemnodetype) {
            onImportProj(curindex.data().toString());
        }
    } break;
    case ProjActionManager::Act_export: {
        if (CustomTreeModel::NodeType_Workspace == itemnodetype) {
            onExportWorkSpace(curindex.data().toString());
        } else if (CustomTreeModel::NodeType_Project == itemnodetype) {
            onExportProj();
        } else if (CustomTreeModel::NodeType_SimuResultSubItem == itemnodetype) {
            onExportSimuResult();
        }
    } break;
    case ProjActionManager::Act_settings: {
        onSettings();
    } break;
    case ProjActionManager::Act_SortProjByName: {
        m_treeModel->setProjectSortType(CustomTreeModel::ProjectSortType_Name, ui.treeView);
    } break;
    case ProjActionManager::Act_SortProjByTime: {
        m_treeModel->setProjectSortType(CustomTreeModel::ProjectSortType_Time, ui.treeView);
    } break;
    default:
        break;
    }
}

void ProjectManagerWidget::onActionTableTriggered()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    QModelIndex curindex = ui.tableView->currentIndex();
    QModelIndexList selectIndexs = ui.tableView->getCurrentSelectedIndexs();
    if (nullptr == action || !curindex.isValid() || m_pTableModel == nullptr) {
        return;
    }
    QVariant var = action->data();
    ProjActionManager::ActionType type = var.value<ProjActionManager::ActionType>();
    PKLProject currProject = getCurrentTreeProject();
    if (currProject == nullptr) {
        return;
    }
    switch (type) {
    case ProjActionManager::Act_BatchModify: {
        onTableBatchModify();
    } break;
    case ProjActionManager::Act_copy: {
        onTableCopy();
        ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerCommon);
        updateAllActionSts(ui.treeView->currentIndex());
    } break;
    case ProjActionManager::Act_paste: {
        onTablePaste();
        ProjActionManager::getInstance().setActTriggerObj(AssistantDefine::WidgetObject_DataManagerCommon);
        updateAllActionSts(ui.treeView->currentIndex());
    } break;
    case ProjActionManager::Act_open: {
        onTableOpenBlock();
    } break;
    case ProjActionManager::Act_new: {
        onTableNew();
    } break;
    case ProjActionManager::Act_delete: {
        onTableDelete();
    } break;
    case ProjActionManager::Act_edit: {
        onTableEdit();
    } break;
    default: {
        break;
    }
    }
}

void ProjectManagerWidget::onTimerOut()
{
    if (m_treeModel == nullptr || m_pTimer == nullptr) {
        return;
    }

    m_pTimer->stop();
    m_treeModel->updateTree(ui.treeView);
    m_treeModel->loadTreeState(ui.treeView);
    onTreeClicked(ui.treeView->currentIndex());
}

void ProjectManagerWidget::onCreateWorkSpace()
{
    KInputDialog *inputDialog = new KInputDialog(this);
    inputDialog->setTitle(AssistantDefine::STR_ACT_NEW);
    inputDialog->setLabelText(QObject::tr("Please input %1").arg(AssistantDefine::KEYWORD_TITLE_WSNAME));
    inputDialog->setTextValue("");
    inputDialog->lineEdit()->setValidator(new QRegExpValidator(QRegExp(PROJDATAMNGKEY::REG_WORKSPACENAME)));
    if (inputDialog->exec() != KBaseDlgBox::Ok)
        return;
    QString wsName = inputDialog->textValue().trimmed();

    if (wsName.isEmpty()) {
        LOGOUT(AssistantDefine::ERRORINFO_WSNAMENULL, LOG_WARNING);
        return;
    }
    if (m_pProjDataManager != nullptr && m_pProjDataManager->createWorkspace(wsName)) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_WORKSPACE, wsName,
                                            true),
               LOG_NORMAL);
        updateUI(getTreeCurrIndexNames());
    }
}

void ProjectManagerWidget::onDeleteWorkSpace()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid()) {
        return;
    }
    QString workspacename = curindex.data().toString();
    if (workspacename.isEmpty()) {
        return;
    }
    QStringList indexNames = m_treeModel->getTreeIndexSiblingIndexNames(curindex);
    //"工作空间[" + workspacename+ "]及其所有内部数据均会被删除。\n该操作无法恢复，是否确认删除？"
    if (!questionDelete(tr("Workspace"), workspacename)) {
        return;
    }

    if (m_pProjDataManager != nullptr && m_pProjDataManager->deleteWorkspace(workspacename)) {
        // 删除工作空间[%1]成功！
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_WORKSPACE,
                                            workspacename, true),
               LOG_NORMAL);
        updateUI(indexNames);
    } else {
        // 删除工作空间[%1]失败！
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_WORKSPACE,
                                            workspacename, false),
               LOG_WARNING);
    }
}

void ProjectManagerWidget::onExportWorkSpace(const QString &workspacename)
{
    if (workspacename.isEmpty() || m_pProjDataManager == nullptr)
        return;
    //"选择导出的目录"
    QString dir = QFileDialog::getExistingDirectory(AssistantDefine::getMainWindow(),
                                                    AssistantDefine::TITLE_SELECTDIRECCTORY, "");

    if (dir.isEmpty()) {
        return;
    }
    m_pProjDataManager->exportWorkSpace(workspacename, dir);
}
void ProjectManagerWidget::onSettings()
{
    ProjectViewServers::getInstance().m_pPropertyManagerServer->ShowProjectProperty();
}
void ProjectManagerWidget::onImportDeviceCSV()
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr || m_pProjDataManager == nullptr
        || m_pProjDataManager->getCurrentProject() == nullptr) {
        LOGOUT(QString("no active project"), LOG_ERROR);
        return;
    }

    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Excel file"), qApp->applicationDirPath(), tr("Files (*.csv)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOGOUT(QString("Cannot open file for reading: %1").arg(fileName), LOG_ERROR);
        return;
    }
    QStringList list;
    list.clear();
    QTextStream in(&file);
    int i = 0;
    QString prototypeName;
    QMap<int, QString> colMapToKey;
    PDeviceModel psysmodel = nullptr;
    QStringList existDeviceNames = m_pProjDataManager->getCurrentProject()->getAllModelName(DeviceModel::Type);
    while (!in.atEnd()) {
        QString fileLine = in.readLine();
        list = fileLine.split(",", QString::SkipEmptyParts);
        // 原型名
        if (i == 0 && list.size() > 1) {
            prototypeName = list.at(1);
            psysmodel = ProjectViewServers::getInstance().m_projectManagerServer->GetSystemDeviceModel(prototypeName);
        }
        if (psysmodel == nullptr) {
            LOGOUT(QString("Prototype name is empty"), LOG_ERROR);
            break;
        }
        // 表头
        if (i == 1) {
            for (int col = 0; col < list.size(); ++col) {
                colMapToKey.insert(col, list[col]);
            }
        }
        // 内容
        if (i > 1) {
            if (list.size() != colMapToKey.size()) {
                break;
            }
            PDeviceModel deviceModel = nullptr;
            if (existDeviceNames.contains(list[0])) {
                deviceModel = findDeviceModel(m_pProjDataManager->getCurrentProject()->getDeviceTypeList(prototypeName),
                                              list[0]);
            } else {
                deviceModel = psysmodel->copy().dynamicCast<DeviceModel>();
            }
            if (deviceModel == nullptr) {
                LOGOUT(QString("新增设备类型数据:%1与其他设备类型名称重复").arg(list[0]), LOG_WARNING);
                break;
            }
            for (int col = 0; col < colMapToKey.size(); ++col) {
                if (0 == col) {
                    deviceModel->setName(list[col]);
                } else {
                    deviceModel->setValueByRoleData(RoleDataDefinition::DeviceTypeParameter,
                                                    RoleDataDefinition::NameRole, colMapToKey[col], list[col]);
                }
            }
            deviceModel->setModifyTime(QDateTime::currentDateTime());
            if (!existDeviceNames.contains(list[0])) {
                ProjectViewServers::getInstance().m_projectManagerServer->AddDeviceModel(deviceModel);
                existDeviceNames.append(deviceModel->getName());
                LOGOUT(QString("新增设备类型数据:%1").arg(deviceModel->getName()), LOG_NORMAL);
            } else {
                LOGOUT(QString("更新设备类型数据:%1").arg(deviceModel->getName()), LOG_WARNING);
            }
        }
        i++;
    }
    file.close();
    updateUI(getTreeCurrIndexNames());
}

void ProjectManagerWidget::onElementsConnectDeviceTypeCSV()
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr || m_pProjDataManager == nullptr
        || m_pProjDataManager->getCurrentProject() == nullptr) {
        LOGOUT(QString("no active project"), LOG_ERROR);
        return;
    }

    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Excel file"), qApp->applicationDirPath(), tr("Files (*.csv)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOGOUT(QString("Cannot open file for reading: %1").arg(fileName), LOG_ERROR);
        return;
    }
    QList<PModel> allElecBoards = m_pProjDataManager->getCurrentProject()->getAllModel(ElecBoardModel::Type);
    QStringList list;
    list.clear();
    QTextStream in(&file);
    int i = 0;
    int maxTitleCol = 0;
    QString prototypeName; // 电气元件类型
    QString boardName;     // 画板名
    QMap<QString, int> keyMaptoCol;
    QString KEY_ELECMENTS = "Elements";
    QString KEY_PROTOTYPENAME = "PrototypeName";
    QString KEY_DRAWBOARDNAME = "DrawboardName";
    QString KEY_DEVICETYPENAME = "DeviceTypeName";
    QList<PModel> needSaveBoards;
    while (!in.atEnd()) {
        QString fileLine = in.readLine();
        list = fileLine.split(",", QString::SkipEmptyParts);
        // 表头
        if (i == 0) {
            for (int col = 0; col < list.size(); ++col) {
                if (list[col].isEmpty()) {
                    break;
                }
                keyMaptoCol[list[col]] = col;
            }
            maxTitleCol = list.size();
            if (!keyMaptoCol.contains(KEY_ELECMENTS) || !keyMaptoCol.contains(KEY_PROTOTYPENAME)
                || !keyMaptoCol.contains(KEY_DRAWBOARDNAME) || !keyMaptoCol.contains(KEY_DEVICETYPENAME)) {
                LOGOUT(QString("数据格式错误"), LOG_ERROR);
                break;
            }
        }
        // 内容
        if (i > 0) {
            if (list.size() != maxTitleCol) {
                break;
            }
            setElecDeviceType(
                    allElecBoards,
                    m_pProjDataManager->getCurrentProject()->getDeviceTypeList(list[keyMaptoCol[KEY_PROTOTYPENAME]]),
                    list[keyMaptoCol[KEY_DRAWBOARDNAME]], list[keyMaptoCol[KEY_PROTOTYPENAME]],
                    list[keyMaptoCol[KEY_ELECMENTS]], list[keyMaptoCol[KEY_DEVICETYPENAME]], needSaveBoards);
        }
        i++;
    }
    file.close();
    for (PModel board : needSaveBoards) {
        ProjectViewServers::getInstance().m_projectManagerServer->SaveBoardModel(board);
    }
    updateUI(getTreeCurrIndexNames());
}

void ProjectManagerWidget::onCreateProj(const QString &workspacenamestr)
{
    if (m_pProjDataManager == nullptr) {
        return;
    }
    QString realworkspacename = "";
    if (workspacenamestr.isEmpty()) {
        QModelIndex curindex = ui.treeView->currentIndex();
        if (!curindex.isValid() || m_treeModel == nullptr) {
            return;
        }
        CustomTreeModel::NodeType itemnodetype =
                CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());
        if (CustomTreeModel::NodeType_Workspace != itemnodetype) {
            return;
        }
        realworkspacename = curindex.data().toString();
    } else {
        realworkspacename = workspacenamestr;
    }
    if (realworkspacename.isEmpty()) {
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    KInputDialog *inputDialog = new KInputDialog(this);
    inputDialog->setTitle(AssistantDefine::STR_ACT_NEW);
    //"为工作空间[" + realworkspacename + "]新建项目
    inputDialog->setLabelText(QObject::tr("Create a new project for the workspace [%1]").arg(realworkspacename));
    inputDialog->setTextValue("");
    inputDialog->lineEdit()->setValidator(new QRegExpValidator(QRegExp(PROJDATAMNGKEY::REG_WORKSPACENAME)));
    if (inputDialog->exec() != KBaseDlgBox::Ok)
        return;
    QString projectName = inputDialog->textValue().trimmed();

    if (projectName.isEmpty()) {
        LOGOUT(AssistantDefine::ERRORINFO_PROJECTNAMENULL, LOG_WARNING);
        return;
    }
    PKLProject newProject = m_pProjDataManager->createProject(realworkspacename, projectName);
    if (newProject != nullptr) {
        activeProjectHandle(realworkspacename, newProject->getProjectName(), true);
        PDrawingBoardClass pdrawboard = ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDrawBoard(
                PMKEY::BoardTypeElectrical, false);
        PDrawingBoardClass ctrdrawboard = ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDrawBoard(
                PMKEY::BoardTypeControl, false);
        // 新建项目%1成功！
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_PROJECT,
                                            projectName, true),
               LOG_NORMAL);
        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(pdrawboard);
        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(ctrdrawboard);
        connectModel(pdrawboard);
        emit addModel(pdrawboard);
        connectModel(ctrdrawboard);
        emit addModel(ctrdrawboard);
        updateUI(indexNames);
    }
}

void ProjectManagerWidget::onDeleteProj()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    QStringList indexNames = m_treeModel->getTreeIndexSiblingIndexNames(curindex);
    QString workspacename;
    QString projname;
    m_treeModel->getWSProjByIndex(curindex, workspacename, projname);
    if (workspacename.isEmpty() || projname.isEmpty()) {
        // 项目信息为空，无法删除.
        LOGOUT(QObject::tr("The project information is empty and cannot be deleted."), LOG_WARNING);
        return;
    }
    //"项目[" + projname + "]及其所有内部数据均会被删除。该操作无法恢复，是否确认删除？"
    if (!questionDelete(tr("Project"), projname)) {
        return;
    }

    if (m_pProjDataManager != nullptr && m_pProjDataManager->deleteProject(workspacename, projname)) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_PROJECT,
                                            projname, true),
               LOG_NORMAL);
        updateUI(indexNames);
    } else {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_PROJECT,
                                            projname, false),
               LOG_WARNING);
    }
}

void ProjectManagerWidget::onImportProj(const QString &workspace)
{
    if (workspace.isEmpty() || m_pProjDataManager == nullptr)
        return;
    //"选择要导入的项目文件"
    QString filePath = QFileDialog::getOpenFileName(AssistantDefine::getMainWindow(),
                                                    QObject::tr("Select the project file to import"), "", "*.npsdp");

    if (filePath.isEmpty()) {
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    PKLProject importProject = m_pProjDataManager->importProject(workspace, filePath);
    if (importProject != nullptr) {
        connectProject(importProject);
        updateUI(indexNames);
        if (m_pProjDataManager->projectMode() != KLProject::ProjectMode_DesignerExterrnal) {
            // 非外部模式导入成功后,弹出窗口提示是否激活项目
            if (KMessageBox::question(this, tr("The project was successfully imported. Activate it or not?"),
                                      KMessageBox::Yes | KMessageBox::No)
                == KMessageBox::Yes) { // 项目导入成功,是否激活?
                activeProjectHandle(workspace, importProject->getProjectName(), true);
            }
        } else {
            // 外部模式导入成功后,直接激活
            activeProjectHandle(workspace, importProject->getProjectName(), true);
        }
    }
}

void ProjectManagerWidget::onExportProj(bool bActive)
{
    if (m_pProjDataManager == nullptr) {
        return;
    }

    PKLProject pCurProject = PKLProject(nullptr);
    if (bActive) {
        pCurProject = m_pProjDataManager->getCurrentProject();
    } else {
        QModelIndex curindex = ui.treeView->currentIndex();
        if (!curindex.isValid() || m_treeModel == nullptr) {
            return;
        }
        pCurProject = m_treeModel->getProjectByIndex(curindex);
    }

    if (pCurProject == nullptr) {
        //"无已激活项目，无法导出"
        LOGOUT(QObject::tr("No activated projects, unable to export!"), LOG_WARNING);
        return;
    }

    QString dir = QFileDialog::getExistingDirectory(AssistantDefine::getMainWindow(),
                                                    AssistantDefine::TITLE_SELECTDIRECCTORY, "");

    if (dir.isEmpty()) {
        return;
    }
    if (m_pProjDataManager->exportProject(pCurProject, dir)) {
        // 导出项目%1成功
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Export, AssistantDefine::LOG_TYPE_PROJECT,
                                            pCurProject->getProjectName(), true),
               LOG_NORMAL);
    } else {
        // // 导出项目%1失败
        // LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Export, AssistantDefine::LOG_TYPE_PROJECT,
        //                                     pCurProject->getProjectName(), false),
        //        LOG_WARNING);
    }
}

void ProjectManagerWidget::onExportSimuResult()
{
    if (ui.treeView == nullptr || m_treeModel == nullptr) {
        return;
    }
    QModelIndex currentIndex = ui.treeView->currentIndex();
    if (!currentIndex.isValid()
        || CustomTreeModel::NodeType_SimuResultSubItem
                != currentIndex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return;
    }
    PKLProject project = m_treeModel->getProjectByIndex(currentIndex);
    if (project == nullptr) {
        return;
    }
    QString dir = QFileDialog::getExistingDirectory(AssistantDefine::getMainWindow(),
                                                    AssistantDefine::TITLE_SELECTDIRECCTORY, "");
    if (dir.isEmpty()) {
        return;
    }
    SimulationResultInfo simuinfo = m_treeModel->getSimuResultInfo(currentIndex);
    if (project->exportSimulationData(simuinfo.name, dir)) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Export, AssistantDefine::LOG_TYPE_SIMULATIONRESULT,
                                            currentIndex.data().toString(), true),
               LOG_NORMAL);
    } else {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Export, AssistantDefine::LOG_TYPE_SIMULATIONRESULT,
                                            currentIndex.data().toString(), false),
               LOG_WARNING);
    }
}

void ProjectManagerWidget::onCreateModel(const QString &boardType)
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    QSharedPointer<KLProject> pproject = m_treeModel->getProjectByIndex(curindex);
    CustomTreeModel::NodeType itemnodetype =
            CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());

    if (pproject == nullptr) {
        //"没有创建项目，无法创建模型！"
        LOGOUT(QObject::tr("Project is empty, unable to create model!"), LOG_WARNING);
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    PDrawingBoardClass pDrawboard =
            ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDrawBoard(boardType);
    if (pDrawboard != nullptr) {
        // 新建模型[1]
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_MODEL,
                                            pDrawboard->getName(), true),
               LOG_NORMAL);
        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(pDrawboard);
        updateUI(indexNames);
        connectModel(pDrawboard);
        emit addModel(pDrawboard);
    }
    // KInputDialog *inputDialog = new KInputDialog(this);
    // inputDialog->setTitle("新建");
    // inputDialog->setLabelText("为项目[" + QFileInfo(pproject->getProjectPath()).baseName() + "]新建模型");
    // inputDialog->setTextValue("");
    // inputDialog->lineEdit()->setValidator(new QRegExpValidator(QRegExp(REG_WORKSPACENAME)));
    // if (inputDialog->exec() != KBaseDlgBox::Ok)
    //     return;
    // QString modelname = inputDialog->textValue().trimmed();

    // if (modelname.isEmpty()) {
    //     LOGOUT("输入不能为空", LOG_WARNING);
    //     return;
    // }
    // QList<PModel> allmodels = pproject->getAllModel();
    // for (PModel pmod: allmodels) {
    //     if (pmod != nullptr && modelname == pmod->getName()) {
    //         LOGOUT(QString("已经存在名为[%1]的模型，无法创建").arg(modelname), LOG_WARNING);
    //         return;
    //     }
    // }
    // if (modelname.isEmpty()) {
    //     LOGOUT("输入不能为空", LOG_WARNING);
    //     return;
    // }
    // // fixme创建不同model
    // //  if (CustomTreeModel::NodeType_CombinedModel == itemnodetype) {

    // // } else if (CustomTreeModel::NodeType_ComplexModel == itemnodetype) {

    // // } else if (CustomTreeModel::NodeType_Network == itemnodetype) {

    // // }
    // PModel tmpmodel = pproject->creatModel(modelname, DrawingBoardClass::Type);
    // if (tmpmodel != nullptr) {
    //     updateUI();
    // } else {
    //     LOGOUT(QString("新建模型%1失败").arg(modelname), LOG_WARNING);
    // }
}

void ProjectManagerWidget::onCreateCodeTemplate()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr || ProjectViewServers::getInstance().m_wizardServer == nullptr) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(curindex);
    PKLProject project = m_treeModel->getProjectByIndex(curindex);
    if (project.isNull()) {
        return;
    }

    // 模块代码编译文件路径
    QString path = project->getProjectDir() + "/ComponentsBin/";
    QDir isExist(path);
    if (!isExist.exists()) {
        isExist.mkdir(path);
    }
    PModel model = ProjectViewServers::getInstance().m_wizardServer->NewWizardDialog(path);
    if (model != nullptr) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_MODEL,
                                            model->getName(), true),
               LOG_NORMAL);
        project->saveModel(model);
        updateUI(indexNames);
        connectModel(model);
        emit addModel(model);
    }
}

void ProjectManagerWidget::onDeleteModel()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr || ui.treeView == nullptr
        || ProjectViewServers::getInstance().m_pGraphicModelingServer == nullptr
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    QStringList targetIndexNames = m_treeModel->getTreeIndexSiblingIndexNames(curindex);
    QSharedPointer<KLProject> project = m_treeModel->getProjectByIndex(curindex);
    if (project == nullptr) {
        return;
    }
    QList<PModel> curProjectModels;
    QStringList modelNameList;
    for (QModelIndex index : getTreeSelectedIndexes()) {
        if (!index.isValid() || m_treeModel->getProjectByIndex(index) != project) {
            continue;
        }
        PModel model = m_treeModel->getModel(index);
        if (model == nullptr) {
            continue;
        }
        curProjectModels.append(model);
        modelNameList.append(model->getName());
    }
    if (curProjectModels.size() <= 0) {
        return;
    }
    QString ModelStr;
    if (modelNameList.size() <= 4) {
        ModelStr = modelNameList.join(" & ");
    } else {
        ModelStr = tr("%1 etc. %2 models").arg(modelNameList[0]).arg(QString::number(modelNameList.size()));
    }

    // 模型[" + pmodel->getName()+ "]及其所有内部数据均会被删除。\n该操作无法恢复，是否确认删除？",
    if (!questionDelete(tr("Model"), ModelStr)) {
        return;
    }

    bool bdeleteModel = false;
    for (PModel model : curProjectModels) {
        if (deleteProjectModel(project, model)) {
            bdeleteModel = true;
        }
    }
    if (bdeleteModel) {
        updateUI(targetIndexNames);
        emit deleteModel();
    }
}

void ProjectManagerWidget::onDeleteDeviceModel()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    QStringList targetIndexNames = m_treeModel->getTreeIndexSiblingIndexNames(curindex);
    QString prototypeName = curindex.data(CustomTreeModel::CustomRole_PrototypeName).toString();
    PKLProject curProject = m_treeModel->getProjectByIndex(curindex);
    if (prototypeName.isEmpty() || curProject == nullptr) {
        return;
    }
    if (!questionDelete(AssistantDefine::DEVICETYPE_NAME, curindex.data().toString())) {
        return;
    }
    bool bchange = false;
    QList<PModel> deviceModels = curProject->getAllModel(DeviceModel::Type);
    for (PModel model : deviceModels) {
        if (model == nullptr) {
            continue;
        }
        PDeviceModel pdevModel = model.dynamicCast<DeviceModel>();
        if (pdevModel == nullptr || prototypeName != pdevModel->getDeviceTypePrototypeName()) {
            continue;
        }
        QString modelName = model->getName();
        if (curProject->deleteModel(model)) {
            emit deleteModel();
            onVerifyDrawBoard(model->getUUID(), model->getModelType());
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                                modelName, true),
                   LOG_NORMAL);
            bchange = true;
        } else {
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                                modelName, false),
                   LOG_WARNING);
        }
    }
    if (bchange) {
        updateUI(targetIndexNames);
    }
}

void ProjectManagerWidget::onDeleteSimuResult()
{
    if (ui.treeView == nullptr || m_treeModel == nullptr) {
        return;
    }
    QModelIndex currentIndex = ui.treeView->currentIndex();
    if (!currentIndex.isValid()
        || CustomTreeModel::NodeType_SimuResultSubItem
                != currentIndex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return;
    }
    QStringList targetIndexNames = m_treeModel->getTreeIndexSiblingIndexNames(currentIndex);
    PKLProject project = m_treeModel->getProjectByIndex(currentIndex);
    SimulationResultInfo simuInfo = m_treeModel->getSimuResultInfo(currentIndex);
    if (project == nullptr || simuInfo.name.isEmpty()) {
        return;
    }
    if (!questionDelete(AssistantDefine::RESULT_NAME, simuInfo.name)) {
        return;
    }
    if (ProjectViewServers::getInstance().m_MonitorPanelPlotServer != nullptr) {
        ProjectViewServers::getInstance().m_MonitorPanelPlotServer->removeBoardVar(QStringList() << simuInfo.name);
    }

    if (project->clearSimulationData(simuInfo.name)) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_SIMULATIONRESULT,
                                            currentIndex.data().toString(), true),
               LOG_NORMAL);
        updateUI(targetIndexNames);
    } else {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_SIMULATIONRESULT,
                                            currentIndex.data().toString(), false),
               LOG_WARNING);
    }
}

void ProjectManagerWidget::onCreateDeviceType()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr
        || ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(curindex);

    PDeviceModel devModel = ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDeviceModel(
            curindex.data(CustomTreeModel::CustomRole_PrototypeName).toString());
    if (devModel != nullptr) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                            devModel->getName(), true),
               LOG_NORMAL);
        updateUI(indexNames);
        connectModel(devModel);
        emit addModel(devModel);
    }
}

void ProjectManagerWidget::onNewComplexInstance()
{
    if (ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr || ui.treeView == nullptr) {
        return;
    }
    QModelIndex curIndex = ui.treeView->currentIndex();
    if (!curIndex.isValid()) {
        return;
    }
    QString cpxProtoName = "";
    if (CustomTreeModel::NodeType_Model == curIndex.data(CustomTreeModel::CustomRole_NodeType).toInt()
        && curIndex.parent().isValid()
        && CustomTreeModel::NodeType_ComplexModel
                == curIndex.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        cpxProtoName = curIndex.data().toString();
    }
    QStringList indexNames = getTreeCurrIndexNames();
    PModel model = ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateComplexInstance(cpxProtoName);
    if (model != nullptr) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_COMPLEXINSTANCE,
                                            model->getName(), true),
               LOG_NORMAL);
        updateUI(indexNames);
        connectModel(model);
        emit addModel(model);
    }
}

void ProjectManagerWidget::onNewBlockInstance(const QString &prototypeName)
{
    if (ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr) {
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    PModel instancemodel =
            ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateBlockInstance(prototypeName);
    if (instancemodel != nullptr) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_MODELINSTANCE,
                                            instancemodel->getName(), true),
               LOG_NORMAL);
        updateUI(indexNames);
        connectModel(instancemodel);
        emit addModel(instancemodel);
    }
}

void ProjectManagerWidget::onOpen()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    PKLProject project = m_treeModel->getProjectByIndex(curindex);
    if (project == nullptr) {
        return;
    }
    if (CustomTreeModel::NodeType_SimuResultSubItem == curindex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        SimulationResultInfo simuInfo = m_treeModel->getSimuResultInfo(curindex);
        ProjectViewServers::getInstance().m_projectManagerServer->OpenQUIKIS(
                project->getProjectDir() + "/" + project->getSimulationDataDirName() + "/" + simuInfo.name);
    } else {
        PModel pmod = m_treeModel->getModel(curindex);
        if (pmod == nullptr) {
            return;
        }
        PDrawingBoardClass pdrawboard = pmod.dynamicCast<DrawingBoardClass>();
        if (pdrawboard->isInstance()) {
            PModel protoModel = getProtoModel(project, pdrawboard);
            if (protoModel != nullptr) {
                ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(
                        protoModel.dynamicCast<DrawingBoardClass>());
                emit openBlockOrBoard();
            }
            return;
        }
        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(pdrawboard);
        emit openBlockOrBoard();
    }
}

void ProjectManagerWidget::onEdit()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(curindex);
    PModel model = m_treeModel->getModel(curindex);
    if (model == nullptr) {
        return;
    }
    QString oldName = model->getName();
    bool change = false;
    if (CustomTreeModel::NodeType_CodeBlockClass == curindex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        PKLProject project = m_treeModel->getProjectByIndex(curindex);
        if (project != nullptr && ProjectViewServers::getInstance().m_wizardServer != nullptr
            && ProjectViewServers::getInstance().m_wizardServer->ChangeWizardDialog(
                    model, project->getProjectDir() + "/ComponentsBin/")) {
            project->saveModel(model);
            change = true;
        }
    } else {
        change = EditModel(model);
    }
    if (change) {
        if (oldName != model->getName()) {
            indexNames.pop_back();
            indexNames.append(model->getName());
        }
        updateUI(indexNames);
    }
}

void ProjectManagerWidget::onRename()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr || m_pProjDataManager == nullptr) {
        return;
    }
    CustomTreeModel::NodeType nodeType =
            CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());
    KInputDialog *inputDialog = new KInputDialog(this);
    QString inputName = "";
    bool clickedOk = false;
    if (CustomTreeModel::NodeType_Project == nodeType) {
        clickedOk = OpenInputDialog(AssistantDefine::STR_ACT_RENAME, QObject::tr("Please input project name"),
                                    curindex.data().toString(), inputName);
    } else if (CustomTreeModel::NodeType_Workspace == nodeType) {
        clickedOk = OpenInputDialog(AssistantDefine::STR_ACT_RENAME, QObject::tr("Please input workspace name"),
                                    curindex.data().toString(), inputName);
    } else if (CustomTreeModel::NodeType_SimuResultSubItem == nodeType) {
        clickedOk =
                OpenInputDialog(AssistantDefine::STR_ACT_RENAME, QObject::tr("Please input simulation result new name"),
                                curindex.data().toString(), inputName);
    } else if (CustomTreeModel::NodeType_Model == nodeType || CustomTreeModel::NodeType_ModelInstance == nodeType) {
        clickedOk = OpenInputDialog(AssistantDefine::STR_ACT_RENAME, QObject::tr("Please input model new name"),
                                    curindex.data().toString(), inputName, NPS::REG_FILE_NAME);
    }
    if (!clickedOk) {
        return;
    }

    if (inputName.isEmpty()) {
        if (CustomTreeModel::NodeType_Project == nodeType) {
            LOGOUT(AssistantDefine::ERRORINFO_PROJECTNAMENULL, LOG_WARNING);
        } else if (CustomTreeModel::NodeType_Workspace == nodeType) {
            LOGOUT(AssistantDefine::ERRORINFO_WSNAMENULL, LOG_WARNING);
        } else if (CustomTreeModel::NodeType_SimuResultSubItem == nodeType) {
            LOGOUT(AssistantDefine::ERRORINFO_SIMURESULTNAMENULL, LOG_WARNING);
        } else if (CustomTreeModel::NodeType_Model == nodeType || CustomTreeModel::NodeType_ModelInstance == nodeType) {
            LOGOUT(AssistantDefine::ERRORINFO_MODELNAMENULL, LOG_WARNING);
        }
        return;
    }
    if (inputName == curindex.data().toString()) {
        return;
    }
    QString wsname;
    QString projname;
    m_treeModel->getWSProjByIndex(curindex, wsname, projname);
    QStringList indexNames = m_treeModel->getIndexNames(curindex);
    if (CustomTreeModel::NodeType_Project == nodeType) {
        if (m_pProjDataManager->renameProject(wsname, projname, inputName)) {
            indexNames.pop_back();
            indexNames.append(inputName);
            updateUI(indexNames);
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Rename, AssistantDefine::LOG_TYPE_PROJECT,
                                                projname, true, inputName),
                   LOG_NORMAL);
        }
    } else if (CustomTreeModel::NodeType_Workspace == nodeType) {
        if (m_pProjDataManager->renameWorkspace(wsname, inputName)) {
            indexNames.pop_back();
            indexNames.append(inputName);
            updateUI(indexNames);
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Rename, AssistantDefine::LOG_TYPE_WORKSPACE,
                                                wsname, true, inputName),
                   LOG_NORMAL);
        }
    } else if (CustomTreeModel::NodeType_SimuResultSubItem == nodeType) {
        PKLProject project = m_treeModel->getProjectByIndex(curindex);
        SimulationResultInfo simuInfo = m_treeModel->getSimuResultInfo(curindex);
        if (project == nullptr || simuInfo.name.isEmpty()) {
            return;
        }
        if (project->renameSimulationData(simuInfo.name, inputName)) {
            indexNames.pop_back();
            indexNames.append(inputName);
            updateUI(indexNames);
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Rename,
                                                AssistantDefine::LOG_TYPE_SIMULATIONRESULT, simuInfo.name, true,
                                                inputName),
                   LOG_NORMAL);
        }
    } else if (CustomTreeModel::NodeType_Model == nodeType || CustomTreeModel::NodeType_ModelInstance == nodeType) {
        PModel model = m_treeModel->getModel(curindex);
        if (model == nullptr || ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr) {
            return;
        }
        QString errorinfo;
        if (!ProjectViewServers::getInstance().m_pPropertyManagerServer->checkNameValid(inputName, model, errorinfo)) {
            LOGOUT(errorinfo, LOG_WARNING);
            return;
        }
        QString oldModelName = model->getName();
        if (ControlBoardModel::Type == model->getModelType() || ComplexBoardModel::Type == model->getModelType()
            || ElecBoardModel::Type == model->getModelType()
            || ((CombineBoardModel::Type == model->getModelType()
                 || ElecCombineBoardModel::Type == model->getModelType())
                && model->getParentModel() == nullptr && !model->isInstance())) {
            model->setPrototypeName(inputName);
            model->setPrototypeName_Readable(inputName);
            model->setPrototypeName_CHS(inputName);
        }
        model->setName(inputName);
        indexNames.pop_back();
        indexNames.append(inputName);
        updateUI(indexNames);
        saveModel(model);
        if (ProjectViewServers::getInstance().m_pUIServer->GetMainUI()) {
            ProjectViewServers::getInstance().m_pUIServer->GetMainUI()->RenameWorkAreaTitle(oldModelName, oldModelName,
                                                                                            inputName, inputName);
        }
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Rename, AssistantDefine::LOG_TYPE_MODEL,
                                            oldModelName, true, inputName),
               LOG_NORMAL);
    }
}

void ProjectManagerWidget::onCopy()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    CustomTreeModel::NodeType nodeType =
            CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());
    CustomMineData *cMineData = getCustomMineData(true);
    if (cMineData == nullptr) {
        return;
    }
    cMineData->setOperation(CustomMineData::Operation_Copy);
    if (CustomTreeModel::NodeType_Project == nodeType) {
        cMineData->addKLProject(m_treeModel->getProjectByIndex(curindex));
        QApplication::clipboard()->setMimeData(cMineData);
    } else if (CustomTreeModel::NodeType_Model == nodeType || CustomTreeModel::NodeType_ModelInstance == nodeType) {
        cMineData->addModels(m_treeModel->getProjectByIndex(curindex),
                             QList<PModel>() << m_treeModel->getModel(curindex));
        QApplication::clipboard()->setMimeData(cMineData);
    } else if (CustomTreeModel::NodeType_DeviceTypeClass == nodeType) {
        cMineData->addModels(m_treeModel->getProjectByIndex(curindex), m_treeModel->getModels(curindex));
        QApplication::clipboard()->setMimeData(cMineData);
    } else if (CustomTreeModel::NodeType_CodeBlockClass == nodeType) {
        cMineData->addModels(m_treeModel->getProjectByIndex(curindex),
                             QList<PModel>() << m_treeModel->getModel(curindex));
        QApplication::clipboard()->setMimeData(cMineData);
    }
}

void ProjectManagerWidget::onCut()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return;
    }
    CustomTreeModel::NodeType nodeType =
            CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());
    if (CustomTreeModel::NodeType_Project == nodeType) {
        QStandardItem *item = m_treeModel->itemFromIndex(curindex);
        if (item != nullptr) {
            item->setIcon(QIcon(":/PluginProjectManagerView/resource/TreeIcons/cut.png"));
        }
        CustomMineData *cMineData = getCustomMineData(true);
        if (cMineData == nullptr) {
            return;
        }
        cMineData->setOperation(CustomMineData::Operation_Cut);
        cMineData->addKLProject(m_treeModel->getProjectByIndex(curindex));
        QApplication::clipboard()->setMimeData(cMineData);
    }
}

void ProjectManagerWidget::onPaste()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr || m_pProjDataManager == nullptr) {
        return;
    }
    CustomTreeModel::NodeType nodeType =
            CustomTreeModel::NodeType(curindex.data(CustomTreeModel::CustomRole_NodeType).toInt());
    CustomMineData *cMineData = getCustomMineData();
    if (cMineData == nullptr) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(curindex);
    if (CustomTreeModel::NodeType_Workspace == nodeType) {
        if (cMineData->getKLProject() == nullptr) {
            return;
        }
        QString oldprojectName = cMineData->getKLProject()->getProjectName();
        QString targetWorkspace = m_treeModel->getWorkSpaceNameByIndex(curindex);
        if (CustomMineData::Operation_Cut == cMineData->getOperation()) {
            QStringList oldwsproj = cMineData->getKLProject()->getProjectDir().section('/', -2).split("/");
            QString oldws;
            if (oldwsproj.size() == 2) {
                oldws = oldwsproj[0];
            }
            if (targetWorkspace == oldws) {
                return;
            }
            PKLProject movProject = m_pProjDataManager->moveProject(cMineData->getKLProject(), targetWorkspace);
            if (movProject != nullptr) {
                LOGOUT(QObject::tr("Project [%1] moved from workspace %2 to workspace %3 and named [%4]")
                               .arg(oldprojectName)
                               .arg(oldws)
                               .arg(targetWorkspace)
                               .arg(movProject->getProjectName()),
                       LOG_NORMAL);
                connectProject(movProject);
                updateUI(indexNames);
            }
            cMineData->addKLProject(PKLProject(nullptr));
        } else if (CustomMineData::Operation_Copy == cMineData->getOperation()) {
            PKLProject cpProject = m_pProjDataManager->copyProject(cMineData->getKLProject(), targetWorkspace);
            if (cpProject != nullptr) {
                LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_PROJECT,
                                                    cpProject->getProjectName(), true),
                       LOG_NORMAL);
                connectProject(cpProject);
                updateUI(indexNames);
            } else {
                LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_PROJECT,
                                                    cMineData->getKLProject()->getProjectName(), false),
                       LOG_WARNING);
            }
        }
    } else if (CustomTreeModel::NodeType_CombinedModel == nodeType || CustomTreeModel::NodeType_ComplexModel == nodeType
               || CustomTreeModel::NodeType_Network == nodeType) {
        PKLProject pProject = m_treeModel->getProjectByIndex(curindex);
        QList<PModel> models = cMineData->getModels();
        if (pProject == nullptr || models.size() <= 0) {
            return;
        }
        bool change = false;
        for (PModel srcmodel : models) {
            if (copyModelToProject(pProject, cMineData->getKLProject(), srcmodel)) {
                change = true;
            }
        }
        if (change) {
            updateUI(indexNames);
        }
    } else if (CustomTreeModel::NodeType_DeviceType == nodeType) {
        PKLProject pProject = m_treeModel->getProjectByIndex(curindex);
        QList<PModel> devcieModels = cMineData->getModels(DeviceModel::Type);
        if (pProject == nullptr || devcieModels.size() <= 0) {
            return;
        }
        bool bchange = false;
        for (PModel model : devcieModels) {
            if (model == nullptr) {
                continue;
            }
            PModel cpmodel = pProject->copyModel(cMineData->getKLProject(), model);
            if (cpmodel != nullptr) {
                connectModel(cpmodel);
                emit addModel(cpmodel);
                LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                                    cpmodel->getName(), true),
                       LOG_NORMAL);
                bchange = true;
            } else {
                LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                                    model->getName(), false),
                       LOG_WARNING);
            }
        }
        if (bchange) {
            updateUI(indexNames);
        }
    } else if (CustomTreeModel::NodeType_DeviceTypeClass == nodeType && curindex.parent().isValid()
               && CustomTreeModel::NodeType_DeviceType
                       == curindex.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        PKLProject pProject = m_treeModel->getProjectByIndex(curindex);
        QList<PModel> devcieModels = cMineData->getModels(
                DeviceModel::Type, curindex.data(CustomTreeModel::CustomRole_PrototypeName).toString());
        if (pProject == nullptr || devcieModels.size() <= 0) {
            return;
        }
        bool bchange = false;
        for (PModel model : devcieModels) {
            if (model == nullptr) {
                continue;
            }
            PModel cpmodel = pProject->copyModel(cMineData->getKLProject(), model);
            if (cpmodel != nullptr) {
                connectModel(cpmodel);
                emit addModel(cpmodel);
                LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                                    cpmodel->getName(), true),
                       LOG_NORMAL);
                bchange = true;
            } else {
                LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Copy, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                                    model->getName(), false),
                       LOG_WARNING);
            }
        }
        if (bchange) {
            updateUI(indexNames);
        }
    } else if (CustomTreeModel::NodeType_CodeBlock == nodeType) {
        PKLProject pProject = m_treeModel->getProjectByIndex(curindex);
        QList<PModel> codeModels = cMineData->getModels(ControlBlock::Type);
        if (pProject == nullptr || codeModels.size() <= 0) {
            return;
        }
        bool change = false;
        for (PModel srcmodel : codeModels) {
            if (copyModelToProject(pProject, cMineData->getKLProject(), srcmodel)) {
                change = true;
            }
        }
        if (change) {
            updateUI(indexNames);
        }
    }
}

void ProjectManagerWidget::onActiveClicked()
{
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_pProjDataManager == nullptr || m_treeModel == nullptr) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(curindex);
    QString projname = curindex.data().toString();
    QString workspacename = m_treeModel->getWorkSpaceNameByIndex(curindex);
    bool bactive = curindex.data(CustomTreeModel::CustomRole_ActiveStatus).toBool();
    int nodetype = curindex.data(CustomTreeModel::CustomRole_NodeType).toInt();
    if (CustomTreeModel::NodeType_Project == nodetype) {
        // 激活项目处理
        QApplication::setOverrideCursor(Qt::WaitCursor);
        activeProjectHandle(workspacename, projname, !bactive);
        QTimer::singleShot(0, []() { QApplication::restoreOverrideCursor(); });
    } else if (CustomTreeModel::NodeType_Model == nodetype) {
        PKLProject pProject = m_treeModel->getProjectByIndex(curindex);
        PModel pmod = m_treeModel->getModel(curindex);
        if (pProject == nullptr || pmod == nullptr || ElecBoardModel::Type != pmod->getModelType()
            || pmod->getUUID().trimmed().isEmpty()) {
            return;
        }
        QVariantMap activemap;
        activemap.insert(pmod->getUUID(), !bactive);
        pProject->setProjectConfig(KL_PRO::BOARD_ISACTIVATE, activemap);
        if (!bactive) {
            AssistantDefine::addModelVarToDict(pmod);
        } else {
            AssistantDefine::deleteModelVarToDict(pmod);
        }
        updateUI(indexNames);
        emit updateNetwork();
        if (ProjectViewServers::getInstance().m_MonitorPanelPlotServer != nullptr) {
            ProjectViewServers::getInstance().m_MonitorPanelPlotServer->refreshBindedVarbleInfo(!bactive,
                                                                                                pmod->getName());
        }
    }
}

void ProjectManagerWidget::onRecord()
{
    if (ui.treeView == nullptr || m_treeModel == nullptr) {
        return;
    }
    QModelIndex currentIndex = ui.treeView->currentIndex();
    if (!currentIndex.isValid()
        || CustomTreeModel::NodeType_SimuResultSubItem
                != currentIndex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(currentIndex);
    PKLProject project = m_treeModel->getProjectByIndex(currentIndex);
    if (project == nullptr) {
        return;
    }
    QString inputName = "";
    bool clickedOk = OpenInputDialog(QObject::tr("Save Current Simulation Result"),
                                     QObject::tr("Please input simulation result new name"),
                                     QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"), inputName);
    if (!clickedOk) {
        return;
    }
    if (inputName.isEmpty()) {
        LOGOUT(AssistantDefine::ERRORINFO_SIMURESULTNAMENULL, LOG_WARNING);
        return;
    }
    SimulationResultInfo simuInfo = m_treeModel->getSimuResultInfo(currentIndex);
    if (project->recordSimulationData(simuInfo.name, inputName)) {
        updateUI(indexNames);
    }
}

void ProjectManagerWidget::onTreeLevelUpDown(bool bup)
{
    if (ui.treeView == nullptr) {
        return;
    }

    QModelIndex currentIndex = ui.treeView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }

    if (bup && currentIndex.parent().isValid()) {
        ui.treeView->setCurrentIndex(currentIndex.parent());
    }

    if (!bup && currentIndex.child(0, 0).isValid()) {
        ui.treeView->setCurrentIndex(currentIndex.child(0, 0));
    }

    currentIndex = ui.treeView->currentIndex();
    onTreeClicked(currentIndex);
}

void ProjectManagerWidget::onFileCreateWorkSpace()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot create workspace.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING),
               LOG_WARNING);
        return;
    }
    onCreateWorkSpace();
}

void ProjectManagerWidget::onFileCreateProj()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot create project.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING), LOG_WARNING);
        return;
    }
    QString workspaceName = getCurrentWorkSpaceName(AssistantDefine::STR_ACT_NEWPROJ);
    if (workspaceName.isEmpty()) {
        return;
    }
    onCreateProj(workspaceName);
}

void ProjectManagerWidget::onFileImportWorkSpace()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot import workspace.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING),
               LOG_WARNING);
        return;
    }
    if (m_pProjDataManager == nullptr) {
        return;
    }
    // 选择要导入的工作空间文件"
    QString filePath = QFileDialog::getOpenFileName(AssistantDefine::getMainWindow(),
                                                    QObject::tr("Select the workspace file to import"), "",
                                                    "*" + PROJDATAMNGKEY::ZIP_WORKSPACE_SUFFIX);

    if (filePath.isEmpty()) {
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    WorkSpaceInfo wsinfo = m_pProjDataManager->importWorkSpace(filePath);
    if (wsinfo.isValid()) {
        for (PKLProject project : wsinfo.allprojects.values()) {
            connectProject(project);
        }
        updateUI(indexNames);
    }
}

void ProjectManagerWidget::onFileImportProj()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot import project.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING), LOG_WARNING);
        return;
    }
    QString workspaceName = getCurrentWorkSpaceName(AssistantDefine::TITLE_IMPORT_PROJECT);
    onImportProj(workspaceName);
}
void ProjectManagerWidget::setDemoINI(bool isshowdemo)
{
    QSettings setting(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                      QApplication::applicationName());
    setting.setValue("Locale/ShowDemo", isshowdemo);
}
bool ProjectManagerWidget::getDemoINI()
{
    QSettings setting(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                      QApplication::applicationName());
    return setting.value("Locale/ShowDemo", true).toBool();
}
void ProjectManagerWidget::onFileImportDemoProj()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot import project.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING), LOG_WARNING);
        return;
    }
    QString workspaceName = getCurrentWorkSpaceName(AssistantDefine::TITLE_IMPORT_EXAMPLEPROJECT);
    if (workspaceName.isEmpty() || m_pProjDataManager == nullptr) {
        return;
    }
    QMap<QString, WorkSpaceInfo> allworkspaceMap = m_pProjDataManager->getAllWorkSpace();
    DemoProjectView *projectView = new DemoProjectView();
    projectView->setMessage(AssistantDefine::TITLE_SELECT_EXAMPLEPROJECT);
    // projectView->setNameList(m_pProjDataManager->getAllDemoProjects());
    projectView->setDemoMap(m_pProjDataManager->getCateDemoProjects());
    projectView->setSelectionMode(QAbstractItemView::SingleSelection);
    KCustomDialog *dialog = new KCustomDialog(AssistantDefine::TITLE_SELECT_EXAMPLEPROJECT, projectView,
                                              KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::NoButton, this);
    dialog->resize(projectView->size() + QSize(32, 86));
    // 单选框
    QFrame *frame = new QFrame(dialog);
    QCheckBox *checkbox = new QCheckBox(frame);
    checkbox->setChecked(getDemoINI());
    QHBoxLayout *layout = new QHBoxLayout(frame);
    QLabel *label = new QLabel(frame);
    label->setText(tr("Always pop up when the program starts"));
    layout->addWidget(checkbox);
    layout->addWidget(label);
    dialog->setCornerWidget(frame, Qt::BottomLeftCorner);

    if (dialog->exec() != KBaseDlgBox::Ok)
        return;

    if (checkbox) {
        setDemoINI(checkbox->isChecked());
    }

    QStringList projectNameList = projectView->getSelectNames();
    if (projectNameList.isEmpty()) {
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    bool change = false;
    QList<PKLProject> importProjectList;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for each (QString data in projectNameList) {
        // check name
        QStringList templist = data.split("~");
        QString projectName = templist.first();
        QString englishName = templist.last();
        int n = 1;
        QString nameTemp = QFileInfo(projectName).baseName();
        if ("en_US" == QPHelper::getLanguage()) {
            nameTemp = englishName;
        }
        while (allworkspaceMap[workspaceName].allprojects.contains(nameTemp)) {
            nameTemp = nameTemp + QString("(%1)").arg(n++);
        }
        auto copyProject = m_pProjDataManager->copyDemoProject(projectName, workspaceName, nameTemp);
        if (copyProject) {
            // 导入项目成功
            connectProject(copyProject);
            LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Import, AssistantDefine::LOG_TYPE_PROJECT,
                                                copyProject->getProjectName(), true),
                   LOG_NORMAL);
            change = true;
            importProjectList << copyProject;
        }
    }
    QApplication::restoreOverrideCursor();

    if (change) {
        updateUI(indexNames);
    }
    if (m_pProjDataManager->projectMode() != KLProject::ProjectMode_DesignerExterrnal) {
        // 非外部模式导入示例项目成功后,弹出窗口提示是否激活项目
        if (importProjectList.size() == 1) {
            // NPS 导入单个示例项目成功后,弹出窗口提示是否激活项目
            if (KMessageBox::question(this, tr("The project was successfully imported. Activate it or not?"),
                                      KMessageBox::Yes | KMessageBox::No)
                == KMessageBox::Yes) { // 项目导入成功,是否激活?
                activeProjectHandle(workspaceName, importProjectList.at(0)->getProjectName(), true);
            }
        }
    } else {
        // 外部模式导入示例项目成功后,直接激活项目
        activeProjectHandle(workspaceName, importProjectList.back()->getProjectName(), true);
    }
}

void ProjectManagerWidget::onFileExportWorkSpace()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot export workspace.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING),
               LOG_WARNING);
        return;
    }
    QString workspaceName = getCurrentWorkSpaceName(AssistantDefine::TITLE_EXPORT_WORKSPACE);
    onExportWorkSpace(workspaceName);
}

void ProjectManagerWidget::onFileExportProj()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot export project.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING), LOG_WARNING);
        return;
    }
    onExportProj(true);
}

void ProjectManagerWidget::onMenuAddDrawBoard()
{
    if (m_DrawboardIsRuning) {
        LOGOUT(QObject::tr("%1 cannot create model.").arg(AssistantDefine::ERRORINFO_DRAWBOARDRUNNING), LOG_WARNING);
        return;
    }
    if (ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr
        || ProjectViewServers::getInstance().m_pGraphicModelingServer == nullptr || m_pProjDataManager == nullptr) {
        return;
    }
    if (m_pProjDataManager->getCurrentProject() == nullptr) {
        LOGOUT(QObject::tr("no active project,cannot create model."), LOG_WARNING);
        return;
    }
    QStringList indexNames = getTreeCurrIndexNames();
    PDrawingBoardClass drawboard = ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDrawBoard();
    if (drawboard != nullptr) {
        ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(drawboard);
        connectModel(drawboard);
        updateUI(indexNames);
    }
}

void ProjectManagerWidget::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (ui.treeView != nullptr) {
        updateAllActionSts(ui.treeView->currentIndex());
    }
}

#include "ManagerCommonWidget.h"

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

USE_LOGOUT_("ManagerCommonWidget")

ManagerCommonWidget::ManagerCommonWidget(QSharedPointer<ProjDataManager> pProjDataManager, QWidget *parent)
    : QMainWindow(parent),
      m_horizontalScrollbar(nullptr),
      m_treeModel(nullptr),
      m_pProxyModel(nullptr),
      m_pTableModel(nullptr),
      m_pProjDataManager(pProjDataManager),
      m_pUndoStack(nullptr),
      m_DrawboardIsRuning(false),
      m_syncModelData(nullptr),
      m_isUpdateWidthOrScroll(false)
{
    ui.setupUi(this);
}

ManagerCommonWidget::~ManagerCommonWidget() { }

void ManagerCommonWidget::initData()
{
    // updateUI();
    QModelIndex parentindex = ui.treeView->model()->index(0, 0);
    QModelIndex defaultindex = ui.treeView->model()->index(0, 0, parentindex);
    ui.treeView->setCurrentIndex(defaultindex);
    onTreeClicked(defaultindex);
}

void ManagerCommonWidget::clickedOpen()
{
    if (ui.treeView != nullptr) {
        onTreeClicked(ui.treeView->currentIndex());
    }
}

void ManagerCommonWidget::SyncModelDataToUI()
{
    if (ui.treeView == nullptr || m_treeModel == nullptr) {
        return;
    }
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid()) {
        return;
    }
    QStringList indexNames = m_treeModel->getIndexNames(curindex);
    updateUI(indexNames);
}

QVariantMap ManagerCommonWidget::getTreeStateMap()
{
    if (m_treeModel != nullptr) {
        return m_treeModel->getTreeCurState(ui.treeView);
    }
    return QVariantMap();
}

void ManagerCommonWidget::initUI()
{
    // tree
    m_treeModel = new CustomTreeModel(m_pProjDataManager, ui.treeView);
    ui.treeView->setModel(m_treeModel);
    m_pTableModel = new TableModelDataModel(m_pProjDataManager, ui.tableView);
    m_pTableModel->setCustomTreeModel(m_treeModel);
    m_pProxyModel = new CustomSortFilterProxyModel(ui.tableView);
    m_pProxyModel->setSourceModel(m_pTableModel);
    ui.tableView->setModel(m_pProxyModel);
    m_horizontalScrollbar = new CustomHorizontalScrollBar;
    ui.gridLayout_3->addWidget(m_horizontalScrollbar, 2, 0, 1, 1);
    ui.comboBox->setView(new QListView());
    ui.comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.toolButton->setIcon(QIcon(":/PluginProjectManagerView/resource/Set.png"));
    ui.toolButton->setToolTip(AssistantDefine::TITLE_TABLESET);
    m_rowlabel = new QLabel();
    ui.statusBar->addWidget(m_rowlabel);
    addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Undo));
    addAction(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Redo));
    ui.toolButton->setAccessibleName(AssistantDefine::ACCESSNAME_COLSET);
    ui.comboBox->setAccessibleName(AssistantDefine::ACCESSNAME_FILTERCOL);
    ui.lineEdit->setAccessibleName(AssistantDefine::ACCESSNAME_FILTERCONTENT);
    ui.statusBar->setSizeGripEnabled(false);
}

void ManagerCommonWidget::connectSignals()
{
    if (ui.treeView != nullptr) {
        QObject::connect(ui.treeView, SIGNAL(clicked(const QModelIndex &)), this,
                         SLOT(onTreeClicked(const QModelIndex &)), Qt::UniqueConnection);
        QObject::connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                         SLOT(onTreeContextMenu(const QPoint &)), Qt::UniqueConnection);
    }
    if (m_pTableModel != nullptr) {
        QObject::connect(m_pTableModel, SIGNAL(ModelValueChanged(const QList<OldNewDataStruct> &)), this,
                         SLOT(onModelValueChanged(const QList<OldNewDataStruct> &)), Qt::UniqueConnection);
        QObject::connect(m_pTableModel, SIGNAL(VerifyDrawBoard(const QString &, const int &)), this,
                         SLOT(onVerifyDrawBoard(const QString &, const int &)), Qt::UniqueConnection);
    }
    if (ui.tableView != nullptr) {
        if (ui.tableView->horizontalHeader() != nullptr) {
            QObject::connect(ui.tableView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this,
                             SLOT(onHorizontalHeaderSectionResized(int, int, int)), Qt::UniqueConnection);
        }
        QObject::connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
                         SLOT(onTableItemDoubleClicked(const QModelIndex &)), Qt::UniqueConnection);
        QObject::connect(ui.tableView, SIGNAL(leftMouseDoubleClickedComponent(const QModelIndex &)), this,
                         SLOT(onTableDoubleClicked(const QModelIndex &)), Qt::UniqueConnection);
        QObject::connect(ui.tableView, SIGNAL(selectedRowsChanged(const int &)), this,
                         SLOT(onSelectedRowsChanged(const int &)), Qt::UniqueConnection);
        QObject::connect(ui.tableView, SIGNAL(tableMenuEnd()), this, SLOT(onTableMenuEnd()), Qt::UniqueConnection);
    }
    if (m_horizontalScrollbar != nullptr) {
        QObject::connect(m_horizontalScrollbar, SIGNAL(scrollbarValueChanged(bool, int, int)), this,
                         SLOT(onHorizontalValueChanged(bool, int, int)), Qt::UniqueConnection);
    }
    if (ui.toolButton != nullptr) {
        QObject::connect(ui.toolButton, SIGNAL(released()), this, SLOT(onTableSettingClicked()), Qt::UniqueConnection);
    }
    if (ui.comboBox != nullptr) {
        QObject::connect(ui.comboBox, SIGNAL(currentTextChanged(const QString &)), this,
                         SLOT(onSearchColBoxChanged(const QString &)), Qt::UniqueConnection);
    }
    if (ui.lineEdit != nullptr) {
        QObject::connect(ui.lineEdit, SIGNAL(textChanged(const QString &)), this,
                         SLOT(onSearchTextChanged(const QString &)), Qt::UniqueConnection);
    }
    QObject::connect(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Undo), SIGNAL(triggered()), this,
                     SLOT(onActionTriggered()), Qt::UniqueConnection);
    QObject::connect(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Redo), SIGNAL(triggered()), this,
                     SLOT(onActionTriggered()), Qt::UniqueConnection);
    ProjActionManager::getInstance().connectSignals(this);
    if (ProjectViewServers::getInstance().m_pPropertyManagerServerIF != nullptr) {
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->connectNotify(
                Notify_DrawingBoardCreate, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->connectNotify(
                Notify_CreateBlockInstance, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->connectNotify(
                Notify_CreateComplexInstance, this,
                SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->connectNotify(
                Notify_CreateDeviceType, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->connectNotify(
                Notify_BlockPropertyChanged, this,
                SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->connectNotify(
                Notify_EditDeviceType, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
    }
    if (ProjectViewServers::getInstance().m_pSimulationManagerServer != nullptr) {
        ProjectViewServers::getInstance().m_pSimulationManagerServer->connectNotify(
                Notify_DrawingBoardRunning, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pSimulationManagerServer->connectNotify(
                Notify_DrawingBoardStopped, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    }

    // 接收项目管理服务通知
    if (ProjectViewServers::getInstance().m_projectManagerServerIF != nullptr) {
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_CurrentProjectChanged, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_CurrentProjectDeactivate, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_ConfigInfoChange, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_CodePrototypeSaved, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_CodePrototypeDestroyed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_DeviceTypeDestroyed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_DrawingBoardFileRenamed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_DrawingBoardCreate, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_DrawingBoardSaved, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_DrawingBoardDestroyed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->connectNotify(
                IPM_Notify_RefreshDatadictionary, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    }
}

void ManagerCommonWidget::disconnectSignals()
{
    if (ui.treeView != nullptr) {
        QObject::disconnect(ui.treeView, SIGNAL(clicked(const QModelIndex &)), this,
                            SLOT(onTreeClicked(const QModelIndex &)));
        QObject::disconnect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                            SLOT(onTreeContextMenu(const QPoint &)));
    }
    if (m_pTableModel != nullptr) {
        QObject::disconnect(m_pTableModel, SIGNAL(ModelValueChanged(const QList<OldNewDataStruct> &)), this,
                            SLOT(onModelValueChanged(const QList<OldNewDataStruct> &)));
        QObject::disconnect(m_pTableModel, SIGNAL(VerifyDrawBoard(const QString &, const int &)), this,
                            SLOT(onVerifyDrawBoard(const QString &, const int &)));
    }
    if (ui.tableView != nullptr) {
        if (ui.tableView->horizontalHeader() != nullptr) {
            QObject::disconnect(ui.tableView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this,
                                SLOT(onHorizontalHeaderSectionResized(int, int, int)));
        }
        QObject::disconnect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
                            SLOT(onTableItemDoubleClicked(const QModelIndex &)));
        QObject::disconnect(ui.tableView, SIGNAL(leftMouseDoubleClickedComponent(const QModelIndex &)), this,
                            SLOT(onTableDoubleClicked(const QModelIndex &)));
        QObject::disconnect(ui.tableView, SIGNAL(selectedRowsChanged(const int &)), this,
                            SLOT(onSelectedRowsChanged(const int &)));
        QObject::disconnect(ui.tableView, SIGNAL(tableMenuEnd()), this, SLOT(onTableMenuEnd()));
    }
    if (m_horizontalScrollbar != nullptr) {
        QObject::disconnect(m_horizontalScrollbar, SIGNAL(scrollbarValueChanged(bool, int, int)), this,
                            SLOT(onHorizontalValueChanged(bool, int, int)));
    }
    if (ui.toolButton != nullptr) {
        QObject::disconnect(ui.toolButton, SIGNAL(released()), this, SLOT(onTableSettingClicked()));
    }
    if (ui.comboBox != nullptr) {
        QObject::disconnect(ui.comboBox, SIGNAL(currentTextChanged(const QString &)), this,
                            SLOT(onSearchColBoxChanged(const QString &)));
    }
    if (ui.lineEdit != nullptr) {
        QObject::disconnect(ui.lineEdit, SIGNAL(textChanged(const QString &)), this,
                            SLOT(onSearchTextChanged(const QString &)));
    }

    QObject::disconnect(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Undo), SIGNAL(triggered()),
                        this, SLOT(onActionTriggered()));
    QObject::disconnect(ProjActionManager::getInstance().getAction(ProjActionManager::Act_Redo), SIGNAL(triggered()),
                        this, SLOT(onActionTriggered()));
    ProjActionManager::getInstance().disconnectSignals(this);

    if (ProjectViewServers::getInstance().m_pPropertyManagerServerIF != nullptr) {
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->disconnectNotify(
                Notify_DrawingBoardCreate, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->disconnectNotify(
                Notify_CreateBlockInstance, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->disconnectNotify(
                Notify_CreateComplexInstance, this,
                SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->disconnectNotify(
                Notify_CreateDeviceType, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->disconnectNotify(
                Notify_BlockPropertyChanged, this,
                SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pPropertyManagerServerIF->disconnectNotify(
                Notify_EditDeviceType, this, SLOT(onPropertyManagerServerMsg(unsigned int, const NotifyStruct &)));
    }
    if (ProjectViewServers::getInstance().m_pSimulationManagerServer != nullptr) {
        ProjectViewServers::getInstance().m_pSimulationManagerServer->disconnectNotify(
                Notify_DrawingBoardRunning, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_pSimulationManagerServer->disconnectNotify(
                Notify_DrawingBoardStopped, this, SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    }
    // 接收项目管理服务通知
    if (ProjectViewServers::getInstance().m_projectManagerServerIF != nullptr) {
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_CurrentProjectChanged, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_CurrentProjectDeactivate, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_ConfigInfoChange, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_CodePrototypeSaved, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_CodePrototypeDestroyed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_DeviceTypeDestroyed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_DrawingBoardFileRenamed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_DrawingBoardCreate, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_DrawingBoardSaved, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_DrawingBoardDestroyed, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
        ProjectViewServers::getInstance().m_projectManagerServerIF->disconnectNotify(
                IPM_Notify_RefreshDatadictionary, this,
                SLOT(onReceiveProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    }
}

void ManagerCommonWidget::updateUI(const QStringList &indexNames) { }

void ManagerCommonWidget::clickedIndex(const QStringList &indexNames)
{
    if (ui.treeView == nullptr || m_treeModel == nullptr) {
        return;
    }
    if (indexNames.size() <= 0) {
        QModelIndex curindex = ui.treeView->currentIndex();
        if (!curindex.isValid()) {
            return;
        }
        onTreeClicked(curindex);
    } else {
        QModelIndex selectindex = m_treeModel->getTreeIndex(indexNames);
        if (!selectindex.isValid()) {
            return;
        }
        ui.treeView->setCurrentIndex(selectindex);
        onTreeClicked(selectindex);
    }
}

void ManagerCommonWidget::createSyncDataModel()
{
    if (m_syncModelData != nullptr) {
        delete m_syncModelData;
        m_syncModelData = nullptr;
    }
    m_syncModelData = new SyncModelData(this);
}

void ManagerCommonWidget::connectModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (m_syncModelData == nullptr || model == nullptr) {
        return;
    }
    m_syncModelData->connectModel(model);
}

void ManagerCommonWidget::disconnectModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (m_syncModelData == nullptr || model == nullptr) {
        return;
    }
    m_syncModelData->disconnectModel(model);
}

void ManagerCommonWidget::connectProject(QSharedPointer<KLProject> project)
{
    if (m_syncModelData == nullptr || project == nullptr) {
        return;
    }
    m_syncModelData->connectProjectAllModels(project);
}

void ManagerCommonWidget::disconnectProject(QSharedPointer<KLProject> project)
{
    if (m_syncModelData == nullptr || project == nullptr) {
        return;
    }
    m_syncModelData->disconnectProjectAllModels(project);
}

CustomMineData *ManagerCommonWidget::getCustomMineData(bool isNullCreate)
{
    if (QApplication::clipboard()->mimeData() == nullptr) {
        if (isNullCreate) {
            return new CustomMineData(CustomMineData::Operation_Invalid);
        }
        return nullptr;
    }
    CustomMineData *cMineData = dynamic_cast<CustomMineData *>((QMimeData *)QApplication::clipboard()->mimeData());
    if (cMineData == nullptr) {
        if (isNullCreate) {
            return new CustomMineData(CustomMineData::Operation_Invalid);
        }
        return nullptr;
    }
    return cMineData;
}

QStringList ManagerCommonWidget::getTreeCurrIndexNames()
{
    if (ui.treeView == nullptr || m_treeModel == nullptr) {
        return QStringList();
    }
    return m_treeModel->getIndexNames(ui.treeView->currentIndex());
}

QList<QString> ManagerCommonWidget::getTableHeadList()
{
    QList<QString> tablelist;
    if (ui.tableView->model() == nullptr) {
        return tablelist;
    }
    int columncount = ui.tableView->model()->columnCount();
    for (int i = 0; i < columncount; ++i) {
        tablelist.append(ui.tableView->model()->headerData(i, Qt::Horizontal).toString().replace("\n", ""));
    }
    return tablelist;
}

bool ManagerCommonWidget::OpenInputDialog(const QString &titleName, const QString &textLabel, const QString &initname,
                                          QString &inputText, const QString &regStr)
{
    inputText = initname;
    KInputDialog *inputDialog = new KInputDialog(this);
    inputDialog->setTitle(titleName);
    inputDialog->setLabelText(textLabel);
    inputDialog->setTextValue(initname);
    if (inputDialog->lineEdit() != nullptr) {
        inputDialog->lineEdit()->setValidator(new QRegExpValidator(QRegExp(regStr)));
    }
    bool clickedOk = false;
    if (inputDialog->exec() == KBaseDlgBox::Ok) {
        inputText = inputDialog->textValue().trimmed();
        clickedOk = true;
    }
    if (inputDialog != nullptr) {
        delete inputDialog;
        inputDialog = nullptr;
    }
    return clickedOk;
}

void ManagerCommonWidget::initTableInfo(const QString &tableName, const QStringList &headerList, const int &freezecols,
                                        const int &colfixedwidth)
{
    QString oldTabelName = m_tableSetStruct.getCurrentTableName();
    m_tableSetStruct.setTableList(tableName, headerList, freezecols);
    m_isUpdateWidthOrScroll = true;
    updateFilterContents(oldTabelName != tableName);
    setTableColumFixedWidth(colfixedwidth);
    initHorizontalScrollbar(freezecols, colfixedwidth);
    onSelectedRowsChanged(0);
    m_isUpdateWidthOrScroll = false;
}

void ManagerCommonWidget::updateFilterContents(bool clearFilter)
{
    if (ui.comboBox == nullptr || ui.lineEdit == nullptr) {
        return;
    }
    if (!clearFilter) {
        onSearchTextChanged(ui.lineEdit->text());
        return;
    }

    QList<TableSetItem> tablelist = m_tableSetStruct.getCurrentTableList();
    if (tablelist.size() <= 0) {
        ui.comboBox->clear();
        ui.lineEdit->setText("");
        return;
    }
    QStringList listcolumnbox = QStringList();
    foreach (TableSetItem titleinfo, tablelist) {
        if (titleinfo.bDisplay) {
            listcolumnbox << titleinfo.colName;
        }
    }
    ui.comboBox->clear();
    ui.comboBox->addItems(listcolumnbox);
    for (int i = 0; i < listcolumnbox.size(); ++i) {
        ui.comboBox->setItemData(i, listcolumnbox[i], Qt::ToolTipRole);
    }
    ui.lineEdit->setText("");
}

void ManagerCommonWidget::initHorizontalScrollbar(const int &freezecols, const int &colfixedwidth)
{
    if (m_horizontalScrollbar == nullptr) {
        return;
    }

    if (ui.tableView->model() == nullptr) {
        m_horizontalScrollbar->initCustomScrollbar(freezecols, 0, 0);
        m_horizontalScrollbar->setVisible(false);
        return;
    }

    int layoutwidth = ui.tableView->viewport()->width();
    if (layoutwidth <= 0) {
        layoutwidth = 773;
    }

    int totalcols = ui.tableView->model()->columnCount();
    int tableminwidth = 0;
    for (int col = 0; col < totalcols - 1; ++col) {
        tableminwidth += ui.tableView->columnWidth(col);
    }
    tableminwidth += colfixedwidth;
    if (tableminwidth <= layoutwidth || freezecols == totalcols) {
        m_horizontalScrollbar->initCustomScrollbar(freezecols, 0, 0);
        m_horizontalScrollbar->setVisible(false);
        return;
    }
    int newmax = calculateScrollbarMax(freezecols);
    int currentpos = m_horizontalScrollbar->value();
    if (newmax == 0) {
        m_horizontalScrollbar->initCustomScrollbar(freezecols, 0, 0);
        m_horizontalScrollbar->setVisible(false);
    } else {
        m_horizontalScrollbar->initCustomScrollbar(freezecols, 0, newmax);
        m_horizontalScrollbar->setVisible(true);
    }
}

void ManagerCommonWidget::setTableColumFixedWidth(const int &width)
{
    if (ui.tableView->model() == nullptr || ui.tableView->viewport() == nullptr) {
        return;
    }
    int totalcol = ui.tableView->model()->columnCount();
    int viewwidth = ui.tableView->viewport()->width();
    QList<TableSetItem> titleproperty = m_tableSetStruct.getCurrentTableList();
    int visibleCols = m_tableSetStruct.getVisibleColumnsNo().size();
    if (titleproperty.size() != totalcol) {
        return;
    }
    if (viewwidth <= 0) {
        viewwidth = 773;
    }
    int realWidth = width;
    if (visibleCols > 0 && visibleCols * width < viewwidth) {
        realWidth = viewwidth / visibleCols;
    }
    for (int i = 0; i < totalcol; ++i) {
        if (!titleproperty[i].bDisplay) {
            ui.tableView->setColumnWidth(i, realWidth);
            ui.tableView->hideColumn(i);
        } else {
            ui.tableView->showColumn(i);
            ui.tableView->setColumnWidth(i, realWidth);
        }
    }
    m_pTableModel->emitHeaderDataChanged();
}

int ManagerCommonWidget::calculateScrollbarMax(const int &freezecol)
{
    if (ui.tableView->model() == nullptr || ui.tableView->viewport() == nullptr) {
        return 0;
    }
    int layoutwidth = ui.tableView->viewport()->width();
    int totalcols = ui.tableView->model()->columnCount();
    QList<int> visiblecolumnsNo = m_tableSetStruct.getVisibleColumnsNo();
    int candisplaycolumns = visiblecolumnsNo.size(); // 表格表头的属性【显示】的列数
    if (candisplaycolumns <= 0) {
        return 0;
    }
    int max = 0;
    int visiblecol = 0;
    int tableminwidth = calculateTableMinWidth(layoutwidth, totalcols, max, visiblecol);
    if (max == 0) {
        // 有隐藏列
        if (visiblecol < candisplaycolumns - 1) {
            if (layoutwidth < tableminwidth) {
                max = candisplaycolumns - visiblecol;
            } else {
                // 隐藏了多少列
                if (freezecol + candisplaycolumns - 2 - visiblecol >= candisplaycolumns) {
                    return 0;
                }
                for (int j = candisplaycolumns - 1 - visiblecol; j > 0; --j) {
                    ui.tableView->showColumn(visiblecolumnsNo[freezecol - 1 + j]);
                    int tmpmax = 0;
                    int tmpvisiblecol = 0;
                    int tmptableminwidth = calculateTableMinWidth(layoutwidth, totalcols, tmpmax, tmpvisiblecol);
                    if (layoutwidth < tmptableminwidth) {
                        max = candisplaycolumns - tmpvisiblecol;
                        break;
                    }
                }
            }
        } else {
            // 没有隐藏列，全部显示了
            if (layoutwidth < tableminwidth) {
                max = candisplaycolumns - visiblecol;
            }
        }
    }
    return max;
}

int ManagerCommonWidget::calculateTableMinWidth(const int &layoutwidth, const int &totalcols, int &max, int &visiblecol)
{
    int tableminwidth = 0;
    max = 0;
    visiblecol = 0;
    if (ui.tableView == nullptr) {
        return 0;
    }
    QList<TableSetItem> titlelist = m_tableSetStruct.getCurrentTableList();
    if (titlelist.size() != totalcols) {
        return 0;
    }
    QList<int> visiblecolumns = m_tableSetStruct.getVisibleColumnsNo();
    for (int col = 0; col < totalcols - 1; ++col) {
        if (ui.tableView->columnWidth(col) == 0 || !titlelist[col].bDisplay) {
            continue;
        }
        tableminwidth += ui.tableView->columnWidth(col);
        if (layoutwidth < tableminwidth) {
            max = visiblecolumns.size() - visiblecol; // 总列数-当前能够显示的列数，就是滚动条需要滚动的总index
            if (max >= visiblecolumns.size()) {
                max = visiblecolumns.size() - 1;
            } else if (max < 0) {
                max = 0;
            }
            break;
        }
        ++visiblecol; // 计算当前在可视范围内，表格能够显示的列数
    }
    tableminwidth += AssistantDefine::TableCol_Width;
    return tableminwidth;
}

void ManagerCommonWidget::setScrollbarStatus()
{
    if (m_horizontalScrollbar == nullptr || ui.tableView->model() == nullptr) {
        return;
    }
    int freezecol = m_horizontalScrollbar->getFreezeColno();
    if (freezecol == ui.tableView->model()->columnCount()) {
        m_horizontalScrollbar->setVisible(false);
        return;
    }
    int oldmax = m_horizontalScrollbar->maximum();
    int newmax = calculateScrollbarMax(freezecol);
    int currentpos = m_horizontalScrollbar->value();
    if (newmax == oldmax) {
        if (currentpos == newmax && m_tableSetStruct.getLastColumnsVisible()) {
            m_horizontalScrollbar->setValue(newmax - 1);
        }
        return;
    }
    if (newmax == 0) {
        m_horizontalScrollbar->setValue(0);
        m_horizontalScrollbar->setRange(0, 0);
        m_horizontalScrollbar->setVisible(false);
    } else {
        m_horizontalScrollbar->setRange(0, newmax);
        if (newmax < oldmax) {
            if (currentpos > newmax) {
                m_horizontalScrollbar->setValue(newmax);
            } else if (currentpos == newmax) {
                m_horizontalScrollbar->setValue(newmax - 1);
            }
        }
        m_horizontalScrollbar->setVisible(true);
    }
}

QSharedPointer<KLProject> ManagerCommonWidget::getCurrentTreeProject()
{
    if (ui.treeView == nullptr) {
        return PKLProject(nullptr);
    }
    QModelIndex curindex = ui.treeView->currentIndex();
    if (!curindex.isValid() || m_treeModel == nullptr) {
        return PKLProject(nullptr);
    }
    return m_treeModel->getProjectByIndex(curindex);
}

QSharedPointer<Kcc::BlockDefinition::Model>
ManagerCommonWidget::getProtoModel(QSharedPointer<KLProject> project,
                                   QSharedPointer<Kcc::BlockDefinition::Model> modelInstance)
{
    if (project == nullptr || modelInstance == nullptr) {
        return PModel(nullptr);
    }

    QList<PModel> ProtoModels = project->getAllModel(modelInstance->getModelType());
    for (PModel protomodel : ProtoModels) {
        if (protomodel == nullptr) {
            continue;
        }
        for (PModel cmodel : protomodel->getInstanceList()) {
            if (cmodel != nullptr && cmodel == modelInstance) {
                return protomodel;
            }
        }
    }
    return PModel(nullptr);
}

QSharedPointer<Kcc::BlockDefinition::Model>
ManagerCommonWidget::getProjectModelByName(const QString &modelName, const int &modelType,
                                           QSharedPointer<KLProject> project)
{
    if (modelName.isEmpty()) {
        return PModel(nullptr);
    }

    if (project == nullptr && m_pProjDataManager != nullptr) {
        project = m_pProjDataManager->getCurrentProject();
    }

    if (project == nullptr) {
        return PModel(nullptr);
    }
    QList<PModel> models = project->getAllModel();
    for (PModel model : models) {
        if (model == nullptr) {
            continue;
        }
        if (modelType != -1 && model->getModelType() != modelType) {
            continue;
        }
        if (model->getName() == modelName) {
            return model;
        } else if (CombineBoardModel::Type == model->getModelType() || ComplexBoardModel::Type == model->getModelType()
                   || ControlBlock::Type == model->getModelType()) {
            for (PModel instance : model->getInstanceList()) {
                if (instance == nullptr) {
                    continue;
                }
                if (instance->getName() == modelName) {
                    return instance;
                }
            }
        }
    }
    return PModel(nullptr);
}

bool ManagerCommonWidget::EditModel(QSharedPointer<Kcc::BlockDefinition::Model> model, bool readOnly)
{
    if (ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr
        || ProjectViewServers::getInstance().m_pGraphicModelingServer == nullptr || model == nullptr) {
        return false;
    }
    bool change = false;
    if (DeviceModel::Type == model->getModelType()) {
        PDeviceModel devModel = model.dynamicCast<DeviceModel>();
        if (devModel == nullptr) {
            return false;
        }
        change = ProjectViewServers::getInstance().m_pPropertyManagerServer->EditDeviceModel(devModel, readOnly);
    } else if (ControlBlock::Type == model->getModelType() || ElectricalBlock::Type == model->getModelType()
               || SlotBlock::Type == model->getModelType() || ElectricalContainerBlock::Type == model->getModelType()) {
        change = ProjectViewServers::getInstance().m_pPropertyManagerServer->ShowBlockProperty(model, readOnly);
    } else if (CombineBoardModel::Type == model->getModelType()) {
        if (model->getParentModel() == nullptr && !model->isInstance()) {
            change = ProjectViewServers::getInstance().m_pPropertyManagerServer->ShowDrawBoardProperty(
                    model.dynamicCast<DrawingBoardClass>(), "", readOnly);
        } else {
            change = ProjectViewServers::getInstance().m_pPropertyManagerServer->ShowBlockProperty(model, readOnly);
        }
    } else if (ElecBoardModel::Type == model->getModelType() || ElecCombineBoardModel::Type == model->getModelType()
               || ControlBoardModel::Type == model->getModelType()
               || (ComplexBoardModel::Type == model->getModelType() && !model->isInstance())) {
        change = ProjectViewServers::getInstance().m_pPropertyManagerServer->ShowDrawBoardProperty(
                model.dynamicCast<DrawingBoardClass>(), "", readOnly);
    } else if (ComplexBoardModel::Type == model->getModelType() && model->isInstance()) {
        change = ProjectViewServers::getInstance().m_pPropertyManagerServer->EditComplexInstance(model, readOnly);
    }
    return change;
}

void ManagerCommonWidget::onTableOpenBlock()
{
    if (ui.tableView == nullptr || ProjectViewServers::getInstance().m_pGraphicModelingServer == nullptr) {
        return;
    }

    QModelIndex curindex = getTableSourceModelIndex();
    QModelIndexList selectIndexs = ui.tableView->getCurrentSelectedIndexs();
    PKLProject currProject = getCurrentTreeProject();
    if (!curindex.isValid() || m_pTableModel == nullptr || currProject == nullptr) {
        return;
    }
    PModel pmod = m_pTableModel->getModelByIndex(curindex);
    if (pmod == nullptr) {
        return;
    }

    PDrawingBoardClass pdrawboard(nullptr);
    if (pmod->getParentModel() != nullptr) {
        pdrawboard = pmod->getParentModel().dynamicCast<DrawingBoardClass>();
    }
    if (pdrawboard == nullptr) {
        return;
    }
    if (pdrawboard->isInstance()) {
        PModel protoModel = getProtoModel(currProject, pdrawboard);
        if (protoModel != nullptr) {
            ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(
                    protoModel.dynamicCast<DrawingBoardClass>(), pmod->getUUID());
            emit openBlockOrBoard();
        }
        return;
    }
    ProjectViewServers::getInstance().m_pGraphicModelingServer->openDrawingBoard(pdrawboard, pmod->getUUID());
    emit openBlockOrBoard();
}

void ManagerCommonWidget::onTableNew()
{
    if (ProjectViewServers::getInstance().m_pPropertyManagerServer == nullptr || ui.tableView == nullptr
        || ui.treeView == nullptr) {
        return;
    }
    QModelIndex curindex = getTableSourceModelIndex();
    PKLProject currProject = getCurrentTreeProject();
    if (!curindex.isValid() || currProject == nullptr
        || TableBaseModel::ModelDataType_ProjectDeviceTypes
                != curindex.data(TableBaseModel::CustomRole_ModelDataType).toInt()) {
        return;
    }

    PDeviceModel devModel = ProjectViewServers::getInstance().m_pPropertyManagerServer->CreateDeviceModel(
            curindex.data(TableBaseModel::CustomRole_DevicePrototype).toString());
    if (devModel != nullptr && currProject->saveModel(devModel)) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_New, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                            devModel->getName(), true),
               LOG_NORMAL);
        onTreeClicked(ui.treeView->currentIndex());
        connectModel(devModel);
        emit addModel(devModel);
    }
}

void ManagerCommonWidget::onTableCopy()
{
    QModelIndex curindex = ui.tableView->currentIndex();
    if (!curindex.isValid() || m_pProxyModel == nullptr) {
        return;
    }
    QModelIndex sourceIndex = m_pProxyModel->mapToSource(curindex);
    PKLProject currProject = getCurrentTreeProject();
    if (!sourceIndex.isValid() || currProject == nullptr) {
        return;
    }
    CustomMineData *cMineData = getCustomMineData(true);
    if (cMineData == nullptr) {
        return;
    }
    cMineData->setOperation(CustomMineData::Operation_Copy);
    cMineData->addModels(currProject, m_pTableModel->getModelsByIndexs(QModelIndexList() << sourceIndex));
    QApplication::clipboard()->setMimeData(cMineData);
    ui.tableView->selectRow(curindex.row());
}

void ManagerCommonWidget::onTablePaste()
{
    QModelIndex curindex = ui.tableView->currentIndex();
    if (!curindex.isValid() || m_pProxyModel == nullptr || m_pTableModel == nullptr) {
        return;
    }
    QModelIndex sourceIndex = m_pProxyModel->mapToSource(curindex);
    CustomMineData *cMineData = getCustomMineData();
    if (!sourceIndex.isValid() || cMineData == nullptr) {
        return;
    }
    QList<PModel> models = cMineData->getModels(
            DeviceModel::Type, sourceIndex.data(TableBaseModel::CustomRole_DevicePrototype).toString());
    if (models.size() != 1) {
        return;
    }
    QList<OldNewDataStruct> modifyList;
    m_pTableModel->pasteDeviceModel(models[0], sourceIndex, modifyList);
    if (modifyList.size() > 0) {
        m_pUndoStack->push(new TableUndoCommand(m_pTableModel, modifyList, false));
    }
    ui.tableView->selectRow(curindex.row());
}

void ManagerCommonWidget::onTableDelete()
{
    QModelIndex modelIndex = getTableSourceModelIndex();
    if (!modelIndex.isValid() || m_pTableModel == nullptr || ui.treeView == nullptr) {
        return;
    }
    TableBaseModel::ModelDataType modelDataType =
            TableBaseModel::ModelDataType(modelIndex.data(TableBaseModel::CustomRole_ModelDataType).toInt());
    if (TableBaseModel::ModelDataType_ProjectDeviceTypes != modelDataType) {
        return;
    }
    PModel model = m_pTableModel->getModelByIndex(modelIndex);
    PKLProject currProject = getCurrentTreeProject();
    if (currProject == nullptr || model == nullptr) {
        return;
    }
    if (!questionDelete(tr("Model"), model->getName())) {
        return;
    }

    if (!currProject->deleteModel(model)) {
        LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                            model->getName(), false),
               LOG_WARNING);
        return;
    }
    onVerifyDrawBoard(model->getUUID(), model->getModelType());
    LOGOUT(AssistantDefine::getOuputLog(AssistantDefine::LogType_Delete, AssistantDefine::LOG_TYPE_DEVICETYPE,
                                        model->getName(), true),
           LOG_NORMAL);
    onTreeClicked(ui.treeView->currentIndex());
    emit deleteModel();
}

void ManagerCommonWidget::onTableEdit()
{
    QModelIndex curindex = ui.tableView->currentIndex();
    if (!curindex.isValid()) {
        return;
    }
#ifdef COMPILER_DEVELOPER_MODE // 开发者模式下可修改系统设备类型
    if (TableBaseModel::ModelDataType_GlobalDeviceTypes
                == curindex.data(TableBaseModel::CustomRole_ModelDataType).toInt()
        && m_pTableModel != nullptr && m_pProxyModel != nullptr) {
        QModelIndex sourceIndex = m_pProxyModel->mapToSource(curindex);
        PModel devModel = m_pTableModel->getModelByIndex(sourceIndex);
        if (devModel == nullptr) {
            return;
        }
        if (ProjectViewServers::getInstance().m_wizardServer->ChangeDeviceWizardDialog(devModel)) {
            BuiltinModelManager::getInstance().getBuiltinProject()->saveModel(devModel);
            onTreeClicked(ui.treeView->currentIndex());
        }
    } else {
        onTableDoubleClicked(curindex);
    }
#else
    onTableDoubleClicked(curindex);
#endif
}

void ManagerCommonWidget::onTableBatchModify()
{
    QModelIndex curindex = getTableSourceModelIndex();
    QModelIndexList selectIndexs = ui.tableView->getCurrentSelectedIndexs();
    if (!curindex.isValid() || m_pProxyModel == nullptr) {
        return;
    }
    QWidget *pwidget =
            NPS::createWidget(curindex.data(NPS::ModelDataTypeRole).toString(), curindex.data(NPS::ModelDataRangeRole));
    if (pwidget == nullptr) {
        KMessageBox::warning(tr("Unable to find corresponding data type"), KMessageBox::Ok, KMessageBox::Ok);
    }
    NPS::setWidgetControlData(pwidget, curindex.data(NPS::ModelDataRealValueRole),
                              curindex.data(NPS::ModelDataTypeRole).toString(), curindex.data(NPS::ModelDataRangeRole));
    KCustomDialog dlg(AssistantDefine::STR_ACT_BATCHMODIFY, pwidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
                      KBaseDlgBox::Ok);
    dlg.resize(50, 30);
    if (dlg.exec() == KBaseDlgBox::Ok) {

        QVariant newvalue;
        QString type = curindex.data(NPS::ModelDataTypeRole).toString();
        if (RoleDataDefinition::ControlMathExpression == type) {
            QVariant tmpvalue;
            QLineEdit *edit = static_cast<QLineEdit *>(pwidget);
            if (edit != nullptr) {
                newvalue = edit->text();
            } else {
                return;
            }

            if (ProjectViewServers::getInstance().m_pCodeManagerServer == nullptr) {
                return;
            }

            QString errorinfo = "";
            int beginindex = -1;
            int endindex = -1;
            int checkstatus = ProjectViewServers::getInstance().m_pCodeManagerServer->CheckMathExpr(
                    edit->text(), errorinfo, beginindex, endindex);
            if (MATH_EXPR_MATCH_PARTIAL == checkstatus || MATH_EXPR_MATCH_FAILED == checkstatus) {
                // 表达式输入错误，修改失败，请重新输入，失败原因：
                LOGOUT(QObject::tr(
                               "Expression input error, modification failed, Please re-enter. Reason for failure:%1")
                               .arg(errorinfo),
                       LOG_WARNING);
                return;
            } else if (!ProjectViewServers::getInstance().m_pCodeManagerServer->CalcMathExpr(edit->text(), tmpvalue,
                                                                                             errorinfo)) {
                // 表达式计算失败，失败原因：
                LOGOUT(QObject::tr("Expression calculation failed, reason for failure:%1").arg(errorinfo), LOG_WARNING);
                return;
            }
        } else {
            newvalue = NPS::getWidgetDataMaptoRealData(pwidget, curindex.data(NPS::ModelDataTypeRole).toString(),
                                                       curindex.data(NPS::ModelDataRangeRole));
        }
        QModelIndexList sourceIndexList = transToSourceIndexes(selectIndexs);
        QList<OldNewDataStruct> modifylist;
        m_pTableModel->batchModifyModel(sourceIndexList, newvalue, type, modifylist);
        m_pUndoStack->push(new TableUndoCommand(m_pTableModel, modifylist, false));
        if (pwidget != nullptr) {
            delete pwidget;
            pwidget = nullptr;
        }
    }
}

QModelIndexList ManagerCommonWidget::transToSourceIndexes(const QModelIndexList &indexlist)
{
    QModelIndexList sourcelist;
    if (m_pProxyModel == nullptr) {
        return QModelIndexList();
    }
    foreach (QModelIndex tmpindex, indexlist) {
        if (tmpindex.isValid()) {
            sourcelist.append(m_pProxyModel->mapToSource(tmpindex));
        }
    }
    return sourcelist;
}

QModelIndex ManagerCommonWidget::getTableSourceModelIndex()
{
    QModelIndex curindex = ui.tableView->currentIndex();
    if (!curindex.isValid() || m_pProxyModel == nullptr) {
        return QModelIndex();
    }
    return m_pProxyModel->mapToSource(curindex);
}

QSharedPointer<Kcc::BlockDefinition::Model> ManagerCommonWidget::getModelByUUIDTYPE(const QString &uuid,
                                                                                    const int &modelType)
{
    PKLProject curproject = m_pProjDataManager->getCurrentProject();
    if (curproject == nullptr) {
        return PModel(nullptr);
    }
    if (DeviceModel::Type == modelType) {
        QList<PModel> deviceModels = curproject->getAllModel(DeviceModel::Type);
        for (PModel model : deviceModels) {
            if (model != nullptr && uuid == model->getUUID()) {
                return model;
            }
        }
    } else if (ElecBoardModel::Type == modelType) {
        QList<PModel> elecModels = curproject->getAllModel(ElecBoardModel::Type);
        for (PModel model : elecModels) {
            if (model != nullptr && uuid == model->getUUID()) {
                return model;
            }
        }
    } else if (ElectricalBlock::Type == modelType) {
        QList<PModel> elecBoardModels = curproject->getAllModel(ElecBoardModel::Type);
        for (PModel elecboard : elecBoardModels) {
            if (elecboard == nullptr) {
                continue;
            }
            PModel block = elecboard->getChildModel(uuid);
            if (block != nullptr) {
                return block;
            }
        }
    } else {
        QList<PModel> ctrBoards = curproject->getAllModel(CombineBoardModel::Type)
                + curproject->getAllModel(ComplexBoardModel::Type) + curproject->getAllModel(ControlBoardModel::Type);
        for (PModel board : ctrBoards) {
            if (board == nullptr) {
                continue;
            }
            PModel block = board->getChildModel(uuid);
            if (block != nullptr) {
                return block;
            }
        }
    }
    return PModel(nullptr);
}

bool ManagerCommonWidget::checkNameValid(QSharedPointer<KLProject> project,
                                         QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &name,
                                         QString &errorinfo)
{
    errorinfo = "";
    if (name.isEmpty()) {
        errorinfo = NPS::ERRORINFO_BLOCKNAME_EMPTY;
        return false;
    }
    if (name.length() > BLOCK_NAME_MAX_LENGHT) {
        errorinfo = NPS::ERRORINFO_NAME_EXCEEDLENGTH;
        return false;
    }
    if (model == nullptr) {
        return true;
    }
    if (model->getName() == name) {
        return true;
    }
    if (model->getParentModel() != nullptr) {
        for (PModel cmodel : model->getParentModel()->getChildModels().values()) {
            if (cmodel != nullptr && cmodel->getName() == name) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
        return true;
    } else if (model->isInstance()) {
        PModel protoModel = getProtoModel(project, model);
        if (protoModel == nullptr) {
            return true;
        }
        if (protoModel->getName() == name) {
            errorinfo = NPS::ERRORINFO_REPEAT;
            return false;
        }
        for (PModel instanceModel : protoModel->getInstanceList()) {
            if (instanceModel != nullptr && instanceModel->getName() == name) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
        return true;
    } else {
        if (ProjectViewServers::getInstance().m_projectManagerServer->IsExistModel(name)) {
            errorinfo = NPS::ERRORINFO_REPEAT;
            return false;
        }
        return true;
    }
}

void ManagerCommonWidget::JumptoPosition(const QStringList &indexNames)
{
    if (m_treeModel == nullptr || ui.treeView == nullptr) {
        return;
    }
    QModelIndex curIndex = m_treeModel->getTreeIndex(indexNames);
    if (curIndex.isValid()) {
        ui.treeView->setCurrentIndex(curIndex);
        onTreeClicked(curIndex);
    }
}

QModelIndexList ManagerCommonWidget::getTreeSelectedIndexes()
{
    if (ui.treeView != nullptr && ui.treeView->selectionModel() != nullptr) {
        return ui.treeView->selectionModel()->selectedIndexes();
    }
    return QModelIndexList();
}

bool ManagerCommonWidget::questionDelete(const QString &typeName, const QString &contentName)
{
    //"typeName[contentName]及其所有内部数据均会被删除。该操作无法恢复，是否确认删除？"
    if (KMessageBox::question(tr("%1 [%2] and all its internal data will be deleted. This operation "
                                 "cannot be restored. Are you sure you to delete it?")
                                      .arg(typeName)
                                      .arg(contentName),
                              KMessageBox::Yes | KMessageBox::No)
        == KMessageBox::Yes) {
        return true;
    }
    return false;
}

bool ManagerCommonWidget::saveModel(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }
    PModel topModel = model->getTopParentModel();
    if (topModel == nullptr) {
        return false;
    }
    // board open not save
    if (ProjectViewServers::getInstance().m_pGraphicModelingServer->hasDrawingBoardById(topModel->getObjectUuid())) {
        return false;
    }
    if (m_pProjDataManager == nullptr) {
        return false;
    }
    PKLProject pCurProject = m_pProjDataManager->getCurrentProject();
    if (pCurProject == nullptr) {
        return false;
    }
    PModel realmodel = pCurProject->findModel(topModel->getModelUuid());
    if (realmodel == nullptr) {
        return false;
    }
    return pCurProject->saveModel(realmodel);
}

void ManagerCommonWidget::onManagerClosed(int btn) { }

void ManagerCommonWidget::onTreeClicked(const QModelIndex &index) { }

void ManagerCommonWidget::onTreeContextMenu(const QPoint &point) { }

void ManagerCommonWidget::onActionTriggered() { }

void ManagerCommonWidget::onModelValueChanged(const QList<OldNewDataStruct> &modifylist)
{
    m_pUndoStack->push(new TableUndoCommand(m_pTableModel, modifylist, false));
}

void ManagerCommonWidget::onVerifyDrawBoard(const QString &uuid, const int &modelType)
{
    if (m_pProjDataManager == nullptr || uuid.isEmpty()) {
        return;
    }
    PKLProject curproject = m_pProjDataManager->getCurrentProject();
    if (curproject == nullptr || ProjectViewServers::getInstance().m_pElecModelCheckServer == nullptr) {
        return;
    }
    if (modelType == ElectricalBlock::Type) {
        PModel model = getModelByUUIDTYPE(uuid, modelType);
        if (model != nullptr && model->getParentModel() != nullptr) {
            ProjectViewServers::getInstance().m_pElecModelCheckServer->VerifyDrawingBoardStatus(
                    model->getParentModel().dynamicCast<DrawingBoardClass>());
        }
    } else if (modelType == ElecBoardModel::Type) {
        PModel model = getModelByUUIDTYPE(uuid, modelType);
        if (model != nullptr) {
            ProjectViewServers::getInstance().m_pElecModelCheckServer->VerifyDrawingBoardStatus(
                    model.dynamicCast<DrawingBoardClass>());
        }
    } else if (modelType == DeviceModel::Type) {
        QList<PModel> elecBoardModels = curproject->getAllModel(ElecBoardModel::Type);
        for (PModel elecboard : elecBoardModels) {
            if (elecboard == nullptr) {
                continue;
            }
            ProjectViewServers::getInstance().m_pElecModelCheckServer->VerifyDrawingBoardStatus(
                    elecboard.dynamicCast<DrawingBoardClass>());
        }
    }
}

void ManagerCommonWidget::onPropertyManagerServerMsg(unsigned int code, const NotifyStruct &structinfo) { }

void ManagerCommonWidget::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &structinfo)
{
    if (Notify_DrawingBoardRunning == code) {
        m_DrawboardIsRuning = true;
        if (m_pTableModel != nullptr) {
            m_pTableModel->SetProjectRuningSts(true);
        }
    } else if (Notify_DrawingBoardStopped == code) {
        updateUI(getTreeCurrIndexNames());
        m_DrawboardIsRuning = false;
        if (m_pTableModel != nullptr) {
            m_pTableModel->SetProjectRuningSts(false);
        }
    }
}

void ManagerCommonWidget::onReceiveProjectManagerServerMsg(uint code, const NotifyStruct &param) { }

void ManagerCommonWidget::onHorizontalValueChanged(bool bscrollright, int begincol, int changecolsno)
{
    if (m_isUpdateWidthOrScroll) {
        return;
    }

    QList<int> visiblecolumnsNo = m_tableSetStruct.getVisibleColumnsNo();
    int candisplaycolumns = visiblecolumnsNo.size(); // 表格表头的属性【显示】的列数
    if (candisplaycolumns <= 0 || begincol < 0)
        return;
    if (bscrollright) {
        if (candisplaycolumns <= begincol + changecolsno - 1)
            return;
        for (int i = 0; i < changecolsno; ++i) {
            ui.tableView->hideColumn(visiblecolumnsNo[begincol + i]);
        }
    } else {
        if (candisplaycolumns <= begincol || begincol < changecolsno - 1)
            return;
        for (int i = 0; i < changecolsno; ++i) {
            ui.tableView->showColumn(visiblecolumnsNo[begincol - i]);
        }
    }
}

void ManagerCommonWidget::onHorizontalHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{
    if (!m_isUpdateWidthOrScroll && oldSize != 0 && newSize != 0 && ui.tableView->model() != nullptr
        && logicalIndex < ui.tableView->model()->columnCount() - 1) {
        m_isUpdateWidthOrScroll = true;
        setScrollbarStatus();
        m_isUpdateWidthOrScroll = false;
    }
}

void ManagerCommonWidget::onTableSettingClicked()
{
    SettingWidget *ptableSettingWidget = new SettingWidget();
    ptableSettingWidget->setTableTitle(m_tableSetStruct.getCurrentTableList(), m_tableSetStruct.currentTableFreezeCols);
    KCustomDialog setCustomDlg(AssistantDefine::TITLE_TABLESET, ptableSettingWidget,
                               KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::Ok);
    setCustomDlg.resize(AssistantDefine::SettingTableDlg_Width, AssistantDefine::SettingTableDlg_Height);

    if (setCustomDlg.exec() == KBaseDlgBox::Ok) {
        onTableSettingOKclicked(ptableSettingWidget->getCurrentTableSetList());
    }
}

void ManagerCommonWidget::onTableSettingOKclicked(const QList<TableSetItem> &curTableSetList)
{
    if (m_horizontalScrollbar != nullptr) {
        QList<int> oldvisibleColumnsNo = m_tableSetStruct.getVisibleColumnsNo();
        QList<TableSetItem> oldtablelist = m_tableSetStruct.getCurrentTableList();
        m_tableSetStruct.saveTableList(curTableSetList);
        QList<int> newvisibleColumnsNo = m_tableSetStruct.getVisibleColumnsNo();
        QList<TableSetItem> tablelist = m_tableSetStruct.getCurrentTableList();
        if (oldvisibleColumnsNo == newvisibleColumnsNo || oldtablelist.size() != tablelist.size()) {
            return;
        }
        int currentvalue = m_horizontalScrollbar->value();
        int freezecol = m_horizontalScrollbar->getFreezeColno();
        int currentcolumn = 0;
        if ((freezecol + currentvalue) < oldvisibleColumnsNo.size()) {
            currentcolumn = oldvisibleColumnsNo[freezecol + currentvalue]; // 非冻结列当前显示的第一列
        } else {
            if (freezecol - 1 >= 0 && freezecol - 1 <= oldvisibleColumnsNo.size()) {
                currentcolumn = oldvisibleColumnsNo[freezecol - 1];
            }
        }
        if (tablelist.size() <= 0) {
            return;
        }
        m_isUpdateWidthOrScroll = true;
        int realWidth = AssistantDefine::TableCol_Width;
        int visibleCols = newvisibleColumnsNo.size();
        if (visibleCols > 0 && ui.tableView->viewport() != nullptr
            && (visibleCols * AssistantDefine::TableCol_Width < ui.tableView->viewport()->width())) {
            realWidth = ui.tableView->viewport()->width() / visibleCols;
        }
        for (int col = 0; col < tablelist.size(); ++col) {
            ui.tableView->setColumnWidth(col, realWidth);
            if (col < freezecol) {
                continue;
            }
            if (col < currentcolumn) {
                int scurrentvalue = m_horizontalScrollbar->value();
                if (tablelist[col].bDisplay && !oldtablelist[col].bDisplay) {
                    m_horizontalScrollbar->setValue(scurrentvalue + 1);
                } else if (!tablelist[col].bDisplay && oldtablelist[col].bDisplay) {
                    m_horizontalScrollbar->setValue(scurrentvalue - 1);
                }
                continue;
            }
            if (tablelist[col].bDisplay) {
                ui.tableView->showColumn(col);
            } else if (!tablelist[col].bDisplay) {
                ui.tableView->hideColumn(col);
            }
        }
        updateFilterContents();
        setScrollbarStatus();
        m_isUpdateWidthOrScroll = false;
    }
}

void ManagerCommonWidget::onSearchColBoxChanged(const QString &textstr)
{
    if (m_pProxyModel != nullptr) {
        m_pProxyModel->setFilterColumn(m_tableSetStruct.getCurrentTitleColNo(textstr));
    }
}

void ManagerCommonWidget::onSearchTextChanged(const QString &strtext)
{
    if (m_pProxyModel != nullptr) {
        m_pProxyModel->setFilterString(strtext);
    }
}

void ManagerCommonWidget::onTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || m_pTableModel == nullptr || m_pProxyModel == nullptr) { // 无效编辑
        return;
    }
    QModelIndex modelIndex = m_pProxyModel->mapToSource(index);
    if (!modelIndex.isValid()) {
        return;
    }

    TableBaseModel::ModelDataType modelDataType =
            TableBaseModel::ModelDataType(modelIndex.data(TableBaseModel::CustomRole_ModelDataType).toInt());
    if (TableBaseModel::ModelDataType_Invalid == modelDataType || TableBaseModel::ModelDataType_Tree == modelDataType) {
        return;
    }
    bool projectActive = modelIndex.data(TableBaseModel::CustomRole_CurProjectActiveSts).toBool();
    if (!projectActive) {
        return;
    }
    PModel model = m_pTableModel->getModelByIndex(modelIndex);
    if (model == nullptr) {
        return;
    }
    bool change = false;
    bool drawboardRruning = index.data(TableBaseModel::CustomRole_DrawboardRunningSts).toBool();
    QString keywordStr = modelIndex.data(NPS::ModelDataKeywordsRole).toString();
    if (TableBaseModel::ModelDataType_CodeBlocks == modelDataType
        || TableBaseModel::ModelDataType_ControlBoardBlocks == modelDataType
        || TableBaseModel::ModelDataType_CombineBoardBlocks == modelDataType
        || TableBaseModel::ModelDataType_ComplexBoardBlocks == modelDataType
        || TableBaseModel::ModelDataType_ControlInstances == modelDataType) {
        // open,edit
        if (AssistantDefine::KEYWORD_TITLE_MODELNAME == keywordStr
            || AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == keywordStr
            || AssistantDefine::KEYWORD_TITLE_SLOT == keywordStr) {
            change = EditModel(model, drawboardRruning);
        }
    } else if (TableBaseModel::ModelDataType_ComplexBoardInstances == modelDataType) {
        // edit
        if (AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == keywordStr) {
            change = EditModel(model, drawboardRruning);
        }
    } else if (TableBaseModel::ModelDataType_ProjectDeviceTypes == modelDataType) {
        // copy,paste,new,delete,edit
        if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == keywordStr) {
            change = EditModel(model, drawboardRruning);
        }
    } else if (TableBaseModel::ModelDataType_ElectricalBlocks == modelDataType) {
        // open edit/edit/edit
        if (AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == keywordStr) {
            change = EditModel(model, drawboardRruning);
        } else if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == keywordStr) {
            PElectricalBlock peblock = model.dynamicCast<ElectricalBlock>();
            if (peblock == nullptr) {
                return;
            }
            PDeviceModel devmodel = peblock->getDeviceModel();
            if (devmodel == nullptr) {
                return;
            }
            change = EditModel(devmodel, drawboardRruning);
        } else if (AssistantDefine::KEYWORD_TITLE_BOARDNAME == keywordStr) {
            PModel boardmodel = model->getParentModel();
            if (boardmodel != nullptr) {
                change = EditModel(boardmodel, drawboardRruning);
            }
        }
    } else if (TableBaseModel::ModelDataType_ControlBlocks == modelDataType) {
        if (AssistantDefine::KEYWORD_TITLE_BOARDNAME == keywordStr) {
            PModel boardmodel = model->getParentModel();
            if (boardmodel != nullptr) {
                change = EditModel(boardmodel, drawboardRruning);
            }
        } else {
            change = EditModel(model, drawboardRruning);
        }
    }
    if (change) {
        onModelChanged(model);
        emit modelChanged(model);
    }
}

void ManagerCommonWidget::onTableItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || m_pTableModel == nullptr || !index.data(NPS::ModelDataEditRole).toBool()) {
        return;
    }
    if (RoleDataDefinition::ControlTypeColor == index.data(NPS::ModelDataTypeRole).toString()) {
        KColorDialog dlg(tr("Select Color"), this);
        QColor curColor;
        QString keyword = index.data(NPS::ModelDataKeywordsRole).toString();
        QString strColor = index.data().toString();
        curColor.setNamedColor(strColor);
        dlg.setCurrentColor(curColor);
        if (dlg.exec() == KColorDialog::Ok) {
            QColor color = dlg.currentColor();
            m_pTableModel->setData(index, color.name(), Qt::EditRole);
        }
    } else if (RoleDataDefinition::ControlTypeFileSelect == index.data(NPS::ModelDataTypeRole).toString()) {
        QString pixpath = QFileDialog::getOpenFileName(
                this, tr("Select File"), "",
                QString("%1(*.txt *.csv);;%2(*)").arg(tr("Common Files")).arg(tr("All Files")));
        if (pixpath.isEmpty()) {
            return;
        }
        m_pTableModel->setData(index, pixpath, Qt::EditRole);
    } else if (RoleDataDefinition::ControlTypeFont == index.data(NPS::ModelDataTypeRole).toString()
               && ProjectViewServers::getInstance().m_pPropertyManagerServer != nullptr) {
        QFont oldFont = index.data(Qt::FontRole).value<QFont>();
        QFont newFont = ProjectViewServers::getInstance().m_pPropertyManagerServer->ShowFontSetDialog(oldFont);
        if (newFont == oldFont) {
            return;
        }
        m_pTableModel->setData(index, newFont, Qt::EditRole);
    }
}

void ManagerCommonWidget::onSelectedRowsChanged(const int &totalselectrow)
{
    int totalrow = 0;
    if (ui.tableView != nullptr && ui.tableView->model() != nullptr) {
        totalrow = ui.tableView->model()->rowCount();
    }
    int selectno = totalselectrow;
    if (totalrow <= 0) {
        m_rowlabel->setText(QString(""));
        return;
    }
    if (selectno > 0) {
        m_rowlabel->setText(QObject::tr("%1 item(s),%2 item(s) selected")
                                    .arg(QString::number(totalrow))
                                    .arg(QString::number(selectno)));
    } else {
        m_rowlabel->setText(QObject::tr("%1 item(s)").arg(QString::number(totalrow)));
    }
}

void ManagerCommonWidget::onTableMenuEnd() { }

void ManagerCommonWidget::onModelChanged(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (m_pTableModel != nullptr) {
        m_pTableModel->emitDataChange(model);
    }
}

void ManagerCommonWidget::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    m_isUpdateWidthOrScroll = true;
    setTableColumFixedWidth(AssistantDefine::TableCol_Width);
    if (m_horizontalScrollbar != nullptr) {
        m_horizontalScrollbar->setValue(0);
    }
    setScrollbarStatus();
    m_isUpdateWidthOrScroll = false;
}

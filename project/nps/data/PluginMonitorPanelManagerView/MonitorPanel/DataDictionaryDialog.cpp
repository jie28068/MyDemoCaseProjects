#include "DataDictionaryDialog.h"
#include "CoreLib/ServerManager.h"
#include "MonitorPanelPlotServer.h"
#include <QHBoxLayout>

USE_LOGOUT_("DataDictionaryDialog")

bool DataDictionaryDialog::m_isShow = false;

DataDictionaryDialog::DataDictionaryDialog(QWidget *parent) : KCustomDialog(parent)
{
    this->setModal(false);
    QWidget *w = new QWidget;
    w->setObjectName("out");
    setCentralWidget(w);
    w->setStyleSheet("QWidget#out{border:0px solid}");

    this->setTitle(tr("Variable Table")); // 变量表

    setWindowFlags(windowFlags() /*| Qt::WindowStaysOnTopHint*/);
    setMargin(1);
    QWidget *dataDictionaryView = createDataDictionaryView();
    if (dataDictionaryView) {
        // this->setCentralWidget(dataDictionaryView);
        QHBoxLayout *layout = new QHBoxLayout(w);
        // layout->setContentsMargins(4, 4, 4, 4);
        layout->addWidget(dataDictionaryView);
    }
    setMinimumSize(QSize(320, 650));
    // this->setContentsMargins(4,4,4,4);
    setHiddenButtonGroup(true);

    m_isShow = true;
}

DataDictionaryDialog::~DataDictionaryDialog()
{
    m_isShow = false;
}

void DataDictionaryDialog::refreshNodeState()
{
    setTreeExpandState(mTree);
}

QWidget *DataDictionaryDialog::createDataDictionaryView()
{
    PIDataDictionaryViewServer dataDictionaryViewServer = RequestServer<IDataDictionaryViewServer>();
    if (!dataDictionaryViewServer) {
        LOGOUT(tr("DataDictionaryViewServer is not registered"), LOG_WARNING); // DataDictionaryViewServer未注册
        return nullptr;
    }

    QWidget *dataDictionaryView = dataDictionaryViewServer->CreateElementTreeViewWidget(nullptr);
    if (!dataDictionaryView) {
        LOGOUT(tr("Failed to create the data dictionary window"), LOG_WARNING); // 创建数据字典窗口失败
        return nullptr;
    }
    m_dataDictionaryViewController =
            QueryInterface<IElementTreeViewController>(QSharedPointer<QWidget>(dataDictionaryView));
    m_dataDictionaryViewController->EnableDisplayVariableValue(true);
    m_dataDictionaryViewController->SetEditable(false);
    m_dataDictionaryViewController->SetStructDrag(false);
    m_dataDictionaryViewController->EnableDisplayVariableValue(false);
    m_dataDictionaryViewController->HideColumn(1);
    m_dataDictionaryViewController->HideColumn(2);
    m_dataDictionaryViewController->SetShowIcon(false);
    m_dataDictionaryViewController->EnableShowProperty(false);
    m_dataDictionaryViewController->ShowSaveButton(false);
    m_dataDictionaryViewController->ShowSearchWidget(true);

    PIDataDictionaryServer dataDictionaryServer = RequestServer<IDataDictionaryServer>();
    if (!dataDictionaryServer) {
        LOGOUT(tr("DataDictionaryServer is not registered"), LOG_WARNING); // DataDictionaryServer未注册
    } else {
        PIDataDictionaryBuilder dataDictionaryBuilder = dataDictionaryServer->GetCurrentRuntimeDictionary();
        if (!dataDictionaryBuilder) {
            LOGOUT(tr("Currently, there is no running data dictionary"), LOG_WARNING); // 当前未有运行的数据字典
        } else {
            unsigned long long id = dataDictionaryBuilder->GetId();
            m_dataDictionaryViewController->LoadDataDictionary(id);
        }
    }
    QList<QTreeView *> allTree = dataDictionaryView->findChildren<QTreeView *>();
    if (allTree.isEmpty() || !allTree[0])
        return dataDictionaryView;
    mTree = allTree[0];
    allTree[0]->setSortingEnabled(true);
    allTree[0]->sortByColumn(0, Qt::AscendingOrder);
    allTree[0]->header()->setStyleSheet(R"(
                QHeaderView::section:horizontal
                {
                    border-top: none;
                    border-left: none;
                    border-right: none;
                    padding-left:10px;
                }
                )");
    allTree[0]->setStyleSheet(R"(QTreeView{margin:0px;})");
    return dataDictionaryView;
}

void DataDictionaryDialog::setTreeExpandState(QTreeView *varTree)
{
    if (!varTree)
        return;
    PIMonitorPanelPlotServer pImonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!pImonitorServer) {
        LOGOUT(tr("IMonitorPanelPlotServer is not registered"), LOG_WARNING); // IMonitorPanelPlotServer未注册
        return;
    }
    MonitorPanelPlotServer *pMonitorServer = static_cast<MonitorPanelPlotServer *>(pImonitorServer.data());
    if (!pMonitorServer) {
        LOGOUT(tr("MonitorPanelPlotServer is not found"), LOG_WARNING); // 未查询到MonitorPanelPlotServer
        return;
    }

    // 获取全部已添加变量路径(画板，原件，变量)
    QStringList allHasPut;
    QList<ItemPlot *> &allPlot = pMonitorServer->getAllItemPlots();
    if (allPlot.isEmpty())
        return;

    for (uint i = 0; i < allPlot.size(); ++i) {
        if (!allPlot[i])
            continue;
        QReadWriteLock &plotLock = allPlot[i]->getPlotLock();
        plotLock.lockForRead();
        const QStringList &pltPara = allPlot[i]->getPlotAllParas();
        plotLock.unlock();
        for (int j = 0; j < pltPara.size(); ++j) {
            if (allHasPut.contains(pltPara[j]))
                continue;
            allHasPut << pltPara[j];
        }
    }

    if (allHasPut.isEmpty()) {
        // varTree->expandAll();
        varTree->expandToDepth(1);
        return;
    }

    QAbstractItemModel *model = varTree->model();
    for (uint dicNum = 0; dicNum < varTree->model()->rowCount(); ++dicNum) {
        QModelIndex dicIndex = varTree->model()->index(dicNum, 0);
        const QString &dicName = dicIndex.data(Qt::DisplayRole).toString();

        for (uint boradNum = 0; boradNum < model->rowCount(dicIndex); ++boradNum) {
            QModelIndex BoardIndex = model->index(boradNum, 0, dicIndex);
            const QString &boardName = BoardIndex.data(Qt::DisplayRole).toString();

            for (uint elementNum = 0; elementNum < model->rowCount(BoardIndex); ++elementNum) {
                QModelIndex elementIndex = model->index(elementNum, 0, BoardIndex);
                const QString &tmpPath =
                        "Dictionary." + dicName + "." + boardName + "." + elementIndex.data(Qt::DisplayRole).toString();
                int tmpSize = 0;
                for (uint paraVecEle = 0; paraVecEle < allHasPut.size(); ++paraVecEle) {
                    if (allHasPut[paraVecEle].contains(tmpPath))
                        ++tmpSize;
                }
                if (tmpSize >= model->rowCount(elementIndex)) {
                    varTree->setExpanded(elementIndex, false);
                } else {
                    varTree->setExpanded(elementIndex, true);
                }
            }
            varTree->setExpanded(BoardIndex, true);
        }
        varTree->setExpanded(dicIndex, true);
    }
}

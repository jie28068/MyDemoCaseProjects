#include "MonitorPanelPlotServer.h"

#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "MonitorPanel/MonitorPanel.h"
#include "ProjectManager/IProjectResource.h"
#include "SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include "def.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <QApplication>

Kcc_SimuNPSAdapter_Namespace

#define ERRORPLOTNAME                                                                                                  \
    LOGOUT(QObject::tr("No Plot control with the specified title name was found"),                                     \
           LOG_ERROR); // 未查询到指定标题名的Plot控件
#define ERRORPLOT LOGOUT(QObject::tr("The specified Plot control was not found"), LOG_ERROR); // 未查询到指定Plot控件
#define LAYOUTERROR                                                                                                    \
    LOGOUT(QObject::tr("ICoreManUILayoutServer is not registered"), LOG_ERROR); // ICoreManUILayoutServer未注册

        KCC_USE_NAMESPACE_COREMANUI

                USE_LOGOUT_("MonitorPanelPlotServer")

                        MonitorPanelPlotServer::MonitorPanelPlotServer(void)
{
    isSimulating = false;
    mTransfer = new SendVarDataThread();
    QObject::connect(mTransfer, &SendVarDataThread::finished, mTransfer, &SendVarDataThread::deleteLater);
    mTransfer->start();
}

MonitorPanelPlotServer::~MonitorPanelPlotServer(void) { }

int MonitorPanelPlotServer::AddPlotCurve(int plotPos, const QString &dataDicElementFullPath)
{
    PICoreManUILayoutServer uiserver = RequestServer<ICoreManUILayoutServer>();
    if (!uiserver)
        -1;

    QList<QWidget *> &listWidget = uiserver->GetMainUI()->WorkAreaWidgets();
    for (QWidget *w : listWidget) {
        if (w->objectName() == "MonitorPanel") {
            MonitorPanel *panel = static_cast<MonitorPanel *>(w);
            if (panel) {
                QList<QWidget *> itemPlots = panel->getItemPlots();
                if (itemPlots.isEmpty() || plotPos >= itemPlots.size())
                    return -1;

                ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotPos]);
                if (itemPlot) {
                    bool bAdd = itemPlot->addCurve(QStringList() << dataDicElementFullPath);
                    return bAdd ? 0 : -1;
                }
            }
        }
    }
    return -1;
}

QStringList MonitorPanelPlotServer::GetPlotTitleList(void) const
{
    QStringList strList;
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return strList;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        strList.append(itemPlot->getTitle());
    }

    return strList;
}

QStringList MonitorPanelPlotServer::GetPlotIdList(void) const
{
    QStringList strList;
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return strList;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        strList.append(itemPlot->getId());
    }

    return strList;
}

bool MonitorPanelPlotServer::AddCurvesToPlot(const QStringList &variableFullPathList, const QString &plotTitle)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            itemPlot->addCurve(variableFullPathList);
            return true;
        }
    }
    ERRORPLOTNAME;
    return false;
}

bool MonitorPanelPlotServer::AddCurvesToPlot(const QStringList &variableFullPathList, int plotIndex)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return false;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return false;
    }
    itemPlot->addCurve(variableFullPathList);
    return true;
}

bool MonitorPanelPlotServer::AddCurvesToPlot(const PlotParam &plotParam, const QStringList &variableFullPathList,
                                             const QString &plotTitle)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            itemPlot->setPlotParam(plotParam);
            itemPlot->addCurve(variableFullPathList);
            return true;
        }
    }
    ERRORPLOTNAME;
    return false;
}

bool MonitorPanelPlotServer::AddCurvesToPlot(const PlotParam &plotParam, const QStringList &variableFullPathList,
                                             int plotIndex)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return false;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return false;
    }
    itemPlot->setPlotParam(plotParam);
    itemPlot->addCurve(variableFullPathList);
    return true;
}

void MonitorPanelPlotServer::ClearCurvesFromPlot(const QString &plotTitle)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            itemPlot->clear();
            return;
        }
    }
    ERRORPLOTNAME;
    return;
}

void MonitorPanelPlotServer::ClearCurvesFromPlot(int plotIndex)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return;
    }
    itemPlot->clear();

    return;
}

PlotPointer MonitorPanelPlotServer::AddNewPlot(const QString &plotTitle)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return nullptr;

    ItemPlot *itemPlot = panel->addItemPlot(plotTitle);
    return (PlotPointer)itemPlot;
}

bool MonitorPanelPlotServer::SetPlotParam(PlotPointer plot, const PlotParam &plotParam)
{
    if (!plot)
        return false;
    ItemPlot *itemPlot = (ItemPlot *)plot;
    itemPlot->setPlotParam(plotParam);
    return true;
}

bool MonitorPanelPlotServer::SetPlotParam(int plotIndex, const PlotParam &plotParam)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return false;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return false;
    }
    itemPlot->setPlotParam(plotParam);

    return true;
}

bool MonitorPanelPlotServer::SetPlotParam(const QString &plotTitle, const PlotParam &plotParam)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            itemPlot->setPlotParam(plotParam);
            return true;
        }
    }
    ERRORPLOTNAME;
    return false;
}

bool MonitorPanelPlotServer::AddCurveToPlot(PlotPointer plot, const QString &variableFullPath)
{
    if (!plot)
        return false;
    ItemPlot *itemPlot = (ItemPlot *)plot;
    return itemPlot->addCurve(QStringList() << variableFullPath);
}

bool MonitorPanelPlotServer::AddCurveToPlot(int plotIndex, const QString &variableFullPath)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return false;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return false;
    }
    return itemPlot->addCurve(QStringList() << variableFullPath);
}

bool MonitorPanelPlotServer::AddCurveToPlot(const QString &plotTitle, const QString &variableFullPath)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            return itemPlot->addCurve(QStringList() << variableFullPath);
        }
    }
    ERRORPLOTNAME;
    return false;
}

bool MonitorPanelPlotServer::ClearCurveDataFromPlot(PlotPointer plot, const QString &variableFullPath)
{
    if (!plot)
        return false;
    ItemPlot *itemPlot = (ItemPlot *)plot;
    itemPlot->clearCurveData(variableFullPath);

    return true;
}

bool MonitorPanelPlotServer::ClearCurveDataFromPlot(int plotIndex, const QString &variableFullPath)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return false;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return false;
    }
    itemPlot->clearCurveData(variableFullPath);

    return true;
}

bool MonitorPanelPlotServer::ClearCurveDataFromPlot(const QString &plotTitle, const QString &variableFullPath)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        //		qDebug() << "itemPlot getId:" << itemPlot->getId() << endl;
        if (itemPlot->getId() == plotTitle) {
            itemPlot->clearCurveData(variableFullPath);
            return true;
        }
    }
    ERRORPLOTNAME;
    return false;
}

bool MonitorPanelPlotServer::clearWidgets()
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    return panel->clearWidgets();
}

// 通过图表名字去删除
bool MonitorPanelPlotServer::removeWidgetFromTitle(const QString &title)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    return panel->removeWidgetFromTitle(title);
}

bool MonitorPanelPlotServer::AddCurvesToNewOnePlot(const QStringList &variableFullPathList, const QString &plotTitle)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;

    ItemPlot *itemPlot = panel->addItemPlot(plotTitle);
    itemPlot->addCurve(variableFullPathList);
    return true;
}

bool MonitorPanelPlotServer::AddCurvesToNewOnePlot(const PlotParam &plotParam, const QStringList &variableFullPathList)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    ItemPlot *itemPlot = panel->addItemPlot("");
    itemPlot->setPlotParam(plotParam);
    itemPlot->addCurve(variableFullPathList);
    return true;
}

bool MonitorPanelPlotServer::AddCurvesToEachNewPlot(const QStringList &variableFullPathList,
                                                    const QStringList &plotTitleList)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;

    for (int i = 0; i < variableFullPathList.size(); i++) {
        QString plotTitle;
        if (i < plotTitleList.size())
            plotTitle = plotTitleList[i];
        ItemPlot *itemPlot = panel->addItemPlot(plotTitle);
        itemPlot->addCurve(QStringList() << variableFullPathList[i]);
    }

    return true;
}

void MonitorPanelPlotServer::ClearCurveDataFromAllPlot(const QStringList &variableFullPathList)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        for (const QString &variableFullPath : variableFullPathList) {
            itemPlot->clearCurveData(variableFullPath);
        }
    }
    return;
}

void MonitorPanelPlotServer::SetDefaultXAxisLength(qreal length)
{
    gConfSet(DefaultXAxisLength, length);
}

bool MonitorPanelPlotServer::setPlotProject(const PlotProject &plotPro) // 设置本图表用于哪个项目
{
    if (plotPro == QuiKIS) //
    {
        GlobalConfigs::getInstance()->set("PlotProject", QVariant(plotPro));
        return true;
    } else {
        MonitorPanel *panel = getMonitorPanel();
        if (!panel)
            return false;
        return panel->setPlotProject(plotPro);
    }
}

bool MonitorPanelPlotServer::setCutType(const CutType &cutType)
{
    GlobalConfigs::getInstance()->set("CutType", QVariant(cutType));
    return true;
}

bool MonitorPanelPlotServer::unmergePlotFromPlotId(const QString &plotId) // 通过图表id去拆分绘图
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotId) {
            QList<PlotInfo> plotInfos = itemPlot->unmergePlot();
            if (plotInfos.size() > 0) {
                ItemPlot *plot = panel->addItemPlot("");
                plot->setUnmergePlotInfo(plotInfos[0]);
            }
            return true;
        }
    }
    ERRORPLOTNAME;
    return false;
}

// 设置图表哪些功能要，哪些功能不要
bool MonitorPanelPlotServer::setPlotFunc(const PlotFunc &plotFunc)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    return panel->setPlotFunc(plotFunc);
}

bool MonitorPanelPlotServer::setPlotMenuVisibleState()
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    return panel->setPlotMenuVisibleState();
}

// 设置绘图对应触发的按钮事件
void MonitorPanelPlotServer::setToolbarTriggered(int index)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;
    return panel->setToolbarTriggered(index);
}

void MonitorPanelPlotServer::setLayoutChoosed(int rows, int cols) // 设置布局，几行几列
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;
    return panel->setLayoutChoosed(rows, cols);
}

bool MonitorPanelPlotServer::AddMarkerToPlot(PlotPointer plot, int markerType)
{
    if (!plot)
        return false;
    ItemPlot *itemPlot = (ItemPlot *)plot;

    MarkerInfo info;
    info.type = (MarkerType)markerType;
    itemPlot->addMarker(info);
    return true;
}

bool MonitorPanelPlotServer::AddMarkerToPlot(int plotIndex, int markerType)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> itemPlots = panel->getItemPlots();
    if (plotIndex >= itemPlots.size()) {
        ERRORPLOT;
        return false;
    }
    ItemPlot *itemPlot = static_cast<ItemPlot *>(itemPlots[plotIndex]);
    if (!itemPlot) {
        ERRORPLOT;
        return false;
    }
    MarkerInfo info;
    info.type = (MarkerType)markerType;
    itemPlot->addMarker(info);

    return true;
}

bool MonitorPanelPlotServer::AddMarkerToPlot(const QString &plotTitle, int markerType)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            MarkerInfo info;
            info.type = (MarkerType)markerType;
            itemPlot->addMarker(info);
            return true;
        }
    }
    ERRORPLOTNAME;
    return false;
}

// panelName是当前区域名, plotTitle为图表名称，isEnable true为启用，false为禁用
bool MonitorPanelPlotServer::setButtonEnable(const QString &panelName, const QString &plotTitle,
                                             RunButtonType buttonType, bool isEnable)
{
    MonitorPanel *panel = getMonitorPanel(panelName);
    if (!panel)
        return false;
    QList<QWidget *> &itemPlots = panel->getItemPlots();

    for (QWidget *w : itemPlots) {
        ItemPlot *itemPlot = static_cast<ItemPlot *>(w);
        if (!itemPlot)
            continue;
        if (itemPlot->getId() == plotTitle) {
            return itemPlot->setButtonEnable(buttonType, isEnable);
        }
    }
    ERRORPLOTNAME;
    return false;
}

void MonitorPanelPlotServer::SetMonitorPanelName(const QString &panelName)
{
    QString name = panelName;
    int pos = name.lastIndexOf('*');
    if (pos == name.length() - 1) {
        name.remove(pos, 1);
    }
    m_panelName = name;
}

bool MonitorPanelPlotServer::AddMarkerToSelectedPlots(int markerType)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return false;
    QList<ItemPlot *> &itemPlots = panel->getSelectedPlots();
    MarkerInfo info;
    info.type = (MarkerType)markerType;
    for (ItemPlot *plot : itemPlots) {
        plot->addMarker(info);
    }

    return true;
}

void MonitorPanelPlotServer::SetPlotDefaultProperty(const PlotDefaultProperty &property)
{
    gDefault->setDefaultProperty(property);
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;
    QList<ItemPlot *> &itemPlots = panel->getAllPlots();
    for (ItemPlot *plot : itemPlots) {
        plot->SetPlotDefaultProperty(property);
    }
}

void MonitorPanelPlotServer::addEllipse(const QString &plotTitle, const QPointF &pos, qreal radius, const QColor &color,
                                        int width)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;

    QList<ItemPlot *> &plots = panel->getAllPlots();
    for (ItemPlot *plot : plots) {
        if (plot->getId() == plotTitle) {
            plot->addEllipse(pos, radius, color, width);
            return;
        }
    }
}

void MonitorPanelPlotServer::clearEllipse(const QString &plotTitle)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;

    QList<ItemPlot *> &plots = panel->getAllPlots();
    for (ItemPlot *plot : plots) {
        if (plot->getId() == plotTitle) {
            plot->clearEllipse();
            return;
        }
    }
}

void MonitorPanelPlotServer::DeleteCurveFromPlot(const QString &plotTitle, const QString &variableFullPath,
                                                 const QString &curveId)
{
    MonitorPanel *panel = getMonitorPanel();
    if (!panel)
        return;

    QList<ItemPlot *> &plots = panel->getAllPlots();
    for (ItemPlot *plot : plots) {
        if (plot->getId() == plotTitle) {
            plot->DeleteCurve(variableFullPath, curveId);
            return;
        }
    }
}

MonitorPanel *MonitorPanelPlotServer::getMonitorPanel(const QString &panelName) const
{
    PICoreManUILayoutServer uiserver = RequestServer<ICoreManUILayoutServer>();
    if (!uiserver) {
        LAYOUTERROR;
        return nullptr;
    }

    if (panelName.isEmpty()) {
        QList<QWidget *> &listWidget = uiserver->GetMainUI()->WorkAreaWidgets();
        for (QWidget *w : listWidget) {
            if (w->objectName() == "MonitorPanel") {
                MonitorPanel *panel = static_cast<MonitorPanel *>(w);
                if (!panel)
                    continue;
                return panel;
            }
        }
    } else {
        QWidget *w = uiserver->GetMainUI()->WorkAreaWidget(panelName);
        if (w) {
            MonitorPanel *panel = dynamic_cast<MonitorPanel *>(w);
            if (panel) {
                return panel;
            }
        }
    }

    // LOGOUT(QObject::tr("未查询到监控面板"),LOG_ERROR);
    return nullptr;
}

MonitorPanel *MonitorPanelPlotServer::getMonitorPanel(void) const
{
    PICoreManUILayoutServer uiserver = RequestServer<ICoreManUILayoutServer>();
    if (!uiserver) {
        LAYOUTERROR;
        return nullptr;
    }

    if (m_panelName.isEmpty()) {
        QList<QWidget *> &listWidget = uiserver->GetMainUI()->WorkAreaWidgets();
        for (QWidget *w : listWidget) {
            if (w->objectName() == "MonitorPanel") {
                MonitorPanel *panel = static_cast<MonitorPanel *>(w);
                if (!panel)
                    continue;
                return panel;
            }
        }
    } else {
        QWidget *w = uiserver->GetMainUI()->WorkAreaWidget(m_panelName);
        if (w) {
            MonitorPanel *panel = dynamic_cast<MonitorPanel *>(w);
            if (panel) {
                return panel;
            }
        }
    }

    // LOGOUT(QObject::tr("未查询到监控面板"),LOG_ERROR);
    return nullptr;
}

PlotActionCheckedState MonitorPanelPlotServer::GetPlotTooBarCheckedState()
{
    MonitorPanel *monitorPanel = getMonitorPanel();
    PlotActionCheckedState pcs;
    if (monitorPanel)
        pcs = monitorPanel->GetPlotTooBarCheckedState();
    return pcs;
}

void MonitorPanelPlotServer::setPlotMenuCheckedState()
{
    PICoreManUILayoutServer pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!pUIServer) {
        // LOGOUT("IWorkareaServer未注册", LOG_ERROR);
        return;
    }
    ICoreManUILayout *ui = pUIServer->GetMainUI();
    if (!ui)
        return;

    PlotActionCheckedState pcs = GetPlotTooBarCheckedState();
    QAction *syncPlotAct = ui->GetRegisterAction("PluginPlotViewsyncPlot");

    bool isChecked = false;
    // Sync
    if (syncPlotAct) {
        isChecked = syncPlotAct->isChecked();
        if (isChecked != pcs.isSyncCheckboxActionChecked)
            syncPlotAct->setChecked(pcs.isSyncCheckboxActionChecked);
    }

    // XScale
    QAction *XScalePlotAct = ui->GetRegisterAction("PluginPlotViewXScalePlot");
    if (XScalePlotAct) {
        isChecked = XScalePlotAct->isChecked();
        if (isChecked != pcs.isXScaleActionChecked)
            XScalePlotAct->setChecked(pcs.isXScaleActionChecked);
    }

    // YScale
    QAction *YScalePlotAct = ui->GetRegisterAction("PluginPlotViewYScalePlot");
    if (YScalePlotAct) {
        isChecked = YScalePlotAct->isChecked();
        if (isChecked != pcs.isYScaleActionChecked)
            YScalePlotAct->setChecked(pcs.isYScaleActionChecked);
    }

    // MousePlot
    QAction *mousePlot = ui->GetRegisterAction("PluginPlotViewmousePlot");
    if (mousePlot) {
        isChecked = mousePlot->isChecked();
        if (isChecked != pcs.isMousePointActionChecked)
            mousePlot->setChecked(pcs.isMousePointActionChecked);
    }
}

void MonitorPanelPlotServer::exportAllPlotTo(QString workAreaName, QString dirName, ExportGraphType graphType)
{
    if (workAreaName.isEmpty() || dirName.isEmpty() || 0 > graphType)
        return;

    PICoreManUILayoutServer pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!pUIServer) {
        LAYOUTERROR;
        return;
    }
    ICoreManUILayout *uiServer = pUIServer->GetMainUI();
    if (nullptr == uiServer)
        return;

    QStringList graphList;
    graphList << ".png"
              << ".jpeg"
              << ".bmp";

    int count = graphList.count();
    if (graphType >= count)
        return;
    QString suffix = graphList.at(graphType);
    QPixmap pixmap;

    if (!dirName.endsWith(QDir::separator()))
        dirName += QDir::separator();
    QString strFile = dirName + workAreaName + suffix;

    QWidget *w; // = uiServer->WorkAreaWidget(workAreaName);
    MonitorPanel *monitorPanel = getMonitorPanel(workAreaName);
    if (!monitorPanel)
        return;
    w = monitorPanel->getScrollAreaWidget();
    if (nullptr == w)
        return;
    pixmap = QPixmap::grabWidget(w);
    pixmap.save(strFile);

    LOGOUT("图形导出成功");
}

void MonitorPanelPlotServer::ShowDataAnalyseInfoPage(const QString &panelName, DataInfoType dit)
{
    // MonitorPanel* monitorPanel = getMonitorPanel(panelName);
    MonitorPanel *monitorPanel = getMonitorPanel();
    if (!monitorPanel)
        return;
    monitorPanel->showDataAnalyseInfoPage(dit);
}

void MonitorPanelPlotServer::ModifyCurveName(const QString &panelName, const QString &plotId,
                                             const QString &variableFullPath, const QString &curveName)
{
    MonitorPanel *monitorPanel = getMonitorPanel();
    if (!monitorPanel)
        return;
    monitorPanel->modifyCurveName(plotId, variableFullPath, curveName);
}

void MonitorPanelPlotServer::rebindVar()
{
    MonitorPanel *pan = getMonitorPanel();
    if (!pan)
        return;
    pan->rebindVar();
}

void MonitorPanelPlotServer::removeBoardVar(const QStringList &paras)
{
    if (paras.isEmpty())
        return;

    ProjectManager::PIProjectManagerServer pProjectMng = RequestServer<ProjectManager::IProjectManagerServer>();
    if (!pProjectMng)
        return;
    QList<ItemPlot *> &allItem = getAllItemPlots();

    QString prefix = "Dictionary.";
    QString xmlFilePath = pProjectMng->GetCurrentProjectAbsoluteDir();
    xmlFilePath += "/MonitorPanel.xml";

    QFile file(xmlFilePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QDomDocument doc;
    doc.setContent(&file);
    file.close();
    QDomElement &root = doc.documentElement();
    if (root.isNull())
        return;
    QDomElement &panel = root.firstChildElement("panel");
    if (panel.isNull())
        return;

    QList<PlotInfo> infoList;
    QDomNodeList &plotList = panel.childNodes();

    for (auto &para : paras) {
        QString partPathNeedDel = prefix + para;
        for (uint i = 0; i < allItem.size(); ++i) {
            allItem[i]->DeleteCurve(partPathNeedDel, "");
        }

        for (int i = 0; i < plotList.size(); ++i) {
            QDomElement &plot = plotList.at(i).toElement();
            if (plot.tagName() != "plot")
                continue;

            QDomElement &varibles = plot.firstChildElement("varibles");
            if (varibles.isNull())
                continue;

            QDomNodeList &varList = varibles.childNodes();
            for (int j = 0; j < varList.size();) {
                QDomElement &varible = varList.at(j).toElement();
                if (varible.tagName() != "varible") {
                    ++j;
                    continue;
                }

                if (varible.attribute("Path").isNull())
                    continue;
                QString tmpPath = varible.attribute("Path");
                if (tmpPath.contains(partPathNeedDel)) {
                    varibles.removeChild(varList.at(j));
                    j = 0;
                } else {
                    ++j;
                }
            }
        }
    }
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return;
    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

bool MonitorPanelPlotServer::refreshBindedVarbleInfo(bool isActive, const QString &boardName)
{
    MonitorPanel *mpn = getMonitorPanel();
    if (!mpn)
        return false;

    QList<ItemPlot *> &allItemPlots = mpn->getAllPlots();
    if (allItemPlots.empty())
        return false;

    QStringList allVar;
    for (uint i = 0; i < allItemPlots.size(); ++i) {
        if (!allItemPlots[i])
            continue;
        QStringList &tmpList = allItemPlots[i]->refreshBindedVar(isActive, boardName);
        for (auto &var : tmpList) {
            if (allVar.contains(var))
                continue;
            allVar << var;
        }
    }
    QMap<QString, QVariant> params;
    params["variablePaths"] = allVar;
    emitNotify(CODE_ADDVARIABLE, params);
    return true;
}

void MonitorPanelPlotServer::renameBoardVar(const QMap<QString, QString> &paras)
{
    if (paras.isEmpty())
        return;
    ProjectManager::PIProjectManagerServer pProjectMng = RequestServer<ProjectManager::IProjectManagerServer>();
    if (!pProjectMng)
        return;
    QList<ItemPlot *> &allItem = getAllItemPlots();

    for (uint i = 0; i < allItem.size(); ++i) {
        allItem[i]->renameVar(paras);
    }

    QString prefix = "Dictionary.";
    QString xmlFilePath = pProjectMng->GetCurrentProjectAbsoluteDir();
    xmlFilePath += "/MonitorPanel.xml";
    QFile file(xmlFilePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QDomDocument doc;
    doc.setContent(&file);
    file.close();
    QDomElement &root = doc.documentElement();
    if (root.isNull())
        return;

    QDomElement &panel = root.firstChildElement("panel");
    if (panel.isNull())
        return;

    QDomNodeList &plotList = panel.childNodes();
    for (auto it = paras.begin(); it != paras.end(); ++it) {
        QStringList &inputOldSplited = it.key().split(".");
        QStringList &inputNewSplited = it.value().split(".");
        if (inputOldSplited.isEmpty() || inputNewSplited.isEmpty() || inputOldSplited.size() > 4
            || inputOldSplited.size() != inputNewSplited.size()) {
            continue;
        }

        for (int i = 0; i < plotList.size(); ++i) {
            QDomElement &plot = plotList.at(i).toElement();
            if (plot.tagName() != "plot")
                continue;

            QDomElement &varibles = plot.firstChildElement("varibles");
            if (varibles.isNull())
                continue;

            QDomNodeList &varList = varibles.childNodes();
            for (int j = 0; j < varList.size(); j++) {
                QDomElement &varible = varList.at(j).toElement();
                if (varible.tagName() != "varible")
                    continue;

                if (varible.attribute("Path").isNull())
                    continue;
                QString tmpPath = varible.attribute("Path");
                if (!tmpPath.contains(prefix + it.key()))
                    continue;

                QStringList &varSplited = tmpPath.split(".");
                if (varSplited.size() != 5u)
                    continue;

                for (int inputIndex = 0; inputIndex < inputNewSplited.size(); ++inputIndex) {
                    if (inputIndex + 1 > varSplited.size() - 1)
                        continue;
                    varSplited[inputIndex + 1] = inputNewSplited[inputIndex];
                }

                QString newVar = varSplited.join(".");

                QString oldName = tmpPath.split(".").back();
                varible.setAttribute("Path", newVar);
                varible.setAttribute("Name", varSplited.back());
                varible.setAttribute("Alias", varSplited[3] + "." + varSplited.back());

                QDomNodeList &curveNodes = varible.childNodes();
                for (int curveIndex = 0; curveIndex < curveNodes.size(); ++curveIndex) {
                    QDomElement &curveVar = curveNodes.at(curveIndex).toElement();
                    if (curveVar.tagName() != "curve")
                        continue;
                    if (curveVar.attribute("Name").isNull())
                        continue;
                    QString curvename = curveVar.attribute("Name");
                    if (curvename == oldName) {
                        curveVar.setAttribute("Name", varSplited.back());
                    }
                    break;
                }
            }
        }
    }

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return;
    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

void MonitorPanelPlotServer::setTransferVar(const QStringList &varList)
{
    if (!mTransfer || varList.empty())
        return;
    mTransfer->setVarList(varList);
}

QList<ItemPlot *> MonitorPanelPlotServer::getAllItemPlots()
{
    QList<ItemPlot *> allPlots;
    MonitorPanel *pane = getMonitorPanel();
    if (!pane)
        return allPlots;
    allPlots = pane->getAllPlots();

    return allPlots;
}

bool MonitorPanelPlotServer::getSimulateState()
{
    return isSimulating;
}

void MonitorPanelPlotServer::setSimulateState(bool b)
{
    isSimulating = b;
}

SendVarDataThread::SendVarDataThread(QObject *parent) : QThread(parent) { }

SendVarDataThread::~SendVarDataThread() { }

void SendVarDataThread::setVarList(const QStringList &allVar)
{
    mVariableListQueue.put(allVar);
}

void SendVarDataThread::run()
{
    while (true) {
        const QStringList &variableList = mVariableListQueue.get();
        PISimuNPSDataDictionaryServer pSimuNPSDataDictionaryServer = RequestServer<ISimuNPSDataDictionaryServer>();
        if (!pSimuNPSDataDictionaryServer)
            continue;
        Kcc::ProjectManager::PIProjectManagerServer projectMng =
                RequestServer<Kcc::ProjectManager::IProjectManagerServer>();
        if (!projectMng)
            continue;

        QSet<QString> storageNames;
        for (auto &var : variableList) {
            QStringList tempUse = var.split(".");
            if (tempUse.size() != 5u)
                continue;
            storageNames << tempUse[1];
        }
        if (storageNames.isEmpty())
            continue;
        LOGOUT(QObject::tr("[Monitor Panel] Reading simulation results, please wait"), LOG_NORMAL);
        QMap<QString, QSharedPointer<QMap<QString, QMap<QString, QMap<quint64, double>>>>> allNeeded;
        for (auto &tempName : storageNames) {
            allNeeded[tempName] = projectMng->GetAllSimulationResultData(tempName);
        }

        for (auto &variableName : variableList) {
            QStringList nameList = variableName.split(".");
            if (nameList.size() != 5)
                continue;
            QString &storage = nameList[1];
            QString &boardName = nameList[2];
            QSharedPointer<QMap<QString, QMap<QString, QMap<quint64, double>>>> allData = allNeeded[storage];
            if (allData.isNull()) {
                continue;
            }
            QString varName = nameList[2] + "." + nameList[3] + "." + nameList[4];
            if (!(*allData).keys().contains(boardName)) {
                for (auto &tempBoardName : allData->keys()) {
                    for (auto &tempVarName : (*allData)[tempBoardName].keys()) {
                        if (tempVarName == varName) {
                            boardName = tempBoardName;
                            break;
                        }
                    }
                }
            }
            QString dicName = storage + "." + varName;
            if (!pSimuNPSDataDictionaryServer->isHaveDictionaryElement(dicName)) {
                continue;
            }

            if (!(*allData).keys().contains(boardName))
                continue;

            if (!(*allData)[boardName].keys().contains(varName))
                continue;
            const QMap<quint64, double> &data = (*allData)[boardName][varName];
            if (data.isEmpty()) {
                continue;
            }

            const int count = data.size() + 1; // 最后一个是标志，表明数据来自数据回放
            if (count == 0)
                continue;

            double *dataArray = new double[count];
            quint64 firstTime = 0.0;
            quint64 space = 1.0;
            int order = 0;
            for (auto &time : data.keys()) {
                if (order == 0)
                    firstTime = time;
                if (order == 1)
                    space = time - firstTime;
                const double &tmpVal = data[time];
                dataArray[order] = tmpVal;
                ++order;
            }
            // 附加一个参数作为标志，InputChannelData暂时无法附加数据，暂时这么写
            dataArray[order] = DBL_MAX;
            if (pSimuNPSDataDictionaryServer->isHaveDictionaryElement(dicName)) {
                pSimuNPSDataDictionaryServer->InputChannelData(dicName, dataArray, count, firstTime, space);
            }
            delete[] dataArray;
        }
        LOGOUT(QObject::tr("[Monitor panel] Simulation result reading completed"), LOG_NORMAL);
    }
}

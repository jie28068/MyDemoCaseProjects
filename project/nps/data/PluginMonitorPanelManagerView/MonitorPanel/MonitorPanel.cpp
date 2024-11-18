#include "MonitorPanel.h"
#include "CoreLib/DocHelper.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "PlotWidget.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "ScopeViewToolbar.h"
#include <QScrollArea>
#include <QVBoxLayout>

USE_LOGOUT_("MonitorPanel")
REG_WORKAREA(MonitorPanel)

MonitorPanel::MonitorPanel() : m_monitorWidget(nullptr)

{
    setMinimumHeight(228);
    setObjectName("MonitorPanel");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_monitorWidget = new MonitorWidget(nullptr);
    layout->addWidget(m_monitorWidget);

    if ((PlotProject)gConfGet("PlotProject").toInt() != QuiKIS) {
        // connect(m_monitorWidget, SIGNAL(modifyNotify()), this, SLOT(onContensChanged()));
        connect(m_monitorWidget, SIGNAL(modifyNotify()), this, SLOT(save()));
    }

    setStyleSheet("MonitorPanel{border: 0px solid;}");

    connect(this, SIGNAL(fullScreenModeSignal(bool)), this, SLOT(onFullScreen(bool)));
}

MonitorPanel::~MonitorPanel()
{
    // save();
}

bool MonitorPanel::init(const QMap<QString, QVariant> &paramMap)
{
    BasicWorkareaContentWidget::init(paramMap);
    // QString strPanelFilePath = paramMap.value("PanelFilePath").toString();
    m_paramMap = paramMap;

    // QString strTitle = getWorkAreaTitle();
    // QStringList strList = strTitle.split("@");
    // if(strList.size() >= 2)
    //{
    //	m_monitorWidget->setCurrentProjectName(strList[0]);
    // }
    // if (m_paramMap["PanelFilePath"].toString().isEmpty())
    //     return false;
    Kcc::ProjectManager::PIProjectManagerServer pProjectManager =
            RequestServer<Kcc::ProjectManager::IProjectManagerServer>();
    if (pProjectManager) {
        QString prjPath = pProjectManager->GetCurrentProjectAbsoluteDir();
        if (!prjPath.isEmpty()) {
            QString truepath = prjPath + "/MonitorPanel.xml";
            if (truepath != m_paramMap["PanelFilePath"].toString()) {
                m_paramMap["PanelFilePath"] = truepath;
            }
        }
    }

    load();
    m_monitorWidget->initIsRunning();
    return true;
}

void MonitorPanel::rebindVar()
{
    m_monitorWidget->initIsRunning();
    return;
}

QIcon MonitorPanel::icon()
{
    return QIcon(":/recode_default");
}

void MonitorPanel::Save()
{
    save();
}

QList<QWidget *> MonitorPanel::getItemPlots()
{
    QList<QWidget *> &tempList = m_monitorWidget->plotScrollAreaWidgets();
    QList<QWidget *> plotList;
    for (QWidget *w : tempList) {
        plotList.append(((PlotWidget *)w)->plot());
    }

    return plotList;
}

QList<QWidget *> MonitorPanel::getPlotWidgets()
{
    return m_monitorWidget->plotScrollAreaWidgets();
}

QList<ItemPlot *> MonitorPanel::getSelectedPlots(void) const
{
    QList<QWidget *> &selectedWidgets = m_monitorWidget->selectedWidgets();
    QList<ItemPlot *> selectedPlots;
    for (QWidget *w : selectedWidgets) {
        selectedPlots.append(static_cast<PlotWidget *>(w)->plot());
    }
    return selectedPlots;
}

QList<QWidget *> MonitorPanel::getSelectedWidgets(void) const
{
    QList<QWidget *> selectedWidgets = m_monitorWidget->selectedWidgets();
    return selectedWidgets;
}

QList<ItemPlot *> MonitorPanel::getAllPlots() const
{
    QList<QWidget *> &widgets = m_monitorWidget->plotScrollAreaWidgets();
    QList<ItemPlot *> items;
    for (QWidget *w : widgets) {
        items.append(static_cast<PlotWidget *>(w)->plot());
    }
    return items;
}

void MonitorPanel::refreshBackgroundColor(void)
{
    QList<ItemPlot *> &allPlots = getAllPlots();
    for (ItemPlot *plot : allPlots) {
        plot->refreshBackgroundColor();
    }
}

void MonitorPanel::showDataAnalyseInfoPage(DataInfoType dit)
{
    QList<QWidget *> &selectedWidgets = m_monitorWidget->selectedWidgets();
    for (QWidget *w : selectedWidgets) {
        static_cast<PlotWidget *>(w)->showDataAnalyseInfoPage(dit);
    }
}

void MonitorPanel::modifyCurveName(const QString &plotId, const QString &variableFullPath, const QString &curveName)
{
    QList<ItemPlot *> &allPlots = getAllPlots();
    for (ItemPlot *plot : allPlots) {
        if (plot->getId() != plotId)
            continue;
        plot->modifyCurveName(variableFullPath, curveName);
        break;
    }
}

bool MonitorPanel::setPlotProject(const PlotProject &plotProject)
{
    GlobalConfigs::getInstance()->set("PlotProject", QVariant(plotProject));
    return true;
}

bool MonitorPanel::setPlotFunc(const PlotFunc &plotFunc)
{
    if (m_monitorWidget) {
        ScopeViewToolbar *toolBar = m_monitorWidget->getScopeViewToolbar();
        if ((CutType)gConfGet("CutType").toInt() == UltimateCut) {
            toolBar->hide();
            return true;
        } else {
            return toolBar->setPlotFunc(plotFunc);
        }
    }
    return false;
}

void MonitorPanel::save()
{
    QString strTemp = m_paramMap["PanelFilePath"].toString();
    m_monitorWidget->save(strTemp);
}

void MonitorPanel::load()
{
    QString strTemp = m_paramMap["PanelFilePath"].toString();
    m_monitorWidget->load(strTemp);
}

void MonitorPanel::setToolbarTriggered(int index)
{
    if (m_monitorWidget)
        m_monitorWidget->setToolbarTriggered(index);
}

void MonitorPanel::setLayoutChoosed(int rows, int cols) // éè??2???￡???DD??áD
{
    if (m_monitorWidget)
        m_monitorWidget->onLayoutChoosed(rows, cols);
}

bool MonitorPanel::setPlotMenuVisibleState()
{
    if (m_monitorWidget) {
        ScopeViewToolbar *toolBar = m_monitorWidget->getScopeViewToolbar();
        if (toolBar)
            toolBar->setPlotMenuVisibleState();
        return true;
    }
    return false;
}

PlotActionCheckedState MonitorPanel::GetPlotTooBarCheckedState()
{
    PlotActionCheckedState pcs;
    if (nullptr == m_monitorWidget)
        return pcs;

    ScopeViewToolbar *toolBar = m_monitorWidget->getScopeViewToolbar();
    if (nullptr == toolBar)
        return pcs;

    pcs = toolBar->GetPlotTooBarCheckedState();

    return pcs;
}

void MonitorPanel::onFullScreen(bool b)
{
    refreshBackgroundColor();
}

QWidget *MonitorPanel::getScrollAreaWidget()
{
    if (!m_monitorWidget)
        return nullptr;

    PlotScrollArea *scrollArea = m_monitorWidget->getScrollArea();
    if (!scrollArea)
        return nullptr;

    return scrollArea->widget();
}
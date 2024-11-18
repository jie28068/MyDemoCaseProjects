#include "MonitorWidget.h"
#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerManager.h"
#include "MonitorPanel/PlotLayoutChooseDialog.h"
#include "MonitorPanel/PlotScrollArea.h"
#include "MonitorPanel/PlotWidget.h"
#include "MonitorPanel/ScopeViewToolbar.h"
#include "MonitorPanel/def.h"
#include "server/UILayout/ICoreMainUIServer.h"
// #include <Windows.h>
#include "DataDictionaryDialog.h"
#include <QtConcurrent>

KCC_USE_NAMESPACE_MONITORPANEL
KCC_USE_NAMESPACE_COREMANUI

// #define TEST
#ifdef TEST
#include "server/MonitorPanel/IMonitorPanelPlotServer.h"
KCC_USE_NAMESPACE_MONITORPANEL
#endif
#include "MonitorPanelPlotServer.h"
#include "SimulationManagerServer/ISimulationManagerServer.h"
using namespace Kcc::SimulationManager;

USE_LOGOUT_("MonitorWidget")

MonitorWidget::MonitorWidget(QWidget *parent)
    : QFrame(parent),
      m_Toolbar(nullptr),
      m_ScrollArea(nullptr),
      m_plotLayoutChooseDialog(nullptr),
      m_isAcceptNotify(true),
      // m_isExternTrigger(false),
      mPlaceHolder(nullptr),
      mDrawSeprator(nullptr),
      mUndoStack(nullptr),
      m_dataDictionaryDialog(nullptr)
{
    mUndoStack = new QUndoStack(this);
    mUndoStack->setUndoLimit(5);
    setObjectName("MonitorWidget");
    this->setMinimumSize(1, 1);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    m_Toolbar = new ScopeViewToolbar();
    layout->addWidget(m_Toolbar);

    mPlaceHolder = new QWidget();
    mDrawSeprator = new QWidget(mPlaceHolder);
    mDrawSeprator->setStyleSheet("QWidget{background-color:#cecece}");
    layout->addWidget(mPlaceHolder);

    m_ScrollArea = new PlotScrollArea(this);
    // m_ScrollArea->installEventFilter(this);
    connect(m_ScrollArea, SIGNAL(modifyNotify()), this, SIGNAL(modifyNotify()));
    connect(m_ScrollArea, &PlotScrollArea::selectPlot, this, &MonitorWidget::setToolBarCopyCutDelBtnState);
    connect(m_ScrollArea, &PlotScrollArea::dragpPloting, this, &MonitorWidget::onDragPloting);
    connect(m_ScrollArea, &PlotScrollArea::dragPlotFinish, this, &MonitorWidget::onDragPlotFinsh);
    layout->addWidget(m_ScrollArea);
    QObject::connect(m_Toolbar, SIGNAL(triggered(int)), this, SLOT(onToolbarTriggered(int)));

    connect(mUndoStack, &QUndoStack::canUndoChanged, this, &MonitorWidget::setUndobtnEnable);
    connect(mUndoStack, &QUndoStack::canRedoChanged, this, &MonitorWidget::setRedobtnEnable);

    PIServerInterfaceBase dataDictionaryServer = RequestServerInterface<
            IDataDictionaryServer>(); // RequestServer(SERVER_GROUP_DATADICTIONARY_NAME,SERVER_INTERFACE_DATADICTIONARY_NAME);
    if (!dataDictionaryServer) {
        LOGOUT(tr("DataDictionaryServer is not loaded"), LOG_ERROR); // DataDictionaryServer 未加载
    } else {
        dataDictionaryServer->connectNotify(Notify_StartRunTime, this,
                                            SLOT(onDataDictionaryServerNotify(unsigned int, const NotifyStruct &)));
        dataDictionaryServer->connectNotify(Notify_StopRunTime, this,
                                            SLOT(onDataDictionaryServerNotify(unsigned int, const NotifyStruct &)));
    }

    QPalette p;
    p.setColor(QPalette::Background, Qt::white);
    this->setPalette(p);
    // Qt5 fix
    mExportPicPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    PIServerInterfaceBase pSm = RequestServerInterface<ISimulationManagerServer>();
    if (pSm) {
        pSm->connectNotify(Notify_DrawingBoardRunning, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSm->connectNotify(Notify_DrawingBoardStopped, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSm->connectNotify(Notify_SimulationPaused, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
        pSm->connectNotify(Notify_SimulationResume, this,
                           SLOT(onReceiveSimulationMessage(unsigned int, const NotifyStruct &)));
    }

    PIMonitorPanelPlotServer iMonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!iMonitorServer)
        return;
    MonitorPanelPlotServer *monitor = static_cast<MonitorPanelPlotServer *>(iMonitorServer.data());
    if (!monitor)
        return;
    if (monitor->getSimulateState()) {
        onSimulatingUpdateBtnState();
    } else {
        onSimulatingStopUpdateBtnState();
    }
}

MonitorWidget::~MonitorWidget()
{
    if (m_plotLayoutChooseDialog)
        delete m_plotLayoutChooseDialog;
}

QList<QWidget *> MonitorWidget::plotScrollAreaWidgets()
{
    return m_ScrollArea->widgets();
}

QList<QWidget *> MonitorWidget::selectedWidgets() const
{
    return m_ScrollArea->selectedWidgets();
}

ItemPlot *MonitorWidget::addItemPlot(const QString &strTitle)
{
    QString plotTitle;
    if (strTitle.length() > 0)
        plotTitle = strTitle;
    else
        plotTitle = QString("plot%1").arg(m_ScrollArea->widgetCount() + 1);
    PlotWidget *plotWidget = new PlotWidget(plotTitle, m_ScrollArea);
    plotWidget->plot()->setCurrentProjectName(m_projectName);
    m_ScrollArea->addWidget(plotWidget);
    m_ScrollArea->changeLayoutAfterAdd();
    int ncount = m_ScrollArea->widgetCount();
    // if (ncount > m_ScrollArea->rows() * m_ScrollArea->cols()) {
    //	updateLayout();
    // }
    int rows = ncount / m_ScrollArea->cols();
    int rem = ncount % m_ScrollArea->cols();
    if (rem > 0)
        rows += 1;
    int maxrow = qFloor(m_ScrollArea->height() / plotWidget->height());
    if (rows > maxrow) {
        m_ScrollArea->verticalScrollBar()->setValue(m_ScrollArea->verticalScrollBar()->maximum());
    }

    emit modifyNotify();
    return plotWidget->plot();
}

// 清空图表
bool MonitorWidget::clearWidgets(void)
{
    if (m_ScrollArea)
        return m_ScrollArea->clearWidgets();
    else
        return false;
}

// 通过图表名字去删除
bool MonitorWidget::removeWidgetFromTitle(const QString &title)
{
    if (m_ScrollArea)
        return m_ScrollArea->removeWidgetFromTitle(title);
    else
        return false;
}

bool MonitorWidget::setPlotProject(const PlotProject &plotPro)
{
    m_plotPro = plotPro;
    return true;
}

void MonitorWidget::setToolbarTriggered(int index)
{
    // m_isExternTrigger = true;
    onToolbarTriggered(index);
    // m_isExternTrigger = false;
}

QList<PlotInfo> MonitorWidget::getPlotInfos(void) const
{
    QList<PlotInfo> infoList;
    for (QWidget *w : m_ScrollArea->widgets()) {
        PlotWidget *plotWidget = static_cast<PlotWidget *>(w);
        if (!plotWidget)
            continue;
        QReadWriteLock &plotLock = plotWidget->plot()->getPlotLock();
        QReadLocker guarfLocker(&plotLock);
        PlotInfo &info = plotWidget->plot()->getPlotInfo();
        infoList.push_back(info);
    }
    return infoList;
}

void MonitorWidget::setPlotInfos(QList<PlotInfo> &infoList)
{
    for (PlotInfo &info : infoList) {
        PlotWidget *plotWidget = new PlotWidget(info.Title, m_ScrollArea);
        QReadWriteLock &plotLock = plotWidget->plot()->getPlotLock();
        plotLock.lockForWrite();
        plotWidget->plot()->setPlotInfo(info, false);
        plotWidget->plot()->setCurrentProjectName(m_projectName);
        plotLock.unlock();
        m_ScrollArea->addWidget(plotWidget);
    }
}

void MonitorWidget::changeLayout(int rows, int cols)
{
    m_ScrollArea->relayout(rows, cols);
    emit modifyNotify();
}

void MonitorWidget::updateLayout()
{
    int ncount = m_ScrollArea->widgetCount();
    int rows = ncount / m_ScrollArea->cols();
    if (ncount % m_ScrollArea->cols() != 0)
        rows++;
    int max_rows = 5;

    QVariant var = GlobalConfigs::getInstance()->get("PlotProject");
    PlotProject plotPro = Default;
    if (var.isValid()) {
        int plotProI = var.toInt();
        plotPro = (PlotProject)plotProI;
        if (plotPro == QuiKIS)
            max_rows = 2;
    }

    if (rows > max_rows)
        rows = max_rows;
    m_ScrollArea->relayout(rows, m_ScrollArea->cols());
}

int MonitorWidget::layoutRows(void) const
{
    return m_ScrollArea->splitRowNum();
}
int MonitorWidget::layoutCols(void) const
{
    return m_ScrollArea->cols();
}

void MonitorWidget::save(const QString &strPath)
{

    QDomDocument doc;
    QDomProcessingInstruction ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'utf-8\'");
    doc.appendChild(ins);

    QDomElement root = doc.createElement("root");
    doc.appendChild(root);
    QDomElement panel = doc.createElement("panel");
    root.appendChild(panel);

    getMonitorData(panel);

    QFile file(strPath);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

void MonitorWidget::load(const QString &strPath)
{
    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QDomDocument doc;
    doc.setContent(&file);
    file.close();
    QDomElement root = doc.documentElement();
    if (root.isNull())
        return;
    QDomElement panel = root.firstChildElement("panel");
    if (panel.isNull())
        return;
    setMonitorData(panel);
}

bool MonitorWidget::setMonitorData(const QDomElement &element)
{
    if (element.isNull())
        return false;

    m_ScrollArea->clearWidgets();

    int rows = element.attribute("rows", "1").toInt();
    int cols = element.attribute("cols", "1").toInt();

    QDomNodeList plotList = element.childNodes();
    bool isNeedUpdateXml = false;
    Kcc::ProjectManager::PIProjectManagerServer projectMng =
            RequestServer<Kcc::ProjectManager::IProjectManagerServer>();
    if (!projectMng)
        return false;
    for (int i = 0; i < plotList.size(); i++) {
        QDomElement plot = plotList.at(i).toElement();
        if (plot.tagName() != "plot")
            continue;

        PlotInfo info;
        info.XType = plot.attribute("XType").toInt();
        info.Title = plot.attribute("Title");
        info.XRange = plot.attribute("XRange").toDouble();
        info.BackgroundColor = QColor(plot.attribute("BackgroundColor"));
        info.LegendSide = plot.attribute("LegendSide").toInt();
        info.GridType = plot.attribute("GridType").toInt();
        if (!plot.attribute("IsAutoYAxis").isNull())
            info.IsAutoYAxis = plot.attribute("IsAutoYAxis").toInt() == 1 ? true : false;
        else
            info.IsAutoYAxis = true;
        if (!plot.attribute("YMin").isNull())
            info.YMin = plot.attribute("YMin").toDouble();
        else
            info.YMin = -1;
        if (!plot.attribute("YMax").isNull())
            info.YMax = plot.attribute("YMax").toDouble();
        else
            info.YMax = 1;

        if (!plot.attribute("IsShowTitle").isNull())
            info.IsShowTitle = plot.attribute("IsShowTitle").toInt() == 1 ? true : false;

        // if(!plot.attribute("IsShowSubTitle").isNull())
        //	info.IsShowSubTitle=plot.attribute("IsShowSubTitle").toInt()==1?true:false;

        // if(!plot.attribute("SubTitle").isNull())
        // info.SubTitle=plot.attribute("SubTitle"); 暂时不需要加载副标题了

        if (!plot.attribute("IsShowXTitle").isNull())
            info.IsShowXTitle = plot.attribute("IsShowXTitle").toInt() == 1 ? true : false;

        if (!plot.attribute("XTitle").isNull())
            info.XTitle = plot.attribute("XTitle");

        if (!plot.attribute("IsShowYTitle").isNull())
            info.IsShowYTitle = plot.attribute("IsShowYTitle").toInt() == 1 ? true : false;

        if (!plot.attribute("YTitle").isNull())
            info.YTitle = plot.attribute("YTitle");

        if (!plot.attribute("XMin").isNull())
            info.XMin = plot.attribute("XMin").toDouble();
        if (!plot.attribute("XMax").isNull())
            info.XMax = plot.attribute("XMax").toDouble();

        if (!plot.attribute("SaveDataMode").isNull())
            info.SaveDataMode = plot.attribute("SaveDataMode").toInt();

        if (!plot.attribute("xDecimal").isNull())
            info.xDecimal = plot.attribute("xDecimal").toInt();

        if (!plot.attribute("yDecimal").isNull())
            info.yDecimal = plot.attribute("yDecimal").toInt();

        if (!plot.attribute("isShowYsCaleNum").isNull())
            info.isShowYsCaleNum = plot.attribute("isShowYsCaleNum").toInt();
        else
            info.isShowYsCaleNum = false;

        if (!plot.attribute("yScaleNum").isNull())
            info.yScaleNum = plot.attribute("yScaleNum").toDouble();
        else
            info.yScaleNum = 1.0;

        int tmpRow = -1;
        int tmpCol = -1;
        if (!plot.attribute("row").isNull())
            tmpRow = plot.attribute("row").toInt();

        if (!plot.attribute("col").isNull())
            tmpCol = plot.attribute("col").toInt();

        QDomElement varibles = plot.firstChildElement("varibles");
        if (!varibles.isNull()) {
            QDomNodeList varList = varibles.childNodes();
            for (int j = 0; j < varList.size(); j++) {
                QDomElement varible = varList.at(j).toElement();
                if (varible.tagName() != "varible")
                    continue;

                PlotVaribleInfo varInfo;
                varInfo.Name = varible.attribute("Name");
                varInfo.Path = varible.attribute("Path");
                QStringList &tempSplited = varInfo.Path.split(".");
                if (tempSplited.size() == 5u) {
                    if (tempSplited[1] == "当前仿真" || tempSplited[1] == projectMng->GetSimulationStorageName()) {
                        tempSplited[1] = projectMng->GetRealTimeSimulationName();
                        varInfo.Path = tempSplited.join(".");
                        isNeedUpdateXml = true;
                    }
                } else {
                    continue;
                }
                varInfo.Alias = varible.attribute("Alias");
                varInfo.Type = (PlotVaribleInfoType)varible.attribute("Type").toInt();
                varInfo.Rows = varible.attribute("Rows").toInt();
                varInfo.Cols = varible.attribute("Cols").toInt();

                QDomNodeList curveList = varible.childNodes();
                for (int k = 0; k < curveList.size(); k++) {
                    QDomElement curve = curveList.at(k).toElement();
                    if (curve.tagName() != "curve")
                        continue;

                    PlotCurveInfo curveInfo;
                    curveInfo.Checked = curve.attribute("Checked").toInt();
                    curveInfo.Name = curve.attribute("Name");
                    curveInfo.Index = curve.attribute("Index").toInt();
                    curveInfo.Row = curve.attribute("Row").toInt();
                    curveInfo.Col = curve.attribute("Col").toInt();
                    curveInfo.Color = QColor(curve.attribute("Color"));
                    curveInfo.Width = curve.attribute("Width").toInt();
                    curveInfo.Style = curve.attribute("Style").toInt();
                    curveInfo.plotType = curve.attribute("plotType").toInt();
                    varInfo.CurveInfoList.push_back(curveInfo);
                }

                info.VaribleInfoList.push_back(varInfo);
            }
        }
        QDomElement markers = plot.firstChildElement("markers");
        if (!markers.isNull()) {
            QDomNodeList markerList = markers.childNodes();
            for (int j = 0; j < markerList.size(); j++) {
                QDomElement marker = markerList.at(j).toElement();
                if (marker.tagName() != "marker")
                    continue;

                MarkerInfo markerInfo;
                markerInfo.color = QColor(marker.attribute("color"));
                markerInfo.id = marker.attribute("id");
                markerInfo.isShow = marker.attribute("isShow").toInt();
                markerInfo.name = marker.attribute("name");
                markerInfo.scale = marker.attribute("scale").toDouble();
                markerInfo.type = (MarkerType)marker.attribute("type").toInt();
                info.Markers.append(markerInfo);
            }
        }

        PlotWidget *plotWidget = new PlotWidget(info.Title, m_ScrollArea);
        QReadWriteLock &plotLock = plotWidget->plot()->getPlotLock();
        plotLock.lockForWrite();
        plotWidget->plot()->setPlotInfo(info, false);
        plotWidget->plot()->setCurrentProjectName(m_projectName);
        plotLock.unlock();
        plotWidget->setPos(tmpRow, tmpCol);
        m_ScrollArea->addWidget(plotWidget);
    }
    m_Toolbar->setPlotNum(m_ScrollArea->widgetCount());
    m_ScrollArea->layoutAllPlotFromFile(rows, cols);
    if (isNeedUpdateXml) {
        Q_EMIT modifyNotify();
    }
    return true;
}

bool MonitorWidget::getMonitorData(QDomElement &element)
{
    QDomDocument doc = element.ownerDocument();
    element.setAttribute("rows", layoutRows());
    element.setAttribute("cols", layoutCols());
    for (QWidget *w : m_ScrollArea->widgets()) {
        PlotWidget *plotWidget = static_cast<PlotWidget *>(w);
        if (!plotWidget)
            continue;
        QReadWriteLock &plotLock = plotWidget->plot()->getPlotLock();
        QReadLocker guarfLocker(&plotLock);
        PlotInfo &infoIter = plotWidget->plot()->getPlotInfo();
        QDomElement plot = doc.createElement("plot");
        element.appendChild(plot);

        plot.setAttribute("XType", infoIter.XType);
        plot.setAttribute("Title", infoIter.Title);
        plot.setAttribute("XRange", infoIter.XRange);
        plot.setAttribute("BackgroundColor", infoIter.BackgroundColor.name());
        plot.setAttribute("LegendSide", infoIter.LegendSide);
        plot.setAttribute("GridType", infoIter.GridType);
        plot.setAttribute("IsAutoYAxis", infoIter.IsAutoYAxis ? 1 : 0);
        plot.setAttribute("YMin", infoIter.YMin);
        plot.setAttribute("YMax", infoIter.YMax);
        plot.setAttribute("IsShowTitle", infoIter.IsShowTitle ? 1 : 0);
        // plot.setAttribute("IsShowSubTitle",infoIter->IsShowSubTitle?1:0);
        // plot.setAttribute("SubTitle",infoIter->SubTitle); 暂时不需要保存了
        plot.setAttribute("IsShowXTitle", infoIter.IsShowXTitle ? 1 : 0);
        plot.setAttribute("XTitle", infoIter.XTitle);
        plot.setAttribute("IsShowYTitle", infoIter.IsShowYTitle ? 1 : 0);
        plot.setAttribute("YTitle", infoIter.YTitle);
        plot.setAttribute("XMin", infoIter.XMin);
        plot.setAttribute("XMax", infoIter.XMax);
        plot.setAttribute("SaveDataMode", infoIter.SaveDataMode);
        plot.setAttribute("xDecimal", infoIter.xDecimal);
        plot.setAttribute("yDecimal", infoIter.yDecimal);
        plot.setAttribute("isShowYsCaleNum", infoIter.isShowYsCaleNum ? 1 : 0);
        plot.setAttribute("yScaleNum", infoIter.yScaleNum);
        const QPair<int, int> &pos = plotWidget->getPos();
        plot.setAttribute("row", pos.first);
        plot.setAttribute("col", pos.second);

        QDomElement varibles = doc.createElement("varibles");
        plot.appendChild(varibles);

        for (QList<PlotVaribleInfo>::iterator varIter = infoIter.VaribleInfoList.begin();
             varIter != infoIter.VaribleInfoList.end(); varIter++) { // varible

            // QList<PlotCurveInfo> CurveInfoList;
            QDomElement varible = doc.createElement("varible");
            varibles.appendChild(varible);

            varible.setAttribute("Name", varIter->Name);
            varible.setAttribute("Path", varIter->Path);
            varible.setAttribute("Alias", varIter->Alias);
            varible.setAttribute("Type", (int)varIter->Type);
            varible.setAttribute("Rows", varIter->Rows);
            varible.setAttribute("Cols", varIter->Cols);

            for (QList<PlotCurveInfo>::iterator curveIter = varIter->CurveInfoList.begin();
                 curveIter != varIter->CurveInfoList.end(); curveIter++) { // curve
                QDomElement curve = doc.createElement("curve");
                varible.appendChild(curve);

                curve.setAttribute("Checked", curveIter->Checked);
                curve.setAttribute("Name", curveIter->Name);
                curve.setAttribute("Index", curveIter->Index);
                curve.setAttribute("Row", curveIter->Row);
                curve.setAttribute("Col", curveIter->Col);
                curve.setAttribute("Color", curveIter->Color.name());
                curve.setAttribute("Width", curveIter->Width);
                curve.setAttribute("Style", curveIter->Style);
                curve.setAttribute("plotType", curveIter->plotType);
            }
        }

        QDomElement markers = doc.createElement("markers");
        plot.appendChild(markers);

        for (QList<MarkerInfo>::iterator markerIter = infoIter.Markers.begin(); markerIter != infoIter.Markers.end();
             markerIter++) {
            QDomElement marker = doc.createElement("marker");
            markers.appendChild(marker);
            marker.setAttribute("color", markerIter->color.name());
            marker.setAttribute("id", markerIter->id);
            marker.setAttribute("isShow", markerIter->isShow);
            marker.setAttribute("name", markerIter->name);
            marker.setAttribute("scale", markerIter->scale);
            marker.setAttribute("type", (int)markerIter->type);
        }
    }
    return true;
}

void MonitorWidget::setCurrentProjectName(const QString &projectName)
{
    m_projectName = projectName;
    for (QWidget *w : m_ScrollArea->widgets()) {
        PlotWidget *plotWidget = static_cast<PlotWidget *>(w);
        if (!plotWidget)
            continue;
        plotWidget->plot()->setCurrentProjectName(projectName);
    }
}

void MonitorWidget::initIsRunning(void)
{
    if (!m_isAcceptNotify)
        return;

    PIDataDictionaryServer dataDictionaryServer = RequestServer<IDataDictionaryServer>();
    if (!dataDictionaryServer) {
        LOGOUT(tr("DataDictionaryServer is not loaded"), LOG_ERROR); // DataDictionaryServer未加载
    }
    PIDataDictionaryBuilder dataDictionaryBuilder = dataDictionaryServer->GetCurrentRuntimeDictionary();
    if (!dataDictionaryBuilder)
        return;

    onStart();
}

bool MonitorWidget::isBinging(void) const
{
    return m_ScrollArea->isBinging();
}

void MonitorWidget::clearPropertyBingToVariable()
{
    for (QWidget *w : m_ScrollArea->widgets()) {
        PlotWidget *plotWidget = qobject_cast<PlotWidget *>(w);
        plotWidget->plot()->clear();
    }
}

void MonitorWidget::setShowBorder(bool b)
{
    if (b) {
        // m_ScrollArea->setFrameShape(QFrame::Box);
        // m_ScrollArea->setFrameShadow(QFrame::Sunken);
        m_ScrollArea->setStyleSheet("PlotScrollArea{border: 1px solid #c8c8c8;}");
    } else {
        // m_ScrollArea->setFrameShape(QFrame::NoFrame);
        m_ScrollArea->setStyleSheet("PlotScrollArea{border: 0px solid;}");
    }
}

void MonitorWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    mPlaceHolder->setFixedHeight(1);
    mPlaceHolder->setFixedWidth(this->width());
    mPlaceHolder->setContentsMargins(8, 0, 8, 0);

    mDrawSeprator->move(8, 0);
    mDrawSeprator->setFixedHeight(1);
    mDrawSeprator->setFixedWidth(mPlaceHolder->width() - 16);
}

void MonitorWidget::onAddItemPlot(void)
{
    // 监控面板限制同时最多15个
    if (m_ScrollArea->widgetCount() >= 15) {
        LOGOUT(tr("The monitor panel can display up to 15 graphs simultaneously"),
               LOG_ERROR); // 监控面板最多同时显示15个波形图
        return;
    }
    // QString strTitle = QString("plot%1").arg(m_ScrollArea->widgetCount() + 1);
    // ItemPlot *plot = addItemPlot(strTitle);

    PlotUndoCommond *addCommand = new PlotUndoCommond("Add Plot", nullptr);
    addCommand->setArea(this);
    addCommand->setType(PlotUndoCommond::actionType::AddPlot);
    mUndoStack->beginMacro(tr("Add Graph"));
    mUndoStack->push(addCommand);
    mUndoStack->endMacro();
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onRemoveItemPlot(void)
{
    if (m_ScrollArea->selectedWidgets().size() == 0)
        return;
    PlotUndoCommond *delCommand = new PlotUndoCommond("DelPlot", nullptr);
    delCommand->setArea(this);
    delCommand->setType(PlotUndoCommond::actionType::DelPlot);
    QList<QWidget *> &w = selectedWidgets();
    for (int i = 0; i < w.size(); ++i) {
        if (!w[i])
            continue;
        PlotWidget *pw = (PlotWidget *)(w[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForRead();
        PlotInfo emptyInfo;
        emptyInfo.id = itp->getPlotInfo().id;
        plotLock.unlock();
        delCommand->addPlotInfo(emptyInfo);
    }
    mUndoStack->beginMacro(tr("Delete Graphs"));
    mUndoStack->push(delCommand);
    mUndoStack->endMacro();
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onCopyItemPlot(bool iscut)
{
    QList<QWidget *> tempList = m_ScrollArea->selectedWidgets();
    if (tempList.size() == 0)
        return;

    // if (m_ScrollArea->widgetCount() + tempList.size() > 15 && !iscut) {
    //     LOGOUT(tr("After copying, the number of graphs will exceed 15, and this copy will be cancelled"),
    //            LOG_ERROR); // 复制后波形图数量将超过15个，本次复制取消
    //     return;
    // }

    PlotUndoCommond *copyCommand = new PlotUndoCommond("CopyPlot", nullptr);
    copyCommand->setArea(this);
    copyCommand->setType(PlotUndoCommond::actionType::CopyPlot);
    for (int i = 0; i < tempList.size(); ++i) {
        if (!tempList[i])
            continue;
        PlotWidget *pw = (PlotWidget *)(tempList[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForRead();
        PlotInfo emptyInfo;
        emptyInfo.id = itp->getPlotInfo().id;
        plotLock.unlock();
        copyCommand->addPlotInfo(emptyInfo);
    }
    mUndoStack->beginMacro(tr("Copy Graphs"));
    mUndoStack->push(copyCommand);
    mUndoStack->endMacro();
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onCutItemPlot(void)
{
    QList<QWidget *> tempList = m_ScrollArea->selectedWidgets();
    if (tempList.size() == 0)
        return;

    PlotUndoCommond *cutCommand = new PlotUndoCommond("CutPlot", nullptr);
    cutCommand->setType(PlotUndoCommond::actionType::CutPlot);
    cutCommand->setArea(this);
    PlotUndoCommond *copyCommand = new PlotUndoCommond("CopyPlot", cutCommand);
    PlotUndoCommond *delCommand = new PlotUndoCommond("DelPlot", cutCommand);
    copyCommand->setArea(this);
    delCommand->setArea(this);
    copyCommand->setType(PlotUndoCommond::actionType::CopyPlot);
    copyCommand->setIsCut(true);
    delCommand->setType(PlotUndoCommond::actionType::DelPlot);
    for (int i = 0; i < tempList.size(); ++i) {
        if (!tempList[i])
            continue;
        PlotWidget *pw = (PlotWidget *)(tempList[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForRead();
        PlotInfo emptyInfo;
        emptyInfo.id = itp->getPlotInfo().id;
        plotLock.unlock();
        copyCommand->addPlotInfo(emptyInfo);
        delCommand->addPlotInfo(emptyInfo);
    }
    mUndoStack->beginMacro(tr("Cut Graphs"));
    mUndoStack->push(cutCommand);
    mUndoStack->endMacro();
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onPasteItemPlot(void)
{
    if (mCopyOrCutInfos.isEmpty() || (mCopyOrCutInfos.back().second.isEmpty()))
        return;

    if (m_ScrollArea->widgetCount() + mCopyOrCutInfos.back().second.size() > 15) {
        LOGOUT(tr("After pasting, the number of graphs will exceed 15. Cancel pasting this time"),
               LOG_ERROR); // 粘贴后波形图数量将超过15个，本次粘贴取消
        return;
    }

    PlotUndoCommond *pasteCommand = new PlotUndoCommond("PastePlot", nullptr);
    pasteCommand->setArea(this);
    pasteCommand->setType(PlotUndoCommond::actionType::PastePlot);
    mUndoStack->beginMacro(tr("Paste Graphs"));
    mUndoStack->push(pasteCommand);
    mUndoStack->endMacro();
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onShowSelected(void)
{
    m_ScrollArea->setOnlyShowSelected(true);
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onShowAll(void)
{
    m_ScrollArea->setOnlyShowSelected(false);
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onSync(void)
{
    bool b = m_ScrollArea->isSync();
    m_ScrollArea->setSync(!b);
}

void MonitorWidget::onAutoStop(void)
{
    bool b = m_ScrollArea->autoStop();
    m_ScrollArea->setAutoStop(!b);
}

void MonitorWidget::onStart(void)
{
    m_ScrollArea->setStart(true);
}

void MonitorWidget::onStop(void)
{
    m_ScrollArea->setStart(false);
}

void MonitorWidget::onToolbarTriggered(int nIndex)
{
    switch (nIndex) {
    case 0: // 添加
        onAddItemPlot();
        break;

    case 1: // 复制
        onCopyItemPlot();
        break;
    case 2: // 剪切
        onCutItemPlot();
        break;
    case 3: // 粘贴
        onPasteItemPlot();
        break;
    case 4: // 删除;
        onRemoveItemPlot();
        break;
    case 5: // 只显示选中
        onShowSelected();
        break;
    case 6: // 显示所有
        onShowAll();
        break;
    case 7: // 是否同步操作
    {
        onSync();
        PIMonitorPanelPlotServer monitorPanelPlotServer = RequestServer<IMonitorPanelPlotServer>();
        if (!monitorPanelPlotServer) {
            LOGOUT(tr("Failed to obtain the MonitorPanelPlotServer"), LOG_WARNING); //"MonitorPanelPlotServer获取失败
            return;
        }
        monitorPanelPlotServer->setPlotMenuCheckedState();
        m_ScrollArea->widget()->setFocus();
        break;
    }
    case 8: // 满一页自动停止
        onAutoStop();
        break;
    case 9: // 开始采集
        onStart();
        break;
    case 10: // 停止采集
        onStop();
        break;
    case 11: // X轴缩放
    {
        m_ScrollArea->setAllowZoomInXAxis(m_Toolbar->GetPlotTooBarCheckedState().isXScaleActionChecked);
        PIMonitorPanelPlotServer monitorPanelPlotServer = RequestServer<IMonitorPanelPlotServer>();
        if (!monitorPanelPlotServer) {
            LOGOUT(tr("Failed to obtain the MonitorPanelPlotServer"), LOG_WARNING); //"MonitorPanelPlotServer获取失败
            return;
        }
        // monitorPanelPlotServer->setPlotMenuCheckedState();
        m_ScrollArea->widget()->setFocus();
        break;
    }
    case 12: // Y轴缩放
    {
        m_ScrollArea->setAllowZoomInYAxis(m_Toolbar->GetPlotTooBarCheckedState().isYScaleActionChecked);

        PIMonitorPanelPlotServer monitorPanelPlotServer = RequestServer<IMonitorPanelPlotServer>();
        if (!monitorPanelPlotServer) {
            LOGOUT(tr("Failed to obtain the MonitorPanelPlotServer"), LOG_WARNING); //"MonitorPanelPlotServer获取失败
            return;
        }
        // monitorPanelPlotServer->setPlotMenuCheckedState();
        m_ScrollArea->widget()->setFocus();
        break;
    }
        // 下面是布局
    case 13: // 布局
        // m_ScrollArea->changeLayout(1,m_ScrollArea->cols());
        {
            if (m_plotLayoutChooseDialog) {
                delete m_plotLayoutChooseDialog;
                m_plotLayoutChooseDialog = nullptr;
            }
            m_plotLayoutChooseDialog = new PlotLayoutChooseDialog(this);
            connect(m_plotLayoutChooseDialog, SIGNAL(finished(int)), this, SLOT(onPlotLayoutChooseDialogFinished(int)));
            connect(m_plotLayoutChooseDialog, SIGNAL(choosed(int, int)), this, SLOT(onLayoutChoosed(int, int)));
            // QPoint point = m_Toolbar->getPlotLayoutChooseDialogShowPos();
            QPoint point = QCursor::pos();
            m_plotLayoutChooseDialog->move(point);
            m_plotLayoutChooseDialog->show();
            m_ScrollArea->widget()->setFocus();
            //::SetWindowPos((HWND)m_plotLayoutChooseDialog->winId(),HWND_TOPMOST,point.x(),point.y(),m_plotLayoutChooseDialog->width(),m_plotLayoutChooseDialog->height(),SWP_SHOWWINDOW);
        }
        break;
    case 19: // 初始坐标
    {
        m_ScrollArea->resetSelectedAxis();
        m_ScrollArea->widget()->setFocus();
    } break;
    case 20: // X轴撑满
        m_ScrollArea->setSelectedXAxisFull();
        m_ScrollArea->widget()->setFocus();
        break;
    case 21: // Y轴撑满
        m_ScrollArea->setSelectedYAxisFull();
        m_ScrollArea->widget()->setFocus();
        break;
    case 22: // 导出
    {
        if ((PlotProject)gConfGet("PlotProject").toInt() != QuiKIS) {
            QPixmap pixmap = QPixmap::grabWidget(m_ScrollArea->widget());
            QString strFile = QFileDialog::getSaveFileName(this, tr("Export"), mExportPicPath, "png(*.png)"); // 导出
            if (strFile.isEmpty())
                break;
            pixmap.save(strFile);
            mExportPicPath = QFileInfo(strFile).absolutePath();
            break;
        }

        // QString defaultName;
        // PIMonitorPanelPlotServer monitorPanelSever = RequestServer<IMonitorPanelPlotServer>();
        // if (monitorPanelSever)
        //     defaultName = monitorPanelSever->GetMonitorPanelName();
        // QPixmap pixmap = QPixmap::grabWidget(m_ScrollArea->widget());

        // QString defaultDir =
        //         gConfGet("General_ExportPath")
        //                 .toString(); // QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
        // if (defaultDir.isEmpty() || defaultName.isEmpty())
        //     return;

        // bool ok = false;
        // int graphType = gConfGet("General_ExportGraphType").toInt(&ok);
        // if (!ok || 0 > graphType)
        //     return;

        // QStringList graphList;
        // graphList << ".png"
        //           << ".jpeg"
        //           << ".bmp";
        // QString suffix = graphList.at(graphType); // 后缀
        // if (suffix.isEmpty())
        //     return;

        // QFileDialog fileDlg(this, tr("Export Graph"), defaultDir + "/" + defaultName + suffix,
        //                     "PNG(*.png);;JPEG(*.jpeg);;BMP(*.bmp)"); // 导出图形
        // fileDlg.setWindowFlags(fileDlg.windowFlags()
        //                        & ~Qt::WindowContextHelpButtonHint); // 去除导出文件对话框的帮助按钮

        // QStringList filterList;
        // filterList << "PNG(*.png)"
        //            << "JPEG(*.jpeg)"
        //            << "BMP(*.bmp)";
        // QString filterName = filterList.at(graphType);
        // if (filterName.isEmpty())
        //     return;

        //// Qt5 fix
        //// fileDlg.selectFilter(filterName);
        // fileDlg.setMimeTypeFilters(filterList);
        // fileDlg.setAcceptMode(QFileDialog::AcceptSave);
        // if (fileDlg.exec()) {
        //     QString strFile = fileDlg.selectedFiles().front();
        //     pixmap.save(strFile);
        //     LOGOUT(tr("Graphics exported successfully")); // 图形导出成功
        // }

        // monitorPanelSever->exportAllPlotTo(defaultName, defaultDir, ExportGraphType::BMP);
        // monitorPanelSever->exportAllPlotTo(defaultName, defaultDir, ExportGraphType::PNG);
        // monitorPanelSever->exportAllPlotTo(defaultName, defaultDir, ExportGraphType::JPEG);

    } break;
    case 23: // 鼠标取点
    {
        m_ScrollArea->setMousePoint(m_Toolbar->GetPlotTooBarCheckedState().isMousePointActionChecked);

        PIMonitorPanelPlotServer monitorPanelPlotServer = RequestServer<IMonitorPanelPlotServer>();
        if (!monitorPanelPlotServer) {
            LOGOUT(tr("Failed to obtain the MonitorPanelPlotServer"), LOG_WARNING); //"MonitorPanelPlotServer获取失败
            return;
        }
        monitorPanelPlotServer->setPlotMenuCheckedState();
    } break;
    case 24: // 全选
    {
        onAllSelect();
    } break;
    case 25: // 变量表
    {
        onDataDictionaryDialog();
    } break;
    case 26: // 合并绘图
    {
        onMergePlot();
    } break;
    case 27: // 拆分绘图
    {
        onUnmergePlot();
    } break;
    case 28: // undo
    {
        onUndo();
        m_ScrollArea->widget()->setFocus();
    } break;
    case 29: // redo
    {
        onRedo();
        m_ScrollArea->widget()->setFocus();
    } break;
    default:
        break;
    }
}

void MonitorWidget::onPlotLayoutChooseDialogFinished(int result)
{
    m_plotLayoutChooseDialog->deleteLater();
    m_plotLayoutChooseDialog = nullptr;
}

void MonitorWidget::onUndo()
{
    QString undotxt = mUndoStack->undoText();
    if (undotxt.isEmpty()) {
        LOGOUT(tr("No action can be undone"), LOG_WARNING);
        return;
    }
    QString forDisplay = "[" + tr("Undo") + "]" + undotxt;
    LOGOUT(forDisplay, LOG_NORMAL);
    mUndoStack->undo();
}

void MonitorWidget::onRedo()
{
    QString redotxt = mUndoStack->redoText();
    if (redotxt.isEmpty()) {
        LOGOUT(tr("No actions can be redone"), LOG_WARNING);
        return;
    }
    QString forDisplay = "[" + tr("Redo") + "]" + redotxt;
    LOGOUT(forDisplay, LOG_NORMAL);
    mUndoStack->redo();
}

void MonitorWidget::onLayoutChoosed(int rows, int cols)
{
    m_ScrollArea->relayout(rows, cols);
    emit modifyNotify();
}

void MonitorWidget::setToolBarCopyCutDelBtnState()
{
    if (!m_Toolbar)
        return;
    PIMonitorPanelPlotServer iMonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!iMonitorServer)
        return;
    MonitorPanelPlotServer *monitor = static_cast<MonitorPanelPlotServer *>(iMonitorServer.data());
    if (!monitor || monitor->getSimulateState())
        return;
    bool enable = true;
    if (selectedWidgets().isEmpty()) {
        enable = false;
    }

    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::copy, enable);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::cut, enable);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::del, enable);
}

void MonitorWidget::setToolBarPasteBtnState()
{
    if (!m_Toolbar)
        return;
    bool enable = true;
    if (mCopyOrCutInfos.isEmpty() || mCopyOrCutInfos.back().second.isEmpty()) {
        enable = false;
    }
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::paste, enable);
}

void MonitorWidget::setUndobtnEnable(bool can)
{
    if (!m_Toolbar)
        return;
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::undo, can);
}

void MonitorWidget::setRedobtnEnable(bool can)
{
    if (!m_Toolbar)
        return;
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::redo, can);
}

void MonitorWidget::onDragPloting()
{
    m_ScrollArea->setCanPopMenu(false);
    setUndobtnEnable(false);
    setRedobtnEnable(false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::add, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::copy, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::cut, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::paste, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::del, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::allSelect, false);
}

void MonitorWidget::onDragPlotFinsh()
{
    if (m_ScrollArea->getPopMenuState()) {
        m_ScrollArea->setCanPopMenu(true);
    }
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::allSelect, true);
    PIMonitorPanelPlotServer iMonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!iMonitorServer)
        return;
    MonitorPanelPlotServer *monitor = static_cast<MonitorPanelPlotServer *>(iMonitorServer.data());
    if (!monitor || monitor->getSimulateState())
        return;

    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::add, true);
    bool canCopy = true;
    if (selectedWidgets().isEmpty()) {
        canCopy = false;
    }
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::copy, canCopy);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::cut, canCopy);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::del, canCopy);
    bool canPaste = true;
    if (mCopyOrCutInfos.isEmpty() || mCopyOrCutInfos.back().second.isEmpty()) {
        canPaste = false;
    }
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::paste, canPaste);
    setUndobtnEnable(mUndoStack->canUndo());
    setRedobtnEnable(mUndoStack->canRedo());
}

void MonitorWidget::onSimulatingStopUpdateBtnState()
{
    m_ScrollArea->setCanPopMenu(true);
    m_ScrollArea->setPopMenuState(true);
    // 按钮状态
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::add, true);
    bool canCopy = true;
    if (selectedWidgets().isEmpty()) {
        canCopy = false;
    }
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::copy, canCopy);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::cut, canCopy);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::del, canCopy);
    bool canPaste = true;
    if (mCopyOrCutInfos.isEmpty() || mCopyOrCutInfos.back().second.isEmpty()) {
        canPaste = false;
    }
    setUndobtnEnable(mUndoStack->canUndo());
    setRedobtnEnable(mUndoStack->canRedo());
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::paste, canPaste);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::resetAxis, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::fullx, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::fully, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::zoomx, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::zoomy, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::mousePt, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::screenShot, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::layout, true);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::syc, true);

    // check状态
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::zoomx, mCanZoomX);
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::zoomy, mCanZoomY);
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::syc, mCanSyc);
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::mousePt, mCanMousePt);
    m_ScrollArea->setSync(mCanSyc);
    m_ScrollArea->setAllowZoomInYAxis(mCanZoomY);
    m_ScrollArea->setAllowZoomInXAxis(mCanZoomX);
    m_ScrollArea->setMousePoint(mCanMousePt);
}

void MonitorWidget::onSimulatingUpdateBtnState()
{
    m_ScrollArea->setCanPopMenu(false);
    m_ScrollArea->setPopMenuState(false);
    // check状态
    PlotActionCheckedState tempState = m_Toolbar->GetPlotTooBarCheckedState();
    mCanZoomX = tempState.isXScaleActionChecked;
    mCanZoomY = tempState.isYScaleActionChecked;
    mCanSyc = tempState.isSyncCheckboxActionChecked;
    mCanMousePt = tempState.isMousePointActionChecked;
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::zoomx, false);
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::zoomy, false);
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::syc, false);
    m_Toolbar->setActionCheckState(ScopeViewToolbar::ActionName::mousePt, false);
    m_ScrollArea->setAllowZoomInXAxis(false);
    m_ScrollArea->setAllowZoomInYAxis(false);
    m_ScrollArea->setMousePoint(false);
    m_ScrollArea->setSync(false);

    // 按钮状态
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::undo, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::redo, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::add, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::copy, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::cut, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::paste, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::del, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::resetAxis, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::fullx, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::fully, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::zoomx, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::zoomy, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::mousePt, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::screenShot, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::layout, false);
    m_Toolbar->setActionState(ScopeViewToolbar::ActionName::syc, false);
}

void MonitorWidget::onAllSelect(void)
{
    m_ScrollArea->allSelect();
    setToolBarCopyCutDelBtnState();
    m_ScrollArea->widget()->setFocus();
}

void MonitorWidget::onMergePlot(void)
{
    QList<QWidget *> widgets = m_ScrollArea->selectedWidgets();
    if (widgets.size() != 2)
        return;

    QList<PlotWidget *> plotWidgets;
    for (QWidget *w : widgets) {
        PlotWidget *plotWidget = static_cast<PlotWidget *>(w);
        if (!plotWidget->plot()->canMergePlot())
            return;
        plotWidgets << plotWidget;
    }
    plotWidgets[1]->plot()->setDeleteAll(false);
    PlotInfo plotInfo = plotWidgets[1]->plot()->getMergePlotInfo();
    m_ScrollArea->removeWidget(plotWidgets[1]);
    QList<PlotInfo> plotInfos;
    plotInfos << plotInfo;
    plotWidgets[0]->plot()->mergePlot(plotInfos);
}

void MonitorWidget::onUnmergePlot(void)
{
    QList<QWidget *> widgets = m_ScrollArea->selectedWidgets();
    if (widgets.size() == 0)
        return;
    QList<PlotWidget *> plotWidgets;
    for (QWidget *w : widgets) {
        PlotWidget *plotWidget = static_cast<PlotWidget *>(w);
        if (!plotWidget->plot()->canUnmergePlot())
            continue;
        plotWidgets << plotWidget;
    }

    for (PlotWidget *plotWidget : plotWidgets) {
        QList<PlotInfo> plotInfos = plotWidget->plot()->unmergePlot();
        if (plotInfos.size() > 0) {
            ItemPlot *plot = addItemPlot("");
            plot->setUnmergePlotInfo(plotInfos[0]);
        }
    }
}

void MonitorWidget::onDataDictionaryDialog(void)
{
    // if (DataDictionaryDialog::isShow())
    //     return;
    if (!m_dataDictionaryDialog) {
        m_dataDictionaryDialog = new DataDictionaryDialog(this);
        connect(m_dataDictionaryDialog, SIGNAL(finished(int)), this, SLOT(onDataDictionaryDialogClosed()));
    }
    m_dataDictionaryDialog->refreshNodeState();
    m_dataDictionaryDialog->show();
}

void MonitorWidget::onDataDictionaryServerNotify(unsigned int code, const NotifyStruct &notifyStruct)
{
    // if(!m_isAcceptNotify)return;
    // switch(code)
    //{
    // case Notify_StartRunTime:
    //	{
    //		m_ScrollArea->setStart(true);
    //	}break;
    // case Notify_StopRunTime:
    //	{
    //		m_ScrollArea->setStart(false);
    //	}break;
    // default:break;
    // }
}

void MonitorWidget::onDataDictionaryDialogClosed(void)
{
    // m_dataDictionaryDialog->deleteLater();
    // m_dataDictionaryDialog = nullptr;
}

void MonitorWidget::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    if (code == Notify_DrawingBoardRunning || code == Notify_SimulationResume) {
        onSimulatingUpdateBtnState();
    } else if (code == Notify_DrawingBoardStopped || code == Notify_SimulationPaused) {
        onSimulatingStopUpdateBtnState();
    }
}

// bool MonitorWidget::eventFilter(QObject *obj, QEvent *e)
//{
//     PIMonitorPanelPlotServer iMonitorServer = RequestServer<IMonitorPanelPlotServer>();
//     if (!iMonitorServer)
//         return QFrame::eventFilter(obj, e);
//     MonitorPanelPlotServer *monitor = static_cast<MonitorPanelPlotServer *>(iMonitorServer.data());
//     if (!monitor)
//         return QFrame::eventFilter(obj, e);
//     if (monitor->getSimulateState())
//         return QFrame::eventFilter(obj, e);
//
//     if (e->type() == QEvent::KeyRelease && obj == static_cast<PlotScrollArea *>(obj)) {
//         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
//         if (keyEvent->matches(QKeySequence::New)) {
//             onAddItemPlot();
//             e->accept();
//             return true;
//         } else if (keyEvent->matches(QKeySequence::SelectAll)) {
//             onAllSelect();
//             e->accept();
//             return true;
//         } else if (keyEvent->matches(QKeySequence::Copy)) {
//             onCopyItemPlot();
//             e->accept();
//             return true;
//         } else if (keyEvent->matches(QKeySequence::Cut)) {
//             onCutItemPlot();
//             e->accept();
//             return true;
//         } else if (keyEvent->matches(QKeySequence::Paste)) {
//             onPasteItemPlot();
//             e->accept();
//             return true;
//         } else if (keyEvent->matches(QKeySequence::Delete)) {
//             onRemoveItemPlot();
//             e->accept();
//             return true;
//         }
//     }
//
//     return QFrame::eventFilter(obj, e);
// }

QPair<bool, QList<PlotInfo>> &MonitorWidget::getLastedList()
{
    if (mCopyOrCutInfos.isEmpty())
        return QPair<bool, QList<PlotInfo>>();
    return mCopyOrCutInfos.back();
}

void MonitorWidget::addInfoList(const QPair<bool, QList<PlotInfo>> &infos)
{
    mCopyOrCutInfos.append(infos);
    while (mCopyOrCutInfos.size() >= 5) {
        mCopyOrCutInfos.pop_front();
    }
}

void MonitorWidget::DelLatestOneInfo()
{
    if (mCopyOrCutInfos.isEmpty())
        return;
    for (auto &it : mCopyOrCutInfos.back().second) {
        for (auto &var : it.VaribleInfoList) {
            if (var.VaribleBase) {
                var.VaribleBase.clear();
            }
            if (var.ElementBase) {
                var.ElementBase.clear();
            }
            for (auto &x : var.CurveInfoList) {
                if (x.ItemPlotCurveObject) {
                    delete x.ItemPlotCurveObject;
                }
            }
        }
    }
    mCopyOrCutInfos.pop_back();
}

PlotUndoCommond::PlotUndoCommond(const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), mAction(actionType::NoDefine), mWidget(nullptr), mBefore_RowAndCol(-1, -1)
{
}

PlotUndoCommond::~PlotUndoCommond() { }

void PlotUndoCommond::setArea(MonitorWidget *pa)
{
    mWidget = pa;
}

void PlotUndoCommond::setType(actionType actype)
{
    mAction = actype;
}

PlotUndoCommond::actionType PlotUndoCommond::getType()
{
    return mAction;
}

void PlotUndoCommond::addPlotInfo(const PlotInfo &info)
{
    mSource.push_back(info);
}

QList<PlotInfo> &PlotUndoCommond::getPlotInfoVec()
{
    return mSource;
}

void PlotUndoCommond::copyCurveData()
{
    for (auto &pin : mSource) {
        copyOne(pin);
    }
}

void PlotUndoCommond::copyOne(PlotInfo &info)
{
    for (auto &eachVar : info.VaribleInfoList) {
        if (eachVar.ElementBase)
            eachVar.ElementBase.clear();
        if (eachVar.VaribleBase)
            eachVar.VaribleBase.clear();

        for (auto &curveIter : eachVar.CurveInfoList) {
            ItemPlotCurve *src = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
            if (!src)
                continue;
            QReadWriteLock &curveLock = src->getCurveLock();
            curveLock.lockForRead();
            curveIter.ItemPlotCurveObject = src->clone();
            curveLock.unlock();
        }
    }
}

QStringList PlotUndoCommond::getAllTitle()
{
    QStringList allTitle;
    for (auto &w : mWidget->getScrollArea()->widgets()) {
        if (!w)
            continue;
        PlotWidget *plot = static_cast<PlotWidget *>(w);
        if (!plot)
            continue;
        if (!plot->plot())
            continue;
        ItemPlot *tempItem = plot->plot();
        QReadWriteLock &lock = tempItem->getPlotLock();
        lock.lockForRead();
        PlotInfo &info = tempItem->getPlotInfo();
        if (!allTitle.contains(info.Title)) {
            allTitle << info.Title;
        }
        lock.unlock();
    }
    return allTitle;
}

void PlotUndoCommond::renameTitle(const QStringList &allHasTitle, QString &needRename)
{
    if (allHasTitle.isEmpty() || !allHasTitle.contains(needRename)) {
        return;
    }

    int i = 0;
    QString tempName;
    do {
        ++i;
        tempName = needRename;
        tempName += QString("(%1)").arg(i);
    } while (allHasTitle.contains(tempName));
    needRename = tempName;
}

void PlotUndoCommond::saveLayOut()
{
    initLayOut();
    if (!mWidget)
        return;
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;

    mBefore_RowAndCol.first = area->splitRowNum();
    mBefore_RowAndCol.second = area->cols();
    mBeforeRealRow = area->realrow();

    QList<QWidget *> &allPlotWidgets = area->widgets();
    if (allPlotWidgets.isEmpty())
        return;
    for (int i = 0; i < allPlotWidgets.size(); ++i) {
        PlotWidget *pw = (PlotWidget *)(allPlotWidgets[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        if (!itp)
            continue;

        QReadWriteLock &plotLock = itp->getPlotLock();
        QReadLocker guard(&plotLock);
        PlotInfo &itinf = itp->getPlotInfo();
        QPair<int, int> tempPos;
        tempPos.first = itinf.mRow;
        tempPos.second = itinf.mCol;
        mBefore_PlotPos[itinf.id] = tempPos;
    }
}

void PlotUndoCommond::initLayOut()
{
    mBefore_RowAndCol.first = -1;
    mBefore_RowAndCol.second = -1;
    mBefore_PlotPos.clear();
}

void PlotUndoCommond::undo()
{
    if (mAction == actionType::AddPlot) {
        undoAddPlot();
    } else if (mAction == actionType::DelPlot) {
        undoDelPlot();
    } else if (mAction == actionType::CopyPlot) {
        undoCopyPlot();
    } else if (mAction == actionType::CutPlot) {
        undoCutPlot();
    } else if (mAction == actionType::PastePlot) {
        undoPastePlot();
    }
}

void PlotUndoCommond::redo()
{
    if (mAction == actionType::AddPlot) {
        redoAddPlot();
    } else if (mAction == actionType::DelPlot) {
        redoDelPlot();
    } else if (mAction == actionType::CopyPlot) {
        redoCopyPlot();
    } else if (mAction == actionType::CutPlot) {
        redoCutPlot();
    } else if (mAction == actionType::PastePlot) {
        redoPastePlot();
    }
}

void PlotUndoCommond::undoAddPlot()
{
    if (mSource.isEmpty())
        return;
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    QList<QWidget *> &allPlotWidget = area->widgets();
    for (int i = 0; i < allPlotWidget.size(); ++i) {
        if (!allPlotWidget[i])
            continue;

        PlotWidget *w = (PlotWidget *)(allPlotWidget[i]);
        if (!w)
            continue;
        ItemPlot *plotPtr = w->plot();
        if (!plotPtr)
            continue;

        QReadWriteLock &plotLock = plotPtr->getPlotLock();
        plotLock.lockForRead();
        PlotInfo &eachOneInfo = plotPtr->getPlotInfo();
        if (eachOneInfo.id == mSource.back().id) {
            mSource.back() = eachOneInfo;
            mSource.back().mRow = -1;
            mSource.back().mCol = -1;
            plotLock.unlock();
            copyOne(mSource.back());
            area->removeWidget(allPlotWidget[i]);
        } else {
            plotLock.unlock();
        }
    }
    // area->intelichangeLayout();
    area->specificLayout(mBeforeRealRow, mBefore_RowAndCol, mBefore_PlotPos);
    emit mWidget->modifyNotify();
    mWidget->getScopeViewToolbar()->setPlotNum(area->widgetCount());
}

void PlotUndoCommond::redoAddPlot()
{
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    saveLayOut();
    QString strTitle = QString("plot%1").arg(area->widgetCount() + 1);
    PlotWidget *pw = new PlotWidget(strTitle, area);
    if (!pw)
        return;
    ItemPlot *itp = pw->plot();
    if (!itp)
        return;
    area->addWidget(pw);
    if (!mSource.isEmpty()) {
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForWrite();
        itp->setPlotInfo(mSource.back());
        itp->getPlotInfo().id = mSource.back().id;
        itp->setCurrentProjectName(mWidget->getPrjName());
        plotLock.unlock();
    } else {
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForWrite();
        itp->setCurrentProjectName(mWidget->getPrjName());
        mSource.push_back(itp->getPlotInfo());
        mSource.back().mRow = -1;
        mSource.back().mCol = -1;
        plotLock.unlock();
        copyCurveData();
    }
    itp->replot();
    // area->intelichangeLayout();
    area->changeLayoutAfterAdd();
    emit mWidget->modifyNotify();
    mWidget->getScopeViewToolbar()->setPlotNum(area->widgetCount());
}

void PlotUndoCommond::undoDelPlot()
{
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    for (auto &info : mSource) {
        PlotWidget *plotWidget = new PlotWidget(info.Title, area);
        if (!plotWidget)
            continue;
        ItemPlot *itp = plotWidget->plot();
        if (!itp)
            continue;
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForWrite();
        itp->setPlotInfo(info);
        PlotInfo &itpInfo = itp->getPlotInfo();
        itpInfo.id = info.id;
        itpInfo.mRow = -1;
        itpInfo.mCol = -1;
        itp->setCurrentProjectName(mWidget->getPrjName());
        plotLock.unlock();
        area->addWidget(plotWidget);
        itp->replot();
    }
    // area->intelichangeLayout();
    area->specificLayout(mBeforeRealRow, mBefore_RowAndCol, mBefore_PlotPos);
    emit mWidget->modifyNotify();
    mWidget->getScopeViewToolbar()->setPlotNum(area->widgetCount());
}

void PlotUndoCommond::redoDelPlot()
{
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    saveLayOut();
    QList<QWidget *> &allPlotWidget = area->widgets();
    for (int i = 0; i < allPlotWidget.size(); ++i) {
        if (!allPlotWidget[i])
            continue;
        PlotWidget *pw = (PlotWidget *)(allPlotWidget[i]);
        if (!pw)
            continue;

        ItemPlot *itp = pw->plot();
        if (!itp)
            continue;

        PlotInfo &itpInfo = itp->getPlotInfo();
        auto it = std::find_if(mSource.begin(), mSource.end(),
                               [&](PlotInfo &eachInfo) { return eachInfo.id == itpInfo.id; });
        if (it == mSource.end()) {
            continue;
        }
        (*it) = itpInfo;
        it->mRow = -1;
        it->mCol = -1;
        copyOne(*it);
        area->removeWidget(allPlotWidget[i]);
    }
    mWidget->setToolBarCopyCutDelBtnState();
    area->intelichangeLayout();
    emit mWidget->modifyNotify();
    mWidget->getScopeViewToolbar()->setPlotNum(area->widgetCount());
}

void PlotUndoCommond::undoCopyPlot()
{
    mWidget->DelLatestOneInfo();
    mWidget->setToolBarPasteBtnState();
}

void PlotUndoCommond::redoCopyPlot()
{
    mWidget->addInfoList(QPair<bool, QList<PlotInfo>>());
    if (mIsCut) {
        mWidget->getLastedList().first = true;
    } else {
        mWidget->getLastedList().first = false;
    }

    QList<PlotInfo> &dstVec = mWidget->getLastedList().second;
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    QList<QWidget *> &allPlotWidget = area->widgets();
    for (int i = 0; i < allPlotWidget.size(); ++i) {
        if (!allPlotWidget[i])
            continue;
        PlotWidget *pw = (PlotWidget *)(allPlotWidget[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        if (!itp)
            continue;

        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForRead();
        auto it = std::find_if(mSource.begin(), mSource.end(),
                               [&](PlotInfo &info) { return info.id == itp->getPlotInfo().id; });
        if (it == mSource.end()) {
            plotLock.unlock();
            continue;
        }
        PlotInfo &itpInfo = itp->getPlotInfo();
        it->id == itpInfo.id;
        //(*it) = itp->getPlotInfo();
        dstVec.push_back(itpInfo);
        plotLock.unlock();
        // copyOne(*it);
        copyOne(dstVec.back());
    }
    mWidget->setToolBarPasteBtnState();
}

void PlotUndoCommond::undoCutPlot()
{
    int count = this->childCount();
    for (int i = count - 1; i >= 0; --i) {
        QUndoCommand *com = const_cast<QUndoCommand *>(this->child(i));
        com->undo();
    }
}

void PlotUndoCommond::redoCutPlot()
{
    int count = this->childCount();
    for (int i = 0; i < count; ++i) {
        QUndoCommand *com = const_cast<QUndoCommand *>(this->child(i));
        com->redo();
    }
}

void PlotUndoCommond::undoPastePlot()
{
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    QList<QWidget *> &allPlotWidget = area->widgets();
    for (int i = 0; i < allPlotWidget.size(); ++i) {
        if (!allPlotWidget[i])
            continue;

        PlotWidget *pw = (PlotWidget *)(allPlotWidget[i]);
        if (!pw)
            continue;
        ItemPlot *itp = pw->plot();
        if (!itp)
            continue;
        QReadWriteLock &plotLock = itp->getPlotLock();
        plotLock.lockForRead();
        auto it = std::find_if(mSource.begin(), mSource.end(),
                               [&](PlotInfo &info) { return info.id == itp->getPlotInfo().id; });
        if (it == mSource.end()) {
            plotLock.unlock();
            continue;
        }
        if (mWidget->getLastedList().first) {
            mWidget->getStashList().back().second.push_back(itp->getPlotInfo());
            copyOne(mWidget->getStashList().back().second.back());
        }
        plotLock.unlock();
        mWidget->getScrollArea()->removeWidget(allPlotWidget[i]);
    }

    mWidget->setToolBarPasteBtnState();

    // area->intelichangeLayout();
    area->specificLayout(mBeforeRealRow, mBefore_RowAndCol, mBefore_PlotPos);
    emit mWidget->modifyNotify();
    mWidget->getScopeViewToolbar()->setPlotNum(area->widgetCount());
}

void PlotUndoCommond::redoPastePlot()
{
    PlotScrollArea *area = mWidget->getScrollArea();
    if (!area)
        return;
    QList<PlotInfo> &srcVec = mWidget->getLastedList().second;
    if (srcVec.isEmpty())
        return;
    saveLayOut();
    if (mSource.isEmpty()) {
        for (PlotInfo &info : srcVec) {
            PlotWidget *plotWidget = new PlotWidget(info.Title, area);
            ItemPlot *itp = plotWidget->plot();
            if (!itp)
                continue;
            QString originName = info.Title;
            renameTitle(getAllTitle(), info.Title);
            QReadWriteLock &plotLock = itp->getPlotLock();
            plotLock.lockForWrite();
            itp->setPlotInfo(info);
            PlotInfo &itpInfo = itp->getPlotInfo();
            itpInfo.mRow = -1;
            itpInfo.mCol = -1;
            info.Title = originName;
            PlotInfo emptyInfo;
            emptyInfo.id = itpInfo.id;
            mSource.push_back(emptyInfo);

            itp->setCurrentProjectName(mWidget->getPrjName());
            plotLock.unlock();
            area->addWidget(plotWidget);
            area->changeLayoutAfterAdd();
            itp->replot();
        }
    } else {
        for (int i = 0; i < srcVec.size(); ++i) {
            PlotWidget *plotWidget = new PlotWidget(srcVec[i].Title, area);
            ItemPlot *itp = plotWidget->plot();
            if (!itp)
                continue;
            QString originName = srcVec[i].Title;
            renameTitle(getAllTitle(), srcVec[i].Title);
            QReadWriteLock &plotLock = itp->getPlotLock();
            plotLock.lockForWrite();
            itp->setPlotInfo(srcVec[i]);
            srcVec[i].Title = originName;
            PlotInfo &itpInfo = itp->getPlotInfo();
            itpInfo.id = mSource[i].id;
            itpInfo.mRow = -1;
            itpInfo.mCol = -1;
            itp->setCurrentProjectName(mWidget->getPrjName());
            plotLock.unlock();
            area->addWidget(plotWidget);
            area->changeLayoutAfterAdd();
            itp->replot();
        }
    }
    // area->intelichangeLayout();
    if (mWidget->getLastedList().first) {
        srcVec.clear();
    }
    mWidget->setToolBarPasteBtnState();

    emit mWidget->modifyNotify();
    mWidget->getScopeViewToolbar()->setPlotNum(area->widgetCount());
}


#include "ItemPlot.h"
#include <qmath.h>
#include <qwt_painter.h>
// #include <qwt_plot_magnifier.h>
// #include <qwt_plot_panner.h>
#include "CustomPlotSettingsDialog.h"
#include "ItemPlotLegend.h"
#include <qwt_scale_widget.h>

#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerNotify.h"
#include "DragDataResolver.h"
#include <QDebug>

#include <qwt_plot_marker.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>

#include "KLProject/klproject_global.h"
#include "MonitorPanelPlotServer.h"
#include "PlotEllipse.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include "server/Base/IServerInterfaceBase.h"
#include "server/PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
#include "server/SimulationManagerServer/ISimulationManagerServer.h"
#include <QtConcurrent>

USE_LOGOUT_("ItemPlot")
using namespace Kcc::SimulationManager;
using namespace Kcc::PluginGraphicModeling;
using namespace Kcc::SimuNPSAdapter;

ItemPlot::ItemPlot(QString strTitle, QWidget *parent)
    : QwtPlot(parent),
      m_plotGrid(nullptr),
      // m_toolbar(nullptr),
      m_isNeedReplot(false),
      m_timerId(0),
      _dataDictionaryServer(nullptr),
      m_settingDialog(nullptr),
      m_xScaleDraw(nullptr),
      m_yScaleDraw(nullptr),
      m_canvas(nullptr),
      m_isAutoStop(false),
      m_isDeleteAll(true),
      mPlotLegend(nullptr),
      m_plotProject(Default),
      m_yRightScaleDraw(nullptr)
// m_isModifyDecimal(nullptr),
// m_isModifySet(false)
{
    setContentsMargins(0, 8, 8, 8);
    setMinimumWidth(128);
    setAcceptDrops(true);
    m_plotInfo.id = ItemPlot::generateUuid();
    m_plotInfo.Title = strTitle;
    m_plotInfo.isShowYsCaleNum = false;
    m_plotInfo.yScaleNum = 1.0;
    plotInfoUpdate();
    init();
    axisWidget(yLeft)->setSpacing(0);
    axisWidget(yLeft)->setMargin(0);
    axisWidget(yLeft)->setBorderDist(0, 0);
    axisWidget(xBottom)->setSpacing(0);
    axisWidget(xBottom)->setMargin(0);
    axisWidget(xBottom)->setBorderDist(0, 0);
    queryDictionaryServerInterface();
    m_timerId = startTimer(150);
    void (ItemPlot::*plotTriger)(int) = &ItemPlot::toolBarTriggerSignal;
    void (ItemPlot::*onPlotTriger)(int) = &ItemPlot::onToolbarTriggered;
    connect(this, plotTriger, this, onPlotTriger);

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
    mWatcher = new plotWatcher(this);
}

ItemPlot::~ItemPlot()
{
    killTimer(m_timerId);
    this->deleteLater();
    if (mPlotLock.tryLockForWrite()) {
        mPlotLock.unlock();
    }
    if (m_isDeleteAll) {
        uninitAll();
    } else {
    }
}

void ItemPlot::init(void)
{
    QwtText txt;
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(12);
    txt.setFont(font);

    setAxisTitle(QwtPlot::xBottom, txt);
    setAxisTitle(QwtPlot::yLeft, txt);
    setAxisTitle(QwtPlot::yRight, txt);

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_canvas = new ItemPlotCanvas(this);
    setCanvas(m_canvas);
    connect(m_canvas, SIGNAL(modifyNotify()), this, SIGNAL(modifyNotify()));

    QObject::connect(m_canvas, SIGNAL(notifyCommand(int, QMap<QString, QVariant>)), this,
                     SIGNAL(notifyCommand(int, QMap<QString, QVariant>)));

    // 关闭自动刷新
    setAutoReplot(false);
    plotLayout()->setAlignCanvasToScales(true);

    // 设置默认的坐标范围
    ProjectManager::PIProjectManagerServer pProjectMng = RequestServer<ProjectManager::IProjectManagerServer>();
    if (pProjectMng) {
        m_plotInfo.XRange = pProjectMng->GetProjectConfig(KL_PRO::RUN_TIME).toDouble();
    }

    if (m_plotInfo.XRange == 0.0) {
        m_plotInfo.XRange = 1.0;
    }
    m_xScaleDraw = new ItemPlotScaleDraw;
    m_xScaleDraw->setColor(QColor("#333333"));
    setAxisScaleDraw(QwtPlot::xBottom, m_xScaleDraw);
    m_yScaleDraw = new ItemPlotScaleDraw(QwtPlot::yLeft);
    m_yScaleDraw->setColor(QColor("#333333"));
    setAxisScaleDraw(QwtPlot::yLeft, m_yScaleDraw);

    QPalette p;
    p.setColor(QPalette::WindowText, Qt::white);
    this->axisWidget(QwtPlot::xBottom)->setPalette(p);
    this->axisWidget(QwtPlot::yLeft)->setPalette(p);
    this->setLegendPosition(QwtPlot::TopLegend);

    applyPlotInfo();

    // 创建工具档=栏
    // m_toolbar = new PlotToolbar(this);
    // m_toolbar->raise();
    // m_toolbar->hide();
    // connect(m_toolbar, SIGNAL(triggered(int)), this, SLOT(onToolbarTriggered(int)));
    // QVariant var = GlobalConfigs::getInstance()->get("PlotProject");
    //// PlotProject plotPro = PlotProject::Default;
    // if (var.isValid()) {
    //     int plotProI = var.toInt();
    //     m_plotProject = (PlotProject)plotProI;
    //     if (m_plotProject == QuiKIS)
    //         m_toolbar->setGeometry(1, 0, 82, 24);
    //     else
    //         m_toolbar->setGeometry(1, 0, 144, 24);
    // }
}

void ItemPlot::queryDictionaryServerInterface(void)
{
    PIServerInterfaceBase psb = RequestServerInterface<IDataDictionaryServer>();
    if (psb) {
        // psb->connectNotify(Notify_StopRunTime,this,SLOT(onDataDictionaryServerNotify(unsigned int,const NotifyStruct
        // &))); psb->connectNotify(Notify_StartRunTime,this,SLOT(onDataDictionaryServerNotify(unsigned int,const
        // NotifyStruct &)));
        _dataDictionaryServer = QueryInterface<IDataDictionaryServer>(psb);
    }
    if (!_dataDictionaryServer) {
        LOGOUT(tr("IDataDictionaryServer is not loaded"), LOG_ERROR); // IDataDictionaryServer未加载
        return;
    }

    PIServerInterfaceBase pDataDiction = RequestServerInterface<ISimuNPSDataDictionaryServer>();
    if (pDataDiction) {
        pDataDiction->connectNotify(Notify_ElementAdded, this,
                                    SLOT(onDataDictionaryServerNotify(unsigned int, const NotifyStruct &)));
    }
}

void ItemPlot::initCurve(PlotCurveInfo &info, QString &varibleName, PlotVaribleInfoType variableType,
                         const QString &varAlias, bool isClone)
{
    if (!info.ItemPlotCurveObject) {
        info.ItemPlotCurveObject = new ItemPlotCurve("");
    } else {
        if (isClone) {
            ItemPlotCurve *tmpCurve = static_cast<ItemPlotCurve *>(info.ItemPlotCurveObject);
            if (!tmpCurve)
                return;
            QReadWriteLock &curveLock = tmpCurve->getCurveLock();
            curveLock.lockForRead();
            info.ItemPlotCurveObject = tmpCurve->clone();
            curveLock.unlock();
        }
    }

    if (info.Name == "") {
        info.Name = defaultCurveName(varibleName, variableType, info.Row, info.Col);
    }
    ItemPlotCurve *plotCurve = static_cast<ItemPlotCurve *>(info.ItemPlotCurveObject);
    if (!plotCurve)
        return;

    plotCurve->setTitle(varAlias);
    QwtPlotCurve::CurveStyle curveStyle = QwtPlotCurve::Lines;
    if (info.plotType == 1) {
        curveStyle = QwtPlotCurve::Steps;
    } else if (info.plotType == 2) {
        curveStyle = QwtPlotCurve::Sticks;
    }
    changeLineStyle(plotCurve, info.Color, info.Width, info.Style);
    plotCurve->setStyle(curveStyle);

    plotCurve->attach(this);
    showCurve(plotCurve, info.Checked);
    return;
}

void ItemPlot::uninitCurve(PlotCurveInfo &info)
{
    if (info.ItemPlotCurveObject) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(info.ItemPlotCurveObject);
        if (!curve)
            return;
        curve->detach();
        delete curve;
        info.ItemPlotCurveObject = nullptr;
    }
}

bool ItemPlot::initVarbleInfo(PlotVaribleInfo &info)
{
    PIDataDictionaryBuilder pBuilder = _dataDictionaryServer->GetCurrentRuntimeDictionary();
    if (!pBuilder)
        return false;

    PIElementBase pBase = pBuilder->FindElement(info.Path);
    if (!pBase) {
        info.Rows = 0;
        info.Cols = 0;
        info.Type = Number;
        // QStringList &varSplit = info.Path.split(".");
        // if (varSplit.size() < 5u)
        //     return false;

        // ProjectManager::PIProjectManagerServer projectMng = RequestServer<ProjectManager::IProjectManagerServer>();
        // if (!projectMng)
        //     return false;
        // QStringList &activeBoard = projectMng->GetActiveBoardModelNames();
        //// 画板已激活，但数据字典仍然找不到该画板的变量,才报错，否则不报错
        // if (activeBoard.contains(varSplit[2])) {
        //     LOGOUT("[" + info.Path + "]" + tr("is not found in data dictionary"), Kcc::LOG_ERROR); //
        //     在数据字典中未找到
        // }
        return false;
    }

    if (pBase->Attribute("ParamType").isValid()) {
        if (pBase->Attribute("ParamType").toString() == "InterParam") {
            LOGOUT(tr("Curve control cannot monitor parameters"), LOG_WARNING); // 曲线控件不能监视参数
            return false;
        }
    }

    if (info.ElementBase)
        info.ElementBase.clear();
    info.ElementBase = pBase;
    info.Name = pBase->Name();
    info.Path = pBase->FullName();
    if (info.Alias.isEmpty()) {
        QStringList &tmpSplit = info.Path.split(".");
        if (tmpSplit.size() == 5u) {
            info.Alias = tmpSplit[3] + "." + tmpSplit.back();
        }
    }

    mWatcher->addVarWatcher(info.Path);

    if (pBase->ElementType().testFlag(ElementType_Number)) {
        info.Type = Number;
        info.Rows = 1;
        info.Cols = 1;
        if (info.CurveInfoList.isEmpty()) {
            PlotCurveInfo curveInfo;
            curveInfo.Checked = true;
            curveInfo.Index = 0;
            curveInfo.Row = 0;
            curveInfo.Col = 0;
            info.CurveInfoList.push_back(curveInfo);
        }
    } else if (pBase->ElementType().testFlag(ElementType_NumberArray)) {
        PIElementNumberArray parray = QueryInterface<IElementNumberArray>(pBase);

        info.Type = NumberArray;

        QVariant var = GlobalConfigs::getInstance()->get("PlotProject");
        PlotProject plotPro = Default;
        if (var.isValid()) {
            int plotProI = var.toInt();
            plotPro = (PlotProject)plotProI;
            if (plotPro == QuiKIS)
                info.Cols = 1; // QuiKIS项目第二个double是用来放其他值的，而不是放另外一条曲线的
            else
                info.Cols = parray->Count();
        }
        info.Rows = 1;

        for (int i = 0; i < info.Cols; i++) {
            PlotCurveInfo curveInfo;
            if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
                if (getId().contains("PHASE_HARMONICS") || getId().contains("SEQUENCE_HARMONICS"))
                    curveInfo.Style = 1;
            }
            // curveInfo.Style = gConfGet(DefaultCurveStyle).toInt();
            curveInfo.Index = i;
            curveInfo.Col = i;
            curveInfo.Row = 0;
            // curveInfo.Checked =false;
            // 如果info.CurveInfoList不是空，应该是之前保存过的而不是新获取的
            if (info.CurveInfoList.size() < i + 1) {
                info.CurveInfoList.push_back(curveInfo);
            }
        }
    } else if (pBase->ElementType().testFlag(ElementType_NumberArray2D)) {
        PIElementNumberArray2D parray = QueryInterface<IElementNumberArray2D>(pBase);

        info.Type = NumberArray2D;
        info.Rows = parray->Row();
        info.Cols = parray->Colum();
        for (int i = 0; i < info.Rows; i++) {
            for (int j = 0; j < info.Cols; j++) {
                PlotCurveInfo curveInfo;
                if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
                    if (getId().contains("PHASE_HARMONICS") || getId().contains("SEQUENCE_HARMONICS"))
                        curveInfo.Style = 1;
                }
                // curveInfo.Style = gConfGet(DefaultCurveStyle).toInt();
                curveInfo.Index = i * info.Cols + j;
                curveInfo.Col = j;
                curveInfo.Row = i;
                // curveInfo.Checked =false;
                // 如果info.CurveInfoList不是空，应该是之前保存过的而不是新获取的
                if (info.CurveInfoList.size() < (i * info.Cols + j + 1)) {
                    info.CurveInfoList.push_back(curveInfo);
                }
            }
        }
    }

    if (info.VaribleBase) {
        info.VaribleBase->RemoveVariableChangedHandler(this);
        info.VaribleBase.clear();
    }

    info.VaribleBase = pBase->RefToVariable();
    info.VaribleBase->AddVariableChangedHandler(this, VARIABLE_REFRESHTIME);
    return true;
}

void ItemPlot::uninitVarbleInfo(PlotVaribleInfo &info)
{
    if (info.VaribleBase) {
        info.VaribleBase->RemoveVariableChangedHandler(this);
        info.VaribleBase.clear();
    }
    if (info.ElementBase) {
        info.ElementBase.clear();
    }
    mWatcher->delVarWatcher(info.Path);
}

void ItemPlot::applyPlotInfo(void)
{
    applyTitle(m_plotInfo);
    // 直接设置颜色不生效
    // this->setCanvasBackground(QBrush(m_plotInfo.BackgroundColor));
    QString canvasStyle = QString("ItemPlotCanvas{border:1px solid #333333; background-color:%1}")
                                  .arg(m_plotInfo.BackgroundColor.name());
    m_canvas->setStyleSheet(canvasStyle);

    // setLegendPosition((QwtPlot::LegendPosition)m_plotInfo.LegendSide);

    if (m_plotInfo.xBottomIsLog) {
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
    } else {
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    }

    // m_xScaleDraw->setIsLog(m_plotInfo.xBottomIsLog);
    // m_canvas->setIsLog(m_plotInfo.xBottomIsLog);
    m_xScaleDraw->setType(m_plotInfo.XType);

    if (m_plotInfo.XType < 2)
        // m_canvas->setXAxisScale(0,m_plotInfo.XRange);
        m_canvas->setAxisRange(QwtPlot::xBottom, 0, m_plotInfo.XRange);
    else
        // m_canvas->setXAxisScale(m_plotInfo.XMin,m_plotInfo.XMax);
        m_canvas->setAxisRange(QwtPlot::xBottom, m_plotInfo.XMin, m_plotInfo.XMax);

    if (m_plotInfo.IsAutoYAxis) {
        autoResetAxis(QwtPlot::yLeft);
    } else {
        m_canvas->setAxisRange(QwtPlot::yLeft, m_plotInfo.YMin, m_plotInfo.YMax);
        if (axisEnabled(QwtPlot::yRight)) {
            m_canvas->setAxisRange(QwtPlot::yRight, m_plotInfo.YRightMin, m_plotInfo.YRightMax);
        }
    }

    if (m_plotInfo.IsShowXTitle) {
        setAxisTitle(QwtPlot::xBottom, m_plotInfo.XTitle);
    } else {
        setAxisTitle(QwtPlot::xBottom, "");
    }

    m_yScaleDraw->setYscaleNum(m_plotInfo.yScaleNum);

    if (m_plotInfo.IsShowYTitle) {
        if (m_plotInfo.isShowYsCaleNum) {
            QString space = "				";
            setAxisTitle(QwtPlot::yLeft, m_plotInfo.YTitle + space + getYscaleString(m_plotInfo.yScaleNum));
        } else {
            setAxisTitle(QwtPlot::yLeft, m_plotInfo.YTitle);
        }
    } else {
        if (m_plotInfo.isShowYsCaleNum) {
            setAxisTitle(QwtPlot::yLeft, getYscaleString(m_plotInfo.yScaleNum));
        } else {
            setAxisTitle(QwtPlot::yLeft, "");
        }
    }

    if (m_plotInfo.GridType == 0) {
        if (m_plotGrid) {
            m_plotGrid->detach();
            delete m_plotGrid;
            m_plotGrid = nullptr;
        }
    } else if (m_plotInfo.GridType == 1) {
        if (!m_plotGrid) {
            // m_plotGrid=new ItemPlotGrid();
            m_plotGrid = new QwtPlotGrid();
            // QPen pen(QColor(66,72,97));
            QPen pen(QColor("#333333"));
            QVector<qreal> pattern;
            pattern.append(3);
            pattern.append(3);
            pen.setDashPattern(pattern);
            m_plotGrid->setMajorPen(pen);
            // m_plotGrid->setMajorPen(QPen(QColor(66,72,97), 0.0f, Qt::DashLine));
            m_plotGrid->setVisible(true);
            m_plotGrid->attach(this);
        }
    }

    m_canvas->setDecimal(m_plotInfo.xDecimal, m_plotInfo.yDecimal);
    m_xScaleDraw->setDecimal(m_plotInfo.xDecimal);
    m_yScaleDraw->setDecimal(m_plotInfo.yDecimal);
    Q_EMIT setAxisDecimal(m_plotInfo.xDecimal, m_plotInfo.yDecimal);
}

void ItemPlot::setPlotInfo(PlotInfo &info, bool needAdd)
{
    uninitAll();

    m_plotInfo = info;
    m_plotInfo.id = ItemPlot::generateUuid();
    applyPlotInfo();

    if (needAdd) {
        for (auto &varIter : m_plotInfo.VaribleInfoList) {
            if (!initVarbleInfo(varIter))
                continue;
            mVarInitState[varIter.Path] = true;
            for (auto &curveIter : varIter.CurveInfoList) {
                initCurve(curveIter, varIter.Name, varIter.Type, varIter.Alias, true);
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
                if (!curve)
                    continue;
                QReadWriteLock &curveLock = curve->getCurveLock();
                QWriteLocker guardLocker(&curveLock);
                ItemPlotCurveSeriesData *curveData = curve->curveData();
                if (!curveData)
                    continue;
                curveData->setType(m_plotInfo.XType);
            }
        }
    }

    // refresh();

    if (m_plotInfo.axisRanges.contains(QwtPlot::xBottom)) {
        AxisRange range = m_plotInfo.axisRanges[QwtPlot::xBottom];
        // m_canvas->setXAxisScale(range.min,range.max);
        m_canvas->setAxisScale(QwtPlot::xBottom, range.min, range.max);
        m_canvas->setPlotCurveDataSelectRange(range.min, range.max);
    }

    if (m_plotInfo.axisRanges.contains(QwtPlot::yLeft)) {
        AxisRange range = m_plotInfo.axisRanges[QwtPlot::yLeft];
        // m_canvas->setYAxisScale(range.min,range.max);
        m_canvas->setAxisScale(QwtPlot::yLeft, range.min, range.max);
    }

    if (m_plotInfo.axisRanges.contains(QwtPlot::yRight)) {
        AxisRange range = m_plotInfo.axisRanges[QwtPlot::yRight];
        // m_canvas->setYAxisScale(range.min,range.max);
        m_canvas->setAxisScale(QwtPlot::yRight, range.min, range.max);
    }

    m_plotInfo.axisRanges.clear();
    m_canvas->setMarkerInfo(m_plotInfo.Markers);
    refreshMarkerTableView();
}

PlotInfo &ItemPlot::getPlotInfo(void)
{
    m_plotInfo.Markers = m_canvas->getMarkerInfo();

    m_plotInfo.axisRanges.clear();
    AxisRange range;
    range.min = axisScaleDiv(QwtPlot::xBottom).lowerBound();
    range.max = axisScaleDiv(QwtPlot::xBottom).upperBound();
    m_plotInfo.axisRanges[QwtPlot::xBottom] = range;

    range.min = axisScaleDiv(QwtPlot::yLeft).lowerBound();
    range.max = axisScaleDiv(QwtPlot::yLeft).upperBound();
    m_plotInfo.axisRanges[QwtPlot::yLeft] = range;

    return m_plotInfo;
}

bool ItemPlot::setButtonEnable(RunButtonType buttonType, bool isEnable)
{
    // if (m_toolbar) {
    //     return m_toolbar->setButtonEnale(buttonType, isEnable);
    // }
    return false;
}

void ItemPlot::setPlotParam(const PlotParam &param)
{
    plotInfoUpdate();
    // m_plotInfo.id = param.id;
    m_plotInfo.XMin = param.xBottomAxisMin;
    m_plotInfo.XMax = param.xBottomAxisMax;
    m_plotInfo.YMin = param.yLeftAxisMin;
    m_plotInfo.YMax = param.yLeftAxisMax;
    m_plotInfo.XRange = param.xBottomAxisLen;
    m_plotInfo.IsAutoYAxis = param.isAutoYLeftAxis;
    m_plotInfo.XType = param.xBottomAxisType;
    m_plotInfo.IsShowTitle = param.isShowMainTitle;
    m_plotInfo.Title = param.mainTitle;
    m_plotInfo.IsShowSubTitle = param.isShowSubTitle;
    m_plotInfo.SubTitle = param.subTitle;
    m_plotInfo.IsShowXTitle = param.isShowXBottomAxisTitle;
    m_plotInfo.XTitle = param.xBottomAxisTitle;
    m_plotInfo.IsShowYTitle = param.isShowYLeftAxisTitle;
    m_plotInfo.YTitle = param.yLeftAxisTitle;
    m_plotInfo.SaveDataMode = param.saveDataMode;
    m_plotInfo.xBottomIsLog = param.xBottomIsLog;
    m_plotInfo.x_name = param.x_name;
    m_plotInfo.x_src = param.x_src;
    m_plotInfo.isXRangeRoll = param.isXRangeRoll;
    m_plotInfo.isLogVisible = param.isLogVisible;

    m_canvas->setPlotInfo(m_plotInfo);
    // 设置小数点后几位精度
    PlotDefaultProperty p;
    p.flags = (PlotFlag)(PlotFlag_XDecimal | PlotFlag_YDecimal);
    SetPlotDefaultProperty(p);

    /*	if(param.b_new)
            {
                    m_plotInfo.y_nameList.clear();
                    m_plotInfo.y_srcList.clear();
            }*/
    if (!m_plotInfo.y_nameList.contains(param.y_name)) {
        if (param.y_name != m_plotInfo.x_name) {
            m_plotInfo.y_nameList.append(param.y_name);
            m_plotInfo.y_srcList.append(param.y_src);
        }
    } else {
        for (int i = 0; i < m_plotInfo.y_nameList.size(); i++) {
            if (m_plotInfo.y_nameList[i] == param.y_name)
                m_plotInfo.y_srcList[i] = param.y_src;
        }
    }
    applyPlotInfo();
    replot(); // 加入这个之后重绘时游标数值也可以跟着变了
}

QReadWriteLock &ItemPlot::getPlotLock()
{
    QReadLocker tmpLock(&mPlotLock);
    return mPlotLock;
}

QString ItemPlot::getYscaleString(double num)
{
    QString scaleString = "10";
    if (num == 1000) {
        scaleString.push_back(0x00B3);
    } else if (num == 100) {
        scaleString.push_back(0x00B2);
    } else if (num == 10) {
        scaleString.push_back(0x00B9);
    } else if (num == 1) {
        scaleString.push_back(0x2070);
    } else if (num == 0.1) {
        scaleString.push_back(0x207B);
        scaleString.push_back(0x00B9);
    } else if (num == 0.01) {
        scaleString.push_back(0x207B);
        scaleString.push_back(0x00B2);
    } else if (num == 0.001) {
        scaleString.push_back(0x207B);
        scaleString.push_back(0x00B3);
    } else {
        scaleString.push_back(0x2070);
    }
    return scaleString;
}

void ItemPlot::setLegendPosition(QwtPlot::LegendPosition pos)
{
    if (plotLayout()->legendPosition() == pos)
        return;

    PlotLayOut *lay = new PlotLayOut;
    setPlotLayout(lay);
    lay->setSpacing(0);
    // lay->setCanvasMargin(0);
    //  lay->setAlignCanvasToScales(true);
    //   插入曲线说明项
    mPlotLegend = new ItemPlotLegend(this);
    mPlotLegend->setStyleSheet("background-color: white;font-size:12px;");
    mPlotLegend->setDefaultItemMode(QwtLegendData::Checkable);
    this->insertLegend(mPlotLegend, pos);
    QObject::connect(mPlotLegend, SIGNAL(checked(const QVariant &, bool, int)),
                     SLOT(legendChecked(const QVariant &, bool)));
}

void ItemPlot::uninitAll(void)
{
    for (auto &varIter : m_plotInfo.VaribleInfoList) {
        uninitVarbleInfo(varIter);
        for (auto &curveIter : varIter.CurveInfoList) {
            uninitCurve(curveIter);
        }
    }
    mVarInitState.clear();
}

void ItemPlot::showSettingDialog(const PlotInfo &plotInfo)
{
    // m_toolbar->setEnabled(false);
    if (!m_settingDialog) {
        m_settingDialog = new CustomPlotSettingsDialog(this);
        connect(m_settingDialog, SIGNAL(changed(bool)), this, SLOT(onSettingsChanged(bool)));
    }

    mPlotLock.lockForRead();
    m_settingDialog->setPlotInfo(plotInfo);
    mPlotLock.unlock();
    m_settingDialog->setNodeState();
    m_settingDialog->show();
}

void ItemPlot::setDefaultStyle()
{
    this->setStyleSheet("ItemPlot{background-color:white}");
    if (mPlotLegend) {
        mPlotLegend->setStyleSheet("background-color: white;font-size:12px;");
    }
}

void ItemPlot::removeVarible(void)
{
    mPlotLock.lockForWrite();
    if (m_plotInfo.VaribleInfoList.size() == 0) {
        mPlotLock.unlock();
        return;
    }

    uninitAll();
    m_plotInfo.VaribleInfoList.clear();
    mPlotLock.unlock();
    autoResetAxis(yLeft);
    replot();

    Q_EMIT modifyNotify();

    refreshMarkerTableView();
}

void ItemPlot::setXType(int xtype)
{
    if (xtype > 1)
        return;

    for (auto &it : m_plotInfo.VaribleInfoList) {
        for (auto &curveInfo : it.CurveInfoList) {
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveInfo.ItemPlotCurveObject);
            if (!curve)
                continue;
            QReadWriteLock &curveLock = curve->getCurveLock();
            QWriteLocker guardLocker(&curveLock);
            ItemPlotCurveSeriesData *curveData = curve->curveData();
            if (!curveData)
                continue;
            curveData->setType(xtype);
            curveData->resetSelectedRange();
        }
    }
}

bool ItemPlot::isInitVarSucess()
{
    if (mVarInitState.isEmpty() || mVarInitState.values().contains(false))
        return false;
    return true;
}

void ItemPlot::resetWatcherPackageSize()
{
    int maxSize = 0;
    for (auto &var : m_plotInfo.VaribleInfoList) {
        QStringList &splited = var.Path.split(".");
        if (splited.size() != 5u || splited[1] != NPS_DataDictionaryNodeName)
            continue;
        for (auto &curveItera : var.CurveInfoList) {
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveItera.ItemPlotCurveObject);
            if (!curve)
                continue;
            QReadWriteLock &curvelock = curve->getCurveLock();
            QReadLocker guard(&curvelock);
            ItemPlotCurveSeriesData *curveData = curve->curveData();
            if (!curveData)
                continue;

            if (maxSize < curveData->len())
                maxSize = curveData->len();
        }
    }

    if (mWatcher) {
        int totalDataSize = mWatcher->getTotalSize();
        int sz = 0;
        if (maxSize < totalDataSize * 0.3) {
            sz = (int)(totalDataSize * 0.01);
        } else if (maxSize >= totalDataSize * 0.3 && maxSize < totalDataSize * 0.75) {
            sz = (int)(totalDataSize * 0.03);
        } else {
            sz = (int)(totalDataSize * 0.05);
        }
        if (sz == 0)
            sz = 1;
        if (sz <= mWatcher->getPackageSize())
            return;
        mWatcher->setPackageSize(sz);
    }
}

void ItemPlot::changeLineStyle(ItemPlotCurve *plotCurve, QColor &color, int w, int style)
{
    if (!plotCurve)
        return;
    QPen tmpPen(color, w);
    switch (style) {
    case 1:
        tmpPen.setStyle(Qt::DashLine);
        tmpPen.setDashOffset(20);
        break;
    case 2:
        tmpPen.setStyle(Qt::DotLine);
        tmpPen.setDashOffset(20);
        break;
    case 3:
        tmpPen.setStyle(Qt::DashDotLine);
        tmpPen.setDashOffset(20);
        break;
    default:
        tmpPen.setStyle(Qt::SolidLine);
        break;
    }
    plotCurve->setPen(tmpPen);
}

ItemPlot *ItemPlot::getItemPlotByVarPath(const QString &varPath)
{
    if (varPath.isEmpty())
        return nullptr;
    PIMonitorPanelPlotServer monitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!monitorServer)
        return nullptr;

    MonitorPanelPlotServer *monitorInstance = static_cast<MonitorPanelPlotServer *>(monitorServer.data());
    if (!monitorInstance)
        return nullptr;

    QList<ItemPlot *> &allPlotItem = monitorInstance->getAllItemPlots();
    for (ItemPlot *p : allPlotItem) {
        if (!p || p == this)
            continue;
        QReadWriteLock &plotLock = p->getPlotLock();
        QReadLocker tempGuard(&plotLock);
        const QStringList &info = p->getPlotAllParas();
        if (info.contains(varPath))
            return p;
    }
    return nullptr;
}

void ItemPlot::copyCurveDataFromSrc(const QMap<QString, ItemPlot *> src)
{
    if (src.isEmpty())
        return;
    QStringList srcList = src.keys();
    // QFuture<void> res = QtConcurrent::map(srcList, [&](QString &eachOne) {
    for (auto &eachOne : src.keys()) {
        if (!src[eachOne])
            continue;
        QReadWriteLock &srcLock = src[eachOne]->getPlotLock();
        QReadLocker srcLocker(&srcLock);
        PlotInfo &info = src[eachOne]->getPlotInfo();
        auto srcVarInfo = std::find_if(info.VaribleInfoList.begin(), info.VaribleInfoList.end(),
                                       [&](PlotVaribleInfo &var) { return var.Path == eachOne; });
        if (srcVarInfo == info.VaribleInfoList.end()) {
            continue;
        }

        ItemPlotCurve *srcCurve = static_cast<ItemPlotCurve *>(srcVarInfo->CurveInfoList[0].ItemPlotCurveObject);
        if (!srcCurve) {
            continue;
        }

        QReadWriteLock &srcCurveLock = srcCurve->getCurveLock();
        QReadLocker srcCurveGuard(&srcCurveLock);
        if (srcCurve->curveData()->data().isEmpty()) {
            continue;
        }

        auto dstVarInfo = std::find_if(m_plotInfo.VaribleInfoList.begin(), m_plotInfo.VaribleInfoList.end(),
                                       [&](PlotVaribleInfo &var) { return var.Path == eachOne; });
        if (dstVarInfo == m_plotInfo.VaribleInfoList.end()) {
            continue;
        }

        ItemPlotCurve *dstCurve = static_cast<ItemPlotCurve *>(dstVarInfo->CurveInfoList[0].ItemPlotCurveObject);
        if (!dstCurve) {
            continue;
        }

        QReadWriteLock &dstCurveLock = dstCurve->getCurveLock();
        QWriteLocker dstCuveGuard(&dstCurveLock);
        ItemPlotCurveSeriesData *dstRawData = dstCurve->curveData();
        *dstRawData = (*srcCurve->curveData());
        dstRawData->setRange(0, dstRawData->data().size() - 1);
    }
    //});
    // res.waitForFinished();
}

void ItemPlot::stop(void)
{
    for (auto &varIter : m_plotInfo.VaribleInfoList) {
        uninitVarbleInfo(varIter);
        mVarInitState[varIter.Path] = false;
    }
}

void ItemPlot::start(void)
{
    for (auto &varIter : m_plotInfo.VaribleInfoList) {
        if (initVarbleInfo(varIter))
            mVarInitState[varIter.Path] = true;
        else
            mVarInitState[varIter.Path] = false;
    }
}

QStringList ItemPlot::restart(void)
{
    QWriteLocker tmpLock(&mPlotLock);
    if (isInitVarSucess())
        return QStringList();

    QStringList varPath;
    for (auto &varIter : m_plotInfo.VaribleInfoList) {
        if (varIter.VaribleBase && varIter.ElementBase) {
            continue;
        }

        if (initVarbleInfo(varIter)) {
            varPath << varIter.Path;
            mVarInitState[varIter.Path] = true;
            for (auto &curveIter : varIter.CurveInfoList) {
                initCurve(curveIter, varIter.Name, varIter.Type, varIter.Alias);
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
                if (!curve)
                    continue;
                QReadWriteLock &curveLock = curve->getCurveLock();
                QWriteLocker guardLocker(&curveLock);
                ItemPlotCurveSeriesData *curveData = (curve->curveData());
                if (!curveData)
                    continue;
                curveData->reset();
                curveData->setType(m_plotInfo.XType);
            }
        } else {
            mVarInitState[varIter.Path] = false;
        }
    }
    return varPath;
}

QStringList ItemPlot::refreshBindedVar(bool isActive, const QString &boardName)
{
    QStringList AllVar;
    if (isActive) {
        mPlotLock.lockForWrite();
        for (auto &varIter : m_plotInfo.VaribleInfoList) {
            QStringList &splited = varIter.Path.split(".");
            if (splited.size() != 5u || splited[1] != NPS_DataDictionaryNodeName || splited[2] != boardName)
                continue;
            if (!initVarbleInfo(varIter))
                continue;
            for (auto &curveIter : varIter.CurveInfoList) {
                initCurve(curveIter, varIter.Name, varIter.Type, varIter.Alias);
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
                if (!curve)
                    continue;
                if (!mVarInitState[varIter.Path]) {
                    QReadWriteLock &curveLock = curve->getCurveLock();
                    curveLock.lockForWrite();
                    ItemPlotCurveSeriesData *curveData = curve->curveData();
                    if (!curveData) {
                        curveLock.unlock();
                        continue;
                    }
                    curveData->setType(m_plotInfo.XType);
                    curveData->reset();
                    curveLock.unlock();
                }
            }
            if (!mVarInitState[varIter.Path]) {
                AllVar << varIter.Path;
                mVarInitState[varIter.Path] = true;
            }
        }
        mPlotLock.unlock();
    } else {
        mPlotLock.lockForWrite();
        for (auto &varIter : m_plotInfo.VaribleInfoList) {
            QStringList &splited = varIter.Path.split(".");
            if (splited.size() != 5u || splited[1] != NPS_DataDictionaryNodeName || splited[2] != boardName)
                continue;
            uninitVarbleInfo(varIter);
            for (auto &curveIter : varIter.CurveInfoList) {
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
                if (!curve)
                    continue;
                curve->setVisible(false);
                curve->detach();
            }
        }
        mPlotLock.unlock();
    }
    setDefaultStyle();
    replot();
    return AllVar;
}

void ItemPlot::renameVar(const QMap<QString, QString> &paras)
{
    QWriteLocker tmpLocker(&mPlotLock);
    if (paras.isEmpty())
        return;
    QString prefix = "Dictionary.";
    QList<PlotVaribleInfo> &allVar = m_plotInfo.VaribleInfoList;

    for (auto it = paras.begin(); it != paras.end(); ++it) {
        QStringList &inputOldSplited = it.key().split(".");
        QStringList &inputNewSplited = it.value().split(".");
        if (inputOldSplited.isEmpty() || inputNewSplited.isEmpty() || inputOldSplited.size() > 4
            || inputOldSplited.size() != inputNewSplited.size()) {
            continue;
        }

        for (auto &var : allVar) {
            if (!var.Path.contains(prefix + it.key()))
                continue;

            QStringList &varSplited = var.Path.split(".");
            if (varSplited.size() != 5u) {
                continue;
            }

            for (int i = 0; i < inputNewSplited.size(); ++i) {
                if (i + 1 > varSplited.size() - 1)
                    continue;
                varSplited[i + 1] = inputNewSplited[i];
            }

            QString oldVar = var.Path;
            QString newVar = varSplited.join(".");

            var.Path = newVar;
            var.Name = varSplited.back();
            var.Alias = varSplited[3] + "." + varSplited.back();
            if (!initVarbleInfo(var))
                continue;
            if (mWatcher) {
                mWatcher->renameVar(oldVar, newVar);
            }
            mVarInitState.remove(oldVar);
            mVarInitState[newVar] = true;
            for (auto &curveVar : var.CurveInfoList) {
                curveVar.Name = var.Name;
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveVar.ItemPlotCurveObject);
                if (!curve)
                    continue;
                curve->setTitle(var.Alias);
            }
        }
    }
    mPlotLegend->update();
}

const QStringList ItemPlot::getPlotAllParas()
{
    QStringList res;
    for (int i = 0; i < m_plotInfo.VaribleInfoList.size(); ++i) {
        res << m_plotInfo.VaribleInfoList[i].Path;
    }
    return res;
}

void ItemPlot::autoResetAxis(QwtPlot::Axis axis)
{
    double yMin = 0.0f, yMax = 0.0f;
    bool b = true;
    for (auto &var : m_plotInfo.VaribleInfoList) {
        for (auto &curve : var.CurveInfoList) {
            if (!curve.ItemPlotCurveObject)
                continue;
            ItemPlotCurve *plotCurve = static_cast<ItemPlotCurve *>(curve.ItemPlotCurveObject);
            if (!plotCurve)
                continue;
            if (plotCurve->yAxis() != axis)
                continue;
            QReadWriteLock &curveLock = plotCurve->getCurveLock();
            QReadLocker guardLocker(&curveLock);
            ItemPlotCurveSeriesData *curveData = plotCurve->curveData();
            if (!curveData)
                continue;
            QRectF rect = curveData->boundingRect();
            if (qIsNaN(rect.bottom()) || qIsInf(rect.bottom()) || qIsInf(rect.top()) || qIsNaN(rect.top())) {
                b = false;
                continue;
            }
            if (b) {
                yMin = rect.bottom();
                yMax = rect.top();
                b = false;
            } else {
                if (rect.top() > yMax)
                    yMax = rect.top();
                if (rect.bottom() < yMin)
                    yMin = rect.bottom();
            }
        }
    }

    // 目前传过来的数据范围在-1e300至1e300
    qreal nAdjust = yMax - yMin;
    if (yMax == 0.0 && yMin == 0.0) {
        nAdjust = 1.0;
    } else {
        if (nAdjust == 0.0) {
            nAdjust = qAbs(yMin) / 10;
        } else {
            nAdjust = qAbs(nAdjust) / 10;
        }
    }
    m_canvas->setAxisRange(axis, yMin - nAdjust, yMax + nAdjust);
}

void ItemPlot::resetAxis(void)
{
    m_canvas->zoomBase(true);
    QReadLocker tmpLock(&mPlotLock);
    if (m_plotInfo.IsAutoYAxis) {
        autoResetAxis(QwtPlot::yLeft);
    } else {
        m_canvas->setAxisRange(QwtPlot::yLeft, m_plotInfo.YMin, m_plotInfo.YMax);
    }
    if (axisEnabled(QwtPlot::yRight)) {
        if (!m_plotInfo.IsAutoYRightAxis)
            m_canvas->setAxisRange(QwtPlot::yRight, m_plotInfo.YRightMin, m_plotInfo.YRightMax);
        else
            autoResetAxis(QwtPlot::yRight);
    }
    replot();
}

void ItemPlot::resizeEvent(QResizeEvent *event)
{
    QwtPlot::resizeEvent(event);
}

void ItemPlot::enterEvent(QEvent *event)
{
    QwtPlot::enterEvent(event);
    // if (m_plotInfo.childsInfo.size() > 1) {
    //     m_toolbar->setMode(1);
    // } else {
    //     m_toolbar->setMode(0);
    // }
    // if ((CutType)gConfGet("CutType").toInt() != UltimateCut) {
    //     m_toolbar->show();
    // }

    this->setStyleSheet("ItemPlot{background-color:#eef5fb}");
    if (mPlotLegend) {
        mPlotLegend->setStyleSheet("background-color: #eef5fb;font-size:12px;");
    }
    m_canvas->setCovered(true);
    m_canvas->update();
}

void ItemPlot::leaveEvent(QEvent *event)
{
    QwtPlot::leaveEvent(event);
    setDefaultStyle();
    m_canvas->setCovered(false);
    m_canvas->update();
}

void ItemPlot::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasText())
        return;
    // if (m_settingDialog)
    //     return;
    DragDataResolver dragDataResolver(event->mimeData()->text(), m_projectName);
    if (!dragDataResolver.isVariable() && !dragDataResolver.isMultiVariable())
        return;
    bool addRst = true;
    if (dragDataResolver.isVariable()) {
        addRst = addCurve(QStringList() << dragDataResolver.variablePath());
    } else if (dragDataResolver.isMultiVariable()) {
        addRst = addCurve(dragDataResolver.variablePathList());
    }
    if (addRst) // 变量添加没有问题
        event->acceptProposedAction();
    else
        event->ignore(); // 不接受拖放事件
}

void ItemPlot::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        DragDataResolver dragDataResolver(event->mimeData()->text(), m_projectName);

        if (!dragDataResolver.isVariable() && !dragDataResolver.isMultiVariable())
            return;
        event->acceptProposedAction();
    }
}

void ItemPlot::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        DragDataResolver dragDataResolver(event->mimeData()->text(), m_projectName);
        if (!dragDataResolver.isVariable() && !dragDataResolver.isMultiVariable())
            return;
        event->acceptProposedAction();
    }
}

void ItemPlot::paintEvent(QPaintEvent *event)
{
    QwtPlot::paintEvent(event);
}

void ItemPlot::mousePressEvent(QMouseEvent *event)
{
    setDefaultStyle();
    m_canvas->setCovered(false);
    m_canvas->update();
}

void ItemPlot::mouseDoubleClickEvent(QMouseEvent *event)
{
    QwtPlot::mouseDoubleClickEvent(event);
    if ((CutType)gConfGet("CutType").toInt() != UltimateCut) {
        if (event->button() == Qt::LeftButton && m_plotInfo.childsInfo.size() == 0)
            onPlotSettingDialog();
    }
}

void ItemPlot::hideEvent(QHideEvent *event)
{
    QwtPlot::hideEvent(event);

    // if (m_settingDialog) {
    //     m_settingDialog->reject(); // 会触发onSettingsChanged
    // }
}

void ItemPlot::onToolbarTriggered(int index)
{
    switch (index) {
    case 0:
        // 设置
        onPlotSettingDialog();
        break;
    case 1:
        // 删除
        removeVarible();
        break;
    case 2: // 数据分析
        m_canvas->onDataAnalyseActionTriggered();
        break;
        break;
    case 3: // 游标
    {
        Q_EMIT setMarkerTableViewShow(false); // isShow为true时一直显示，为false是则根据原来状态置反
    } break;

    case 4: // start
    {
        NotifyStruct notifyStruct;
        notifyStruct.paramMap["state"] = "start";
        commonRunStateHandle(notifyStruct);
    } break;
    case 5: // pause
    {
        NotifyStruct notifyStruct;
        notifyStruct.paramMap["state"] = "pause";
        commonRunStateHandle(notifyStruct);
    } break;
    case 6: // stop
    {
        NotifyStruct notifyStruct;
        notifyStruct.paramMap["state"] = "stop";
        commonRunStateHandle(notifyStruct);
    } break;
    default:
        break;
    }
}

void ItemPlot::commonRunStateHandle(NotifyStruct &notifyStruct)
{
    notifyStruct.code = Notify_PlotRunState;
    PlotInfo tmpInfo = getPlotInfo();
    notifyStruct.paramMap["x_name"] = tmpInfo.x_name;
    notifyStruct.paramMap["x_src"] = tmpInfo.x_src;
    notifyStruct.paramMap["plot_id"] = tmpInfo.id;

    PIMonitorPanelPlotServer pMonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!pMonitorServer) {
        // LOGOUT("IMonitorPanelPlotServer未注册", LOG_ERROR);
        return;
    }
    notifyStruct.paramMap["panel_name"] = pMonitorServer->GetMonitorPanelName();

    GraphCols graCols;
    const QStringList &y_nameList = tmpInfo.y_nameList;
    const QStringList &y_srcList = tmpInfo.y_srcList;
    if (y_nameList.size() == y_srcList.size()) {
        for (int i = 0; i < y_nameList.size(); i++) {
            ColInfo colInfo;
            colInfo.src = y_srcList.at(i);
            colInfo.name = y_nameList.at(i);
            graCols.append(colInfo);
        }
        notifyStruct.paramMap["y_GraphCols"] = QVariant::fromValue(graCols);
        PIServerInterfaceBase monitorPanelPlotServerBase = RequestServerInterface<IMonitorPanelPlotServer>();
        if (monitorPanelPlotServerBase)
            monitorPanelPlotServerBase->emitNotify(notifyStruct);
    }
}

QwtText ItemPlot::getAxisTitle(const QString &strTitle, const QColor &color)
{
    QwtText txt;
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(12);
    txt.setFont(font);
    txt.setText(strTitle);
    txt.setColor(color);
    return txt;
}

// 少部分图表信息刷新
void ItemPlot::plotInfoUpdate()
{
    if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
        QRgb rgb = gConfGet("General_BackgroundColor").toUInt();
        PlotDefaultProperty prop;
        prop.flags = PlotFlags(PlotFlag_BgColor | PlotFlag_GridType | PlotFlag_CurveWidth);
        prop.bgColor.setRgb(rgb);
        prop.gridType = gConfGet("General_GridType").toInt();
        prop.curveWidth = gConfGet("General_LineWidth").toInt();
        gDefault->setDefaultProperty(prop);
    }

    if (gDefault->isBgColor()) {
        m_plotInfo.BackgroundColor = gDefault->bgColor();
    }
    if (gDefault->isGridType()) {
        m_plotInfo.GridType = gDefault->gridType();
    }

    if (gDefault->isXDecimal()) {
        m_plotInfo.xDecimal = gDefault->xDecimal();
    }

    if (gDefault->isYDecimal()) {
        m_plotInfo.yDecimal = gDefault->yDecimal();
    }
}

void ItemPlot::cpoyPlotJustSettingInfo(PlotInfo &dst, const PlotInfo &src)
{
    dst.id = src.id;
    dst.XType = src.XType;
    dst.Title = src.Title;
    dst.SubTitle = src.SubTitle;
    dst.XRange = src.XRange;
    dst.BackgroundColor = src.BackgroundColor;
    dst.LegendSide = src.LegendSide;
    dst.GridType = src.GridType;
    dst.IsAutoYAxis = src.IsAutoYAxis;
    dst.YMin = src.YMin;
    dst.YMax = src.YMax;
    dst.IsShowTitle = src.IsShowTitle;
    dst.IsShowSubTitle = src.IsShowSubTitle;
    dst.XTitle = src.XTitle;
    dst.IsShowXTitle = src.IsShowXTitle;
    dst.YTitle = src.YTitle;
    dst.IsShowYTitle = src.IsShowYTitle;
    dst.XMin = src.XMin;
    dst.XMax = src.XMax;
    dst.xBottomIsLog = src.xBottomIsLog;
    dst.isLogVisible = src.isLogVisible;
    dst.SaveDataMode = src.SaveDataMode;
    // dst.Markers = src.Markers;
    dst.y_nameList = src.y_nameList;
    dst.y_srcList = src.y_srcList;
    dst.x_name = src.x_name;
    dst.x_src = src.x_src;
    dst.xDecimal = src.xDecimal;
    dst.yDecimal = src.yDecimal;
    dst.isXRangeRoll = src.isXRangeRoll;

    dst.YRightTitle = src.YRightTitle;
    dst.IsShowYRightTitle = src.IsShowYRightTitle;
    dst.IsAutoYRightAxis = src.IsAutoYRightAxis;
    dst.YRightMin = src.YRightMin;
    dst.YRightMax = src.YRightMax;
    dst.yRightDecimal = src.yRightDecimal;
    // dst.childsInfo = src.childsInfo;
    // dst.axisRanges = src.axisRanges;
    dst.isShowYsCaleNum = src.isShowYsCaleNum;
    dst.yScaleNum = src.yScaleNum;
}

void ItemPlot::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId) {
        QReadLocker tmpLock(&mPlotLock);
        if (!m_isNeedReplot) {
            return;
        }
        m_isNeedReplot = false;
        refresh();
    }
}

void ItemPlot::legendChecked(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = infoToItem(itemInfo);
    if (plotItem) {
        showCurve(plotItem, on);
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(plotItem);
        if (!curve)
            return;
        mPlotLock.lockForRead();
        for (uint i = 0; i < m_plotInfo.VaribleInfoList.size(); ++i) {
            QList<PlotCurveInfo> &allCurve = m_plotInfo.VaribleInfoList[i].CurveInfoList;

            auto var = std::find_if(allCurve.begin(), allCurve.end(),
                                    [&](PlotCurveInfo &lamb) { return lamb.ItemPlotCurveObject == curve; });
            if (var != allCurve.end()) {
                var->Checked = on;
                break;
            }
        }
        mPlotLock.unlock();
        Q_EMIT modifyNotify();
    }
}

void ItemPlot::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QwtLegend *lgd = qobject_cast<QwtLegend *>(legend());
    QList<QWidget *> legendWidgets = lgd->legendWidgets(itemToInfo(item));
    if (legendWidgets.size() == 1) {
        QwtLegendLabel *legendLabel = qobject_cast<QwtLegendLabel *>(legendWidgets[0]);
        if (legendLabel) {
            legendLabel->setChecked(on);
        }
    }
    replot();
}

void ItemPlot::initCurveStyle(QwtPlotItem *item)
{
    item->setVisible(true);
    QwtLegend *lgd = qobject_cast<QwtLegend *>(legend());
    QList<QWidget *> legendWidgets = lgd->legendWidgets(itemToInfo(item));
    if (legendWidgets.size() == 1) {
        QwtLegendLabel *legendLabel = qobject_cast<QwtLegendLabel *>(legendWidgets[0]);
        if (legendLabel) {
            QPalette p;
            // p.setColor(QPalette::Button,QColor(130,135,144));
            // p.setColor(QPalette::Dark,QColor(130,135,144));
            p.setColor(QPalette::Button, Qt::black);
            p.setColor(QPalette::Dark, Qt::black);
            legendLabel->setPalette(p);
            legendLabel->setChecked(true);
            // legendLabel->setStyleSheet("font-size:12px;");
        }
    }
}

void ItemPlot::onPlotSettingDialog(void)
{
    // if (CustomPlotSettingsDialog::isShow())
    //     return;

    // QReadLocker tmpLock(&mPlotLock);
    // if (m_settingDialog)
    //     return;

    showSettingDialog(m_plotInfo);
}

void ItemPlot::onDataDictionaryServerNotify(unsigned int code, const NotifyStruct &cmdStruct)
{
    if (code == Notify_ElementAdded) {
        QStringList &paralst = cmdStruct.paramMap["name"].toStringList();
        if (paralst.size() != 4)
            return;
        QString prefix = "Dictionary.";
        mPlotLock.lockForWrite();
        for (auto &var : m_plotInfo.VaribleInfoList) {
            if (var.Path != prefix + paralst.join("."))
                continue;
            if (!var.ElementBase || !var.VaribleBase)
                continue;
            initVarbleInfo(var);
        }
        mPlotLock.unlock();
    }
}

void ItemPlot::applyPlotInfoChanged(PlotInfo &plotInfo)
{
    mPlotLock.lockForWrite();
    bool isModify = false;
    if (m_plotInfo.Title != plotInfo.Title || m_plotInfo.IsShowTitle != plotInfo.IsShowTitle) {
        m_plotInfo.Title = plotInfo.Title;
        m_plotInfo.IsShowTitle = plotInfo.IsShowTitle;
        applyTitle(plotInfo);
        isModify = true;
    }

    if (m_plotInfo.xDecimal != plotInfo.xDecimal || m_plotInfo.yDecimal != plotInfo.yDecimal) {
        m_plotInfo.xDecimal = plotInfo.xDecimal;
        m_plotInfo.yDecimal = plotInfo.yDecimal;

        m_canvas->setDecimal(plotInfo.xDecimal, plotInfo.yDecimal);
        m_xScaleDraw->setDecimal(plotInfo.xDecimal);
        m_yScaleDraw->setDecimal(plotInfo.yDecimal);
        if (!isModify)
            isModify = true;
    }

    m_xScaleDraw->clearCache();
    m_yScaleDraw->clearCache();

    // NPS用不到
    // if (m_plotInfo.xBottomIsLog != plotInfo.xBottomIsLog)
    //{
    //	m_plotInfo.xBottomIsLog = plotInfo.xBottomIsLog;
    //
    //	if (plotInfo.xBottomIsLog) {
    //		setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
    //	}
    //	else {
    //		setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    //	}
    //}

    if (m_plotInfo.IsShowXTitle != plotInfo.IsShowXTitle || m_plotInfo.XTitle != plotInfo.XTitle) {
        m_plotInfo.IsShowXTitle = plotInfo.IsShowXTitle;
        m_plotInfo.XTitle = plotInfo.XTitle;

        if (plotInfo.IsShowXTitle) {
            setAxisTitle(QwtPlot::xBottom, plotInfo.XTitle);
        } else {
            setAxisTitle(QwtPlot::xBottom, "");
        }
        if (!isModify)
            isModify = true;
    }

    if (m_plotInfo.yScaleNum != plotInfo.yScaleNum || m_plotInfo.IsShowYTitle != plotInfo.IsShowYTitle
        || m_plotInfo.isShowYsCaleNum != plotInfo.isShowYsCaleNum || m_plotInfo.YTitle != plotInfo.YTitle) {
        m_plotInfo.yScaleNum = plotInfo.yScaleNum;
        m_plotInfo.IsShowYTitle = plotInfo.IsShowYTitle;
        m_plotInfo.isShowYsCaleNum = plotInfo.isShowYsCaleNum;
        m_plotInfo.YTitle = plotInfo.YTitle;

        m_yScaleDraw->setYscaleNum(plotInfo.yScaleNum);
        if (plotInfo.IsShowYTitle) {
            if (plotInfo.isShowYsCaleNum) {
                QString space = "				";
                setAxisTitle(QwtPlot::yLeft, plotInfo.YTitle + space + getYscaleString(plotInfo.yScaleNum));
            } else {
                setAxisTitle(QwtPlot::yLeft, plotInfo.YTitle);
            }
        } else {
            if (plotInfo.isShowYsCaleNum) {
                setAxisTitle(QwtPlot::yLeft, getYscaleString(plotInfo.yScaleNum));
            } else {
                setAxisTitle(QwtPlot::yLeft, "");
            }
        }
        if (!isModify)
            isModify = true;
    }

    if (m_plotInfo.BackgroundColor != plotInfo.BackgroundColor) {
        // 直接设置颜色不生效
        // this->setCanvasBackground(QBrush(plotInfo.BackgroundColor));
        QString canvasStyleSheet = QString("ItemPlotCanvas{border:1px solid #333333; background-color:%1}")
                                           .arg(plotInfo.BackgroundColor.name());
        m_canvas->setStyleSheet(canvasStyleSheet);
        m_plotInfo.BackgroundColor = plotInfo.BackgroundColor;
        if (!isModify)
            isModify = true;
    }

    // NPS用不到
    // if (m_plotInfo.LegendSide != plotInfo.LegendSide)
    //     setLegendPosition((QwtPlot::LegendPosition)plotInfo.LegendSide);

    if (m_plotInfo.GridType != plotInfo.GridType) {
        m_plotInfo.GridType = plotInfo.GridType;
        if (plotInfo.GridType == 0) {
            if (m_plotGrid) {
                m_plotGrid->detach();
                // delete m_plotGrid;
                // m_plotGrid = nullptr;
            }
        } else if (plotInfo.GridType == 1) {
            if (!m_plotGrid) {
                // m_plotGrid=new ItemPlotGrid();
                m_plotGrid = new QwtPlotGrid();
                // QPen pen(QColor(66,72,97));
                QPen pen(QColor("#333333"));
                QVector<qreal> pattern;
                pattern.append(3);
                pattern.append(3);
                pen.setDashPattern(pattern);
                m_plotGrid->setMajorPen(pen);
                // m_plotGrid->setMajorPen(QPen(QColor(66,72,97), 0.0f, Qt::DashLine));
                m_plotGrid->setVisible(true);
                m_plotGrid->attach(this);
            } else {
                m_plotGrid->attach(this);
            }
        }
        if (!isModify)
            isModify = true;
    }

    // 这里改进写法，因为平台的写法效率低、难看懂还容易出bug
    QStringList needAddVarPath;
    QList<PlotVaribleInfo> &allPlotVarInfo = m_plotInfo.VaribleInfoList;
    QList<PlotVaribleInfo> &dlgPlotVarinfo = plotInfo.VaribleInfoList;
    bool needResetAxis = false;
    for (auto it = allPlotVarInfo.begin(); it != allPlotVarInfo.end();) {
        auto dlgit = std::find_if(dlgPlotVarinfo.begin(), dlgPlotVarinfo.end(),
                                  [&](PlotVaribleInfo &lamb) { return lamb.Path == it->Path; });
        if (dlgit != dlgPlotVarinfo.end()) {
            for (auto &curveInfo : it->CurveInfoList) {
                if (dlgit->CurveInfoList.empty())
                    continue;

                ItemPlotCurve *plotCurve = (ItemPlotCurve *)(curveInfo.ItemPlotCurveObject);
                if (!plotCurve)
                    continue;
                if (curveInfo.Color != dlgit->CurveInfoList[0].Color || curveInfo.Width != dlgit->CurveInfoList[0].Width
                    || curveInfo.Style != dlgit->CurveInfoList[0].Style
                    || curveInfo.plotType != dlgit->CurveInfoList[0].plotType) {
                    curveInfo.Color = dlgit->CurveInfoList[0].Color;
                    curveInfo.Width = dlgit->CurveInfoList[0].Width;
                    curveInfo.Style = dlgit->CurveInfoList[0].Style;
                    curveInfo.plotType = dlgit->CurveInfoList[0].plotType;

                    QwtPlotCurve::CurveStyle curveStyle = QwtPlotCurve::Lines;
                    if (curveInfo.plotType == 1) {
                        curveStyle = QwtPlotCurve::Steps;
                    } else if (curveInfo.plotType == 2) {
                        curveStyle = QwtPlotCurve::Sticks;
                    }
                    changeLineStyle(plotCurve, curveInfo.Color, curveInfo.Width, curveInfo.Style);
                    plotCurve->setStyle(curveStyle);
                    if (!isModify)
                        isModify = true;
                }

                if (it->Alias != dlgit->Alias) {
                    it->Alias = dlgit->Alias;
                    plotCurve->setTitle(it->Alias);
                    if (!isModify)
                        isModify = true;
                }
            }

            dlgPlotVarinfo.erase(dlgit);
            ++it;
        } else {
            if (!isModify)
                isModify = true;
            needResetAxis = true;
            uninitVarbleInfo(*it);
            mVarInitState.remove(it->Path);
            for (int j = 0; j < it->CurveInfoList.size(); ++j) {
                uninitCurve(it->CurveInfoList[j]);
            }
            it = allPlotVarInfo.erase(it);
        }
    }

    QMap<QString, ItemPlot *> copySrc;
    for (auto &it : dlgPlotVarinfo) {
        if (!isModify)
            isModify = true;
        allPlotVarInfo.push_back(it);
        PlotVaribleInfo &lastOne = allPlotVarInfo.back();
        if (!initVarbleInfo(lastOne))
            continue;
        mVarInitState[it.Path] = true;
        for (auto &curveIter : lastOne.CurveInfoList) {
            initCurve(curveIter, lastOne.Name, lastOne.Type, lastOne.Alias);
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
            if (!curve)
                continue;
            QReadWriteLock &curveLock = curve->getCurveLock();
            QWriteLocker guardLocker(&curveLock);
            ItemPlotCurveSeriesData *curveData = curve->curveData();
            if (!curveData)
                continue;
            curveData->reset();
            curveData->setType(m_plotInfo.XType);
        }

        ItemPlot *src = getItemPlotByVarPath(it.Path);
        if (src) {
            copySrc[it.Path] = src;
            needResetAxis = true;
        } else {
            needAddVarPath << it.Path;
        }
    }

    copyCurveDataFromSrc(copySrc);

    if (m_plotInfo.IsAutoYAxis != plotInfo.IsAutoYAxis || m_plotInfo.YMin != plotInfo.YMin
        || m_plotInfo.YMax != plotInfo.YMax) {

        m_plotInfo.IsAutoYAxis = plotInfo.IsAutoYAxis;
        m_plotInfo.YMin = plotInfo.YMin;
        m_plotInfo.YMax = plotInfo.YMax;

        if (m_plotInfo.IsAutoYAxis) {
            needResetAxis = true;
        } else {
            m_canvas->setAxisRange(QwtPlot::yLeft, m_plotInfo.YMin, m_plotInfo.YMax);
        }
        if (!isModify)
            isModify = true;
    }

    if ((m_plotInfo.XRange != plotInfo.XRange && m_plotInfo.XType == plotInfo.XType && m_plotInfo.XType < 2)
        || m_plotInfo.XType != plotInfo.XType) {
        m_xScaleDraw->setType(plotInfo.XType);
        setXType(plotInfo.XType);
        m_plotInfo.XType = plotInfo.XType;
        m_plotInfo.XRange = plotInfo.XRange;
        m_plotInfo.XMax = plotInfo.XMax;
        m_plotInfo.XMin = plotInfo.XMin;

        if (plotInfo.XType < 2)
            m_canvas->setAxisRange(QwtPlot::xBottom, 0, plotInfo.XRange);
        else
            m_canvas->setAxisRange(QwtPlot::xBottom, plotInfo.XMin, plotInfo.XMax);
        if (!isModify)
            isModify = true;
    }

    if (!needAddVarPath.empty()) {
        mPlotLock.unlock();
        emitAddVariableNotify(needAddVarPath);
        setDefaultStyle();
        mWatcher->resetWatcherMap();
        replot();
        return;
    }

    if (needResetAxis) {
        autoResetAxis(yLeft);
    }

    if (!copySrc.isEmpty()) {
        mWatcher->resetWatcherMap();
        calAxisRangeWhileAddPt();
    }

    mPlotLock.unlock();

    if (isModify) {
        setDefaultStyle();
        replot();
    }
}

void ItemPlot::applyTitle(const PlotInfo &info)
{
    QwtText txt;
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(12);
    txt.setColor("#333333");

    if (info.IsShowTitle && !info.IsShowSubTitle) {
        font.setBold(true);
        txt.setFont(font);
        txt.setText(info.Title);
        QwtPlot::setTitle(txt);
    } else if (info.IsShowTitle && info.IsShowSubTitle) {
        font.setBold(false);
        txt.setFont(font);
        QString strTitle = QString("<html><head/><body><p align=\"center\"><b>%1</b><br>%2</p></body></html>")
                                   .arg(info.Title)
                                   .arg(info.SubTitle);
        txt.setText(strTitle);
        QwtPlot::setTitle(txt);
    } else if (!info.IsShowTitle && info.IsShowSubTitle) {
        font.setBold(false);
        txt.setFont(font);
        QString strTitle =
                QString("<html><head/><body><p align=\"center\"><br>%2</p></body></html>").arg(info.SubTitle);
        txt.setText(strTitle);
        QwtPlot::setTitle(txt);
    } else {
        txt.setFont(font);
        txt.setText("");
        QwtPlot::setTitle(txt);
    }
}

void ItemPlot::emitAddVariableNotify(const QStringList &variablePaths)
{
    if (variablePaths.isEmpty())
        return;

    PIServerInterfaceBase monitorPanelPlotServerBase = RequestServerInterface<IMonitorPanelPlotServer>();
    if (!monitorPanelPlotServerBase)
        return;

    QMap<QString, QVariant> params;
    params["variablePaths"] = variablePaths;
    monitorPanelPlotServerBase->emitNotify(CODE_ADDVARIABLE, params);
}

QString ItemPlot::defaultCurveName(const QString &variableName, PlotVaribleInfoType variableType, int curveRow,
                                   int curveCol)
{
    switch (variableType) {
    case Number:
        return variableName;
    case NumberArray: {
        if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
            return QString("%1").arg(variableName);
        }
        return QString("%1[%2]").arg(variableName).arg(curveCol);
    }

    case NumberArray2D:
        return QString("%1[%2,%3]").arg(variableName).arg(curveRow).arg(curveCol);
    }

    return QString();
}

void ItemPlot::onSettingsChanged(bool b)
{
    if (b) {
        PlotInfo plotInfo = m_settingDialog->getPlotInfo();
        applyPlotInfoChanged(plotInfo);

        Q_EMIT modifyNotify();
    }
    // m_settingDialog->deleteLater();
    // m_settingDialog = nullptr;

    // m_toolbar->setEnabled(true);
}

void ItemPlot::onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param)
{
    if (code != Notify_DrawingBoardRunning && code != Notify_DrawingBoardStopped && code != Notify_SimulationPaused
        && code != Notify_SimulationResume) {
        return;
    }

    if (code == Notify_DrawingBoardRunning || code == Notify_SimulationResume) {
        QReadLocker plotGuard(&mPlotLock);
        ProjectManager::PIProjectManagerServer pProjectManager = RequestServer<ProjectManager::IProjectManagerServer>();
        if (pProjectManager) {
            double totalTime = 0.0;
            double eachStep = 0.0;
            totalTime = pProjectManager->GetProjectConfig(KL_PRO::RUN_TIME).toDouble();
            eachStep = pProjectManager->GetProjectConfig(KL_PRO::STEP_SIZE).toDouble();
            int totalDataSize = totalTime / eachStep;
            if (mWatcher) {
                mWatcher->setPackageSize(0);
                mWatcher->setTotalSize(totalDataSize);
                resetWatcherPackageSize();
            }
        }

        if (code == Notify_DrawingBoardRunning) {
            for (auto &var : m_plotInfo.VaribleInfoList) {
                QStringList splited = var.Path.split(".");
                if (splited.size() != 5u || splited[1] != NPS_DataDictionaryNodeName)
                    continue;
                for (auto &curveInfo : var.CurveInfoList) {
                    ItemPlotCurve *curveObj = static_cast<ItemPlotCurve *>(curveInfo.ItemPlotCurveObject);
                    if (!curveObj) {
                        continue;
                    }

                    QReadWriteLock &curveLock = curveObj->getCurveLock();
                    QWriteLocker guard(&curveLock);
                    curveObj->curveData()->reset();
                }
            }
            mWatcher->resetWatcherMap();
            m_canvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, true);
            replot();
        }
    }

    if (code == Notify_DrawingBoardStopped || code == Notify_SimulationPaused) {
        QWriteLocker plotGuard(&mPlotLock);
        m_isNeedReplot = true;
        m_canvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);
    }
}

void ItemPlot::setXAxisFull(void)
{
    double nMin = 0, nMax = 0;
    bool b = true;
    QReadLocker guardLocker(&mPlotLock);
    for (auto &varIter : m_plotInfo.VaribleInfoList) {
        for (auto &curveIter : varIter.CurveInfoList) {
            if (!curveIter.Checked || !curveIter.ItemPlotCurveObject)
                continue;
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
            if (!curve || !curve->isVisible())
                continue;
            QReadWriteLock &curveLock = curve->getCurveLock();
            QReadLocker curveGuard(&curveLock);
            ItemPlotCurveSeriesData *curveData = curve->curveData();
            double tempMin = 0, tempMax = 0;
            if (!curveData->getXRange(tempMin, tempMax))
                continue;

            if (b) {
                b = false;
                nMin = tempMin;
                nMax = tempMax;
            } else {
                if (nMin > tempMin)
                    nMin = tempMin;
                if (nMax < tempMax)
                    nMax = tempMax;
            }
        }
    }
    if (b)
        return;
    m_canvas->setPlotCurveDataSelectRange(nMin, nMax);
    setAxisScale(QwtPlot::xBottom, nMin, nMax);
    replot();
    m_canvas->refreshAllMarkerPoint();
}

void ItemPlot::setYAxisFull(void)
{
    double nMin = 0, nMax = 0;
    bool b = true;
    QReadLocker guardLocker(&mPlotLock);
    for (auto &varIter : m_plotInfo.VaribleInfoList) {
        for (auto &curveIter : varIter.CurveInfoList) {
            if (!curveIter.Checked || !curveIter.ItemPlotCurveObject)
                continue;
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
            if (!curve || !curve->isVisible())
                continue;
            QReadWriteLock &curveLock = curve->getCurveLock();
            QReadLocker curveGuard(&curveLock);
            ItemPlotCurveSeriesData *curveData = curve->curveData();
            if (!curveData)
                continue;
            double tempMin = 0, tempMax = 0;
            if (!curveData->getYRange(tempMin, tempMax))
                continue;
            if (b) {
                b = false;
                nMin = tempMin;
                nMax = tempMax;
            } else {
                if (nMin > tempMin)
                    nMin = tempMin;
                if (nMax < tempMax)
                    nMax = tempMax;
            }
        }
    }
    if (b)
        return;

    qreal nAdjust = nMax - nMin;
    if (nMin == 0.0 && nMax == 0.0) {
        nAdjust = 1.0;
    } else {
        if (nAdjust == 0.0) {
            nAdjust = qAbs(nMin) / 16;
        } else {
            nAdjust = qAbs(nAdjust) / 16;
        }
    }

    nMin -= nAdjust;
    nMax += nAdjust;
    if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
        if (m_plotInfo.id == "IMPEDANCE_ANALYSIS:Phase") // 绘制阻抗分析相位图时，指定其Y轴刻度间隔为45。
            setAxisScale(QwtPlot::yLeft, nMin, nMax, 45);

    } else {
        setAxisScale(QwtPlot::yLeft, nMin, nMax);
    }
    replot();
    m_canvas->refreshAllMarkerPoint();
}

void ItemPlot::clearCurveData(const QString &variablePath)
{
    return;
    QReadLocker guardLocker(&mPlotLock);
    QList<PlotVaribleInfo> &allVar = m_plotInfo.VaribleInfoList;
    auto varIter = std::find_if(allVar.begin(), allVar.end(),
                                [&](PlotVaribleInfo &lamb) { return lamb.Path == variablePath; });
    if (varIter == m_plotInfo.VaribleInfoList.end())
        return;

    for (auto &curveIter : varIter->CurveInfoList) {
        if (!curveIter.ItemPlotCurveObject)
            continue;
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
        if (!curve)
            continue;
        QReadWriteLock &curveLock = curve->getCurveLock();
        QWriteLocker tpLock(&curveLock);
        curve->curveData()->reset();
    }
    m_isNeedReplot = true;
}

void ItemPlot::DeleteCurve(const QString &part_path, const QString &curveId)
{
    QWriteLocker tmpLocker(&mPlotLock);
    QList<PlotVaribleInfo>::iterator varIter;
    for (varIter = m_plotInfo.VaribleInfoList.begin(); varIter != m_plotInfo.VaribleInfoList.end();) {
        if (varIter->Path.contains(part_path)) {
            uninitVarbleInfo(*varIter);
            mVarInitState.remove(varIter->Path);
            for (auto &curveIter : varIter->CurveInfoList) {
                uninitCurve(curveIter);
            }
            varIter = m_plotInfo.VaribleInfoList.erase(varIter);
        } else {
            ++varIter;
        }
    }
    m_isNeedReplot = true;
}

void ItemPlot::addMarker(const MarkerInfo &info)
{
    Q_EMIT setMarkerTableViewShow(true);
    m_canvas->addMarker(info);
}

bool ItemPlot::isBinging(void) const
{
    return m_plotInfo.VaribleInfoList.size();
}

void ItemPlot::SetPlotDefaultProperty(const PlotDefaultProperty &property)
{
    if (property.flags & PlotFlag_BgColor) {
        m_plotInfo.BackgroundColor = property.bgColor;
    }

    if (property.flags & PlotFlag_GridType) {
        m_plotInfo.GridType = property.gridType;
    }

    if (property.flags & PlotFlag_XDecimal) {
        m_plotInfo.xDecimal = property.xDecimal;
    }

    if (property.flags & PlotFlag_YDecimal) {
        m_plotInfo.yDecimal = property.yDecimal;
    }

    // applyPlotInfo();

    if (property.flags & PlotFlag_CurveStyle || property.flags & PlotFlag_CurveWidth) {
        for (QList<PlotVaribleInfo>::iterator varIter = m_plotInfo.VaribleInfoList.begin();
             varIter != m_plotInfo.VaribleInfoList.end(); varIter++) {
            for (QList<PlotCurveInfo>::iterator curveIter = varIter->CurveInfoList.begin();
                 curveIter != varIter->CurveInfoList.end(); curveIter++) {
                if (!curveIter->ItemPlotCurveObject)
                    continue;

                if (property.flags & PlotFlag_CurveWidth) {
                    curveIter->Width = property.curveWidth;
                    static_cast<ItemPlotCurve *>(curveIter->ItemPlotCurveObject)
                            ->setPen(curveIter->Color, curveIter->Width);
                }
                if (property.flags & PlotFlag_CurveStyle) {
                    curveIter->Style = property.curveStyle;
                    QwtPlotCurve::CurveStyle curveStyle;
                    if (curveIter->Style == 0)
                        curveStyle = QwtPlotCurve::Lines;
                    else if (curveIter->Style == 1)
                        curveStyle = QwtPlotCurve::Steps;
                    else
                        curveStyle = QwtPlotCurve::Sticks;
                    static_cast<ItemPlotCurve *>(curveIter->ItemPlotCurveObject)->setStyle(curveStyle);
                }
            }
        }
    }

    m_isNeedReplot = true;
}

void ItemPlot::addEllipse(const QPointF &pos, qreal radius, const QColor &color, int width)
{
    m_ellipseList.append(new PlotEllipse(pos, radius, color, width, QString::null, this));
    replot();
}

void ItemPlot::clearEllipse(void)
{
    for (PlotEllipse *ellipse : m_ellipseList) {
        delete ellipse;
    }
    m_ellipseList.clear();
    replot();
}

ItemPlot *ItemPlot::clone(void)
{

    ItemPlot *plot = new ItemPlot("");
    // plot->m_plotInfo = m_plotInfo;

    return plot;
}

// isMerge为true时为合并，为false时为拆分
void ItemPlot::mergeInfoHandleEmit(bool isMerge, QList<PlotInfo> &plotInfos)
{
    if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
        NotifyStruct notifyStruct;
        if (isMerge)
            notifyStruct.code = Notify_PlotMerge;
        else
            notifyStruct.code = Notify_PlotUnmerge;
        QStringList plotIdList;
        for (PlotInfo &plotInfo : plotInfos) {
            plotIdList.append(plotInfo.id);
        }
        notifyStruct.paramMap["changed_plots"] = QVariant::fromValue(plotIdList);
        PIServerInterfaceBase monitorPanelPlotServerBase = RequestServerInterface<IMonitorPanelPlotServer>();
        if (monitorPanelPlotServerBase)
            monitorPanelPlotServerBase->emitNotify(notifyStruct);
    }
}

void ItemPlot::mergePlot(QList<PlotInfo> &plotInfos)
{
    // 目前只能两个合并，且合并后的Plot不能重复再与其它plot合并
    // 通过childsInfo.size()判断plot是否已合并过
    if (this->m_plotInfo.childsInfo.size() > 0 || plotInfos.size() == 0) {
        return;
    }
    if (plotInfos[0].childsInfo.size() > 0) {
        return;
    }

    m_plotInfo.childsInfo.append(PlotInfo(m_plotInfo));
    m_plotInfo.childsInfo.append(plotInfos[0]);

    m_plotInfo.IsAutoYRightAxis = plotInfos[0].IsAutoYAxis;
    m_plotInfo.YRightTitle = plotInfos[0].YTitle;
    m_plotInfo.IsShowYRightTitle = plotInfos[0].IsShowYTitle;
    m_plotInfo.YRightMin = plotInfos[0].YMin;
    m_plotInfo.YRightMax = plotInfos[0].YMax;
    m_plotInfo.yRightDecimal = plotInfos[0].yDecimal;
    m_plotInfo.Title = QString(m_plotInfo.childsInfo[0].Title + "," + m_plotInfo.childsInfo[1].Title);

#if 0
	if(m_plotInfo.VaribleInfoList.size() > 0 && plotInfos[0].VaribleInfoList.size() > 0)
	{
		if(m_plotInfo.VaribleInfoList[0].CurveInfoList[0].Color == plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Color)
		{
			for(int i = 0; i < CURVECOLORS_COUNT; i++)
			{
				if(QColor(CURVECOLORS[i]) == plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Color)
					continue;
				plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Color = QColor(CURVECOLORS[i]);
				ItemPlotCurve* curve = static_cast<ItemPlotCurve*>(plotInfos[0].VaribleInfoList[0].CurveInfoList[0].ItemPlotCurveObject);
				curve->setPen(plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Color,plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Width);
				break;
			}
		}
	}
#endif
    m_plotInfo.VaribleInfoList.append(plotInfos[0].VaribleInfoList);
    // 图表的id为两张子图id以逗号连接而成
    m_plotInfo.id.clear();
    for (PlotInfo &pi : m_plotInfo.childsInfo) {
        m_plotInfo.id += pi.id + ",";
    }
    m_plotInfo.id.chop(1); // 最后去掉最后的这个逗号

    mergeInfoHandleEmit(true, m_plotInfo.childsInfo);

    int k = m_plotInfo.VaribleInfoList.size() - plotInfos[0].VaribleInfoList.size();
    for (int i = 0; i < plotInfos[0].VaribleInfoList.size(); i++, k++) {
        for (int j = 0; j < m_plotInfo.VaribleInfoList[k].CurveInfoList.size(); j++) {
            m_plotInfo.VaribleInfoList[k].CurveInfoList[j].YAxis = 1;
            ItemPlotCurve *curve =
                    static_cast<ItemPlotCurve *>(m_plotInfo.VaribleInfoList[k].CurveInfoList[j].ItemPlotCurveObject);
            curve->setYAxis(QwtPlot::yRight);
            curve->attach(this);
            showCurve(curve, m_plotInfo.VaribleInfoList[k].CurveInfoList[j].isVisible);
        }
        // if(m_plotInfo.VaribleInfoList[k].ElementBase)
        //{
        //	m_plotInfo.VaribleInfoList[k].VaribleBase = m_plotInfo.VaribleInfoList[k].ElementBase->RefToVariable();
        // }
        // else
        //{
        //	m_plotInfo.VaribleInfoList[k].VaribleBase.clear();
        // }

        if (m_plotInfo.VaribleInfoList[k].VaribleBase) {
            m_plotInfo.VaribleInfoList[k].VaribleBase->AddVariableChangedHandler(this, VARIABLE_REFRESHTIME);
        }
    }

    enableAxis(QwtPlot::yRight, true);

    if (!m_yRightScaleDraw) {
        m_yRightScaleDraw = new ItemPlotScaleDraw(QwtPlot::yRight);
        QPalette p;
        p.setColor(QPalette::WindowText, Qt::white);
        this->axisWidget(QwtPlot::yRight)->setPalette(p);
    }
    setAxisScaleDraw(QwtPlot::yRight, m_yRightScaleDraw);
    m_yRightScaleDraw->setDecimal(m_plotInfo.yRightDecimal);
    // 设置左右坐标轴的标题及颜色
    if (m_plotInfo.IsShowYRightTitle) {
#if 1
        setAxisTitle(QwtPlot::yRight,
                     getAxisTitle(plotInfos[0].YTitle, plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Color));
#endif
    } else {
        setAxisTitle(QwtPlot::yRight, "");
    }
#if 1
    if (m_plotInfo.IsShowYTitle) {
        setAxisTitle(QwtPlot::yLeft,
                     getAxisTitle(m_plotInfo.YTitle, m_plotInfo.VaribleInfoList[0].CurveInfoList[0].Color));
    } else {
        setAxisTitle(QwtPlot::yLeft, "");
    }
    // 设置左右y轴的刻度颜色
    m_yScaleDraw->setColor(m_plotInfo.VaribleInfoList[0].CurveInfoList[0].Color);
    m_yRightScaleDraw->setColor(plotInfos[0].VaribleInfoList[0].CurveInfoList[0].Color);
#endif
    // applyYRightAxisInfo();

    if (m_plotInfo.XType < 2)
        // m_canvas->setXAxisScale(0,m_plotInfo.XRange);
        m_canvas->setAxisRange(QwtPlot::xBottom, 0, m_plotInfo.XRange);
    else
        // m_canvas->setXAxisScale(m_plotInfo.XMin,m_plotInfo.XMax);
        m_canvas->setAxisRange(QwtPlot::xBottom, m_plotInfo.XMin, m_plotInfo.XMax);

    if (m_plotInfo.IsAutoYAxis) {
        autoResetAxis(QwtPlot::yLeft);
    } else {
        // m_canvas->setYAxisScale(m_plotInfo.YMin,m_plotInfo.YMax);
        m_canvas->setAxisRange(QwtPlot::yLeft, m_plotInfo.YMin, m_plotInfo.YMax);
    }
    if (m_plotInfo.IsAutoYRightAxis) {
        autoResetAxis(QwtPlot::yRight);
    } else {
        // m_canvas->setYAxisScale(m_plotInfo.YRightMin,m_plotInfo.YRightMax);
        m_canvas->setAxisRange(QwtPlot::yRight, m_plotInfo.YRightMin, m_plotInfo.YRightMax);
    }

    applyTitle(m_plotInfo);
    refreshMarkerTableView();
    replot();
}

QList<PlotInfo> ItemPlot::unmergePlot(void)
{
    QList<PlotInfo> plotInfos;

    if (m_plotInfo.childsInfo.size() != 2) {
        return plotInfos;
    }

    mergeInfoHandleEmit(false, m_plotInfo.childsInfo);

    PlotInfo info = m_plotInfo.childsInfo[1];
    info.IsAutoYAxis = m_plotInfo.IsAutoYRightAxis;
    info.YTitle = m_plotInfo.YRightTitle;
    info.IsShowYTitle = m_plotInfo.IsShowYRightTitle;
    info.YMin = m_plotInfo.YRightMin;
    info.YMax = m_plotInfo.YRightMax;
    info.yDecimal = m_plotInfo.yRightDecimal;

    for (int i = 0; i < info.VaribleInfoList.size(); i++) {
        for (int j = 0; j < m_plotInfo.VaribleInfoList.size(); j++) {
            if (info.VaribleInfoList[i].Path == m_plotInfo.VaribleInfoList[j].Path) {
                info.VaribleInfoList[i] = m_plotInfo.VaribleInfoList[j];
                if (info.VaribleInfoList[i].VaribleBase) {
                    info.VaribleInfoList[i].VaribleBase->RemoveVariableChangedHandler(this);
                }
                for (int k = 0; k < info.VaribleInfoList[i].CurveInfoList.size(); k++) {
                    if (info.VaribleInfoList[i].CurveInfoList[k].ItemPlotCurveObject) {
                        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(
                                info.VaribleInfoList[i].CurveInfoList[k].ItemPlotCurveObject);
                        curve->setYAxis(QwtPlot::yLeft);
                        curve->detach();
                        info.VaribleInfoList[i].CurveInfoList[k].isVisible = curve->isVisible();
                    }
                }
                m_plotInfo.VaribleInfoList.removeAt(j);
                break;
            }
        }
    }

    ///////////////////////////////////////////////////////////
    enableAxis(QwtPlot::yRight, false);

    if (m_plotInfo.IsShowYTitle) {
        setAxisTitle(QwtPlot::yLeft, getAxisTitle(m_plotInfo.YTitle, Qt::black));
    } else {
        setAxisTitle(QwtPlot::yLeft, "");
    }
    // 设置左右y轴的刻度颜色
    m_yScaleDraw->setColor(Qt::black);

    m_plotInfo.Title = m_plotInfo.childsInfo.at(0).Title;
    applyTitle(m_plotInfo);
    info.Title = m_plotInfo.childsInfo.at(1).Title;

    m_plotInfo.id = m_plotInfo.childsInfo.at(0).id;
    info.id = m_plotInfo.childsInfo.at(1).id;

    m_plotInfo.childsInfo.clear();
    //////////////////////////////////////////////////////////
    plotInfos << info;
    return plotInfos;
}

void ItemPlot::setUnmergePlotInfo(const PlotInfo &plotInfo)
{

    m_plotInfo = plotInfo;
    applyPlotInfo();

    for (QList<PlotVaribleInfo>::iterator varIter = m_plotInfo.VaribleInfoList.begin();
         varIter != m_plotInfo.VaribleInfoList.end(); varIter++) {
        for (QList<PlotCurveInfo>::iterator curveIter = varIter->CurveInfoList.begin();
             curveIter != varIter->CurveInfoList.end(); curveIter++) {
            if (curveIter->ItemPlotCurveObject) {
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter->ItemPlotCurveObject);
                curve->attach(this);
                showCurve(curve, curveIter->isVisible);
            }
        }
        /*	if(varIter->ElementBase)
                {
                        varIter->VaribleBase = varIter->ElementBase->RefToVariable();
                }
                else
                {
                        varIter->VaribleBase.clear();
                }*/

        if (varIter->VaribleBase) {
            varIter->VaribleBase->AddVariableChangedHandler(this, VARIABLE_REFRESHTIME);
        }
    }
    m_canvas->setMarkerInfo(m_plotInfo.Markers);
    refreshMarkerTableView();
    QStringList variablePaths;
    for (PlotVaribleInfo &variableInfo : m_plotInfo.VaribleInfoList) {
        variablePaths << variableInfo.Path;
    }

    emitAddVariableNotify(variablePaths);
}

PlotInfo ItemPlot::getMergePlotInfo(void)
{
    for (QList<PlotVaribleInfo>::iterator varIter = m_plotInfo.VaribleInfoList.begin();
         varIter != m_plotInfo.VaribleInfoList.end(); varIter++) {
        for (QList<PlotCurveInfo>::iterator curveIter = varIter->CurveInfoList.begin();
             curveIter != varIter->CurveInfoList.end(); curveIter++) {
            if (curveIter->ItemPlotCurveObject) {
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveIter->ItemPlotCurveObject);
                curve->detach();
                curveIter->isVisible = curve->isVisible();
            }
            if (varIter->VaribleBase) {
                varIter->VaribleBase->RemoveVariableChangedHandler(this);
            }
        }
    }
    return m_plotInfo;
}

bool ItemPlot::canUnmergePlot(void) const
{
    return (m_plotInfo.childsInfo.size() > 1);
}

bool ItemPlot::canMergePlot(void) const
{
    // 只有有且只有一个变量且xType == 2 时才会合并，仅用于IS
    //  return (m_plotInfo.childsInfo.size() == 0 && m_plotInfo.VaribleInfoList.size() == 1 && m_plotInfo.XType == 2);
    return (m_plotInfo.childsInfo.size() == 0 /*&& m_plotInfo.VaribleInfoList.size() == 1*/ && m_plotInfo.XType == 2);
}

void ItemPlot::modifyCurveName(const QString &variableFullPath, const QString &curveName)
{
    for (QList<PlotVaribleInfo>::iterator variable = m_plotInfo.VaribleInfoList.begin();
         variable != m_plotInfo.VaribleInfoList.end(); variable++) {
        if (variable->Path != variableFullPath)
            continue;
        int count = variable->CurveInfoList.size();
        if (count == 0) {
            break;
        } else if (count > 1) {
            for (int i = 0; i < count; i++) {
                variable->CurveInfoList[i].Name = QString("%1%2").arg(curveName).arg(i + 1);
                if (variable->CurveInfoList[i].ItemPlotCurveObject)
                    static_cast<ItemPlotCurve *>(variable->CurveInfoList[i].ItemPlotCurveObject)
                            ->setTitle(variable->CurveInfoList[i].Name);
            }
        } else {
            variable->CurveInfoList[0].Name = curveName;
            if (variable->CurveInfoList[0].ItemPlotCurveObject)
                static_cast<ItemPlotCurve *>(variable->CurveInfoList[0].ItemPlotCurveObject)
                        ->setTitle(variable->CurveInfoList[0].Name);
        }

        break;
    }
}

QString ItemPlot::generateUuid()
{
    QString str = QUuid::createUuid().toString().remove("{").remove("}");
    return str;
}

void ItemPlot::OnVariableUpdate(PIVariableBase trigger, PIVariableBase &thisVariable, const DataInfo &series,
                                const int arrayIndex, const VariableType variableType, const NumberUnit &value)
{
}

bool ItemPlot::addPoint(ItemPlotCurveSeriesData *curveData, double value, double timeStamp, double otherData)
{
    QList<QPointF> &dataVec = curveData->data();
    if (dataVec.isEmpty() && timeStamp != 0.0)
        return false;
    if (!dataVec.isEmpty() && dataVec.back().x() >= timeStamp)
        return false;
    curveData->append(QPointF(timeStamp, value));
    return true;
}

QSizeF ItemPlot::getXAxisRange(void)
{
    return QSizeF();
}

void ItemPlot::refreshMarkerTableView()
{
    QStringList curveNames;
    for (PlotVaribleInfo &varible : m_plotInfo.VaribleInfoList) {
        ;
        for (PlotCurveInfo &curve : varible.CurveInfoList) {
            if (!curve.Checked)
                continue;
            curveNames << curve.Name;
        }
    }
    Q_EMIT setCurveNames(curveNames);
    m_canvas->refreshAllMarkerPoint();
}

void ItemPlot::calAxisRangeWhileAddPt()
{
    double curPos = 0;
    for (PlotVaribleInfo &varInfo : m_plotInfo.VaribleInfoList) {
        for (PlotCurveInfo &curveInfo : varInfo.CurveInfoList) {
            ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveInfo.ItemPlotCurveObject);
            if (!curve)
                continue;
            double tempPos = 0.0;
            QReadWriteLock &curveLock = curve->getCurveLock();
            QReadLocker curveGuard(&curveLock);
            if (curve->curveData()->data().isEmpty())
                continue;
            if (m_plotInfo.XType == 0) {
                tempPos = curve->curveData()->len() - 1;
            } else if (m_plotInfo.XType == 1) {
                tempPos = curve->curveData()->data().back().x();
            }
            if (tempPos > curPos)
                curPos = tempPos;
        }
    }

    int nTemp = curPos / m_plotInfo.XRange;
    if (nTemp == 0 || qAbs(nTemp * m_plotInfo.XRange - curPos) > 0.0000001) {
        m_canvas->setAxisScale(QwtPlot::xBottom, nTemp * m_plotInfo.XRange, (nTemp + 1) * m_plotInfo.XRange);
        for (PlotVaribleInfo &varInfo : m_plotInfo.VaribleInfoList) {
            for (PlotCurveInfo &curveInfo : varInfo.CurveInfoList) {
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveInfo.ItemPlotCurveObject);
                if (!curve)
                    continue;
                QReadWriteLock &curveLock = curve->getCurveLock();
                QWriteLocker curveGuard(&curveLock);
                curve->curveData()->setSelectedRange(nTemp * m_plotInfo.XRange, (nTemp + 1) * m_plotInfo.XRange);
            }
        }
    } else {
        m_canvas->setAxisScale(QwtPlot::xBottom, (nTemp - 1) * m_plotInfo.XRange, nTemp * m_plotInfo.XRange);
        for (PlotVaribleInfo &varInfo : m_plotInfo.VaribleInfoList) {
            for (PlotCurveInfo &curveInfo : varInfo.CurveInfoList) {
                ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(curveInfo.ItemPlotCurveObject);
                if (!curve)
                    continue;
                QReadWriteLock &curveLock = curve->getCurveLock();
                QWriteLocker curveGuard(&curveLock);
                curve->curveData()->setSelectedRange((nTemp - 1) * m_plotInfo.XRange, nTemp * m_plotInfo.XRange);
            }
        }
    }
}

void ItemPlot::refresh(void)
{
    calAxisRangeWhileAddPt();
    if (m_plotInfo.IsAutoYAxis) {
        autoResetAxis(QwtPlot::yLeft);
    }

    if (axisEnabled(QwtPlot::yRight) && m_plotInfo.IsAutoYRightAxis) {
        autoResetAxis(QwtPlot::yRight);
    }

    replot();
}

void ItemPlot::OnVariableDataListUpdate(PIVariableBase trigger, PIVariableBase &thisVariable, const DataInfo &series,
                                        IDataPackList *pDataList)
{
    QReadLocker guard(&mPlotLock);
    PlotVaribleInfo *varIter = nullptr;
    QList<PlotVaribleInfo> &allVar = m_plotInfo.VaribleInfoList;
    for (int i = 0; i < allVar.size(); ++i) {
        if (!allVar[i].ElementBase)
            continue;
        if (allVar[i].ElementBase->Id() == thisVariable->Element()->Id()) {
            varIter = &(allVar[i]);
        }
    }

    if (!varIter) {
        return;
    }

    if (varIter->Type != Number) {
        return;
    }

    QList<NumberUnit> valueList;
    pDataList->GetValueList(valueList);

    if (valueList.empty()) {
        return;
    }

    bool isFromFile = false;
    if (valueList.back().toDouble() == DBL_MAX)
        isFromFile = true;

    bool inputdata = true;
    QList<QPointF> tempVec;
    for (int j = 0; j < varIter->CurveInfoList.size(); j++) {
        ItemPlotCurve *curve = static_cast<ItemPlotCurve *>(varIter->CurveInfoList[j].ItemPlotCurveObject);
        if (!curve)
            continue;
        QReadWriteLock &curveLock = curve->getCurveLock();
        QWriteLocker guardLocker(&curveLock);
        ItemPlotCurveSeriesData *curveData = curve->curveData();
        for (uint i = 0; i < valueList.size(); ++i) {
            const double &timeStamp = (double)pDataList->GetDataTimestamp(i).number / 1000000;
            if (timeStamp == 0 && !curveData->data().isEmpty()) {
                if (isFromFile) {
                    if (curveData->data()[0].x() == 0.0) {
                        inputdata = false;
                        break;
                    } else {
                        tempVec = curveData->data();
                        curveData->reset();
                    }
                } else {
                    curveData->reset();
                }
            }

            if (!curveData->data().empty()) {
                if (timeStamp <= curveData->data().back().x()) {
                    continue;
                }
            }

            if (isFromFile && i == valueList.size() - 1) // 数据回放过滤最后一个标志位
                break;
            curveData->append(QPointF(timeStamp, valueList[i].toDouble()));
            if (isFromFile)
                continue;
            mWatcher->updateVarWatcher(varIter->Path, 1);
        }

        if (!tempVec.empty()) {
            for (int m = 0; m < tempVec.size(); ++m) {
                if (!curveData->data().empty()) {
                    if (tempVec[m].x() <= curveData->data().back().x()) {
                        continue;
                    }
                }
                curveData->append(tempVec[m]);
            }
        }
        tempVec.clear();
    }

    if (isFromFile) {
        if (!inputdata)
            return;
        mWatcher->resetWatcherMap();
        m_isNeedReplot = true;
    } else {
        if (mWatcher->isNeedReplot()) {
            resetWatcherPackageSize();
            mWatcher->resetWatcherMap();
            m_isNeedReplot = true;
        }
    }
}

void ItemPlot::notifySelecting(const AxisRanges &rect)
{
    if (!m_canvas)
        return;
    m_canvas->drawSelected(rect);
}

void ItemPlot::notifySelected(const AxisRanges &rect)
{
    if (!m_canvas)
        return;

    m_canvas->zoomSelected(rect);
}

void ItemPlot::notifyMouseMove(QPointF &point)
{
    if (!m_canvas)
        return;
    m_canvas->drawMouseMovePoint(point);
}

void ItemPlot::notifyResetCoordinate()
{
    resetAxis();
}

bool ItemPlot::addCurve(const QStringList &variableFullPathList)
{
    if (variableFullPathList.isEmpty())
        return false;

    mPlotLock.lockForWrite();
    QStringList allNeedToEmit;
    QMap<QString, ItemPlot *> copySrc;
    for (const QString &varPath : variableFullPathList) {
        auto tempFind = std::find_if(m_plotInfo.VaribleInfoList.begin(), m_plotInfo.VaribleInfoList.end(),
                                     [&](PlotVaribleInfo &info) { return info.Path == varPath; });
        if (tempFind != m_plotInfo.VaribleInfoList.end()) {
            LOGOUT("[" + varPath + "]" + tr("has been added to the curve control and cannot be bound repeatedly"),
                   LOG_WARNING);
            continue;
        }

        PlotVaribleInfo tempInfo;
        tempInfo.Path = varPath;
        m_plotInfo.VaribleInfoList.append(tempInfo);
        PlotVaribleInfo &newVar = m_plotInfo.VaribleInfoList.back();
        if (!initVarbleInfo(newVar)) {
            LOGOUT(tr("Add") + "[" + varPath + "]" + tr("to the curve control failed"),
                   LOG_WARNING); // 添加 到曲线控件失败
            continue;
        }
        mVarInitState[varPath] = true;

        QVector<QColor> hasUsed;
        for (auto &plotInfo : m_plotInfo.VaribleInfoList) {
            for (auto &curInfo : plotInfo.CurveInfoList) {
                if (!hasUsed.contains(curInfo.Color))
                    hasUsed << curInfo.Color;
            }
        }

        for (auto &curveIter : newVar.CurveInfoList) {
            bool setColor = false;
            for (const auto &x : defaultCurveColorVec) {
                if (hasUsed.contains(x)) {
                    continue;
                } else {
                    curveIter.Color = x;
                    setColor = true;
                    break;
                }
            }
            if (!setColor) {
                curveIter.Color = genRandomColor();
                while (hasUsed.contains(curveIter.Color)
                       || (curveIter.Color.red() >= 249 && curveIter.Color.green() >= 249
                           && curveIter.Color.blue() >= 249)) {
                    curveIter.Color = genRandomColor();
                }
            }
            initCurve(curveIter, newVar.Name, newVar.Type, newVar.Alias);
            ItemPlotCurve *tempCurve = static_cast<ItemPlotCurve *>(curveIter.ItemPlotCurveObject);
            if (!tempCurve)
                continue;
            QReadWriteLock &curveLock = tempCurve->getCurveLock();
            QWriteLocker guardLocker(&curveLock);
            ItemPlotCurveSeriesData *curveData = tempCurve->curveData();
            if (!curveData)
                continue;
            curveData->reset();
            curveData->setType(m_plotInfo.XType);
        }

        ItemPlot *src = getItemPlotByVarPath(varPath);
        if (src) {
            copySrc[varPath] = src;
        } else {
            allNeedToEmit << varPath;
        }
    }
    copyCurveDataFromSrc(copySrc);

    if (!allNeedToEmit.isEmpty()) {
        mPlotLock.unlock();
        emitAddVariableNotify(allNeedToEmit);
        setDefaultStyle();
        replot();
        Q_EMIT modifyNotify();
        return true;
    }

    if (!copySrc.isEmpty()) {
        autoResetAxis(yLeft);
        calAxisRangeWhileAddPt();
    }

    mPlotLock.unlock();
    setDefaultStyle();
    replot();
    mWatcher->resetWatcherMap();
    Q_EMIT modifyNotify();
    return true;
}

// 获取图表名字，如果是QuiKIS项目则使用id
QString ItemPlot::getTitle(void) const
{
    /*	if((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS)
            {
                    return getId();
            }
            else*/
    return m_plotInfo.Title;
}

ItemPlotScaleDraw::ItemPlotScaleDraw(QwtPlot::Axis axis)
    : m_time(QDateTime::currentDateTime()),
      m_type(0),
      m_offset(0.0f),
      m_axis(axis)
      //, m_xBottomIsLog(false)
      ,
      m_decimal(-1),
      m_color(Qt::black)
{
    if (m_axis == QwtPlot::xBottom) {
        setLabelAlignment(Qt::AlignHCenter | Qt::AlignBottom); // 改为水平居中显示 2022.11.16
                                                               //  setLabelRotation(-15);
    }
    mYScaleNum = 1.0;
    setTickLength(QwtScaleDiv::MinorTick, 0);
    setTickLength(QwtScaleDiv::MajorTick, 0);
    setTickLength(QwtScaleDiv::MediumTick, 0);
}

void ItemPlotScaleDraw::setType(int nType)
{
    m_type = nType;
    invalidateCache();
}

void ItemPlotScaleDraw::setOffset(double offset)
{
    m_offset = offset;
    invalidateCache();
}

void ItemPlotScaleDraw::setColor(const QColor &color)
{
    m_color = color;
    invalidateCache();
}

void ItemPlotScaleDraw::setDecimal(int decimal)
{
    m_decimal = decimal;
    invalidateCache();
}

QwtText ItemPlotScaleDraw::label(double v) const
{
    QwtText text;
    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(12);
    text.setFont(font);
    text.setColor(m_color);

    if (m_axis == QwtPlot::xBottom) {
        QList<qreal> tickList = scaleDiv().ticks(QwtScaleDiv::MajorTick);
        if (tickList[tickList.size() - 1] - v < DBL_EPSILON && m_type == 1) {
            text.setText(QString("%1(s)").arg(formatValue(v + m_offset, false)));
        } else {
            text.setText(formatValue(v + m_offset, false));
        }
        return text;
    } else {
        double showVal = v / mYScaleNum;
        if (m_axis == QwtPlot::yLeft)
            text.setText(formatValue(showVal, true));
        else
            text.setText(formatValue(showVal, false));
        return text;
    }
}

QString ItemPlotScaleDraw::formatValue(qreal value, int fillLength) const
{
    QString strValue = QString::number(value, 'E');
    int startPos = strValue.indexOf(".", 0);
    int endPos = strValue.indexOf("E", startPos);
    int curPos;
    for (curPos = endPos - 1; curPos > startPos; curPos--) {
        if (strValue.at(curPos) != QChar('0')) {
            curPos++;
            break;
        }
    }
    int len = endPos - curPos;
    if (len > 0) {
        strValue.remove(curPos, len);
    }

    if (strValue.length() < fillLength && fillLength > 0) {
        QString strZero(fillLength - strValue.length(), QChar(' '));
        strValue = strZero + strValue;
    }
    return strValue;
}

QString ItemPlotScaleDraw::formatValue(qreal value, bool bfillSpace) const
{
    QString strValue;
    qreal absValue = qAbs(value);
    if (absValue >= 10000 || (absValue < 0.001 && absValue != 0)) {
        return formatValue(value, bfillSpace ? 8 : 0);
    }
    QString strFomat;
    if (m_decimal < 0) {
        strFomat = "%g";
    }
    // else if(m_decimal == 0)
    //{
    //	strFomat = "%d";
    // }
    else {
        strFomat = QString("%.%1f").arg(m_decimal);
    }
    if (bfillSpace) {
        strFomat.insert(1, "8");
    }

    strValue.sprintf(strFomat.toLatin1(), value);

    return strValue;
}

void ItemPlotGrid::draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &rect) const
{
    if (yEnabled()) {
        QList<qreal> tickList = yScaleDiv().ticks(QwtScaleDiv::MajorTick);
        if (tickList.size() == 0)
            return;

        for (int i = 0; i < tickList.size(); i++) {
            tickList[i] = yMap.transform(tickList[i]);
        }
        if ((int)tickList[tickList.size() - 1] != (int)rect.top()) {
            tickList.push_back(rect.top());
        }
        if ((int)tickList[0] != (int)rect.height()) {
            tickList.push_front(rect.height());
        }
        bool bColor = true;
        p->save();
        p->setBrush(QColor(226, 233, 242));
        p->setPen(QColor(226, 233, 242));
        for (int i = tickList.size() - 1; i > 0; i--) {
            if (bColor) {
                QRectF colorRect(rect.left(), tickList[i], rect.width(), tickList[i - 1] - tickList[i]);
                p->drawRect(colorRect);
            }
            bColor = !bColor;
        }
        p->restore();
    }
    QwtPlotGrid::draw(p, xMap, yMap, rect);
    // drawGridLines(p,xMap,yMap,rect);
}

void ItemPlotGrid::drawLines(QPainter *painter, const QRectF &canvasRect, Qt::Orientation orientation,
                             const QwtScaleMap &scaleMap, const QList<double> &values) const
{
    const double x1 = canvasRect.left();
    const double x2 = canvasRect.right() - 1.0;
    const double y1 = canvasRect.top();
    const double y2 = canvasRect.bottom() - 1.0;

    const bool doAlign = QwtPainter::roundingAlignment(painter);

    for (int i = 0; i < values.count(); i++) {
        double value = scaleMap.transform(values[i]);
        if (doAlign)
            value = qRound(value);

        if (orientation == Qt::Horizontal) {
            if (qwtFuzzyGreaterOrEqual(value, y1) && qwtFuzzyLessOrEqual(value, y2)) {
                QwtPainter::drawLine(painter, x1, value, x2, value);
            }
        } else {
            if (qwtFuzzyGreaterOrEqual(value, x1) && qwtFuzzyLessOrEqual(value, x2)) {
                QwtPainter::drawLine(painter, value, y1, value, y2);
            }
        }
    }
}

void ItemPlotGrid::drawGridLines(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
                                 const QRectF &canvasRect) const
{
    //  draw minor grid lines
    QPen minorPen = this->majorPen();
    minorPen.setCapStyle(Qt::FlatCap);

    painter->setPen(minorPen);

    if (xEnabled() && xMinEnabled()) {
        drawLines(painter, canvasRect, Qt::Vertical, xMap, xScaleDiv().ticks(QwtScaleDiv::MinorTick));
        drawLines(painter, canvasRect, Qt::Vertical, xMap, xScaleDiv().ticks(QwtScaleDiv::MediumTick));
    }

    //  draw major grid lines
    QPen majorPen = this->majorPen();
    majorPen.setCapStyle(Qt::FlatCap);

    painter->setPen(majorPen);

    if (xEnabled()) {
        drawLines(painter, canvasRect, Qt::Vertical, xMap, xScaleDiv().ticks(QwtScaleDiv::MajorTick));
    }
}

void PlotLayOut::activate(const QwtPlot *plot, const QRectF &rect, Options options /*= 0x00*/)
{
    QwtPlotLayout::activate(plot, rect, options);
    const QMargins &plotMargin = plot->contentsMargins();
    QRectF rectTitle, recLegend;
    QPoint anchor(0, plotMargin.top());
    if (plot->title().text() != "") {
        rectTitle.moveTo(anchor);
        rectTitle.setWidth(rect.width());
        rectTitle.setHeight(titleRect().height());
        setTitleRect(rectTitle);
        anchor.setY(plotMargin.top() + titleRect().height());
    }

    recLegend.moveTo(anchor);
    recLegend.setWidth(rect.width());
    recLegend.setHeight(legendRect().height());
    setLegendRect(recLegend);
}

plotWatcher::plotWatcher(QObject *parent) : QObject(parent) { }

plotWatcher::~plotWatcher() { }

void plotWatcher::setPackageSize(int size)
{
    QWriteLocker guard(&mLock);
    mPackageSize = size;
}

int plotWatcher::getPackageSize()
{
    QReadLocker guard(&mLock);
    return mPackageSize;
}

void plotWatcher::setTotalSize(int totalSize)
{
    QWriteLocker guard(&mLock);
    mTotalSize = totalSize;
}

int plotWatcher::getTotalSize()
{
    QReadLocker guarf(&mLock);
    return mTotalSize;
}

void plotWatcher::clear()
{
    QWriteLocker guard(&mLock);
    mCounter.clear();
}

void plotWatcher::addVarWatcher(const QString &path)
{
    QStringList &tmpSplit = path.split(".");
    if (tmpSplit.size() != 5u) {
        return;
    }
    if (tmpSplit[1] != NPS_DataDictionaryNodeName)
        return;
    QWriteLocker guard(&mLock);
    if (mCounter.keys().contains(path))
        return;
    mCounter[path] = 0;
}

void plotWatcher::delVarWatcher(const QString &path)
{
    QWriteLocker guard(&mLock);
    if (!mCounter.keys().contains(path))
        return;
    mCounter.remove(path);
}

void plotWatcher::updateVarWatcher(const QString &path, int num)
{
    QWriteLocker guard(&mLock);
    if (!mCounter.keys().contains(path))
        return;
    mCounter[path] += num;
}

void plotWatcher::resetWatcherMap()
{
    QWriteLocker guard(&mLock);
    for (auto &x : mCounter.keys()) {
        mCounter[x] = 0;
    }
}

bool plotWatcher::isNeedReplot()
{
    QReadLocker guard(&mLock);
    QList<int> &allVals = mCounter.values();

    bool isSend = true;
    for (auto &x : allVals) {
        if (x < mPackageSize) {
            isSend = false;
            break;
        }
    }

    return isSend;
}

void plotWatcher::renameVar(const QString &oldVar, const QString &newVar)
{
    QWriteLocker guard(&mLock);
    if (!mCounter.keys().contains(oldVar))
        return;
    int num = mCounter[oldVar];
    mCounter.remove(oldVar);
    mCounter[newVar] = num;
}

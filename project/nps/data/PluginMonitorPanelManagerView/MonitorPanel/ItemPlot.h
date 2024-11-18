#ifndef ITEMPLOT_H
#define ITEMPLOT_H

#include <QObject>
#include <QReadWriteLock>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_picker_machine.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_draw.h>
#include <qwt_series_data.h>
#include <qwt_system_clock.h>

#include "PlotToolbar.h"

#include "../pluginmonitorpanelmanagerview_global.h"
#include "ItemPlotCanvas.h"
#include "ItemPlotCurve.h"
#include "ItemPlotLegend.h"
#include "ItemPlotPicker.h"
#include "def.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"

#define VARIABLE_REFRESHTIME 0

class CustomPlotSettingsDialog;
class PlotEllipse;

KCC_USE_NAMESPACE_MONITORPANEL

class PlotLayOut : public QwtPlotLayout
{
public:
    PlotLayOut() { }
    virtual ~PlotLayOut() { }

protected:
    virtual void activate(const QwtPlot *plot, const QRectF &rect, Options options = 0x00);
};

class ItemPlotScaleDraw : public QwtScaleDraw
{
public:
    ItemPlotScaleDraw(QwtPlot::Axis axis = QwtPlot::xBottom);

    void setType(int nType);
    void setOffset(double offset);
    void clearCache() { invalidateCache(); }
    void setColor(const QColor &color);
    void setDecimal(int decimal);

    virtual QwtText label(double v) const;

    void setYscaleNum(double scaleNum) { mYScaleNum = scaleNum; }

protected:
    QString formatValue(qreal value, int fillLength) const;
    QString formatValue(qreal value, bool bfillSpace) const;

private:
    QDateTime m_time;
    int m_type;
    double m_offset;
    QwtPlot::Axis m_axis;
    int m_decimal;
    QColor m_color;
    double mYScaleNum;
};

class plotWatcher : public QObject
{
    Q_OBJECT

public:
    plotWatcher(QObject *parent);
    virtual ~plotWatcher();

public:
    void setPackageSize(int size);
    int getPackageSize();
    void setTotalSize(int totalSize);
    int getTotalSize();

    void clear();
    void addVarWatcher(const QString &path);
    void delVarWatcher(const QString &path);
    void updateVarWatcher(const QString &path, int num);
    void resetWatcherMap();
    bool isNeedReplot();
    void renameVar(const QString &oldVar, const QString &newVar);

private:
    int mPackageSize = 0;
    int mTotalSize = 0;
    QReadWriteLock mLock;
    QMap<QString, int> mCounter;
};

class PLUGINMONITORPANELMANAGERVIEW_EXPORT ItemPlot : public QwtPlot, public IVariableNotifyHandler
{
    Q_OBJECT

public:
    ItemPlot(QString strTitle, QWidget *parent = nullptr);
    virtual ~ItemPlot();

    enum PlotState { State_Stop = 0, State_Start };
    void setPlotInfo(PlotInfo &info, bool needAdd = true);
    PlotInfo &getPlotInfo(void);
    bool setButtonEnable(RunButtonType buttonType, bool isEnable);
    void setDefaultStyle();

    void setPlotParam(const PlotParam &param);
    QReadWriteLock &getPlotLock();

    QString getYscaleString(double num);

    void setAutoStop(bool b) { m_isAutoStop = b; }
    bool autoStop(void) { return m_isAutoStop; }

    void stop(void);
    void start(void);
    QStringList restart(void);

    QStringList refreshBindedVar(bool isActive, const QString &boardName); // 激活或者取消激活画板调用
    void renameVar(const QMap<QString, QString> &paras); // 已添加到示波器的变量发生了重命名
    const QStringList getPlotAllParas();                 // 返回全部路径

    void autoResetAxis(QwtPlot::Axis axis);
    void resetAxis(void);

    void setCurrentProjectName(const QString &projectName) { m_projectName = projectName; }

    void setAllowZoomInXAxis(bool b) { m_canvas->setAllowZoomInXAxis(b); }
    void setAllowZoomInYAxis(bool b) { m_canvas->setAllowZoomInYAxis(b); }
    void setMousePoint(bool b) { m_canvas->setMousePoint(b); }
    void setCanPopMenu(bool can) { m_canvas->setCanPopMenu(can); }

    // void addPoint(int nIndex,double nValue,QDateTime &valueTime);

    bool addCurve(const QStringList &variableFullPathList);
    QString getTitle(void) const;
    QString getId(void) const { return m_plotInfo.id; }
    void setTitle(const QString &strTitle)
    {
        m_plotInfo.Title = strTitle;
        QwtPlot::setTitle(strTitle);
    }

    void setXAxisFull(void);
    void setYAxisFull(void);

    void clear(void) { removeVarible(); }
    void clearCurveData(const QString &variablePath);

    void addMarker(const MarkerInfo &info);

    bool isBinging(void) const;

    void SetPlotDefaultProperty(const PlotDefaultProperty &property);
    void addEllipse(const QPointF &pos, qreal radius, const QColor &color, int width);
    void clearEllipse(void);

    ItemPlot *clone(void); // 未实现，也未使用

    void setDeleteAll(bool isDeleteAll = true) { m_isDeleteAll = isDeleteAll; }

    // QuiKIS用////////////////////////////////
    void mergePlot(QList<PlotInfo> &plotInfos); // plotInfos中要合并的其它Plot，暂时只支持两个Plot合并
    QList<PlotInfo> unmergePlot(void);
    void setUnmergePlotInfo(const PlotInfo &plotInfo);
    PlotInfo getMergePlotInfo(void);
    bool canUnmergePlot(void) const;
    bool canMergePlot(void) const;

    void DeleteCurve(const QString &variablePath, const QString &curveId);

    void setDataAnalyseObserver(IDataAnalyseObserver *dataAnalyseObserver)
    {
        m_canvas->setDataAnalyseObserver(dataAnalyseObserver);
    }
    // void updateToolbarButtonState(void) { m_toolbar->setShowOtherPageActionCheckedState(true); }

    void modifyCurveName(const QString &variableFullPath, const QString &curveName);

    void refreshBackgroundColor(void) { QwtPlot::setCanvasBackground(QBrush(m_plotInfo.BackgroundColor)); }

    static QString generateUuid();

public:
    virtual void OnVariableUpdate(PIVariableBase trigger, PIVariableBase &thisVariable, const DataInfo &series,
                                  const int arrayIndex, const VariableType variableType, const NumberUnit &value);
    virtual void OnVariableDataListUpdate(PIVariableBase trigger, PIVariableBase &thisVariable, const DataInfo &series,
                                          IDataPackList *pDataList);
    virtual void notifySelecting(const AxisRanges &rect);
    virtual void notifySelected(const AxisRanges &rect);
    virtual void notifyMouseMove(QPointF &point);
    virtual void notifyResetCoordinate();
signals:
    void select(QWidget *w);
    void notifyCommand(int nCommand, QMap<QString, QVariant> paramMap);
    void modifyNotify(void);
    // void setMarkerInfo(const QList<MarkerInfo>& infos);
    void setCurveNames(const QStringList &curveNames);

    void setMarkerTableViewShow(bool isShow); // isShow为true时一直显示，为false是则根据原来状态置反

    void setAxisDecimal(int xDecimal, int yDecimal);
    void toolBarTriggerSignal(int);

protected:
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void hideEvent(QHideEvent *event);
protected slots:
    void onToolbarTriggered(int index);
    void timerEvent(QTimerEvent *event) override;
    void legendChecked(const QVariant &itemInfo, bool on);
    void onDataDictionaryServerNotify(unsigned int code, const NotifyStruct &cmdStruct);
    void onSettingsChanged(bool b);
    void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param);

private:
    void init(void);
    void onPlotSettingDialog(void);
    void queryDictionaryServerInterface(void);
    void removeVarible(void);

    void setXType(int xtype);
    bool isInitVarSucess();
    void resetWatcherPackageSize();

    void changeLineStyle(ItemPlotCurve *plotCurve, QColor &color, int w, int style);

    ItemPlot *getItemPlotByVarPath(const QString &varPath);
    void copyCurveDataFromSrc(const QMap<QString, ItemPlot *> src);

    void applyPlotInfo(void);
    void initCurve(PlotCurveInfo &info, QString &varibleName, PlotVaribleInfoType variableType, const QString &varAlias,
                   bool isClone = false);
    void uninitCurve(PlotCurveInfo &info);
    bool initVarbleInfo(PlotVaribleInfo &info);
    void uninitVarbleInfo(PlotVaribleInfo &info);

    void showCurve(QwtPlotItem *item, bool on);
    void initCurveStyle(QwtPlotItem *item);
    void setLegendPosition(QwtPlot::LegendPosition pos);

    void uninitAll(void);

    void showSettingDialog(const PlotInfo &plotInfo);

    inline bool addPoint(ItemPlotCurveSeriesData *curveData, double value, double timeStamp, double otherData = 0);

    void applyPlotInfoChanged(PlotInfo &plotInfo);

    void applyTitle(const PlotInfo &info);

    void emitAddVariableNotify(const QStringList &variablePaths);
    QString defaultCurveName(const QString &variableName, PlotVaribleInfoType variableType, int curveRow, int curveCol);

    QSizeF getXAxisRange(void);

    void refreshMarkerTableView();

    void calAxisRangeWhileAddPt();
    void refresh(void);

    void applyYRightAxisInfo(void) { }
    void commonRunStateHandle(NotifyStruct &notifyStruct);              // 通用的运行状态处理
    void mergeInfoHandleEmit(bool isMerge, QList<PlotInfo> &plotInfos); // 为true时为合并，为false时为拆分

    QwtText getAxisTitle(const QString &strTitle, const QColor &color);
    void plotInfoUpdate(); // 少部分图表信息刷新

    void cpoyPlotJustSettingInfo(PlotInfo &dst, const PlotInfo &src); // 只拷贝和曲线点无关的设置
private:
    QwtPlotGrid *m_plotGrid;
    // PlotToolbar *m_toolbar;
    PIDataDictionaryServer _dataDictionaryServer;
    CustomPlotSettingsDialog *m_settingDialog;
    ItemPlotScaleDraw *m_xScaleDraw;
    ItemPlotScaleDraw *m_yScaleDraw;
    ItemPlotScaleDraw *m_yRightScaleDraw;
    ItemPlotCanvas *m_canvas;
    QList<PlotEllipse *> m_ellipseList;
    ItemPlotLegend *mPlotLegend;

private:
    std::atomic_bool m_isNeedReplot;
    QReadWriteLock mPlotLock;
    int m_timerId;
    PlotInfo m_plotInfo;
    bool m_isAutoStop;
    QString m_projectName;
    PlotProject m_plotProject;
    QMap<QString, bool> mVarInitState;
    bool m_isDeleteAll;
    plotWatcher *mWatcher;
};

class ItemPlotGrid : public QwtPlotGrid
{
public:
    ItemPlotGrid() { }
    ~ItemPlotGrid() { }

    virtual void draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &rect) const;

private:
    void drawLines(QPainter *painter, const QRectF &canvasRect, Qt::Orientation orientation,
                   const QwtScaleMap &scaleMap, const QList<double> &values) const;

    void drawGridLines(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
                       const QRectF &canvasRect) const;
};

#endif // ITEMPLOTCURVE_H

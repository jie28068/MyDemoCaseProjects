#ifndef MONITORPANELPLOTSERVER_H
#define MONITORPANELPLOTSERVER_H

#include "CoreLib/ServerBase.h"
#include "MonitorPanel/ItemPlot.h"
#include "MonitorPanel/def.h"
#include "MonitorPanelServer/IMonitorPanelServer.h"
#include "blocking_queue.h"

using namespace Kcc;
KCC_USE_NAMESPACE_MONITORPANEL;

class MonitorPanel;

class SendVarDataThread : public QThread
{
public:
    SendVarDataThread(QObject *parent = nullptr);
    virtual ~SendVarDataThread();

public:
    void setVarList(const QStringList &allVar);

protected:
    void run();

private:
    BlockingQueue<QStringList> mVariableListQueue;
};

class MonitorPanelPlotServer : public ServerBase, public IMonitorPanelPlotServer
{
public:
    MonitorPanelPlotServer(void);
    ~MonitorPanelPlotServer(void);

    // IMonitorPanelPlotServer
    virtual int AddPlotCurve(int plotPos, const QString &dataDicElementFullPath);

    virtual QStringList GetPlotTitleList(void) const;
    virtual QStringList GetPlotIdList(void) const;
    virtual bool AddCurvesToPlot(const QStringList &variableFullPathList, const QString &plotTitle);
    virtual bool AddCurvesToPlot(const QStringList &variableFullPathList, int plotIndex);

    virtual bool AddCurvesToNewOnePlot(const QStringList &variableFullPathList, const QString &plotTitle);
    virtual bool AddCurvesToEachNewPlot(const QStringList &variableFullPathList, const QStringList &plotTitleList);

    virtual void ClearCurveDataFromAllPlot(const QStringList &variableFullPathList);

    virtual void SetDefaultXAxisLength(qreal length);
    /////////////////////////////////////////////////////////////////

    virtual bool AddCurvesToNewOnePlot(const PlotParam &plotParam, const QStringList &variableFullPathList);

    virtual bool AddCurvesToPlot(const PlotParam &plotParam, const QStringList &variableFullPathList,
                                 const QString &plotTitle);
    virtual void ClearCurvesFromPlot(const QString &plotTitle);
    virtual bool AddCurvesToPlot(const PlotParam &plotParam, const QStringList &variableFullPathList, int plotIndex);
    virtual void ClearCurvesFromPlot(int plotIndex);

    virtual PlotPointer AddNewPlot(const QString &plotTitle);
    virtual bool SetPlotParam(PlotPointer plot, const PlotParam &plotParam);
    virtual bool AddCurveToPlot(PlotPointer plot, const QString &variableFullPath);
    virtual bool ClearCurveDataFromPlot(PlotPointer plot, const QString &variableFullPath);

    virtual bool SetPlotParam(int plotIndex, const PlotParam &plotParam);
    virtual bool AddCurveToPlot(int plotIndex, const QString &variableFullPath);
    virtual bool ClearCurveDataFromPlot(int plotIndex, const QString &variableFullPath);

    virtual bool SetPlotParam(const QString &plotTitle, const PlotParam &plotParam);
    virtual bool AddCurveToPlot(const QString &plotTitle, const QString &variableFullPath);
    virtual bool ClearCurveDataFromPlot(const QString &plotTitle, const QString &variableFullPath);
    virtual bool clearWidgets();                              // 清空图表
    virtual bool removeWidgetFromTitle(const QString &title); // 通过图表名字去删除

    virtual bool setPlotProject(const PlotProject &plotPro);   // 设置本图表用于哪个项目
    virtual bool setPlotFunc(const PlotFunc &plotFunc);        // 设置图表哪些功能要，哪些功能不要
    virtual bool setCutType(const CutType &cutType);           // 设置裁剪类型
    virtual bool unmergePlotFromPlotId(const QString &plotId); // 通过图表id去拆分绘图
    virtual void setToolbarTriggered(int index);
    virtual void setLayoutChoosed(int rows, int cols); // 设置布局，几行几列
    virtual bool AddMarkerToPlot(PlotPointer plot, int markerType);
    virtual bool AddMarkerToPlot(int plotIndex, int markerType);
    virtual bool AddMarkerToPlot(const QString &plotTitle, int markerType);

    // panelName是当前区域名, plotTitle为图表名称，isEnable true为启用，false为禁用
    //	virtual bool setButtonEnable(const QString& plotTitle,RunButtonType buttonType,bool isEnable);
    virtual bool setButtonEnable(const QString &panelName, const QString &plotTitle, RunButtonType buttonType,
                                 bool isEnable);

    virtual void SetMonitorPanelName(const QString &panelName);

    virtual QString GetMonitorPanelName() const { return m_panelName; }

    virtual bool AddMarkerToSelectedPlots(int markerType);

    /************************************************************************/
    /* 添加最大值、最小值的游标  0:最大值；1：最小值           2022.11.1 add  */
    /************************************************************************/
    // virtual bool AddMaxValueOrMinValueMarkerToPlot(int type);

    virtual void SetPlotDefaultProperty(const PlotDefaultProperty &property);

    virtual void addEllipse(const QString &plotTitle, const QPointF &pos, qreal radius, const QColor &color, int width);
    virtual void clearEllipse(const QString &plotTitle);

    // 获取当前工作区的toolBar中X轴缩放、Y轴缩放、同步等Action的checked状态 2022.11.3 新增
    virtual void DeleteCurveFromPlot(const QString &plotTitle, const QString &variableFullPath, const QString &curveId);

    virtual PlotActionCheckedState GetPlotTooBarCheckedState();
    // 更新绘图菜单栏action的check状态		2022.11.3 新增
    virtual void setPlotMenuCheckedState();
    // 更新绘图菜单栏action的是否显示/可用状态
    virtual bool setPlotMenuVisibleState();

    // 提供图形导出功能(All Open WorkAreaWidget Graph)	2022.11.7 New
    virtual void exportAllPlotTo(QString workAreaName, QString dirName, ExportGraphType graphType);

    virtual void ShowDataAnalyseInfoPage(const QString &panelName, DataInfoType dit);

    virtual void ModifyCurveName(const QString &panelName, const QString &plotId, const QString &variableFullPath,
                                 const QString &curveName);

    virtual void rebindVar();

    virtual void removeBoardVar(const QStringList &paras);

    virtual bool refreshBindedVarbleInfo(bool isActive, const QString &boardName);

public:
    QList<ItemPlot *> getAllItemPlots();

    bool getSimulateState();

    void setSimulateState(bool b);

    void renameBoardVar(const QMap<QString, QString> &paras);

    void setTransferVar(const QStringList &varList);

private:
    bool isSimulating;
    MonitorPanel *getMonitorPanel(void) const;
    MonitorPanel *getMonitorPanel(const QString &panelName) const;
    QString m_panelName;
    SendVarDataThread *mTransfer;
};

#endif

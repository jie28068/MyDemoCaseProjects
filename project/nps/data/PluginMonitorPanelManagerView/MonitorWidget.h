#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include "CoreLib/ServerNotify.h"
#include "CoreUi/Common/BasicWorkareaContentWidget.h"
#include "PlotWidget.h"
#include "pluginmonitorpanelmanagerview_global.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <QFrame>
#include <QUndoStack>
#include <QWidget>

KCC_USE_NAMESPACE_MONITORPANEL

class ItemPlot;
class PlotScrollArea;
class ScopeViewToolbar;
class PlotLayoutChooseDialog;
struct PlotInfo;
class QDomElement;

class DataDictionaryDialog;
class MonitorWidget;

class PlotUndoCommond : public QUndoCommand
{
public:
    PlotUndoCommond(const QString &text, QUndoCommand *parent);
    virtual ~PlotUndoCommond();

public:
    enum class actionType { AddPlot, DelPlot, CopyPlot, CutPlot, PastePlot, NoDefine };

public:
    void setArea(MonitorWidget *pa);

    void setType(actionType actype);
    actionType getType();

    void addPlotInfo(const PlotInfo &info);
    QList<PlotInfo> &getPlotInfoVec();

public:
    virtual void undo();
    virtual void redo();

private:
    void undoAddPlot();
    void redoAddPlot();

    void undoDelPlot();
    void redoDelPlot();

    void undoCopyPlot();
    void redoCopyPlot();

    void undoCutPlot();
    void redoCutPlot();

    void undoPastePlot();
    void redoPastePlot();

public:
    void copyCurveData();
    void copyOne(PlotInfo &info);
    void setIsCut(bool b) { mIsCut = b; }

private:
    QStringList getAllTitle();
    void renameTitle(const QStringList &allHasTitle, QString &needRename);
    void saveLayOut();
    void initLayOut();

private:
    actionType mAction;
    QList<PlotInfo> mSource;
    MonitorWidget *mWidget;
    bool mIsCut = false;

    QPair<int, int> mBefore_RowAndCol;
    int mBeforeRealRow = 0;
    QMap<QString, QPair<int, int>> mBefore_PlotPos;
};

class PLUGINMONITORPANELMANAGERVIEW_EXPORT MonitorWidget : public QFrame
{
    Q_OBJECT

public:
    MonitorWidget(QWidget *parent);
    ~MonitorWidget();
    QList<QWidget *> plotScrollAreaWidgets();
    QList<QWidget *> selectedWidgets() const;
    ItemPlot *addItemPlot(const QString &strTitle);
    bool clearWidgets(void);
    bool removeWidgetFromTitle(const QString &title); // 通过图表名字去删除

    bool setPlotProject(const PlotProject &plotPro);
    void setToolbarTriggered(int index);

    QList<PlotInfo> getPlotInfos(void) const;
    void setPlotInfos(QList<PlotInfo> &infoList);

    void changeLayout(int rows, int cols);
    void updateLayout();
    int layoutRows(void) const;
    int layoutCols(void) const;

    void save(const QString &strPath);
    void load(const QString &strPath);

    bool setMonitorData(const QDomElement &element);
    bool getMonitorData(QDomElement &element);

    void setCurrentProjectName(const QString &projectName);
    void setAcceptNotify(bool b) { m_isAcceptNotify = b; }
    ScopeViewToolbar *getScopeViewToolbar() { return m_Toolbar; }
    PlotScrollArea *getScrollArea() { return m_ScrollArea; } // 2022.11.15

    void initIsRunning(void);

    bool isBinging(void) const;
    void clearPropertyBingToVariable();

    void setShowBorder(bool b);
    QString getPrjName() { return m_projectName; }

    QPair<bool, QList<PlotInfo>> &getLastedList();
    QList<QPair<bool, QList<PlotInfo>>> &getStashList() { return mCopyOrCutInfos; }
    void addInfoList(const QPair<bool, QList<PlotInfo>> &infos);
    void DelLatestOneInfo();

protected:
    virtual void resizeEvent(QResizeEvent *event);

public slots:
    void onStart(void);
    void onStop(void);
    void onLayoutChoosed(int rows, int cols);
#pragma region 设置工具栏按钮状态
    void setToolBarCopyCutDelBtnState();
    void setToolBarPasteBtnState();
    void setUndobtnEnable(bool can);
    void setRedobtnEnable(bool can);
    void onDragPloting();
    void onDragPlotFinsh();
    void onSimulatingStopUpdateBtnState();
    void onSimulatingUpdateBtnState();
#pragma endregion

signals:
    void modifyNotify(void);
protected slots:
    void onAddItemPlot(void);
    void onRemoveItemPlot(void);
    void onCopyItemPlot(bool iscut = false);
    void onCutItemPlot(void);
    void onPasteItemPlot(void);
    void onShowSelected(void);
    void onShowAll(void);
    void onSync(void);
    void onAutoStop(void);
    void onToolbarTriggered(int nIndex);
    void onPlotLayoutChooseDialogFinished(int result);
    void onUndo();
    void onRedo();

    void onAllSelect(void);
    void onMergePlot(void);
    void onUnmergePlot(void);

    void onDataDictionaryDialog(void);
    // notify
    void onDataDictionaryServerNotify(unsigned int code, const NotifyStruct &notifyStruct);
    void onDataDictionaryDialogClosed(void);
    void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param);

private:
    bool mCanZoomX = false;
    bool mCanZoomY = false;
    bool mCanSyc = false;
    bool mCanMousePt = false;

private:
    PlotScrollArea *m_ScrollArea;
    ScopeViewToolbar *m_Toolbar;
    // true是拷贝的，false是剪切
    QList<QPair<bool, QList<PlotInfo>>> mCopyOrCutInfos;
    PlotLayoutChooseDialog *m_plotLayoutChooseDialog;

    QString m_projectName;
    QString mExportPicPath;
    bool m_isAcceptNotify; // 是否接收数据字典服务通知处理

    PlotProject m_plotPro;

    // bool m_isExternTrigger; // 是否是外部接口触发而不是绘图插件鼠标点击,true为外部触发,false为绘图插件鼠标点击

    DataDictionaryDialog *m_dataDictionaryDialog;
    QWidget *mPlaceHolder;
    QWidget *mDrawSeprator;
    QUndoStack *mUndoStack;
};

#endif // MONITORWIDGET_H

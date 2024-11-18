#ifndef SCOPEVIEWTOOLBAR_H
#define SCOPEVIEWTOOLBAR_H

#include "def.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include <QSignalMapper>
#include <QToolBar>
#include <QToolButton>

KCC_USE_NAMESPACE_MONITORPANEL

class ScopeViewToolbar : public QToolBar
{
    Q_OBJECT

public:
    ScopeViewToolbar(QWidget *parent = nullptr);
    virtual ~ScopeViewToolbar();

public:
    enum class ActionName {
        add,
        undo,
        redo,
        copy,
        cut,
        paste,
        del,
        allSelect,
        showSelect,
        showAll,
        resetAxis,
        fullx,
        fully,
        zoomx,
        zoomy,
        layout,
        syc,
        mousePt,
        varTable,
        screenShot
    };

public:
    QPoint getPlotLayoutChooseDialogShowPos(void);
    bool setPlotFunc(const PlotFunc &plotFunc);
    void switchActionChecked(int index); // 根据序号切换不同action的checked状态

    PlotActionCheckedState GetPlotTooBarCheckedState();
    bool setPlotMenuVisibleState();
    void uiActionUpdate(bool isMergeExist); // 将界面上的action也更新
    void setPlotNum(int num);

signals:
    void triggered(int index);

public:
    void setActionState(ActionName actName, bool isEnable);
    void setActionCheckState(ActionName actName, bool can);

protected slots:
    void onStartActionTriggered();
    void onStopActionTriggered();

private:
    QLabel *mDisplayNum;

    QAction *mUndoAction;
    QAction *mRedoAction;

    QAction *m_addAction;
    QAction *m_delAction;
    QAction *m_copyAction;
    QAction *m_cutAction;
    QAction *m_pasteAction;

    QAction *m_showSelectedAction;
    QAction *m_showAllAction;

    // QAction *m_mergeAction;
    // QAction *m_breakAction;

    // QToolButton *m_selectRangeToolButton;
    // QToolButton *m_selectPointToolButton;

    QAction *m_syncCheckboxAction;

    QAction *m_autoStopAction;
    QAction *m_startAction;
    QAction *m_stopAction;

    QAction *m_zoomInXAxisAction;
    QAction *m_zoomInYAxisAction;

    QAction *m_layoutRow_1;
    QAction *m_layoutRow_2;
    QAction *m_layoutRow_3;
    QAction *m_layoutCol_1;
    QAction *m_layoutCol_2;
    QAction *m_layoutCol_3;

    QAction *m_resetAxisAction;

    QAction *m_xAxisFullAction;
    QAction *m_yAxisFullAction;

    QAction *m_exportAction;

    QAction *m_mousePointAction;

    QAction *m_allSelectAction;

    QAction *m_dataDictionaryAction;

    QAction *m_mergeSeparator;
    QAction *m_mergeAction;
    QAction *m_unmergeAction;

    QSignalMapper *m_signalMapper;
};

#endif // SCOPEVIEWTOOLBAR_H

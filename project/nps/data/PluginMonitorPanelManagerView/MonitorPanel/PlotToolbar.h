#ifndef PLOTTOOLBAR_H
#define PLOTTOOLBAR_H

#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include <QAction>
#include <QSignalMapper>
#include <QToolBar>

KCC_USE_NAMESPACE_MONITORPANEL

class PlotToolbar : public QToolBar
{
    Q_OBJECT

public:
    PlotToolbar(QWidget *parent);
    virtual ~PlotToolbar();

    bool setButtonEnale(RunButtonType buttonType, bool isEnable);
    void setMode(int mode)
    {
        if (mode == 0) {
            m_settingAction->setVisible(true);
            if (m_startAction)
                m_startAction->setVisible(true);
            if (m_stopAction)
                m_stopAction->setVisible(true);
            if (m_pauseAction)
                m_pauseAction->setVisible(true);
        } else {
            m_settingAction->setVisible(false);
            if (m_startAction)
                m_startAction->setVisible(false);
            if (m_stopAction)
                m_stopAction->setVisible(false);
            if (m_pauseAction)
                m_pauseAction->setVisible(false);
        }
    }

    void setShowOtherPageActionCheckedState(bool bChecked) { m_showCorsorTableAction->setChecked(bChecked); }

protected slots:
    void onStartTriggered(void);
    void onStopTriggered(void);
    void onPauseTriggered(void);

signals:
    void triggered(int index);

private:
    QSignalMapper *m_signalMapper;

    QAction *m_settingAction;
    QAction *m_removeAction;
    QAction *m_dataAnalyseAction;
    QAction *m_showCorsorTableAction; // 由之前的游标页面是否显示改为为附加页面是否显示之用

    QAction *m_startAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
};

#endif // PLOTTOOLBAR_H

#include "PlotToolbar.h"
#include "CoreLib/GlobalConfigs.h"
#include "def.h"

KCC_USE_NAMESPACE_MONITORPANEL

PlotToolbar::PlotToolbar(QWidget *parent)
    : QToolBar(parent), m_signalMapper(nullptr), m_startAction(nullptr), m_stopAction(nullptr), m_pauseAction(nullptr)
{
    // setWindowFlags(Qt::WindowStaysOnTopHint);
    setFixedHeight(24);
    setFixedWidth(62);
    // setIconSize(QSize(16,16));
    // setStyleSheet("background-color: rgb(255, 255, 255, 200);");
    // this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_signalMapper = new QSignalMapper(this);

    m_settingAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    m_settingAction->setToolTip(tr("Settings")); // 设置

    // m_settingAction->setIcon(QIcon(":/image/设置.png"));
    m_signalMapper->setMapping(m_settingAction, 0);
    QToolButton *setBtn = (QToolButton *)this->widgetForAction(m_settingAction);
    setBtn->setObjectName("set");
    setBtn->setFixedSize(16, 16);

    // addSeparator();

    m_removeAction = addAction(/*QIcon(":/Resources/png/test.png"),*/ tr(""), m_signalMapper, SLOT(map()));
    m_removeAction->setToolTip(tr("Clear All Curves")); // 清空所有曲线

    // m_removeAction->setIcon(QIcon(":/image/清空.png"));
    m_signalMapper->setMapping(m_removeAction, 1);
    QToolButton *clearAllCurveBtn = (QToolButton *)this->widgetForAction(m_removeAction);
    clearAllCurveBtn->setObjectName("removeallcurve");
    clearAllCurveBtn->setFixedSize(16, 16);

    QVariant var = GlobalConfigs::getInstance()->get("PlotProject");
    PlotProject plotPro = Default;
    if (var.isValid()) {
        int plotProI = var.toInt();
        plotPro = (PlotProject)plotProI;
        if (plotPro == QuiKIS)
            m_removeAction->setVisible(false);
        else
            addSeparator();
    }

    m_dataAnalyseAction = addAction(tr(""), m_signalMapper, SLOT(map()));
    m_dataAnalyseAction->setToolTip(tr("Display Analysis Data")); // 显示分析数据

    // m_dataAnalyseAction->setIcon(QIcon(":/image/统计数据.png"));
    m_signalMapper->setMapping(m_dataAnalyseAction, 2);
    QToolButton *showAnalasisInfoBtn = (QToolButton *)this->widgetForAction(m_dataAnalyseAction);
    showAnalasisInfoBtn->setObjectName("showanalasisinfo");
    showAnalasisInfoBtn->setFixedSize(16, 16);

    // 判断后面要改下，改成按不同项目来区分，临时先这么搞吧,
    if (gConfGet(IsShowCorsorTableView).toBool()) {
        m_showCorsorTableAction = addAction(tr(""), m_signalMapper, SLOT(map()));
        m_showCorsorTableAction->setCheckable(true);
        m_showCorsorTableAction->setToolTip(tr("Display cursor settings window")); // 是否显示游标设置窗
        m_showCorsorTableAction->setIcon(QIcon(":/image/游标.png"));
        m_signalMapper->setMapping(m_showCorsorTableAction, 3);

        addSeparator();

        m_startAction = addAction(tr(""), m_signalMapper, SLOT(map()));
        m_startAction->setToolTip(tr("Start")); // 开始
        m_startAction->setIcon(QIcon(":/image/开始.png"));
        m_signalMapper->setMapping(m_startAction, 4);
        connect(m_startAction, SIGNAL(triggered(bool)), this, SLOT(onStartTriggered()));

        m_pauseAction = addAction(tr(""), m_signalMapper, SLOT(map()));
        m_pauseAction->setToolTip(tr("Pause")); // 暂停
        m_pauseAction->setIcon(QIcon(":/image/暂停.png"));
        m_signalMapper->setMapping(m_pauseAction, 5);
        connect(m_pauseAction, SIGNAL(triggered(bool)), this, SLOT(onPauseTriggered()));
        m_pauseAction->setEnabled(false);

        m_stopAction = addAction(tr(""), m_signalMapper, SLOT(map()));
        m_stopAction->setToolTip(tr("Stop")); // 停止
        m_stopAction->setIcon(QIcon(":/image/停止.png"));
        m_signalMapper->setMapping(m_stopAction, 6);
        connect(m_stopAction, SIGNAL(triggered(bool)), this, SLOT(onStopTriggered()));
        m_stopAction->setEnabled(false);
    }

    connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(triggered(int)));
    this->setIconSize(QSize(16, 16));
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    raise();
}

PlotToolbar::~PlotToolbar() { }

bool PlotToolbar::setButtonEnale(RunButtonType buttonType, bool isEnable)
{
    switch (buttonType) {
    case StartButton:
        m_startAction->setEnabled(isEnable);
        break;
    case PauseButton:
        m_pauseAction->setEnabled(isEnable);
        break;
    case StopButton:
        m_stopAction->setEnabled(isEnable);
        break;
    default:
        return false;
    }
    return true;
}

void PlotToolbar::onStartTriggered(void)
{
    if ((PlotProject)gConfGet("PlotProject").toInt() != QuiKIS) //???QuiKIS????????
    {
        m_startAction->setEnabled(false);
        m_pauseAction->setEnabled(true);
        m_stopAction->setEnabled(true);
    }
}

void PlotToolbar::onStopTriggered(void)
{
    if ((PlotProject)gConfGet("PlotProject").toInt() != QuiKIS) //???QuiKIS????????
    {
        m_startAction->setEnabled(true);
        m_pauseAction->setEnabled(false);
        m_stopAction->setEnabled(false);
    }
}

void PlotToolbar::onPauseTriggered(void)
{
    if ((PlotProject)gConfGet("PlotProject").toInt() != QuiKIS) //???QuiKIS????????
    {
        m_startAction->setEnabled(true);
        m_pauseAction->setEnabled(false);
        m_stopAction->setEnabled(true);
    }
}

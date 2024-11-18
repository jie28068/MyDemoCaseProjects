#ifndef MONITORPANEL_H
#define MONITORPANEL_H

#include "../MonitorWidget.h"
#include "MonitorPanel/ItemPlot.h"
#include "MonitorPanel/PlotScrollArea.h"
#include "MonitorPanel/def.h"
#include <QWidget>

class MonitorPanel : public BasicWorkareaContentWidget
{
    Q_OBJECT
    DEFINE_WORKAREA
public:
    MonitorPanel();
    ~MonitorPanel();

    virtual bool IsReadOnly() { return false; }
    virtual bool init(const QMap<QString, QVariant> &paramMap);
    void rebindVar();
    virtual QIcon icon();
    virtual void Save();

    QList<QWidget *> getItemPlots();
    QList<QWidget *> getPlotWidgets();

    QList<ItemPlot *> getSelectedPlots(void) const;
    QList<QWidget *> getSelectedWidgets(void) const;
    QList<ItemPlot *> getAllPlots() const;

    void showDataAnalyseInfoPage(DataInfoType dit);

    void modifyCurveName(const QString &plotId, const QString &variableFullPath, const QString &curveName);

    ItemPlot *addItemPlot(const QString &strTitle) { return m_monitorWidget->addItemPlot(strTitle); }
    bool removeWidgetFromTitle(const QString &title) { return m_monitorWidget->removeWidgetFromTitle(title); }
    bool clearWidgets(void) { return m_monitorWidget->clearWidgets(); }
    bool setPlotProject(const PlotProject &plotProject);
    bool setPlotFunc(const PlotFunc &plotFunc); //
    // index 5:显示选中, 6:显示所有, 7:同步操作, 11:X轴缩放, 12:Y轴缩放, 13:布局, 19: 初始坐标, 20:X轴撑满, 21:Y轴撑满,
    // 22:导出, 23:鼠标取点
    void setToolbarTriggered(int index);
    void setLayoutChoosed(int rows, int cols); // 设置布局，几行几列

    bool setPlotMenuVisibleState();
    void refreshBackgroundColor(void);

    PlotActionCheckedState GetPlotTooBarCheckedState();

    QWidget *getScrollAreaWidget(); // 2022.11.15
protected slots:
    void save();
    void onFullScreen(bool b);

private:
    void load();

private:
    QMap<QString, QVariant> m_paramMap;
    MonitorWidget *m_monitorWidget;
};

#endif // MONITORPANEL_H

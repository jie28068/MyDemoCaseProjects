#ifndef CUSTOMPLOTSETTINGSDIALOG_H
#define CUSTOMPLOTSETTINGSDIALOG_H

#include "KLWidgets/KCustomDialog.h"
// #include "PlotSettingsDialog.h"

#include "ui_PlotSettingsDialog.h"
#include <QDialog>
// #include "CurveSettingTableModel.h"
#include "CurveSettingTreeModel.h"
#include "server/DataDictionary/IDataDictionaryDDXServer.h"
#include "server/DataDictionary/IDataDictionaryServer.h"
#include "server/DataDictionary/IDataDictionaryViewServer.h"

////////////////////////////////////////////////////////////////////////////
class PlotSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    PlotSettingsDialog(QWidget *parent = 0);
    ~PlotSettingsDialog();

    void setPlotInfo(const PlotInfo &plotInfo);
    PlotInfo getPlotInfo(void);

    // signals:
    //	void changed(PlotInfo info);
    void confirm();
    bool getIsModifyDecimal() { return m_isModifyDecimal; }
    bool getIsModifySet() { return m_isModifySet; }
    virtual void reject() { }
    void refreshNode();

protected:
    void initUI(void);

protected slots:
    /*  void onOK(void);
      void onCancel(void);*/
    void onDelete(void);
    void onClear(void);
    void onSetColorBtnClick();

    void onAutoYAxisChecked(bool b);

    void onShowTitleChecked(bool b);
    void onShowSubTitleChecked(bool b);
    void onShowXTitleChecked(bool b);
    void onShowYTitleChecked(bool b);
    void onXTypeChanged(int index);

    // 用于限制X轴、Y轴的最大值不能小于最小值、最小值不能大于最大值
    void onXMinValueEditingFinished();
    void onXMaxValueEditingFinished();
    void onYMinValueEditingFinished();
    void onYMaxValueEditingFinished();

private:
    bool comparePlotInfo(const PlotInfo &plotInfo1, const PlotInfo &plotInfo2); //??????????
    QWidget *createDataDictionaryView(void);

    void setTreeExpandState(QTreeView *varTree);

    void FillYscaleCombo(QComboBox *b);

private:
    Ui::PlotSettingsDialog ui;
    CurveSettingTreeModel *m_model;
    PlotInfo m_plotInfo;
    PIElementTreeViewController m_dataDictionaryViewController;

    bool m_isModifyDecimal; //????o悫1y??表殍??DT??1y????

    bool m_isModifySet;
};

class CustomPlotSettingsDialog : public KCustomDialog
{
    Q_OBJECT
public:
    CustomPlotSettingsDialog(QWidget *p = nullptr);
    ~CustomPlotSettingsDialog();

    void setPlotInfo(const PlotInfo &plotInfo);
    PlotInfo getPlotInfo(void);

    void setNodeState() { m_pPlotSettingsDialog->refreshNode(); }

    void confirm();

    bool getIsModifyDecimal() { return m_pPlotSettingsDialog->getIsModifyDecimal(); } //????o悫1y??表殍??DT??1y????

    bool getIsModifySet() { return m_pPlotSettingsDialog->getIsModifySet(); }

    static bool isShow(void) { return m_isShow; }
signals:
    void changed(bool b);
protected slots:
    void onFinished(int result);

private:
    static bool m_isShow;

    PlotSettingsDialog *m_pPlotSettingsDialog;
};

#endif

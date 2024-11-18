#include "CustomPlotSettingsDialog.h"

#include "CoreLib/GlobalConfigs.h"
#include "CoreLib/ServerBase.h"
#include "CoreLib/ServerManager.h"
#include "KLWidgets/KColorDialog.h"
#include "MonitorPanelPlotServer.h"
#include "server/MonitorPanelServer/IMonitorPanelServer.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <QListView>

KCC_USE_NAMESPACE_MONITORPANEL
KCC_USE_NAMESPACE_COREMANUI
USE_LOGOUT_("PlotSettingsDialog")

bool CustomPlotSettingsDialog::m_isShow = false;

PlotSettingsDialog::PlotSettingsDialog(QWidget *parent)
    : QDialog(parent), m_model(nullptr), m_isModifyDecimal(false), m_isModifySet(false)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setModal(false);
    this->setStyleSheet(R"(PlotSettingsDialog{border: 0px solid;})");
    initUI();
}

PlotSettingsDialog::~PlotSettingsDialog() { }

void PlotSettingsDialog::setPlotInfo(const PlotInfo &plotInfo)
{
    m_plotInfo = plotInfo;

    //////////////////
    // if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
    //     m_plotInfo.XType = 2;
    //     ui.comboBox_XType->hide();
    //     ui.label_XType->hide();
    // } else {
    if (m_plotInfo.XType > 1)
        m_plotInfo.XType = 1;
    ui.comboBox_XType->removeItem(2);
    //}
    ///////////////////////////////////

    // 设置X、Y轴范围输入范围
    static int ret = 0;
    ret++;
    if (1 == ret) {
        ui.doubleSpinBox_XRangeMin->setRange(m_plotInfo.XMin, m_plotInfo.XMax);
        ui.doubleSpinBox_XRangeMax->setRange(m_plotInfo.XMin, m_plotInfo.XMax);
        // ui.doubleSpinBox_YRangeMin->setRange(m_plotInfo.YMin, m_plotInfo.YMax);
        // ui.doubleSpinBox_YRangeMax->setRange(m_plotInfo.YMin, m_plotInfo.YMax);
    }

    ui.comboBox_XType->setCurrentIndex(m_plotInfo.XType);
    ui.doubleSpinBox_XRangeMin->setValue(m_plotInfo.XMin);
    ui.doubleSpinBox_XRangeMax->setValue(m_plotInfo.XMax);
    ui.lineEdit_Title->setText(m_plotInfo.Title);
    ui.doubleSpinBox_XRange->setValue(m_plotInfo.XRange);
    // ui.pushButton_BackColor->setColor(m_plotInfo.BackgroundColor);
    ui.bgcolor_edit->setText(m_plotInfo.BackgroundColor.name());
    ui.comboBox_LegendSide->setCurrentIndex(m_plotInfo.LegendSide);
    ui.comboBox_GridType->setCurrentIndex(m_plotInfo.GridType);

    ui.doubleSpinBox_YRangeMin->setValue(m_plotInfo.YMin);
    ui.doubleSpinBox_YRangeMax->setValue(m_plotInfo.YMax);

    ui.checkBox_AutoYAxis->setChecked(m_plotInfo.IsAutoYAxis);
    if (m_plotInfo.IsAutoYAxis) {
        ui.doubleSpinBox_YRangeMin->setEnabled(false);
        ui.doubleSpinBox_YRangeMax->setEnabled(false);
    } else {
        ui.doubleSpinBox_YRangeMin->setEnabled(true);
        ui.doubleSpinBox_YRangeMax->setEnabled(true);
    }

    // if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
    //     ui.checkBox_AutoYAxis->setChecked(false);
    //     ui.checkBox_AutoYAxis->hide();
    //     ui.doubleSpinBox_YRangeMin->setEnabled(true);
    //     ui.doubleSpinBox_YRangeMax->setEnabled(true);
    // }

    ui.checkBox_ShowTitle->setChecked(m_plotInfo.IsShowTitle);

    ui.checkBox_ShowSubTitle->setChecked(m_plotInfo.IsShowSubTitle);
    ui.lineEdit_SubTitle->setText(m_plotInfo.SubTitle);

    ui.checkBox_ShowXTitle->setChecked(m_plotInfo.IsShowXTitle);
    ui.lineEdit_XTitle->setText(m_plotInfo.XTitle);

    ui.checkBox_ShowYTitle->setChecked(m_plotInfo.IsShowYTitle);
    ui.lineEdit_YTitle->setText(m_plotInfo.YTitle);

    ui.comboBox_SaveDataMode->setCurrentIndex(m_plotInfo.SaveDataMode);

    onShowSubTitleChecked(m_plotInfo.IsShowSubTitle);
    onShowTitleChecked(m_plotInfo.IsShowTitle);
    onShowXTitleChecked(m_plotInfo.IsShowXTitle);
    onShowYTitleChecked(m_plotInfo.IsShowYTitle);
    onXTypeChanged(m_plotInfo.XType);

    m_model->setPlotVaribleInfoList(m_plotInfo.VaribleInfoList);
    ui.treeView->expandAll();

    ui.comboBox_XDecimal->setCurrentIndex(m_plotInfo.xDecimal + 1);
    ui.comboBox_YDecimal->setCurrentIndex(m_plotInfo.yDecimal + 1);

    ui.checkBox_xBottomLog->setVisible(m_plotInfo.isLogVisible);
    if (m_plotInfo.isLogVisible)
        ui.checkBox_xBottomLog->setChecked(m_plotInfo.xBottomIsLog);

    ui.IsShowYscaleNum->setChecked(m_plotInfo.isShowYsCaleNum);
    if (m_plotInfo.yScaleNum == 1000) {
        ui.YscaleCombo->setCurrentIndex(0);
    } else if (m_plotInfo.yScaleNum == 100) {
        ui.YscaleCombo->setCurrentIndex(1);
    } else if (m_plotInfo.yScaleNum == 10) {
        ui.YscaleCombo->setCurrentIndex(2);
    } else if (m_plotInfo.yScaleNum == 1) {
        ui.YscaleCombo->setCurrentIndex(3);
    } else if (m_plotInfo.yScaleNum == 0.1) {
        ui.YscaleCombo->setCurrentIndex(4);
    } else if (m_plotInfo.yScaleNum == 0.01) {
        ui.YscaleCombo->setCurrentIndex(5);
    } else if (m_plotInfo.yScaleNum == 0.001) {
        ui.YscaleCombo->setCurrentIndex(6);
    } else {
        ui.YscaleCombo->setCurrentIndex(3);
    }
}

PlotInfo PlotSettingsDialog::getPlotInfo(void)
{
    confirm();
    return m_plotInfo;
}

void PlotSettingsDialog::confirm()
{
    PlotInfo plotInfo;
    plotInfo = m_plotInfo;

    m_plotInfo.XType = ui.comboBox_XType->currentIndex();
    m_plotInfo.Title = ui.lineEdit_Title->text();
    m_plotInfo.XRange = ui.doubleSpinBox_XRange->value();
    // m_plotInfo.BackgroundColor = ui.pushButton_BackColor->getColor();
    if (!ui.bgcolor_edit->text().isEmpty()) {
        m_plotInfo.BackgroundColor = QColor(ui.bgcolor_edit->text());
    }
    m_plotInfo.LegendSide = ui.comboBox_LegendSide->currentIndex();
    m_plotInfo.GridType = ui.comboBox_GridType->currentIndex();

    m_plotInfo.IsAutoYAxis = ui.checkBox_AutoYAxis->isChecked();
    m_plotInfo.YMin = ui.doubleSpinBox_YRangeMin->value();
    m_plotInfo.YMax = ui.doubleSpinBox_YRangeMax->value();

    m_plotInfo.IsShowTitle = ui.checkBox_ShowTitle->isChecked();

    m_plotInfo.IsShowSubTitle = ui.checkBox_ShowSubTitle->isChecked();
    m_plotInfo.SubTitle = ui.lineEdit_SubTitle->text();

    m_plotInfo.IsShowXTitle = ui.checkBox_ShowXTitle->isChecked();
    m_plotInfo.XTitle = ui.lineEdit_XTitle->text();

    m_plotInfo.IsShowYTitle = ui.checkBox_ShowYTitle->isChecked();
    m_plotInfo.YTitle = ui.lineEdit_YTitle->text();

    m_plotInfo.XMin = ui.doubleSpinBox_XRangeMin->value();
    m_plotInfo.XMax = ui.doubleSpinBox_XRangeMax->value();

    m_plotInfo.SaveDataMode = ui.comboBox_SaveDataMode->currentIndex();

    m_plotInfo.isShowYsCaleNum = ui.IsShowYscaleNum->isChecked();
    switch (ui.YscaleCombo->currentIndex()) {
    case 0:
        m_plotInfo.yScaleNum = 1000;
        break;
    case 1:
        m_plotInfo.yScaleNum = 100;
        break;
    case 2:
        m_plotInfo.yScaleNum = 10;
        break;
    case 3:
        m_plotInfo.yScaleNum = 1;
        break;
    case 4:
        m_plotInfo.yScaleNum = 0.1;
        break;
    case 5:
        m_plotInfo.yScaleNum = 0.01;
        break;
    case 6:
        m_plotInfo.yScaleNum = 0.001;
        break;
    default:
        m_plotInfo.yScaleNum = 1;
        break;
    }

    // if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
    //     if (m_plotInfo.xDecimal != (ui.comboBox_XDecimal->currentIndex() - 1)
    //         || m_plotInfo.yDecimal != (ui.comboBox_YDecimal->currentIndex() - 1))
    //         m_isModifyDecimal = true;
    // }

    m_plotInfo.xDecimal = ui.comboBox_XDecimal->currentIndex() - 1;
    m_plotInfo.yDecimal = ui.comboBox_YDecimal->currentIndex() - 1;

    // if(m_plotInfo.VaribleInfoList.size()>0);
    // m_plotInfo.VaribleInfoList[0].CurveInfoList=m_model->getCurveInfo();
    m_plotInfo.VaribleInfoList = m_model->getPlotVaribleInfoList();
    m_plotInfo.xBottomIsLog = ui.checkBox_xBottomLog->isChecked();

    // m_isModifySet = comparePlotInfo(plotInfo, m_plotInfo);
}

void PlotSettingsDialog::refreshNode()
{
    QList<QTreeView *> allTree = ui.splitter->widget(0)->findChildren<QTreeView *>();
    if (allTree.isEmpty() || !allTree[0])
        return;
    setTreeExpandState(allTree[0]);
}

bool PlotSettingsDialog::comparePlotInfo(const PlotInfo &plotInfo1, const PlotInfo &plotInfo2)
{
    double min_value = 0.0001;
    if (plotInfo1.XType != plotInfo2.XType)
        return true;
    if (plotInfo1.Title != plotInfo2.Title)
        return true;
    if (fabs(plotInfo1.XRange - plotInfo2.XRange) > min_value)
        return true;
    if (plotInfo1.BackgroundColor != plotInfo2.BackgroundColor)
        return true;
    if (plotInfo1.LegendSide != plotInfo2.LegendSide)
        return true;
    if (plotInfo1.GridType != plotInfo2.GridType)
        return true;
    // if(plotInfo1.IsAutoYAxis != plotInfo2.IsAutoYAxis)
    //	return true;
    if (fabs(plotInfo1.YMin - plotInfo2.YMin) > min_value)
        return true;
    if (fabs(plotInfo1.YMax - plotInfo2.YMax) > min_value)
        return true;
    if (plotInfo1.IsShowTitle != plotInfo2.IsShowTitle)
        return true;
    if (plotInfo1.IsShowSubTitle != plotInfo2.IsShowSubTitle)
        return true;
    if (plotInfo1.SubTitle != plotInfo2.SubTitle)
        return true;
    if (plotInfo1.IsShowXTitle != plotInfo2.IsShowXTitle)
        return true;
    if (plotInfo1.XTitle != plotInfo2.XTitle)
        return true;
    if (plotInfo1.IsShowYTitle != plotInfo2.IsShowYTitle)
        return true;
    if (plotInfo1.YTitle != plotInfo2.YTitle)
        return true;
    if (fabs(plotInfo1.XMin - plotInfo2.XMin) > min_value)
        return true;
    if (fabs(plotInfo1.XMax - plotInfo2.XMax) > min_value)
        return true;
    if (plotInfo1.SaveDataMode != plotInfo2.SaveDataMode)
        return true;
    if (plotInfo1.xDecimal != plotInfo2.xDecimal)
        return true;
    if (plotInfo1.yDecimal != plotInfo2.yDecimal)
        return true;
    if (plotInfo1.xBottomIsLog != plotInfo2.xBottomIsLog)
        return true;
    return false;
}

void PlotSettingsDialog::initUI(void)
{
    ui.splitter->setChildrenCollapsible(false);
    ui.splitter->setStretchFactor(0, 2);
    ui.splitter->setStretchFactor(1, 8);
    ui.bgcolor_edit->setValidator(new QRegExpValidator(QRegExp("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$"), ui.bgcolor_edit));
    ui.lineEdit_Title->setMaxLength(32);
    ui.lineEdit_Title->setPlaceholderText(tr("Up to 32 characters can be entered"));
    ui.lineEdit_XTitle->setMaxLength(32);
    ui.lineEdit_XTitle->setPlaceholderText(tr("Up to 32 characters can be entered"));
    ui.lineEdit_YTitle->setMaxLength(8);
    ui.lineEdit_YTitle->setPlaceholderText(tr("Up to 8 characters can be entered"));

    FillYscaleCombo(ui.YscaleCombo);
    ui.checkBox_ShowSubTitle->setVisible(false);
    ui.label_8->setVisible(false);
    ui.lineEdit_SubTitle->setVisible(false); // 副标签暂时不需要，隐藏掉
    ui.comboBox_LegendSide->setVisible(false);
    ui.label_5->setVisible(false);
    ui.YscaleCombo->setCurrentIndex(3);
    ui.label_3->setVisible(false);
    ui.comboBox_SaveDataMode->setVisible(false);
    ui.comboBox_XType->setView(new QListView());
    ui.comboBox_XDecimal->setView(new QListView());
    ui.YscaleCombo->setView(new QListView());
    ui.comboBox_YDecimal->setView(new QListView());
    ui.comboBox_GridType->setView(new QListView());

    // int curveTreeViewType = gConfGet(CurveTreeViewType).toInt();
    m_model = new CurveSettingTreeModel(1, ui.treeView);
    ui.treeView->setItemDelegateForColumn(2, new ColorLabelDelegate(ui.treeView));
    // if((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS)
    ui.treeView->setItemDelegateForColumn(3, new LineWidthItemDelegate(ui.treeView)); // 线宽
    ui.treeView->setItemDelegateForColumn(4, new StyleComboBoxItemDelegate(ui.treeView, true));
    ui.treeView->setItemDelegateForColumn(7, new StyleLineEditDelegate(ui.treeView));
    ui.treeView->setItemDelegateForColumn(8, new StyleComboBoxItemDelegate(ui.treeView, false));

    ui.treeView->setModel(m_model);
    ui.treeView->header()->moveSection(5, 2);
    // if (curveTreeViewType != 0) {
    ui.treeView->header()->moveSection(6, 1);
    //}
    ui.treeView->header()->moveSection(7, 1);

    // QStringList strList = gConfGet(CurveTreeViewHideColumns).toStringList();
    // if (strList.size() > 0) {
    //     if (strList.contains("索引号")) {
    ui.treeView->hideColumn(1);
    //    }
    //    if (strList.contains("别名")) {
    ui.treeView->hideColumn(5);
    //    }
    //    if (strList.contains("线宽")) {
    //        ui.treeView->hideColumn(3);
    //    }
    //    if (strList.contains("路径")) {
    //        ui.treeView->hideColumn(6);
    //    }
    //}

    ui.treeView->header()->setMinimumSectionSize(60);
    ui.treeView->header()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui.treeView->header()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui.treeView->header()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui.treeView->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui.treeView->header()->setSectionResizeMode(7, QHeaderView::Interactive);
    ui.treeView->header()->setSectionResizeMode(8, QHeaderView::Fixed);
    ui.treeView->setColumnWidth(0, 80);
    ui.treeView->setColumnWidth(7, 160);
    ui.treeView->setColumnWidth(2, 60);
    ui.treeView->setColumnWidth(3, 60);
    ui.treeView->setColumnWidth(4, 80);
    ui.treeView->setColumnWidth(8, 80);
    // if (gConfGet(IsShowDictionaryView).toBool()) {
    QWidget *dataDictionaryView = createDataDictionaryView();
    if (dataDictionaryView) {
        dataDictionaryView->setMinimumWidth(256);
        dataDictionaryView->setMaximumWidth(320);
        ui.splitter->insertWidget(0, dataDictionaryView);
    }
    //}
    ui.doubleSpinBox_XRange->setMaximum(DBL_MAX);
    // if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
    //     //		ui.lineEdit_Title->setEnabled(false);
    //     ui.lineEdit_SubTitle->setEnabled(false);
    // }
    //  connect(ui.pushButton_OK,SIGNAL(clicked()),this,SLOT(onOK()));
    //  connect(ui.pushButton_Cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
    ui.pushButton_Delete->setShortcut(QKeySequence::Delete);
    // if ((PlotProject)gConfGet("PlotProject").toInt()
    //     == QuiKIS) // 2022.11.1修改为：仅当工程为QuiKIS，隐藏“删除选中变量”和“清除”，按钮
    //{
    //     ui.verticalLayout->removeItem(ui.gridLayout_2); // 去除界面中的两个按钮所在的布局器
    //     ui.pushButton_Delete->hide();
    //     ui.pushButton_Clear->hide();
    // } else {
    connect(ui.pushButton_Delete, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(ui.pushButton_Clear, SIGNAL(clicked()), this, SLOT(onClear()));
    //}

    connect(ui.checkBox_AutoYAxis, SIGNAL(clicked(bool)), this, SLOT(onAutoYAxisChecked(bool)));
    connect(ui.checkBox_ShowTitle, SIGNAL(clicked(bool)), this, SLOT(onShowTitleChecked(bool)));
    connect(ui.checkBox_ShowSubTitle, SIGNAL(clicked(bool)), this, SLOT(onShowSubTitleChecked(bool)));
    connect(ui.checkBox_ShowXTitle, SIGNAL(clicked(bool)), this, SLOT(onShowXTitleChecked(bool)));
    connect(ui.checkBox_ShowYTitle, SIGNAL(clicked(bool)), this, SLOT(onShowYTitleChecked(bool)));
    connect(ui.comboBox_XType, SIGNAL(currentIndexChanged(int)), this, SLOT(onXTypeChanged(int)));

    connect(ui.doubleSpinBox_XRangeMin, SIGNAL(editingFinished()), this, SLOT(onXMinValueEditingFinished()));
    connect(ui.doubleSpinBox_XRangeMax, SIGNAL(editingFinished()), this, SLOT(onXMaxValueEditingFinished()));
    connect(ui.doubleSpinBox_YRangeMin, SIGNAL(editingFinished()), this, SLOT(onYMinValueEditingFinished()));
    connect(ui.doubleSpinBox_YRangeMax, SIGNAL(editingFinished()), this, SLOT(onYMaxValueEditingFinished()));
    connect(ui.colorBtn, &QPushButton::clicked, this, &PlotSettingsDialog::onSetColorBtnClick);
}

// void PlotSettingsDialog::onOK(void)
//{
//     m_plotInfo.XType=ui.comboBox_XType->currentIndex();
//     m_plotInfo.Title=ui.lineEdit_Title->text();
//     m_plotInfo.XRange=ui.doubleSpinBox_XRange->value();
//     m_plotInfo.BackgroundColor=ui.pushButton_BackColor->getColor();
//     m_plotInfo.LegendSide=ui.comboBox_LegendSide->currentIndex();
//     m_plotInfo.GridType=ui.comboBox_GridType->currentIndex();
//     //if(m_plotInfo.VaribleInfoList.size()>0);
//     //m_plotInfo.VaribleInfoList[0].CurveInfoList=m_model->getCurveInfo();
//     m_plotInfo.VaribleInfoList=m_model->getPlotVaribleInfoList();
//
//
//     QDialog::accept();
// }

// void PlotSettingsDialog::onCancel(void)
//{
//     QDialog::reject();
// }

void PlotSettingsDialog::onDelete(void)
{
    for (auto &x : ui.treeView->selectionModel()->selectedIndexes()) {
        if (!x.isValid())
            return;
        m_model->deletePlotVaribleInfo(x);
    }
    // QModelIndex index = ui.treeView->currentIndex();
    // if (!index.isValid())
    //     return;
    // m_model->deletePlotVaribleInfo(index);
    ui.treeView->expandAll();
}

void PlotSettingsDialog::onClear(void)
{
    m_model->clearPlotVaribleInfoList();
}

void PlotSettingsDialog::onSetColorBtnClick()
{
    QColor clr;

    KColorDialog dlg(tr("Select Color"), this);
    dlg.setCurrentColor(m_plotInfo.BackgroundColor);
    if (dlg.exec() != KColorDialog::Ok)
        return;
    clr = dlg.currentColor();
    if (!clr.isValid())
        return;
    ui.bgcolor_edit->setText(clr.name());
}

void PlotSettingsDialog::onAutoYAxisChecked(bool b)
{
    if (b) {
        ui.doubleSpinBox_YRangeMin->setEnabled(false);
        ui.doubleSpinBox_YRangeMax->setEnabled(false);
    } else {
        ui.doubleSpinBox_YRangeMin->setEnabled(true);
        ui.doubleSpinBox_YRangeMax->setEnabled(true);
    }
}

void PlotSettingsDialog::onShowTitleChecked(bool b)
{
    /*if(b)
    {
            ui.lineEdit_Title->setEnabled(true);
            ui.checkBox_ShowSubTitle->setEnabled(true);
    }
    else
    {
            ui.lineEdit_Title->setEnabled(false);
            ui.checkBox_ShowSubTitle->setEnabled(false);
            ui.checkBox_ShowSubTitle->setChecked(false);
            ui.lineEdit_SubTitle->setEnabled(false);
    }*/
}

void PlotSettingsDialog::onShowSubTitleChecked(bool b)
{
    // ui.lineEdit_SubTitle->setEnabled(b);
}

void PlotSettingsDialog::onShowXTitleChecked(bool b)
{
    // ui.lineEdit_XTitle->setEnabled(b);
}

void PlotSettingsDialog::onShowYTitleChecked(bool b)
{
    // ui.lineEdit_YTitle->setEnabled(b);
}

void PlotSettingsDialog::onXTypeChanged(int index)
{
    if (index < 2) {
        ui.label_XRange1->setVisible(true);
        ui.doubleSpinBox_XRange->setVisible(true);
        ui.label_XRange2->setVisible(false);
        ui.label_XRangeTo->setVisible(false);
        ui.doubleSpinBox_XRangeMax->setVisible(false);
        ui.doubleSpinBox_XRangeMin->setVisible(false);
        ui.checkBox_xBottomLog->setVisible(false);
    } else {
        ui.label_XRange1->setVisible(false);
        ui.doubleSpinBox_XRange->setVisible(false);
        ui.label_XRange2->setVisible(true);
        ui.label_XRangeTo->setVisible(true);
        ui.doubleSpinBox_XRangeMax->setVisible(true);
        ui.doubleSpinBox_XRangeMin->setVisible(true);
        ui.checkBox_xBottomLog->setVisible(true);
    }

    // if (index == 0) {
    //     ui.doubleSpinBox_XRange->setMaximum(1000000);
    // } else if (index == 1) {
    //     ui.doubleSpinBox_XRange->setMaximum(1000);
    // }
}

void PlotSettingsDialog::onXMinValueEditingFinished()
{
    double value = ui.doubleSpinBox_XRangeMin->value();
    double max = ui.doubleSpinBox_XRangeMax->value();

    double x = ui.doubleSpinBox_XRangeMin->minimum();

    if (value >= max) {
        int decimal = ui.doubleSpinBox_XRangeMin->decimals();
        ui.doubleSpinBox_XRangeMin->setValue(max - pow(0.1, decimal));
        return;
    }

    if (value < m_plotInfo.XMin)
        value = m_plotInfo.XMin;

    ui.doubleSpinBox_XRangeMin->setValue(value);
}

void PlotSettingsDialog::onXMaxValueEditingFinished()
{
    double value = ui.doubleSpinBox_XRangeMax->value();
    double min = ui.doubleSpinBox_XRangeMin->value();

    if (value <= min) {
        int decimal = ui.doubleSpinBox_XRangeMax->decimals();
        ui.doubleSpinBox_XRangeMax->setValue(min + pow(0.1, decimal));
        return;
    }

    if (value > m_plotInfo.XMax)
        value = m_plotInfo.XMax;

    ui.doubleSpinBox_XRangeMax->setValue(value);
}

void PlotSettingsDialog::onYMinValueEditingFinished()
{
    double value = ui.doubleSpinBox_YRangeMin->value();
    double max = ui.doubleSpinBox_YRangeMax->value();

    double x = ui.doubleSpinBox_YRangeMin->minimum();

    if (value >= max) {
        int decimal = ui.doubleSpinBox_YRangeMin->decimals();
        ui.doubleSpinBox_YRangeMin->setValue(max - pow(0.1, decimal));
        return;
    }

    ui.doubleSpinBox_YRangeMin->setValue(value);
}

void PlotSettingsDialog::onYMaxValueEditingFinished()
{
    double value = ui.doubleSpinBox_YRangeMax->value();
    double min = ui.doubleSpinBox_YRangeMin->value();

    if (value <= min) {
        int decimal = ui.doubleSpinBox_YRangeMax->decimals();
        ui.doubleSpinBox_YRangeMax->setValue(min + pow(0.1, decimal));
        return;
    }

    ui.doubleSpinBox_YRangeMax->setValue(value);
}

QWidget *PlotSettingsDialog::createDataDictionaryView(void)
{
    PIDataDictionaryServer dataDictionaryServer = RequestServer<IDataDictionaryServer>();
    if (!dataDictionaryServer) {
        LOGOUT(tr("DataDictionaryServer is not found"), LOG_WARNING); //"未查询到 DataDictionaryServer 服务"
        return nullptr;
    }

    PIDataDictionaryBuilder dataDictionaryBuilder = dataDictionaryServer->GetCurrentRuntimeDictionary();

    if (!dataDictionaryBuilder) {
        LOGOUT(tr("Currently, there is no running data dictionary"), LOG_WARNING); // 当前未有运行的数据字典
        return nullptr;
    }

    PIDataDictionaryViewServer dataDictionaryViewServer = RequestServer<IDataDictionaryViewServer>();
    if (!dataDictionaryViewServer) {
        LOGOUT(tr("DataDictionaryViewServer is not found"), LOG_WARNING); // 未查询到DataDictionaryViewServer
        return nullptr;
    }

    QWidget *dataDictionaryView = dataDictionaryViewServer->CreateElementTreeViewWidget(nullptr);
    if (!dataDictionaryView) {
        LOGOUT(tr("Failed to create the data dictionary window"), LOG_WARNING); // 创建数据字典窗口失败
        return nullptr;
    }
    m_dataDictionaryViewController =
            QueryInterface<IElementTreeViewController>(QSharedPointer<QWidget>(dataDictionaryView));
    m_dataDictionaryViewController->EnableDisplayVariableValue(true);
    m_dataDictionaryViewController->SetEditable(false);
    m_dataDictionaryViewController->SetStructDrag(false);
    m_dataDictionaryViewController->EnableDisplayVariableValue(false);
    m_dataDictionaryViewController->HideColumn(1);
    m_dataDictionaryViewController->HideColumn(2);
    m_dataDictionaryViewController->SetShowIcon(false);
    m_dataDictionaryViewController->EnableShowProperty(false);
    m_dataDictionaryViewController->ShowSaveButton(false);
    m_dataDictionaryViewController->ShowSearchWidget(true);
    unsigned long long id = dataDictionaryBuilder->GetId();
    m_dataDictionaryViewController->LoadDataDictionary(id);

    QList<QTreeView *> allTree = dataDictionaryView->findChildren<QTreeView *>();
    if (allTree.isEmpty() || !allTree[0])
        return dataDictionaryView;
    allTree[0]->setSortingEnabled(true);
    allTree[0]->sortByColumn(0, Qt::AscendingOrder);
    allTree[0]->setIndentation(20);
    // 数据字典插件的qss文件需要知道更多详细信息，暂时写在这里，后面改过去
    allTree[0]->header()->setStyleSheet(R"(
                QHeaderView::section:horizontal
                {
                    border-top: none;
                    border-left: none;
                    border-right: none;
                    padding-left:10px;
                }
                )");
    allTree[0]->setStyleSheet(R"(QTreeView{margin:0px;})");
    return dataDictionaryView;
}

void PlotSettingsDialog::setTreeExpandState(QTreeView *varTree)
{
    if (!varTree)
        return;
    PIMonitorPanelPlotServer pImonitorServer = RequestServer<IMonitorPanelPlotServer>();
    if (!pImonitorServer) {
        LOGOUT(tr("IMonitorPanelPlotServer is not found"), LOG_WARNING); // 未查询到IMonitorPanelPlotServer
        return;
    }
    MonitorPanelPlotServer *pMonitorServer = static_cast<MonitorPanelPlotServer *>(pImonitorServer.data());
    if (!pMonitorServer) {
        LOGOUT(tr("MonitorPanelPlotServer is not found"), LOG_WARNING); // 未查询到MonitorPanelPlotServer
        return;
    }

    // 获取全部已添加变量路径(画板，原件，变量)
    QStringList allHasPut;
    QList<ItemPlot *> &allPlot = pMonitorServer->getAllItemPlots();
    if (allPlot.isEmpty())
        return;

    for (uint i = 0; i < allPlot.size(); ++i) {
        if (!allPlot[i])
            continue;
        QReadWriteLock &plotLock = allPlot[i]->getPlotLock();
        plotLock.lockForRead();
        const QStringList &pltPara = allPlot[i]->getPlotAllParas();
        plotLock.unlock();
        for (int j = 0; j < pltPara.size(); ++j) {
            if (allHasPut.contains(pltPara[j]))
                continue;
            allHasPut << pltPara[j];
        }
    }

    if (allHasPut.isEmpty()) {
        // varTree->expandAll();
        varTree->expandToDepth(1);
        return;
    }

    QAbstractItemModel *model = varTree->model();
    for (uint dicNum = 0; dicNum < varTree->model()->rowCount(); ++dicNum) {
        QModelIndex dicIndex = varTree->model()->index(dicNum, 0);
        const QString &dicName = dicIndex.data(Qt::DisplayRole).toString();

        for (uint boradNum = 0; boradNum < model->rowCount(dicIndex); ++boradNum) {
            QModelIndex BoardIndex = model->index(boradNum, 0, dicIndex);
            const QString &boardName = BoardIndex.data(Qt::DisplayRole).toString();

            for (uint elementNum = 0; elementNum < model->rowCount(BoardIndex); ++elementNum) {
                QModelIndex elementIndex = model->index(elementNum, 0, BoardIndex);
                const QString &tmpPath =
                        "Dictionary." + dicName + "." + boardName + "." + elementIndex.data(Qt::DisplayRole).toString();
                int tmpSize = 0;
                for (uint paraVecEle = 0; paraVecEle < allHasPut.size(); ++paraVecEle) {
                    if (allHasPut[paraVecEle].contains(tmpPath))
                        ++tmpSize;
                }
                if (tmpSize >= model->rowCount(elementIndex)) {
                    varTree->setExpanded(elementIndex, false);
                } else {
                    varTree->setExpanded(elementIndex, true);
                }
            }
            varTree->setExpanded(BoardIndex, true);
        }
        varTree->setExpanded(dicIndex, true);
    }
}

void PlotSettingsDialog::FillYscaleCombo(QComboBox *b)
{
    QString base("10");
    base.push_back(0x00B3);
    b->addItem(base);

    base = "10";
    base.push_back(0x00B2);
    b->addItem(base);

    base = "10";
    base.push_back(0x00B9);
    b->addItem(base);

    base = "10";
    base.push_back(0x2070);
    b->addItem(base);

    base = "10";
    base.push_back(0x207B);
    base.push_back(0x00B9);
    b->addItem(base);

    base = "10";
    base.push_back(0x207B);
    base.push_back(0x00B2);
    b->addItem(base);

    base = "10";
    base.push_back(0x207B);
    base.push_back(0x00B3);
    b->addItem(base);
}

CustomPlotSettingsDialog::CustomPlotSettingsDialog(QWidget *p /*= nullptr*/) : KCustomDialog(p)
{
    m_pPlotSettingsDialog = new PlotSettingsDialog();
    setCentralWidget(m_pPlotSettingsDialog);
    setStandardButtons(KCustomDialog::Ok | KCustomDialog::Cancel);
    setTitle(tr("Curve Settings")); // 曲线设置
    // setMargin(1);
    //  if ((PlotProject)gConfGet("PlotProject").toInt() == QuiKIS) {
    //      resize(600, 700);
    //  } else {
    resize(960, 700);
    //}
    setMinimumHeight(700);
    // this->setModal(false);

    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));

    setWindowFlags(windowFlags() /*| Qt::WindowStaysOnTopHint*/);

    m_isShow = true;
}

CustomPlotSettingsDialog::~CustomPlotSettingsDialog()
{
    m_isShow = false;
}

void CustomPlotSettingsDialog::setPlotInfo(const PlotInfo &plotInfo)
{
    m_pPlotSettingsDialog->setPlotInfo(plotInfo);
}

PlotInfo CustomPlotSettingsDialog::getPlotInfo(void)
{
    return m_pPlotSettingsDialog->getPlotInfo();
}

void CustomPlotSettingsDialog::confirm()
{
    m_pPlotSettingsDialog->confirm();
}

void CustomPlotSettingsDialog::onFinished(int result)
{
    bool b = false;
    if (result == KCustomDialog::Ok) {
        b = true;
    }

    emit changed(b);
}

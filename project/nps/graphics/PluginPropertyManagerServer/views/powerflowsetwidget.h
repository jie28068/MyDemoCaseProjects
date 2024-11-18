#ifndef POWERFLOWSETWIDGET_H
#define POWERFLOWSETWIDGET_H

#include <QWidget>
// #include <QLineEdit>
#include "customtablewidget.h"
#include "ColorMatchWidget.h"

static const QString TITLE_POWERFLOW_SET = QObject::tr("Power Flow Calculation - Execution"); // 潮流计算-执行
static const QString LOGTAG_PFCLC = QObject::tr("Power Flow Calculation");                    // 潮流计算
static const QString LOGTAG_PFPARAM = QObject::tr("Parameter");                               // 参数

static const QString PFSET_DISPLAYNAME_BASEMVA = QObject::tr("Benchmark Capacity(MVA)");            // 基准容量
static const QString PFSET_DISPLAYNAME_OUTPUTCLCRESULT = QObject::tr("Output Calculation Results"); // 输出计算结果
static const QString PFSET_DISPLAYNAME_BASEMVAAC = QObject::tr("AC Benchmark Capacity(MVA)"); // 交流基准容量
static const QString PFSET_DISPLAYNAME_BASEMVADC = QObject::tr("DC Benchmark Capacity(MVA)"); // 直流基准容量
static const QString PFSET_DISPLAYNAME_POL = QObject::tr("DC System Polarity");               // 直流系统极性
static const QString PFSET_DISPLAYNAME_POWERFLOW_TYPE = QObject::tr("Power Flow Calculation Type"); // 潮流计算类型
static const QString PFSET_DISPLAYNAME_MPOP_PF_ALG = QObject::tr("AC power flow algorithm"); // AC潮流计算算法
static const QString PFSET_DISPLAYNAME_MPOP_PF_CURRENT_BALANCE =
        QObject::tr("Type of nodal balance equation"); // 节点平衡方程的类型
static const QString PFSET_DISPLAYNAME_MPOP_VERBOSE =
        QObject::tr("Amount of progress info printed"); // 输出进度信息的数量
static const QString PFSET_DISPLAYNAME_MPOP_OUT_ALL =
        QObject::tr("Controls pretty-printing of results"); // 控制完整的输出结果
static const QString PFSET_DISPLAYNAME_MPOP_OUT_SYS_SUM = QObject::tr("Print system summary");  // 输出系统总结
static const QString PFSET_DISPLAYNAME_MPOP_OUT_AREA_SUM = QObject::tr("Print area summaries"); // 输出区域摘要
static const QString PFSET_DISPLAYNAME_MPOP_OUT_BUS = QObject::tr("Print bus detail");          // 输出母线细节
static const QString PFSET_DISPLAYNAME_MPOP_OUT_BRANCH = QObject::tr("Print branch detail");    // 输出支路细节
static const QString PFSET_DISPLAYNAME_MPOP_OUT_GEN = QObject::tr("Print generator detail");    // 输出发电机细节
static const QString PFSET_DISPLAYNAME_MPOP_OUT_LIM_ALL =
        QObject::tr("Controls constraint info output"); // 控制约束信息输出
static const QString PFSET_DISPLAYNAME_MPOP_OUT_LIM_V = QObject::tr("Control voltage limit info"); // 控制电压限制信息

static const QString TITLE_COLOR_MATCH = QObject::tr("Color Matching"); // 图标配色页面
static const QString PFSET_KWD_1_L = QString("1.");
static const QString PFSET_KWD_0_95 = QString("0.95");
static const QString PFSET_KWD_0_9 = QString("0.9");
static const QString PFSET_KWD_1_U = QString("1.u");
static const QString PFSET_KWD_1_07 = QString("1.07");
static const QString PFSET_KWD_1_1 = QString("1.1");
class PowerFlowSetWidget : public CWidget
{
    Q_OBJECT

public:
    PowerFlowSetWidget(QWidget *parent = nullptr);
    ~PowerFlowSetWidget();

    // common
    virtual bool saveData() override;
    virtual bool checkLegitimacy(QString &errorinfo) override;

    void setFlowListData(const QList<CustomModelItem> &listdata);
    void updatePFCFG();
    void updateColorMatch();
    QVariant getParamValueByKewWord(const QString &keyword);
    CustomModelItem getParamItemByKewWord(const QString &keyword);
    QVariantMap getPFParamMap();
    // QString getSaveDirPath();

private slots:
    void onTableDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);

private:
    QList<CustomModelItem> getAllDataList();
    void initUI();
    // private slots:
    //	void onSelectDir();
private:
    CustomTableWidget *m_tableWidget;
    ColorMatchWidget *m_lowlimitWidget;
    ColorMatchWidget *m_upperlimitWidget;
    // QLineEdit *m_lineDir;
};

#endif // POWERFLOWSETWIDGET_H

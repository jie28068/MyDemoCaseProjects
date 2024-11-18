#include "powerflowsetwidget.h"

#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "PropertyDialog.h"
#include "PropertyServerMng.h"
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

using namespace Kcc::BlockDefinition;
static const QString PFTYPE_RANGE = QString("AC%1%2%3DC%4%5")
                                            .arg(NPS::MappingSymbol_2Well)
                                            .arg("0")
                                            .arg(NPS::Delimiters_2Dollar)
                                            .arg(NPS::MappingSymbol_2Well)
                                            .arg("1");
static const QString POL_RANGE = QObject::tr("Monopolar%1%2%3Bipolar%4%5")
                                         .arg(NPS::MappingSymbol_2Well)
                                         .arg("1")
                                         .arg(NPS::Delimiters_2Dollar)
                                         .arg(NPS::MappingSymbol_2Well)
                                         .arg("2");

static const QString PF_ALG_RANGE = QObject::tr("NR%1GS%1FDXB%1FDBX").arg(NPS::Delimiters_2Dollar);
static const QString VERBOSE_RANGE = QObject::tr("print no progress info%1%2%3"
                                                 "print a little progress info%1%4%3"
                                                 "print a lot of progress info%1%5%3"
                                                 "print all progress info%1%6")
                                             .arg(NPS::MappingSymbol_2Well)
                                             .arg(0)
                                             .arg(NPS::Delimiters_2Dollar)
                                             .arg(1)
                                             .arg(2)
                                             .arg(3);
static const QString OUT_ALL_RANGE = QObject::tr("individual flags control what prints%1%2%3"
                                                 "do not print anything%1%4%3"
                                                 "print everything%1%5")
                                             .arg(NPS::MappingSymbol_2Well)
                                             .arg(-1)
                                             .arg(NPS::Delimiters_2Dollar)
                                             .arg(0)
                                             .arg(1);
static const QString OUT_LIM_ALL_RANGE = QObject::tr("individual flags control what constraint info prints%1%2%3"
                                                     "no constraint info%1%4%3"
                                                     "binding constraint info%1%5%3"
                                                     "all constraint info%1%6")
                                                 .arg(NPS::MappingSymbol_2Well)
                                                 .arg(-1)
                                                 .arg(NPS::Delimiters_2Dollar)
                                                 .arg(0)
                                                 .arg(1)
                                                 .arg(2);
static const QString OUT_LIM_V_RANGE = QObject::tr("do not print%1%2%3"
                                                   "print binding constraints only%1%4%3"
                                                   "print all constraints%1%5")
                                               .arg(NPS::MappingSymbol_2Well)
                                               .arg(0)
                                               .arg(NPS::Delimiters_2Dollar)
                                               .arg(1)
                                               .arg(2);
USE_LOGOUT_("PowerFlowSetWidget")
PowerFlowSetWidget::PowerFlowSetWidget(QWidget *parent) : CWidget(parent), m_tableWidget(nullptr)
{
    initUI();
}

PowerFlowSetWidget::~PowerFlowSetWidget() { }

bool PowerFlowSetWidget::saveData()
{
    if (PropertyServerMng::getInstance().m_projectManagerServer != nullptr) {
        auto SaveConfigFunc = [this](const QString &kl_pro, const QString &pfset_kwd) -> void {
            QVariant varCfg = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(kl_pro);
            CustomModelItem varItem = getParamItemByKewWord(pfset_kwd);
            if (!varCfg.toString().isEmpty() && varItem.value != varCfg) {
                LOGOUT(NPS::outputLog(LOGTAG_PFCLC, LOGTAG_PFPARAM, varItem.name, varCfg, varItem.value, pfset_kwd,
                                      varItem.valueType, varItem.controlLimit),
                       LOG_NORMAL);
            }
            PropertyServerMng::getInstance().m_projectManagerServer->SetProjectConfig(kl_pro, varItem.value);
        };

        SaveConfigFunc(KL_PRO::SB, PFSET_KWD_BASEMVA);
        SaveConfigFunc(KL_PRO::ACSB, PFSET_KWD_BASEMVAAC);
        SaveConfigFunc(KL_PRO::DCSB, PFSET_KWD_BASEMVADC);
        SaveConfigFunc(KL_PRO::POL, PFSET_KWD_POl);
        // SaveConfigFunc(KL_PRO::POWERFLOW_TYPE, PFSET_KWD_POWERFLOW_TYPE);
        SaveConfigFunc(KL_PRO::OUTPUTCLCRESULT, PFSET_KWD_OUTPUTCLCRESULT);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_PF_ALG, PFSET_KWD_MPOP_PF_ALG);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_PF_CURRENT_BALANCE, PFSET_KWD_MPOP_PF_CURRENT_BALANCE);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_VERBOSE, PFSET_KWD_MPOP_VERBOSE);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_ALL, PFSET_KWD_MPOP_OUT_ALL);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_SYS_SUM, PFSET_KWD_MPOP_OUT_SYS_SUM);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_AREA_SUM, PFSET_KWD_MPOP_OUT_AREA_SUM);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_BUS, PFSET_KWD_MPOP_OUT_BUS);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_BRANCH, PFSET_KWD_MPOP_OUT_BRANCH);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_GEN, PFSET_KWD_MPOP_OUT_GEN);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_LIM_ALL, PFSET_KWD_MPOP_OUT_LIM_ALL);
        SaveConfigFunc(KL_PRO::PFSET_MPOP_OUT_LIM_V, PFSET_KWD_MPOP_OUT_LIM_V);

        // 保存电压等级颜色匹配
        PropertyServerMng::getInstance().m_projectManagerServer->SetProjectConfig(KL_PRO::PFSET_LOWERLIMIT,
                                                                                  m_lowlimitWidget->getNewListData());
        PropertyServerMng::getInstance().m_projectManagerServer->SetProjectConfig(KL_PRO::PFSET_UPPERLIMIT,
                                                                                  m_upperlimitWidget->getNewListData());
    }

    return false;
}

bool PowerFlowSetWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (m_tableWidget == nullptr) {
        errorinfo == tr("table object is empty!"); // 表格对象为空！
        return false;
    }
    // if (getSaveDirPath().isEmpty()){
    //	errorinfo = QString("文件保存路径为空，请选择路径！");
    //	return false;
    // }
    return m_tableWidget->checkLegitimacy(errorinfo);
}

void PowerFlowSetWidget::setFlowListData(const QList<CustomModelItem> &listdata)
{
    if (m_tableWidget != nullptr) {
        m_tableWidget->setListData(listdata);
    }
}

void PowerFlowSetWidget::updatePFCFG()
{
    int baseMVA = 100;
    int baseMVAAC = 100;
    int baseMVADC = 100;
    int pol = 2;    // 1是单极(Monopolar)，2是双极(Bipolar)
    int pftype = 0; // ac=0,dc=1
    bool pfoutputLog = true;
    QString pf_alg = "NR";
    bool pf_current_balance = false;
    int verbose = 1;
    int out_all = -1;
    bool out_sys_sum = true;
    bool out_area_sum = false;
    bool out_bus = true;
    bool out_branch = true;
    bool out_gen = false;
    int out_lim_all = -1;
    int out_lim_v = 1;
    if (PropertyServerMng::getInstance().m_projectManagerServer != nullptr) {
        QVariant CFGbaseMVA = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::SB);
        if (!CFGbaseMVA.toString().isEmpty()) {
            baseMVA = CFGbaseMVA.toInt();
        }

        QVariant CFGbaseMVAAC = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::ACSB);
        if (!CFGbaseMVAAC.toString().isEmpty()) {
            baseMVAAC = CFGbaseMVAAC.toInt();
        }
        QVariant CFGbaseMVADC = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::DCSB);
        if (!CFGbaseMVADC.toString().isEmpty()) {
            baseMVADC = CFGbaseMVADC.toInt();
        }
        QVariant CFGpol = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::POL);
        if (!CFGpol.toString().isEmpty()) {
            pol = CFGpol.toInt();
        }
        // QVariant CFGpftype =
        //         PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::POWERFLOW_TYPE);
        // if (!CFGpftype.toString().isEmpty()) {
        //     pftype = CFGpftype.toInt();
        // }
        QVariant CFGpfoutputLog =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::OUTPUTCLCRESULT);
        if (!CFGpfoutputLog.toString().isEmpty()) {
            pfoutputLog = CFGpfoutputLog.toBool();
        }
        QVariant CFGpfAlg =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_MPOP_PF_ALG);
        if (!CFGpfAlg.toString().isEmpty()) {
            pf_alg = CFGpfAlg.toString();
        }
        QVariant CFGpfCurrentBalance = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(
                KL_PRO::PFSET_MPOP_PF_CURRENT_BALANCE);
        if (!CFGpfCurrentBalance.toString().isEmpty()) {
            pf_current_balance = CFGpfCurrentBalance.toBool();
        }
        QVariant CFGverbose =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_MPOP_VERBOSE);
        if (!CFGverbose.toString().isEmpty()) {
            verbose = CFGverbose.toInt();
        }
        QVariant CFGoutAll =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_MPOP_OUT_ALL);
        if (!CFGoutAll.toString().isEmpty()) {
            out_all = CFGoutAll.toInt();
        }
        QVariant CFGoutSysSum = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(
                KL_PRO::PFSET_MPOP_OUT_SYS_SUM);
        if (!CFGoutSysSum.toString().isEmpty()) {
            out_sys_sum = CFGoutSysSum.toBool();
        }
        QVariant CFGoutAreaSum = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(
                KL_PRO::PFSET_MPOP_OUT_AREA_SUM);
        if (!CFGoutAreaSum.toString().isEmpty()) {
            out_area_sum = CFGoutAreaSum.toBool();
        }
        QVariant CFGoutBus =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_MPOP_OUT_BUS);
        if (!CFGoutBus.toString().isEmpty()) {
            out_bus = CFGoutBus.toBool();
        }
        QVariant CFGoutBranch = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(
                KL_PRO::PFSET_MPOP_OUT_BRANCH);
        if (!CFGoutBranch.toString().isEmpty()) {
            out_branch = CFGoutBranch.toBool();
        }
        QVariant CFGoutGen =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_MPOP_OUT_GEN);
        if (!CFGoutGen.toString().isEmpty()) {
            out_gen = CFGoutGen.toBool();
        }
        QVariant CFGoutLimAll = PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(
                KL_PRO::PFSET_MPOP_OUT_LIM_ALL);
        if (!CFGoutLimAll.toString().isEmpty()) {
            out_lim_all = CFGoutLimAll.toInt();
        }
        QVariant CFGoutLimV =
                PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_MPOP_OUT_LIM_V);
        if (!CFGoutLimV.toString().isEmpty()) {
            out_lim_v = CFGoutLimV.toInt();
        }
    }

    QList<CustomModelItem> listdata;
    // listdata.append(CustomModelItem(PFSET_KWD_POWERFLOW_TYPE, PFSET_DISPLAYNAME_POWERFLOW_TYPE, pftype,
    //                                 RoleDataDefinition::ControlTypeComboBox, true, PFTYPE_RANGE));
    listdata.append(CustomModelItem(PFSET_KWD_BASEMVA, PFSET_DISPLAYNAME_BASEMVA, baseMVA,
                                    RoleDataDefinition::ControlTypeInt, true, "[1,]"));
    listdata.append(CustomModelItem(PFSET_KWD_BASEMVAAC, PFSET_DISPLAYNAME_BASEMVAAC, baseMVAAC,
                                    RoleDataDefinition::ControlTypeInt, true, "[1,]"));
    listdata.append(CustomModelItem(PFSET_KWD_BASEMVADC, PFSET_DISPLAYNAME_BASEMVADC, baseMVADC,
                                    RoleDataDefinition::ControlTypeInt, true, "[1,]"));
    listdata.append(CustomModelItem(PFSET_KWD_POl, PFSET_DISPLAYNAME_POL, pol, RoleDataDefinition::ControlTypeComboBox,
                                    true, POL_RANGE));
    listdata.append(CustomModelItem(PFSET_KWD_OUTPUTCLCRESULT, PFSET_DISPLAYNAME_OUTPUTCLCRESULT, pfoutputLog,
                                    RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_PF_ALG, PFSET_DISPLAYNAME_MPOP_PF_ALG, pf_alg,
                                    RoleDataDefinition::ControlTypeComboBox, true, PF_ALG_RANGE));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_PF_CURRENT_BALANCE, PFSET_DISPLAYNAME_MPOP_PF_CURRENT_BALANCE,
                                    pf_current_balance, RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_VERBOSE, PFSET_DISPLAYNAME_MPOP_VERBOSE, verbose,
                                    RoleDataDefinition::ControlTypeComboBox, true, VERBOSE_RANGE));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_ALL, PFSET_DISPLAYNAME_MPOP_OUT_ALL, out_all,
                                    RoleDataDefinition::ControlTypeComboBox, true, OUT_ALL_RANGE));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_SYS_SUM, PFSET_DISPLAYNAME_MPOP_OUT_SYS_SUM, out_sys_sum,
                                    RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_AREA_SUM, PFSET_DISPLAYNAME_MPOP_OUT_AREA_SUM, out_area_sum,
                                    RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_BUS, PFSET_DISPLAYNAME_MPOP_OUT_BUS, out_bus,
                                    RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_BRANCH, PFSET_DISPLAYNAME_MPOP_OUT_BRANCH, out_branch,
                                    RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_GEN, PFSET_DISPLAYNAME_MPOP_OUT_GEN, out_gen,
                                    RoleDataDefinition::ControlTypeCheckbox, true, ""));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_LIM_ALL, PFSET_DISPLAYNAME_MPOP_OUT_LIM_ALL, out_lim_all,
                                    RoleDataDefinition::ControlTypeComboBox, true, OUT_LIM_ALL_RANGE));
    listdata.append(CustomModelItem(PFSET_KWD_MPOP_OUT_LIM_V, PFSET_DISPLAYNAME_MPOP_OUT_LIM_V, out_lim_v,
                                    RoleDataDefinition::ControlTypeComboBox, true, OUT_LIM_V_RANGE));

    setFlowListData(listdata);
}

QVariant PowerFlowSetWidget::getParamValueByKewWord(const QString &keyword)
{
    QList<CustomModelItem> listdata = getAllDataList();
    foreach (CustomModelItem item, listdata) {
        if (keyword == item.keywords) {
            return item.value;
        }
    }
    return QVariant();
}

CustomModelItem PowerFlowSetWidget::getParamItemByKewWord(const QString &keyword)
{
    QList<CustomModelItem> listdata = getAllDataList();
    foreach (CustomModelItem item, listdata) {
        if (keyword == item.keywords) {
            return item;
        }
    }
    return CustomModelItem();
}

QVariantMap PowerFlowSetWidget::getPFParamMap()
{
    QVariantMap parammap;
    QList<CustomModelItem> listdata = getAllDataList();
    foreach (CustomModelItem item, listdata) {
        if (!item.keywords.isEmpty()) {
            parammap.insert(item.keywords, item.value);
        }
    }
    return parammap;
}

// QString PowerFlowSetWidget::getSaveDirPath()
//{
//	if (m_lineDir != nullptr)
//		return m_lineDir->text();
//	return "";
// }

void PowerFlowSetWidget::onTableDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem) { }

QList<CustomModelItem> PowerFlowSetWidget::getAllDataList()
{
    if (m_tableWidget == nullptr)
        return QList<CustomModelItem>();
    return m_tableWidget->getNewListData();
}

void PowerFlowSetWidget::initUI()
{
    setMinimumWidth(820);
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setMargin(0);
    m_tableWidget = new CustomTableWidget();
    updatePFCFG();
    QWidget *matchWidget = new QWidget();
    QVBoxLayout *vlayout = new QVBoxLayout(matchWidget);
    m_lowlimitWidget = new ColorMatchWidget(matchWidget);
    m_upperlimitWidget = new ColorMatchWidget(matchWidget);
    vlayout->addWidget(m_lowlimitWidget);
    vlayout->addWidget(m_upperlimitWidget);

    updateColorMatch();
    CommonWrapper *wrapper = new CommonWrapper();
    wrapper->addWidget(m_tableWidget, TITLE_POWERFLOW_SET);
    wrapper->addWidget(matchWidget, TITLE_COLOR_MATCH);
    PropertyDialog *centralWidget = new PropertyDialog(this);
    centralWidget->setProperty("isDlgmemoryBlock", "false");
    centralWidget->setStyleSheet("PropertyDialog{border:0px;}");
    centralWidget->setPropertyWrapper(wrapper);
    pGridLayout->addWidget(centralWidget, 0, 0, 1, 4);

    connect(m_tableWidget, SIGNAL(tableDataItemChanged(const CustomModelItem &, const CustomModelItem &)), this,
            SLOT(onTableDataItemChanged(const CustomModelItem &, const CustomModelItem &)), Qt::UniqueConnection);
}
void PowerFlowSetWidget::updateColorMatch()
{
    QList<ColorMatchModelItem> listdata;
    ColorMatchModelItem item1;
    ColorMatchModelItem item2;
    ColorMatchModelItem item3;
    item1.m_typelist << RoleDataDefinition::ControlTypeTextbox << RoleDataDefinition::ControlTypeDouble
                     << RoleDataDefinition::ControlTypeColor;
    item1.m_caneditlist << false << true << true;
    item2.m_typelist << RoleDataDefinition::ControlTypeTextbox << RoleDataDefinition::ControlTypeDouble
                     << RoleDataDefinition::ControlTypeColor;
    item2.m_caneditlist << false << true << true;
    item3.m_typelist << RoleDataDefinition::ControlTypeTextbox << RoleDataDefinition::ControlTypeDouble
                     << RoleDataDefinition::ControlTypeColor;
    item3.m_caneditlist << false << true << true;
    QVariantList listval = PropertyServerMng::getInstance()
                                   .m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_LOWERLIMIT)
                                   .toList();
    if (listval.isEmpty()) {
        item1.m_datalist << "<=" << 1.0 << QColor(0, 191, 0);
        listdata.append(item1);
        item2.m_datalist << "<=" << 0.95 << QColor(0, 255, 255);
        listdata.append(item2);
        item3.m_datalist << "<=" << 0.9 << QColor(0, 0, 255);
        listdata.append(item3);
    } else {
        item1.m_datalist << "<=" << listval.at(0).toDouble() << listval.at(1).value<QColor>();
        listdata.append(item1);
        item2.m_datalist << "<=" << listval.at(2).toDouble() << listval.at(3).value<QColor>();
        listdata.append(item2);
        item3.m_datalist << "<=" << listval.at(4).toDouble() << listval.at(5).value<QColor>();
        listdata.append(item3);
    }
    if (m_lowlimitWidget != nullptr) {
        m_lowlimitWidget->setListData(listdata, QStringList() << " " << tr("Voltage Lower Limit") << tr("Color"));
    }

    listdata.clear();
    item1.m_datalist.clear();
    item2.m_datalist.clear();
    item3.m_datalist.clear();

    listval = PropertyServerMng::getInstance()
                      .m_projectManagerServer->GetProjectConfig(KL_PRO::PFSET_UPPERLIMIT)
                      .toList();
    if (listval.isEmpty()) {
        item1.m_datalist << ">=" << 1.0 << QColor(0, 191, 0);
        listdata.append(item1);
        item2.m_datalist << ">=" << 1.07 << QColor(255, 255, 102);
        listdata.append(item2);
        item3.m_datalist << ">=" << 1.1 << QColor(255, 0, 0);
        listdata.append(item3);
    } else {
        item1.m_datalist << ">=" << listval.at(0).toDouble() << listval.at(1).value<QColor>();
        listdata.append(item1);
        item2.m_datalist << ">=" << listval.at(2).toDouble() << listval.at(3).value<QColor>();
        listdata.append(item2);
        item3.m_datalist << ">=" << listval.at(4).toDouble() << listval.at(5).value<QColor>();
        listdata.append(item3);
    }
    if (m_upperlimitWidget != nullptr) {
        m_upperlimitWidget->setListData(listdata, QStringList() << " " << tr("Voltage Upper Limit") << tr("Color"));
    }
}

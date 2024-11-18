#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QApplication>

#define INI_PATH        (QApplication::applicationDirPath() + "/set.ini")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wage(10000),
    otherDeduction(0),
    accumulationFund(0),
    medicalInsurance(0),
    endowmentInsurance(0),
    unemploymentInsurance(0),
    childrenEducation(0),
    continuingEducation(0),
    treatmentForSeriousDisease(0),
    homeLoanInterest(0),
    housingRents(0),
    careForTheElderly(0),
    cumulativeTaxableIncome_previousMonth(0),
    cumulativeTaxPayable_previousMonth(0)
{
    ui->setupUi(this);
    setWindowTitle(tr("Personal Income Tax Calculator V1.0 (By bailiyang)"));
    setStatusBar();

    QPixmap pixmap(":/taxTable.png");
    ui->label_img->setPixmap(pixmap);

    // 从ini读取参数，更新至界面
    loadParams();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 本月累计应纳税所得额 = (税前月工资 - 5000 - 五险一金 - 专项附加扣除 - 其他扣除) + 上月累计应纳税所得额
// 本月累计应纳税额 = 本月累计应纳税所得额 × 税率 - 速算扣除数
// 个人所得税 = 本月累计应纳税额 - 上月累计应纳税额
// 到手所得 = 税前月工资 - 五险一金 - 个人所得税
void MainWindow::on_pushButton_ok_clicked()
{
    getParamsFromUi();
    if (!isValid())
    {
        return;
    }

    // 将参数保存至ini文件
    saveParams();

    // 累计五险一金
    double fiveOneInsuranceFund = accumulationFund + medicalInsurance +
            endowmentInsurance + unemploymentInsurance;
    QString text = QString(tr("Five One Insurance Fund(%1)")).arg(fiveOneInsuranceFund);
    ui->groupBox_fiveOneInsuranceFund->setTitle(text);

    // 累计专项附加扣除
    double specialAdditionalDeduction = childrenEducation + continuingEducation + treatmentForSeriousDisease +
            homeLoanInterest + housingRents + careForTheElderly;
    text = QString(tr("Special Additional Deduction(%1)")).arg(specialAdditionalDeduction);
    ui->groupBox_specialAdditionalDeduction->setTitle(text);

    // 计算本月累计应纳税所得额
    double cumulativeTaxableIncome = (wage - 5000 - fiveOneInsuranceFund - specialAdditionalDeduction - otherDeduction) +
            cumulativeTaxableIncome_previousMonth;
    if (cumulativeTaxableIncome < 0)
    {
        cumulativeTaxableIncome = 0; // 无需交税
    }
    ui->lineEdit_cumulativeTaxableIncome->setText(QString::number(cumulativeTaxableIncome, 'g', 7));

    // 计算本月累计应纳税额
    float rate = 0.0;
    int offset = 0;
    getRateAndOffset(cumulativeTaxableIncome, rate, offset);
    double cumulativeTaxPayable = cumulativeTaxableIncome * rate - offset;
    ui->lineEdit_cumulativeTaxPayable->setText(QString::number(cumulativeTaxPayable, 'g', 7));

    // 计算个人所得税
    double personalIncomeTax = cumulativeTaxPayable - cumulativeTaxPayable_previousMonth;
    ui->lineEdit_personalIncomeTax->setText(QString::number(personalIncomeTax, 'g', 7));

    // 计算到手所得
    double takeHomePay = wage - fiveOneInsuranceFund - personalIncomeTax;
    ui->lineEdit_takeHomePay->setText(QString::number(takeHomePay, 'g', 7));
}

void MainWindow::setStatusBar()
{
    QLabel *permanent = new QLabel(this);
    permanent->setText(tr("Welcome to my blog ") + "<a href=\"https://blog.csdn.net/zyhse\">https://blog.csdn.net/zyhse</a>");
    permanent->setTextFormat(Qt::RichText);
    permanent->setOpenExternalLinks(true);
    ui->statusBar->addPermanentWidget(permanent);
}

void MainWindow::getParamsFromUi()
{
    // 税前月工资
    wage = ui->lineEdit_wage->text().toDouble();

    // 其他扣除
    otherDeduction = ui->lineEdit_otherDeduction->text().toDouble();

    // 累计五险一金
    accumulationFund = ui->lineEdit_accumulationFund->text().toDouble(); // 公积金
    medicalInsurance = ui->lineEdit_medicalInsurance->text().toDouble(); // 医疗保险
    endowmentInsurance = ui->lineEdit_endowmentInsurance->text().toDouble(); // 养老保险
    unemploymentInsurance = ui->lineEdit_unemploymentInsurance->text().toDouble(); // 失业保险

    // 累计专项附加扣除
    childrenEducation = ui->lineEdit_childrenEducation->text().toDouble(); // 子女教育
    continuingEducation = ui->lineEdit_continuingEducation->text().toDouble(); // 继续教育
    treatmentForSeriousDisease = ui->lineEdit_treatmentForSeriousDisease->text().toDouble(); // 大病医疗
    homeLoanInterest = ui->lineEdit_homeLoanInterest->text().toDouble(); // 住房贷款利息
    housingRents = ui->lineEdit_housingRents->text().toDouble(); // 住房租金
    careForTheElderly = ui->lineEdit_careForTheElderly->text().toDouble(); // 赡养老人

    // 提供上月数据
    cumulativeTaxableIncome_previousMonth = ui->lineEdit_cumulativeTaxableIncome_previousMonth->text().toDouble(); // 累计应纳税所得额
    cumulativeTaxPayable_previousMonth = ui->lineEdit_cumulativeTaxPayable_previousMonth->text().toDouble(); // 累计应纳税额
}

void MainWindow::setParamsToUi()
{
    // 税前月工资
    ui->lineEdit_wage->setText(QString::number(wage, 'g', 7));

    // 其他扣除
    ui->lineEdit_otherDeduction->setText(QString::number(otherDeduction, 'g', 7));

    // 累计五险一金
    ui->lineEdit_accumulationFund->setText(QString::number(accumulationFund, 'g', 7)); // 公积金
    ui->lineEdit_medicalInsurance->setText(QString::number(medicalInsurance, 'g', 7)); // 医疗保险
    ui->lineEdit_endowmentInsurance->setText(QString::number(endowmentInsurance, 'g', 7)); // 养老保险
    ui->lineEdit_unemploymentInsurance->setText(QString::number(unemploymentInsurance, 'g', 7)); // 失业保险

    // 累计专项附加扣除
    ui->lineEdit_childrenEducation->setText(QString::number(childrenEducation, 'g', 7)); // 子女教育
    ui->lineEdit_continuingEducation->setText(QString::number(continuingEducation, 'g', 7)); // 继续教育
    ui->lineEdit_treatmentForSeriousDisease->setText(QString::number(treatmentForSeriousDisease, 'g', 7)); // 大病医疗
    ui->lineEdit_homeLoanInterest->setText(QString::number(homeLoanInterest, 'g', 7)); // 住房贷款利息
    ui->lineEdit_housingRents->setText(QString::number(housingRents, 'g', 7)); // 住房租金
    ui->lineEdit_careForTheElderly->setText(QString::number(careForTheElderly, 'g', 7)); // 赡养老人

    // 提供上月数据
    ui->lineEdit_cumulativeTaxableIncome_previousMonth->setText(QString::number(cumulativeTaxableIncome_previousMonth, 'g', 7)); // 累计应纳税所得额
    ui->lineEdit_cumulativeTaxPayable_previousMonth->setText(QString::number(cumulativeTaxPayable_previousMonth, 'g', 7)); // 累计应纳税额
}

bool MainWindow::isValid()
{
    if (wage <= 0)
    {
        QMessageBox::warning(nullptr, tr("Warning"), tr("Wage can't <= 0"));
        return false;
    }

    QVector<double> params;
    params.push_back(otherDeduction);
    params.push_back(accumulationFund);
    params.push_back(medicalInsurance);
    params.push_back(endowmentInsurance);
    params.push_back(unemploymentInsurance);
    params.push_back(childrenEducation);
    params.push_back(continuingEducation);
    params.push_back(treatmentForSeriousDisease);
    params.push_back(homeLoanInterest);
    params.push_back(housingRents);
    params.push_back(careForTheElderly);
    params.push_back(cumulativeTaxableIncome_previousMonth);
    params.push_back(cumulativeTaxPayable_previousMonth);
    foreach (double value, params)
    {
        if (value < 0)
        {
            QMessageBox::warning(nullptr, tr("Warning"), QString(tr("The value %1 is illegal and must be >=0")).arg(value));
            return false;
        }
    }
    return true;
}

void MainWindow::getRateAndOffset(double cumulativeTaxableIncome, float &rate, int &offset)
{
    Q_ASSERT(cumulativeTaxableIncome >= 0);
    if (cumulativeTaxableIncome >= 0 && cumulativeTaxableIncome <= 36000)
    {
        rate = 0.03f;
        offset = 0;
    }
    else if (cumulativeTaxableIncome > 36000 && cumulativeTaxableIncome <= 144000)
    {
        rate = 0.1f;
        offset = 2520;
    }
    else if (cumulativeTaxableIncome > 144000 && cumulativeTaxableIncome <= 300000)
    {
        rate = 0.2f;
        offset = 16920;
    }
    else if (cumulativeTaxableIncome > 300000 && cumulativeTaxableIncome <= 420000)
    {
        rate = 0.25f;
        offset = 31920;
    }
    else if (cumulativeTaxableIncome > 420000 && cumulativeTaxableIncome <= 660000)
    {
        rate = 0.3f;
        offset = 52920;
    }
    else if (cumulativeTaxableIncome > 660000 && cumulativeTaxableIncome <= 960000)
    {
        rate = 0.35f;
        offset = 85920;
    }
    else // cumulativeTaxableIncome > 960000
    {
        rate = 0.45f;
        offset = 181920;
    }
}

void MainWindow::saveParams()
{
    // 从界面获取最新参数
    getParamsFromUi();

    // 将最新参数保存至ini
    QSettings setting(INI_PATH, QSettings::IniFormat);
    if (!setting.isWritable())
    {
        QMessageBox::information(nullptr, tr("Information"), tr("Parameter saving failed, The set.ini file does not have write permission"));
        return;
    }

    setting.setValue("Params/wage", wage);
    setting.setValue("Params/otherDeduction", otherDeduction);
    setting.setValue("Params/accumulationFund", accumulationFund);
    setting.setValue("Params/medicalInsurance", medicalInsurance);
    setting.setValue("Params/endowmentInsurance", endowmentInsurance);
    setting.setValue("Params/unemploymentInsurance", unemploymentInsurance);
    setting.setValue("Params/childrenEducation", childrenEducation);
    setting.setValue("Params/continuingEducation", continuingEducation);
    setting.setValue("Params/treatmentForSeriousDisease", treatmentForSeriousDisease);
    setting.setValue("Params/homeLoanInterest", homeLoanInterest);
    setting.setValue("Params/housingRents", housingRents);
    setting.setValue("Params/careForTheElderly", careForTheElderly);
    setting.setValue("Params/cumulativeTaxableIncome_previousMonth", cumulativeTaxableIncome_previousMonth);
    setting.setValue("Params/cumulativeTaxPayable_previousMonth", cumulativeTaxPayable_previousMonth);
}

void MainWindow::loadParams()
{
    // 从ini读取参数
    QSettings setting(INI_PATH, QSettings::IniFormat);
    wage = setting.value("Params/wage", 10000).toDouble();
    otherDeduction = setting.value("Params/otherDeduction").toDouble();
    accumulationFund = setting.value("Params/accumulationFund").toDouble();
    medicalInsurance = setting.value("Params/medicalInsurance").toDouble();
    endowmentInsurance = setting.value("Params/endowmentInsurance").toDouble();
    unemploymentInsurance = setting.value("Params/unemploymentInsurance").toDouble();
    childrenEducation = setting.value("Params/childrenEducation").toDouble();
    continuingEducation = setting.value("Params/continuingEducation").toDouble();
    treatmentForSeriousDisease = setting.value("Params/treatmentForSeriousDisease").toDouble();
    homeLoanInterest = setting.value("Params/homeLoanInterest").toDouble();
    housingRents = setting.value("Params/housingRents").toDouble();
    careForTheElderly = setting.value("Params/careForTheElderly").toDouble();
    cumulativeTaxableIncome_previousMonth = setting.value("Params/cumulativeTaxableIncome_previousMonth").toDouble();
    cumulativeTaxPayable_previousMonth = setting.value("Params/cumulativeTaxPayable_previousMonth").toDouble();

    // 将参数更新至界面
    setParamsToUi();
}

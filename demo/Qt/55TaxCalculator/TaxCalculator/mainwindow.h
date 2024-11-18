#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_ok_clicked();

private:
    void setStatusBar();

    void getParamsFromUi(); // 获取界面上参数至变量
    void setParamsToUi(); // 设置变量至界面

    // 检查输入参数有效性
    bool isValid();

    // 根据累计应纳税所得额，获取税率和速算扣除数
    void getRateAndOffset(double cumulativeTaxableIncome, float& rate, int& offset);

    void saveParams(); // 保存参数至配置
    void loadParams(); // 从配置加载参数

private:
    Ui::MainWindow *ui;
    double wage; // 税前月工资
    double otherDeduction; // 其他扣除

    // 五险一金
    double accumulationFund; // 公积金
    double medicalInsurance; // 医疗保险
    double endowmentInsurance; // 养老保险
    double unemploymentInsurance; // 失业保险

    // 专项附加扣除
    double childrenEducation; // 子女教育
    double continuingEducation; // 继续教育
    double treatmentForSeriousDisease; // 大病医疗
    double homeLoanInterest; // 住房贷款利息
    double housingRents; // 住房租金
    double careForTheElderly; // 赡养老人

    // 提供上月数据
    double cumulativeTaxableIncome_previousMonth; // 累计应纳税所得额
    double cumulativeTaxPayable_previousMonth; // 累计应纳税额
};

#endif // MAINWINDOW_H

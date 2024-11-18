#ifndef DISPERSEDSELECT_H
#define DISPERSEDSELECT_H

#include "KLineEdit.h"
#include "TypeItemView.h"
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

// static const QString MATH_DISP_METHOD = QString("数值离散方法(WF)");
static const QString METHOD_IMPLICIT_TRAP_INTEGRAL =
        QObject::tr("Implicit Trapezoidal Integration Method");                // 隐式梯形积分法
static const QString METHOD_BACK_EULER = QObject::tr("Backward Euler Method"); // 后向欧拉法
static const QString METHOD_WEIGHT_INTEGRAL = QObject::tr("Weighted Numerical Integration Method"); // 权重数值积分法
static const QString WEIGHT_FACTOR = QObject::tr("Weighting Factor");                               // 权重因子

// 元件模型所选择的数值离散方法，隐式梯形积分法，值为：1；后向欧拉法，值为0；权重数值积分法，输入框（输入框名为“权重因子”）
class DispersedSelect : public CWidget
{
    Q_OBJECT

public:
    DispersedSelect(QWidget *parent = nullptr);
    ~DispersedSelect();
    void setSelectValue(const QString &value);
    void setMethodLabelName(const QString &strtext);
    /// <summary>
    /// 获取值对应的数值计算方法名称
    /// </summary>
    /// <param name="value">值</param>
    /// <returns>数值计算方法名称</returns>
    static const QString &getMethodNameByValue(const QString &value)
    {
        if ("0" == value) {
            return METHOD_BACK_EULER;
        } else if ("1" == value) {
            return METHOD_IMPLICIT_TRAP_INTEGRAL;
        } else {
            return METHOD_WEIGHT_INTEGRAL;
        }
    }
    QComboBox *getComboBox();
    QLineEdit *getCurrentLineEdit();

private:
    void initUI();
    void initLabel(QLabel *label);
    void initLineEdit(QLineEdit *lineEdit);
    void setLineEditValidator(const QString &regstr);
    void setLineEditValue(const QString &textstr);

public slots:
    void onBoxIndexChanged(const QString &textstr);

private:
    QComboBox *m_pComboBox;
    KLineEdit *m_pLineEdit;
    QLabel *m_mathDispMethod;
    int m_selectIndex;
};

#endif // DISPERSEDSELECT_H

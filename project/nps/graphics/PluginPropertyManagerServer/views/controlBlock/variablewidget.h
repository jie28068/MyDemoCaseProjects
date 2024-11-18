#ifndef VARIABLEWIDGET_H
#define VARIABLEWIDGET_H

#include "TypeItemView.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSet>
#include <QSharedPointer>

namespace Kcc {
namespace BlockDefinition {
class Model;
class Variable;
}
}
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

struct VarWidgetData {
    VarWidgetData(const QString &_displayName = "", const QString &_groupName = "",
                  const QStringList &_varList = QStringList(), QLineEdit *_lineEdit = nullptr)
        : displayName(_displayName), groupName(_groupName), varList(_varList), lineEdit(_lineEdit)
    {
    }
    QString displayName; // 显示名
    QString groupName;   // 组名
    QStringList varList; // 变量列表
    QLineEdit *lineEdit; // 组对应的控件
};
class VarLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    VarLineEdit(QWidget *parent) : QLineEdit(parent) { }

protected:
    void focusOutEvent(QFocusEvent *event) override;
};

// label和lineedit在一行的控件，初始化时可传入QMap<QString,QList<QString>>，根据多少对key-value追加多少行（前提：追加value有效的行）
class VariableWidget : public CWidget
{
    Q_OBJECT

public:
    VariableWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, QWidget *parent = nullptr);
    ~VariableWidget();

    bool saveData(QSharedPointer<Kcc::BlockDefinition::Model> model);

    virtual bool checkLegitimacy(QString &errorinfo) override;
    virtual void setCWidgetReadOnly(bool bReadOnly) override;

    void setVars(int index, const QList<QString> &varLst);

public slots:
    void onUpdate();

private:
    void initUI();
    void initLabel(QLabel *&label);
    void initLineEdit(QLineEdit *&lineEdit);
    bool checkVarSum(int maxSize);              // 检查变量个数
    bool checkVarnameValid(QString &errorinfo); // 检查名称合法性
    QStringList removeDumplicates(const QStringList &orgLst);
    // 将变量名后面的数字去掉
    QString splitVariable(const QString &name);
    /// @brief 添加UIwidget，一行QLabel+LineEdit
    /// @param gridlayout 显示名称
    /// @param displayName 显示名称
    /// @param groupName 组名（model中）
    /// @return
    bool addVarGroupWidget(QGridLayout *gridlayout, const QString &displayName, const QString &groupName);
    /// @brief 获取变量显示名称，有别名的显示别名，没有的显示
    /// @param grouplist
    /// @return
    QStringList getVarKeyList(const QList<QSharedPointer<Kcc::BlockDefinition::Variable>> &grouplist);

    // 获取除当前模块外的其它控制模块的参数名，用于参数名的更新
    QStringList getAllCtrlBlockParaNames(QSharedPointer<Kcc::BlockDefinition::Model> curBlock);
    /// @brief 去除特殊字符
    /// @param varName
    /// @return
    QString updateVarName(const QString &varName);

private:
    QSharedPointer<Kcc::BlockDefinition::Model> m_model;
    QList<VarWidgetData> m_allDataList; // 所有显示的数据
    QPushButton *m_pUpdateBtn;          // 更新按钮
    QStringList m_VarNames;             // 当前画板其他模块所有变量别名
    QStringList m_allGroupName;         // 所有组名
};

#endif // VARIABLEWIDGET_H

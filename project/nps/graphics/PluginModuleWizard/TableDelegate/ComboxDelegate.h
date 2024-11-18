#pragma once
#include "KCustomDialog.h"
#include "TableBaseDelegate.h"
#include <QGridLayout>
#include <QLabel>
class MatrixWidget;
class ComboxDelegate : public TableBaseDelegate
{
    Q_OBJECT

public:
    enum ParamType {
        ParamType_Other,   // 其他类型参数
        ParamType_SumSign, // 加法模块的Sign参数
        ParamType_AddSign, // 圆形加法模块的Sign参数
    };
    ComboxDelegate(QObject *parent);
    ~ComboxDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private:
    ComboxDelegate::ParamType getParamType(const QModelIndex &index) const;
    /// @brief 从矩阵类型字符串中获取矩阵的详细属性
    /// @param str
    /// @return 矩阵类型
    Kcc::BlockDefinition::MatrixInfo getMatrixDataFormString(const QString &str) const;

private slots:
    void onCurrentIndexChanged(int index);

private:
    mutable QStringList typeList;
    QStringList limitTypeList;
    mutable QString m_str; // 当前选择的下拉选项
};

class MatrixWidget : public QWidget
{
    Q_OBJECT
public:
    MatrixWidget(QWidget *parent = nullptr);
    ~MatrixWidget();
    QString typeConversion(const QString &str);
    QString getResultName();
    QString getRow() { return row == "-1" ? "X" : row; }
    QString getClounm() { return clounm == "-1" ? "X" : clounm; }
    QString getType() { return type; }

private slots:
    void onChangedText(const QString &str);

private:
    QComboBox *rowComboBox;
    QComboBox *cloumComboBox;
    QComboBox *typeComboBox;
    QLabel *resultLabel;
    QString row;
    QString clounm;
    QString type;
};
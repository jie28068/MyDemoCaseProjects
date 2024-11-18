#ifndef VARIABLEWIDGET_H
#define VARIABLEWIDGET_H

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

// label和lineedit在一行的控件，初始化时可传入QMap<QString,QList<QString>>，根据多少对key-value追加多少行（前提：追加value有效的行）
class VariableWidget : public QWidget
{
    Q_OBJECT

public:
    VariableWidget(const QMap<QString, QList<QString>> &variablemap, QWidget *parent = nullptr);
    ~VariableWidget();

private:
    void initUI(const QMap<QString, QList<QString>> &variablemap);
    void initLabel(QLabel *label);
    void initLineEdit(QLineEdit *lineEdit);
    QString mergeVariables(const QList<QString> &strlist);

private:
};

#endif // VARIABLEWIDGET_H

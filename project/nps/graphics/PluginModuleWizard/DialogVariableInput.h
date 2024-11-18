#ifndef DIALOGVARIABLEINPUT
#define DIALOGVARIABLEINPUT

#include "ui_DialogVariableInput.h"
#include <QDialog>
#include <QRegExpValidator>
#include <QWidget>

// 填写变量名称对话框
class DialogVariableInput : public QDialog
{
    Q_OBJECT
public:
    explicit DialogVariableInput(QWidget *parent = nullptr) : QDialog(parent)
    {
        ui.setupUi(this);
        ui.lineEdit->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z_][a-zA-Z0-9_]{1,32}$"), this));
        setWindowFlags(Qt::WindowCloseButtonHint);
        ui.okButton->setText(QObject::tr("OK"));
    }
    QString text() { return ui.lineEdit->text(); }

private:
    Ui::DialogVariableInput ui;
};

#endif

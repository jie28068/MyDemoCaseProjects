#ifndef HRMTOOL_H
#define HRMTOOL_H

#include <QWidget>
#include "serviceobject.h"
#include "FunctionWidgets/plancreator.h"
#include "managetoolclient.h"
#include <QButtonGroup>

namespace Ui {
class HRMTool;
}

class HRMTool :public QWidget, public ServiceObject
{
    Q_OBJECT

public:
    explicit HRMTool(QWidget *parent = nullptr);
    ~HRMTool();

private:


signals:
    void return_mainPage();

private slots:
    void on_btn_overview_clicked();

    void on_btn_tool_clicked();

    void on_btn_mainPage_clicked();

    void on_btn_resume_selection_clicked();

    void on_stackedWidget_currentChanged(int arg1);

private:
    Ui::HRMTool *ui;
    PlanCreator pc;
    QButtonGroup* btnGroup;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // HRMTOOL_H

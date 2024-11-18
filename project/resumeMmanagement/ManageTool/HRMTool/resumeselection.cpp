#include "resumeselection.h"
#include "ui_resumeselection.h"

ResumeSelection::ResumeSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResumeSelection)
{
    ui->setupUi(this);
    mapBtnWidget.insert(ui->btn_new_selection,ui->page_new_selection);
    mapBtnWidget.insert(ui->btn_cur_selection,ui->page_selection);
    mapBtnWidget.insert(ui->btn_complete_selection,ui->page_selection_complete);

    btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn_new_selection, 0);
    btnGroup->addButton(ui->btn_cur_selection, 1);
    btnGroup->addButton(ui->btn_complete_selection, 2);

    ui->frame->hide();
}

ResumeSelection::~ResumeSelection()
{
    delete ui;
}

void ResumeSelection::display(QString strDepartment)
{
    display(departMentMap(strDepartment));
}

void ResumeSelection::display(Department department)
{
    switch (department) {
    case DP_PERSON:
        ui->btn_new_selection->setVisible(true);
        ui->btn_cur_selection->setVisible(false);
        ui->btn_complete_selection->setVisible(false);
        ui->stackedWidget->setCurrentWidget(ui->page_new_selection);
        break;
    case DP_OTHER:
        ui->btn_new_selection->setVisible(false);
        ui->btn_cur_selection->setVisible(true);
        ui->btn_complete_selection->setVisible(false);
        ui->stackedWidget->setCurrentWidget(ui->page_selection);
        break;
    default:
        ui->btn_new_selection->setVisible(true);
        ui->btn_cur_selection->setVisible(false);
        ui->btn_complete_selection->setVisible(false);
        ui->stackedWidget->setCurrentWidget(ui->page_new_selection);
        break;
    }
//    initDisp();
}

void ResumeSelection::on_btn_cur_selection_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_selection);
}


void ResumeSelection::on_btn_new_selection_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_new_selection);
}


void ResumeSelection::on_btn_complete_selection_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_selection_complete);
}

void ResumeSelection::createFilterPlan(QStringList idList)
{
    ui->page_new_selection->planCreateStart(PlanCreator::PLAN_FILTER, idList);
}

ResumeSelection::Department ResumeSelection::departMentMap(QString strdpmt)
{
    if(strdpmt=="人事部"){
        return Department::DP_PERSON;
    }else{
        return Department::DP_OTHER;
    }
}

void ResumeSelection::initDisp()
{
    QMapIterator<QPushButton*,QWidget*> i(mapBtnWidget);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        if(i.key()->isVisible()){
            i.key()->setChecked(true);
            ui->stackedWidget->setCurrentWidget(i.value());
            break;
        }
    }

}

void ResumeSelection::showEvent(QShowEvent *event)
{
    UserModel* userinfo = ManageToolClient::instance().getUserInfo();
    display(userinfo->department());
    QWidget::showEvent(event);
}

void ResumeSelection::on_stackedWidget_currentChanged(int arg1)
{
    btnGroup->button(arg1)->setChecked(true);
}


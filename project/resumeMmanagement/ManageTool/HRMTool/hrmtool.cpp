#include "hrmtool.h"
#include "ui_hrmtool.h"
#include <QDebug>

#include "managetoolclient.h"

#define RETURN_FAIL   "-1"
#define RETURN_SUCCEED   "0"

HRMTool::HRMTool(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::HRMTool)
{
    ui->setupUi(this);

    btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn_overview, 0);
    btnGroup->addButton(ui->btn_tool, 1);
    btnGroup->addButton(ui->btn_resume_selection, 2);

    ui->stackedWidget->setCurrentIndex(0);
}

HRMTool::~HRMTool()
{
    delete ui;
}

void HRMTool::on_btn_overview_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_archive_preview);
}

void HRMTool::on_btn_tool_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_upload_resume);
}

void HRMTool::on_btn_mainPage_clicked()
{
    emit return_mainPage();    
}


void HRMTool::on_btn_resume_selection_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_resume_selection);
    QStringList planList = ui->page_archive_preview->getPlanList();
    ui->page_resume_selection->createFilterPlan(planList);
}

void HRMTool::showEvent(QShowEvent *event)
{
    UserModel* userinfo = ManageToolClient::instance().getUserInfo();
    if(userinfo->department() == "人事部")
        ui->stackedWidget->setCurrentWidget(ui->page_archive_preview);
    else
        ui->stackedWidget->setCurrentWidget(ui->page_resume_selection);

    QWidget::showEvent(event);
}


void HRMTool::on_stackedWidget_currentChanged(int arg1)
{
    btnGroup->button(arg1)->setChecked(true);
}


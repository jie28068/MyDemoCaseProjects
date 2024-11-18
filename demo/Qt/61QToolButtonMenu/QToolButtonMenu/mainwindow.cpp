#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolButton>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createToolButton();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createToolButton()
{
    // 创建QToolButton
    QToolButton *toolButton = new QToolButton(this);
    toolButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    toolButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    // 创建QMenu
    QMenu *menu = new QMenu();
    menu->addAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical), tr("图标视图"));
    menu->addAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion), tr("细节视图"));

    // 为QToolButton设置菜单
    toolButton->setMenu(menu);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    bool ret = rtspServer.pushDesktop("127.0.0.1", 8554);
    if (ret)
    {
        QMessageBox::information(nullptr, "Info", "Push streaming desktop successfully");
    }
    else
    {
        QMessageBox::information(nullptr, "Info", "Failed to push streaming desktop");
    }
    ui->pushButton->setEnabled(false);
}

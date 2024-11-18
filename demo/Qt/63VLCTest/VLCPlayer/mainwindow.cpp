#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player.setMedia(qApp->applicationDirPath() + "/video.mp4");
    player.setVideoOutput(ui->label_video->winId());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_play_clicked()
{
    player.play();
}

void MainWindow::on_pushButton_pause_clicked()
{
    player.pause();
}

void MainWindow::on_pushButton_resume_clicked()
{
    player.resume();
}

void MainWindow::on_pushButton_stop_clicked()
{
    player.stop();
}

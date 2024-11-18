/*
 * 需要先安装LAVFilters解码器，此代码方可正常运行。
 * 下载地址：https://github.com/Nevcairiel/LAVFilters/releases
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 布局界面
    videoWidget = new QVideoWidget();
    delete centralWidget();
    this->setCentralWidget(videoWidget);

    // 设置视频文件，并播放程序所在目录下的video.mp4
    player = new QMediaPlayer;
    player->setVideoOutput(videoWidget);
    player->setMedia(QUrl::fromLocalFile(qApp->applicationDirPath() + "/video.mp4"));
    player->play();
}

MainWindow::~MainWindow()
{
    delete player;
    player = nullptr;

    delete videoWidget;
    videoWidget = nullptr;

    delete ui;
}

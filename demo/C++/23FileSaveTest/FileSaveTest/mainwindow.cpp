#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUuid>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);

    // 生成256KB测试数据，因为每次推送数据不能大于BLOCKSIZE=512KB
    createTestData(data, 256 * 1024);
    fileSave = new FileSave();

    // 每隔1s推送256KB数据，用于模拟真实接收数据环境
    startTimer(1000);
}

MainWindow::~MainWindow()
{
    delete fileSave;
    fileSave = nullptr;

    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    fileSave->pushPacket((unsigned char*)data.data(), data.length());
}

void MainWindow::createTestData(QByteArray& data, int size)
{
    data.resize(size);
    for (int i = 0; i < size; i++)
    {
        data[i] = i % 128;
    }
}

void MainWindow::on_pushButton_start_clicked()
{
    QString fileName = QUuid::createUuid().toString();
    QString filePath = qApp->applicationDirPath() + "/" + fileName + ".data";
    fileSave->Start(filePath.toStdString().c_str());

    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
}

void MainWindow::on_pushButton_stop_clicked()
{
    fileSave->Stop();

    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include "FileSave.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();

private:
    void createTestData(QByteArray& data, int size);

private:
    Ui::MainWindow *ui;
    QByteArray data;
    FileSave* fileSave;
};

#endif // MAINWINDOW_H

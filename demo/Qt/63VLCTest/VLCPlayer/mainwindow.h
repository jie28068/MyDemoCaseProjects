#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VLCPlayer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_play_clicked();
    void on_pushButton_pause_clicked();
    void on_pushButton_resume_clicked();
    void on_pushButton_stop_clicked();

private:
    Ui::MainWindow *ui;
    VLCPlayer player;
};

#endif // MAINWINDOW_H

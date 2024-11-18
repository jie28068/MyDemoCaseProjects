#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Platform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void appendLog(const QString& log);

    static void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private slots:
    void on_pushButton_openFile_clicked();
    void on_pushButton_build_clicked();
    void comboBox_platform_currentIndexChanged(const QString &text);

private:
    bool buildOpenCLProgram(const QString &clSourceFileName, const cl_context &context,
                            cl_device_id deviceID, cl_program &program);

private:
    Ui::MainWindow *ui;
    Platform* platform;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

#define CHECK_OPENCL_ERROR(actual, msg) \
        if(actual != CL_SUCCESS) \
        { \
            qDebug() << "Error: " << msg << " Error code : " << actual; \
            return; \
        }

static MainWindow* logWidget = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    logWidget = this;
    qInstallMessageHandler(logOutput);

    // 获取当前系统上支持的平台，将这些平台名称添加到comboBox_platform中
    platform = new Platform();
    QStringList platformNames = platform->getPlatformNames();
    ui->comboBox_platform->addItems(platformNames);
    ui->comboBox_platform->setCurrentIndex(0);

    // 获取选择平台下，所有GPU设备名称
    QString curPlatform = ui->comboBox_platform->currentText();
    QStringList deviceNames = platform->getGPUDeviceNames(curPlatform);
    ui->comboBox_device->addItems(deviceNames);
    ui->comboBox_device->setCurrentIndex(0);
    connect(ui->comboBox_platform, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::comboBox_platform_currentIndexChanged);
}

MainWindow::~MainWindow()
{
    delete platform;
    delete ui;
}

void MainWindow::appendLog(const QString &log)
{
    ui->textEdit_buildInfo->append(log);
}

void MainWindow::logOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (logWidget)
    {
        logWidget->appendLog(msg);
    }
}

void MainWindow::on_pushButton_openFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Kernel File", QString(), "Kernel Files(*.cl)");
    if (fileName.isEmpty())
    {
        return;
    }
    ui->lineEdit_kernelFile->setText(fileName);
}

void MainWindow::on_pushButton_build_clicked()
{
    ui->textEdit_buildInfo->clear();
    qDebug() << "building...";

    // 获取选择设备的deviceID
    QString platformName = ui->comboBox_platform->currentText();
    QString deviceName = ui->comboBox_device->currentText();
    Platform platform;
    cl_device_id deviceID = platform.getGPUDeviceID(platformName, deviceName);

    // 创建设备的上下文Context
    cl_int status = 0;
    cl_context context = clCreateContext(0, 1, &deviceID, NULL, NULL, &status);
    CHECK_OPENCL_ERROR(status, "clCreateContext failed");

    // 编译program
    cl_program program;
    bool ret = buildOpenCLProgram(ui->lineEdit_kernelFile->text(), context, deviceID, program);
    if (ret)
    {
        qDebug() << "build" << ui->lineEdit_kernelFile->text() << "is successful";
        clReleaseProgram(program);
    }
    clReleaseContext(context);
}

void MainWindow::comboBox_platform_currentIndexChanged(const QString &text)
{
    QStringList deviceNames = platform->getGPUDeviceNames(text);
    ui->comboBox_device->clear();
    ui->comboBox_device->addItems(deviceNames);
}

/**
 * @brief MainWindow::buildOpenCLProgram
 * 编译*.cl源文件，生成cl_program并返回。
 * 实际可编译多个cl文件为一个cl_program，这里只用到一个cl文件，故只编译一个即可。
 * @param clSourceFileNames *.cl文件名称
 * @param context           设备上下文
 * @param deviceID          设备ID
 * @param program           输出编译生成的cl_program
 * @return 返回编译结果
 */
bool MainWindow::buildOpenCLProgram(const QString &clSourceFileName, const cl_context &context, cl_device_id deviceID, cl_program& program)
{
    // 读取cl源文件
    QFile file(clSourceFileName);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "Error: file open failed : " << clSourceFileName;
        return false;
    }
    QByteArray source = file.readAll();
    file.close();

    // 创建Program
    cl_int status = 0;
    const char* data = source.data();
    size_t size = source.size();
    program = clCreateProgramWithSource(context, 1, &data, &size, &status);

    // 为所有指定的设备创建一个cl程序可执行文件
    std::string str = ui->lineEdit_buildOptions->text().toStdString();
    const char* options = str.c_str();
    status = clBuildProgram(program, 1, &deviceID, options, NULL, NULL); //"-x clc++"
    if(status != CL_SUCCESS)
    {
        if(status == CL_BUILD_PROGRAM_FAILURE)
        {
            // 获取buildLog长度
            size_t buildLogSize = 0;
            cl_int logStatus = clGetProgramBuildInfo (
                            program,
                            deviceID,
                            CL_PROGRAM_BUILD_LOG,
                            buildLogSize,
                            nullptr,
                            &buildLogSize);

            // 获取buildLog
            char* buildLog = (char*)malloc(buildLogSize);
            memset(buildLog, 0, buildLogSize);
            logStatus = clGetProgramBuildInfo (
                            program,
                            deviceID,
                            CL_PROGRAM_BUILD_LOG,
                            buildLogSize,
                            buildLog,
                            nullptr);

            // 打印buildLog
            qDebug() << buildLog;
            free(buildLog);
            return false;
        }
        qDebug() << "cl file build failed : " << clSourceFileName;
        return false;
    }
    return true;
}

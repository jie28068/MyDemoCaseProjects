#include "ExportDrawingboardToFMU.h"
#include "CoreLib/ServerManager.h"
#include "server/CodeManagerServer/ICodeManagerServer.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegExpValidator>

USE_LOGOUT_("ExportDrawingboardToFMU")

using namespace Kcc::CodeManager;

static const QRegExp REG_STRING("^[a-zA-Z_]([a-zA-Z0-9_])*$");

ExportDrawingboardToFMU::ExportDrawingboardToFMU(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    initUI();
}

ExportDrawingboardToFMU::~ExportDrawingboardToFMU() { }

void ExportDrawingboardToFMU::initUI()
{
    connect(ui.exportPath_btn, SIGNAL(clicked(bool)), this, SLOT(onExportPathBtnClicked(bool)));
    ui.modelName->setValidator(new QRegExpValidator(REG_STRING, this));
    ui.modelType->addItem(QString("CoSimulation"));
    ui.FMIVersion->addItem(QString("2"));
}

void ExportDrawingboardToFMU::onExportPathBtnClicked(bool checked)
{
    QString path = QFileDialog::getSaveFileName(this, tr("fmu export path"), "", tr("*.fmu")); // FMU导出路径
    ui.exportPath->setText(path);
}

bool ExportDrawingboardToFMU::getFmiParams(FmiParam &params)
{
    QString fmiPath = ui.exportPath->text();
    QFileInfo fileInfo(fmiPath);
    if (fmiPath.isEmpty() || !fileInfo.absoluteDir().exists()) {
        LOGOUT(tr("the export file path does not exist"), LOG_ERROR); // 导出文件路径不存在
        return false;
    }
    QString modelName = ui.modelName->text();
    if (modelName.isEmpty()) {
        LOGOUT(tr("model name cannot be empty"), LOG_ERROR); // 模型名称不能为空
        return false;
    }
    params.fmuPath = fmiPath.toStdString();
    params.author = ui.modelAutor->text().toStdString();
    params.modelName = modelName.toStdString();
    params.type = ui.modelType->currentText().toStdString();
    params.version = ui.FMIVersion->currentText().toInt();
    params.includeSources = ui.addCode->isChecked();
    return true;
}

#ifndef EXPORTDRAWINGBOARDTOFMU_H
#define EXPORTDRAWINGBOARDTOFMU_H

#include "ui_ExportDrawingboardToFMU.h"
#include <QWidget>

namespace Kcc {
namespace CodeManager {
struct FmiParam;
}
}

class ExportDrawingboardToFMU : public QWidget
{
    Q_OBJECT

public:
    ExportDrawingboardToFMU(QWidget *parent = 0);
    ~ExportDrawingboardToFMU();

    bool getFmiParams(Kcc::CodeManager::FmiParam &params);

private:
    void initUI();

private slots:
    void onExportPathBtnClicked(bool checked);

private:
    Ui::ExportDrawingboardToFMU ui;
};

#endif // EXPORTDRAWINGBOARDTOFMU_H

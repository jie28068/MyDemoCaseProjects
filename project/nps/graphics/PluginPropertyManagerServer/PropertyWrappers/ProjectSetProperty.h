#pragma once

#include "CommonWrapper.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "KLineEdit.h"
#include "SimuParamWidget.h"
#include "server/SimulationCore/ISimuInstance.h"
#include <QTextEdit>

// using namespace Kcc::DataManager;
using namespace Kcc::BlockDefinition;
using namespace Kcc::SimulationManager;
class FileSaveConfigWidget;
class BaseInfoWidget;
class ProjectSetProperty : public CommonWrapper
{
public:
    ProjectSetProperty(int NpsOrCad);
    virtual ~ProjectSetProperty();

    // 名称，展示在标题栏
    virtual QString getTitle() override;

    // 弹出Dialog并得到交互结果时调用此接口
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;

    // 在窗口点击OK时，判断弹窗中的值是否符合要求，不符合返回具体信息errorinfo
    virtual bool checkValue(QString &errorinfo = QString()) override;

private:
    void init();
    SimuParamWidget::SimulationType m_NpsOrCad;

    FileSaveConfigWidget *m_pFileSaveConfigWidget;
    BaseInfoWidget *m_baseinfoWidget;
};

// 项目的文件保存设置
class FileSaveConfigWidget : public QWidget
{
public:
    /// @brief 与仿真引擎无关的配置参数
    struct SimuCommonConfig {
        int m_fileSize;       // 保存文件大小
        int m_dataPrecision;  // 数据保存精度
        QString m_fileSuffix; // 文件后缀类型
        SimuCommonConfig() : m_dataPrecision(15), m_fileSuffix(".csv") { }
    };
    FileSaveConfigWidget(QWidget *parent = nullptr) : QWidget(parent) { initUI(); }

    void getParams(SimuCommonConfig &simuCom);
    void setParams(const SimuCommonConfig &simuCom);
    void setPythPath(const QString &path);
    QString getPythonPath();
    bool checkValue(QString &errorinfo);

private slots:
    void onSelectButtonClicked();
    void onCheckBox(int checked);

private:
    void initUI();

    QSpinBox *m_spin_precision;
    QSpinBox *m_spin_fileSize;
    QComboBox *m_combo_suffix;
    QCheckBox *m_checkbox;
    KLineEdit *m_pathedit;
    QPushButton *m_selectpath;
};

class BaseInfoWidget : public QWidget
{
private:
    void initUI();

    QTextEdit *m_textEdit;
    QCheckBox *m_checkbox;

public:
    BaseInfoWidget(QWidget *parent = nullptr) : QWidget(parent) { initUI(); };

    QString getDescription() { return m_textEdit->toPlainText(); };
    void setDescription(const QString &str) { m_textEdit->setText(str); };
    bool showSimuSort() { return m_checkbox->isChecked(); };
    void setShowSimuSort(bool checked) { m_checkbox->setChecked(checked); };
};

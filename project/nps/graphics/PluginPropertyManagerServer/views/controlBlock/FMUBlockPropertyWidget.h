#ifndef FMUBLOCKPROPERTY_H
#define FMUBLOCKPROPERTY_H

#include <QLabel>
#include <QPushButton>

#include "KLineEdit.h"
#include "TypeItemView.h"
#include "blockproperty.h"
#include "customlineedit.h"
#include "customtablewidget.h"

static const QString FMUPATH = "fmuPath";
static const QString FMUDESCRIPTION = "description";
static const QString PROTOTYPENAME_FMU = "FMU";
static const QStringList listFMUInfoKeys = QStringList() << "modelName"
                                                         << "fmiType"
                                                         << "fmiVersion"
                                                         << "fmuPath"
                                                         << "numberOfContinuousStates"
                                                         << "numberOfEventIndicators"
                                                         << "generationDateAndTime"
                                                         << "generationTool"
                                                         << "description";

namespace Kcc {
namespace BlockDefinition {
class Model;
class ControlBlock;
class VariableGroup;
}
}
class FMUBlockPropertyWidget : public CWidget
{
    Q_OBJECT

public:
    FMUBlockPropertyWidget(QSharedPointer<Kcc::BlockDefinition::ControlBlock> pControlBlock, QWidget *parent = nullptr);
    ~FMUBlockPropertyWidget();

    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    bool checkValueChanged();
    bool saveData(QSharedPointer<Kcc::BlockDefinition::Model> model);
signals:
    void onFMUBlockChanged(QSharedPointer<Kcc::BlockDefinition::Model>);

private:
    void initUI();
    QList<CustomModelItem> getModelItem(QSharedPointer<Kcc::BlockDefinition::VariableGroup> &fmuParaGroup,
                                        QStringList &listheader, bool bparam = false, bool bReadOnly = false);

    bool isFileExist(QString fullFileName);

private slots:
    void onBlockFileLoading();

private:
    QSharedPointer<Kcc::BlockDefinition::ControlBlock> m_pControlBlock;
    CustomTableWidget *m_pFMUInfoWidget;

    QLabel *m_pLabelFilePath;
    QPushButton *m_pathPushButton;
    QLabel *m_pLabelDescription;
    CustomLineEdit *m_pLineEditFilePath;
    QTextEdit *m_pTextEditDescription;

    QString m_strFilePathFMUOld;
    bool m_bValueChanged;
};

#endif // FMUBLOCKPROPERTY_H

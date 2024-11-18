#ifndef CONTROLPARAMETERWIDGET_H
#define CONTROLPARAMETERWIDGET_H

#pragma once
#include "customtablewidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>

namespace Kcc {
namespace BlockDefinition {
class Model;
}
}

class ControlParameterWidget : public CWidget
{
    Q_OBJECT
public:
    ControlParameterWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, QWidget *parent = nullptr);
    ~ControlParameterWidget();

    QList<CustomModelItem> getNewListData();
    void updateCustomTableWidget();
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    virtual bool checkLegitimacy(QString &errorinfo) override;

private slots:
    void onShowSpecifiedModel(const QUrl &url);

private:
    void InitUI();
    void initData();

    QStringList initCorrespondModelsData(QSharedPointer<Kcc::BlockDefinition::Model> drawBoard, QString boardPath);

    QSharedPointer<Kcc::BlockDefinition::Model> getSubSystemModel(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                                  const QStringList &pathList, const int &index);

    QStringList getSubSystemCorrespondData(QSharedPointer<Kcc::BlockDefinition::Model> drawBoard, QString boardPath);

    QStringList getParentBoardCorrespondData(QSharedPointer<Kcc::BlockDefinition::Model> drawBoard, QString boardPath);

    QString getParentBoardPath(QSharedPointer<Kcc::BlockDefinition::Model> model);

    QSharedPointer<Kcc::BlockDefinition::Model> getTopParentBoard(QSharedPointer<Kcc::BlockDefinition::Model> model);

private:
    CustomTableWidget *m_tableWidget;
    QListWidget *m_pModelNameListWidget;
    QSharedPointer<Kcc::BlockDefinition::Model> m_pModel;
    QSharedPointer<Kcc::BlockDefinition::Model> m_pCurrentDrawBoard;
};

#endif
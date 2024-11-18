#ifndef FEATURECURVEWIDGET_H
#define FEATURECURVEWIDGET_H

#pragma once
#include "GraphWidget.h"
#include "TypeItemView.h"
#include "customtablewidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QSharedPointer>
#include <QSpacerItem>

namespace FCKEY {
static const QString KEYWORD_T = "T";
static const QString KEYWORD_S = "S";
static const QString DISPLAYNAME_T = QObject::tr("Tempeture(deg.C)");               // 温度T (deg.C)
static const QString DISPLAYNAME_S = QObject::tr("Strength of Illumination(W/m2)"); // 光照强度S (W/m2)
}

namespace Kcc {
namespace BlockDefinition {
class ElectricalBlock;
class DeviceModel;
}
}
class FeatureCurveWidget : public CWidget
{
    Q_OBJECT
public:
    FeatureCurveWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> elecblock, QWidget *parent = nullptr);
    ~FeatureCurveWidget();
    virtual bool saveData() override;
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    virtual bool checkLegitimacy(QString &errorinfo) override;

public slots:
    void updateGraphWidget(QSharedPointer<Kcc::BlockDefinition::DeviceModel> deviceModel);
    void onTableDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);

private:
    void InitUI();
    bool saveVariableData(const QString &keyword, const QString &variableType);

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_electricalBlock;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_currentDevicetype;
    CustomTableWidget *m_tableWidget;
    QLabel *m_tipsLabel;
    GraphWidget *m_UIGraphWidget;
    GraphWidget *m_UPGraphWidget;
};

#endif
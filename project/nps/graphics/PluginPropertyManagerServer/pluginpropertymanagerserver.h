#ifndef PLUGINPROPERTYMANAGERSERVER_H
#define PLUGINPROPERTYMANAGERSERVER_H

#include "CoreLib/Module.h"
#include "CurrentSimuWidget.h"
#include "FaultSimuWidget.h"
#include "KLWidgets/KCustomDialog.h"
#include "ServerBase.h"
#include "powerflowsetwidget.h"
#include "server/UILayout/ICoreMainUIServer.h"
#include <QAction>
#include <QWidget>

#define SERVER_VERSION 1
namespace Kcc {
namespace BlockDefinition {
class Model;
class VariableGroup;
class Variable;
class DeviceModel;
class ControlBlock;
class ElectricalBlock;
class SlotBlock;
class DrawingBoardClass;
class ElecBoardModel;
class CombineBoardModel;
class ControlBoardModel;
class ComplexBoardModel;
class ElectricalContainerBlock;
}
}

using Kcc::NotifyStruct;

class PropertyManagerServer;

class PluginPropertyManagerServer : public QWidget, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginPropertyManagerServer(QString strName);
    ~PluginPropertyManagerServer();

    void init(KeyValueMap &params) override;
    void unInit(KeyValueMap &saveParams) override;

public slots:
    void onRecieveGraphiceServerManagerMsg(unsigned int code, const NotifyStruct &param);
    void onRecieveSimulationServerManagerMsg(unsigned int code, const NotifyStruct &param);
private slots:
    void onOpenCurrentVariables();
    void onPowerFlowCalcSet();
    void onImplementclicked();
    void onFaultSimuSet();
    // void onVarOKclicked();
private:
    void updatePowerFlowParam(const int &basemva, const int &frequency,
                              QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pdrawboard);
    int getAngleValue(const QString &prototypename, QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype);
    QString getConnectionStr(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype, const QString &keyword);
    double getPFX(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model, const int &baseMVA, const int &frequency,
                  const double &LS);
    double getHFBUSVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model);
    void getShuntFilter(QSharedPointer<Kcc::BlockDefinition::Model> model, const int &frequency, double &pvalue,
                        double &qvalue);
    void getLoadPQ(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype, const int &frequency, double &pvalue,
                   double &qvalue);
    double getICRZBValue(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model, const int &baseMVA);

    void getLineRXB(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> elecModel,
                    QSharedPointer<Kcc::BlockDefinition::DeviceModel> devmodel, const int &basemva,
                    const int &frequency, QString &r, QString &x, QString &b);

    // double getRatio(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock,
    //                 QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype);
    // double getBusBarVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock);

private:
    PropertyManagerServer *propertyManagerServer;
    QAction *m_openCurrentVariables;
    QAction *m_powerFlowCalcSet;
    QAction *m_faultSimuAction;
    // 当前仿真窗口设置为成员变量 保持声明周期
    KCustomDialog *m_pPowerFlowCalcSetDialog;
    PowerFlowSetWidget *m_pPowerFlowCalcSetWidget;
    QString m_projectName;
};

#endif // PLUGINPROPERTYMANAGERSERVER_H

#pragma once

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "SimuNPSBusinessHooksServer.h"

// 电气画板业务钩子服务类
class ElectricalCanvasHooksServer : public SimuNPSBusinessHooksServer
{
public:
    ElectricalCanvasHooksServer(QSharedPointer<CanvasContext> pcanvascontext,
                                QSharedPointer<Kcc::BlockDefinition::Model> model);

    virtual bool enableCreate(int cpsourcetype, QMap<QString, QSharedPointer<SourceProxy>> cpsourcemap);
    virtual bool enableCreate(int cpsourcetype, QSharedPointer<SourceProxy> cpsource);
    virtual void setCreatePortContext(QSharedPointer<PortContext> portCtx);
    virtual double getElectricalBlockVoltage(QSharedPointer<SourceProxy> source);
    virtual QString getElectricalBlockPhase(QSharedPointer<SourceProxy> source);
    virtual int getElectricalPhaseNumber(QSharedPointer<SourceProxy> source);
    virtual bool isBlockCtrlCombined(QSharedPointer<SourceProxy> source);
    /// @brief 校验画板状态
    virtual void verifyCanvasStatus();
    virtual bool checkNameValid(const QString &nameString);
    virtual void showWaringDialg(const QString &tips);
    virtual void showPropertyDialog();
    virtual void changeTextNameHint(const QString &oldname, const QString &newname);
    virtual QList<showDataStruct> getPowerFlowData(QSharedPointer<SourceProxy> source);
    virtual int getBusBarPortNumber(QSharedPointer<SourceProxy> source);
    virtual void generatePecadCase();
    virtual void copyModelVariableGroups(PSourceProxy srcSource, PSourceProxy dstSource);
    virtual QColor GetBusbarVoltageLevelColor(const double &voltageLevel);

    /// @brief 获取电气元件端口的类型(ac/dc)
    virtual QString getElectricalPortType(QSharedPointer<SourceProxy> source, QString portUUID);

    /// @brief 设置电气元件(母线)  "AC" 参数  0:dc 1:ac
    virtual void setElectricalACDCType(QSharedPointer<SourceProxy> source, int type);

    virtual bool isBlockCtrlCodeType(QSharedPointer<SourceProxy> source);

    virtual void copyModelAssociationRelation(const QString &boardUUID, const QString &srcBlockUUID,
                                              const QString &dstBlockUUID);
    virtual bool isElecCombineModel(const QString &blockUUID);

    virtual bool checkBlockState(QSharedPointer<SourceProxy> source);

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> getBlock(QSharedPointer<SourceProxy> source);
    // QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> getDrawingBoard(const QString &uuid);
};

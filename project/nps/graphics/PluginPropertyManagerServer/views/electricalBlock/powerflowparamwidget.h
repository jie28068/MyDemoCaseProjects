#ifndef POWERFLOWPARAMWIDGET_H
#define POWERFLOWPARAMWIDGET_H

#include "customtablewidget.h"

namespace Kcc {
namespace BlockDefinition {
class DeviceModel;
class ElectricalBlock;
}
}
class PowerFlowParamWidget : public CustomTableWidget
{
    Q_OBJECT

public:
    PowerFlowParamWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock,
                         const QList<CustomModelItem> &paramlist, QWidget *parent = nullptr);
    ~PowerFlowParamWidget();
    void updatePFList(const QList<CustomModelItem> &paramlist,
                      QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype =
                              QSharedPointer<Kcc::BlockDefinition::DeviceModel>(nullptr));
    bool saveData();
public slots:
    void onDeviceTypeChanged(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype);
    void onParamChanged(const QList<CustomModelItem> &paramlist);

private:
    QList<CustomModelItem> getTideList(const QList<CustomModelItem> &paramlist = QList<CustomModelItem>(),
                                       bool bfromout = false,
                                       QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype =
                                               QSharedPointer<Kcc::BlockDefinition::DeviceModel>(nullptr));
    int getCurrentProjBaseVMA();
    int getCurrentProjFrequency();
    int getAngleValue(const QString &prototypename, QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype);
    QString getConnectionStr(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype, const QString &keyword);
    void setItemValue(QList<CustomModelItem> &pflist, const QString &keyword, const QVariant &value);
    // double getRatio(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock,
    //                 QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype);
    // double getBusBarVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock);
    void getShuntFilter(const QList<CustomModelItem> paramlist, double &pvalue, double &qvalue, bool bfromout);
    /// @brief 获取实际值
    /// @param paramlist 电气参数
    /// @param keyword 关键字
    /// @param bfromout 从外部则使用paramlist里面的值，否则到model里面找
    /// @param groupName 组名
    /// @return
    QVariant getKeyValue(const QList<CustomModelItem> &paramlist, const QString &keyword, bool bfromout = false,
                         const QString &groupName = "");
    /// @brief 获取负载的PQ值
    /// @param devicetype 设备类型
    /// @param pvalue
    /// @param qvalue
    void getLoadPQ(QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicetype, double &pvalue, double &qvalue);
    /// @brief 获取标幺值（电容、电感、电阻）
    /// @param model
    /// @return
    double getICRZBValue(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model);
    /// @brief 获取电压表电抗x
    /// @param model
    /// @param LS 漏感
    /// @return
    double getPFX(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model, const double &LS);
    /// @brief 获取高压侧电压
    /// @param model
    /// @return
    double getHFBUSVoltage(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> model);
    /// @brief 获取线路的rxb
    /// @param elecModel
    /// @param devmodel
    /// @param r
    /// @param x
    /// @param b
    void getLineRXB(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> elecModel,
                    QSharedPointer<Kcc::BlockDefinition::DeviceModel> devmodel, const double &lineLength, QString &r,
                    QString &x, QString &b);

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElecBlock;
    QList<CustomModelItem> m_oldList;
    QList<CustomModelItem> m_curParamList;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_tmpDeviceModel;
};

#endif // POWERFLOWPARAMWIDGET_H

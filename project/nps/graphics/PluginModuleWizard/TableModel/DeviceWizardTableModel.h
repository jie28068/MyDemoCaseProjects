#pragma once
#include "CustomProxyModel.h"
#include "WizardTableAbstractModel.h"
/// 电气设备类型代理模型
struct DeviceTableItem {
    DeviceTableItem(const QString &_strName, const QString &_type, const QString &_range, const QString &_nameCHS,
                    const QString &_value, const int _order, const QString &_unit, const int _mode,
                    const QString &controltype, const QVariant &controlValue)
        : strName(_strName),
          type(_type),
          range(_range),
          nameCHS(_nameCHS),
          nvalue(_value),
          order(_order),
          unit(_unit),
          nModeType(_mode),
          nControlTyep(controltype),
          nControlValue(controlValue)

    {
    }
    QString strName;        // 名称（key）
    QString type;           // 变量类型
    QString range;          // 参数范围
    QString nameCHS;        // 中文名称
    QString nvalue;         // 参数值
    int order;              // 累加计算位
    QString unit;           // 单位
    QString nControlTyep;   // 控件类型
    QVariant nControlValue; // 控件值
    int nModeType;          // 可见类型
};

class DeviceWizardTableModel : public WizardTableAbstractModel<DeviceTableItem>
{
public:
    enum Column { // 列
        ParamName = 0,
        ParamType,
        ParamRange,
        ParamNameCHS,
        ParamValue,
        ParamUnit,
        ParamMode,
        ControlTyep,
        ControlValue,
    };
    DeviceWizardTableModel(QObject *parent = nullptr);
    void addRow(const QString &_strName, const QString &_type, const QString &_range, const QString &_nameCHS,
                const QString &_value, const int _order, const QString &_unit, const int _mode,
                const QString &controltype, const QVariant &controlValue);
    /// @brief 获取指定数据
    /// @param strName
    /// @param type
    /// @return
    QVariant getParameter(const QString &strName, DeviceWizardTableModel::Column type);
    /// @brief 行数
    /// @param parent
    /// @return
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /// @brief 列数
    /// @param parent
    /// @return
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /// @brief 获取数据
    /// @param index
    /// @param role
    /// @return
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /// @brief 单元格操作
    /// @param index
    /// @return
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    /// @brief 设置数据
    /// @param index
    /// @param value
    /// @param role
    /// @return
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    /// @brief 表头数据
    /// @param section
    /// @param orientation
    /// @param role
    /// @return
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

class DveiceTableProxyModel : public CustomProxyModel
{
public:
    DveiceTableProxyModel(QObject *parent = nullptr) : CustomProxyModel(parent) { }
};
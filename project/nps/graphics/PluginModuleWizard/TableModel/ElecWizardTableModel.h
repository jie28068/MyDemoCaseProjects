#pragma once

#include "CustomProxyModel.h"
#include "WizardTableAbstractModel.h"

/// @brief 电气参数列表
struct ElecTableItem {
    ElecTableItem(const QString &_strName, const int _classify, const QVariant &_type, const QString &_range,
                  const QString &_nameCHS, const QString &_value, const int &_mode, const int _order,
                  const QString &_unit, const QString &_nControlTyep, const QVariant &_nControlValue,
                  const int _portType = 0, const QString &_portPosition = "", const QString &_portText = "")
        : strName(_strName),
          classify(_classify),
          type(_type),
          range(_range),
          nameCHS(_nameCHS),
          nvalue(_value),
          mode(_mode),
          order(_order),
          unit(_unit),
          portType(_portType),
          portPosition(_portPosition),
          nControlTyep(_nControlTyep),
          nControlValue(_nControlValue),
          portText(_portText)
    {
    }
    QString strName;        // 名称（key）
    int classify;           // 参数类型
    QVariant type;          // 变量类型
    QString range;          // 参数范围
    QString nameCHS;        // 中文名称
    QString nvalue;         // 参数值
    int mode;               // 是否可见
    int order;              // 累加计算位
    QString unit;           // 单位
    int portType;           // 端口类型
    QString portPosition;   // 端口位置
    QString nControlTyep;   // 控件类型
    QVariant nControlValue; // 控件值
    QString portText;       // 端口悬空文本
};

class ElecWizardTableModel : public WizardTableAbstractModel<ElecTableItem>
{
public:
    enum Column { // 列
        ParamName = 0,
        ParamClassify,
        ParamType,
        ParamNameCHS,
        ParamValue,
        ParamMode,
        ParamUnit,
        ControlTyep,
        ControlValue,
        ParamRange,
        ParamPortType,
        ParemPortPosition,
        ParemPortText,
        end,
    };

    enum Category {              // 行
        PortParameter = 0,       // 端口
        ElectricalParameter,     // 电气参数
        loadFlowParameter,       // 潮流参数
        loadFlowResultParameter, // 潮流结果
        simulationParameter,     // 仿真设置
        initSimulationParameter, // 初始值设置
    };

    static const int ElecCategoryRole = Qt::UserRole + 1;

    ElecWizardTableModel(QObject *parent = nullptr);
    ~ElecWizardTableModel();

    /// @brief 添加行数据
    /// @param _strName 名称
    /// @param _type    类型
    /// @param _range   范围
    /// @param _nameCHS 中文名
    /// @param _value   值
    /// @param _mode    可见/编辑
    /// @param _order   计数位
    /// @param _unit    单位
    void addRow(const QString &_strName, Category _classify, const QVariant &_type, const QString &_range,
                const QString &_nameCHS, const QString &_value, const int &_mode, const int _order,
                const QString &_unit, const QString &_nControlTyep, const QVariant &_nControlValue,
                const int protype = 0, const QString &portposition = "", const QString &_portText = "");

    /// @brief 获取指定列的参数值
    /// @param strName
    /// @param type
    /// @return
    QVariant getParameter(const QString &strName, ElecWizardTableModel::Category classify,
                          ElecWizardTableModel::Column type);

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

/// @brief 电气代理排序
class ElecWizardTableProxyModel : public CustomProxyModel
{
public:
    ElecWizardTableProxyModel(QObject *parent = nullptr) : CustomProxyModel(parent) { }

public:
    /// @brief 排序
    /// @param left
    /// @param right
    /// @return
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};
#pragma once

#include "CustomProxyModel.h"
#include "WizardTableAbstractModel.h"
struct TableItem {
    TableItem(const QString &_strName, int _nCategory, const QString &_strType, const QVariant &_value,
              const QString &_strNameCHS, const bool &_boolIsshowPortName, const int _nlimitTytpe,
              const int &nnmodetype, const QString &_nControlTyep, const QVariant &_nControlValue,
              const QString &_strDescription, const int &_index)
        : strName(_strName),
          nCategory(_nCategory),
          strParamType(_strType),
          value(_value),
          strNameCHS(_strNameCHS),
          isShowPortName(_boolIsshowPortName),
          nLimitTytpe(_nlimitTytpe),
          strDescription(_strDescription),
          Index(_index),
          nControlTyep(_nControlTyep),
          nControlValue(_nControlValue),
          nModeType(nnmodetype)
    {
    }
    QString strName;
    int nCategory;
    QString strParamType;
    QVariant value;
    QString strNameCHS;
    bool isShowPortName;
    int nLimitTytpe;
    int nModeType; // 可见类型
    QString strDescription;
    int Index;
    QString nControlTyep;   // 控件类型
    QVariant nControlValue; // 控件值
};

class WizardTableModel : public WizardTableAbstractModel<TableItem>
{
public:
    enum Column {
        Name = 0,
        ParamCategory,
        ParamType,
        ParamValue,
        NameCHS,
        IsshowPortName,
        LimitType,
        ParamMode,
        ControlTyep,
        ControlValue,
        Description,
        Index,
    };

    enum Category { Input, Output, DisState, ConState, Internal, Param };

    static const int CategoryRole = Qt::UserRole + 1;

    WizardTableModel(QObject *parent = nullptr);
    ~WizardTableModel();

    void addRow(const QString &strName, Category category, const QString &strType, const QVariant &value,
                const QString &strNameCHS, const bool isShowportName, const int &nlimitTytpe, const int &nnmodetype,
                const QString &_nControlTyep, const QVariant &_nControlValue, const QString &strDescriptionconst,
                const int &Index);

    void setPrototypeName(const QString &prototypename);
    PVariable initVarible(PVariable variable, const QString &strName, int order = 0);

    // QAbstractTableModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

// 实现排序的代理Model
class WizardTableProxyModel : public CustomProxyModel
{
public:
    explicit WizardTableProxyModel(QObject *parent = nullptr) : CustomProxyModel(parent) { }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#ifndef FAULTSIMUWIDGET_H
#define FAULTSIMUWIDGET_H

#pragma once
#include "PropertyTableModel.h"
#include "ServerBase.h"
#include "TypeItemView.h"
#include <QGridLayout>
class FaultSimuModel;
class FaultSimuModelItem;

namespace Kcc {
namespace BlockDefinition {
class Model;
}
}

using Kcc::NotifyStruct;
static const QString TITLE_FAULT_SIMU = QObject::tr("Fault Simulation"); // 故障模拟
class FaultSimuWidget : public CWidget
{
    Q_OBJECT
public:
    FaultSimuWidget(QWidget *parent = nullptr);
    ~FaultSimuWidget();
public slots:
    void onRecieveSimulationServerManagerMsg(unsigned int code, const NotifyStruct &param);

private:
    void initUI();
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> getBreakerList();

private:
    TableView *m_tableView;
    FaultSimuModel *m_tableModel;
};

class FaultSimuModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        DeviceTypeName = 0,
    };

    FaultSimuModel(bool isboardRunning, QObject *parent = nullptr);
    ~FaultSimuModel();
    void setBoardRunningSts(bool boardRunning);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    // 自定义函数
    void setModelData(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &modellist);
    QStringList getHeaderList() const;

private:
    bool isDataValid(const QModelIndex &index) const;
    bool isModelCubicOff(QSharedPointer<Kcc::BlockDefinition::Model> model) const;
    void setCubicSts(QSharedPointer<Kcc::BlockDefinition::Model> model, bool cubicOff);

private:
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_data;
    QList<FaultSimuModelItem> m_columnItems;
    bool m_isBoardRunning;
};

struct FaultSimuModelItem {
    FaultSimuModelItem(const QString &_Keywords, const QString &_TitleName,
                       const QString &_DataType = Kcc::BlockDefinition::RoleDataDefinition::ControlTypeTextbox,
                       const QVariant &_DataRange = "", const QVariantMap &otherproperty = QVariantMap())
        : Keywords(_Keywords),
          TitleName(_TitleName),
          DataType(_DataType),
          DataRange(_DataRange),
          otherPropertyMap(otherproperty)
    {
    }
    QString Keywords;
    QString TitleName;
    QString DataType;
    QVariant DataRange;
    QVariantMap otherPropertyMap; // 其他属性
};

#endif
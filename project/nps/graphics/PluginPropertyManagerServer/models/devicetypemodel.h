#ifndef DEVICETYPEMODEL_H
#define DEVICETYPEMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

namespace Kcc {
namespace BlockDefinition {
class DeviceModel;
}
}
static const int ModelDataTypeRole = Qt::DisplayRole + 401; // 数据类型
static const QString DataType_String = "string";
static const QString alphabet = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";

struct DeviceListItemProperties {
    DeviceListItemProperties(const QString &_Keywords, const QString &_TitleName, const QString &_DataType,
                             const QVariant _DataRange = "")
        : Keywords(_Keywords), TitleName(_TitleName), DataType(_DataType), DataRange(_DataRange)
    {
    }
    QString Keywords;
    QString TitleName;
    QString DataType;
    QVariant DataRange;
};

class DeviceTypeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        DeviceTypeName = 0,
    };

    DeviceTypeModel(QObject *parent = nullptr);
    ~DeviceTypeModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    // 自定义函数
    void updateDeviceData(const QList<QSharedPointer<Kcc::BlockDefinition::DeviceModel>> &devicelist,
                          QSharedPointer<Kcc::BlockDefinition::DeviceModel> sysdevice);
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> getDeviceModel(const QModelIndex &index);

private:
    QString toLineBreakAtENWord(const QString &original) const;

private:
    QList<QSharedPointer<Kcc::BlockDefinition::DeviceModel>> m_data;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_systemDevicetype;
    QList<DeviceListItemProperties> m_itemPropertyList; // 记录对应列item的属性
};

// 排序和筛选 Model
class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit CustomSortFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent), m_strFilterString(""), m_FilterColumn(-1)
    {
    }

    void setFilterString(const QString &strFilter);

    void setFilterColumn(const int &col);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_strFilterString;
    int m_FilterColumn;
};

#endif // DEVICETYPEMODEL_H

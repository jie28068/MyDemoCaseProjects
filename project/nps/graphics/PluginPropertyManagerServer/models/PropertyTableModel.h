#ifndef CUSTOMTABLEMODEL_H
#define CUSTOMTABLEMODEL_H

#include "GlobalAssistant.h"
#include <QAbstractTableModel>
#include <QDateTime>
#include <QSharedPointer>
#include <QStyledItemDelegate>

static const QString KEY_DISCRIPTION = "KYE_DISCRIPTION";                 // 描述信息
static const QString KEY_UUID = "KEY_UUID";                               // 模型uuid
static const QString KEY_MODELTYPE = "KEY_MODELTYPE";                     // 模型类型
static const QString KEY_ISPROTOTYPE = "KEY_ISPROTOTYPE";                 // 是否原型还是实例
static const QString KEY_INDEX = "KEY_INDEX";                             // 参数index
static const QString KEY_VARIABLECHECKENABLE = "KEY_VARIABLECHECKENABLE"; // 是否可勾选
static const QString KEY_VARIABLECHECKED = "KEY_VARIABLECHECKED";         // 勾选状态
static const QString KEY_VARIABLECHECKEDTIME = "KEY_VARIABLECHECKEDTIME"; // 勾选结果保存的时间
static const QString KEY_REALMODEL = "KEY_REALMODEL";                     // 实际模型给构造型用
static const QString KEY_PARAMEXPVALUE = "ParamExpValue";                 // 表达式的值用来显示tooltips
static const QString KEY_PROTOTYPENAME = "KEY_PROTOTYPENAME";             // 原型名
static const QString KEY_BLOCKPIXMAP = "KEY_BLOCKPIXMAP";                 // 模块图，来源于元件窗口
static const QString KEY_GROUPNAME = "KEY_GROUPNAME";                     // 来源于哪个group的数据
static const QString KEY_DISPLAYVALUEADD = "KEY_DISPLAYVALUEADD";         // 展示数据添加字符

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

// 表格model
struct CustomModelItem {
    CustomModelItem()
        : keywords(""),
          name(""),
          value(QVariant()),
          valueType(""),
          bcanEdit(false),
          controlLimit(QVariant()),
          otherPropertyMap(QVariantMap())
    {
    }

    CustomModelItem(const QString &_keywords, const QString &_name, const QVariant &_value,
                    const QString &_strValueType, const bool &_bcanEdit, const QVariant _controlLimit = "",
                    const QVariantMap otherproperty = QVariantMap())
        : keywords(_keywords),
          name(_name),
          value(_value),
          valueType(_strValueType),
          bcanEdit(_bcanEdit),
          controlLimit(_controlLimit),
          otherPropertyMap(otherproperty)
    {
    }
    inline bool isValid() const { return !keywords.trimmed().isEmpty(); }
    inline bool operator==(const CustomModelItem &other) const
    {
        return (keywords == other.keywords) && (name == other.name) && (value == other.value)
                && (valueType == other.valueType) && (bcanEdit == other.bcanEdit)
                && (controlLimit == other.controlLimit) && (otherPropertyMap == other.otherPropertyMap);
    }
    inline bool operator!=(const CustomModelItem &other) const { return !(*this == other); }
    void setProperty(const QString &key, const QVariant &value)
    {
        if (otherPropertyMap.contains(key)) {
            otherPropertyMap[key] = value;
        } else {
            otherPropertyMap.insert(key, value);
        }
    }

    QVariantMap getPropertyMap() { return otherPropertyMap; }
    QString keywords; // 作为key
    QString name;     // 作为log打印&显示--只有两列的时候显示,
    QVariant value;
    QString valueType;
    bool bcanEdit;
    QVariant controlLimit;
    QVariantMap otherPropertyMap;
};
class PropertyTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColumnFirst = 0, //
        ColumnSecond,    //
        ColumnThird,     //
        ColumnFourth,    //
    };

    enum CustomRole {
        CustomRole_ColorRole = 500,
    };
    PropertyTableModel(QObject *parent = nullptr);
    ~PropertyTableModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    // 自定义函数
    void setModelData(const QList<CustomModelItem> &listdata, const QStringList &listheader);
    bool checkValueChange();
    bool checkValueLegitimacy(QString &errorinfo);
    QList<CustomModelItem> getNewListData();
    QList<CustomModelItem> getOldListData();
    CustomModelItem getModelItem(const QString &keywords, bool bnew = true);
    void updateValue(const QString &keywords, const QVariant &value, bool bdescription = false,
                     bool bsendSignal = true);
    void updateValue(const QString &keywords, const CustomModelItem &item, bool bsendSignal = true);
    void setModelDataReadOnly(bool bReadOnly);
    void setModelDataColorUse(bool use);
signals:
    void modelDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);

protected:
    QList<CustomModelItem> m_data;
    QList<CustomModelItem> m_olddata;
    QStringList m_listHeader;
    bool m_bReadOnly;
    bool m_useColor;
};

// 表格代理
class CustomTableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    enum Column {
        DeviceTypeName = 0,
    };
    CustomTableItemDelegate(QObject *parent);
    ~CustomTableItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
};

#endif // CUSTOMTABLEMODEL_H

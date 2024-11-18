#ifndef COLORMATCHMODEL_H
#define COLORMATCHMODEL_H

#include "GlobalAssistant.h"
#include <QAbstractTableModel>
#include <QDateTime>
#include <QSharedPointer>
#include <QStyledItemDelegate>

static const QString KEY_FIRST = "KEY_FIRST";                 // 
static const QString KEY_SECOND = "KEY_SECOND";               // 
static const QString KEY_THIRD = "KEY_THIRD";                 // 
static const QString KEY_FOURTH = "KEY_FOURTH";               // 


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

// ����model
struct ColorMatchModelItem {

    ColorMatchModelItem(const QList<QVariant> dataMap = QList<QVariant>(),
    const QList<QString> typeMap = QList<QString>(),const QList<bool> caneditMap = QList<bool>())
        : m_datalist(dataMap),
          m_typelist(typeMap),
          m_caneditlist(caneditMap)
    {
    }
    inline bool isValid() const { return !m_datalist.isEmpty(); }
    inline bool operator==(const ColorMatchModelItem &other) const
    {
        return (m_datalist == other.m_datalist) && (m_typelist == other.m_typelist) && (m_caneditlist == other.m_caneditlist);
    }
    inline bool operator!=(const ColorMatchModelItem &other) const { return !(*this == other); }
    
    QList<QVariant> m_datalist;//
    QList<QString> m_typelist;//
    QList<bool> m_caneditlist;// 
};
class ColorMatchModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum CustomRole {
        DataRole_Color,
        DataRole_Text
    };
    ColorMatchModel(QObject *parent = nullptr);
    ~ColorMatchModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    // 
    void setModelData(const QList<ColorMatchModelItem> &listdata, const QStringList &listheader);
    QVariantList getNewListData();
    ColorMatchModelItem getModelItem(const QString &keywords, bool bnew = true);
    void updateValue(int row,int col,QVariant val, bool bsendSignal = true);
    void setModelDataReadOnly(bool bReadOnly);
    void setModelDataColorUse(bool use);
signals:
    void modelDataItemChanged(const ColorMatchModelItem &olditem, const ColorMatchModelItem &newitem);

protected:
    QList<ColorMatchModelItem> m_data;
    QStringList m_listHeader;
    bool m_bReadOnly;
    bool m_useColor;
};


#endif // COLORMATCHMODEL_H

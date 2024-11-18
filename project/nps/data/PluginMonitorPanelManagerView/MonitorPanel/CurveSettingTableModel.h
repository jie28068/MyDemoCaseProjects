#ifndef CURVESETTINGTABLEMODEL_H
#define CURVESETTINGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QComboBox>
#include <QStyledItemDelegate>

#include "def.h"
//
// #ifndef RGB
// #define RGB(r,g,b)          ((QRgb)(((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|(((QRgb)(unsigned
// char)(r))<<16))) #endif
//
// #define CURVECOLORS_COUNT	18
//
// const QRgb CURVECOLORS[] = {RGB(255,255,17),//黄
//							RGB(75,159,14),//绿
//							RGB(19,159,255),//蓝
//							RGB(191,79,31),//棕
//							RGB(254,67,101),//红
//							RGB(179,214,110),
//							RGB(248,147,29),
//							RGB(137,190,178),
//							RGB(222,211,140),
//							RGB(38,157,128),
//							RGB(255,0,255),
//							RGB(0,0,255),
//							RGB(0,255,0),
//							RGB(0,255,255),
//							RGB(255,0,0),
//							RGB(255,255,0),
//							RGB(255,255,255),
//							RGB(0,0,0)
//							};
//
//
// class ColorComboBoxItemDelegate : public QStyledItemDelegate
//{
//	Q_OBJECT
// public:
//	ColorComboBoxItemDelegate(QObject *parent=nullptr);
//	~ColorComboBoxItemDelegate();
//
//	virtual QWidget* createEditor ( QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index )
//const; 	virtual void setEditorData ( QWidget *editor, const QModelIndex &index ) const; 	virtual void setModelData (
//QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const; 	virtual void updateEditorGeometry (
//QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
//};

class CurveSettingTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CurveSettingTableModel(QObject *parent);
    ~CurveSettingTableModel();

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setCurveInfo(QList<PlotCurveInfo> &info);
    QList<PlotCurveInfo> getCurveInfo(void);

    // void setCurveInfo(QList<CurveInfo> & info);
    // QList<CurveInfo> getCurveInfo(void);

    // void addCurveInfo(CurveInfo &info);

    // void deleteCurveInfo(int nRow);
    // void clearCurveInfo(void);
protected:
    // virtual QStringList mimeTypes() const override;
    // virtual QMimeData* mimeData(const QModelIndexList &indexes) const override;
    // virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex
    // &parent) override;
private:
    QList<QString> m_headerList;

    QList<PlotCurveInfo> m_curveList;
};

#endif // CURVESETTINGTABLEMODEL_H

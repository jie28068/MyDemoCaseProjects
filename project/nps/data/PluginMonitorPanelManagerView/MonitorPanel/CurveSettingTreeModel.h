#ifndef CURVESETTINGTREEMODEL_H
#define CURVESETTINGTREEMODEL_H

#include <QAbstractItemModel>
#include <QComboBox>
#include <QIcon>
#include <QStyledItemDelegate>
#include <QVector>

#include "def.h"

class StyleLineEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StyleLineEditDelegate(QObject *parent = nullptr);
    virtual ~StyleLineEditDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;
};

// 曲线设置对话框中的样式下拉combobox
class StyleComboBoxItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StyleComboBoxItemDelegate(QObject *parent = nullptr, bool isIcon = false);
    virtual ~StyleComboBoxItemDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;

private:
    bool mIsIconCombo = false;
};

// 曲线设置中颜色块
class ColorLabelDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ColorLabelDelegate(QObject *parent = nullptr);
    virtual ~ColorLabelDelegate();

    // virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index)
    // const; virtual void setEditorData(QWidget *editor, const QModelIndex &index) const; virtual void
    // setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};
// 曲线设置 设置线宽spinbox
class LineWidthItemDelegate : public QStyledItemDelegate // QItemDelegate
{
    Q_OBJECT
public:
    LineWidthItemDelegate(QObject *parent = nullptr);
    virtual ~LineWidthItemDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;
};

class CurveSettingTreeItem
{
public:
    explicit CurveSettingTreeItem(const PlotCurveInfo &itemData, int m_type, CurveSettingTreeItem *parent = nullptr);
    explicit CurveSettingTreeItem(int type, CurveSettingTreeItem *parent = nullptr);

    ~CurveSettingTreeItem();

    QVariant data(int col, int role);
    Qt::ItemFlags flags(int col);

    CurveSettingTreeItem *child(int i);
    CurveSettingTreeItem *parent(void);

    QList<CurveSettingTreeItem *> &childs(void) { return m_childItems; }
    int childCount(void) const;
    int pos(void) const;
    bool setData(int col, const QVariant &val, int role);
    int columnCount(void) const;
    bool append(CurveSettingTreeItem *item);
    bool remove(int i);
    bool remove(CurveSettingTreeItem *item);
    int level(void) const;
    void setLevel(int n);
    void setIcon(QString path);
    void setItemData(PlotCurveInfo &itemData) { m_itemData = itemData; }
    void setVaribleInfo(PlotVaribleInfo &varibleInfo) { m_varibleInfo = varibleInfo; }
    PlotCurveInfo getItemData(void) { return m_itemData; }
    PlotVaribleInfo getVaribleInfo(void) { return m_varibleInfo; }

private:
    QList<CurveSettingTreeItem *> m_childItems;
    CurveSettingTreeItem *m_parentItem;
    int m_level;
    bool m_isChecked;
    // QString								m_iconPath;
    QIcon m_icon;
    // QVector<QVariant>					m_itemData;
    PlotCurveInfo m_itemData;
    PlotVaribleInfo m_varibleInfo;
    int m_type;
};
/////////////////////////////////////////////////////////////////////////////////
class CurveSettingTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    CurveSettingTreeModel(int type, QObject *parent);
    virtual ~CurveSettingTreeModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole) override;
    // bool						insertRows(int row, int count, const QModelIndex &parent =
    // QModelIndex())override;
    // bool						removeRows(int row, int count, const QModelIndex &parent =
    // QModelIndex())override;

    CurveSettingTreeItem *root(void) const;
    CurveSettingTreeItem *itemFromIndex(const QModelIndex &index) const;
    void refresh(void);

    void setPlotVaribleInfoList(QList<PlotVaribleInfo> &varibleInfoList);
    QList<PlotVaribleInfo> getPlotVaribleInfoList(void);
    void addPlotVaribleInfo(PlotVaribleInfo &varibleInfo);
    void deletePlotVaribleInfo(QModelIndex &index);
    void clearPlotVaribleInfoList(void);

private:
    QStringList m_headerList;
    CurveSettingTreeItem *m_rootItem;
    int m_type;
};

#endif // CURVESETTINGTREEMODEL_H

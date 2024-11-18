#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include "KLWidgets/KItemView.h"
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QTableView>
#include <QWidget>

class TableView;
class SettingTableModel;

struct TableSetItem {
    TableSetItem(const QString &_colName, bool _bDisplay) : colName(_colName), bDisplay(_bDisplay) { }
    QString colName; // 列表头名称
    bool bDisplay;   // 当前列是否显示
};

struct TableSetStruct {
    TableSetStruct(const QString _currentTableName = "")
        : currentTableName(_currentTableName), currentTableFreezeCols(0)
    {
        tableSetPropertyMap.clear();
    }
    // 是否为空
    bool bNullStruct()
    {
        if (tableSetPropertyMap.size() > 0) {
            return false;
        } else {
            return true;
        }
    }
    void clear()
    {
        currentTableName = "";
        currentTableFreezeCols = 0;
        tableSetPropertyMap.clear();
    }
    QList<TableSetItem> getCurrentTableList() { return tableSetPropertyMap[currentTableName]; }

    void saveTableList(const QList<TableSetItem> &tableSetlist)
    {
        tableSetPropertyMap[currentTableName] = tableSetlist;
    }

    void setTableList(const QString &tablename, const QList<QString> &tablelist, const int &freezecols)
    {
        currentTableName = tablename;
        currentTableFreezeCols = freezecols;
        if (!tableSetPropertyMap.contains(tablename)
            || tableSetPropertyMap[currentTableName].size() != tablelist.size()) {
            QList<TableSetItem> tablesetlist;
            foreach (QString titlename, tablelist) {
                tablesetlist.append(TableSetItem(titlename, true));
            }
            tableSetPropertyMap[tablename] = tablesetlist;
        }
    }

    QList<int> getVisibleColumnsNo()
    {
        QList<int> visiblecolumnsNo; // 表头显示的列;0表示第一列，与表格保持一致
        QList<TableSetItem> tablelist = tableSetPropertyMap[currentTableName];
        for (int i = 0; i < tablelist.size(); ++i) {
            if (tablelist[i].bDisplay) {
                visiblecolumnsNo.append(i);
            }
        }
        return visiblecolumnsNo;
    }

    bool getLastColumnsVisible()
    {
        QList<int> visiblecolumnsNo; // 表头显示的列;0表示第一列，与表格保持一致
        QList<TableSetItem> tablelist = tableSetPropertyMap[currentTableName];
        if (tablelist.isEmpty()) {
            return false;
        }
        return tablelist[tablelist.size() - 1].bDisplay;
    }
    QString getCurrentTableName() { return currentTableName; }

    const int getCurrentTitleColNo(const QString &titleName)
    {
        QList<TableSetItem> tablelist = tableSetPropertyMap[currentTableName];
        for (int i = 0; i < tablelist.size(); ++i) {
            if (titleName == tablelist[i].colName) {
                return i;
            }
        }
        return 0;
    }

    QString currentTableName;   // 当前表格名
    int currentTableFreezeCols; // 当前表格冻结列数
    // key对应的表格名，value对应表格中表头的属性。
    QMap<QString, QList<TableSetItem>> tableSetPropertyMap;
};

class ColSetTableHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    enum CheckState { AllChecked, PartChecked, NoChecked };

    ColSetTableHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ColSetTableHeaderView();

    void setColumnCheckable(int col, bool checkable);
    void setHeaderCheckStatus(ColSetTableHeaderView::CheckState checkedStatus);
signals:
    void columuSectionClicked(const int &col, bool checked);

private slots:
    void onSectionsClicked(int index);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
    QMap<int, bool> m_columnCheckedMap;
    CheckState m_checkState;
    bool m_bCheckboxEnable;
    int m_freezecol;
};

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();
    void setTableTitle(const QList<TableSetItem> &tabletitlelist, const int freezecol);
    QList<TableSetItem> getCurrentTableSetList();
private slots:
    void onColumnAllSelected(const int &col, bool selected);
    void onItemCheckStatusChanged();

private:
    ColSetTableHeaderView::CheckState getHeaderCheckStatus(const QList<TableSetItem> &listdata);

private:
    KTableView *m_tableview;
    ColSetTableHeaderView *m_pHeaderView;
    SettingTableModel *m_settingTableModel;
};

// 设置表格table
class SettingTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        TableColumnDisplay = 0,
        TableColumnName,
    };

    SettingTableModel(QObject *parent = nullptr);
    ~SettingTableModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void updateTableSetting(const QList<TableSetItem> &tablelist, const int &freezecolno);
    void setAllColumnStatus(bool bdisplay);
    void clear();
    QList<TableSetItem> getCurrentTableInfo();
signals:
    void itemCheckStatusChanged();

private:
    QStringList m_listHeader;
    int m_freezeColno;
    // key是表格名，QList<>是表格的所有表头，属性存在里面的QList<QVariant>是表头名，是否显示
    QList<TableSetItem> m_tablePropertyList;
};

#endif // SETTINGWIDGET_H

#ifndef ADJUSTLISTVIEWWIDGET_H
#define ADJUSTLISTVIEWWIDGET_H

#include <QAbstractListModel>
#include <QListView>
#include <QPainter>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QWidget>

class CustomListModel;
class AdjustListView;

class AdjustListViewWidget : public QWidget
{
    Q_OBJECT

public:
    AdjustListViewWidget(const QStringList &listdata, const QMap<QString, QStringList> &listdatamap,
                         QWidget *parent = nullptr);
    ~AdjustListViewWidget();

    QStringList getGroupList();
    QMap<QString, QStringList> getGroupItemMap();
private slots:
    void onDeleteGroupClicked();
    void onDeleteGroupItemClicked();
    void onGroupItemClicked(const QModelIndex &index);
    void onGroupAdjustFinished();
    void onGroupItemAdjustFinished();

private:
    void initUI();
    void initQlistView(QListView *listview);

private:
    AdjustListView *m_pGroupList;
    AdjustListView *m_pGroupDataList;
    CustomListModel *m_pGroupModel;
    CustomListModel *m_pGroupDataModel;
    QStringList m_groupListData;
    QMap<QString, QStringList> m_groupMapData;
};

class CustomListModel : public QAbstractListModel
{
public:
    CustomListModel(QObject *parent = nullptr);
    ~CustomListModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setListData(const QStringList &listdata);

private:
    QList<QString> m_data;
};

class AdjustListView : public QListView
{
    Q_OBJECT

public:
    explicit AdjustListView(QWidget *parent = nullptr);

    bool isDraging() const { return m_IsDraging; }
    int offset() const { return 19; }
    int highlightedRow() const { return m_theHighlightedRow; }
    int dragRow() const { return m_theDragRow; }
    int selectedRow() const { return m_theSelectedRow; }
    static QString myMimeType() { return "AdjustListViewMineData"; }
signals:
    void adjustFinished();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void drawText(QDrag *drag, const QString &text);

private:
    QPoint startPos;
    bool m_IsDraging;
    int m_theHighlightedRow;
    int m_oldHighlightedRow;
    int m_theDragRow;
    int m_theSelectedRow;
    int m_theInsertRow;
};

// delegate，绘制文本
class CustomListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CustomListDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
#endif // ADJUSTLISTVIEWWIDGET_H

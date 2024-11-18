#ifndef TABLEHEADERVIEW_H
#define TABLEHEADERVIEW_H

#include <QHeaderView>
#include <QMap>
#include <QMouseEvent>
#include <QObject>
class TableHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    enum CheckState { AllChecked, PartChecked, NoChecked };

    TableHeaderView(Qt::Orientation orientation, QWidget *parent);
    ~TableHeaderView();

    void setColumnCheckable(int col, bool checkable);
    void setCheckBoxEnableStatus(bool enable);
    void setColAllSelected();

    void setHeaderCheckStatus(TableHeaderView::CheckState checkedStatus, bool ColCheckedFlag);

signals:
    void columuSectionClicked(int logicalIndex, bool checked);

public slots:
    void onSectionsClicked(int index);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QMap<int, bool> m_columnCheckedMap;
    int m_bIsAllChecked;
    bool m_bCheckboxEnable;
    QPoint m_ptMouseRelease;
};

#endif // TABLEHEADERVIEW_H

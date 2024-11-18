#ifndef SIMULATIONLISTITEMDELEGATE_H
#define SIMULATIONLISTITEMDELEGATE_H

#include <QItemDelegate>
#include <QListView>
#include <QPainter>
#include <QStyledItemDelegate>

class SimulationListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SimulationListItemDelegate(QListView *parent = nullptr);
    ~SimulationListItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    void drawText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SIMULATIONLISTITEMDELEGATE_H

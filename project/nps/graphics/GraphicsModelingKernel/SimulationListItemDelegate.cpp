#include "SimulationListItemDelegate.h"

SimulationListItemDelegate::SimulationListItemDelegate(QListView *parent) : QStyledItemDelegate(parent) { }

SimulationListItemDelegate::~SimulationListItemDelegate() { }

void SimulationListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

void SimulationListItemDelegate::drawText(QPainter *painter, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
}

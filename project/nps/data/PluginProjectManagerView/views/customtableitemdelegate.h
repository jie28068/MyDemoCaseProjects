#ifndef CUSTOMTABLEITEMDELEGATE_H
#define CUSTOMTABLEITEMDELEGATE_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QWidget>

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

#endif // CUSTOMTABLEITEMDELEGATE_H

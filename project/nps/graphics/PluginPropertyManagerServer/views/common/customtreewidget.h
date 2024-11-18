#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTreeWidget(QWidget *parent = nullptr, bool isDoubleClickExpand = false);
    ~CustomTreeWidget() { }
    QString getCurrentItemPathText(const QTreeWidgetItem &item);

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QStringList getTreeSelectItemsName(const QTreeWidgetItem &sel);

signals:
    void doubleClickedItem(QTreeWidgetItem *item);

private:
    bool m_isDoubleClickExpand;
    QList<QTreeWidgetItem *> m_selItems;
};

#endif // CUSTOMTREEWIDGET_H
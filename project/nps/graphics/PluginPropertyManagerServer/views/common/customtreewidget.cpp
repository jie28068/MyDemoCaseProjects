#include "customtreewidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget *parent, bool isDoubleClickExpand)
    : QTreeWidget(parent), m_isDoubleClickExpand(isDoubleClickExpand)
{
    setHeaderHidden(true);
}

QString CustomTreeWidget::getCurrentItemPathText(const QTreeWidgetItem &item)
{
    QString text;
    QList<QString> itemsTextList;
    itemsTextList = getTreeSelectItemsName(item); // 遍历所选的item的所有父节点item

    // 构建字符串
    int nInitIndex = itemsTextList.size() - 1;
    for (int index = nInitIndex; index >= 0; index--) {
        auto itemText = itemsTextList.at(index);
        text.append(itemText);
        text.append(".");
    }
    text.chop(1); // 删除最后的逗号
    return text;
}

void CustomTreeWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);

    m_selItems = this->selectedItems();

    QTreeWidgetItem *sel = m_selItems.first();
    QString text = getCurrentItemPathText(*sel);
    QByteArray textData = text.toLocal8Bit(); // 转换编码并传给QByteArray

    // 设置拖放的数据
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-qabstractitemmodeldatalist", textData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
}

void CustomTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_isDoubleClickExpand) {
        QTreeWidgetItem *item = itemAt(event->pos());
        if (item) {
            event->ignore();              // 如果双击的item不为空，则不处理展开操作
            emit doubleClickedItem(item); // 发送双击信号
        } else {
            QTreeWidget::mouseDoubleClickEvent(event);
        }
        return;
    }
    return QTreeWidget::mouseDoubleClickEvent(event);
}

QStringList CustomTreeWidget::getTreeSelectItemsName(const QTreeWidgetItem &sel)
{
    QList<QString> itemsTextList;
    QString str = sel.text(0);
    itemsTextList.push_front(str);

    auto parentItem = sel.parent();
    if (!parentItem) {
        return itemsTextList;
    }
    itemsTextList << getTreeSelectItemsName(*parentItem);
    return itemsTextList;
}

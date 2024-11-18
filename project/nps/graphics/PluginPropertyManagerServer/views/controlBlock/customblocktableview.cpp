#include "customblocktableview.h"

#include "tableheaderview.h"

CustomBlockTableView::CustomBlockTableView(QWidget *parent)
    : QTableView(parent),
      IsDraging(false),
      curHighlightedRow(-2),
      oldHighlightedRow(-2),
      curDragRow(-1),
      curInsertRow(-1)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setStyleSheet("QListView::Item{height:20px;}");
    // 列撑满整个表格
    //  this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 去掉表格边框线
    //  this->setShowGrid(false);
    // 整行选择
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 按住ctrl多选
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void CustomBlockTableView::dragEnterEvent(QDragEnterEvent *event)
{
    CustomBlockTableView *source = qobject_cast<CustomBlockTableView *>(event->source());
    if (source && source == this) {
        IsDraging = true; // IsDraging(标志位)判断是否正在拖拽
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void CustomBlockTableView::dragLeaveEvent(QDragLeaveEvent *event)
{
    oldHighlightedRow = curHighlightedRow;
    curHighlightedRow = -2;
    // 刷新以使dropIndicator消失,
    updateDropIndicator(oldHighlightedRow);
    IsDraging = false;
    curInsertRow = -1;
    event->accept();
}
// dropIndicator显示与消失
void CustomBlockTableView::updateDropIndicator(int row)
{
    // 这里用的是update(QModelIndex)，这里也可以使用update(QRect)，在表格所有列绘制分隔符
    for (int updateColI = 0; updateColI < this->model()->columnCount(); updateColI++) {
        // 刷新新区域使dropIndicator显示
        update(model()->index(row, updateColI));
        update(model()->index(row + 1, updateColI));
    }
}
void CustomBlockTableView::dragMoveEvent(QDragMoveEvent *event)
{
    CustomBlockTableView *source = qobject_cast<CustomBlockTableView *>(event->source());
    if (source && source == this) {
        oldHighlightedRow = curHighlightedRow;
        curHighlightedRow = indexAt(event->pos() - QPoint(0, offset())).row();

        // offset() = 9 = rowHeight / 2 - 1，其中 rowHeight 是行高
        if (event->pos().y() >= offset()) {
            if (oldHighlightedRow != curHighlightedRow) {
                // 刷新以使dropIndicator消失
                updateDropIndicator(oldHighlightedRow);

                // 刷新新区域使dropIndicator显示
                updateDropIndicator(curHighlightedRow);

            } else {
                // 刷新新区域使dropIndicator显示
                updateDropIndicator(curHighlightedRow);
            }
            curInsertRow = curHighlightedRow + 1;
        } else {
            curHighlightedRow = -1;
            // 刷新新区域使dropIndicator显示，第一行
            updateDropIndicator(0);

            curInsertRow = 0;
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void CustomBlockTableView::dropEvent(QDropEvent *event)
{
    CustomBlockTableView *source = qobject_cast<CustomBlockTableView *>(event->source());
    if (source && source == this) {

        IsDraging = false; // 完成拖拽
        oldHighlightedRow = curHighlightedRow;
        curHighlightedRow = -2;

        // 刷新以使dropIndicator消失
        updateDropIndicator(oldHighlightedRow);

        // 这里我像QListWidget那样调用父类dropEvent(event)发现不起作用(原因尚不明)，没办法，只能删除旧行，插入新行
        // 从event->mimeData()取出拖拽数据
        QList<int> text;
        //                QIcon icon;
        QByteArray itemData = event->mimeData()->data(myMimeType());
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        dataStream >> text; // >> icon;

        for (int rowI = 0; rowI < text.size(); rowI++) {
            if (curInsertRow == text.at(rowI) || curInsertRow == text.at(rowI) + 1) {
                curDragRows.clear();
                return;
            }
        }

        qSort(text.begin(), text.end(), compareBarData);
        QList<QList<QStandardItem *>> standardItemRow;

        for (int rowI = 0; rowI < text.size(); rowI++) {
            QList<QStandardItem *> standardItem;
            for (int i = 0; i < this->model()->columnCount(); i++) {
                QModelIndex index = this->model()->index(text.at(rowI), i);
                QString name = this->model()->data(index).toString();
                standardItem.append(new QStandardItem(name));
                // qDebug() << "[" << __LINE__ << "-" << __FUNCTION__ << "]" << name;
            }
            standardItemRow.append(standardItem);
        }

        for (int rowI = 0; rowI < text.size(); rowI++) {
            model()->insertRow(curInsertRow + rowI); // 插入新行
            QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
            for (int i = 0; i < this->model()->columnCount(); i++) {
                listModel->setItem(curInsertRow + rowI, i, standardItemRow[rowI][i]);
            }
            for (int j = 0; j < this->model()->rowCount(); j++) {
                listModel->item(j, 0)->setCheckable(
                        true); // 重新使能被拖动项的复选功能，否则拖动改变顺序后不可勾选或者取消勾选
                listModel->item(j, 0)->setCheckState(listModel->item(j, 0)->checkState());
            }
        }
        setCurrentIndex(model()->index(curInsertRow, 0)); // 插入行保持选中状态
        event->setDropAction(Qt::MoveAction);
        event->accept();
        update();
        this->horizontalHeader()->viewport()->update();
    }
}

/**!使用startDrag()则不需要判断拖拽距离,按下鼠标时触发
 */
void CustomBlockTableView::startDrag(Qt::DropActions)
{
    // 清空当前拖拽列表，防止重复拖拽选中
    curDragRows.clear();
    // 当前拖拽行
    curDragRow = currentIndex().row();
    QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
    QModelIndexList modelIndexList = this->selectionModel()->selectedRows();
    // 放入QMimeData容器中的拖拽数据（选中的行索引）
    QList<int> text;
    foreach (QModelIndex modelIndex, modelIndexList) {
        // 把当前拖拽数据放到拖拽列表中，实现后面拖拽完成删除旧的行
        curDragRows.append(modelIndex.row());
        // 把拖拽数据的行索引放在QMimeData容器中
        text.append(modelIndex.row());
    }
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << text;

    // 设置拖拽对象，可自动定义拖拽的缩略图样式可自己继承 QWidget 画一个出来
    /**QWidget* widget= new QWidget(this);
    widget->setupthumbnail(icon, text);
    QPixmap pixmap = widget->grab();
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
    */
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(myMimeType(), itemData);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    // 删除的行需要根据curInsertRow和curDragRow的大小关系来判断，大于当前放下的行需要加上放下的行数，小于当前放下的行则不需加
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
        int sizecurDragRows = curDragRows.size();
        qSort(curDragRows.begin(), curDragRows.end(), compareBarData);
        int theRemoveRow = -1;
        for (int i = curDragRows.size() - 1; i >= 0; i--) {
            if (curInsertRow < curDragRows.at(i))
                theRemoveRow = curDragRows.at(i) + sizecurDragRows;
            else
                theRemoveRow = curDragRows.at(i);
            model()->removeRow(theRemoveRow);
            curDragRows.removeAt(i);
        }
    }
}
bool compareBarData(const int &barAmount1, const int &barAmount2)
{
    if (barAmount1 < barAmount2) //<升序排列,>降序排列
    {
        return true;
    }
    return false;
}

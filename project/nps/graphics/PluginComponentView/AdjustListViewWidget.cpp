#include "AdjustListViewWidget.h"

#include <QApplication>
#include <QDrag>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>

static const int POLYGON = 4; // 等腰三角形直角边长
static const int WIDTH = 1;   // 分隔符粗细的一半

AdjustListViewWidget::AdjustListViewWidget(const QStringList &listdata, const QMap<QString, QStringList> &listdatamap,
                                           QWidget *parent)
    : QWidget(parent),
      m_pGroupList(nullptr),
      m_pGroupDataList(nullptr),
      m_pGroupModel(nullptr),
      m_pGroupDataModel(nullptr)
{
    m_groupListData = listdata;
    m_groupMapData = listdatamap;
    initUI();
}

AdjustListViewWidget::~AdjustListViewWidget() { }

QStringList AdjustListViewWidget::getGroupList()
{
    return m_groupListData;
}

QMap<QString, QStringList> AdjustListViewWidget::getGroupItemMap()
{
    return m_groupMapData;
}

void AdjustListViewWidget::onDeleteGroupClicked()
{
    if (m_pGroupList != nullptr && !m_pGroupList->currentIndex().data().toString().isEmpty()
        && m_pGroupDataList != nullptr) {
        m_groupListData.removeOne(m_pGroupList->currentIndex().data().toString());
        m_groupMapData.remove(m_pGroupList->currentIndex().data().toString());
        QStandardItemModel *groupModel = new QStandardItemModel();
        for (int i = 0; i < m_groupListData.size(); ++i) {
            groupModel->setItem(i, 0, new QStandardItem(m_groupListData[i]));
        }
        m_pGroupList->setModel(groupModel);
        m_pGroupList->setCurrentIndex(groupModel->index(0, 0));
        QStandardItemModel *grouplistModel = new QStandardItemModel();
        for (int j = 0; j < m_groupMapData[m_groupListData[0]].size(); ++j) {
            grouplistModel->setItem(j, 0, new QStandardItem(m_groupMapData[m_groupListData[0]][j]));
        }
        m_pGroupDataList->setModel(grouplistModel);
    }
}

void AdjustListViewWidget::onDeleteGroupItemClicked()
{
    if (m_pGroupList != nullptr && !m_pGroupList->currentIndex().data().toString().isEmpty()
        && m_pGroupDataList != nullptr && !m_pGroupDataList->currentIndex().data().toString().isEmpty()
        && m_groupMapData.contains(m_pGroupList->currentIndex().data().toString())) {
        m_groupMapData[m_pGroupList->currentIndex().data().toString()].removeOne(
                m_pGroupDataList->currentIndex().data().toString());
        QStandardItemModel *grouplistModel = new QStandardItemModel();
        for (int j = 0; j < m_groupMapData[m_pGroupList->currentIndex().data().toString()].size(); ++j) {
            grouplistModel->setItem(
                    j, 0, new QStandardItem(m_groupMapData[m_pGroupList->currentIndex().data().toString()][j]));
        }
        m_pGroupDataList->setModel(grouplistModel);
    }
}

void AdjustListViewWidget::onGroupItemClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= m_groupListData.size()) {
        return;
    }
    QStandardItemModel *grouplistModel = new QStandardItemModel();
    for (int j = 0; j < m_groupMapData[m_groupListData[index.row()]].size(); ++j) {
        grouplistModel->setItem(j, 0, new QStandardItem(m_groupMapData[m_groupListData[index.row()]][j]));
    }
    m_pGroupDataList->setModel(grouplistModel);
}

void AdjustListViewWidget::onGroupAdjustFinished()
{
    if (m_pGroupList != nullptr && m_pGroupList->model() != nullptr) {
        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(m_pGroupList->model());
        if (listModel != nullptr) {
            QStringList grouplist;
            for (int i = 0; i < listModel->rowCount(); ++i) {
                grouplist.append(listModel->index(i, 0).data().toString());
            }
            m_groupListData = grouplist;
        }
    }
}

void AdjustListViewWidget::onGroupItemAdjustFinished()
{
    if (m_pGroupList != nullptr && m_pGroupDataList != nullptr
        && !m_pGroupList->currentIndex().data().toString().isEmpty() && m_pGroupDataList->model() != nullptr) {
        QStandardItemModel *blocklistModel = qobject_cast<QStandardItemModel *>(m_pGroupDataList->model());
        if (blocklistModel != nullptr) {
            QString groupname = m_pGroupList->currentIndex().data().toString();
            if (m_groupMapData.contains(groupname)) {
                QStringList blocklist;
                for (int i = 0; i < blocklistModel->rowCount(); ++i) {
                    blocklist.append(blocklistModel->index(i, 0).data().toString());
                }
                m_groupMapData[groupname] = blocklist;
            }
        }
    }
}

void AdjustListViewWidget::initUI()
{
    this->resize(640, 480);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    this->setObjectName("AdjustListViewWidget");
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setContentsMargins(10, 10, 10, 0);
    QLabel *pGroupLabel = new QLabel(this);
    pGroupLabel->setText(tr("group list:")); // 组列表：
    m_pGroupList = new AdjustListView(this);
    // initQlistView(m_pGroupList);
    m_pGroupList->setItemDelegate(new CustomListDelegate(m_pGroupList));
    QStandardItemModel *groupModel = new QStandardItemModel();
    for (int i = 0; i < m_groupListData.size(); ++i) {
        groupModel->setItem(i, 0, new QStandardItem(m_groupListData[i]));
    }
    m_pGroupList->setModel(groupModel);
    m_pGroupList->setCurrentIndex(groupModel->index(0, 0));

    QLabel *pGroupListLabel = new QLabel(this);
    pGroupListLabel->setText(tr("list of items within the group:")); // 组内Item列表：
    m_pGroupDataList = new AdjustListView(this);
    // initQlistView(m_pGroupDataList);
    m_pGroupDataList->setItemDelegate(new CustomListDelegate(m_pGroupDataList));
    QStandardItemModel *grouplistModel = new QStandardItemModel();
    for (int j = 0; j < m_groupMapData[m_groupListData[0]].size(); ++j) {
        grouplistModel->setItem(j, 0, new QStandardItem(m_groupMapData[m_groupListData[0]][j]));
    }
    m_pGroupDataList->setModel(grouplistModel);
    pGridLayout->addWidget(pGroupLabel, 0, 0, 1, 1);
    pGridLayout->addWidget(m_pGroupList, 1, 0, 4, 1);
    pGridLayout->addWidget(pGroupListLabel, 0, 1, 1, 1);
    pGridLayout->addWidget(m_pGroupDataList, 1, 1, 4, 1);
    QPushButton *pDeleteGroupButton = new QPushButton(this);
    pDeleteGroupButton->setText(tr("delete group")); // 删除组
    pDeleteGroupButton->setFixedWidth(100);
    QPushButton *pDeleteGroupDataItemButton = new QPushButton(this);
    pDeleteGroupDataItemButton->setText(tr("delete items within the group")); // 删除组内Item
    pDeleteGroupDataItemButton->setFixedWidth(100);
    connect(pDeleteGroupButton, SIGNAL(released()), this, SLOT(onDeleteGroupClicked()));
    connect(pDeleteGroupDataItemButton, SIGNAL(released()), this, SLOT(onDeleteGroupItemClicked()));
    pGridLayout->addWidget(pDeleteGroupButton, 0, 2, 1, 1);
    pGridLayout->addWidget(pDeleteGroupDataItemButton, 1, 2, 1, 1);
    setLayout(pGridLayout);
    connect(m_pGroupList, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onGroupItemClicked(const QModelIndex &)));

    connect(m_pGroupList, SIGNAL(adjustFinished()), this, SLOT(onGroupAdjustFinished()));
    connect(m_pGroupDataList, SIGNAL(adjustFinished()), this, SLOT(onGroupItemAdjustFinished()));
}

void AdjustListViewWidget::initQlistView(QListView *listview)
{
    if (listview == nullptr) {
        return;
    }

    // listview->setFrameShape(QFrame::NoFrame);
    // listview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // listview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // listview->setUniformItemSizes(true);
    ////setSpacing(5);
    // listview->setMouseTracking(true);
    // listview->setContentsMargins(0, 0, 0, 0);
    ////下面三句实现流式布局
    // listview->setViewMode(QListView::IconMode);
    // listview->setFlow(QListView::LeftToRight);
    // listview->setResizeMode(QListView::Adjust);

    // listview->setIconSize(QSize(25, 25));
    listview->setFocusPolicy(Qt::NoFocus); // 这样可禁用tab键和上下方向键并且除去复选框
    // listview->setFixedHeight(320);
    listview->setFont(QFont("宋体", 10, QFont::DemiBold));
    listview->setStyleSheet(
            //"*{outline:0px;}"  //除去复选框
            "QListView{background:rgb(245, 245, 247); border:0px; margin:0px 0px 0px 0px;}"
            "QListView::Item{height:40px; border:0px; padding-left:14px; color:rgba(200, 40, 40, 255);}"
            "QListView::Item:hover{color:rgba(40, 40, 200, 255); padding-left:14px;}"
            "QListView::Item:selected{color:rgba(40, 40, 200, 255); padding-left:15px;}");
}

// listmodel
CustomListModel::CustomListModel(QObject *parent /*= nullptr*/) : QAbstractListModel(parent) { }

CustomListModel::~CustomListModel() { }

int CustomListModel::rowCount(const QModelIndex &parent /*= QModelIndex() */) const
{
    return m_data.size();
}

int CustomListModel::columnCount(const QModelIndex &parent /*= QModelIndex() */) const
{
    return 1;
}

QVariant CustomListModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole && index.row() < m_data.size()) {
        return m_data[index.row()];
    }
    return QVariant();
}

void CustomListModel::setListData(const QStringList &listdata)
{
    beginResetModel();
    m_data.clear();
    m_data = listdata;
    endResetModel();
}

// deleaget
CustomListDelegate::CustomListDelegate(QObject *parent) { }

void CustomListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*painter->save();
    QRectF textRect(option.rect.x() + 8, option.rect.y() + 8, option.rect.width() - 16,
            option.rect.height() - 16);
    painter->drawText(textRect, index.data().toString());
    painter->restore();*/
    QStyleOptionViewItemV3 option3(option);
    QWidget *dragwidget = const_cast<QWidget *>(option3.widget);
    AdjustListView *dragView = dynamic_cast<AdjustListView *>(dragwidget);
    if (dragView == nullptr) {
        return;
    }
    bool isDraging = dragView->isDraging();

    QRect rect = option.rect;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    if (option.state & (QStyle::State_MouseOver | QStyle::State_Selected)) {

        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(dragView->model());
        QStandardItem *item = listModel->item(index.row());
        // item->setIcon(item->Img_hover);

        if (option.state & QStyle::State_MouseOver) { }
        if (option.state & QStyle::State_Selected) {
            painter->setBrush(QColor(180, 0, 0));
            painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

            painter->setBrush(QColor(230, 231, 234));
            painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());
        }
    } else {
        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(dragView->model());
        QStandardItem *item = listModel->item(index.row());
        // item->setIcon(item->Img);
    }

    // begin drag
    if (isDraging) {
        int theDragRow = dragView->dragRow();
        int theSelectedRow = dragView->selectedRow();
        int UpRow = dragView->highlightedRow();
        int DownRow = UpRow + 1;
        int rowCount = dragView->model()->rowCount() - 1;

        // 只绘制空隙,而不绘制DropIndicator(这种情况是当拖拽行非选中行时，需要在选中行上面或者下面绘制空隙用来显示DropIndicator)
        if (index.row() == theSelectedRow && theDragRow != theSelectedRow) {
            if (index.row() == UpRow && index.row() != theDragRow - 1) {
                int offset = 3;
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1);
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH + POLYGON) + 1);
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - (offset + WIDTH) + 1);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (offset + WIDTH) + 1);
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (offset + WIDTH + POLYGON) + 1);
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - (offset + WIDTH) + 1);

                painter->setBrush(QColor(245, 245, 247));
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1, rect.width(),
                                  offset + WIDTH);
                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1, rect.width(),
                                  offset + WIDTH);
            } else if (index.row() == DownRow && index.row() != theDragRow + 1) {
                int offset = 3;

                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + offset + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + offset + WIDTH + POLYGON);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + offset + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + offset + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1,
                                                   rect.topRight().y() + offset + WIDTH + POLYGON);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1,
                                                   rect.topRight().y() + offset + WIDTH);

                painter->setBrush(QColor(245, 245, 247));
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), offset + WIDTH);
            }
        }

        // 绘制DropIndicator
        if (index.row() == UpRow && index.row() != theDragRow - 1 && index.row() != theDragRow) {
            painter->setBrush(QColor(66, 133, 244));

            if (UpRow == rowCount) {
                // 到达尾部,三角形向上移动一个WIDTH的距离,以使分隔符宽度*2
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1 - WIDTH);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);

                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - 2 * WIDTH + 1, rect.width(),
                                  2 * WIDTH); // rect
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
            } else {
                // 正常情况,组成上半部分(+1是根据实际情况修正)
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1);
                trianglePolygon_bottomLeft
                        << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1);
                trianglePolygon_bottomRight
                        << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - WIDTH + 1);

                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1, rect.width(),
                                  WIDTH); // rect
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
            }
        } else if (index.row() == DownRow && index.row() != theDragRow + 1 && index.row() != theDragRow) {
            painter->setBrush(QColor(66, 133, 244));

            if (DownRow == 0) {
                // reach the head
                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1,
                                                   rect.topRight().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1,
                                                   rect.topRight().y() + WIDTH + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), 2 * WIDTH); // rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            } else {
                // normal
                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH));
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH));
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1, rect.topRight().y() + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), WIDTH); // rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            }
        }

        // 高亮拖拽行(使拖拽行的样式和选中相同)
        if (index.row() == theDragRow && theDragRow != theSelectedRow) {

            painter->setBrush(QColor(180, 0, 0));
            painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

            painter->setBrush(QColor(230, 231, 234));
            painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

            // opt.state is used to hightlight the font
            QStyleOptionViewItem opt(option);
            opt.state |= QStyle::State_Selected;
            QStyledItemDelegate::paint(painter, opt, index);
            return;
        }

        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    // end drag
    QStyledItemDelegate::paint(painter, option, index);
}

QSize CustomListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(60, 30);
}

AdjustListView::AdjustListView(QWidget *parent)
    : QListView(parent),
      m_IsDraging(false),
      m_theHighlightedRow(-2),
      m_oldHighlightedRow(-2),
      m_theDragRow(-1),
      m_theSelectedRow(-1),
      m_theInsertRow(-1)
{
    setAcceptDrops(true);
}

void AdjustListView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        startPos = event->pos();
    }
    QListView::mousePressEvent(event);
}

void AdjustListView::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->pos() - startPos).manhattanLength() > 5)
        return;

    QModelIndex index = indexAt(event->pos());
    setCurrentIndex(index); // 鼠标relesse时才选中
    QListView::mouseReleaseEvent(event);
}

void AdjustListView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if ((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance())
            return;

        QModelIndex theDragIndex = indexAt(startPos);
        m_theDragRow = theDragIndex.row();
        m_theSelectedRow = currentIndex().row();
        // setCurrentIndex(theDragIndex);  //拖拽即选中

        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
        QStandardItem *theDragItem = listModel->item(m_theDragRow);

        //[1]把拖拽数据放在QMimeData容器中
        QString text = theDragItem->text();
        /* QIcon icon = theDragItem->Img;
         QIcon icon_hover = theDragItem->Img_hover;*/
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << text;
        // dataStream << text << icon << icon_hover;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(myMimeType(), itemData);
        //[1]

        //[2]设置拖拽时的缩略图
        // thumbnail* DragImage = new thumbnail(this);
        // DragImage->setupthumbnail(icon_hover, text);
        ////DragImage->setIconSize(18);  //default:20
        // QPixmap pixmap = DragImage->grab();

        QDrag *drag = new QDrag(this);
        drawText(drag, text);
        drag->setMimeData(mimeData);

        //[2]

        // 删除的行需要根据theInsertRow和theDragRow的大小关系来判断
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            int theRemoveRow = -1;
            if (m_theInsertRow < 0 || m_theInsertRow == m_theDragRow || m_theInsertRow == m_theDragRow + 1) {
                return;
            }
            if (m_theInsertRow < m_theDragRow)
                theRemoveRow = m_theDragRow + 1;
            else
                theRemoveRow = m_theDragRow;
            model()->removeRow(theRemoveRow);
            emit adjustFinished();
        }
    }
}

void AdjustListView::dragEnterEvent(QDragEnterEvent *event)
{
    AdjustListView *source = qobject_cast<AdjustListView *>(event->source());
    if (source && source == this) {
        // IsDraging(标志位)判断是否正在拖拽
        m_IsDraging = true;
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void AdjustListView::dragLeaveEvent(QDragLeaveEvent *event)
{
    m_oldHighlightedRow = m_theHighlightedRow;
    m_theHighlightedRow = -2;

    // 之前QListWidget用的是update(QRect)，这里用的是update(QModelIndex)，当然这里也可以使用update(QRect)，只是想换一种方法而已
    update(model()->index(m_oldHighlightedRow, 0));     // UpRow
    update(model()->index(m_oldHighlightedRow + 1, 0)); // DownRow

    m_IsDraging = false; // IsDraging(标志位)判断是否正在拖拽

    m_theInsertRow = -1;
    event->accept();
}

void AdjustListView::dragMoveEvent(QDragMoveEvent *event)
{
    AdjustListView *source = qobject_cast<AdjustListView *>(event->source());
    if (source && source == this) {

        m_oldHighlightedRow = m_theHighlightedRow;
        m_theHighlightedRow = indexAt(event->pos() - QPoint(0, offset())).row();

        // offset() = 19 = 40 / 2 - 1，其中40是行高
        if (event->pos().y() >= offset()) {

            if (m_oldHighlightedRow != m_theHighlightedRow) {
                // 刷新旧区域使dropIndicator消失
                update(model()->index(m_oldHighlightedRow, 0));
                update(model()->index(m_oldHighlightedRow + 1, 0));

                // 刷新新区域使dropIndicator显示
                update(model()->index(m_theHighlightedRow, 0));
                update(model()->index(m_theHighlightedRow + 1, 0));
            } else {
                update(model()->index(m_theHighlightedRow, 0));
                update(model()->index(m_theHighlightedRow + 1, 0));
            }

            m_theInsertRow = m_theHighlightedRow + 1;
        } else {
            m_theHighlightedRow = -1;
            update(model()->index(0, 0));
            update(model()->index(1, 0));
            m_theInsertRow = 0;
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void AdjustListView::dropEvent(QDropEvent *event)
{
    AdjustListView *source = qobject_cast<AdjustListView *>(event->source());
    if (source && source == this) {

        m_IsDraging = false; // IsDraging(标志位)判断是否正在拖拽

        m_oldHighlightedRow = m_theHighlightedRow;
        m_theHighlightedRow = -2;

        // 刷新旧区域使dropIndicator消失
        update(model()->index(m_oldHighlightedRow, 0));
        update(model()->index(m_oldHighlightedRow + 1, 0));

        if (m_theInsertRow == m_theDragRow || m_theInsertRow == m_theDragRow + 1)
            return;

        // 这里我像QListWidget那样调用父类dropEvent(event)发现不起作用(原因尚不明)，没办法，只能删除旧行，插入新行
        //  if(theSelectedRow == theDragRow){
        //  QListView::dropEvent(event);
        //  return;
        // }

        QString text;
        // QIcon icon, icon_hover;
        QByteArray itemData = event->mimeData()->data(myMimeType());
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        // dataStream >> text >> icon >> icon_hover;
        dataStream >> text;

        model()->insertRow(m_theInsertRow);

        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
        listModel->setItem(m_theInsertRow, 0, new QStandardItem(text));

        // 插入行保持选中状态
        if (m_theDragRow == m_theSelectedRow)
            setCurrentIndex(model()->index(m_theInsertRow, 0));

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void AdjustListView::drawText(QDrag *drag, const QString &text)
{
    int DRAWING_ITEM_WIDTH = 80;
    int DRAWING_ITEM_HEIGHT = 30;
    QPixmap pixmapController(DRAWING_ITEM_WIDTH, DRAWING_ITEM_HEIGHT);
    pixmapController.fill(QColor("#FFFFFF"));
    QPainter painter(&pixmapController);
    QPen pen(QColor("#35405C"));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 8));
    QRectF textRect(6, 5, DRAWING_ITEM_WIDTH - 12, DRAWING_ITEM_HEIGHT - 10);
    QRectF drawRect(0, 0, DRAWING_ITEM_WIDTH - 1, DRAWING_ITEM_HEIGHT - 1);
    QRectF fillRect(2, 2, DRAWING_ITEM_WIDTH - 4, DRAWING_ITEM_HEIGHT - 4);
    QFontMetrics fontWidth(painter.font());
    QString elidnote = fontWidth.elidedText(text, Qt::ElideRight, 2 * textRect.width() - 6);
    int textW = painter.fontMetrics().width(text);
    int textH = painter.fontMetrics().height();
    painter.fillRect(fillRect, QColor("#E6ECF5"));
    painter.drawText(textRect, Qt::TextWrapAnywhere | Qt::AlignCenter, elidnote);
    painter.drawRect(drawRect);

    if (!pixmapController.isNull()) {
        drag->setPixmap(pixmapController);
        drag->setHotSpot(QPoint(pixmapController.width() / 2, pixmapController.height() / 2));
    }
}

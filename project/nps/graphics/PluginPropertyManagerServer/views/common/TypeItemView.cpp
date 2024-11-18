#include "TypeItemView.h"

TreeView::TreeView(QWidget *parent) : KTreeView(parent) { }

TreeView::~TreeView() { }

TableView::TableView(QWidget *parent /*= nullptr*/) : KTableView(parent)
{
    this->horizontalHeader()->setHighlightSections(false);
    this->horizontalHeader()->setMaximumHeight(60); // 限制表头的自适应高度值最大为60px
    this->verticalHeader()->setDefaultSectionSize(26);
    this->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QObject::connect(this->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onVHeaderSectionClicked(int)));

    this->setEditTriggers(QAbstractItemView::DoubleClicked);
    int horizontalHeaderHeight = this->horizontalHeader()->height();
    int rowHeight = 50;
    this->setMinimumHeight(horizontalHeaderHeight + rowHeight);
}

TableView::~TableView() { }

QModelIndexList TableView::getSelectedIndexes()
{
    return m_modelIndexList;
}

void TableView::mousePressEvent(QMouseEvent *e)
{
    // 只处理左键
    if (e->button() == Qt::LeftButton) {
        QModelIndex index = this->indexAt(e->pos());
        if (index.isValid()) {
            m_bMousePressed = true;
        }
    }
    QTableView::mousePressEvent(e);
}

void TableView::mouseReleaseEvent(QMouseEvent *e)
{
    QTableView::mouseReleaseEvent(e);
    QModelIndex index = this->indexAt(e->pos());
    if (index.isValid()) {
        int rows = getSelectedRows();
        emit selectedRowsChanged(getSelectedRows());
    } else {
        emit selectedRowsChanged(0);
    }
}

void TableView::mouseMoveEvent(QMouseEvent *e)
{
    // 只处理左键
    QTableView::mouseMoveEvent(e);
    if (m_bMousePressed) {
        QModelIndex index = this->indexAt(e->pos());
        if (index.isValid()) {
            emit selectedRowsChanged(getSelectedRows());
        }
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void TableView::focusOutEvent(QFocusEvent *e)
{
    clearSelection();
    emit selectedRowsChanged(0);
    QWidget::focusOutEvent(e);
}

void TableView::focusInEvent(QFocusEvent *e)
{
    emit selectedRowsChanged(getSelectedRows());
    QWidget::focusInEvent(e);
}

int TableView::getSelectedRows()
{
    QModelIndexList allselects = this->selectedIndexes();
    QList<int> rowlist;
    m_modelIndexList.clear();
    foreach (QModelIndex index, allselects) {
        if (!index.isValid()) {
            continue;
        }
        bool haverow = false;
        for (int i = 0; i < rowlist.size(); ++i) {
            if (rowlist[i] == index.row()) {
                haverow = true;
                break;
            }
        }
        if (!haverow) {
            rowlist.append(index.row());
            m_modelIndexList.append(index);
        }
    }
    return rowlist.size();
}

void TableView::onVHeaderSectionClicked(int logicalIndex)
{
    emit selectedRowsChanged(getSelectedRows());
    emit selectedIndexsChanged(m_modelIndexList);
}

CWidget::CWidget(QWidget *parent) : QWidget(parent) { }

CWidget::~CWidget() { }

bool CWidget::saveData()
{
    return false;
}

void CWidget::setCWidgetReadOnly(bool bReadOnly)
{
    this->setDisabled(bReadOnly);
}

bool CWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    return true;
}

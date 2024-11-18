#include "tableheaderview.h"
#include <QCheckBox>
#include <QPainter>
#include <QStyleFactory>

TableHeaderView::TableHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent), m_bIsAllChecked(-1), m_bCheckboxEnable(true), m_ptMouseRelease(QPoint())
{
    setSectionsClickable(true);
    setStretchLastSection(true);
    setHighlightSections(false);
    setSectionResizeMode(QHeaderView::Interactive);

    connect(this, SIGNAL(sectionClicked(int)), this, SLOT(onSectionsClicked(int)));
}

TableHeaderView::~TableHeaderView() { }

void TableHeaderView::setColumnCheckable(int col, bool checkable)
{
    if (checkable) {
        // m_columnCheckedMap[col] = false;
    } else if (m_columnCheckedMap.contains(col)) {
        m_columnCheckedMap.remove(col);
    }
}

void TableHeaderView::setCheckBoxEnableStatus(bool enable)
{
    m_bCheckboxEnable = enable;
    setEnabled(enable);
    update();
}

void TableHeaderView::setColAllSelected()
{
    m_columnCheckedMap[0] = true;
    adjustSize();
}

void TableHeaderView::onSectionsClicked(int index)
{
    if (0 == index) {
        QRect r = QRect(5, 5, 16, 16);
        if (r.contains(m_ptMouseRelease)) {
            m_columnCheckedMap[index] = !m_columnCheckedMap.value(index);
            emit columuSectionClicked(index, m_columnCheckedMap[index]);
        }
    }
}

void TableHeaderView::setHeaderCheckStatus(TableHeaderView::CheckState checkedStatus, bool ColCheckedFlag)
{
    m_columnCheckedMap[0] = ColCheckedFlag;
    m_bIsAllChecked = checkedStatus;
    viewport()->update();
}

void TableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (m_columnCheckedMap.contains(logicalIndex)) {
        QStyleOptionButton styleOption;
        styleOption.rect = QRect(rect.left() + 5, rect.top() + 5, 16, 16);
        styleOption.state = m_bCheckboxEnable ? QStyle::State_Enabled : QStyle::State_None;

        QCheckBox *pCheckBox = new QCheckBox(dynamic_cast<QWidget *>(this->parent()));

        if (m_bIsAllChecked == CheckState::PartChecked) {
            styleOption.state |= QStyle::State_NoChange;
        } else if (m_bIsAllChecked == CheckState::AllChecked) {
            styleOption.state |= QStyle::State_On;
        } else if (m_bIsAllChecked == CheckState::NoChecked) {
            styleOption.state |= QStyle::State_Off;
        }

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &styleOption, painter, pCheckBox);
    }
}

void TableHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    m_ptMouseRelease = e->pos();
    QRect r = QRect(5, 5, 16, 16);
    int nIndex = logicalIndexAt(e->pos());
    if (0 == nIndex) {
        e->ignore();
        m_columnCheckedMap[0] = !m_columnCheckedMap.value(0);
        emit columuSectionClicked(0, m_columnCheckedMap[0]);
    } else {
        e->accept();
        QHeaderView::mouseReleaseEvent(e);
    }
}

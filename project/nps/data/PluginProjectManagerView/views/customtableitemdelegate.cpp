#include "customtableitemdelegate.h"

#include <QComboBox>

#include "GlobalAssistant.h"
#include "TableModelDataModel.h"

CustomTableItemDelegate::CustomTableItemDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

CustomTableItemDelegate::~CustomTableItemDelegate() { }

QWidget *CustomTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (!index.isValid() || !index.data(NPS::ModelDataEditRole).toBool()) {
        return nullptr;
    }
    return NPS::createWidget(index.data(NPS::ModelDataTypeRole).toString(), index.data(NPS::ModelDataRangeRole),
                             parent);
}

void CustomTableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    NPS::setWidgetControlData(editor, index.data(NPS::ModelDataRealValueRole),
                              index.data(NPS::ModelDataTypeRole).toString(), index.data(NPS::ModelDataRangeRole));
}

void CustomTableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (model == nullptr || !index.isValid() || editor == nullptr) {
        return;
    }

    model->setData(index,
                   NPS::getWidgetDataMaptoRealData(editor, index.data(NPS::ModelDataTypeRole).toString(),
                                                   index.data(NPS::ModelDataRangeRole)),
                   Qt::EditRole);
}

void CustomTableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void CustomTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    if (!index.isValid()) {
        return;
    }
    if (index.data(NPS::ModelDataEditRole).toBool()
        && NPS::TCOMBOBOX_CONTROLTYPES.contains(index.data(NPS::ModelDataTypeRole).toString())) {
        // 画个箭头
        painter->save();
        QRectF rect;
        QPointF p1(option.rect.x() + option.rect.width() - (NPS::ARROWLEFTDIS + NPS::ARROWWIDTH),
                   option.rect.y() + (option.rect.height() - NPS::ARROWHEIGHT) / 2);
        QPointF p2(option.rect.x() + option.rect.width() - (NPS::ARROWLEFTDIS + NPS::ARROWWIDTH / 2),
                   option.rect.y() + (option.rect.height() - NPS::ARROWHEIGHT) / 2 + NPS::ARROWHEIGHT);
        QPointF p3(option.rect.x() + option.rect.width() - NPS::ARROWLEFTDIS,
                   option.rect.y() + (option.rect.height() - NPS::ARROWHEIGHT) / 2);
        QPen pen;
        pen.setColor(QColor("#554d4b")); // #574f4d 		pen.setWidthF(1.5);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->drawLine(p1, p2);
        painter->drawLine(p2, p3);
        painter->restore();
    }
}

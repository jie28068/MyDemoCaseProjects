#include "qwcomboboxdelegate.h"

#include    <QComboBox>

QWComboBoxDelegate::QWComboBoxDelegate(QObject *parent):QItemDelegate(parent)
{

}

QWidget *QWComboBoxDelegate::createEditor(QWidget *parent,
       const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *editor = new QComboBox(parent);

    editor->addItem("you");
    editor->addItem("lang");
    editor->addItem("yiban");
    editor->addItem("bujige");

    return editor;
}

void QWComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString str = index.model()->data(index, Qt::EditRole).toString();

    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentText(str);
}

void QWComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);

    QString str = comboBox->currentText();

    model->setData(index, str, Qt::EditRole);
}

void QWComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

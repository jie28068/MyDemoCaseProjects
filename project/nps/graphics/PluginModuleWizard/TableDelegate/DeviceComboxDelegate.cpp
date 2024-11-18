#include "DeviceComboxDelegate.h"

DeviceComboxDelegate::DeviceComboxDelegate(QObject *parent) : TableBaseDelegate(parent)
{
    typeList << Global::DataType_Double << Global::DataType_Bool << Global::DataType_Int << Global::DataType_String
             << Global::DataType_Enum;
}

QWidget *DeviceComboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (!index.isValid()) {
        return nullptr;
    }
    switch (index.column()) {
    case DeviceWizardTableModel::ParamType: {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(typeList);
        return editor;
    }
    case DeviceWizardTableModel::ParamMode: {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(Global::variableState);
        return editor;
    }
    case DeviceWizardTableModel::ControlTyep: {
        QComboBox *editor = new QComboBox(parent);
        auto names = Global::ControlTyeName.keys();
        std::sort(names.begin(), names.end(),
                  [](const QString &s1, const QString &s2) { return s1.length() < s2.length(); });
        editor->addItems(names);
        return editor;
    }
    default: {
        QLineEdit *editor = new QLineEdit(parent);
        return editor;
    }
    }
}

void DeviceComboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    switch (index.column()) {
    case DeviceWizardTableModel::ParamType: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox != nullptr) {
            return;
        }
        QString value = index.data(Qt::EditRole).toString();
        combox->setCurrentIndex(combox->findText(value));
    }
    case DeviceWizardTableModel::ParamMode: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        int value = index.data(Qt::EditRole).toInt();
        combox->setCurrentIndex(value);
        break;
    }
    case DeviceWizardTableModel::ControlTyep: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        QString value = index.data(Qt::EditRole).toString();
        combox->setCurrentIndex(combox->findText(value));
        break;
    }
    default: {
        QLineEdit *edit = static_cast<QLineEdit *>(editor);
        if (edit != nullptr) {
            edit->setText(index.data(Qt::DisplayRole).toString());
        }
    }
    }
}

void DeviceComboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid() || model == nullptr) {
        return;
    }
    switch (index.column()) {
    case DeviceWizardTableModel::ParamType: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        QString value = index.data(Qt::EditRole).toString();
        if (combox == nullptr) {
            break;
        }
        if (combox->currentText() == "") {
            model->setData(index, value, Qt::EditRole);
        } else {
            model->setData(index, combox->currentText(), Qt::EditRole);
        }
        break;
    }
    case DeviceWizardTableModel::ControlTyep: {
        QString value = index.data(Qt::EditRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox->currentText() == "") {
            model->setData(index, value, Qt::EditRole);
        } else {
            model->setData(index, combox->currentText(), Qt::EditRole);
        }
        break;
    }
    case DeviceWizardTableModel::ParamMode: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        QString value = index.data(Qt::EditRole).toString();
        if (combox == nullptr) {
            break;
        }
        if (combox->currentText() == "") {
            model->setData(index, value, Qt::EditRole);
        } else {
            model->setData(index, combox->findText(combox->currentText()), Qt::EditRole);
        }
        break;
    }
    default: {
        QLineEdit *edit = static_cast<QLineEdit *>(editor);
        if (edit != nullptr) {
            model->setData(index, edit->text(), Qt::EditRole);
        }
        break;
    }
    }
}

void DeviceComboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

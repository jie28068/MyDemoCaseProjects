#include "ElecComboxDelegate.h"

ElecComboxDelegate::ElecComboxDelegate(QObject *parent) : TableBaseDelegate(parent)
{
    typeList << Global::DataType_Double << Global::DataType_Bool << Global::DataType_Int << Global::DataType_String
             << Global::DataType_Enum << Global::DataType_Enummap;
    portList << Global::DataType_Double << Global::DataType_Bool;
}

ElecComboxDelegate::~ElecComboxDelegate() { }

QWidget *ElecComboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (!index.isValid()) {
        return nullptr;
    }

    switch (index.column()) {
    case ElecWizardTableModel::ParamType: {
        QComboBox *editor = new QComboBox(parent);
        editor->setEditable(true);
        auto str = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
        if (str == tr("Port")) {
            editor->addItems(portList); // 若是端口设置该类型组
        } else {
            editor->addItems(typeList);
        }
        return editor;
    }
    case ElecWizardTableModel::ParamRange: {
        auto *editor = new QLineEdit(parent);
        auto str = index.sibling(index.row(), ElecWizardTableModel::ParamType).data().toString();
        if (str == Global::DataType_Double || str == Global::DataType_Int) {
            editor->setValidator(
                    new QRegExpValidator(QRegExp(QString("^(\\[|\\()(\\-?\\d*),\\s*(\\-?\\d*)(\\]|\\))$")), editor));
        }
        return editor;
    }
    case ElecWizardTableModel::ParamMode: {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(Global::variableState);
        return editor;
    }
    case ElecWizardTableModel::ParamPortType: {
        auto str = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
        if (str == tr("Port")) {
            QComboBox *editor = new QComboBox(parent);
            editor->addItems(Global::portTypeList);
            return editor;
        }
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
    case ElecWizardTableModel::ParemPortPosition: {
        auto *editor = new QLineEdit(parent);
        editor->setValidator(
                new QRegExpValidator(QRegExp(QString("^(\\()(\\d+)(\\.\\d+)?,\\s*(\\d+)(\\.\\d+)?(\\))$")), editor));
        return editor;
    }
    case ElecWizardTableModel::ParamValue: {
        auto str = index.sibling(index.row(), ElecWizardTableModel::ParamType).data().toString();
        if (str == Global::DataType_Bool) {
            QComboBox *editor = new QComboBox(parent);
            editor->addItems(QStringList() << "true"
                                           << "false");
            return editor;
        }
        auto *editor = new QLineEdit(parent);
        return editor;
    }
    case ElecWizardTableModel::ControlTyep: {
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

void ElecComboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    switch (index.column()) {
    case ElecWizardTableModel::ParamType: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        QString value = index.data(Qt::EditRole).toString();
        typeList.append(value);
        typeList = typeList.toSet().toList();
        combox->setCurrentIndex(combox->findText(value));
        break;
    }
    case ElecWizardTableModel::ParamMode: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        int value = index.data(Qt::EditRole).toInt();
        combox->setCurrentIndex(value);
        break;
    }
    case ElecWizardTableModel::ParamPortType: {
        auto str = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
        if (str == tr("Port")) {
            QComboBox *combox = static_cast<QComboBox *>(editor);
            if (combox == nullptr) {
                return;
            }
            int value = index.data(Qt::EditRole).toInt();
            combox->setCurrentIndex(value);
            break;
        }
    }
    case ElecWizardTableModel::ParamValue: {
        auto type = index.sibling(index.row(), ElecWizardTableModel::ParamType).data().toString();
        if (Global::DataType_Bool == type) {
            bool value = index.data(Qt::EditRole).toBool();
            QComboBox *combox = static_cast<QComboBox *>(editor);
            if (combox != nullptr) {
                int curIndex = combox->findText(value ? "true" : "false");
                combox->setCurrentIndex(curIndex);
            }
        } else {
            QLineEdit *edit = static_cast<QLineEdit *>(editor);
            if (edit != nullptr) {
                edit->setText(index.data(Qt::EditRole).toString());
            }
        }
        break;
    }
    case ElecWizardTableModel::ControlTyep: {
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
        break;
    }
    }
}

void ElecComboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid() || model == nullptr) {
        return;
    }
    switch (index.column()) {
    case ElecWizardTableModel::ParamType: {
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
    case ElecWizardTableModel::ParamMode: {
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
    case ElecWizardTableModel::ParamPortType: {
        auto str = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
        if (str == tr("Port")) {
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
    }
    case ElecWizardTableModel::ParamValue: {
        auto type = index.sibling(index.row(), ElecWizardTableModel::ParamType).data().toString();
        if (Global::DataType_Bool == type) {
            QComboBox *combox = static_cast<QComboBox *>(editor);
            if (combox != nullptr) {
                model->setData(index, combox->currentText(), Qt::EditRole);
            }
        } else {
            QLineEdit *edit = static_cast<QLineEdit *>(editor);
            if (edit != nullptr) {
                model->setData(index, edit->text(), Qt::EditRole);
            }
        }
        break;
    }
    case ElecWizardTableModel::ControlTyep: {
        QString value = index.data(Qt::EditRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox->currentText() == "") {
            model->setData(index, value, Qt::EditRole);
        } else {
            model->setData(index, combox->currentText(), Qt::EditRole);
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

void ElecComboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

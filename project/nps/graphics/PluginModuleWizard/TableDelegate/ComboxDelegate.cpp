#include "ComboxDelegate.h"

ComboxDelegate::ComboxDelegate(QObject *parent) : TableBaseDelegate(parent)
{
    typeList << Global::DataType_Double << Global::DataType_Bool << Global::DataType_KccString
             << Global::DataType_DoubleVector << Global::DataType_DoubleComplex << Global::DataType_VoidBuffer
             << Global::DataType_Matrix;
    limitTypeList << Global::LimitType_NO << Global::LimitType_UPPER << Global::LimitType_LOWER;
}

ComboxDelegate::~ComboxDelegate() { }

void ComboxDelegate::onCurrentIndexChanged(int index)
{
    QComboBox *comboBox = static_cast<QComboBox *>(sender());
    QString str = comboBox->itemText(index);
    if (str == Global::DataType_Matrix) {
        MatrixWidget *m_widget = new MatrixWidget(comboBox);
        KCustomDialog *dialog = new KCustomDialog(Global::DataType_Matrix, m_widget,
                                                  KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::Ok, comboBox);
        if (dialog->exec() == KBaseDlgBox::Ok) {
            QString str = m_widget->getResultName();
            comboBox->addItem(str);
            comboBox->setCurrentText(str);
        } else {
            comboBox->setCurrentText(m_str);
        }
    }
}

QWidget *ComboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (!index.isValid()) {
        return nullptr;
    }

    switch (index.column()) {
    case WizardTableModel::ParamType: {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(typeList);
        switch (getParamType(index)) {
        case ComboxDelegate::ParamType_Other: {
            editor->setEditable(true);
            connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
            break;
        }
        case ComboxDelegate::ParamType_SumSign:
        case ComboxDelegate::ParamType_AddSign: {
            editor->setEditable(false);
            editor->setEnabled(false);
            break;
        }
        default:
            break;
        }
        return editor;
    }
    case WizardTableModel::LimitType: {
        QString category = index.sibling(index.row(), WizardTableModel::ParamCategory).data(Qt::DisplayRole).toString();
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(limitTypeList);
        if (category != tr("Module Parameters") && category != tr("Input Variables")) {
            editor->setEnabled(false);
        } else {
            editor->setEnabled(true);
            editor->setEditable(false);
        }
        return editor;
    }
    case WizardTableModel::ControlTyep: {
        auto str = index.sibling(index.row(), WizardTableModel::ParamType).data().toString();
        QComboBox *editor = new QComboBox(parent);
        auto names = Global::ControlTyeName.keys();
        std::sort(names.begin(), names.end(),
                  [](const QString &s1, const QString &s2) { return s1.length() < s2.length(); });
        if (str.startsWith("Matrix")) {
            editor->addItems(names);
        } else {
            names.removeAll(QObject::tr("MatrixType"));
            editor->addItems(names);
        }

        return editor;
    }
    case WizardTableModel::ParamValue: {
        // 返回当前值所对应的类型字段值
        QString type = index.sibling(index.row(), WizardTableModel::ParamType).data(Qt::EditRole).toString();
        switch (getParamType(index)) {
        case ComboxDelegate::ParamType_Other: {
            if (Global::DataType_Bool == type) {
                QComboBox *editor = new QComboBox(parent);
                editor->addItems(QStringList() << "true"
                                               << "false");
                return editor;
            } else {
                auto *editor = new QLineEdit(parent);
                if (Global::DataType_DoubleVector == type) {
                    editor->setValidator(new QRegExpValidator(QRegExp(Global::REG_VECTOR_DOUBLE), editor));
                } else if (Global::DataType_DoubleComplex == type) {
                    editor->setValidator(new QRegExpValidator(QRegExp(Global::REG_COMPLEX_DOUBLE), editor));
                } else if (Global::DataType_Double == type) {
                    editor->setValidator(new QRegExpValidator(QRegExp(Global::REG_DOUBLE), editor));
                }
                return editor;
            }
        }
        case ComboxDelegate::ParamType_SumSign: {
            auto *editor = new QLineEdit(parent);
            editor->setValidator(new QRegExpValidator(QRegExp(Global::REG_SIGNVECTOR), editor));
            return editor;
        }
        case ComboxDelegate::ParamType_AddSign: {
            auto *editor = new QLineEdit(parent);
            editor->setValidator(new QRegExpValidator(QRegExp(Global::REG_SIGNVECTOR), editor));
            return editor;
        }
        default:
            break;
        }
    }
    case WizardTableModel::ParamMode: {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(Global::variableState);
        return editor;
    }
    default: {
        QLineEdit *editor = new QLineEdit(parent);
        return editor;
    }
    }
}

void ComboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    switch (index.column()) {
    case WizardTableModel::ParamType: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        QString value = index.data(Qt::EditRole).toString();
        m_str = value;
        typeList.append(value);
        typeList = typeList.toSet().toList();
        switch (getParamType(index)) {
        case ComboxDelegate::ParamType_Other: {
            combox->setCurrentIndex(combox->findText(value));
            break;
        }
        case ComboxDelegate::ParamType_SumSign:
        case ComboxDelegate::ParamType_AddSign: {
            combox->setCurrentIndex(combox->findText(Global::DataType_KccString));
            break;
        }
        default:
            break;
        }
        break;
    }
    case WizardTableModel::LimitType: {
        QString category = index.sibling(index.row(), WizardTableModel::ParamCategory).data(Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr || (category != tr("Module Parameters") && category != tr("Input Variables"))) {
            return;
        }
        int value = index.data(Qt::EditRole).toInt();
        combox->setCurrentIndex(value);
        break;
    }
    case WizardTableModel::ControlTyep: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        QString value = index.data(Qt::EditRole).toString();
        combox->setCurrentIndex(combox->findText(value));
        break;
    }
    case WizardTableModel::ParamValue: {
        switch (getParamType(index)) {
        case ComboxDelegate::ParamType_Other: {
            QString type = index.sibling(index.row(), WizardTableModel::ParamType).data(Qt::EditRole).toString();
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
        case ComboxDelegate::ParamType_SumSign:
        case ComboxDelegate::ParamType_AddSign: {
            QLineEdit *edit = static_cast<QLineEdit *>(editor);
            if (edit != nullptr) {
                edit->setText(index.data(Qt::EditRole).toString());
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case WizardTableModel::ParamMode: {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr) {
            return;
        }
        int value = index.data(Qt::EditRole).toInt();
        combox->setCurrentIndex(value);
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

void ComboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid() || model == nullptr) {
        return;
    }
    switch (index.column()) {
    case WizardTableModel::ParamType: {
        QString value = index.data(Qt::EditRole).toString();
        switch (getParamType(index)) {
        case ComboxDelegate::ParamType_Other: {
            QComboBox *combox = static_cast<QComboBox *>(editor);
            if (combox == nullptr) {
                break;
            }
            QModelIndex index_controlType = index.sibling(index.row(), WizardTableModel::ControlTyep);
            QModelIndex index_controlValue = index.sibling(index.row(), WizardTableModel::ControlValue);
            if (combox->currentText() == "") {
                model->setData(index, value, Qt::EditRole);
            } else {
                auto str = combox->lineEdit()->text();
                if (str.startsWith("Matrix") || str.startsWith("RowVector")
                    || str.startsWith("Vector")) { // 由用户自定义输入的矩阵类型
                    Kcc::BlockDefinition::MatrixInfo marix = getMatrixDataFormString(str);
                    QVariant variant = QVariant::fromValue(marix);
                    model->setData(index_controlValue, variant);
                    model->setData(
                            index_controlType,
                            Global::ControlTyeName.key(Kcc::BlockDefinition::RoleDataDefinition ::ControlMatrixType));
                } else {
                    if (value != str) {
                        if (str == Global::DataType_Bool) { // bool默认为勾选框
                            model->setData(index_controlType,
                                           Global::ControlTyeName.key(
                                                   Kcc::BlockDefinition::RoleDataDefinition ::ControlTypeCheckbox));
                        } else { // 其余默认为数学表达式
                            model->setData(index_controlType,
                                           Global::ControlTyeName.key(
                                                   Kcc::BlockDefinition::RoleDataDefinition ::ControlMathExpression));
                        }
                        model->setData(index_controlValue, "");
                    }
                }
                model->setData(index, str, Qt::EditRole);
            }
            break;
        }
        case ComboxDelegate::ParamType_SumSign:
        case ComboxDelegate::ParamType_AddSign: {
            model->setData(index, Global::DataType_KccString, Qt::EditRole);
            break;
        }
        default:
            break;
        }
        break;
    }
    case WizardTableModel::LimitType: {
        QString category = index.sibling(index.row(), WizardTableModel::ParamCategory).data(Qt::DisplayRole).toString();
        QString value = index.data(Qt::EditRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox == nullptr || (category != tr("Module Parameters") && category != tr("Input Variables"))) {
            break;
        }
        if (combox->currentText() == "") {
            model->setData(index, value, Qt::EditRole);
        } else {
            model->setData(index, combox->currentIndex(), Qt::EditRole);
        }
        break;
    }
    case WizardTableModel::ControlTyep: {
        QString value = index.data(Qt::EditRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        if (combox->currentText() == "") {
            model->setData(index, value, Qt::EditRole);
        } else {
            model->setData(index, combox->currentText(), Qt::EditRole);
        }
        break;
    }
    case WizardTableModel::ParamValue: {
        QModelIndex typeIndex = index.sibling(index.row(), WizardTableModel::ParamType);
        QString type = typeIndex.data(Qt::EditRole).toString();
        // 特殊处理，sum的sign只能输入+-
        switch (getParamType(index)) {
        case ComboxDelegate::ParamType_Other: {
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
        case ComboxDelegate::ParamType_SumSign:
        case ComboxDelegate::ParamType_AddSign: {
            QLineEdit *edit = static_cast<QLineEdit *>(editor);
            if (edit != nullptr) {
                model->setData(index, edit->text(), Qt::EditRole);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case WizardTableModel::ParamMode: {
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

void ComboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

ComboxDelegate::ParamType ComboxDelegate::getParamType(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return ComboxDelegate::ParamType_Other;
    }
    QString varnamestr = index.sibling(index.row(), WizardTableModel::Name).data().toString();
    QString prototypename = index.data(Global::PrototypeNameRole).toString();
    // 特殊处理，sum的sign只能输入+-任意个数
    if (Global::PROTOTYPENAME_SUM == prototypename && Global::KEYWORDS_SUMADD_SIGN == varnamestr) {
        return ComboxDelegate::ParamType_SumSign;
    } else if (Global::PROTOTYPENAME_ADD == prototypename && Global::KEYWORDS_SUMADD_SIGN == varnamestr) {
        return ComboxDelegate::ParamType_AddSign;
    }
    return ComboxDelegate::ParamType_Other;
}

Kcc::BlockDefinition::MatrixInfo ComboxDelegate::getMatrixDataFormString(const QString &str) const
{
    Kcc::BlockDefinition::MatrixInfo marix;
    // if (m_widget) {
    //     marix.rowNo = m_widget->getRow();
    //     marix.colNo = m_widget->getClounm();
    //     marix.dataType = m_widget->getType();
    // } else
    QString value = str;
    // 对矩阵类型字符串逐一拆分，得到行列与类型
    if (str.startsWith("Matrix")) {
        value.remove("Matrix");
    } else if (str.startsWith("RowVector")) {
        value.remove("RowVector");
    } else if (str.startsWith("Vector")) {
        value.remove("Vector");
    }
    QRegExp regex("[^0-9X]+$");
    int pos = regex.indexIn(value);
    if (pos != -1) {
        QString result = value.mid(pos);
        if (result == "i") {
            marix.dataType = "int";
        } else if (result == "f") {
            marix.dataType = "float";
        } else if (result == "d") {
            marix.dataType = "double";
        } else if (result == "cf") {
            marix.dataType = "complex<float>";
        } else if (result == "cd") {
            marix.dataType = "complex<double>";
        } else {
            marix.dataType = "double";
        }
        QString row_cloum = value.left(pos);
        if (row_cloum.size() == 2) {
            marix.rowNo = row_cloum.at(0);
            marix.colNo = row_cloum.at(1);
        } else if (row_cloum.size() == 1) {
            if (str.startsWith("Matrix")) {
                marix.rowNo = row_cloum.at(0);
                marix.colNo = row_cloum.at(0);
            } else if (str.startsWith("RowVector")) {
                marix.rowNo = "1";
                marix.colNo = row_cloum.at(0);
            } else if (str.startsWith("Vector")) {
                marix.rowNo = row_cloum.at(0);
                marix.colNo = "1";
            }
        } else {
            marix.rowNo = "";
            marix.colNo = "";
        }
    }
    return marix;
}

MatrixWidget::MatrixWidget(QWidget *parent) : QWidget(parent), type("int"), row("-1"), clounm("-1")
{
    QLabel *rowLabel = new QLabel(tr("Row"));
    QLabel *clounmLabel = new QLabel(tr("Clounm"));
    QLabel *typeLabel = new QLabel(tr("Type"));
    QLabel *matrixLabel = new QLabel(tr("Matrix Types"));
    resultLabel = new QLabel;
    resultLabel->setText("MatrixXi");
    rowComboBox = new QComboBox;
    rowComboBox->addItems(QStringList() << "-1"
                                        << "1"
                                        << "2"
                                        << "3"
                                        << "4");
    cloumComboBox = new QComboBox;
    cloumComboBox->addItems(QStringList() << "-1"
                                          << "1"
                                          << "2"
                                          << "3"
                                          << "4");
    typeComboBox = new QComboBox;
    typeComboBox->addItems(QStringList() << "int"
                                         << "float"
                                         << "double"
                                         << "complex<float>"
                                         << "complex<double>");
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(2);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(rowLabel, 0, 0);
    layout->addWidget(rowComboBox, 0, 1);
    layout->addWidget(clounmLabel, 1, 0);
    layout->addWidget(cloumComboBox, 1, 1);
    layout->addWidget(typeLabel, 2, 0);
    layout->addWidget(typeComboBox, 2, 1);
    layout->addWidget(line, 3, 0, 1, 2);
    layout->addWidget(matrixLabel, 4, 0);
    layout->addWidget(resultLabel, 4, 1);

    setLayout(layout);
    connect(rowComboBox, &QComboBox::currentTextChanged, this, &MatrixWidget::onChangedText);
    connect(cloumComboBox, &QComboBox::currentTextChanged, this, &MatrixWidget::onChangedText);
    connect(typeComboBox, &QComboBox::currentTextChanged, this, &MatrixWidget::onChangedText);
}

MatrixWidget::~MatrixWidget() { }

QString MatrixWidget::typeConversion(const QString &str)
{
    if (str == "int") {
        return "i";
    } else if (str == "float") {
        return "f";
    } else if (str == "double") {
        return "d";
    } else if (str == "complex<float>") {
        return "cf";
    } else if (str == "complex<double>") {
        return "cd";
    }
    return QString();
}

QString MatrixWidget::getResultName()
{
    if (resultLabel) {
        return resultLabel->text();
    }
    return QString();
}

void MatrixWidget::onChangedText(const QString &str)
{
    QObject *senderObj = sender();
    QString reshult;
    // 矩阵类型行为1时，前缀是RowVector，此时列不能选1。列为1时，前缀是Vector，此时行不能选1。其他时候前缀都是Matrix。
    if (rowComboBox->currentText() == "1") {
        cloumComboBox->removeItem(cloumComboBox->findText("1"));
    } else {
        if (-1 == cloumComboBox->findText("1")) {
            cloumComboBox->addItem("1");
            cloumComboBox->model()->sort(0, Qt::AscendingOrder);
        }
    }
    if (cloumComboBox->currentText() == "1") {
        rowComboBox->removeItem(rowComboBox->findText("1"));
    } else {
        if (-1 == rowComboBox->findText("1")) {
            rowComboBox->addItem("1");
            rowComboBox->model()->sort(0, Qt::AscendingOrder);
        }
    }
    // end

    if (senderObj == rowComboBox) {
        row = str;
    } else if (senderObj == cloumComboBox) {
        clounm = str;
    } else if (senderObj == typeComboBox) {
        type = str;
    }

    if (row == clounm) {
        reshult = QString("Matrix%1%2").arg(row == "-1" ? "X" : row).arg(typeConversion(type));
    } else {
        if (row == "1") {
            reshult = QString("RowVector%1%2").arg(clounm == "-1" ? "X" : clounm).arg(typeConversion(type));
        } else if (clounm == "1") {
            reshult = QString("Vector%1%2").arg(row == "-1" ? "X" : row).arg(typeConversion(type));
        } else {
            reshult = QString("Matrix%1%2%3")
                              .arg(row == "-1" ? "X" : row)
                              .arg(clounm == "-1" ? "X" : clounm)
                              .arg(typeConversion(type));
        }
    }
    resultLabel->setText(reshult);
}

#include "dispersedselect.h"

// #include "drawobj.h"
#include "GlobalAssistant.h"

DispersedSelect::DispersedSelect(QWidget *parent /*= nullptr*/)
    : CWidget(parent), m_pComboBox(nullptr), m_pLineEdit(nullptr)
{
    initUI();
}

DispersedSelect::~DispersedSelect() { }

// void DispersedSelect::setSelectIndex( const int &index )
//{
//	if (m_pComboBox !=nullptr) {
//		m_pComboBox->setCurrentIndex(index);
//		switch (index) {
//		case 0:
//			setLineEditValue(METHOD_IMPLICIT_TRAP_INTEGRAL);
//			break;
//		case 1:
//			setLineEditValue(METHOD_BACK_EULER);
//			break;
//		case 2:
//			setLineEditValue(METHOD_WEIGHT_INTEGRAL);
//			break;
//		default:
//			break;
//		}
//	}
// }

void DispersedSelect::setSelectValue(const QString &value)
{
    // 后向欧拉
    if ("0" == value) {
        m_pComboBox->setCurrentIndex(1);
        setLineEditValue(METHOD_BACK_EULER);
    } else if ("1" == value) {
        m_pComboBox->setCurrentIndex(0);
        setLineEditValue(METHOD_IMPLICIT_TRAP_INTEGRAL);
    } else {
        m_pComboBox->setCurrentIndex(2);
        setLineEditValue(METHOD_WEIGHT_INTEGRAL);
        m_pLineEdit->setText(value);
    }
}

void DispersedSelect::setMethodLabelName(const QString &strtext)
{
    if (m_mathDispMethod != nullptr) {
        m_mathDispMethod->setText(strtext);
    }
}

QComboBox *DispersedSelect::getComboBox()
{
    return m_pComboBox;
}

QLineEdit *DispersedSelect::getCurrentLineEdit()
{
    return m_pLineEdit;
}

void DispersedSelect::initUI()
{
    QGridLayout *gridlayout = new QGridLayout(this);
    gridlayout->setMargin(0);

    m_mathDispMethod = new QLabel(this);
    // initLabel(m_mathDispMethod);
    m_mathDispMethod->setText("");
    QSizePolicy labelsizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    labelsizePolicy.setHeightForWidth(m_mathDispMethod->sizePolicy().hasHeightForWidth());
    m_mathDispMethod->setSizePolicy(labelsizePolicy);
    gridlayout->addWidget(m_mathDispMethod, 0, 0, 1, 1);

    m_pComboBox = new QComboBox(this);
    m_pComboBox->setView(new QListView());
    QStringList boxlist = QStringList() << METHOD_IMPLICIT_TRAP_INTEGRAL << METHOD_BACK_EULER << METHOD_WEIGHT_INTEGRAL;
    m_pComboBox->addItems(boxlist);
    QSizePolicy bxsizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    bxsizePolicy.setHeightForWidth(m_pComboBox->sizePolicy().hasHeightForWidth());
    m_pComboBox->setSizePolicy(bxsizePolicy);
    // m_pComboBox->setFixedWidth(125);
    gridlayout->addWidget(m_pComboBox, 0, 1, 1, 2);
    QLabel *valueLabel = new QLabel(this);
    QSizePolicy vsizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    vsizePolicy.setHeightForWidth(valueLabel->sizePolicy().hasHeightForWidth());
    valueLabel->setSizePolicy(vsizePolicy);
    valueLabel->setText(WEIGHT_FACTOR);
    m_pLineEdit = new KLineEdit(this);
    m_pLineEdit->setToolTip(NPS::WF_DTM_TIPS);
    initLineEdit(m_pLineEdit);
    gridlayout->addWidget(valueLabel, 1, 0, 1, 1);
    gridlayout->addWidget(m_pLineEdit, 1, 1, 1, 2);
    this->setLayout(gridlayout);

    connect(m_pComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onBoxIndexChanged(const QString &)));
}

void DispersedSelect::initLabel(QLabel *label)
{
    if (label != nullptr) {
        label->setMinimumSize(QSize(120, 0));
        label->setMaximumSize(QSize(120, 16777215));
    }
}

void DispersedSelect::initLineEdit(QLineEdit *lineEdit)
{
    if (lineEdit != nullptr) {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setMinimumSize(QSize(0, 0));
    }
}

void DispersedSelect::setLineEditValidator(const QString &regstr)
{
    if (m_pLineEdit != nullptr) {
        QRegExpValidator *pVariableValidator = new QRegExpValidator(QRegExp(regstr), this);
        m_pLineEdit->setValidator(pVariableValidator);
    }
}

void DispersedSelect::onBoxIndexChanged(const QString &textstr)
{
    setLineEditValue(textstr);
}

void DispersedSelect::setLineEditValue(const QString &textstr)
{
    if (m_pLineEdit == nullptr) {
        return;
    }
    if (METHOD_IMPLICIT_TRAP_INTEGRAL == textstr) {
        m_pLineEdit->setText("1");
        m_pLineEdit->setEnabled(false);
        m_pLineEdit->setPlaceholderText("");
    } else if (METHOD_BACK_EULER == textstr) {
        m_pLineEdit->setText("0");
        m_pLineEdit->setEnabled(false);
        m_pLineEdit->setPlaceholderText("");
    } else if (METHOD_WEIGHT_INTEGRAL == textstr) {
        setLineEditValidator(NPS::REG_DOUBLE);
        m_pLineEdit->setText("");
        m_pLineEdit->setEnabled(true);
        m_pLineEdit->setPlaceholderText(tr(
                "Please enter the weight factor, range: greater than 0 and less than 1")); // 请输入权重因子，范围：大于0且小于1
    }
}
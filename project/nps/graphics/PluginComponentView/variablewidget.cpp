#include "variablewidget.h"
#include "KLineEdit.h"
#include <QRegExpValidator>
#include <QSpacerItem>

VariableWidget::VariableWidget(const QMap<QString, QList<QString>> &variablemap, QWidget *parent /*= nullptr*/)
    : QWidget(parent)
{
    initUI(variablemap);
}

VariableWidget::~VariableWidget() { }

void VariableWidget::initUI(const QMap<QString, QList<QString>> &variablemap)
{
    QMap<QString, QList<QString>>::const_iterator iter = variablemap.begin();
    QGridLayout *gridlayout = new QGridLayout(this);
    gridlayout->setMargin(0);
    int layoutnumber = 0;
    for (; iter != variablemap.end(); ++iter) {
        if (!(iter.key().isEmpty()) && iter.value().size() > 0) {
            QLabel *plabel = new QLabel(this);
            initLabel(plabel);
            plabel->setText(iter.key());
            gridlayout->addWidget(plabel, layoutnumber, 0, 1, 1);
            KLineEdit *pLineEdit = new KLineEdit(this);
            initLineEdit(pLineEdit);
            QString variablestr = mergeVariables(iter.value());
            pLineEdit->setText(variablestr);
            pLineEdit->setToolTip(variablestr);
            gridlayout->addWidget(pLineEdit, layoutnumber, 1, 1, 3);
            layoutnumber += 1;
        }
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridlayout->addItem(verticalSpacer, layoutnumber, 0, 1, 4);
    this->setLayout(gridlayout);
}

void VariableWidget::initLabel(QLabel *label)
{
    if (label != nullptr) {
        label->setMinimumSize(QSize(100, 0));
        label->setMaximumSize(QSize(100, 16777215));
    }
}

void VariableWidget::initLineEdit(QLineEdit *lineEdit)
{
    if (lineEdit != nullptr) {
        QRegExpValidator *pVariableValidator = new QRegExpValidator(QRegExp("([a-zA-Z][a-zA-Z0-9]*,)*"), this);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setMinimumSize(QSize(0, 0));
        lineEdit->setValidator(pVariableValidator);
        lineEdit->setEnabled(false);
    }
}

QString VariableWidget::mergeVariables(const QList<QString> &strlist)
{
    QString strtmp = "";
    QList<QString>::const_iterator iter = strlist.begin();
    for (; iter != strlist.end(); ++iter) {
        if (strtmp.isEmpty()) {
            strtmp = *iter;
        } else {
            strtmp += "," + *iter;
        }
    }
    return strtmp;
}

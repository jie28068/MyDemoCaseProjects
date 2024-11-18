#include "customlineedit.h"
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

CustomLineEdit::CustomLineEdit(QWidget *parent /*= 0*/, bool isSearchEdit) : KLineEdit(parent)
{
    // 给LineEdit内添加 clearAll按钮
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    if (isSearchEdit) {
        QPushButton *iconButton = new QPushButton(this);
        iconButton->setObjectName("iconButton");
        iconButton->setFlat(true);

        hLayout->addWidget(iconButton);
        setTextMargins(15, 0, 20, 0);
    } else {
        setTextMargins(0, 0, 20, 0);
    }
    hLayout->addStretch();

    clearAllbutton = new QPushButton(this);
    clearAllbutton->setObjectName("clearAllButton");
    clearAllbutton->setVisible(false);
    hLayout->addWidget(clearAllbutton);

    hLayout->setContentsMargins(2, 0, 2, 0);
    setLayout(hLayout);

    installEventFilter(this);

    connect(clearAllbutton, SIGNAL(clicked()), this, SLOT(onClearText()));
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
}

CustomLineEdit::~CustomLineEdit() { }

void CustomLineEdit::onClearText()
{
    this->clear();
}

void CustomLineEdit::onTextChanged(const QString &)
{
    if (this->text().length() == 0 || !this->isEnabled()) {
        clearAllbutton->setVisible(false);
    } else {
        clearAllbutton->setVisible(true);
    }
}

bool CustomLineEdit::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
            if (lineEdit) {
                return true; // 输入框截获事件，不传递给下一个过滤器或目标对象
            }
        }
    }
    return QWidget::eventFilter(obj, evt);
}

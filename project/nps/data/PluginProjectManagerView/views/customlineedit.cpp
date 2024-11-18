#include "customlineedit.h"
#include <QEvent>

CustomLineEdit::CustomLineEdit(QWidget *parent /*= 0*/) : KLineEdit(parent)
{
    // 给LineEdit内添加 clearAll按钮
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addStretch();

    clearAllbutton = new QPushButton(this);
    clearAllbutton->setObjectName("clearAllButton");
    clearAllbutton->setVisible(false);
    clearAllbutton->installEventFilter(this);
    hLayout->addWidget(clearAllbutton);

    hLayout->setContentsMargins(2, 0, 2, 0);
    setTextMargins(0, 0, 20, 0);
    setLayout(hLayout);

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
    if (this->text().length() == 0)
        clearAllbutton->setVisible(false);
    else
        clearAllbutton->setVisible(true);
}

bool CustomLineEdit::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == clearAllbutton) {
        if (evt->type() == QEvent::Enter) {
            setCursor(Qt::ArrowCursor);
        }

        if (evt->type() == QEvent::Leave) {
            setCursor(Qt::IBeamCursor);
        }
    }
    return QWidget::eventFilter(obj, evt);
}

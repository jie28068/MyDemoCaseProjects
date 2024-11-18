#include "search_line_edit.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

SearchLineEdit::SearchLineEdit(QWidget *parent /* = 0 */) : KLineEdit(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QPushButton *iconButton = new QPushButton(this);
    iconButton->setObjectName("iconButton");
    iconButton->setFlat(true);

    mainLayout->addWidget(iconButton);
    mainLayout->addStretch();
    mainLayout->setContentsMargins(2, 0, 2, 0);
    setTextMargins(15, 0, 20, 0);
    setLayout(mainLayout);

    // QObject::connect(clear_button_, SIGNAL(clicked()), this, SLOT(onClearText()));
    QObject::connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));

    setPlaceholderText(tr("search")); // 搜索
}

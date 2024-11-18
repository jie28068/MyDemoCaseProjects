#include "ElideLineText.h"

ElideLineText::ElideLineText(QWidget *parent) : QLineEdit(parent), m_text("")
{
    this->setStyleSheet("QLineEdit{border-width:0;border-style:outset;\
						background-color:transparent;\
						padding:0;}");
    this->setReadOnly(true);
}

ElideLineText::~ElideLineText() { }
// 使用setElideText设置文本才有缩略和提示
void ElideLineText::setElideText(const QString &text)
{
    m_text = text;
    elideText();
}

void ElideLineText::elideText()
{
    QFontMetrics fontWidth(this->font());
    QString elidnote = fontWidth.elidedText(m_text, Qt::ElideRight, this->width() - 6);
    this->setText(elidnote);
    this->setToolTip(m_text);
}

void ElideLineText::resizeEvent(QResizeEvent *e)
{
    elideText();
}

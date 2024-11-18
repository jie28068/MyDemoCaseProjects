#include "FontWidget.h"

#include <QGridLayout>
// #include <QListView>
#include "GraphicsModelingKernel/GraphicsKernelDefinition.h"
#include <QDebug>
#include <QGroupBox>
#include <QLineEdit>
#include <QStyle>

// 字体设置
FontWidget::FontWidget(const QFont &initialfont, QWidget *parent)
    : QFontDialog(initialfont, parent), m_oldFont(initialfont), m_currentFont(initialfont)
{
    initUI(initialfont);
}

FontWidget::~FontWidget() { }

void FontWidget::setSampleFont(const QFont &font)
{
    // qDebug() << "setSampleFont=====================begin";
#if QT_VERSION == QT_VERSION_CHECK(4, 8, 7)
    QGridLayout *grid = (QGridLayout *)this->layout();
    if (grid != nullptr) {
        QLayoutItem *itemsamplegroup = grid->itemAtPosition(4, 2);
        QGroupBox *samplegroup = dynamic_cast<QGroupBox *>(itemsamplegroup->widget());
        QLineEdit *sampleEdit = nullptr;
        if (itemsamplegroup != nullptr) {
            foreach (QObject *obj, samplegroup->children()) {
                sampleEdit = dynamic_cast<QLineEdit *>(obj);
                if (sampleEdit != nullptr) {
                    // “字体样式”初始化特殊处理（直接对字体设置窗口中的用于显示字体样式的QListView进行修改设置）
                    //  qDebug() <<"sample文本=====" << sampleEdit->text();
                    QString textdecorationstr = "none";
                    if (font.underline() && font.strikeOut()) {
                        textdecorationstr = "line-through underline";
                    } else if (font.underline() && !font.strikeOut()) {
                        textdecorationstr = "underline";
                    } else if (!font.underline() && font.strikeOut()) {
                        textdecorationstr = "line-through";
                    }
                    QString italicstr = ((QFont::StyleItalic == font.style()) ? "italic" : "normal");
                    QString weightstr = "normal";
                    switch (font.weight()) {
                    case QFont::Light:
                        weightstr = "lighter";
                        break;
                    case QFont::Normal:
                        weightstr = "normal";
                        break;
                    case QFont::Black:
                        weightstr = "bolder";
                        break;
                    case QFont::DemiBold:
                    case QFont::Bold:
                        weightstr = "bold";
                        break;
                    }
                    sampleEdit->setStyleSheet(QString("QFontDialog QGroupBox QLineEdit {\
													  font-size:%1pt;\
													  font-family:\"%2\";\
													  text-decoration:%3;\
													  font-style:%4;\
													  font-weight:%5;}")
                                                      .arg(QString::number(font.pointSize()))
                                                      .arg(font.family())
                                                      .arg(textdecorationstr)
                                                      .arg(italicstr)
                                                      .arg(weightstr));
                    /*qDebug() << QString("QFontDialog QGroupBox QLineEdit {\
                            font-size:%1pt;\
                            font-family:\"%2\";\
                            text-decoration:%3;\
                            font-style:%4;\
                            font-weight:%5;}")
                            .arg(QString::number(font.pointSize ()))
                            .arg(font.family())
                            .arg(textdecorationstr)
                            .arg(italicstr)
                            .arg(weightstr);*/
                }
            }
        }
    }

#endif
    // qDebug() << "setSampleFont=====================end";
}

QFont FontWidget::OldFont()
{
    return m_oldFont;
}

QFont FontWidget::CurrentFont()
{
    return m_currentFont;
}

void FontWidget::onFontChanged(const QFont &newfont)
{
    m_currentFont = newfont;
    setSampleFont(newfont);
}
void FontWidget::initUI(const QFont &initialfont)
{
    // 字体设置框不显示书写系统
    QGridLayout *grid = (QGridLayout *)this->layout();
    if (grid) {
        // （5，0）书写系统标题，（7，0）选择书写系统的下拉框
        QLayoutItem *itemText = grid->itemAtPosition(5, 0);
        QLayoutItem *itemWriteSysBox = grid->itemAtPosition(7, 0);
        // QLayoutItem* itemStyleList = grid->itemAtPosition(2, 2);
        if (itemText && itemWriteSysBox) {
            itemText->widget()->setVisible(false);
            itemWriteSysBox->widget()->setVisible(false);
        }
    }
    setSampleFont(initialfont);
    this->setOption(QFontDialog::NoButtons, true);
    connect(this, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onFontChanged(const QFont &)));
}
#include "textboxproperty.h"
#include "GraphicsModelingKernel/GraphicsKernelDefinition.h"
#include <QDebug>
#include <QLabel>
#include <QTimer>

TextboxProperty::TextboxProperty(QSharedPointer<SourceProxy> sourceProxy)
    : CommonWrapper(), m_sourceProxy(sourceProxy), m_pTextboxEditWidget(nullptr), m_pTextboxFontWidget(nullptr)
{
}

TextboxProperty::~TextboxProperty() { }

void TextboxProperty::init()
{
    CommonWrapper::init();
    m_pTextboxEditWidget = new TextboxEditWdiget(m_sourceProxy);
    m_pTextboxEditWidget->setEditAreaText(m_sourceProxy->getAnnotationMap()[Annotation::content].toString());

    m_pTextFont = m_sourceProxy->getAnnotationMap()[Annotation::textFont].value<QFont>();
    m_pTextNewFont = m_pTextFont;

    const QString qstrFont = m_pTextNewFont.toString();
    m_FontTmp.fromString(qstrFont);
    m_FontTmp.resolve(QFont::AllPropertiesResolved);
    m_pTextboxFontWidget = new QFontDialog(m_FontTmp);
    m_pTextboxFontWidget->setOption(QFontDialog::NoButtons, true);
    connect(m_pTextboxFontWidget, SIGNAL(currentFontChanged(const QFont &)), this,
            SLOT(onCurrentFontChanged(const QFont &)));

    addWidget(m_pTextboxEditWidget, TEXT_EDIT);
    addWidget(m_pTextboxFontWidget, TEXT_FONT);

    QTimer::singleShot(500, this, SLOT(onTimer()));
}

QString TextboxProperty::getTitle()
{
    return QString(TITLE_NAME);
}

void TextboxProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        bool bvchanged = false;
        bool isNull = false;
        QString oldText = m_sourceProxy->getAnnotationMap()[Annotation::content].value<QString>();
        QString newText = m_pTextboxEditWidget->getEditAreaText();
        if (oldText != newText) {
            m_sourceProxy->setAnnotationProperty(Annotation::content, newText);
            bvchanged = true;
        }
        if (newText == "" || oldText == "") {
            isNull = true;
        }
        // m_pTextFont = m_pTextboxFontWidget->currentFont();

        // pointSize：-1，即表示font无改动
        if (m_pTextFont != m_pTextNewFont) {
            QString name = m_pTextNewFont.family();
            m_sourceProxy->setAnnotationProperty(Annotation::textFont, m_pTextNewFont);
            bvchanged = true;
        }

        QString strTextNewColor = m_pTextboxEditWidget->currentTextColor();
        if (m_sourceProxy->getAnnotationMap()[Annotation::textColor].toString() != strTextNewColor) {
            m_sourceProxy->setAnnotationProperty(Annotation::textColor, strTextNewColor);
            bvchanged = true;
        }

        QString strFrameNewColor = m_pTextboxEditWidget->currentLineColor();
        if (m_sourceProxy->getAnnotationMap()[Annotation::segmentColor].toString() != strFrameNewColor) {
            m_sourceProxy->setAnnotationProperty(Annotation::segmentColor, strFrameNewColor);
            bvchanged = true;
        }
        bool isShowFrame = m_pTextboxEditWidget->showFramChecked();
        if (m_sourceProxy->getAnnotationMap()[Annotation::isShowframe].toBool() != isShowFrame || isNull) {
            if (isNull) {
                m_sourceProxy->setAnnotationProperty(Annotation::isShowframe, false);
            } else {
                m_sourceProxy->setAnnotationProperty(Annotation::isShowframe, isShowFrame);
            }
            bvchanged = true;
        }

        int curAlignment = m_pTextboxEditWidget->getCurAlignment();
        if (m_sourceProxy->getAnnotationMap()[Annotation::alignment].toInt() != curAlignment) {
            m_sourceProxy->setAnnotationProperty(Annotation::alignment, curAlignment);
            bvchanged = true;
        }

        qreal segmentWidth = m_pTextboxEditWidget->getWidth();
        if (m_sourceProxy->getAnnotationMap()[Annotation::segmentWidth].toReal() != segmentWidth) {
            m_sourceProxy->setAnnotationProperty(Annotation::segmentWidth, segmentWidth);
            bvchanged = true;
        }

        if (bvchanged) {
            // m_pTextboxBlock->modifyTime = QDateTime::currentDateTime();
            m_bPropertyIsChanged = true;
        }
    }
}

bool TextboxProperty::checkValue(QString &errorinfo /*= QString( ) */)
{
    errorinfo = "";
    return true;
}

void TextboxProperty::setFontListCurrentIndex(QWidget *widget)
{
    QListView *listView = qobject_cast<QListView *>(widget);
    if (listView) {
        int nRow = listView->model()->rowCount();
        for (int i = 0; i < nRow; i++) {
            QModelIndex index = listView->model()->index(i, 0);
            auto strIndex = index.data();
            if (strIndex == m_FontTmp.style() || strIndex == m_FontTmp.family() || strIndex == m_FontTmp.pointSize()) {
                listView->setCurrentIndex(index);
            }
        }
    }
}

void TextboxProperty::onCurrentFontChanged(const QFont &font)
{
    m_pTextNewFont = font;
}

void TextboxProperty::onTimer()
{
#if QT_VERSION == QT_VERSION_CHECK(4, 8, 7)
    QGridLayout *grid = (QGridLayout *)m_pTextboxFontWidget->layout();
    if (grid) {
        // （5，0）书写系统标题，（7，0）选择书写系统的下拉框
        QLayoutItem *itemText = grid->itemAtPosition(5, 0);
        QLayoutItem *itemWriteSysBox = grid->itemAtPosition(7, 0);
        QLayoutItem *itemFamilyList = grid->itemAtPosition(2, 0);
        QLayoutItem *itemStyleList = grid->itemAtPosition(2, 2);
        QLayoutItem *itemSizeList = grid->itemAtPosition(2, 4);
        QLayoutItem *itemEffects = grid->itemAtPosition(4, 0);

        if (itemText && itemWriteSysBox) {
            itemText->widget()->setVisible(false);
            itemWriteSysBox->widget()->setVisible(false);
        }
    }
#endif
}

TextboxEditWdiget::TextboxEditWdiget(QSharedPointer<SourceProxy> pSource, QWidget *parent /*= nullptr*/)
    : CommentEditWdiget(pSource, parent)
{
    initUI();
}

QString TextboxEditWdiget::getEditAreaText()
{
    return m_pTextEdit->document()->toPlainText();
}

void TextboxEditWdiget::setEditAreaText(QString text)
{
    m_pTextEdit->setPlainText(text);
}

QString TextboxEditWdiget::currentTextColor()
{
    return m_pTextColorEdit->getColor();
}

QString TextboxEditWdiget::currentLineColor()
{
    return m_pLineColorEdit->getColor();
}

int TextboxEditWdiget::getCurAlignment()
{
    return m_pAlignmentGroup->checkedId();
}

qreal TextboxEditWdiget::getWidth()
{
    return m_pLineWidthSpinBox->value();
}

void TextboxEditWdiget::initUI()
{
    initDataList();

    m_pGridLayout = new QGridLayout(this);
    m_pGridLayout->setMargin(0);
    QLabel *pNameLabel = new QLabel(this);
    pNameLabel->setMinimumSize(QSize(100, 0));
    pNameLabel->setMaximumSize(QSize(100, 16777215));
    pNameLabel->setText(tr("Text Content")); // 文本内容
    pNameLabel->setAlignment(Qt::AlignTop);
    m_pGridLayout->addWidget(pNameLabel, 0, 0, 1, 1);
    m_pTextEdit = new QTextEdit(this);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    // sizePolicy.setHeightForWidth(m_lineEdit->sizePolicy().hasHeightForWidth());
    // m_pTextEdit->setSizePolicy(sizePolicy);
    m_pGridLayout->addWidget(m_pTextEdit, 0, 1, 10, 4);

    QLabel *pTextColorLabel = new QLabel(this);
    pTextColorLabel->setMinimumSize(QSize(100, 0));
    pTextColorLabel->setMaximumSize(QSize(100, 16777215));
    pTextColorLabel->setText(tr("Text Color")); // 文本颜色
    m_pGridLayout->addWidget(pTextColorLabel, 11, 0, 1, 1);

    QString textColor = m_pSourceProxy->getAnnotationMap()[Annotation::textColor].toString();
    m_pTextColorEdit = new ColorWidget(QColor(textColor), this);
    m_pTextColorEdit->setMinimumSize(QSize(134, 26));
    m_pTextColorEdit->setMaximumSize(QSize(134, 16777215));
    m_pGridLayout->addWidget(m_pTextColorEdit, 11, 1, 1, 1);

    m_pAlignmentLayout = new QHBoxLayout();
    QLabel *pAlignmentLabel = new QLabel(this);
    pAlignmentLabel->setMinimumSize(QSize(125, 0));
    pAlignmentLabel->setMaximumSize(QSize(125, 16777215));
    pAlignmentLabel->setText(tr("Align Type"));                             // 对齐方式
    m_pAlignmentLayout->addWidget(pAlignmentLabel);
    QRadioButton *butLeft = new QRadioButton(tr("Left Aligned"), this);     // 居左
    QRadioButton *butCenter = new QRadioButton(tr("Middle Aligned"), this); // 居中
    QRadioButton *butRight = new QRadioButton(tr("Right Aligned"), this);   // 居右
    m_pAlignmentGroup = new QButtonGroup(this);
    m_pAlignmentGroup->addButton(butLeft, 1);
    m_pAlignmentGroup->addButton(butCenter, 2);
    m_pAlignmentGroup->addButton(butRight, 3);
    setDefaultCheckAlignMent(m_pSourceProxy->getAnnotationMap()[Annotation::alignment].toInt());
    m_pAlignmentLayout->addWidget(butLeft);
    m_pAlignmentLayout->addWidget(butCenter);
    m_pAlignmentLayout->addWidget(butRight);
    m_pGridLayout->addLayout(m_pAlignmentLayout, 12, 0, 1, 4);

    QSpacerItem *vspacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_pGridLayout->addItem(vspacer, 13, 0, 1, 4);

    m_pFrameCheckBox = new QCheckBox(this);
    m_pFrameCheckBox->setCheckable(true);
    m_pFrameCheckBox->setChecked(true);
    m_pFrameCheckBox->setMinimumSize(QSize(125, 0));
    m_pFrameCheckBox->setMaximumSize(QSize(125, 16777215));
    m_pFrameCheckBox->setText(tr("Show Borders")); // 显示边框
    m_pFrameCheckBox->geometry();
    m_pFrameCheckBox->setChecked(m_pSourceProxy->getAnnotationMap()[Annotation::isShowframe].toBool());
    m_pGridLayout->addWidget(m_pFrameCheckBox, 14, 0, 1, 1);

    QLabel *pLineColorLabel = new QLabel(this);
    pLineColorLabel->setMinimumSize(QSize(100, 0));
    pLineColorLabel->setMaximumSize(QSize(100, 16777215));
    pLineColorLabel->setText(tr("Border Color")); // 边框颜色
    m_pGridLayout->addWidget(pLineColorLabel, 15, 0, 1, 1);

    QString lineColor = m_pSourceProxy->getAnnotationMap()[Annotation::segmentColor].toString();
    m_pLineColorEdit = new ColorWidget(QColor(lineColor), this);
    m_pLineColorEdit->setMinimumSize(QSize(134, 26));
    m_pLineColorEdit->setMaximumSize(QSize(134, 16777215));
    m_pGridLayout->addWidget(m_pLineColorEdit, 15, 1, 1, 1);

    QLabel *pLineWidthLabel = new QLabel(this);
    pLineWidthLabel->setMinimumSize(QSize(100, 0));
    pLineWidthLabel->setMaximumSize(QSize(100, 16777215));
    pLineWidthLabel->setText(tr("Border Width")); // 边框宽度
    m_pGridLayout->addWidget(pLineWidthLabel, 16, 0, 1, 1);

    m_pLineWidthSpinBox = new QDoubleSpinBox(this);
    m_pLineWidthSpinBox->setMinimumSize(QSize(134, 0));
    m_pLineWidthSpinBox->setMaximumSize(QSize(134, 16777215));
    m_pLineWidthSpinBox->setRange(0.1, 5.0);
    m_pLineWidthSpinBox->setSingleStep(0.1);
    m_pLineWidthSpinBox->setDecimals(1);
    m_pLineWidthSpinBox->setValue(m_pSourceProxy->getAnnotationMap()[Annotation::segmentWidth].toReal());
    m_pGridLayout->addWidget(m_pLineWidthSpinBox, 16, 1, 1, 1);

    QSpacerItem *vspacer2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_pGridLayout->addItem(vspacer2, 17, 0, 1, 4);
}
void TextboxEditWdiget::setDefaultCheckAlignMent(int alignment)
{
    QAbstractButton *button = m_pAlignmentGroup->button(alignment);
    if (button) {
        button->setChecked(true);
    }
}

bool TextboxEditWdiget::showFramChecked()
{
    return m_pFrameCheckBox->isChecked();
}

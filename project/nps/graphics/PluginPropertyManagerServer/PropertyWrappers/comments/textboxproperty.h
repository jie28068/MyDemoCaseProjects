#ifndef TEXTBOXPROPERTY_H
#define TEXTBOXPROPERTY_H

#include "CommonWrapper.h"
#include "commenteditwdiget.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontDialog>
#include <QGridLayout>
#include <QRadioButton>
#include <QTextEdit>

#define TITLE_NAME QObject::tr("Text Box Properties") // 文本框属性
#define TEXT_EDIT QObject::tr("Text Editing")         // 文本编辑
#define TEXT_FONT QObject::tr("Text Settings")        // 文本设定

class TextboxEditWdiget;

class TextboxProperty : public CommonWrapper
{
    Q_OBJECT
public:
    TextboxProperty(QSharedPointer<SourceProxy> sourceProxy);
    ~TextboxProperty();
    void init();

    // CommonWrapper
    virtual QString getTitle() override;
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;
    virtual bool checkValue(QString &errorinfo = QString()) override;

private:
    void setFontListCurrentIndex(QWidget *widget);

private slots:
    void onCurrentFontChanged(const QFont &font);
    void onTimer();

private:
    TextboxEditWdiget *m_pTextboxEditWidget;
    QFontDialog *m_pTextboxFontWidget;

    QFont m_pTextFont;
    QFont m_pTextNewFont;
    QFont m_FontTmp; // 需要设置的字体
    QSharedPointer<SourceProxy> m_sourceProxy;
};

class TextboxEditWdiget : public CommentEditWdiget
{
    Q_OBJECT
public:
    TextboxEditWdiget(QSharedPointer<SourceProxy> pSource, QWidget *parent = nullptr);

    QString getEditAreaText();

    void setEditAreaText(QString);

    QString currentTextColor();

    QString currentLineColor();

    bool showFramChecked();

    int getCurAlignment();

    qreal getWidth();

private:
    void initUI();

    void setDefaultCheckAlignMent(int alignment);

private:
    QHBoxLayout *m_pAlignmentLayout;
    QTextEdit *m_pTextEdit;
    ColorWidget *m_pTextColorEdit;
    QComboBox *m_pTextSizeComboBox;
    QCheckBox *m_pFrameCheckBox;
    QDoubleSpinBox *m_pLineWidthSpinBox;
    ColorWidget *m_pLineColorEdit;
    QButtonGroup *m_pAlignmentGroup;
};

#endif // TEXTBOXPROPERTY_H

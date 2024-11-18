#ifndef ELIDELINETEXT_H
#define ELIDELINETEXT_H

#include <QLineEdit>

// 根据拖拽宽度自动省略末尾字符，省略后放在文本上有提示，默认只读，不可编辑
class ElideLineText : public QLineEdit
{
    Q_OBJECT

public:
    ElideLineText(QWidget *parent);
    ~ElideLineText();
    void setElideText(const QString &text);
    void elideText();

protected:
    virtual void resizeEvent(QResizeEvent *e);

private:
    QString m_text;
};

#endif // ELIDELINETEXT_H

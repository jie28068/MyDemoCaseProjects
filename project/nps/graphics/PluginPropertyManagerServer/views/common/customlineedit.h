#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include "KLineEdit.h"
#include <QHBoxLayout>
#include <QPushButton>
class CustomLineEdit : public KLineEdit
{
    Q_OBJECT

public:
    CustomLineEdit(QWidget *parent = 0, bool isSearchEdit = false);
    ~CustomLineEdit();

private:
    bool eventFilter(QObject *obj, QEvent *evt) override;

private slots:
    void onClearText();
    void onTextChanged(const QString &);

private:
    QPushButton *clearAllbutton;
};

#endif // CUSTOMLINEEDIT_H

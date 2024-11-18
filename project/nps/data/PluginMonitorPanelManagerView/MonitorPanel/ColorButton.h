#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QColorDialog>
#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget *parent);
    ~ColorButton();

    void setColor(QColor &clr);
    QColor getColor(void);

protected:
    void paintEvent(QPaintEvent *event);
protected slots:
    void onColorDialog(void);

private:
    QColor m_color;
};

#endif // COLORBUTTON_H

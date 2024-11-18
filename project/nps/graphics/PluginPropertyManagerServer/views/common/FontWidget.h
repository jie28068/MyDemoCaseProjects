#ifndef FONTDIALOG_H
#define FONTDIALOG_H

#include <QFontDialog>

class FontWidget : public QFontDialog
{
    Q_OBJECT
public:
    explicit FontWidget(const QFont &initialfont, QWidget *parent = nullptr);
    ~FontWidget();
    void setSampleFont(const QFont &font);
    QFont OldFont();
    QFont CurrentFont();
private slots:
    void onFontChanged(const QFont &);

private:
    void initUI(const QFont &initialfont);

private:
    QFont m_oldFont;
    QFont m_currentFont;
};

#endif // FONTDIALOG_H

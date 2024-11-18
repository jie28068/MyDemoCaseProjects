#ifndef PLOTLAYOUTCHOOSEDIALOG_H
#define PLOTLAYOUTCHOOSEDIALOG_H

#include <QDialog>
#include <QFocusEvent>
#include <QPaintEvent>
#include <QPainter>

#define MAXROWS 5
#define MAXCOLS 3

#define PLOTLAYOUTCHOOSEDIALOG_WIDTH 88
#define PLOTLAYOUTCHOOSEDIALOG_HEIGHT 158

class PlotLayoutChooseDialog : public QDialog
{
    Q_OBJECT

public:
    PlotLayoutChooseDialog(QWidget *parent = nullptr);
    ~PlotLayoutChooseDialog();

    int rows(void);
    int cols(void);

signals:

    void choosed(int rows, int cols);

protected:
    void paintEvent(QPaintEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;

    void focusOutEvent(QFocusEvent *e) override;

private:
    int m_rows;
    int m_cols;
    int whiteArea_w;
    int whiteArea_h;
    int blueArea_w;
    int blueArea_h;
};

#endif // PLOTLAYOUTCHOOSEDIALOG_H

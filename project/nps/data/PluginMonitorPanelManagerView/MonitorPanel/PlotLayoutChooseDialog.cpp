#include "PlotLayoutChooseDialog.h"

PlotLayoutChooseDialog::PlotLayoutChooseDialog(QWidget *parent)
    : QDialog(parent), m_rows(0), m_cols(0), whiteArea_w(25), whiteArea_h(25), blueArea_w(21), blueArea_h(21)
{
    resize(PLOTLAYOUTCHOOSEDIALOG_WIDTH, PLOTLAYOUTCHOOSEDIALOG_HEIGHT);
    Qt::WindowFlags winFlags = windowFlags();
    winFlags |= Qt::FramelessWindowHint;
    winFlags |= Qt::SubWindow;
    winFlags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(winFlags);
    setMouseTracking(true);
    // setFocusPolicy(Qt::NoFocus);
    // setModal(false);
    setFocus();
    this->setStyleSheet("PlotLayoutChooseDialog{border:1px solid #a3a3a3;background-color:white;}");
}

PlotLayoutChooseDialog::~PlotLayoutChooseDialog() { }

int PlotLayoutChooseDialog::rows(void)
{
    if (m_rows == 0)
        m_rows = 1;
    return m_rows;
}

int PlotLayoutChooseDialog::cols(void)
{
    if (m_cols == 0)
        m_cols = 1;
    return m_cols;
}

void PlotLayoutChooseDialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);
    QPainter painter(this);
    painter.save();

    painter.setPen(QPen(QColor("#35405c")));
    QFont tempFont("微软雅黑");
    tempFont.setPixelSize(14);
    painter.setFont(tempFont);
    if (m_rows > 0 && m_cols > 0) {
        QString strTemp = QString("%1x%2").arg(m_rows).arg(m_cols);
        painter.drawText(QRect(0, 0, PLOTLAYOUTCHOOSEDIALOG_WIDTH, 20), Qt::AlignCenter, strTemp);
    }

    painter.setPen(QPen(QColor("#cecece"), 1, Qt::SolidLine));
    painter.setBrush(Qt::white);
    int space = 0;
    for (int i = 0; i < MAXROWS; i++) {
        for (int j = 0; j < MAXCOLS; j++) {
            if (i != 0 || j != 0) {
                space = 2;
            } else {
                space = 0;
            }
            QRect rc(4 + whiteArea_w * j + j * space, 20 + whiteArea_h * i + i * space, whiteArea_w, whiteArea_h);
            painter.drawRect(rc);
        }
    }

    painter.setPen(QPen(QColor("#B7CADE"), 1, Qt::SolidLine));
    painter.setBrush(QBrush(QColor("#B7CADE")));
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            if (i != 0 || j != 0) {
                space = 6;
            } else {
                space = 0;
            }
            QRect rc(6 + blueArea_w * j + j * space, 22 + blueArea_h * i + i * space, blueArea_w, blueArea_h);
            painter.drawRect(rc);
        }
    }

    painter.restore();
}

void PlotLayoutChooseDialog::mousePressEvent(QMouseEvent *e)
{
    QDialog::mousePressEvent(e);

    if (m_rows > 0 && m_cols > 0) {
        emit choosed(m_rows, m_cols);
    }

    accept();
}

void PlotLayoutChooseDialog::mouseMoveEvent(QMouseEvent *e)
{
    QDialog::mouseMoveEvent(e);

    QRect rect(4, 16, PLOTLAYOUTCHOOSEDIALOG_WIDTH - 8, PLOTLAYOUTCHOOSEDIALOG_HEIGHT - 20);
    int rows = 0;
    int cols = 0;
    if (rect.contains(e->pos())) {
        int w = (PLOTLAYOUTCHOOSEDIALOG_WIDTH - 8) / MAXCOLS;
        int h = (PLOTLAYOUTCHOOSEDIALOG_HEIGHT - 20) / MAXROWS;
        cols = (e->pos().x() - 4) / w;
        if ((e->pos().x() - 4) % w != 0)
            cols++;
        if (cols > MAXCOLS)
            cols = MAXCOLS;
        rows = (e->pos().y() - 16) / h;
        if ((e->pos().y() - 16) % h != 0)
            rows++;
        if (rows > MAXROWS)
            rows = MAXROWS;
    }

    if (rows == m_rows && cols == m_cols)
        return;
    m_rows = rows;
    m_cols = cols;
    update();
}

void PlotLayoutChooseDialog::leaveEvent(QEvent *e)
{
    QDialog::leaveEvent(e);
    m_rows = 0;
    m_cols = 0;
    update();
}

void PlotLayoutChooseDialog::focusOutEvent(QFocusEvent *e)
{
    reject();
}

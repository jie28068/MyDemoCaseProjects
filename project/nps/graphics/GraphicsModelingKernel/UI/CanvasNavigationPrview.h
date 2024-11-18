#ifndef CANVASNAVIGATIONPRVIEW_H
#define CANVASNAVIGATIONPRVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPoint>
#include <QVBoxLayout>
#include <QWidget>

class CanvasNavigationPrview;

class CanvasNavigationWidget : public QWidget
{
    Q_OBJECT
public:
    CanvasNavigationWidget(CanvasNavigationPrview *view, QWidget *parent = 0);
    ~CanvasNavigationWidget();

private:
    CanvasNavigationPrview *m_pPreview;
    QVBoxLayout *m_pLayout;
};

class CanvasNavigationPrview : public QGraphicsView
{
    Q_OBJECT

public:
    CanvasNavigationPrview(QWidget *parent = 0);
    ~CanvasNavigationPrview();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void resizeEvent(QResizeEvent *event) override;

    virtual void paintEvent(QPaintEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;

signals:
    void previewSceneRectChanged(QPointF rect);

    void previewSceneRectChangeFinished(QPointF pos);

    void escapeKeyPressed(QEvent *event);

public slots:
    void onSceneViewChanged(QRectF rect);

    void onPreviewSceneRectChanged(const QRectF &rect);

private:
    QRectF viewRect;
    QRectF senceRect;
    bool isMousePressed;
};

#endif // CANVASNAVIGATIONPRVIEW_H

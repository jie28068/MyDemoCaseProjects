#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListView>
#include <QMouseEvent>

class MyListView : public QListView
{
    Q_OBJECT
public:
    MyListView(const QString &strGroupName, QWidget *parent = nullptr);
    ~MyListView();

    bool isEmpty();

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *e) override;
    virtual void focusOutEvent(QFocusEvent *e);

signals:
    void leftMouseButtonDoubleClicked(const QString &strTypeName);

private:
    void calculateHeight();
    void drawText(QDrag *drag, const QModelIndex &index);

private:
    QString m_strGroupName;
};

#endif // MYLISTVIEW_H

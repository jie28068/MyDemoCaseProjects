#ifndef CUSTOMBLOCKTABLEVIEW_H
#define CUSTOMBLOCKTABLEVIEW_H

#include <QDebug>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QListView>
#include <QMimeData>
#include <QObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>

class CustomBlockTableView : public QTableView
{
    Q_OBJECT

public:
    explicit CustomBlockTableView(QWidget *parent = nullptr);

    bool isDraging() const { return IsDraging; }
    int offset() const { return 12; }
    int highlightedRow() const { return curHighlightedRow; }
    int dragRow() const { return curDragRow; }
    static QString myMimeType() { return QString(""); }

private:
    // dropIndicator显示与消失
    void updateDropIndicator(int row);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

private:
    bool IsDraging;
    // 当前鼠标所在位置 行号
    int curHighlightedRow;
    // 被拖拽的行所在的位置
    int oldHighlightedRow;
    // 拖拽的行
    int curDragRow;
    // 当前鼠标放下所在行
    int curInsertRow;
    // 需要删除的旧行（行号）
    QList<int> oldSelectRow;
    // 选中的拖拽行
    QList<int> curDragRows;
};

bool compareBarData(const int &barAmount1, const int &barAmount2);

#endif // CUSTOMBLOCKTABLEVIEW_H

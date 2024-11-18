#pragma once
#include "WizardPageParamNew.h"
#include <QHeaderView>
#include <QLabel>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QTableView>
class MyTableView : public QTableView
{
    Q_OBJECT
public:
    MyTableView(QLabel *label, QSortFilterProxyModel *sortModel, QWidget *parent = 0);
    ~MyTableView() { delete frozenTableView; }

protected:
    void init();
    /// @brief 在QTableView的大小调整时进行自定义的处理操作,更新表格的内容,调整表格的列宽和行高
    /// @param event
    void resizeEvent(QResizeEvent *event) override;
    /// @brief 可以自定义滚动的行为
    /// @param index 表示需要滚动到可见区域的模型索引。
    /// @param hint 表示滚动的提示，有多个选项可用，其中EnsureVisible是默认选项，表示确保索引可见。
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

    virtual void mousePressEvent(QMouseEvent *event) override;

public:
    void updateFrozenTableGeometry(int row = -1);

private:
    QLabel *messageLabel;
    QTableView *frozenTableView;

private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    /// @brief 拖拽表头时，实时变换长度
    /// @param logicalIndex
    /// @param oldSize
    /// @param newSize
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
};

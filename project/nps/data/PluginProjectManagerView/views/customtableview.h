#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include "CustomMineData.h"
#include "KLWidgets/KItemView.h"
#include "ProjActionManager.h"
#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>

class CustomHorizontalScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    CustomHorizontalScrollBar(QWidget *parent = nullptr);
    ~CustomHorizontalScrollBar();
    void initCustomScrollbar(const int &freezecols, int min, int max);
    // virtual QSize sizeHint() const;
    int getFreezeColno() { return m_freezeCols; }
public slots:
    void onTableRangeChanged(int min, int max);

private slots:
    void onValueChanged(int value);

signals:
    void scrollbarValueChanged(bool bscrollright, int begincol, int changecolno);

private:
    int m_freezeCols;
    int m_oldposition;
};

class CustomTableView : public KTableView
{
    Q_OBJECT

public:
    CustomTableView(QWidget *parent = nullptr);
    ~CustomTableView();
    const QModelIndexList getCurrentSelectedIndexs() { return this->selectedIndexes(); }
    /// @brief 获取表格menu可编辑列表
    /// @param tableindex 没有则默认当前index
    /// @return
    const QList<ProjActionManager::ActionType> getTableMenuEnableList(const QModelIndex &tableindex = QModelIndex());
signals:
    void leftMouseDoubleClickedComponent(const QModelIndex &index);
    void selectedRowsChanged(const int &selectnos);
    void tableMenuEnd();

public slots:
    void onClickedMouseRightButton(const QPoint &point);
    void onVHeaderDoubleClicked(int logicalIndex);
    void onVHeaderSectionClicked(int logicalIndex);
    void onHeaderMenuRequested(const QPoint &pos);
    void onCornerBtnClicked();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void focusInEvent(QFocusEvent *e);

private:
    void createCustomMenu(const QModelIndex &index);
    int getSelectRowNos();

private:
    bool m_bMousePressed;
    int m_selectStartRow;
};

#endif // CUSTOMTABLEVIEW_H

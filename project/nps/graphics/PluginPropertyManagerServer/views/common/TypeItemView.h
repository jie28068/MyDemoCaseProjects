#ifndef TYPEITEMVIEW_H
#define TYPEITEMVIEW_H

#include "KLWidgets/KItemView.h"
#include <QHeaderView>
#include <QScrollBar>
#include <QWidget>
class TreeView : public KTreeView
{
    Q_OBJECT

public:
    TreeView(QWidget *parent = nullptr);
    ~TreeView();

private:
};

class TableView : public KTableView
{
    Q_OBJECT

public:
    TableView(QWidget *parent = nullptr);
    ~TableView();
    QModelIndexList getSelectedIndexes();

private:
    int getSelectedRows();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void focusInEvent(QFocusEvent *e);

signals:
    void selectedRowsChanged(const int &selectnos);

    void selectedIndexsChanged(QModelIndexList &modelIndexList);

public slots:
    void onVHeaderSectionClicked(int logicalIndex);

private:
    bool m_bMousePressed;
    QModelIndexList m_modelIndexList;
};

class CWidget : public QWidget
{
    Q_OBJECT

public:
    CWidget(QWidget *parent = nullptr);
    ~CWidget();
    virtual bool saveData();
    virtual void setCWidgetReadOnly(bool bReadOnly);
    virtual bool checkLegitimacy(QString &errorinfo);
};

#endif // TYPEITEMVIEW_H

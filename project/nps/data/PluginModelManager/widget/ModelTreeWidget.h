#ifndef MODELTREEVIEW_H
#define MODELTREEVIEW_H

#include "ModelTreeView.h"
#include "TreeModel.h"
#include "searchLineEdit.h"

class ModelTreeWidget : public QWidget
{
    Q_OBJECT
public:
    ModelTreeWidget(QWidget *parent = nullptr, bool isShowLeaf = false);
    ~ModelTreeWidget();

    void init();

private slots:
    void onFilter(const QString &strFilter);

public:
    ModelTreeView *m_modelTreeView; // 树视图

    bool m_isShowLeaf;
};
#endif
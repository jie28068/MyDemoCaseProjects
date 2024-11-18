#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "CustomTreeModel.h"
#include "KLWidgets/KItemView.h"

class TreeView : public KTreeView
{
    Q_OBJECT

public:
    TreeView(QWidget *parent);
    ~TreeView();
};

#endif // TREEVIEW_H

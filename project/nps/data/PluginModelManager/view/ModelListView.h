#ifndef MODELLISTVIEW_H
#define MODELLISTVIEW_H

#include "KLWidgets/KItemView.h"
#include "ListModel.h"

class ModelListView : public KListView
{
    Q_OBJECT
public:
    ModelListView(QWidget *parent);
    ~ModelListView();

    void refreshListModel(TreeItem *parentItem);

private slots:
    void onListViewContextMenu(const QPoint &pos); // 右键菜单

    void onAddProjectModel();
    void onAddToolkitModel();

    void onEditModel();

    void onDeleteModel();

private:
    ListModel *m_listModel;
};

#endif
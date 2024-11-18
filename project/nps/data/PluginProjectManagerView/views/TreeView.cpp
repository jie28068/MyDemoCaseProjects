#include "TreeView.h"
#include "ProjActionManager.h"
#include <QMenu>
#include <QMouseEvent>

TreeView::TreeView(QWidget *parent) : KTreeView(parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

TreeView::~TreeView() { }

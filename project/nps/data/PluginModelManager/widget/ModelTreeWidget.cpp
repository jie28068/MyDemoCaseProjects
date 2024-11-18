#include "ModelTreeWidget.h"
#include <QApplication>
#include <QVBoxLayout>

ModelTreeWidget::ModelTreeWidget(QWidget *parent, bool isShowLeaf) : QWidget(parent), m_isShowLeaf(isShowLeaf)
{
    init();
}

ModelTreeWidget::~ModelTreeWidget() { }

void ModelTreeWidget::init()
{
    // 创建树形管理视图
    m_modelTreeView = new ModelTreeView(this, m_isShowLeaf);
    m_modelTreeView->initTreeView();
    SearchLineEdit *serarchLineEdit = new SearchLineEdit(this);
    connect(serarchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilter(QString)));

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(serarchLineEdit, 0, 0);
    pLayout->addWidget(m_modelTreeView, 1, 0);
}

void ModelTreeWidget::onFilter(const QString &strFilter)
{
    m_modelTreeView->treeFilter(strFilter);
}

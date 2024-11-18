#include "ModelListView.h"
#include "CoreLib/ServerManager.h"
#include "DSNFuctionMng.h"
#include "KMessageBox.h"
#include "ListDelegate.h"
#include "ModelManagerConst.h"
#include "ModelManagerViewServerMng.h"
#include "ModelTreeView.h"
#include "ModelViewDefine.h"
#include "TreeModelMng.h"
#include <QMenu>

USE_LOGOUT_("ModelListView")

ModelListView::ModelListView(QWidget *parent) : KListView(parent), m_listModel(nullptr)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setViewMode(QListView::IconMode);
    this->setIconSize(ModelViewDefine::ListItemSize);
    // this->setGridSize(ModelViewDefine::ListItemSize);
    this->setSpacing(10);
    this->setMovement(QListView::Snap);
    this->setResizeMode((QListView::Adjust));

#ifdef COMPILER_DEVELOPER_MODE
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
#endif

    m_listModel = new ListModel(this);
    this->setModel(m_listModel);

    this->setItemDelegate(new ListDelegate(this));

    connect(this, &ModelListView::customContextMenuRequested, this, &ModelListView::onListViewContextMenu);
}

ModelListView::~ModelListView() { }

void ModelListView::refreshListModel(TreeItem *parentItem)
{
    m_listModel->refreshModelData(parentItem);
}

void ModelListView::onEditModel()
{
    auto indexList = this->selectedIndexes();
    if (indexList.isEmpty() || !m_listModel || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }
    auto index = indexList.at(0);
    auto curItem = m_listModel->getItemByRow(index.row());
    if (!curItem) {
        return;
    }

    QString modelName = curItem->m_name;
    if (curItem->m_model) {
        modelName = curItem->m_model->getPrototypeName();
    }

    if (curItem->m_toolkitType == KL_TOOLKIT::kProjectType) { // 项目内
        ModelTreeView::editProjectItem(curItem, modelName);
    } else {
        ModelTreeView::editToolkitItem(curItem, modelName);
    }
}

void ModelListView::onDeleteModel()
{
    auto indexList = this->selectedIndexes();
    if (indexList.isEmpty() || !m_listModel || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }
    auto index = indexList.at(0);
    auto curItem = m_listModel->getItemByRow(index.row());
    if (!curItem) {
        return;
    }

    if (KMessageBox::Cancel
        == KMessageBox::warning(ModelViewDefine::IS_DEL_ITEM, KMessageBox::Ok | KMessageBox::Cancel, KMessageBox::Ok)) {
        return;
    }

    if (!ModelTreeView::deleteItem(curItem)) {
        return;
    }

    // 删除树节点,非项目相关的
    if (curItem->parent() && curItem->m_toolkitType != KL_TOOLKIT::kProjectType) {
        curItem->parent()->removeChild(curItem);
        m_listModel->refreshModelData(curItem->parent());
    }
}

void ModelListView::onListViewContextMenu(const QPoint &pos)
{
    if (!m_listModel || !m_listModel->getParentItem()) {
        return;
    }

    QModelIndex index = indexAt(pos);
    QMenu menu;

    if (!index.isValid()) { // 点击空白处
        auto parentItem = m_listModel->getParentItem();
        if (!parentItem) {
            return;
        }
        if (parentItem->m_type != TreeItem::BranchNode) { // 父节点不为组
            return;
        }
        if (parentItem->m_toolkitType == KL_TOOLKIT::kProjectType) { // 项目内
            menu.addAction(tr("Add"), this, SLOT(onAddProjectModel()));
        } else { // 系统、用户工具箱
            bool canOperate = true;
            if (parentItem->m_toolkitType == KL_TOOLKIT::kSystemType) { // 开发模块下,才能操作系统工具箱
#ifdef COMPILER_DEVELOPER_MODE
                canOperate = true;
#else
                canOperate = false;
#endif
            }

            if (canOperate) {
                menu.addAction(tr("Add"), this, SLOT(onAddToolkitModel()));
            }
        }
    } else { // 点击列表元素
        auto item = m_listModel->getItemByRow(index.row());
        if (!item) {
            return;
        }
        if (item->m_type == TreeItem::LeafNode) {
            if (item->m_toolkitType == KL_TOOLKIT::kProjectType) { // 项目内
                menu.addAction(tr("Edit"), this, SLOT(onEditModel()));
                menu.addAction(tr("Delete"), this, SLOT(onDeleteModel()));
            } else { // 系统工具箱
                bool canOperate = true;
                if (item->m_toolkitType == KL_TOOLKIT::kSystemType) { // 开发模块下,才能操作系统工具箱
#ifdef COMPILER_DEVELOPER_MODE
                    canOperate = true;
#else
                    canOperate = false;
#endif
                }
                if (canOperate) {
                    menu.addAction(tr("Edit"), this, SLOT(onEditModel()));
                    if (item->m_modelType == ControlBlock::Type || item->m_modelType == ElectricalBlock::Type) {
                        menu.addAction(tr("Delete"), this, SLOT(onDeleteModel()));
                    }
                }
            }
        }
    }

    menu.exec(cursor().pos());
}

void ModelListView::onAddProjectModel()
{
    auto parentItem = m_listModel->getParentItem();
    if (!parentItem || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    PModel newModel = ModelTreeView::addProjectItem(parentItem);
}

void ModelListView::onAddToolkitModel()
{
    auto parentItem = m_listModel->getParentItem();
    if (!parentItem || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    PModel newModel = ModelTreeView::addToolkitItem(parentItem);

    // 非项目相关的
    if (newModel) {
        // 刷新列表元素
        TreeItem *addItem = new TreeItem(newModel->getPrototypeName(), TreeItem::LeafNode, parentItem);
        TreeModelMng::getInstance().fillTreeItem(addItem);
        parentItem->addChild(addItem);

        m_listModel->refreshModelData(parentItem);
    }
}

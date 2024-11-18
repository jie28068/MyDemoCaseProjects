#include "ModelTreeView.h"
#include "CoreLib/ServerManager.h"
#include "DSNFuctionMng.h"
#include "GlobalAssistant.h"
#include "KMessageBox.h"
#include "ModelManagerConst.h"
#include "ModelManagerViewServerMng.h"
#include "TranslationTool.h"
#include "TreeModelMng.h"
#include <ModelViewDefine.h>
#include <QFile>
#include <QHeaderView>

USE_LOGOUT_("ModelTreeView")

ModelTreeView::ModelTreeView(QWidget *parent, bool isShowLeaf)
    : KTreeView(parent),
      m_isShowLeaf(isShowLeaf),
      m_treeModel(nullptr),
      m_proxyModel(nullptr),
      m_addToolkitDlg(nullptr),
      m_leAddToolkit(nullptr)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setSelectionBehavior(QTreeView::SelectRows);  // 一次选中整行
    this->setSelectionMode(QTreeView::SingleSelection); // 单选，配合上面的整行就是一次选单行
    this->setFocusPolicy(Qt::NoFocus);                  // 去掉鼠标移到单元格上时的虚线框
    this->header()->setStretchLastSection(true);        // 最后一列自适应宽度
    this->setHeaderHidden(true);                        // 设置表头隐藏

    this->setDragEnabled(true);
    this->setDropIndicatorShown(true);

    connect(this, &ModelTreeView::customContextMenuRequested, this, &ModelTreeView::onTreeContextMenu);

    m_menu = new QMenu(this);

    // 添加工具箱弹窗
    m_leAddToolkit = new QLineEdit(m_addToolkitDlg);
    m_leAddToolkit->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME)));

    m_addToolkitDlg = new KCustomDialog(tr("Tool Kit Name"), m_leAddToolkit);
    connect(m_addToolkitDlg, &KCustomDialog::closed, this, &ModelTreeView::onCloseAddToolKit);
}

ModelTreeView::~ModelTreeView() { }

void ModelTreeView::initTreeView()
{
    m_treeModel = TreeModelMng::getInstance().getTreeModel();
    m_proxyModel = new TreeViewProxyModel(m_isShowLeaf, this);
    m_proxyModel->setSourceModel(m_treeModel);
    this->setModel(m_proxyModel);
    m_proxyModel->setFilterString(QString());

    if (!ModelManagerViewServerMng::getInstance().isInited()) {
        return;
    }

    auto &projectServerIF = ModelManagerViewServerMng::getInstance().m_pProjectMngServerIF;

    // 绑定 项目管理服务通知
    projectServerIF->connectNotify(IPM_Notify_CodePrototypeSaved, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_CodePrototypeDestroyed, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_DrawingBoardDestroyed, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_DrawingBoardSaved, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_DrawingBoardFileRenamed, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_CurrentProjectChanged, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_CurrentProjectDeactivate, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
    projectServerIF->connectNotify(IPM_Notify_CopyModelFinished, this,
                                   SLOT(onRecevieProjectManagerServerMsg(unsigned int, const NotifyStruct &)));
}

TreeItem *ModelTreeView::getItemByIndex(const QModelIndex &index)
{
    auto mapIndex = m_proxyModel->mapToSource(index);
    return m_treeModel->itemFromIndex(mapIndex);
}

PModel ModelTreeView::addProjectItem(TreeItem *parentItem)
{
    PModel newModel;
    if (parentItem->m_toolkitType == KL_TOOLKIT::kProjectType) { // 项目内
        if (ControlBlock::Type == parentItem->m_modelType) {     // 本地代码型
            if (!ModelManagerViewServerMng::getInstance().m_pProjectMngServer->GetCurProject()) {
                LOGOUT(QObject::tr("There are currently no active projects, unable to create a new module！"),
                       LOG_ERROR);
                return newModel;
            }
            QString projectPath =
                    ModelManagerViewServerMng::getInstance().m_pProjectMngServer->GetCurrentProjectAbsoluteDir()
                    + KL_TOOLKIT::DLL_DIR_NAME;
            newModel = ModelManagerViewServerMng::getInstance().m_pWizardServer->NewWizardDialog(projectPath);
            if (newModel) {
                ModelManagerViewServerMng::getInstance().m_pProjectMngServer->SaveBoardModel(newModel);
            }
        }
    }

    return newModel;
}

PModel ModelTreeView::addToolkitItem(TreeItem *parentItem)
{
    PModel newModel;
    if (parentItem->m_toolkitName == KL_TOOLKIT::ELECTRICAL_TOOLKIT) { // 电气工具箱
        // TODO 根据用户选择 创建不同类型的model （电气model、、电气构造
        newModel = ModelManagerViewServerMng::getInstance().m_pWizardServer->NewElecWizardDialog();
        if (newModel) {
            ModelManagerViewServerMng::getInstance().m_pModelManagerServer->AddToolkitModel(parentItem->m_toolkitName,
                                                                                            newModel);
        }
    } else { // 控制工具箱
        // TODO 根据用户选择 创建不同类型的model （控制代码、控制构造
        auto dllPath = DSNFuctionMng::getInstance().getDllPathByToolkitName(parentItem->m_toolkitName);
        newModel = ModelManagerViewServerMng::getInstance().m_pWizardServer->NewWizardDialog(dllPath);
        if (newModel) {
            ModelManagerViewServerMng::getInstance().m_pModelManagerServer->AddToolkitModel(parentItem->m_toolkitName,
                                                                                            newModel);
        }
    }

    return newModel;
}

void ModelTreeView::editProjectItem(TreeItem *item, const QString &modelName)
{
    switch (item->m_modelType) {
    case ControlBlock::Type: {
        auto model = ModelManagerViewServerMng::getInstance().m_pProjectMngServer->GetControlModel(modelName);
        if (!model) {
            return;
        }
        QString projectPath =
                ModelManagerViewServerMng::getInstance().m_pProjectMngServer->GetCurrentProjectAbsoluteDir()
                + KL_TOOLKIT::DLL_DIR_NAME;
        bool isChanged =
                ModelManagerViewServerMng::getInstance().m_pWizardServer->ChangeWizardDialog(model, projectPath);
        if (isChanged) {
            ModelManagerViewServerMng::getInstance().m_pProjectMngServer->SaveControlModel(model);
        }
    } break;
    case CombineBoardModel::Type:
    case ElecCombineBoardModel::Type: {
        ModelManagerViewServerMng::getInstance().m_pDrawingBoardServer->modifyUserDefineBlock(modelName);
    } break;
    default:
        break;
    }
}

void ModelTreeView::editToolkitItem(TreeItem *item, const QString &modelName)
{
    if (item->m_toolkitName == KL_TOOLKIT::ELECTRICAL_TOOLKIT) {
        if (item->m_modelType == ElectricalBlock::Type) {
            auto model = ModelManagerViewServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(modelName);
            if (model) {
                bool isChanged =
                        ModelManagerViewServerMng::getInstance().m_pWizardServer->ChangeElecWizardDialog(model);
                if (isChanged) {
                    ModelManagerViewServerMng::getInstance().m_pModelManagerServer->SaveToolkitModel(
                            item->m_toolkitName, model);
                }
            }
        } else if (item->m_modelType == ElecCombineBoardModel::Type) { // 电气构造
            ModelManagerViewServerMng::getInstance().m_pDrawingBoardServer->modifyUserDefineBlock(modelName);
        }
    } else { // 控制工具箱
        if (item->m_modelType == ControlBlock::Type) {
            auto dllPath = DSNFuctionMng::getInstance().getDllPathByToolkitName(item->m_toolkitName);
            auto model = ModelManagerViewServerMng::getInstance().m_pModelManagerServer->GetToolkitModel(modelName);
            if (model) {
                bool isChanged =
                        ModelManagerViewServerMng::getInstance().m_pWizardServer->ChangeWizardDialog(model, dllPath);
                if (isChanged) {
                    ModelManagerViewServerMng::getInstance().m_pModelManagerServer->SaveToolkitModel(
                            item->m_toolkitName, model);
                }
            }
        } else if (item->m_modelType == CombineBoardModel::Type) { // 构造型
            ModelManagerViewServerMng::getInstance().m_pDrawingBoardServer->modifyUserDefineBlock(modelName);
        }
    }
}

bool ModelTreeView::deleteItem(TreeItem *item, QModelIndex parentIndex, TreeModel *treeModel)
{
    for (int i = 0; i < item->children().size(); i++) {
        if (item->m_type == TreeItem::HeadNode || item->m_type == TreeItem::BranchNode) {
            auto index = parentIndex.child(i, 0);
            auto childItem = item->child(i);
            deleteItem(childItem, index, treeModel);
        }
    }

    if (item->m_type == TreeItem::HeadNode || item->m_type == TreeItem::BranchNode) { // 删除组和工具箱
        // 删除树节点,非项目相关的
        if (item->parent() && item->m_toolkitType != KL_TOOLKIT::kProjectType) {
            treeModel->removeChildItem(item);
        }
        if (item->m_type == TreeItem::HeadNode) { // 从模型管理服务删除该工具箱
            ModelManagerViewServerMng::getInstance().m_pModelManagerServer->removeToolkit(item->m_name);
        }
    } else if (item->m_type == TreeItem::LeafNode) { // 叶
        QString modelName = item->m_name;
        if (item->m_model) {
            modelName = item->m_model->getPrototypeName();
        }

        if (item->m_toolkitType == KL_TOOLKIT::kProjectType) { // 项目内
            if (item->m_modelType == ControlBlock::Type) {     // 控制代码型
                PModel delModel =
                        ModelManagerViewServerMng::getInstance().m_pProjectMngServer->GetControlModel(modelName);
                if (delModel) {
                    return ModelManagerViewServerMng::getInstance().m_pProjectMngServer->DestroyBoardModel(delModel);
                }
            } else {
                PModel delModel =
                        ModelManagerViewServerMng::getInstance().m_pProjectMngServer->GetBoardModelByName(modelName);
                if (delModel) {
                    return ModelManagerViewServerMng::getInstance().m_pProjectMngServer->DestroyBoardModel(delModel);
                }
            }
        } else { // 删除工具箱的模块
            ModelManagerViewServerMng::getInstance().m_pModelManagerServer->RemoveToolkitModel(item->m_toolkitName,
                                                                                               modelName);
            treeModel->removeChildItem(item);
        }
    }

    return true;
}

void ModelTreeView::onRecevieProjectManagerServerMsg(uint code, const NotifyStruct &param)
{
    auto curIndex = this->currentIndex();

    // recordExpand(this->rootIndex());

    // m_proxyModel->begin();

    // 修改树model数据
    TreeModelMng::getInstance().refreshProjectItem();

    // m_proxyModel->end();

    // this->reset();

    // restoreExpand();

    if (curIndex.isValid()) {
        if (!m_isShowLeaf) {
            setCurrentIndex(curIndex);
            emit clicked(curIndex);
        }
    }
}

void ModelTreeView::onTreeContextMenu(const QPoint &pos)
{
    m_menu->clear();

    QModelIndex index = indexAt(pos);
    if (!index.isValid()) {
        m_menu->addAction(tr("Add Tool Kit"), this, SLOT(onAddToolkit()));
    } else {
        TreeItem *item = this->getItemByIndex(index);
        if (!item) {
            return;
        }

        fillMenuByTreeItem(item); // 填充菜单
    }

    m_menu->exec(cursor().pos());
}

void ModelTreeView::onAddToolkit()
{
    TreeItem *rootItem = m_treeModel->rootItem();
    if (!rootItem || !m_addToolkitDlg || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    m_addToolkitDlg->exec();
}

void ModelTreeView::onAddGroupItem()
{
    auto curIndex = this->currentIndex();
    if (!curIndex.isValid() || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    TreeItem *curItem = getItemByIndex(curIndex);
    if (!curItem) {
        return;
    }

    // 获取用户输入的组名称
    QLineEdit *lineEdit = new QLineEdit(this);
    KCustomDialog *inputDlg = new KCustomDialog(tr("Grounp Name"), lineEdit);
    if (inputDlg->exec() != KCustomDialog::Ok) {
        return;
    }
    QString groupName = lineEdit->text();

    TreeItem *addItem = new TreeItem(groupName, TreeItem::BranchNode, curItem);
    TreeModelMng::getInstance().fillTreeItem(addItem);

    m_treeModel->addChildItem(addItem);

    setCurrentIndex(curIndex);
    emit clicked(curIndex);
}

void ModelTreeView::onAddProjectItem()
{
    auto curIndex = this->currentIndex();
    if (!curIndex.isValid() || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    TreeItem *curItem = getItemByIndex(curIndex);
    if (!curItem) {
        return;
    }

    PModel newModel = addProjectItem(curItem);
}

void ModelTreeView::onAddToolkitItem()
{
    auto curIndex = this->currentIndex();
    if (!curIndex.isValid() || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    TreeItem *curItem = getItemByIndex(curIndex);
    if (!curItem) {
        return;
    }

    PModel newModel = addToolkitItem(curItem);

    if (newModel && curItem->m_toolkitType != kProjectType) {
        // 刷新树节点
        TreeItem *addItem = new TreeItem(newModel->getPrototypeName(), TreeItem::LeafNode, curItem);
        TreeModelMng::getInstance().fillTreeItem(addItem);

        m_treeModel->addChildItem(addItem);

        setCurrentIndex(curIndex);
        emit clicked(curIndex);
    }
}

void ModelTreeView::onEditItem()
{
    auto curIndex = this->currentIndex();
    if (!curIndex.isValid() || !ModelManagerViewServerMng::getInstance().m_pWizardServer
        || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }

    TreeItem *curItem = getItemByIndex(curIndex);
    if (!curItem) {
        return;
    }

    QString modelName = curItem->m_name;
    if (curItem->m_model) {
        modelName = curItem->m_model->getPrototypeName();
    }

    if (curItem->m_toolkitType == KL_TOOLKIT::kProjectType) { // 项目内
        editProjectItem(curItem, modelName);
    } else {
        editToolkitItem(curItem, modelName);
    }
}

void ModelTreeView::onDeleteItem()
{
    auto curIndex = this->currentIndex();
    if (!curIndex.isValid()) {
        return;
    }

    TreeItem *curItem = getItemByIndex(curIndex);
    if (!curItem) {
        return;
    }

    auto parentIndex = curIndex.parent();
    if (KMessageBox::Cancel
        == KMessageBox::warning(ModelViewDefine::IS_DEL_ITEM, KMessageBox::Ok | KMessageBox::Cancel, KMessageBox::Ok)) {
        return;
    }

    auto sourceIndex = m_proxyModel->mapToSource(parentIndex);
    if (!deleteItem(curItem, sourceIndex, m_treeModel)) {
        return;
    }

    if (parentIndex.isValid()) {
        setCurrentIndex(parentIndex);
        emit clicked(parentIndex);
    }
}

void ModelTreeView::onCloseAddToolKit(int ret)
{
    if (ret != KBaseDlgBox::Ok) {
        return;
    }

    TreeItem *rootItem = m_treeModel->rootItem();
    QString addToolkitName = m_leAddToolkit->text();

    // 检查合法性
    auto allToolkitNames = ModelManagerViewServerMng::getInstance().m_pModelManagerServer->GetToolkitNames();
    for (auto toolkitName : allToolkitNames) {
        if ((TranslationTool::getTrToolkitName(toolkitName) == addToolkitName) // 中英文翻译
            || (toolkitName == addToolkitName)) {
            KMessageBox::warning(tr("There is a toolkit with the same name!"));
            return;
        }
    }

    TreeItem *addItem = new TreeItem(addToolkitName, TreeItem::HeadNode, rootItem);
    addItem->m_toolkitName = addToolkitName;
    addItem->m_toolkitType = kUserType;

    m_treeModel->addChildItem(addItem);

    // 添加该工具箱到模型管理服务
    ModelManagerViewServerMng::getInstance().m_pModelManagerServer->addToolkit(addToolkitName);
}

void ModelTreeView::fillMenuByTreeItem(TreeItem *item)
{
    if (!item) {
        return;
    }

    if (item->m_toolkitType == KL_TOOLKIT::kProjectType) { // 本项目
        switch (item->m_type) {
        case TreeItem::BranchNode: {
            m_menu->addAction(tr("Add"), this, SLOT(onAddProjectItem()));
        } break;
        case TreeItem::LeafNode: {
            m_menu->addAction(tr("Edit"), this, SLOT(onEditItem()));
            m_menu->addAction(tr("Delete"), this, SLOT(onDeleteItem()));
        } break;
        }
    } else {
        bool canOperate = true;
        if (item->m_toolkitType == KL_TOOLKIT::kSystemType) { // 开发模块下,才能操作系统工具箱
#ifdef COMPILER_DEVELOPER_MODE
            canOperate = true;
#else
            canOperate = false;
#endif
        }

        if (canOperate) {
            switch (item->m_type) {
            case TreeItem::HeadNode: {
                m_menu->addAction(tr("Add Group"), this, SLOT(onAddGroupItem()));
                m_menu->addAction(tr("Delete Tool Kit"), this, SLOT(onDeleteItem())); // 删除工具箱
            } break;
            case TreeItem::BranchNode: {
                m_menu->addAction(tr("Add"), this, SLOT(onAddToolkitItem()));
                m_menu->addAction(tr("Add Group"), this, SLOT(onAddGroupItem()));
                m_menu->addAction(tr("Delete Group"), this, SLOT(onDeleteItem()));
            } break;
            case TreeItem::LeafNode: { // 叶子节点
                m_menu->addAction(tr("Edit"), this, SLOT(onEditItem()));
                if (item->m_modelType == ControlBlock::Type || item->m_modelType == ElectricalBlock::Type) {
                    m_menu->addAction(tr("Delete"), this, SLOT(onDeleteItem()));
                }
            } break;
            }
        }
    }
}

void ModelTreeView::recordExpand(QModelIndex parentIndex)
{
    int row = m_proxyModel->rowCount(parentIndex);
    for (int i = 0; i < row; i++) {
        QModelIndex index = m_proxyModel->index(i, 0, parentIndex);
        if (this->isExpanded(index)) {
            m_expandNode.push_back(index);
        }
        recordExpand(index);
    }
}

void ModelTreeView::restoreExpand()
{
    for (auto index : m_expandNode) {
        if (index.isValid()) {
            this->expand(index);
        }
    }
    m_expandNode.clear();
}

void ModelTreeView::onFilter(const QString &strFilter)
{
    treeFilter(strFilter);
}
void ModelTreeView::treeFilter(const QString &strFilter)
{
    m_proxyModel->setFilterString(strFilter);
}
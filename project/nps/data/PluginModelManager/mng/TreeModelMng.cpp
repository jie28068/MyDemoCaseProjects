#include "TreeModelMng.h"
#include "CoreLib/ServerManager.h"
#include "DSNFuctionMng.h"
#include "ModelManagerConst.h"
#include "ModelManagerViewServerMng.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "TreeModelMng.h"
#include <ModelViewDefine.h>
#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>

USE_LOGOUT_("TreeModelMng")

TreeModelMng &TreeModelMng::getInstance()
{
    static TreeModelMng instance;
    return instance;
}

TreeModelMng::TreeModelMng()
{
    m_treeModel = new TreeModel();
}

TreeModelMng::~TreeModelMng() { }

void TreeModelMng::initTreeModel()
{
    initProjectModels();
    initUserModels();
    initSystemModels();
}

void TreeModelMng::saveTreeModel()
{
    TreeItem *rootItem = m_treeModel->rootItem();
    for (auto item : rootItem->children()) {
        if (item->m_toolkitType == KL_TOOLKIT::kProjectType) {
            continue;
        }

#ifndef COMPILER_DEVELOPER_MODE // 非开发模式 不保存系统级工具箱
        if (item->m_toolkitType == KL_TOOLKIT::kSystemType) {
            continue;
        }
#endif

        saveJson(item);
    }
}

void TreeModelMng::initProjectModels()
{
    auto &proServer = ModelManagerViewServerMng::getInstance().m_pProjectMngServer;
    if (!proServer || !m_treeModel) {
        return;
    }
    TreeItem *rootItem = m_treeModel->rootItem();

    TreeItem *projectItem = new TreeItem(ModelViewDefine::NODE_THE_PRJ, TreeItem::HeadNode, rootItem);
    projectItem->m_toolkitName = KL_TOOLKIT::PROJECT_TOOLKIT; // 项目工具箱
    projectItem->m_toolkitType = KL_TOOLKIT::kProjectType;
    rootItem->addChild(projectItem);

    // 本地代码型
    TreeItem *codeItem = new TreeItem(ModelViewDefine::NODE_LOCAL_CODE_MODEL, TreeItem::BranchNode, projectItem);
    codeItem->m_modelType = ControlBlock::Type;
    projectItem->addChild(codeItem);
    auto codeModelList = proServer->GetAllBoardModel(ControlBlock::Type);
    for (auto codeModel : codeModelList) {
        TreeItem *item = new TreeItem(codeModel->getPrototypeName(), TreeItem::LeafNode, codeItem);
        fillTreeItem(item, true);
        codeItem->addChild(item);
    }

    // 本地控制构造型
    TreeItem *combineItem = new TreeItem(ModelViewDefine::NODE_CONTROL_COMB_MODEL, TreeItem::BranchNode, projectItem);
    combineItem->m_modelType = CombineBoardModel::Type;
    projectItem->addChild(combineItem);
    auto combineModelList = proServer->GetAllBoardModel(CombineBoardModel::Type);
    for (auto combineModel : combineModelList) {
        TreeItem *item = new TreeItem(combineModel->getPrototypeName(), TreeItem::LeafNode, combineItem);
        fillTreeItem(item, true);
        combineItem->addChild(item);
    }

    // 本地电气构造型
    TreeItem *eleCombineItem = new TreeItem(ModelViewDefine::NODE_ELEC_COMB_MODEL, TreeItem::BranchNode, projectItem);
    eleCombineItem->m_modelType = ElecCombineBoardModel::Type;
    projectItem->addChild(eleCombineItem);
    auto eleCombineModelList = proServer->GetAllBoardModel(ElecCombineBoardModel::Type);
    for (auto eleCombineModel : eleCombineModelList) {
        TreeItem *item = new TreeItem(eleCombineModel->getPrototypeName(), TreeItem::LeafNode, eleCombineItem);
        fillTreeItem(item, true);
        eleCombineItem->addChild(item);
    }

    m_treeModel->m_codeItem = codeItem;
    m_treeModel->m_combineItem = combineItem;
    m_treeModel->m_eleCombineItem = eleCombineItem;
}

void TreeModelMng::initUserModels()
{
    // TODO
}

void TreeModelMng::initSystemModels()
{
    // 找到所有系统库Model配置文件
    QDir dir(KL_TOOLKIT::MODEL_DIR);
    if (!dir.exists()) {
        return;
    }
    QFileInfoList infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    // 排序
    for (int i = 0; i < infoList.size(); i++) {
        auto fileInfo = infoList.at(i);
        auto fileName = fileInfo.baseName();
        if (fileName == KL_TOOLKIT::CONTROL_TOOLKIT || fileName == KL_TOOLKIT::ELECTRICAL_TOOLKIT) {
            infoList.removeAt(i);
            infoList.prepend(fileInfo); // 放到列表头
        }
    }

    TreeItem *rootItem = m_treeModel->rootItem();
    // 解析配置文件.json
    for (auto fileInfo : infoList) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();

        // 以配置文件名称作为一级节点名称
        TreeItem *kitItem = new TreeItem(fileInfo.baseName(), TreeItem::HeadNode, rootItem);
        fillTreeItem(kitItem); // 填充Item信息
        kitItem->m_toolkitName = kitItem->m_name;
        kitItem->m_toolkitType = (ToolkitType)obj.value(ModelViewDefine::JSON_GOURP_OBJ_NAME).toInt(-1);
        if (kitItem->m_toolkitType == -1) { // 没有工具箱类型字段，默认为系统级
            kitItem->m_toolkitType = KL_TOOLKIT::kSystemType;
        }
        rootItem->addChild(kitItem);

        // 解析模块库
        parseJsonObj(obj, kitItem);

        // 初始化授权
        initAuthorization(kitItem);
    }
}

void TreeModelMng::initAuthorization(TreeItem *item)
{
    QString toolkitName = item->m_name;

#ifdef COMPILER_DEVELOPER_MODE
    authorizeItem(item, true);
    return;
#endif

    bool isLicensed = DSNFuctionMng::getInstance().isToolKitsLicensed(toolkitName);
    authorizeItem(item, isLicensed);
}

void TreeModelMng::refreshProjectItem()
{
    auto &proServer = ModelManagerViewServerMng::getInstance().m_pProjectMngServer;
    if (!proServer || !m_treeModel) {
        return;
    }

    // 清除项目item
    m_treeModel->removeChildrenItems(m_treeModel->m_codeItem);
    m_treeModel->removeChildrenItems(m_treeModel->m_combineItem);
    m_treeModel->removeChildrenItems(m_treeModel->m_eleCombineItem);

    auto codeModelList = proServer->GetAllBoardModel(ControlBlock::Type);
    addItemByModelList(codeModelList, m_treeModel->m_codeItem);

    auto combineModelList = proServer->GetAllBoardModel(CombineBoardModel::Type);
    addItemByModelList(combineModelList, m_treeModel->m_combineItem);

    auto eleCombineModelList = proServer->GetAllBoardModel(ElecCombineBoardModel::Type);
    addItemByModelList(eleCombineModelList, m_treeModel->m_eleCombineItem);
}

void TreeModelMng::addItemByModelList(QList<PModel> modelList, TreeItem *parentItem)
{
    for (auto model : modelList) {
        TreeItem *item = new TreeItem(model->getPrototypeName(), TreeItem::LeafNode, parentItem);
        fillTreeItem(item, true);

        m_treeModel->addChildItem(item);
    }
}

void TreeModelMng::authorizeItem(TreeItem *item, bool isAuthorised)
{
    if (!item) {
        return;
    }

    item->m_isAuthorized = isAuthorised;

    for (auto childItem : item->children()) {
        authorizeItem(childItem, isAuthorised);
    }
}

void TreeModelMng::parseJsonObj(QJsonObject obj, TreeItem *parentItem)
{
    if (obj.contains(ModelViewDefine::JSON_GOURP_OBJ)) { // 分组节点
        QJsonArray array = obj.value(ModelViewDefine::JSON_GOURP_OBJ).toArray();
        for (auto ele : array) {
            auto obj = ele.toObject();
            TreeItem *item = new TreeItem(obj.value(ModelViewDefine::JSON_GOURP_OBJ_NAME).toString(),
                                          TreeItem::BranchNode, parentItem);
            item->m_Expand = obj.value(ModelViewDefine::JSON_GOURP_OBJ_Expand).toBool();
            fillTreeItem(item);
            parentItem->addChild(item);

            parseJsonObj(obj, item);
        }
    }

    if (obj.contains(ModelViewDefine::JSON_MODEL_OBJ)) { // Model节点（叶子节点）
        QJsonArray array = obj.value(ModelViewDefine::JSON_MODEL_OBJ).toArray();
        for (auto ele : array) {
            auto obj = ele.toObject();
            TreeItem *item = new TreeItem(obj.value(ModelViewDefine::JSON_MODEL_OBJ_NAME).toString(),
                                          TreeItem::LeafNode, parentItem);
            item->m_isShow = obj.value(ModelViewDefine::JSON_MODEL_OBJ_SHOW).toBool(true);
            fillTreeItem(item);

            if (item->m_isShow) {
                parentItem->addChild(item);
            }
        }
    }
}

void TreeModelMng::fillTreeItem(TreeItem *item, bool isProject)
{
    if (item->m_type != TreeItem::LeafNode || !ModelManagerViewServerMng::getInstance().m_pModelManagerServer) {
        return;
    }
    auto parentItem = item->parent();
    if (!parentItem) {
        return;
    }

    auto &modelServer = ModelManagerViewServerMng::getInstance().m_pModelManagerServer;
    auto &projectServer = ModelManagerViewServerMng::getInstance().m_pProjectMngServer;
    PModel model;
    if (isProject) { // 项目内
        auto curProject = projectServer->GetCurProject();
        if (!curProject) {
            return;
        }
        model = curProject->getModel(item->m_name);
    } else { // 工具箱
        model = modelServer->GetToolkitModel(item->m_name);
    }

    if (model) {
        item->m_image = getModelImageData(model); // 填充图片
        item->m_model = model;                    // 填充模型指针
        item->m_modelType = model->getModelType();

        // 填充名称
        if (item->m_modelType == ElectricalBlock::Type) {
            item->m_name = model->getPrototypeName_CHS(); // 电气需要翻译
        } else if (item->m_modelType == ControlBlock::Type) {
            item->m_name = model->getPrototypeName_Readable();
            if (item->m_name.isEmpty()) {
                item->m_name = model->getPrototypeName();
            }
        }
    } else {
        item->m_isShow = false;
        LOGOUT(QObject::tr("Toolkit[%1] no model[%2]").arg(parentItem->m_toolkitName).arg(item->m_name),
               Kcc::LOG_WARNING);
    }
}

bool TreeModelMng::saveJson(TreeItem *item)
{
    QJsonDocument jsDoc;
    QJsonObject jsObj;

    QJsonArray groupArray;
    for (auto childItem : item->children()) {
        QJsonObject groupObj;
        fillJsObj(groupObj, childItem); // 分组层级
        groupArray.append(groupObj);
    }

    jsObj.insert(ModelViewDefine::JSON_TOOLKIT_TYPE, item->m_toolkitType);
    jsObj.insert(ModelViewDefine::JSON_GOURP_OBJ, groupArray);
    jsDoc.setObject(jsObj);

    QFile file(KL_TOOLKIT::MODEL_DIR + item->m_name + ".json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8"); // 设置写入编码是UTF8
    stream << jsDoc.toJson();
    file.close();

    return true;
}

void TreeModelMng::fillJsObj(QJsonObject &groupObj, TreeItem *item)
{
    if (item->m_type == TreeItem::BranchNode) {
        QJsonArray modelArray;
        QJsonArray groupArray;

        for (auto childItem : item->children()) {
            if (childItem->m_type == TreeItem::LeafNode) { // model
                QJsonObject modelObj;
                QString prototypeName = childItem->m_name;
                // 原型名称
                if (childItem->m_model) {
                    prototypeName = childItem->m_model->getPrototypeName();
                }
                modelObj.insert(ModelViewDefine::JSON_MODEL_OBJ_NAME, prototypeName);
                modelObj.insert(ModelViewDefine::JSON_MODEL_OBJ_SHOW, childItem->m_isShow);
                modelArray.append(modelObj);
            } else if (childItem->m_type == TreeItem::BranchNode) { // 组
                QJsonObject nextGroupObj;
                fillJsObj(nextGroupObj, childItem);
                groupArray.append(nextGroupObj);
            }
        }

        groupObj.insert(ModelViewDefine::JSON_GOURP_OBJ_NAME, item->m_name);
        groupObj.insert(ModelViewDefine::JSON_GOURP_OBJ_Expand, item->m_Expand);
        groupObj.insert(ModelViewDefine::JSON_GOURP_OBJ_ORDER, item->m_order);
        groupObj.insert(ModelViewDefine::JSON_GOURP_OBJ, groupArray);
        groupObj.insert(ModelViewDefine::JSON_MODEL_OBJ, modelArray);
    }
}

QVariant TreeModelMng::getModelImageData(PModel model)
{
    // 存在内置图片
    if (model->getResource().contains(Kcc::BlockDefinition::PNG_NORMAL_PIC)) {
        return model->getResource().value((Kcc::BlockDefinition::PNG_NORMAL_PIC)); // PNG图片
    }

    // 为构造模块，切有背景图片
    if ((model->getModelType() == CombineBoardModel::Type || model->getModelType() == ElecCombineBoardModel::Type)
        && model->getResource().contains(CANVAS_BACKGROUND_IMAGE)) {
        auto imageData = model->getResource().value((Kcc::BlockDefinition::CANVAS_BACKGROUND_IMAGE)); // 背景图片
        QImage image = qvariant_cast<QImage>(imageData);
        if (!image.isNull()) {
            return image;
        }
    }

    return QVariant();
}

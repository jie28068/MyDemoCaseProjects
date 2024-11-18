#include "controlblocktypeselectview.h"

// 注册及请求服务所需要的头文件
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"
const static QString LINE_PROTOTYPE_NAME = "Line";
const static QString LOAD_PROTOTYPE_NAME = "Load";
const static QString BUSBAR_PROTOTYPE_NAME = "Busbar";
const static QString GROUND_PROTOTYPE_NAME = "Ground";
const static QString DBUSBAR_PROTOTYPE_NAME = "DotBusbar";
const static QString RESISTOR_PROTOTYPE_NAME = "Resistor";
const static QString CAPACITOR_PROTOTYPE_NAME = "Capacitor";
const static QString SM_PROTOTYPE_NAME = "SynchronousMachine";
const static QString TWT_PROTOTYPE_NAME = "TwoWindingTransformer";
// 特殊模块 电气接口模块
static const QString SPECIAL_BLOCK_ELECINTERFACE = "ElectricalInterface";
static const QString TITLE_MODULE_NAME = QObject::tr("Module Name");       // 模型名
static const QString TITLE_REMARKS = QObject::tr("Remarks");               // 备注
static const QString TITLE_CLASS = QObject::tr("Class");                   // 类型
static const QString TITLE_ELEMENT_SELECT = QObject::tr("Element Select"); // 元件选择
static const QString ControllerGroupOther = QString("接口模块");
using namespace Kcc::BlockDefinition;

USE_LOGOUT_("ControlBlockTypeSelectView")

ControlBlockTypeSelectView::ControlBlockTypeSelectView(ControlBlockTypeSelectView::SelectType selectType,
                                                       QSharedPointer<Kcc::BlockDefinition::Variable> filterVariable,
                                                       QWidget *parent)
    : QWidget(parent), m_ptreeModel(nullptr), m_pTableModel(nullptr), m_selectType(selectType)
{
    ui.setupUi(this);
    if (ControlBlockTypeSelectView::SelectType_Invalid == selectType) {
        return;
    }

    InitUI(selectType, filterVariable);
}

ControlBlockTypeSelectView::~ControlBlockTypeSelectView() { }

QSharedPointer<Kcc::BlockDefinition::Model> ControlBlockTypeSelectView::getSelectedModel()
{
    if (m_pTableModel == nullptr || ControlBlockTypeSelectView::SelectType_Invalid == m_selectType) {
        return PModel(nullptr);
    }
    if (ControlBlockTypeSelectView::SelectType_AllInstance == m_selectType
        || ControlBlockTypeSelectView::SelectType_CtrInstance == m_selectType
        || ControlBlockTypeSelectView::SelectType_ElecInstance == m_selectType) {
        return findSelectInstacne();
    } else {
        return findSelectPrototype();
    }
}

void ControlBlockTypeSelectView::onClickedTreeItem(const QModelIndex &index)
{
    if (!index.isValid() || m_ptreeModel == nullptr || m_pTableModel == nullptr) {
        return;
    }

    m_pTableModel->updateBlockNameList(m_groupInfoMap[index.data().toString()]);
    if (ui.tableView->verticalScrollBar() != nullptr) {
        ui.tableView->verticalScrollBar()->setValue(0);
    }
}
void ControlBlockTypeSelectView::onDoubleClickedTableItem(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.data(NPS::ModelDataEditRole).toBool()) {
        return;
    }
    emit typeSelect(getSelectedModel());
}

void ControlBlockTypeSelectView::InitUI(ControlBlockTypeSelectView::SelectType selectType,
                                        QSharedPointer<Kcc::BlockDefinition::Variable> filterVariable)
{
    // 创建model
    m_ptreeModel = new QStandardItemModel(ui.treeView);
    m_pTableModel = new CustomTableModel(ui.tableView);
    m_pTableModel->setModelDataColorUse(false);
    // 添加分类
    m_groupInfoMap.clear();
    if (PropertyServerMng::getInstance().m_ControlComponentServer == nullptr
        || PropertyServerMng::getInstance().m_ElectricalComponentServer == nullptr) {
        return;
    }
    QString filterGroup;
    QString filterBlockProto;
    if (filterVariable != nullptr) {
        filterGroup = filterVariable->getData(RoleDataDefinition::FilerBelongTypeRole).toString();
        filterBlockProto = filterVariable->getData(RoleDataDefinition::FilerProtoTypeRole).toString();
    }
    if (ControlBlockTypeSelectView::SelectType_ProjectCtrPrototoType == selectType) {
        addGroupInfo(PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentInfo(GetDataType_onlyProj),
                     false);
    } else if (ControlBlockTypeSelectView::SelectType_CtrPrototoType == selectType) {
        addGroupInfo(PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentInfo(), false);
    } else if (ControlBlockTypeSelectView::SelectType_ElecPrototoType == selectType) {
        addGroupInfo(PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentInfo(), true);
    } else if (ControlBlockTypeSelectView::SelectType_AllPrototoType == selectType) {
        addGroupInfo(PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentInfo(), false);
        addGroupInfo(PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentInfo(), true);
    } else if (ControlBlockTypeSelectView::SelectType_CtrInstance == selectType) {
        addInstances(getInstanceList(ControlBlock::Type), NPS::ControllerGroupCustomCodeInstance);
        addInstances(getInstanceList(CombineBoardModel::Type), NPS::ControllerGroupCustomBoardInstance);
    } else if (ControlBlockTypeSelectView::SelectType_ElecInstance == selectType) {
        addGroupInfo(CMA::getProjectModels(ElecBoardModel::Type), filterBlockProto);
    } else if (ControlBlockTypeSelectView::SelectType_AllInstance == selectType) {
        if (NPS::FilterBelongType_Control == filterGroup) {
            addInstances(getInstanceList(ControlBlock::Type), NPS::ControllerGroupCustomCodeInstance);
            addInstances(getInstanceList(CombineBoardModel::Type), NPS::ControllerGroupCustomBoardInstance);
        } else if (NPS::FilterBelongType_Electrical == filterGroup) {
            addGroupInfo(CMA::getProjectModels(ElecBoardModel::Type), filterBlockProto);
        } else {
            addInstances(getInstanceList(ControlBlock::Type), NPS::ControllerGroupCustomCodeInstance);
            addInstances(getInstanceList(CombineBoardModel::Type), NPS::ControllerGroupCustomBoardInstance);
            addGroupInfo(CMA::getProjectModels(ElecBoardModel::Type), filterBlockProto);
        }
    }

    ui.tableView->verticalHeader()->setVisible(false);
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView->setItemDelegate(new CustomTableItemDelegate(this));
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.treeView->setModel(m_ptreeModel);
    ui.tableView->setModel(m_pTableModel);
    ui.treeView->expandAll();
    int maxWidth = 100;
    for (int logicalIndex = 0; logicalIndex < m_ptreeModel->rowCount(); ++logicalIndex) {
        ui.treeView->resizeColumnToContents(logicalIndex);
        int rwidth = ui.treeView->columnWidth(logicalIndex) + 5;
        if (rwidth > maxWidth) {
            maxWidth = rwidth;
        }
    }
    ui.treeView->setFixedWidth(maxWidth);
    ui.widgetTree->setFixedWidth(maxWidth + 2);
    ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QModelIndex index = m_ptreeModel->index(0, 0);
    ui.treeView->setCurrentIndex(index);
    onClickedTreeItem(index);

    connect(ui.treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onClickedTreeItem(const QModelIndex &)));
    connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(onDoubleClickedTableItem(const QModelIndex &)));
    this->setMinimumWidth(ui.widgetTree->width() + 10 + 480);
}

void ControlBlockTypeSelectView::addGroupInfo(const Kcc::PluginComponent::ComponentInfo &info, bool electrical)
{
    if (m_ptreeModel == nullptr || PropertyServerMng::getInstance().m_ControlComponentServer == nullptr
        || PropertyServerMng::getInstance().m_ElectricalComponentServer == nullptr) {
        return;
    }

    QString realgroupname;
    QVariantMap otherpromap;
    foreach (QString groupname, info.listGroup) {
        if (ControllerGroupOther == groupname) {
            continue;
        }
        if (electrical) {
            realgroupname = NPS::ElectricGroup;
        } else {
            realgroupname = groupname;
        }
        if (info.mapComponents[groupname].contains(NPS::PROTOTYPENAME_SLOT)) {
            continue;
        }

        if (!m_groupInfoMap.contains(realgroupname)) {
            GroupInfo info(realgroupname);
            m_ptreeModel->appendRow(new QStandardItem(realgroupname));
            // 模块名 备注
            info.bElectrical = electrical;
            info.groupTitle = QStringList() << TITLE_MODULE_NAME << TITLE_REMARKS;
            m_groupInfoMap.insert(realgroupname, info);
        }
        foreach (QString prototypename, info.mapComponents[groupname]) {
            otherpromap.clear();
            otherpromap.insert(KEY_ISPROTOTYPE, true);
            otherpromap.insert(KEY_PROTOTYPENAME, prototypename);
            otherpromap.insert(KEY_BLOCKPIXMAP,
                               electrical
                                       ? PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentIcon(
                                               prototypename, BLOCKTYPE_PROTOTYPE)
                                       : PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentIcon(
                                               prototypename, BLOCKTYPE_PROTOTYPE));
            PModel model = CMA::getModelByPrototype(prototypename, CMA::ProtoRange_OnlyBuildIn);
            m_groupInfoMap[realgroupname].groupModelList.append(
                    CustomModelItem(prototypename,
                                    (model != nullptr) ? (ElectricalBlock::Type == model->getModelType()
                                                                  ? model->getPrototypeName_CHS(false)
                                                                  : model->getPrototypeName_Readable())
                                                       : prototypename,
                                    (model != nullptr) ? model->getPrototypeName_CHS() : prototypename,
                                    RoleDataDefinition::ControlTypeTextbox, false, "", otherpromap));
        }
    }
}

void ControlBlockTypeSelectView::addInstances(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> instanceModels,
                                              const QString &groupName)
{
    if (groupName.isEmpty() || instanceModels.size() <= 0) {
        return;
    }
    QVariantMap otherPropertyMap;
    for (PModel model : instanceModels) {
        if (model == nullptr) {
            continue;
        }
        otherPropertyMap.clear();
        if (!m_groupInfoMap.contains(groupName)) {
            GroupInfo info(groupName);
            m_ptreeModel->appendRow(new QStandardItem(groupName));
            info.bElectrical = false;
            info.groupTitle = QStringList() << TITLE_MODULE_NAME << TITLE_REMARKS;
            m_groupInfoMap.insert(groupName, info);
        }
        otherPropertyMap.insert(KEY_UUID, model->getUUID());
        otherPropertyMap.insert(KEY_ISPROTOTYPE, false);
        otherPropertyMap.insert(KEY_MODELTYPE, model->getModelType());
        m_groupInfoMap[groupName].groupModelList.append(
                CustomModelItem(model->getName(), model->getName(), QObject::tr(model->getPrototypeName().toUtf8()),
                                RoleDataDefinition::ControlTypeTextbox, false, "", otherPropertyMap));
    }
}

void ControlBlockTypeSelectView::addGroupInfo(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> elecboards,
                                              const QString &filterprototype)
{
    if (m_ptreeModel == nullptr || PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    if (!m_groupInfoMap.contains(NPS::ElectricGroup)) {
        GroupInfo info(NPS::ElectricGroup);
        m_ptreeModel->appendRow(new QStandardItem(NPS::ElectricGroup));
        info.bElectrical = true;
        info.groupTitle = QStringList() << QStringList() << TITLE_CLASS << TITLE_ELEMENT_SELECT;
        m_groupInfoMap.insert(NPS::ElectricGroup, info);
    }
    QStringList rangelist;
    QStringList blocklist;
    for (PModel model : elecboards) {
        if (model == nullptr) {
            continue;
        }
        rangelist.append(model->getName());
    }

    m_groupInfoMap[NPS::ElectricGroup].groupModelList.append(
            CustomModelItem(CMA::ELEC_DRAWBOARD_NAME, CMA::ELEC_DRAWBOARD_NAME, "",
                            RoleDataDefinition::ControlTypeComboBox, true, rangelist.join(",")));
    QVariantMap otherPropertyMap;
    if (filterprototype.isEmpty()) {
        m_groupInfoMap[NPS::ElectricGroup].groupModelList.append(
                CustomModelItem(CMA::ELEC_ELEMENT_PROTOTYPENAME, CMA::ELEC_ELEMENT_PROTOTYPENAME, "",
                                RoleDataDefinition::ControlTypeComboBox, true, "", otherPropertyMap));
    } else {
        otherPropertyMap[KEY_PROTOTYPENAME] = filterprototype;
        PModel model = CMA::getModelByPrototype(filterprototype);
        m_groupInfoMap[NPS::ElectricGroup].groupModelList.append(CustomModelItem(
                CMA::ELEC_ELEMENT_PROTOTYPENAME, CMA::ELEC_ELEMENT_PROTOTYPENAME,
                (model != nullptr) ? model->getPrototypeName_CHS() : filterprototype,
                RoleDataDefinition::ControlTypeComboBox, false,
                (model != nullptr) ? model->getPrototypeName_CHS() : filterprototype, otherPropertyMap));
    }
    m_groupInfoMap[NPS::ElectricGroup].groupModelList.append(CustomModelItem(
            CMA::ELEC_ELEMENT_NAME, CMA::ELEC_ELEMENT_NAME, "", RoleDataDefinition::ControlTypeComboBox, true, ""));
}

QList<CustomModelItem> ControlBlockTypeSelectView::getBlockList(const QString &groupname)
{
    if (m_groupInfoMap.contains(groupname)) {
        return m_groupInfoMap[groupname].groupModelList;
    }
    return QList<CustomModelItem>();
}

QSharedPointer<Kcc::BlockDefinition::Model> ControlBlockTypeSelectView::findSelectInstacne()
{
    if (getCurGroupName() == NPS::ElectricGroup) {
        return findElectricalBlock();
    } else {
        return findControlInstance();
    }
}

QSharedPointer<Kcc::BlockDefinition::Model> ControlBlockTypeSelectView::findElectricalBlock()
{
    if (m_pTableModel == nullptr) {
        return PModel(nullptr);
    }
    CustomModelItem itemboard = m_pTableModel->getModelItem(CMA::ELEC_DRAWBOARD_NAME);
    CustomModelItem itemprototype = m_pTableModel->getModelItem(CMA::ELEC_ELEMENT_PROTOTYPENAME);
    CustomModelItem itemblockname = m_pTableModel->getModelItem(CMA::ELEC_ELEMENT_NAME);
    if (itemboard.value.toString().isEmpty() || itemprototype.value.toString().isEmpty()
        || itemblockname.value.toString().isEmpty()) {
        return PModel(nullptr);
    }
    QList<PModel> elecboards = CMA::getProjectModels(ElecBoardModel::Type);

    for (PModel model : elecboards) {
        if (model == nullptr || itemboard.value.toString() != model->getName()) {
            continue;
        }
        for (PModel eblock : model->getChildModels().values()) {
            if (eblock->getName() == itemblockname.value.toString()) {
                return eblock;
            }
        }
    }
    return PModel(nullptr);
}

QSharedPointer<Kcc::BlockDefinition::Model> ControlBlockTypeSelectView::findControlInstance()
{
    QModelIndex index = getTableCurIndex();
    if (!index.isValid()) {
        return PModel(nullptr);
    }
    CustomModelItem item = m_pTableModel->getModelItem(index.data(NPS::ModelDataKeywordsRole).toString());
    QString uuidblock = item.otherPropertyMap[KEY_UUID].toString();
    int modelType = item.otherPropertyMap[KEY_MODELTYPE].toInt();
    QList<PModel> instanceList = getInstanceList(modelType);
    for (PModel model : instanceList) {
        if (model != nullptr && model->getUUID() == uuidblock) {
            return model;
        }
    }
    return PModel(nullptr);
}

QSharedPointer<Kcc::BlockDefinition::Model> ControlBlockTypeSelectView::findSelectPrototype()
{
    if (PropertyServerMng::getInstance().m_ModelManagerServer == nullptr) {
        return PModel(nullptr);
    }
    QModelIndex tableindex = getTableCurIndex();
    if (!tableindex.isValid()) {
        LOGOUT(tr("selection failed, please reselect!"), LOG_WARNING); // 选择失败,请重新选择！
        return PModel(nullptr);
    }
    QString prototype = tableindex.sibling(tableindex.row(), 0).data(NPS::ModelDataKeywordsRole).toString();
    return CMA::getModelByPrototype(prototype);
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>> ControlBlockTypeSelectView::getInstanceList(int modelType)
{

    QList<PModel> instanceModels;
    QList<PModel> ProtoModels = CMA::getProjectModels(modelType);
    for (PModel model : ProtoModels) {
        if (model == nullptr || model->getInstanceList().size() <= 0) {
            continue;
        }
        instanceModels.append(model->getInstanceList());
    }
    qSort(instanceModels.begin(), instanceModels.end(), [&instanceModels](PModel lhs, PModel rhs) -> bool {
        if (lhs == nullptr || rhs == nullptr) {
            return false;
        }
        return lhs->getName() < rhs->getName();
    });

    return instanceModels;
}

QString ControlBlockTypeSelectView::getCurGroupName()
{
    if (ui.treeView == nullptr) {
        return QString();
    }
    QModelIndex index = ui.treeView->currentIndex();
    if (!index.isValid()) {
        return QString();
    }
    return index.data().toString();
}

QModelIndex ControlBlockTypeSelectView::getTableCurIndex()
{
    if (ui.tableView == nullptr) {
        return QModelIndex();
    }
    return ui.tableView->currentIndex();
}

// tablemodel
CustomTableModel::CustomTableModel(QObject *parent /*= nullptr*/) : PropertyTableModel(parent), m_bElecBlock(false) { }

CustomTableModel::~CustomTableModel() { }

bool CustomTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    if (index.data(NPS::ModelDataKeywordsRole).toString() == CMA::ELEC_DRAWBOARD_NAME) {
        CustomModelItem itemeleproto = getModelItem(CMA::ELEC_ELEMENT_PROTOTYPENAME);
        CustomModelItem itemelename = getModelItem(CMA::ELEC_ELEMENT_NAME);
        PModel boardmodel = findBoardModel(value.toString());
        if (itemeleproto.bcanEdit) {
            itemeleproto.value = "";
            itemeleproto.otherPropertyMap[KEY_PROTOTYPENAME] = "";
            itemeleproto.controlLimit = getProtoRange(boardmodel);
        }
        itemelename.value = "";
        itemelename.controlLimit =
                getNameRange(boardmodel, itemeleproto.otherPropertyMap[KEY_PROTOTYPENAME].toString());
        updateValue(CMA::ELEC_DRAWBOARD_NAME, value);
        updateValue(CMA::ELEC_ELEMENT_PROTOTYPENAME, itemeleproto, false);
        updateValue(CMA::ELEC_ELEMENT_NAME, itemelename, false);
        return true;
    } else if (index.data(NPS::ModelDataKeywordsRole).toString() == CMA::ELEC_ELEMENT_PROTOTYPENAME) {
        CustomModelItem boardnameitem = getModelItem(CMA::ELEC_DRAWBOARD_NAME);
        PModel boardmodel = findBoardModel(boardnameitem.value.toString());
        CustomModelItem itemeleproto = getModelItem(CMA::ELEC_ELEMENT_PROTOTYPENAME);
        itemeleproto.otherPropertyMap[KEY_PROTOTYPENAME] = m_ChsMapToProtoType[value.toString()];
        itemeleproto.value = value;
        updateValue(CMA::ELEC_ELEMENT_PROTOTYPENAME, itemeleproto, false);
        CustomModelItem itemelename = getModelItem(CMA::ELEC_ELEMENT_NAME);
        itemelename.controlLimit =
                getNameRange(boardmodel, itemeleproto.otherPropertyMap[KEY_PROTOTYPENAME].toString());
        itemelename.value = "";
        updateValue(CMA::ELEC_ELEMENT_NAME, itemelename, false);
        return true;
    } else if (index.data(NPS::ModelDataKeywordsRole).toString() == CMA::ELEC_ELEMENT_NAME) {
        updateValue(CMA::ELEC_ELEMENT_NAME, value);
        return true;
    }

    return PropertyTableModel::setData(index, value, role);
}

QVariant CustomTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid() || index.column() >= columnCount() || index.row() >= rowCount()) {
        return QVariant();
    }
    if (Qt::DecorationRole == role && index.column() == 0
        && m_data[index.row()].otherPropertyMap[KEY_ISPROTOTYPE].toBool()) {
        return QIcon(m_data[index.row()].otherPropertyMap[KEY_BLOCKPIXMAP].value<QPixmap>());
    }
    return PropertyTableModel::data(index, role);
}

QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (Qt::Vertical == orientation) {
        if (section < rowCount() && Qt::DisplayRole == role) {
            return QString::number(section + 1);
        }
    }
    return PropertyTableModel::headerData(section, orientation, role);
}

void CustomTableModel::updateBlockNameList(const GroupInfo &groupInfo)
{
    beginResetModel();
    m_bElecBlock = groupInfo.bElectrical;
    PropertyTableModel::setModelData(groupInfo.groupModelList, groupInfo.groupTitle);
    endResetModel();
}

QSharedPointer<Kcc::BlockDefinition::Model> CustomTableModel::findBoardModel(const QString &boardname)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || boardname.isEmpty()) {
        return PModel(nullptr);
    }
    QList<PModel> alleleboard = CMA::getProjectModels(ElecBoardModel::Type);
    for (PModel model : alleleboard) {
        if (model != nullptr && model->getName() == boardname) {
            return model;
        }
    }
    return PModel(nullptr);
}

QString CustomTableModel::getProtoRange(QSharedPointer<Kcc::BlockDefinition::Model> boardmodel)
{
    m_ChsMapToProtoType.clear();
    QStringList sortlist;
    if (boardmodel == nullptr || PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return QString();
    }
    QString chs;
    PModel busModel = CMA::getModelByPrototype(NPS::PROTOTYPENAME_BUSBAR);
    for (PModel cmodel : boardmodel->getChildModels().values()) {
        if (cmodel == nullptr) {
            continue;
        }
        if (NPS::PROTOTYPENAME_BUSBAR == cmodel->getPrototypeName()
            || NPS::PROTOTYPENAME_DOTBUSBAR == cmodel->getPrototypeName()) {
            if (busModel != nullptr) {
                chs = busModel->getPrototypeName_CHS();
            } else {
                chs = cmodel->getPrototypeName_CHS();
            }
        } else {
            chs = cmodel->getPrototypeName_CHS();
        }
        if (!sortlist.contains(chs)) {
            sortlist.append(chs);
            if (NPS::PROTOTYPENAME_BUSBAR == cmodel->getPrototypeName()
                || NPS::PROTOTYPENAME_DOTBUSBAR == cmodel->getPrototypeName()) {
                m_ChsMapToProtoType.insert(chs, NPS::PROTOTYPENAME_BUSBAR);
            } else {
                m_ChsMapToProtoType.insert(chs, cmodel->getPrototypeName());
            }
        }
    }
    qSort(sortlist.begin(), sortlist.end(),
          [&sortlist](const QString &lhs, const QString &rhs) -> bool { return lhs < rhs; });
    return sortlist.join(",");
}

QString CustomTableModel::getNameRange(QSharedPointer<Kcc::BlockDefinition::Model> boardmodel, const QString &prototype)
{
    QStringList sortlist;
    if (boardmodel == nullptr || prototype.isEmpty()) {
        return QString();
    }
    for (PModel cmodel : boardmodel->getChildModels().values()) {
        if (cmodel == nullptr) {
            continue;
        }
        if (NPS::PROTOTYPENAME_BUSBAR == prototype
            && (NPS::PROTOTYPENAME_BUSBAR == cmodel->getPrototypeName()
                || NPS::PROTOTYPENAME_DOTBUSBAR == cmodel->getPrototypeName())) {
            sortlist.append(cmodel->getName());
            continue;
        }

        if (prototype == cmodel->getPrototypeName()) {
            sortlist.append(cmodel->getName());
        }
    }
    qSort(sortlist.begin(), sortlist.end(),
          [&sortlist](const QString &lhs, const QString &rhs) -> bool { return lhs < rhs; });
    return sortlist.join(",");
}

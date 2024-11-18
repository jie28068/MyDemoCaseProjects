#include "CustomTreeModel.h"

#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "ProjDataManager.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "ProjectViewServers.h"
#include "TreeView.h"

using namespace Kcc::ProjectManager;
#include <QDebug>
#include <QPainter>
#include <QtSvg/QSvgRenderer>

USE_LOGOUT_("CustomTreeModel")

CustomTreeModel::CustomTreeModel(QSharedPointer<ProjDataManager> projDataManager, QObject *parent)
    : QStandardItemModel(parent),
      m_pProjDataManager(projDataManager),
      m_curSelectedItemText(""),
      m_scrollIndex(0),
      m_widgetObject(AssistantDefine::WidgetObject_Invalid),
      m_projSortType(CustomTreeModel::ProjectSortType_Name)
{
}

CustomTreeModel::~CustomTreeModel() { }

void CustomTreeModel::updateTree(TreeView *pTreeView)
{
    if (m_pProjDataManager == nullptr) {
        return;
    }
    QMap<QString, WorkSpaceInfo> allworkspace = m_pProjDataManager->getAllWorkSpace();
    this->clear();
    QStandardItem *rootitem = new QStandardItem(
            QIcon(":/PluginProjectManagerView/resource/TreeIcons/database-normal.png"), AssistantDefine::DATAROOT_NAME);
    rootitem->setData(CustomTreeModel::NodeType_DataRoot, CustomTreeModel::CustomRole_NodeType);
    this->appendRow(rootitem);
    // 创建全局库
    CreateGlobalLibrary(rootitem);
    bool bwsactive = false;
    bool projactive = false;
    int rowcount = rootitem->rowCount();
    int colcount = rootitem->columnCount();
    // 排序工作空间
    QStringList wsNameList = allworkspace.keys();
    qSort(wsNameList.begin(), wsNameList.end(),
          [&wsNameList](const QString &lhs, const QString &rhs) -> bool { return NPS::compareString(lhs, rhs); });
    PKLProject curproj = m_pProjDataManager->getCurrentProject();
    QString curprojname;
    if (curproj != nullptr) {
        curprojname = curproj->getProjectPath();
    }

    for (QString wsname : wsNameList) {
        bwsactive = false;
        QStandardItem *wsitem =
                new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/user-normal.png"), wsname);
        wsitem->setData(CustomTreeModel::NodeType_Workspace, CustomTreeModel::CustomRole_NodeType);
        // 排序项目
        QStringList projectNameList = sortProjectList(allworkspace[wsname].allprojects);
        for (QString projname : projectNameList) {
            if (allworkspace[wsname].allprojects[projname] == nullptr) {
                continue;
            }
            projactive = false;
            QString projpath = allworkspace[wsname].allprojects[projname]->getProjectPath();

            if (allworkspace[wsname].allprojects[projname] == curproj && curproj != nullptr) {
                projactive = true;
            }

            QStandardItem *projitem = new QStandardItem(
                    QIcon(":/PluginProjectManagerView/resource/TreeIcons/project-normal.png"), projname);
            projitem->setData(projactive, CustomTreeModel::CustomRole_ActiveStatus);
            projitem->setData(CustomTreeModel::NodeType_Project, CustomTreeModel::CustomRole_NodeType);
            QFont tmpfont = projitem->font();
            if (projactive) {
                tmpfont.setBold(true);
                projitem->setIcon(QIcon(":/PluginProjectManagerView/resource/TreeIcons/project-active.png"));
                bwsactive = true;
            } else {
                tmpfont.setBold(false);
            }
            projitem->setFont(tmpfont);
            createProjectSubTree(allworkspace[wsname].allprojects[projname], projitem, projactive);
            wsitem->appendRow(projitem);
        }
        wsitem->setData(bwsactive, CustomTreeModel::CustomRole_ActiveStatus);
        QFont wstmpfont = wsitem->font();
        if (bwsactive) {
            wstmpfont.setBold(true);
            wsitem->setIcon(QIcon(":/PluginProjectManagerView/resource/TreeIcons/user-active.png"));
        } else {
            wstmpfont.setBold(false);
        }
        wsitem->setFont(wstmpfont);
        rootitem->appendRow(wsitem);
    }
}

void CustomTreeModel::updateNetWorkTree(TreeView *pTreeView)
{
    if (m_pProjDataManager == nullptr) {
        this->clear();
        return;
    }
    this->clear();
    // 1 device type item
    QStandardItem *DeviceTypeItem =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/deviceType-normal.png"),
                              AssistantDefine::DEVICETYPE_NAME);
    DeviceTypeItem->setData(CustomTreeModel::NodeType_NetworkDeviceType, CustomTreeModel::CustomRole_NodeType);
    // 2electrical elements
    QStandardItem *elecElements =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/element-normal.png"),
                              AssistantDefine::ELECTRICAL_ELEMENTS);
    elecElements->setData(CustomTreeModel::NodeType_NetworkElecElement, CustomTreeModel::CustomRole_NodeType);
    // 3block template
    QStandardItem *blockTemplate =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlSystem-normal.png"),
                              AssistantDefine::MODEL_TEMPLATE);
    blockTemplate->setData(CustomTreeModel::NodeType_NetworkBlockTemplate, CustomTreeModel::CustomRole_NodeType);
    // 4block instance
    QStandardItem *blockInstacne =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlSystem-normal.png"),
                              AssistantDefine::MODEL_INSTANCE);
    blockInstacne->setData(CustomTreeModel::NodeType_NetworkBlockInstance, CustomTreeModel::CustomRole_NodeType);
    // 5complex model template
    QStandardItem *complexTemplate =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-normal.png"),
                              AssistantDefine::COMPLEXMODEL_TEMPLATE);
    complexTemplate->setData(CustomTreeModel::NodeType_NetworkComplexTemplate, CustomTreeModel::CustomRole_NodeType);
    // 6complex model instance
    QStandardItem *complexInstance =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-instance.png"),
                              AssistantDefine::COMPLEXMODEL_INSTANCE);
    complexInstance->setData(CustomTreeModel::NodeType_NetworkComplexInstance, CustomTreeModel::CustomRole_NodeType);
    QStandardItem *controlsystem =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlSystem-normal.png"),
                              AssistantDefine::CONTROLSYSTEM);
    controlsystem->setData(CustomTreeModel::NodeType_NetworkControlSystem, CustomTreeModel::CustomRole_NodeType);
    PKLProject pCurProject = m_pProjDataManager->getCurrentProject();
    if (pCurProject != nullptr) {
        // 1 device type data
        DeviceTypeItem->appendRows(getModelItems(pCurProject->getAllModel(DeviceModel::Type), DeviceModel::Type));
        // 2electrical elements data
        elecElements->appendRows(getModelItems(getElecElements(pCurProject), ElectricalBlock::Type));
        // 3block template
        // 4block instance
        blockInstacne->appendRows(
                getModelItems(pCurProject->getAllModel(ControlBlock::Type), ControlBlock::Type,
                              QIcon(":/PluginProjectManagerView/resource/TreeIcons/codeBlock-instance.png"),
                              QVariantMap(), true)
                + getModelItems(pCurProject->getAllModel(CombineBoardModel::Type), CombineBoardModel::Type,
                                QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlBlock-instance.png"),
                                QVariantMap(), true));
        // 5complex model template
        complexTemplate->appendRows(
                getModelItems(pCurProject->getAllModel(ComplexBoardModel::Type), ComplexBoardModel::Type,
                              QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-normal.png")));
        complexInstance->appendRows(
                getModelItems(getAllInstance(pCurProject, ComplexBoardModel::Type), ComplexBoardModel::Type,
                              QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-instance.png")));
        controlsystem->appendRows(
                getControlSysItems(pCurProject->getAllModel(ControlBoardModel::Type), ControlBoardModel::Type,
                                   QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlSystem-normal.png")));
    }

    this->appendRow(DeviceTypeItem);
    this->appendRow(elecElements);
    this->appendRow(blockTemplate);
    this->appendRow(blockInstacne);
    this->appendRow(complexTemplate);
    this->appendRow(complexInstance);
    this->appendRow(controlsystem);
}
QList<QStandardItem *>
CustomTreeModel::getControlSysItems(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &modellist,
                                    const int &modeltype, const QIcon &icon)
{
    QList<QStandardItem *> itemlist;
    QList<QString> additemList;
    QString itemtext = "";
    QString prototypeName = "";
    QIcon mIcon = icon;
    QList<QStandardItem *> Blockitems;
    for (PModel pmod : modellist) {
        if (pmod == nullptr || modeltype != pmod->getModelType()) {
            continue;
        }
        itemtext = "";
        prototypeName = "";
        Blockitems.clear();
        if (ControlBlock::Type == modeltype && !pmod->isInstance()) {
            itemtext = pmod->getPrototypeName();
        } else if ((ControlBlock::Type == modeltype && pmod->isInstance())) {
            itemtext = pmod->getName();
        } else if (CombineBoardModel::Type == modeltype || ControlBoardModel::Type == modeltype) {
            itemtext = pmod->getName();
            Blockitems =
                    getControlSysItems(pmod->getChildModels().values(), ControlBlock::Type,
                                       QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlBlock-normal.png"))
                    + getControlSysItems(
                            pmod->getChildModels().values(), CombineBoardModel::Type,
                            QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlSystem-normal.png"));
        }

        if (!additemList.contains(itemtext)) {
            additemList.append(itemtext);
        } else {
            continue;
        }

        if (itemtext.isEmpty()) {
            continue;
        }
        QStandardItem *item = new QStandardItem(mIcon, itemtext);
        if (ControlBlock::Type == modeltype && !pmod->isInstance()) {
            item->setData(CustomTreeModel::NodeType_NetworkControlBlock, CustomTreeModel::CustomRole_NodeType);
            item->setData(pmod->getParentModelUUID(), CustomTreeModel::CustomRole_ParentUUID);
        } else if (CombineBoardModel::Type == modeltype || ControlBoardModel::Type == modeltype) {
            item->setData(CustomTreeModel::NodeType_Model, CustomTreeModel::CustomRole_NodeType);
            item->appendRows(Blockitems);
        } else {
            item->setData(pmod->isInstance() ? CustomTreeModel::NodeType_ModelInstance
                                             : CustomTreeModel::NodeType_Model,
                          CustomTreeModel::CustomRole_NodeType);
        }

        itemlist.append(item);
    }
    // sort
    qSort(itemlist.begin(), itemlist.end(), [&](const QStandardItem *lhs, const QStandardItem *rhs) -> bool {
        return NPS::compareString(lhs->text(), rhs->text());
    });
    return itemlist;
}
void CustomTreeModel::setInitTreeState(const QVariantMap &treeStsMap)
{
    m_treeStateMap.clear();
    for (QString keystr : treeStsMap.keys()) {
        m_treeStateMap.insert(keystr, TreeItemState(keystr, treeStsMap[keystr].toBool()));
    }
}

QVariantMap CustomTreeModel::getTreeCurState(TreeView *pTreeView)
{
    if (pTreeView == nullptr) {
        return QVariantMap();
    }
    saveTreeState(pTreeView);
    QVariantMap stsMap;
    for (QString keystr : m_treeStateMap.keys()) {
        stsMap.insert(keystr, m_treeStateMap[keystr].bExpand);
    }
    return stsMap;
}

CustomTreeModel::ProjectSortType CustomTreeModel::getSortType()
{
    return m_projSortType;
}

void CustomTreeModel::setInitSortType(const CustomTreeModel::ProjectSortType &initSorttype)
{
    m_projSortType = initSorttype;
}

void CustomTreeModel::saveTreeState(TreeView *pTreeView)
{
    m_treeStateMap.clear();

    for (int i = 0; i < rowCount(); i++) {
        if (item(i) == nullptr || !item(i)->index().isValid()) {
            continue;
        }

        if (pTreeView->isExpanded(item(i)->index())) {
            m_treeStateMap.insert(item(i)->text(), TreeItemState(item(i)->text(), true));
            saveTreeState(pTreeView, item(i), item(i)->text());
        }
    }

    m_curSelectedItemText = getIndexNames(pTreeView->currentIndex()).join("|");
    if (pTreeView->verticalScrollBar() != nullptr) {
        m_scrollIndex = pTreeView->verticalScrollBar()->value();
    } else {
        m_scrollIndex = 0;
    }
}

void CustomTreeModel::loadTreeState(TreeView *pTreeView)
{
    if (pTreeView == nullptr) {
        return;
    }

    pTreeView->setCurrentIndex(item(0)->index());
    for (int i = 0; i < rowCount(); i++) {
        if (item(i) == nullptr) {
            continue;
        }

        if (m_treeStateMap.contains(item(i)->text())) {
            pTreeView->expand(item(i)->index());
        }
        if (m_curSelectedItemText == item(i)->text()) {
            pTreeView->setCurrentIndex(item(i)->index());
        }
        loadTreeState(pTreeView, item(i), item(i)->text());
    }

    if (pTreeView->verticalScrollBar() != nullptr) {
        if (pTreeView->verticalScrollBar()->maximum() >= m_scrollIndex) {
            pTreeView->verticalScrollBar()->setValue(m_scrollIndex);
        } else {
            pTreeView->verticalScrollBar()->setValue(pTreeView->verticalScrollBar()->maximum());
        }
    }
}

QModelIndex CustomTreeModel::getTreeIndex(const QStringList &indexNames)
{
    if (rowCount() <= 0) {
        return CustomTreeModel::index(-1, -1);
    }

    if (indexNames.size() <= 0) {
        return CustomTreeModel::index(0, 0);
    }
    QStandardItem *targetItem = nullptr;
    QStringList tmpIndexNames = indexNames;
    for (int rowno = 0; rowno < rowCount(); ++rowno) {
        QStandardItem *item = CustomTreeModel::item(rowno);
        if (item == nullptr) {
            continue;
        }
        if (item->text() == indexNames[0]) {
            if (tmpIndexNames.size() == 1) {
                targetItem = item;
            } else {
                tmpIndexNames.pop_front();
                targetItem = getTreeItem(item, tmpIndexNames);
            }
            break;
        }
    }
    if (targetItem != nullptr) {
        return CustomTreeModel::indexFromItem(targetItem);
    }
    return CustomTreeModel::index(0, 0);
}

QStandardItem *CustomTreeModel::getTreeItem(QStandardItem *parentItem, const QStringList &indexNames)
{
    if (parentItem == nullptr) {
        return nullptr;
    }
    if (indexNames.size() <= 0) {
        return parentItem;
    }

    for (int row = 0; row < parentItem->rowCount(); ++row) {
        QStandardItem *item = parentItem->child(row);
        if (item == nullptr) {
            continue;
        }
        if (item->text() == indexNames[0]) {
            if (indexNames.size() == 1) {
                return item;
            } else {
                QStringList tmpIndexNames = indexNames;
                tmpIndexNames.pop_front();
                return getTreeItem(item, tmpIndexNames);
            }
        }
    }
    return parentItem;
}

QStringList CustomTreeModel::getIndexNames(const QModelIndex &index)
{
    QStringList names;
    if (!index.isValid()) {
        return names;
    }
    QStandardItem *item = QStandardItemModel::itemFromIndex(index);
    if (item == nullptr) {
        return names;
    }
    names.append(item->text());
    QStandardItem *tmpitem = item->parent();
    while (tmpitem != nullptr) {
        names.prepend(tmpitem->text());
        tmpitem = tmpitem->parent();
    }
    return names;
}

QStringList CustomTreeModel::getTreeIndexSiblingIndexNames(const QModelIndex &index)
{
    if (!index.isValid()) {
        return QStringList();
    }
    QStringList indexNames = getIndexNames(index);
    indexNames.pop_back();
    QStandardItem *item = QStandardItemModel::itemFromIndex(index);
    if (item == nullptr) {
        return indexNames;
    }
    QStandardItem *parentItem = item->parent();
    if (parentItem == nullptr) {
        return indexNames;
    }
    if (parentItem->rowCount() <= 1 || item->row() >= parentItem->rowCount() || item->row() < 0) {
        return indexNames;
    }
    int targetrow = item->row();
    QStandardItem *targetItem = nullptr;
    if (targetrow == 0) {
        targetItem = parentItem->child(targetrow + 1);
    } else {
        targetItem = parentItem->child(targetrow - 1);
    }
    if (targetItem != nullptr) {
        indexNames.append(targetItem->text());
    }
    return indexNames;
}

QSharedPointer<KLProject> CustomTreeModel::getProjectByIndex(const QModelIndex &index)
{
    if (!index.isValid() || nullptr == m_pProjDataManager) {
        return QSharedPointer<KLProject>(nullptr);
    }
    if (AssistantDefine::WidgetObject_NetworkTree == m_widgetObject) {
        return m_pProjDataManager->getCurrentProject();
    }

    if ((CustomTreeModel::NodeType_DeviceTypeClass == index.data(CustomTreeModel::CustomRole_NodeType).toInt()
         && index.parent().isValid()
         && CustomTreeModel::NodeType_GlobalDeviceType
                 == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt())
        || CustomTreeModel::NodeType_GlobalDeviceType == index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return m_pProjDataManager->getSystemProject();
    }

    QString Workspacename = "";
    QString projname = "";
    getWSProjByIndex(index, Workspacename, projname);
    if (!Workspacename.isEmpty() && !projname.isEmpty()) {
        return m_pProjDataManager->getProject(Workspacename, projname);
    }

    return QSharedPointer<KLProject>(nullptr);
}

QString CustomTreeModel::getWorkSpaceNameByIndex(const QModelIndex &index)
{
    QString Workspacename = "";
    QString projname = "";
    getWSProjByIndex(index, Workspacename, projname);
    return Workspacename;
}

void CustomTreeModel::getWSProjByIndex(const QModelIndex &index, QString &wsname, QString &projname)
{
    wsname = "";
    projname = "";
    if (!index.isValid()) {
        return;
    }
    if (AssistantDefine::WidgetObject_NetworkTree == m_widgetObject) {
        PKLProject curProject = getProjectByIndex(index);
        if (curProject != nullptr) {
            projname = curProject->getProjectName();
            QStringList pathlist = QFileInfo(curProject->getProjectPath()).path().split("/");
            if (pathlist.size() >= 2) {
                wsname = pathlist[pathlist.size() - 2];
            }
        }
        return;
    }
    QModelIndex tmpindex = index;
    while (tmpindex.isValid()) {
        if (CustomTreeModel::NodeType_Project == tmpindex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            projname = tmpindex.data().toString();
            if (tmpindex.parent().isValid()) {
                wsname = tmpindex.parent().data().toString();
            }
            break;
        } else if (CustomTreeModel::NodeType_Workspace == tmpindex.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            wsname = tmpindex.data().toString();
            projname = "";
        }

        tmpindex = tmpindex.parent();
    }
}

Kcc::BlockDefinition::SimulationResultInfo CustomTreeModel::getSimuResultInfo(const QModelIndex &index)
{
    if (!index.isValid()
        || CustomTreeModel::NodeType_SimuResultSubItem != index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return Kcc::BlockDefinition::SimulationResultInfo();
    }
    PKLProject project = getProjectByIndex(index);
    if (project == nullptr) {
        return Kcc::BlockDefinition::SimulationResultInfo();
    }
    QList<SimulationResultInfo> simuResultList = project->getAllSimulationDataInfo();
    for (SimulationResultInfo item : simuResultList) {
        if (AssistantDefine::CURRENTSIMULATION_NAME == index.data().toString()
            && item.name == AssistantDefine::CURRENTSIMULATION_REALNAME) {
            return item;
        } else if (item.name == index.data().toString()) {
            return item;
        }
    }
    return Kcc::BlockDefinition::SimulationResultInfo();
}

QList<Kcc::BlockDefinition::SimulationResultInfo> CustomTreeModel::getSimuResultInfoList(const QModelIndex &index)
{
    if (!index.isValid()
        || CustomTreeModel::NodeType_SimuResult != index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return QList<Kcc::BlockDefinition::SimulationResultInfo>();
    }
    PKLProject project = getProjectByIndex(index);
    if (project == nullptr) {
        return QList<Kcc::BlockDefinition::SimulationResultInfo>();
    }
    QList<SimulationResultInfo> list = project->getAllSimulationDataInfo();
    return list;
}

bool CustomTreeModel::isOriginResultInfo(const QModelIndex &index)
{
    SimulationResultInfo resultInfo = getSimuResultInfo(index);
    PKLProject project = getProjectByIndex(index);
    if (project == nullptr || resultInfo.name.isEmpty()) {
        return false;
    }
    return !project->isFromRecord(resultInfo.name);
}

QSharedPointer<Kcc::BlockDefinition::Model> CustomTreeModel::getModel(const QModelIndex &index)
{
    if (!index.isValid() || !index.parent().isValid()
        || (index.data(CustomTreeModel::CustomRole_NodeType).toInt() != CustomTreeModel::NodeType_Model
            && index.data(CustomTreeModel::CustomRole_NodeType).toInt() != CustomTreeModel::NodeType_ModelInstance
            && index.data(CustomTreeModel::CustomRole_NodeType).toInt() != CustomTreeModel::NodeType_CodeBlockClass)) {
        return PModel(nullptr);
    }
    // 工具箱的模块
    if (index.parent().parent().isValid()
        && CustomTreeModel::NodeType_GlobalCtrBlock
                == index.parent().parent().data(CustomTreeModel::CustomRole_NodeType).toInt()
        && ProjectViewServers::getInstance().m_pModelManagerServer != nullptr) {
        QString toolkitName;
        QString prototypeName = index.data(CustomTreeModel::CustomRole_PrototypeName).toString();
        if (ProjectViewServers::getInstance().m_pModelManagerServer->ContainToolkitModel(prototypeName, toolkitName)) {
            return ProjectViewServers::getInstance().m_pModelManagerServer->GetToolkitModel(prototypeName, toolkitName);
        }
    }
    // 项目的模块
    PKLProject pProject = getProjectByIndex(index);
    if (pProject == nullptr) {
        return PModel(nullptr);
    }
    CustomTreeModel::NodeType itemnodetype =
            CustomTreeModel::NodeType(index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt());
    if (CustomTreeModel::NodeType_CodeBlockClass == index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        return pProject->getModel(index.data().toString());
    } else if (CustomTreeModel::NodeType_CombinedModel == itemnodetype
               || CustomTreeModel::NodeType_ComplexModel == itemnodetype
               || CustomTreeModel::NodeType_Network == itemnodetype || NodeType_NetworkComplexTemplate == itemnodetype
               || NodeType_NetworkComplexInstance == itemnodetype) {
        if (CustomTreeModel::NodeType_Model == index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            return pProject->getModel(index.data().toString());
        } else if (CustomTreeModel::NodeType_ModelInstance
                   == index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            QList<PModel> instanceModels = getAllInstance(pProject, ControlBlock::Type)
                    + getAllInstance(pProject, CombineBoardModel::Type)
                    + getAllInstance(pProject, ComplexBoardModel::Type);
            for (PModel model : instanceModels) {
                if (model != nullptr && index.data().toString() == model->getName()) {
                    return model;
                }
            }
        }
    }

    return PModel(nullptr);
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>> CustomTreeModel::getModels(const QModelIndex &index)
{
    QList<PModel> modellist;
    if (!index.isValid()) {
        return modellist;
    }

    CustomTreeModel::NodeType itemnodetype =
            CustomTreeModel::NodeType(index.data(CustomTreeModel::CustomRole_NodeType).toInt());
    PKLProject pProject = getProjectByIndex(index);
    if (CustomTreeModel::NodeType_CombinedModel == itemnodetype) {
        if (pProject != nullptr) {
            return pProject->getAllModel(CombineBoardModel::Type);
        }
    } else if (CustomTreeModel::NodeType_ComplexModel == itemnodetype) {
        if (pProject != nullptr) {
            return pProject->getAllModel(ComplexBoardModel::Type);
        }
    } else if (CustomTreeModel::NodeType_Network == itemnodetype) {
        if (pProject != nullptr) {
            return pProject->getAllModel(ElecBoardModel::Type) + pProject->getAllModel(ElecCombineBoardModel::Type)
                    + pProject->getAllModel(ControlBoardModel::Type) + getAllInstance(pProject, ControlBlock::Type)
                    + getAllInstance(pProject, CombineBoardModel::Type)
                    + getAllInstance(pProject, ComplexBoardModel::Type);
        }
    } else if (CustomTreeModel::NodeType_DeviceTypeClass == itemnodetype) {
        CustomTreeModel::DeviceTypeClassType devtype = getDeviceType(index);
        if (CustomTreeModel::DeviceTypeClassType_Other == devtype) {
            return modellist;
        } else if (CustomTreeModel::DeviceTypeClassType_Project == devtype) {
            PKLProject pProject = getProjectByIndex(index);
            if (pProject == nullptr) {
                return modellist;
            }
            QString deviceprototypename = index.data(CustomTreeModel::CustomRole_PrototypeName).toString();
            if (deviceprototypename.isEmpty()) {
                return modellist;
            }

            QList<PModel> models = pProject->getAllModel(DeviceModel::Type);
            for (PModel pmodel : models) {
                if (pmodel == nullptr) {
                    continue;
                }
                PDeviceModel pdevModel = pmodel.dynamicCast<DeviceModel>();
                if (pdevModel == nullptr) {
                    continue;
                }
                if (pdevModel->getDeviceTypePrototypeName() == deviceprototypename) {
                    modellist.append(pdevModel);
                }
            }
        } else if (CustomTreeModel::DeviceTypeClassType_System == devtype) {
            if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
                return modellist;
            }
            QString deviceprototypename = index.data(CustomTreeModel::CustomRole_PrototypeName).toString();
            QList<PDeviceModel> sysdevices =
                    ProjectViewServers::getInstance().m_projectManagerServer->GetSystemDeviceModels();
            for (PDeviceModel pdevmodel : sysdevices) {
                if (pdevmodel == nullptr) {
                    continue;
                }
                if (pdevmodel->getDeviceTypePrototypeName() == deviceprototypename) {
                    modellist.append(pdevmodel);
                }
            }
        }
    } else if (CustomTreeModel::NodeType_CodeBlockClass == itemnodetype) {
        if (pProject == nullptr) {
            return modellist;
        }
        QList<PModel> allBoards = pProject->getAllBoardModel();
        for (PModel boardModel : allBoards) {
            if (boardModel == nullptr) {
                continue;
            }
            for (PModel block : boardModel->getChildModels()) {
                if (block != nullptr && index.data().toString() == block->getPrototypeName()) {
                    modellist.append(block);
                }
            }
        }
    } else if (CustomTreeModel::NodeType_NetworkControlBlock == itemnodetype) {
        if (pProject == nullptr) {
            return modellist;
        }
        auto getMatchModels = [&modellist](auto func, PModel boardModel, QString blockname, QString parentuuid) {
            if (boardModel == nullptr) {
                return;
            }
            for (PModel block : boardModel->getChildModels()) {
                if (block != nullptr && blockname == block->getPrototypeName()
                    && parentuuid == boardModel->getObjectUuid()) {
                    modellist.append(block);
                }
                if (block->getModelType() == CombineBoardModel::Type) {
                    func(func, block, blockname, parentuuid);
                }
            }
        };
        // 从控制系统开始递归寻找同一画板下此类型的所有模块
        QList<PModel> allBoards = pProject->getAllModel(ControlBoardModel::Type);
        for (PModel boardModel : allBoards) {
            getMatchModels(getMatchModels, boardModel, index.data().toString(),
                           index.data(CustomTreeModel::CustomRole_ParentUUID).toString());
        }

    } else if (CustomTreeModel::NodeType_ElecElementClass == itemnodetype) {
        if (!index.parent().isValid()) {
            return modellist;
        }
        if (CustomTreeModel::NodeType_NetworkElecElement
            == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            return getElecElements(getProjectByIndex(index),
                                   index.data(CustomTreeModel::CustomRole_PrototypeName).toString());
        } else {
            return getElecElements(getModel(index.parent()),
                                   index.data(CustomTreeModel::CustomRole_PrototypeName).toString());
        }
    } else if (CustomTreeModel::NodeType_NetworkBlockTemplate == itemnodetype) {
        if (pProject == nullptr) {
            return modellist;
        }
        return pProject->getAllModel(ControlBlock::Type) + pProject->getAllModel(CombineBoardModel::Type);
    } else if (CustomTreeModel::NodeType_NetworkBlockInstance == itemnodetype) {
        return getAllInstance(pProject, ControlBlock::Type) + getAllInstance(pProject, CombineBoardModel::Type);
    } else if (CustomTreeModel::NodeType_NetworkBlockInstanceClass == itemnodetype) {
        return getAllInstance(pProject, ControlBlock::Type, index.data().toString())
                + getAllInstance(pProject, CombineBoardModel::Type, index.data().toString());
    } else if (CustomTreeModel::NodeType_NetworkComplexTemplate == itemnodetype) {
        if (pProject == nullptr) {
            return modellist;
        }
        return pProject->getAllModel(ComplexBoardModel::Type);
    } else if (CustomTreeModel::NodeType_NetworkComplexInstance == itemnodetype) {
        return getAllInstance(pProject, ComplexBoardModel::Type);
    } else {
        PModel curmodel = getModel(index);
        if (curmodel != nullptr) {
            modellist.append(curmodel);
        }
    }

    return modellist;
}

bool CustomTreeModel::getProjectActiveSts(const QModelIndex &index)
{
    PKLProject pProject = getProjectByIndex(index);

    if (pProject == nullptr || m_pProjDataManager == nullptr) {
        return false;
    }
    PKLProject pCurProject = m_pProjDataManager->getCurrentProject();
    if (pCurProject == nullptr) {
        return false;
    }

    return pProject->getProjectPath() == pCurProject->getProjectPath();
}

void CustomTreeModel::setProjectSortType(const CustomTreeModel::ProjectSortType &sorttype, TreeView *ptreeView)
{
    if (m_projSortType != sorttype) {
        m_projSortType = sorttype;
        saveTreeState(ptreeView);
        updateTree(ptreeView);
        loadTreeState(ptreeView);
    }
}

void CustomTreeModel::saveTreeState(TreeView *pTreeView, QStandardItem *item_, const QString keyword)
{
    if (pTreeView == nullptr || item_ == nullptr) {
        return;
    }

    for (int i = 0; i < item_->rowCount(); i++) {
        QStandardItem *item = item_->child(i);
        if (item == nullptr) {
            continue;
        }

        if (pTreeView->isExpanded(item->index())) {
            m_treeStateMap.insert(keyword + "|" + item->text(), TreeItemState(keyword + "|" + item->text(), true));
            saveTreeState(pTreeView, item, keyword + "|" + item->text());
        }
    }
}

void CustomTreeModel::loadTreeState(TreeView *pTreeView, QStandardItem *item_, const QString keyword)
{
    if (pTreeView == nullptr || item_ == nullptr) {
        return;
    }

    for (int i = 0; i < item_->rowCount(); i++) {
        QStandardItem *item = item_->child(i);
        if (item == nullptr) {
            continue;
        }
        if (m_treeStateMap.contains(keyword + "|" + item->text())) {
            pTreeView->expand(item->index());
        }
        if (m_curSelectedItemText == (keyword + "|" + item->text())) {
            pTreeView->setCurrentIndex(item->index());
        }

        loadTreeState(pTreeView, item, keyword + "|" + item->text());
    }
}

void CustomTreeModel::CreateGlobalLibrary(QStandardItem *&itemRoot)
{
    if (itemRoot->hasChildren() && itemRoot->child(0)->text() == AssistantDefine::GLOBALLIB_NAME) {
        return;
    }
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr
        || ProjectViewServers::getInstance().m_pModelManagerServer == nullptr) {
        return;
    }

    // deviceTypeMap = pDataManagerServer->GetBuiltinDeviceTypes();

    QStandardItem *itemGlobalLibrary =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/database-normal.png"),
                              AssistantDefine::GLOBALLIB_NAME);
    itemGlobalLibrary->setData(CustomTreeModel::NodeType_GlobalLib, CustomTreeModel::CustomRole_NodeType);
    itemRoot->insertRow(0, itemGlobalLibrary);

    QStandardItem *deviceType =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/deviceType-normal.png"),
                              AssistantDefine::DEVICETYPE_NAME);
    deviceType->setData(CustomTreeModel::NodeType_GlobalDeviceType, CustomTreeModel::CustomRole_NodeType);
    itemGlobalLibrary->appendRow(deviceType);
    QList<PDeviceModel> devicelist = ProjectViewServers::getInstance().m_projectManagerServer->GetSystemDeviceModels();
    QList<PModel> devicemodellist;
    for (PDeviceModel pdevmodel : devicelist) {
        devicemodellist.append(pdevmodel);
    }
    deviceType->appendColumn(getModelItems(devicemodellist, DeviceModel::Type));
    QStandardItem *contorlModel =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/buildinBlock-normal.png"),
                              AssistantDefine::CONTROLMODEL);
    contorlModel->setData(CustomTreeModel::NodeType_GlobalCtrBlock, CustomTreeModel::CustomRole_NodeType);
    itemGlobalLibrary->appendRow(contorlModel);

    if (nullptr == ProjectViewServers::getInstance().m_pCtrComponentServer) {
        return;
    }

    auto result =
            ProjectViewServers::getInstance().m_pCtrComponentServer->GetComponentInfo(GetDataType::GetDataType_onlyCfg);
    // TODO
    //  QList<PModel> models =
    //  ProjectViewServers::getInstance().m_pModelManagerServer->GetToolkitModels(KL_TOOLKIT::CONTROL_TOOLKIT_NAME);
    for (QString str : result.listGroup) {
        QStandardItem *itemTemp = new QStandardItem(str);
        if (result.mapComponents[str].size() > 0) {
            contorlModel->appendRow(itemTemp);
        }

        for (QString var : result.mapComponents[str]) {
            QStandardItem *itemTemp_ =
                    new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/buildinBlock-normal.png"),
                                      QObject::tr(var.toUtf8()));
            itemTemp_->setData(var, CustomTreeModel::CustomRole_PrototypeName);
            itemTemp_->setData(CustomTreeModel::NodeType_Model, CustomTreeModel::CustomRole_NodeType);
            itemTemp->appendRow(itemTemp_);
        }
    }
}

void CustomTreeModel::createProjectSubTree(QSharedPointer<KLProject> pProject, QStandardItem *&projitem,
                                           bool projactive)
{
    if (pProject == nullptr || projitem == nullptr
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    // local lib
    QStandardItem *localLibItem = new QStandardItem(
            QIcon(":/PluginProjectManagerView/resource/TreeIcons/localLib-normal.png"), AssistantDefine::LOCALLIB_NAME);
    localLibItem->setData(CustomTreeModel::NodeType_LocalLib, CustomTreeModel::CustomRole_NodeType);
    createProjectLocalLib(pProject, localLibItem);
    // network
    QStandardItem *netModelItem = new QStandardItem(
            QIcon(":/PluginProjectManagerView/resource/TreeIcons/network-normal.png"), AssistantDefine::NETMODEL_NAME);
    netModelItem->setData(CustomTreeModel::NodeType_Network, CustomTreeModel::CustomRole_NodeType);
    QVariantMap boardactivemap;
    if (projactive && pProject->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
        boardactivemap = pProject->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
    }

    netModelItem->appendRows(
            getModelItems(pProject->getAllModel(ElecBoardModel::Type), ElecBoardModel::Type,
                          QIcon(":/PluginProjectManagerView/resource/TreeIcons/electricalSystem-normal.png"),
                          projactive ? boardactivemap : QVariantMap())
            + getModelItems(pProject->getAllModel(ElecCombineBoardModel::Type), ElecCombineBoardModel::Type,
                            QIcon(":/PluginProjectManagerView/resource/TreeIcons/elecCombineBoard-normal.png"))
            + getModelItems(pProject->getAllModel(ControlBoardModel::Type), ControlBoardModel::Type,
                            QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlSystem-normal.png"))
            + getModelItems(getAllInstance(pProject, ComplexBoardModel::Type), ComplexBoardModel::Type,
                            QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-instance.png"))
            + getModelItems(getAllInstance(pProject, CombineBoardModel::Type), CombineBoardModel::Type,
                            QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlBlock-instance.png"))
            + getModelItems(getAllInstance(pProject, ControlBlock::Type), ControlBlock::Type,
                            QIcon(":/PluginProjectManagerView/resource/TreeIcons/codeBlock-instance.png")));
    // function delete
    // QStandardItem *functionModelItem =
    //         new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/functionBlock-normal.png"),
    //                           AssistantDefine::FUNCTIONMODEL_NAME);
    // functionModelItem->setData(CustomTreeModel::NodeType_FunctionBlock, CustomTreeModel::CustomRole_NodeType);
    // result
    QStandardItem *resultItem = new QStandardItem(
            QIcon(":/PluginProjectManagerView/resource/TreeIcons/simuresult-normal.png"), AssistantDefine::RESULT_NAME);
    resultItem->setData(CustomTreeModel::NodeType_SimuResult, CustomTreeModel::CustomRole_NodeType);
    QList<SimulationResultInfo> simuList = pProject->getAllSimulationDataInfo();
    qSort(simuList.begin(), simuList.end(),
          [&simuList](const SimulationResultInfo &lhs, const SimulationResultInfo &rhs) -> bool {
              return NPS::compareString(lhs.name, rhs.name);
          });
    // add resule sub item
    for (SimulationResultInfo simuItem : simuList) {
        QStandardItem *simuSubItem = new QStandardItem(
                QIcon(":/PluginProjectManagerView/resource/TreeIcons/simuresult-normal.png"),
                simuItem.name == AssistantDefine::CURRENTSIMULATION_REALNAME ? AssistantDefine::CURRENTSIMULATION_NAME
                                                                             : simuItem.name);
        simuSubItem->setData(CustomTreeModel::NodeType_SimuResultSubItem, CustomTreeModel::CustomRole_NodeType);
        resultItem->appendRow(simuSubItem);
    }
    projitem->appendRow(localLibItem);
    projitem->appendRow(netModelItem);
    projitem->appendRow(resultItem);
}

void CustomTreeModel::createProjectLocalLib(QSharedPointer<KLProject> pProject, QStandardItem *&localLibItem)
{
    if (pProject == nullptr || localLibItem == nullptr
        || ProjectViewServers::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    // device type item
    QStandardItem *subDeviceTypeItem =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/deviceType-normal.png"),
                              AssistantDefine::DEVICETYPE_NAME);
    subDeviceTypeItem->setData(CustomTreeModel::NodeType_DeviceType, CustomTreeModel::CustomRole_NodeType);
    subDeviceTypeItem->appendRows(getModelItems(pProject->getAllModel(DeviceModel::Type), DeviceModel::Type));

    // code item
    QStandardItem *subCodeModelItem =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/codeBlock-normal.png"),
                              AssistantDefine::CODEMODEL_NAME);
    subCodeModelItem->setData(CustomTreeModel::NodeType_CodeBlock, CustomTreeModel::CustomRole_NodeType);
    subCodeModelItem->appendRows(
            getModelItems(pProject->getAllModel(ControlBlock::Type), ControlBlock::Type,
                          QIcon(":/PluginProjectManagerView/resource/TreeIcons/codeBlock-normal.png")));

    // combined item
    QStandardItem *subCombinedModelItem =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlBlock-normal.png"),
                              AssistantDefine::COMBINEDMODEL_NAME);

    subCombinedModelItem->setData(CustomTreeModel::NodeType_CombinedModel, CustomTreeModel::CustomRole_NodeType);
    subCombinedModelItem->appendRows(
            getModelItems(pProject->getAllModel(CombineBoardModel::Type), CombineBoardModel::Type,
                          QIcon(":/PluginProjectManagerView/resource/TreeIcons/controlBlock-normal.png")));

    // complex item
    QStandardItem *subComplexModelItem =
            new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-normal.png"),
                              AssistantDefine::COMPLEXMODEL_NAME);
    subComplexModelItem->setData(CustomTreeModel::NodeType_ComplexModel, CustomTreeModel::CustomRole_NodeType);
    subComplexModelItem->appendRows(
            getModelItems(pProject->getAllModel(ComplexBoardModel::Type), ComplexBoardModel::Type,
                          QIcon(":/PluginProjectManagerView/resource/TreeIcons/complexModel-normal.png")));

    localLibItem->appendRow(subDeviceTypeItem);
    localLibItem->appendRow(subCodeModelItem);
    localLibItem->appendRow(subCombinedModelItem);
    localLibItem->appendRow(subComplexModelItem);
}

QList<QStandardItem *>
CustomTreeModel::getModelItems(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &modellist,
                               const int &modeltype, const QIcon &icon, const QVariantMap &drawboardactivemap,
                               bool isInstanceClass)
{
    QList<QStandardItem *> itemlist;
    QList<QString> additemList;
    QString itemtext = "";
    QString prototypeName = "";
    QIcon mIcon = icon;
    QList<QStandardItem *> eleboardBlockitems;
    for (PModel pmod : modellist) {
        if (pmod == nullptr) {
            continue;
        }
        itemtext = "";
        prototypeName = "";
        eleboardBlockitems.clear();
        if (DeviceModel::Type == modeltype) {
            prototypeName =
                    pmod->getValueByRoleData(RoleDataDefinition::DeviceTypeGroup, RoleDataDefinition::VariableTypeRole,
                                             RoleDataDefinition::DeviceTypePrototypeName)
                            .toString();
            if (prototypeName.isEmpty()) {
                continue;
            }
            QString toolkitName;
            PModel elecBlock = ProjectViewServers::getInstance().m_pModelManagerServer->GetToolkitModel(prototypeName,
                                                                                                        toolkitName);
            if (elecBlock == nullptr) {
                continue;
            }
            if (!additemList.contains(prototypeName)) {
                mIcon = QIcon(":/PluginProjectManagerView/resource/ElectricalIcons/" + prototypeName + ".png");
                itemtext = elecBlock->getPrototypeName_CHS();
                additemList.append(prototypeName);
            } else {
                continue;
            }
        } else if (ElectricalBlock::Type == modeltype) {
            prototypeName = ((pmod->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) ? NPS::PROTOTYPENAME_BUSBAR
                                                                                        : pmod->getPrototypeName());
            if (!additemList.contains(prototypeName)) {
                mIcon = QIcon(":/PluginProjectManagerView/resource/ElectricalIcons/" + prototypeName + ".png");
                itemtext = pmod->getPrototypeName_CHS();
                additemList.append(prototypeName);
            } else {
                continue;
            }
        } else {
            if (ControlBlock::Type == modeltype && !pmod->isInstance()) {
                itemtext = pmod->getPrototypeName();
            } else if (CombineBoardModel::Type == modeltype || ComplexBoardModel::Type == modeltype
                       || ControlBoardModel::Type == modeltype
                       || (ControlBlock::Type == modeltype && pmod->isInstance())) {
                itemtext = pmod->getName();
            } else if (ElecBoardModel::Type == modeltype || ElecCombineBoardModel::Type == modeltype) {
                itemtext = pmod->getName();
                eleboardBlockitems = getModelItems(pmod->getChildModels().values(), ElectricalBlock::Type);
            }
            if (!additemList.contains(itemtext)) {
                additemList.append(itemtext);
            } else {
                continue;
            }
        }

        if (itemtext.isEmpty()) {
            continue;
        }
        QStandardItem *item = new QStandardItem(mIcon, itemtext);
        if (isInstanceClass) {
            item->setData(CustomTreeModel::NodeType_NetworkBlockInstanceClass, CustomTreeModel::CustomRole_NodeType);
        } else if (DeviceModel::Type == modeltype) {
            item->setData(CustomTreeModel::NodeType_DeviceTypeClass, CustomTreeModel::CustomRole_NodeType);
            item->setData(prototypeName, CustomTreeModel::CustomRole_PrototypeName);
        } else if (ControlBlock::Type == modeltype && !pmod->isInstance()) {
            item->setData(CustomTreeModel::NodeType_CodeBlockClass, CustomTreeModel::CustomRole_NodeType);
        } else if (ElectricalBlock::Type == modeltype) {
            item->setData(prototypeName, CustomTreeModel::CustomRole_PrototypeName);
            item->setData(CustomTreeModel::NodeType_ElecElementClass, CustomTreeModel::CustomRole_NodeType);
        } else if (ElecBoardModel::Type == modeltype) {
            item->setData(CustomTreeModel::NodeType_Model, CustomTreeModel::CustomRole_NodeType);
            QFont tmpfont = item->font();
            if (drawboardactivemap.value(pmod->getUUID(), false).toBool()) {
                tmpfont.setBold(true);
                item->setIcon(QIcon(":/PluginProjectManagerView/resource/TreeIcons/"
                                    "electricalSystem-active.png"));
                item->setData(true, CustomTreeModel::CustomRole_ActiveStatus);
                item->setFont(tmpfont);
            }
            item->appendRows(eleboardBlockitems);
        } else if (ElecCombineBoardModel::Type == modeltype) {
            item->setData(CustomTreeModel::NodeType_Model, CustomTreeModel::CustomRole_NodeType);
            item->appendRows(eleboardBlockitems);
        } else {
            item->setData(pmod->isInstance() ? CustomTreeModel::NodeType_ModelInstance
                                             : CustomTreeModel::NodeType_Model,
                          CustomTreeModel::CustomRole_NodeType);
        }

        itemlist.append(item);
    }
    // sort
    qSort(itemlist.begin(), itemlist.end(), [&](const QStandardItem *lhs, const QStandardItem *rhs) -> bool {
        return NPS::compareString(lhs->text(), rhs->text());
    });
    return itemlist;
}

CustomTreeModel::DeviceTypeClassType CustomTreeModel::getDeviceType(const QModelIndex &index)
{
    if (!index.isValid() || !index.parent().isValid()) {
        return CustomTreeModel::DeviceTypeClassType_Other;
    }
    if (CustomTreeModel::NodeType_DeviceTypeClass == index.data(CustomTreeModel::CustomRole_NodeType).toInt()) {
        if (CustomTreeModel::NodeType_DeviceType == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()
            || CustomTreeModel::NodeType_NetworkDeviceType
                    == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            return CustomTreeModel::DeviceTypeClassType_Project;
        } else if (CustomTreeModel::NodeType_GlobalDeviceType
                   == index.parent().data(CustomTreeModel::CustomRole_NodeType).toInt()) {
            return CustomTreeModel::DeviceTypeClassType_System;
        }
    }
    return CustomTreeModel::DeviceTypeClassType_Other;
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>>
CustomTreeModel::getAllInstance(QSharedPointer<KLProject> pProject, const int &modelType, const QString &prototypeName)
{
    QList<PModel> instanceModels;
    if (pProject == nullptr) {
        return instanceModels;
    }
    QList<PModel> ProtoModels = pProject->getAllModel(modelType);
    for (PModel model : ProtoModels) {
        if (model == nullptr || model->getInstanceList().size() <= 0
            || (!prototypeName.isEmpty() && model->getName() != prototypeName)) {
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

QList<QSharedPointer<Kcc::BlockDefinition::Model>> CustomTreeModel::getElecElements(QSharedPointer<KLProject> project,
                                                                                    const QString &prototype)
{
    if (project == nullptr) {
        return QList<PModel>();
    }

    QList<PModel> allElecBlocks;
    for (PModel pmod : project->getAllModel(ElecBoardModel::Type)) {
        if (pmod == nullptr) {
            continue;
        }
        allElecBlocks.append(getElecElements(pmod, prototype));
    }
    return allElecBlocks;
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>>
CustomTreeModel::getElecElements(QSharedPointer<Kcc::BlockDefinition::Model> elecBoardModel, const QString &prototype)
{
    if (elecBoardModel == nullptr) {
        return QList<PModel>();
    }
    if (prototype.isEmpty()) {
        return elecBoardModel->getChildModels().values();
    }

    QList<PModel> boardElecBlocks;
    for (PModel cmodel : elecBoardModel->getChildModels().values()) {
        if (cmodel == nullptr) {
            continue;
        }
        if (prototype == NPS::PROTOTYPENAME_BUSBAR) {
            if (NPS::PROTOTYPENAME_DOTBUSBAR == cmodel->getPrototypeName()
                || NPS::PROTOTYPENAME_BUSBAR == cmodel->getPrototypeName()) {
                boardElecBlocks.append(cmodel);
            }
        } else if (cmodel->getPrototypeName() == prototype) {
            boardElecBlocks.append(cmodel);
        }
    }
    return boardElecBlocks;
}

const QStringList CustomTreeModel::sortProjectList(const QMap<QString, QSharedPointer<KLProject>> &projectMap)
{
    QStringList projectNameList;
    if (CustomTreeModel::ProjectSortType_Time == m_projSortType) {
        QMap<QString, QString> nameTimeMap;
        for (PKLProject project : projectMap.values()) {
            if (project == nullptr) {
                continue;
            }
            projectNameList.append(project->getProjectName());
            nameTimeMap.insert(project->getProjectName(), project->getProjectProperty(KL_PRO::CREATE_TIME));
        }
        qSort(projectNameList.begin(), projectNameList.end(),
              [&nameTimeMap](const QString &plhs, const QString &prhs) -> bool {
                  return QDateTime::fromString(nameTimeMap[plhs], KL_PRO::DATE_FORMAT)
                          < QDateTime::fromString(nameTimeMap[prhs], KL_PRO::DATE_FORMAT);
              });
    } else {
        projectNameList = projectMap.keys();
        qSort(projectNameList.begin(), projectNameList.end(),
              [&projectNameList](const QString &plhs, const QString &prhs) -> bool {
                  return NPS::compareString(plhs, prhs);
              });
    }
    return projectNameList;
}

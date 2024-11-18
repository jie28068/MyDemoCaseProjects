#include "ComponentListModel.h"
#include "ComplexBoardModel.h"
#include "ContextMenu.h"
#include "ControlBoardModel.h"
#include "ElecBoardModel.h"
#include <QFile>

#define DisableColor "#7d7d7d"

#define LineColor "#003756"
#define LineLineColor "#006ba5"
#define TextColor "#0079C2"

QSet<QString> BaseListModel::ms_existBlockNames;
using namespace Global;

BaseListModel::BaseListModel(QObject *parent) : QAbstractListModel(parent) { }

Qt::ItemFlags BaseListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (index.data(DragEnabledRole).toBool()) {
        return flags | Qt::ItemIsDragEnabled;
    }
    return flags;
}

Qt::DropActions BaseListModel::supportedDropActions() const
{
    // 由于在view中重新实现了startdrag，因此model设置为IgnoreAction
    return Qt::IgnoreAction;
}

QStringList BaseListModel::mimeTypes() const
{
    // 使用自定义的数据，无需从基类中获取数据
    // return QAbstractListModel::mimeTypes() << DRAG_MIME_DATA_TYPE_NAME << DRAG_MIME_DATA_CUSTOM_NAME;
    return QStringList() << DRAG_MIME_DATA_TYPE_NAME << DRAG_MIME_DATA_CUSTOM_NAME;
}

QMimeData *BaseListModel::mimeData(const QModelIndexList &indexes) const
{
    // 使用自定义的数据，无需从基类中获取数据
    // QMimeData *mimeData = QAbstractListModel::mimeData(indexes);
    QMimeData *mimeData = new QMimeData;

    // 目前仅支持单个选中拖拽，不支持多个拖拽
    Q_ASSERT(indexes.size() == 1);
    QModelIndex index = indexes[0];

    // 与画板约定的数据字段, 仅一个name
    QString strName = index.data(Global::NameRole).toString();
    QByteArray byData;
    QDataStream dataTypeName(&byData, QIODevice::WriteOnly);
    if (Device == index.data(CategoryRole).toInt()) {
        // External模块只放电气构造画板中
        dataTypeName << strName
                     << ((strName == NPS::PROTOTYPENAME_EXTERNAL || strName == NPS::PROTOTYPENAME_SINGAL)
                                 ? ElecCombineBoardModel::Type
                                 : ElecBoardModel::Type);
    } else if (Controller == index.data(CategoryRole).toInt()) {
        // fixme Slot模块只能放在复合画板中的临时解决方案
        // In/Out只能放构造控制画板
        dataTypeName << strName
                     << (strName == PrototypeName_Slot ? ComplexBoardModel::Type
                                                       : ((strName == PrototypeName_In || strName == PrototypeName_Out)
                                                                  ? CombineBoardModel::Type
                                                                  : ControlBoardModel::Type));
    } else {
        dataTypeName << strName << -1;
    }
    mimeData->setData(DRAG_MIME_DATA_TYPE_NAME, byData);

    // 内部使用的自定义数据
    QString strGroupName = index.data(Global::GroupNameRole).toString();
    QByteArray byCustomData;
    QDataStream dataCustomData(&byCustomData, QIODevice::WriteOnly);
    dataCustomData << strName << strGroupName << index.row();
    mimeData->setData(DRAG_MIME_DATA_CUSTOM_NAME, byCustomData);

    return mimeData;
}

QString BaseListModel::dropMimeDataCustom(const QMimeData *pMimeData, const QMap<int, QVariant> &mapGroupRoleTemplate)
{
    QString strOldGroupName;
    QString strName;
    int nRow = -1;
    QByteArray byCustomData = pMimeData->data(DRAG_MIME_DATA_CUSTOM_NAME);
    QDataStream dataStream(&byCustomData, QIODevice::ReadOnly);
    dataStream >> strName >> strOldGroupName >> nRow;

    QModelIndex i = index(nRow, 0);
    if (!i.isValid()) {
        return QString();
    }

    // 目前仅控制模块支持拖动更换分组
    if (data(i, CategoryRole).toInt() != Controller) {
        return QString();
    }

    QMapIterator<int, QVariant> iter(mapGroupRoleTemplate);
    while (iter.hasNext()) {
        iter.next();
        if (iter.key() == CanDeleteRole) {
            // CanDelete在变更分组时不修改
            continue;
        }
        setData(i, iter.value(), iter.key());
    }

    // 添加部分属性
    QMap<int, QVariant> temp(mapGroupRoleTemplate);
    temp[Global::NameRole] = strName;
    temp[ControllerTypeRole] = data(i, ControllerTypeRole);
    emit itemGroupChanged(temp);
    return strOldGroupName;
}

QByteArray BaseListModel::getSvgByteArray(const QString &svgpath)
{
    QFile svgfile(svgpath);
    if (!svgfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QByteArray();
    }
    QByteArray svgcontent = svgfile.readAll();
    svgfile.close();
    svgcontent = svgcontent.replace(LineColor, DisableColor)
                         .replace(TextColor, DisableColor)
                         .replace(LineLineColor, DisableColor);
    return svgcontent;
}

//////////////////////////////////////////////////////////////////////////

ControlBlockListModel::ControlBlockListModel(Global::PGroupConfigClass pgroupclass, QList<PModel> &listData,
                                             QObject *parent)
    : ComponentListModel(pgroupclass, Controller, parent), m_customlistData(listData)
{
    loadData(pgroupclass);
}

QVariant ControlBlockListModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    /*if (role == ControllerTypeRole) {
        return m_data[index.row()]->drawingBoard ? Board : Code;
    }
    if (role == BuildInRole) {
        return m_data[index.row()]->isBuildIn;
    }
    if (role == CanDeleteRole) {
        return m_data[index.row()]->canDelete;
    }
    if (role == ControlClassificationRole) {
        return m_data[index.row()]->classification;
    } */
    return ComponentListModel::data(index, role);
}

void ControlBlockListModel::reloadCustomListData(QList<PModel> &listData)
{
    m_customlistData = listData;
}

void ControlBlockListModel::loadData(Global::PGroupConfigClass pgroupclass)
{
    if (pgroupclass == nullptr) {
        return;
    }
    ms_existBlockNames.clear();
    foreach (QString groupname, pgroupclass->mapGroups.keys()) {
        // 如果当前没有激活项目不显示本地代码型和构造型
        if (m_pProjectServer) {
            PKLProject project = m_pProjectServer->GetCurProject();
            if (project == nullptr) {
                if (QObject::tr(groupname.toUtf8()) == ControllerGroupCustomCode
                    || QObject::tr(groupname.toUtf8()) == ControllerGroupCustomBoard) {
                    continue;
                }
            }
        }
        if (pgroupclass->mapGroups[groupname] != nullptr) {
            foreach (QString prototypename, pgroupclass->mapGroups[groupname]->groupBlocksMap.keys()) {
                // 目前只有本地代码型和构造型有创建实例
                if (QObject::tr(groupname.toUtf8()) == ControllerGroupCustomCode) {
                    m_mapCPTItem.insert(prototypename,
                                        CPTModelItem(groupname, true,
                                                     ContextMenu::AddComponentToBoard
                                                             | ContextMenu::CreateComponentInstance
                                                             | ContextMenu::ModifyControlComponent
                                                             | ContextMenu::RemoveCodeComponent));

                } else if (QObject::tr(groupname.toUtf8()) == ControllerGroupCustomBoard) {
                    m_mapCPTItem.insert(prototypename,
                                        CPTModelItem(groupname, true,
                                                     ContextMenu::AddComponentToBoard
                                                             | ContextMenu::CreateComponentInstance
                                                             | ContextMenu::ModifyBoardComponent
                                                             | ContextMenu::RemoveCodeComponent));
                } else {
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
                    // 插槽和电气接口模块 只有添加画板项
                    if (prototypename == "Slot" || prototypename == "ElectricalInterface") {
                        m_mapCPTItem.insert(
                                prototypename,
                                CPTModelItem(groupname, true, ContextMenu::AddComponentToBoard | ContextMenu::Help));
                    } else {
                        m_mapCPTItem.insert(
                                prototypename,
                                CPTModelItem(groupname, true, ContextMenu::AddComponentToBoard | ContextMenu::Help|ContextMenu::ModifyControlComponent));
                    }

#else
                    m_mapCPTItem.insert(prototypename, CPTModelItem(groupname, true, ContextMenu::AddComponentToBoard));
#endif
                }
                ms_existBlockNames << prototypename;
                m_data.append(pgroupclass->mapGroups[groupname]->groupBlocksMap[prototypename]);
            }
        }
    }
}

DeviceBlockListModel::DeviceBlockListModel(Global::PGroupConfigClass pgroupclass, QObject *parent)
    : ComponentListModel(pgroupclass, Device, parent)
{
    loadData(pgroupclass);
}

QVariant DeviceBlockListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == UrlNoDragRole) {
        return m_svgContentNoDrag[m_data[index.row()].prototypeName];
    }

    return ComponentListModel::data(index, role);
}

void DeviceBlockListModel::loadData(Global::PGroupConfigClass pgroupclass)
{
    // #ifdef QT_DEBUG
    //  m_pDeviceGroupClass->mapGroups.clear();
    if (pgroupclass == nullptr) {
        return;
    }
    m_svgContentNoDrag.clear();
    foreach (QString groupname, pgroupclass->mapGroups.keys()) {
        if (m_pProjectServer && !m_pProjectServer->GetCurProject()) {
            if (tr(groupname.toUtf8()) == DeviceGroupLocalCombine) {
                continue;
            }
        }
        if (pgroupclass->mapGroups[groupname] != nullptr) {
            foreach (QString prototypename, pgroupclass->mapGroups[groupname]->groupBlocksMap.keys()) {
                if (QObject::tr(groupname.toUtf8()) == DeviceGroupLocalCombine) {
                    m_mapCPTItem.insert(
                            prototypename,
                            CPTModelItem(groupname, true,
                                         ContextMenu::AddComponentToBoard | ContextMenu::ModifyBoardComponent));
                } else {
                    m_mapCPTItem.insert(prototypename,
                                        CPTModelItem(groupname, true,
                                                     ContextMenu::AddComponentToBoard | ContextMenu::Help
                                                             | ContextMenu::elecModifyCodeComponent));
                    m_svgContentNoDrag.insert(prototypename,
                                              getSvgByteArray(pgroupclass->mapGroups[groupname]
                                                                      ->groupBlocksMap[prototypename]
                                                                      .viewIcon.toString()));
                }
                ms_existBlockNames << prototypename;
                m_data.append(pgroupclass->mapGroups[groupname]->groupBlocksMap[prototypename]);
            }
        }
    }
}

MeasurerBlockListModel::MeasurerBlockListModel(Global::PGroupConfigClass pgroupclass, QObject *parent)
    : ComponentListModel(pgroupclass, Measurer, parent)
{
    loadData(pgroupclass);
}

void MeasurerBlockListModel::loadData(Global::PGroupConfigClass pgroupclass)
{
    /*foreach(const PBlock &pBlock, listData) {
    ms_existBlockNames << pBlock->prototypeName;

    m_mapGroupName.insert(pBlock->prototypeName, MeasurerGroupMeasurer);
    m_mapContextType.insert(pBlock->prototypeName, ContextMenu::Ignore);
    }*/
}

////////////////////////////////////////////////////////////////////////////

bool ComponentProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (!m_strFilterGroupName.isEmpty()) {
        if (index.isValid()) {
            QString strGroupName = index.data(Global::GroupNameRole).toString();
            if (strGroupName != m_strFilterGroupName) {
                return false;
            }
        }
    }
    if (!index.data(Global::ItemNeedDisplay).toBool()) {
        return false;
    }
    QString strTypeName = index.data(ReadableNameRole).toString();
    QString strDisplayName = index.data(Global::DisplayNameRole).toString();
    QString strPrototypeName = index.data(Global::NameRole).toString();
    // 过滤到Scope模块 http://10.10.40.11/simunps/simunps-front/-/issues/141
    if (strTypeName == "Scope") {
        return false;
    }

    if (m_strFilterString.isEmpty()) {
        return true;
    }

    return (QObject::tr(strTypeName.toUtf8()).contains(m_strFilterString, Qt::CaseInsensitive)
            || QObject::tr(strDisplayName.toUtf8()).contains(m_strFilterString, Qt::CaseInsensitive)
            || QObject::tr(strPrototypeName.toUtf8()).contains(m_strFilterString, Qt::CaseInsensitive))
            || (strTypeName.contains(m_strFilterString, Qt::CaseInsensitive)
                || strDisplayName.contains(m_strFilterString, Qt::CaseInsensitive)
                || strPrototypeName.contains(m_strFilterString, Qt::CaseInsensitive));
}

bool ComponentProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return left.data(ItemDisplayOrderRole).toInt() < right.data(ItemDisplayOrderRole).toInt();
}

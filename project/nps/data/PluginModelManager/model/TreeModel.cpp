#include "TreeModel.h"
#include "KLModelDefinitionCore.h"
#include "KLModelDefinitionCore/GlobalAssistant.h"
#include "ModelManagerConst.h"
#include "ModelViewDefine.h"
#include "QImage"
#include "TranslationTool.h"
#include <QBrush>
#include <QIcon>
#include <QMimeData>

QString m_strFilterString;

using namespace Kcc::BlockDefinition;

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent), m_codeItem(nullptr), m_combineItem(nullptr), m_eleCombineItem(nullptr)
{
    m_rootItem = new TreeItem();
}

TreeModel::~TreeModel()
{
    delete m_rootItem;
}

TreeItem *TreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return item;
    } else {
        return m_rootItem;
    }
}

void TreeModel::addChildItem(TreeItem *childItem)
{
    auto parentItem = childItem->parent();
    if (!parentItem) {
        return;
    }
    auto parentIndex = parentItem->m_index;
    if (!parentIndex.isValid()) {
        return;
    }

    int first = parentItem->childCount();

    beginInsertRows(parentIndex, first, first);

    parentItem->addChild(childItem);

    endInsertRows();
}

void TreeModel::removeChildItem(TreeItem *childItem)
{
    auto parentItem = childItem->parent();
    if (!parentItem) {
        return;
    }
    auto parentIndex = parentItem->m_index;
    if (!parentIndex.isValid()) {
        return;
    }

    int first = childItem->row();

    beginRemoveRows(parentIndex, first, first);

    parentItem->removeChild(childItem);

    endRemoveRows();
}

void TreeModel::removeChildrenItems(TreeItem *parentItem)
{
    if (!parentItem) {
        return;
    }
    auto parentIndex = parentItem->m_index;
    if (!parentIndex.isValid()) {
        return;
    }
    if (parentItem->childCount() <= 0) {
        return;
    }

    beginRemoveRows(parentIndex, 0, parentItem->childCount() - 1);

    parentItem->removeChildren();

    endRemoveRows();
}

TreeItem *TreeModel::rootItem()
{
    return m_rootItem;
}

// 获取index.row行，index.column列数据
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = itemFromIndex(index);
    if (item) {
        switch (role) {
        case Qt::DisplayRole:
            if (item->m_type == TreeItem::LeafNode) {
                return item->m_name;
            } else if (item->m_type == TreeItem::HeadNode) {
                return TranslationTool::getTrToolkitName(item->m_name); // 翻译 工具箱名称
            } else {
                return QObject::tr(item->m_name.toUtf8());
            }
        case Qt::ToolTipRole: {
            if (item->m_model) {
                return item->m_model->getPrototypeName_CHS();
            } else {
                return QObject::tr(item->m_name.toUtf8());
            }
        }
        case Qt::DecorationRole: {
            if (item->m_image.isValid()) {
                QImage image = qvariant_cast<QImage>(item->m_image);
                image = image.scaled(ModelViewDefine::TreeIconSize.width(), ModelViewDefine::TreeIconSize.height(),
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                return image;
            }
        } break;
        case Qt::TextColorRole: {
            auto flags = this->flags(index);
            bool isEnabled = flags.testFlag(Qt::ItemIsEnabled);
            if (!isEnabled) {
                return ModelViewDefine::TEXTCOLOR_DISABLE;
            }
        } break;
        case ModelViewDefine::TypeRole:
            return item->m_type;
        case ModelViewDefine::ImageRole:
            return item->m_image;
        case ModelViewDefine::ModelType:
            return item->m_modelType;
        default:
            break;
        }

        return QVariant();
    }

    return QVariant();
}

// 在parent节点下，第row行，第column列位置上创建索引
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = itemFromIndex(parent);
    TreeItem *item = parentItem->child(row);
    if (item) {
        item->m_index = createIndex(row, column, item);
        return item->m_index;
    } else
        return QModelIndex();
}

// 创建index的父索引
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *item = itemFromIndex(index);
    TreeItem *parentItem = item->parent();
    if (parentItem == m_rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

// 获取索引parent下有多少行
int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeItem *item = itemFromIndex(parent);
    if (item) {
        return item->childCount();
    }

    return 0;
}

// 返回索引parent下有多少列
int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    auto curItem = itemFromIndex(index);
    if (curItem->m_isAuthorized) { // 已授权
        flags |= Qt::ItemIsDragEnabled;
    } else {
        return (flags & ~Qt::ItemIsEnabled); // 未授权
    }

    return flags;
}

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    if (indexes.size() < 1) {
        return mimeData;
    }
    QModelIndex index = indexes[0];
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    if (!index.isValid()) {
        return mimeData;
    }

    auto curItem = itemFromIndex(index);
    auto modelName = curItem->m_name;
    if (curItem->m_model) {
        modelName = curItem->m_model->getPrototypeName();
    }

    switch (curItem->m_modelType) {
    case ElectricalBlock::Type:
        stream << modelName
               << ((modelName == NPS::PROTOTYPENAME_EXTERNAL || modelName == NPS::PROTOTYPENAME_SINGAL)
                           ? ElecCombineBoardModel::Type
                           : ElecBoardModel::Type);
        break;
    case ElecCombineBoardModel::Type:
        stream << modelName << ElecBoardModel::Type;
        break;
    case ControlBlock::Type:
        stream << modelName
               << (modelName == NPS::PROTOTYPENAME_SLOT
                           ? ComplexBoardModel::Type
                           : ((modelName == NPS::PROTOTYPENAME_CTRLIN || modelName == NPS::PROTOTYPENAME_CTRLOUT)
                                      ? CombineBoardModel::Type
                                      : ControlBoardModel::Type));
        break;
    case CombineBoardModel::Type:
        stream << modelName << ControlBoardModel::Type;
        break;
    default:
        stream << modelName << -1;
        break;
    }

    mimeData->setData(KL_TOOLKIT::DRAG_MIME_TYPE_STRING, encodedData);
    return mimeData;
}

QStringList TreeModel::mimeTypes() const
{
    QStringList types;
    types << KL_TOOLKIT::DRAG_MIME_TYPE_STRING;
    return types;
}

TreeViewProxyModel::TreeViewProxyModel(bool isShowLeaf, QObject *parent)
    : QSortFilterProxyModel(parent), m_isShowLeaf(isShowLeaf)
{
}

void TreeViewProxyModel::setFilterString(const QString &strFilter)
{
    m_strFilterString = strFilter;
    invalidateFilter();
}

bool TreeViewProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    QString strDisplayName = index.data(Qt::DisplayRole).toString();
    TreeModel *treeModel = qobject_cast<TreeModel *>(sourceModel());
    if (!treeModel) {
        return false;
    }
    auto item = treeModel->itemFromIndex(index);
    if (!item) {
        return false;
    }
    if (item->m_type == TreeItem::LeafNode && !m_isShowLeaf) { // 不显示叶子节点
        return false;
    }

    bool m_pFilter = QObject::tr(strDisplayName.toUtf8()).contains(m_strFilterString, Qt::CaseInsensitive)
            || strDisplayName.contains(m_strFilterString, Qt::CaseInsensitive);
    if (m_pFilter) {
        return true;
    } else {
        // 不符合条件的父节点 对其子节点进行递归判断处理
        for (int i = 0; i < sourceModel()->rowCount(index); i++) {
            if (TreeViewProxyModel::filterAcceptsRow(i, index)) {
                return true;
            }
        }
        return false;
    }
}

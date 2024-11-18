#include "ListModel.h"
#include "KLModelDefinitionCore.h"
#include "KLModelDefinitionCore/GlobalAssistant.h"
#include "ModelManagerConst.h"
#include "ModelViewDefine.h"
#include <QMimeData>
#include <QPixmap>

using namespace Kcc::BlockDefinition;

ListModel::ListModel(QObject *parent) : QAbstractListModel(parent), m_parentItem(nullptr) { }

ListModel::~ListModel() { }

TreeItem *ListModel::getItemByRow(int row)
{
    if (row < m_lstItem.size()) {
        return m_lstItem.at(row);
    }
    return nullptr;
}

void ListModel::refreshModelData(TreeItem *parentItem)
{
    beginResetModel();

    m_parentItem = parentItem;
    m_lstItem = parentItem->children();

    endResetModel();
}

Qt::ItemFlags ListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags(index);

    if (index.isValid() && m_lstItem.size() > index.row()) {
        if (m_lstItem[index.row()]->m_isAuthorized) { // 已授权
#ifdef COMPILER_DEVELOPER_MODE
            return (flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
#else
            return (flags | Qt::ItemIsDragEnabled);
#endif
        } else {
            return (flags & ~Qt::ItemIsEnabled); // 未授权
        }
    }

    return flags;
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    return m_lstItem.size();
}

Qt::DropActions ListModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool ListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);

    if (!data->hasFormat(KL_TOOLKIT::DRAG_MIME_TYPE_STRING))
        return false;

    if (column > 0)
        return false;

    if (!parent.isValid()) {
        return false;
    }

    return true;
}

bool ListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                             const QModelIndex &parent)
{
    if (!data->hasFormat(KL_TOOLKIT::DRAG_MIME_TYPE_STRING))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    QByteArray encodedData = data->data(KL_TOOLKIT::DRAG_MIME_TYPE_STRING);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QString modelName;
    int modelType = -1;
    stream >> modelName >> modelType;

    TreeItem *firstItem = [&]() -> TreeItem * {
        for (auto item : m_lstItem) {
            auto name = item->m_name;
            if (item->m_model) {
                name = item->m_model->getPrototypeName();
            }
            if (name == modelName) {
                return item;
            }
        }
        return nullptr;
    }();

    if (!firstItem) {
        return false;
    }

    // 交换,更新数据
    beginResetModel();
    m_lstItem.swap(m_lstItem.indexOf(firstItem), parent.row());
    m_parentItem->setChildren(m_lstItem);
    endResetModel();

    return true;
}

QStringList ListModel::mimeTypes() const
{
    QStringList types;
    types << KL_TOOLKIT::DRAG_MIME_TYPE_STRING;
    return types;
}

QMimeData *ListModel::mimeData(const QModelIndexList &indexes) const
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

    auto curItem = m_lstItem[index.row()];
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
    // stream << modelName << curItem->m_modelType;

    mimeData->setData(KL_TOOLKIT::DRAG_MIME_TYPE_STRING, encodedData);
    return mimeData;
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row >= m_lstItem.size()) {
        return QVariant();
    }
    auto item = m_lstItem[row];
    if (item) {
        switch (role) {
        case Qt::DecorationRole: {
            QPixmap pixmap = qvariant_cast<QPixmap>(item->m_image);
            return pixmap;
        } break;
        case Qt::DisplayRole: {
            if (item->m_type == TreeItem::LeafNode) {
                return item->m_name;
            } else {
                return QObject::tr(item->m_name.toUtf8());
            }
        } break;
        case Qt::ToolTipRole: {
            if (item->m_model) {
                return item->m_model->getPrototypeName_CHS();
            } else {
                return QObject::tr(item->m_name.toUtf8());
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
    }

    return QVariant();
}

#pragma once

#include "CoreLib/ServerManager.h"
#include "GlobalDefinition.h"
#include "KLModelDefinitionCore/GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
#include <QAbstractListModel>
#include <QMimeData>
#include <QSet>
#include <QSortFilterProxyModel>

using namespace Kcc::ProjectManager;
using namespace Kcc::BlockDefinition;

static const QString PrototypeName_In = QString("In");
static const QString PrototypeName_Out = QString("Out");
static const QString PrototypeName_Slot = QString("Slot");
static const QString PrototypeName_BusCreator = QString("BusCreator");
static const QString PrototypeName_BusSelector = QString("BusSelector");
static const QString PrototypeName_ElectricalInterface = QString("ElectricalInterface");
static const QString PrototypeName_FMU = QString("FMU");

struct CPTModelItem {
    CPTModelItem(const QString _groupName = "", const bool _bDraggable = false, const int _contextType = -1,
                 const QStringList _cblocklist = QStringList())
        : groupName(_groupName), bDraggable(_bDraggable), contextType(_contextType), containBlockList(_cblocklist)
    {
    }
    QString groupName;            // block对应的组名
    bool bDraggable;              // block模块是否可以拖拽
    int contextType;              // block对应的交互菜单
    QStringList containBlockList; // 模块包含的原型。构造型模块独有
};

// 列表视图使用的model基类
class BaseListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CustomRole {
        CustomRole_ListDataType = 600,
    };
    BaseListModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    // 自定义拖拽，不新增和删除元素，仅修改数据，返回旧分组名供ToolPage使用，mapGroupRoleTempalte表示该分组的通用属性
    QString dropMimeDataCustom(const QMimeData *pMimeData, const QMap<int, QVariant> &mapGroupRoleTemplate);

    static QSet<QString> getExistBlockNames() { return ms_existBlockNames; }
    QByteArray getSvgByteArray(const QString &svgpath);

signals:
    void itemGroupChanged(const QMap<int, QVariant> &mapRoleData);

protected:
    static QSet<QString> ms_existBlockNames; // 已存在的元器件名称，包括电气、控制、测量
};

//////////////////////////////////////////////////////////////////////////

// 用模板来适配不同类型元器件
template<typename T>
class ComponentListModel : public BaseListModel
{
public:
    ComponentListModel(T pgroupclass, Global::Category category, QObject *parent = nullptr)
        : BaseListModel(parent), m_category(category), m_curBoardName(""), m_pGroupClass(pgroupclass)
    {
        m_strModuleName = nameFromCategory(category);
        Q_ASSERT(!m_strModuleName.isEmpty());
        init();
    }

    void init()
    {
        m_pProjectServer = RequestServer<IProjectManagerServer>();
        if (!m_pProjectServer) {
            return;
        }
    }

    void reloadData(T pgroupclass)
    {
        beginResetModel();
        m_data.clear();
        m_mapCPTItem.clear();
        loadData(pgroupclass);
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if (parent.isValid()) {
            return 0;
        }
        return m_data.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid()) {
            return QVariant();
        }

        if (role == Qt::DisplayRole) {
            return m_data[index.row()].prototypeName;
        }
        if (role == Qt::ToolTipRole) {
            return QObject::tr(m_data[index.row()].viewName.toUtf8());
        }
        if (role == Qt::WhatsThisRole) {
            return m_data[index.row()].viewName;
        }
        if (role == Global::CategoryRole) {
            return m_category;
        }
        if (role == Global::ModuleNameRole) {
            return m_strModuleName;
        }
        if (role == Global::GroupNameRole) {
            return m_mapCPTItem[m_data[index.row()].prototypeName].groupName;
        }
        if (role == Global::ContextTypeRole) {
            return m_mapCPTItem[m_data[index.row()].prototypeName].contextType;
        }
        if (role == Global::NameRole) {
            return m_data[index.row()].prototypeName;
        }
        if (role == Global::ItemDisplayTypeRole) {
            return m_data[index.row()].displaytype;
        }
        if (role == Global::ItemDisplayOrderRole) {
            return m_data[index.row()].displayorder;
        }
        if (role == Global::ItemNeedDisplay) {
            return m_data[index.row()].display;
        }
        if (role == Global::ReadableNameRole) {
            return m_data[index.row()].prototypeName;
            /*return m_data[index.row()]->prototypeName_Readable.isEmpty() ? m_data[index.row()]->prototypeName
                : m_data[index.row()]->prototypeName_Readable;*/
        }
        if (role == Global::DisplayNameRole) {
            // return m_data[index.row()]->prototypeName_CHS;
            return m_data[index.row()].viewName;
        }
        if (role == Global::UrlRole) {
            // return m_data[index.row()]->parameterMap[Block::pictureList];
            return m_data[index.row()].viewIcon;
        }
        if (role == Global::DragEnabledRole) {
            return m_mapCPTItem[m_data[index.row()].prototypeName].bDraggable;
        }
        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        if (!index.isValid()) {
            return false;
        }

        if (role == Global::CategoryRole) {
            m_category = static_cast<Global::Category>(value.toInt());
        } else if (role == Global::ModuleNameRole) {
            m_strModuleName = value.toString();
        } else if (role == Global::GroupNameRole) {
            m_mapCPTItem[m_data[index.row()].prototypeName].groupName = value.toString();
        } else if (role == Global::ContextTypeRole) {
            m_mapCPTItem[m_data[index.row()].prototypeName].contextType = value.toInt();
        } else if (role == Global::NameRole) {
            m_data[index.row()].prototypeName = value.toString();
        } else if (role == Global::NameRole) {
            m_data[index.row()].prototypeName = value.toString();
        } else if (role == Global::DisplayNameRole) {
            m_data[index.row()].viewName = value.toString();
        } else if (role == Global::DragEnabledRole) {
            // m_bDragEnabled = value.toBool();
            // emit dataChanged(index, index.sibling(rowCount(),index.column()));
            return true;
        } else {
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }

    T getConfigClass() { return m_pGroupClass; }

protected:
    virtual void loadData(T pgroupclass) = 0;

protected:
    Global::Category m_category;              // 类别
    QString m_strModuleName;                  // 模块名
    QString m_curBoardName;                   // 当前画板名
    QMap<QString, CPTModelItem> m_mapCPTItem; // block对应的模块属性
    QList<Global::GroupClass::GroupBlockParamater> m_data;
    T m_pGroupClass; // 分组数据
public:
    PIProjectManagerServer m_pProjectServer; // 项目管理服务接口
};

// 控制
class ControlBlockListModel : public ComponentListModel<Global::PGroupConfigClass>
{
    Q_OBJECT
public:
    ControlBlockListModel(Global::PGroupConfigClass pgroupclass, QList<PModel> &listData, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void reloadCustomListData(QList<PModel> &listData);

protected:
    void loadData(Global::PGroupConfigClass pgroupclass) override;

private:
    // 找到构造模块如何替换
    QStringList getBlockContainsSterotype(PModel pcblock); // 获取模块包含的构造型画板列表

private:
    QList<PModel> m_customlistData;
};

// 电气
class DeviceBlockListModel : public ComponentListModel<Global::PGroupConfigClass>
{
    Q_OBJECT
public:
    DeviceBlockListModel(Global::PGroupConfigClass pgroupclass, QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    void loadData(Global::PGroupConfigClass pgroupclass) override;

private:
    QMap<QString, QByteArray> m_svgContentNoDrag;
};

// 测量
class MeasurerBlockListModel : public ComponentListModel<Global::PGroupConfigClass>
{
    Q_OBJECT
public:
    MeasurerBlockListModel(Global::PGroupConfigClass pgroupclass, QObject *parent = nullptr);

protected:
    void loadData(Global::PGroupConfigClass pgroupclass) override;
};

//////////////////////////////////////////////////////////////////////////
// 排序和筛选 Model
class ComponentProxyModel : public QSortFilterProxyModel
{
public:
    explicit ComponentProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) { }

    void setFilterString(const QString &strFilter)
    {
        m_strFilterString = strFilter;
        invalidateFilter();
    }

    void setFilterGroupName(const QString &strName) { m_strFilterGroupName = strName; }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_strFilterString;
    QString m_strFilterGroupName;
};

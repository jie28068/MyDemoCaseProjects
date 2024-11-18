#ifndef TABLEBASEMODEL_H
#define TABLEBASEMODEL_H

#include "AssistantDefine.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/BlockDefinition.h"
#include "KLProject/KLProject.h"
#include <QAbstractTableModel>
#include <QColor>
#include <QList>
#include <QSharedPointer>

namespace Kcc {
namespace BlockDefinition {
class Model;
class ElectricalBlock;
class ControlBlock;
class VariableGroup;
class DrawingBoardClass;
class Variable;
class DeviceModel;
class SlotBlock;
}
}

namespace ITEMKEY {
static const QString NOT_USECOLOR = QString("NOT_USECOLOR");
static const QString EXCLUDE_PORT = QString("EXCLUDE_PORT");             // 剔除端口
static const QString EXCLUDE_NOTNUMBER = QString("EXCLUDE_NOTNUMBER");   // 剔除非数值的变量
static const QString DISPLAY_ADD_SYMBOL = QString("DISPLAY_ADD_SYMBOL"); // 展示的时候添加符号
}
struct OldNewValue {
    OldNewValue(const QVariant &_oldvalue = QVariant(), const QVariant &_newvalue = QVariant())
        : oldValue(_oldvalue), newValue(_newvalue)
    {
    }
    QVariant oldValue;
    QVariant newValue;
};
struct OldNewDataStruct {
    OldNewDataStruct(const QString &_modelUUID = "", const int &_modelType = -1)
        : modelUUID(_modelUUID), modelType(_modelType)
    {
    }
    bool isValid() { return modelUUID.isEmpty() ? false : true; }
    void addChangedItem(const int &grouptype, const QString &keyword, const QVariant &oldvalue,
                        const QVariant &newvalue)
    {
        if (oldValueMap.contains(grouptype)) {
            oldValueMap[grouptype][keyword] = oldvalue;
            NewValueMap[grouptype][keyword] = newvalue;
        } else {
            QMap<QString, QVariant> oldmap;
            QMap<QString, QVariant> newmap;
            oldmap.insert(keyword, oldvalue);
            newmap.insert(keyword, newvalue);
            oldValueMap.insert(grouptype, oldmap);
            NewValueMap.insert(grouptype, newmap);
        }
    }
    QMap<int, QMap<QString, QVariant>> getValueMap(bool bnew)
    {
        if (bnew) {
            return NewValueMap;
        } else {
            return oldValueMap;
        }
    }
    const QMap<QString, QVariant> getGroupMap(const int &grouptype, bool bnew = true)
    {
        if (bnew) {
            return NewValueMap.value(grouptype, QMap<QString, QVariant>());
        }
        return oldValueMap.value(grouptype, QMap<QString, QVariant>());
    }
    const QVariant getItemValue(const int &grouptype, const QString &keyword, bool bnew = true)
    {
        if (bnew) {
            if (NewValueMap.contains(grouptype) && NewValueMap[grouptype].contains(keyword)) {
                return NewValueMap[grouptype][keyword];
            }
            return QVariant();
        }
        if (oldValueMap.contains(grouptype) && oldValueMap[grouptype].contains(keyword)) {
            return oldValueMap[grouptype][keyword];
        }
        return QVariant();
    }
    QString modelUUID;
    int modelType;
    // int varclass,QMap<key,value>
    QMap<int, QMap<QString, QVariant>> oldValueMap;
    QMap<int, QMap<QString, QVariant>> NewValueMap;
};

class TableBaseModelItem;
class ModelRowItem;

class TableBaseModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableBaseModel(QObject *parent = nullptr);
    ~TableBaseModel();

    enum ModelDataType {
        ModelDataType_Invalid,
        ModelDataType_Tree,
        ModelDataType_BuildInBlocks,
        ModelDataType_CodeBlocks,
        ModelDataType_CombineBoardBlocks,
        ModelDataType_ComplexBoardBlocks,
        ModelDataType_ControlBoardBlocks,
        ModelDataType_ElectricalBlocks,
        ModelDataType_GlobalDeviceTypes,
        ModelDataType_ProjectDeviceTypes,
        ModelDataType_CodeInstance,
        ModelDataType_CombineBoardInstance,
        ModelDataType_ComplexBoardInstances,
        ModelDataType_ControlInstances,
        ModelDataType_ControlBlocks
    };
    enum VariableClass {
        VariableClass_Other,
        VariableClass_CtrInput,
        VariableClass_CtrOutPut,
        VariableClass_CtrParam,
        VariableClass_CtrCState,
        VariableClass_CtrDState,
        VariableClass_ElecParam,
        VariableClass_ElecDeviceType,
        VariableClass_ElecSimuParam,
        VariableClass_ElecInitParam,
        VariableClass_ElecBusbarParam,
        VariableClass_ElecPFResult, // PowerFlowResult
    };

    enum CustomRole {
        CustomRole_VariableClass = Qt::UserRole + 601,
        CustomRole_ModelDataType = Qt::UserRole + 602,
        CustomRole_PrototypeName = Qt::UserRole + 603,
        CustomRole_NotUseColor = Qt::UserRole + 605,
        CustomRole_CurProjectActiveSts = Qt::UserRole + 606,
        CustomRole_DevicePrototype = Qt::UserRole + 607,
        CustomRole_DrawboardRunningSts = Qt::UserRole + 608,
        CustomRole_CanOpen = Qt::UserRole + 609
    };
    enum ModelData {
        ModelData_Value = 0, // value
        ModelData_Edit,      // 是否可编辑
    };
    enum HeaderBreak {
        HeaderBreak_None, // 不换行
        HeaderBreak_Unit, // 在单位处换行
    };

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // 自定义函数
    void setReadOnlyData(const QList<QStringList> &listdata, const QStringList &titlelist);
    TableBaseModel::ModelDataType getModelDataType();
    void setWidgetObject(AssistantDefine::WidgetObject wobj) { m_widgetObject = wobj; }
    AssistantDefine::WidgetObject getWidgetObject() { return m_widgetObject; }
    QSharedPointer<Kcc::BlockDefinition::Model> getModelByIndex(const QModelIndex &index);
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> getModelsByIndexs(const QModelIndexList &indexList);
    /// @brief 隐藏显示表格header的时候，最后一列触发信号就会stretch
    void emitHeaderDataChanged();
    void SetProjectRuningSts(bool runing);
signals:
    void ModelValueChanged(const QList<OldNewDataStruct> &modifylist);
    void VerifyDrawBoard(const QString &uuid, const int &modelType);

protected:
    /// @brief 数据有效性检查
    /// @param index
    /// @return
    bool isModelDataValid(const QModelIndex &index) const;
    /// @brief 重写模型数据
    /// @param index 索引
    /// @param modeldata 数据类型
    /// @return
    virtual QVariant TModelData(const QModelIndex &index, TableBaseModel::ModelData modeldata) const;
    /// @brief 清楚数据
    void clear();
    /// @brief 获取表格名，用来打log
    /// @return
    QString getTableName();
    // common

    /// @brief 获取模块对应的组列表的variable列表
    /// @param pmodel 模块
    /// @param grouplist 组名列表
    /// @param recursive 是否递归
    /// @param excludekeys 剔除的keword列表
    /// @return
    QList<QSharedPointer<Kcc::BlockDefinition::Variable>>
    getVarGroupList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QStringList &grouplist,
                    bool recursive = false, const QStringList &excludekeys = QStringList()) const;
    /// @brief 获取模块对应组名的variable列表
    /// @param pmodel 模块
    /// @param groupName 组名
    /// @param recursive 是否递归
    /// @param excludekeys 剔除的keword列表
    /// @return
    QList<QSharedPointer<Kcc::BlockDefinition::Variable>>
    getVarGroupList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                    bool recursive = false, const QStringList &excludekeys = QStringList()) const;
    /// @brief 获取组内keyword的列表
    /// @param pmodel 模块
    /// @param groupName 组名
    /// @param recursive 是否递归
    /// @param excludekeys 剔除的keyword列表
    /// @return
    QStringList getVarGroupStringList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &groupName,
                                      bool recursive = false, const QStringList &excludekeys = QStringList()) const;
    QList<ModelRowItem> getModelRowItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                            const QStringList &groupList, bool recursive = false, bool readOndy = false,
                                            const QStringList &excludekeys = QStringList(), const QString &path = "");
    /// @brief 获取模型组内变量RowItem列表
    /// @param pmodel 模型
    /// @param groupName 组名
    /// @param recursive 是否递归
    /// @param readOndy 是否只读
    /// @param excludekeys 剔除的keyword列表
    /// @param path 路径
    /// @return
    QList<ModelRowItem> getModelRowItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                            const QString &groupName, bool recursive = false, bool readOndy = false,
                                            const QStringList &excludekeys = QStringList(), const QString &path = "");
    /// @brief 获取模型参数列表
    /// @param pmodel 模型
    /// @param prototypeName 原型名
    /// @param groupName 组名
    /// @param recursive 是否递归
    /// @param excludekeys 剔除的keyword列表
    /// @param path 路径
    /// @return
    QList<TableBaseModelItem> getTableBaseModelItemList(QSharedPointer<Kcc::BlockDefinition::Model> pmodel,
                                                        const QString &prototypeName, const QString &groupName,
                                                        bool recursive = false,
                                                        const QStringList &excludekeys = QStringList(),
                                                        const QString &path = "");
    /// @brief 是否为端口
    /// @param pmodel
    /// @param keyword
    /// @return
    bool isPort(QSharedPointer<Kcc::BlockDefinition::Model> pmodel, const QString &keyword) const;
    /// @brief 获取同一行的indexs Map
    /// @param indexlist 所有index
    /// @return
    QMap<int, QModelIndexList> getSameRowIndexes(const QModelIndexList &indexlist);
    /// @brief 模型组名转为表格分组
    /// @param groupName 组名
    /// @return
    TableBaseModel::VariableClass GroupNameMapToVariableClass(const QString &groupName) const;
    /// @brief 表格分组转换为模型组名
    /// @param varclass 表格分组
    /// @return
    QString VariableClassMapToGroupName(const TableBaseModel::VariableClass &varclass) const;
    /// @brief 找到同当前index同行且关键字为keyword所在index
    /// @param index 当前index
    /// @param keyword 关键字
    /// @return
    const QModelIndex findKeywordIndex(const QModelIndex &index, const QString &keyword);

protected:
    QList<TableBaseModelItem> m_ModelItemList; // 存储list每列的一些属性。
    ModelDataType m_ModelDataType;
    QList<QStringList> m_data;
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_modelData;
    QSharedPointer<Kcc::BlockDefinition::Model> m_instanceModel;
    QList<ModelRowItem> m_instanceData;
    // 仅 elecblock uuid为key，连接侧的block为value,记录当前uuid连接的模块信息
    QMap<QString, QMap<QString, QSharedPointer<Kcc::BlockDefinition::ElectricalBlock>>> m_conElecBusBarMap;
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_drawBoardList;
    bool m_curProjActive;                         // 项目激活状态
    AssistantDefine::WidgetObject m_widgetObject; // 只需要设置一次
    QString m_devicePrototype;                    // 仅device使用
    QVariantMap m_boardActiveStsMap;              // 仅电气元件使用，画板未激活不可编辑
    bool m_DrawboardRunningSts;                   // 正在运行仿真
    HeaderBreak m_headerBreak;                    // 是否换行
};

struct TableBaseModelItem {
    TableBaseModelItem(const QString &_Keywords, const QString &_TitleName,
                       const QString _DataType = Kcc::BlockDefinition::RoleDataDefinition::ControlTypeTextbox,
                       const QVariant _DataRange = "",
                       TableBaseModel::VariableClass _varClass = TableBaseModel::VariableClass_Other,
                       const QString _prototypename = "", const QVariantMap otherproperty = QVariantMap())
        : Keywords(_Keywords),
          TitleName(_TitleName),
          DataType(_DataType),
          DataRange(_DataRange),
          varClass(_varClass),
          prototypename(_prototypename),
          otherPropertyMap(otherproperty)
    {
    }
    QString Keywords;
    QString TitleName;
    QString DataType;
    QVariant DataRange;
    TableBaseModel::VariableClass varClass;
    QString prototypename;        // 为空表示公用的列，如name，edittime
    QVariantMap otherPropertyMap; // 其他属性
};

struct ModelRowItem {
    ModelRowItem(const QString &_Keywords, PVariable _pVar = nullptr,
                 QSharedPointer<Kcc::BlockDefinition::Model> _model = nullptr, const QString &_groupName = "")
        : Keywords(_Keywords), pVar(_pVar), model(_model), groupName(_groupName)
    {
    }
    inline bool isValid() const
    {
        if (!Keywords.isEmpty() && pVar != nullptr && model != nullptr && !groupName.isEmpty()) {
            return true;
        }
        return false;
    }
    QString getPath() const
    {
        QStringList keylist = Keywords.split("/", QString::SkipEmptyParts, Qt::CaseSensitive);
        keylist.pop_back();
        return keylist.join("/");
    }
    QString getName() const { return Keywords.section('/', -1); }

    QString Keywords;
    PVariable pVar;
    QSharedPointer<Kcc::BlockDefinition::Model> model;
    QString groupName;
};

#endif // TABLEBASEMODEL_H

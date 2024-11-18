#ifndef CURRENTSIMUWIDGET_H
#define CURRENTSIMUWIDGET_H

#include "tableheaderview.h"
#include "ui_CurrentSimuWidget.h"
#include <QAbstractTableModel>
#include <QDateTime>
#include <QLabel>
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

namespace Kcc {
namespace BlockDefinition {
class Model;
class DrawingBoardClass;
class ElectricalBlock;
class ControlBlock;
class VariableGroup;
class Variable;
class CombineBoardModel;
}
}

static const QString PARAMTYPE_OUTPUT = QObject::tr("Output Variables"); // 输出变量
static const QString PARAMTYPE_INPUT = QObject::tr("Input Variables");   // 输入变量
// #ifdef COMPILER_PRODUCT_SIMUNPS
// static const QString PARAMTYPE_DISSTATE = QObject::tr("State Variable"); // 状态变量，在nps里面叫法前面不带离散
// #endif
// #ifdef COMPILER_PRODUCT_DESIGNER
static const QString PARAMTYPE_DISSTATE = QObject::tr("Discrete State Variable"); // 离散状态变量
// #endif
static const QString PARAMTYPE_CONSTATE = QObject::tr("Continue State Variable"); // 连续状态变量
static const QString PARAMTYPE_PARAM = QObject::tr("Parameter");                  // 参数

static const QString CHECKSTATE_ALL = QObject::tr("All");               // 全部
static const QString CHECKSTATE_CHECKED = QObject::tr("Selected");      // 选中
static const QString CHECKSTATE_UNCHECKED = QObject::tr("Unselected "); // 未选中

class VarTableModel;
class CustomSortFilterModel;
struct NameKeyItem {
    NameKeyItem &operator=(const NameKeyItem &other)
    {
        keywords = other.keywords;
        name = other.name;
    }
    bool operator==(const NameKeyItem &other) const
    {
        if (keywords == other.keywords && name == other.name) {
            return true;
        }
        return false;
    }
    NameKeyItem(const QString _keywords = "", const QString _name = "") : keywords(_keywords), name(_name) { }
    QString keywords; // key或对应uuid
    QString name;     // 名称，一般用来显示
};
struct VarTableItem {
    VarTableItem &operator=(const VarTableItem &other)
    {
        boardNameKey = other.boardNameKey;
        blockNameKey = other.blockNameKey;
        varNameKey = other.varNameKey;
        varClassName = other.varClassName;
        varAlias = other.varAlias;
        bSelected = other.bSelected;
        bSupportParamAdj = other.bSupportParamAdj;
        prototypeName = other.prototypeName;
        bBoardActive = other.bBoardActive;
        selectSaveTime = other.selectSaveTime;
        bCanSetVar = other.bCanSetVar;
    }
    bool operator==(const VarTableItem &other) const
    {
        if (boardNameKey == other.boardNameKey && blockNameKey == other.blockNameKey && varNameKey == other.varNameKey
            && varClassName == other.varClassName && varAlias == other.varAlias && bSelected == other.bSelected
            && bSupportParamAdj == other.bSupportParamAdj && prototypeName == other.prototypeName
            && bBoardActive == other.bBoardActive && selectSaveTime == other.selectSaveTime
            && bCanSetVar == other.bCanSetVar) {
            return true;
        }
        return false;
    }
    VarTableItem(const NameKeyItem _boardNameKey = NameKeyItem(), const NameKeyItem _blockNameKey = NameKeyItem(),
                 const NameKeyItem _varNameKey = NameKeyItem(), const QString _strVarAlias = "",
                 const QString _varClassName = "", bool _bSelected = false, bool _bSupportParamAdj = false,
                 const QString _prototypeName = "", bool _bBoardActive = false,
                 const QDateTime _modifytime = QDateTime(), bool _bCanSetVar = true)
        : boardNameKey(_boardNameKey),
          blockNameKey(_blockNameKey),
          varNameKey(_varNameKey),
          varClassName(_varClassName),
          varAlias(_strVarAlias),
          bSelected(_bSelected),
          bSupportParamAdj(_bSupportParamAdj),
          prototypeName(_prototypeName),
          bBoardActive(_bBoardActive),
          selectSaveTime(_modifytime),
          bCanSetVar(_bCanSetVar)
    {
    }
    QString getDictVarName()
    {
        return QString("%1.%2.%3").arg(boardNameKey.name).arg(blockNameKey.name).arg(varNameKey.name);
    }
    NameKeyItem boardNameKey; // 画板名与uuid
    NameKeyItem blockNameKey; // 模块名与uuid
    NameKeyItem varNameKey;   // 变量名与key
    QString varClassName;     // 变量类型名
    QString varAlias;         // 别名
    bool bSelected;           // 是否勾选变量
    bool bSupportParamAdj;    // 是否支持动态调参
    QString prototypeName;    // 模块原型名
    bool bBoardActive;        // 画板激活状态
    QDateTime selectSaveTime; // 选择保存的时间
    bool bCanSetVar;          // 是否可勾选（电气元件根据当前模块状态设置）
};

// 当前仿真画面的所有内容
class CurrentSimuWidget : public QMainWindow
{
    Q_OBJECT

public:
    // 控制模块参数类型
    enum CBlockParamType {
        CBlockParamType_OutPut,   // 输出变量
        CBlockParamType_DisState, // 离散状态变量
        CBlockParamType_ConState, // 连续状态变量
        CBlockParamType_Param,    // 参数
        CBlockParamType_InPut,    // 输入变量
    };
    CurrentSimuWidget(const QString &projname, QWidget *parent = nullptr);
    ~CurrentSimuWidget();
    void initData();
    void saveVariableData();

private:
    void InitUI();

    void initBoardData(QSharedPointer<Kcc::BlockDefinition::Model> drawboard);

    QList<VarTableItem> getCBlockModelList(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard,
                                           QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock);

    QList<VarTableItem> getEblockModelList(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard,
                                           QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> eblock,
                                           bool bBoardActive);

    QList<VarTableItem> getCBlockParam(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard,
                                       QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock,
                                       const QSharedPointer<Kcc::BlockDefinition::VariableGroup> &parameters,
                                       CBlockParamType cbparamtype);
    // QString getBlockAliasName(const QString &blockuuid, const QString &varkey,
    //                           const QMap<QString, DrawingBoardClass::VarAliasInfo> &aliasmap);
    // bool getSupportParamAdj(const QString &blockuuid, const QString &varkey,
    //                         const QList<DrawingBoardClass::BlockKeyParameter> &paramadjlist);
    void keywordsMaptoName(QString keywords, QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock,
                           QString &varrealname);

    void tranKeyToName(QStringList paramkeylist, QString &varrealname,
                       QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock);

    QSharedPointer<Kcc::BlockDefinition::Variable> getVariableByName(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                                     QString &varrealname);

    void initCombineModelSimuParameters(QSharedPointer<Kcc::BlockDefinition::Model> combineModel, QStandardItem *item,
                                        QMap<QString, QList<VarTableItem>> &allVarmap, QString &path);

    bool checkedChildItemText(QStandardItem *item, QString &text);

    void setTableHeaderCheckStatus(QString nodeName, QString prototypeName = "");

    void updateSelectedItemsCheckStatus(const QModelIndex &index);

    bool saveDrawboardData(QSharedPointer<Kcc::BlockDefinition::Model> drawboard, QString boardnames,
                           QMap<QString, QList<VarTableItem>> newVarMap);

    void getSlotConnectionsModel(QSharedPointer<Kcc::BlockDefinition::Model> &pBlock);

    bool eventFilter(QObject *obj, QEvent *evt) override;

private slots:
    void onClickedTreeItem(const QModelIndex &index);
    void onTableSelectedRowsChanged(const int &selectedrows);
    void onSearchColBoxChanged(int col);
    void onSearchTextChanged(const QString &textstr);
    void onTableItemClicked(const QModelIndex &index);
    void onChangeColSelectStatus(int col, bool isSelected);
    void slotHeaderClicked(int logicalIndex);
    void onChangeHeaderViewEnabled(bool enabled);
    void onTableSelectedAllRows();
    void onGetSelectedIndexs(QModelIndexList &indexlist);

private:
    Ui::CurrentSimuWidget ui;
    VarTableModel *m_pTableModel;
    TableHeaderView *m_pHeaderView;
    CustomSortFilterModel *m_tableProxyModel;

    QLabel *m_itemLabel;
    QString m_projectName;
    QString m_strBlockPrototypeName;
    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_lstAllboards;
    QList<VarTableItem> m_datalList;
    QStandardItemModel *m_pModel;
    QStandardItemModel *m_ptreeModel;
    QMap<QString, QList<VarTableItem>> m_allVarMap;
    QModelIndexList m_selected;
    QVariantMap m_boardActiveStsMap;
    bool m_isElecboardActived;
};

// 自定义table
class VarTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    VarTableModel(const QString &projname, QObject *parent = nullptr);
    ~VarTableModel();
    void init();
    enum CustomRole {
        CustomRole_PrototypeName = Qt::DisplayRole + 500,
    };
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    // 自定义函数
    void clear();
    void initData(const QMap<QString, QList<VarTableItem>> &allVarMap);
    void switchDataToBoard(const QString &boardname);
    QStringList getHeaderList();
    QMap<QString, QList<VarTableItem>> getOldAllVarMap();
    QMap<QString, QList<VarTableItem>> getNewAllVarMap();

private:
    int getHeaderTitleCol(const QString &titlename) const;

    bool getSystemRunningStatus();

signals:
    void changeHeaderViewEnabled(bool enabled) const;

private:
    QString m_currentBoardName;
    QString m_projectName;
    QMap<QString, QList<VarTableItem>> m_allVarmap;    // 画板为key,value QList<VarTableItem>
    QMap<QString, QList<VarTableItem>> m_allVarmapOld; // 画板为key,value QList<VarTableItem>
    QList<VarTableItem> m_data;
    QStringList m_headerData;
};

class CustomSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CustomSortFilterModel(QObject *parent = nullptr);
    ~CustomSortFilterModel();
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setFilterString(const QString &strFilter);

    void setFilterColumn(const QString &colname, const int &colno);
    void setFilterPrototypeName(const QString &prototypestr);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_strFilterString;
    QString m_FilterColumnStr;
    int m_FilterCol;
    QString m_prototypeName;
};

class CheckBoxDelegate : public QStyledItemDelegate
{
public:
    CheckBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
#endif // CURRENTSIMUWIDGET_H

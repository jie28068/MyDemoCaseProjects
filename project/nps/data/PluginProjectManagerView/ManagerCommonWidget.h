#ifndef MANAGERCOMMONWIDGET_H
#define MANAGERCOMMONWIDGET_H

#pragma once
#include "AssistantDefine.h"
#include "CustomMineData.h"
#include "CustomSortFilterProxyModel.h"
#include "CustomTreeModel.h"
#include "GlobalAssistant.h"
#include "KLWidgets/KColorDialog.h"
#include "ProjDataManager.h"
#include "SettingWidget.h"
#include "TableModelDataModel.h"
#include "server/Base/IServerInterfaceBase.h"
#include "ui_ProjectManagerWidget.h"
#include "undocommand.h"
#include "undostackmanager.h"
#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QSharedPointer>
#include <QTimer>

class ProjDataManager;
class KLProject;
class SyncModelData;
using Kcc::NotifyStruct;
namespace Kcc {
namespace BlockDefinition {
class Model;
}
}
class ManagerCommonWidget : public QMainWindow
{
    Q_OBJECT
public:
    ManagerCommonWidget(QSharedPointer<ProjDataManager> pProjDataManager, QWidget *parent = nullptr);
    ~ManagerCommonWidget();

    virtual void initData();
    virtual void clickedOpen();
    virtual void SyncModelDataToUI();
    virtual QVariantMap getTreeStateMap();
public slots:
    virtual void onManagerClosed(int btn);
    virtual void onTreeClicked(const QModelIndex &index);
    virtual void onTreeContextMenu(const QPoint &point);
    virtual void onActionTriggered();
    virtual void onModelValueChanged(const QList<OldNewDataStruct> &modifylist);
    virtual void onVerifyDrawBoard(const QString &uuid, const int &modelType);
    virtual void onPropertyManagerServerMsg(unsigned int code, const NotifyStruct &structinfo);
    virtual void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &structinfo);
    virtual void onReceiveProjectManagerServerMsg(uint code, const NotifyStruct &param);
    void onHorizontalValueChanged(bool bscrollright, int begincol, int changecolsno);
    void onHorizontalHeaderSectionResized(int logicalIndex, int oldSize, int newSize);
    void onTableSettingClicked();
    void onTableSettingOKclicked(const QList<TableSetItem> &curTableSetList);
    void onSearchColBoxChanged(const QString &textstr);
    void onSearchTextChanged(const QString &strtext);
    void onTableDoubleClicked(const QModelIndex &index);
    void onTableItemDoubleClicked(const QModelIndex &index);
    virtual void onSelectedRowsChanged(const int &totalselectrow);
    virtual void onTableMenuEnd();
    void onModelChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);

signals:
    void openBlockOrBoard();
    void deleteModel();
    void addModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void modelChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);

protected:
    virtual void resizeEvent(QResizeEvent *e) override;

protected:
    virtual void initUI();
    virtual void connectSignals();
    virtual void disconnectSignals();
    /// @brief 更新数据
    /// @param indexNames item根节点到当前节点的列表，为空则全部数据刷新
    virtual void updateUI(const QStringList &indexNames);
    /// @brief 选中某个item
    /// @param indexNames item根节点到当前节点的列表，为空则默认点击当前item
    virtual void clickedIndex(const QStringList &indexNames);
    /// @brief 创建模型数据与视图同步机制
    void createSyncDataModel();
    /// @brief 添加模型追加到同步数据
    /// @param model
    void connectModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 断开模型同步数据
    /// @param model
    void disconnectModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 添加项目模型到同步数据
    /// @param project 项目
    void connectProject(QSharedPointer<KLProject> project);
    /// @brief 断开项目模型同步连接
    /// @param project
    void disconnectProject(QSharedPointer<KLProject> project);
    /// @brief 获取自定义minedata
    /// @param isNullCreate true，如果为空创建(一般是copy和cut时用)，否则不创建(使用的时候为false，如paste)
    /// @return
    CustomMineData *getCustomMineData(bool isNullCreate = false);
    /// @brief 获取当前treeindex names
    /// @return
    QStringList getTreeCurrIndexNames();
    /// @brief 获取表格水平表头列表
    /// @return
    QList<QString> getTableHeadList();
    /// @brief 打开输入框
    /// @param titleName 标题
    /// @param textLabel 提示信息
    /// @param initname 初始化输入框文字，如重命名名称为原先的名字
    /// @param inputText 返回的新text
    /// @return true点击的确定，否则是false
    bool OpenInputDialog(const QString &titleName, const QString &textLabel, const QString &initname,
                         QString &inputText, const QString &regStr = PROJDATAMNGKEY::REG_WORKSPACENAME);
    /// @brief 更新表格数据
    /// @param tableName 表格名(tree)
    /// @param headerList 表格title
    /// @param freezecols
    /// @param colfixedwidth
    void initTableInfo(const QString &tableName, const QStringList &headerList, const int &freezecols,
                       const int &colfixedwidth);
    void updateFilterContents(bool clearFilter = true);
    /// @brief 初始化滚动条
    /// @param freezecols 冻结列数
    /// @param colfixedwidth 列初始宽度
    void initHorizontalScrollbar(const int &freezecols, const int &colfixedwidth);
    /// @brief 设置表格列宽
    /// @param width 初始列宽
    void setTableColumFixedWidth(const int &width);
    int calculateScrollbarMax(const int &freezecol);
    int calculateTableMinWidth(const int &layoutwidth, const int &totalcols, int &max, int &visiblecol);
    void setScrollbarStatus();
    /// @brief 获取当前tree所在项目
    /// @return
    QSharedPointer<KLProject> getCurrentTreeProject();
    /// @brief 获取实例的原型Model
    /// @param project 项目
    /// @param modelInstance 实例
    /// @return
    QSharedPointer<Kcc::BlockDefinition::Model>
    getProtoModel(QSharedPointer<KLProject> project, QSharedPointer<Kcc::BlockDefinition::Model> modelInstance);
    /// @brief 通过模型名字获取模型
    /// @param modelName 模型名
    /// @param modelType 模型类型
    /// @param project 项目，为空则当前项目
    /// @return
    QSharedPointer<Kcc::BlockDefinition::Model> getProjectModelByName(const QString &modelName,
                                                                      const int &modelType = -1,
                                                                      QSharedPointer<KLProject> project = nullptr);
    /// @brief 编辑model，包括画板，模块，实例
    /// @param model 带编辑的model
    /// @param readOnly 是否只读
    /// @return
    bool EditModel(QSharedPointer<Kcc::BlockDefinition::Model> model, bool readOnly = false);
    // 表格相关右键操作
    /// @brief 打开模块
    void onTableOpenBlock();
    /// @brief 新建
    void onTableNew();
    /// @brief 拷贝
    void onTableCopy();
    /// @brief 粘贴
    void onTablePaste();
    /// @brief 删除
    void onTableDelete();
    /// @brief 编辑
    void onTableEdit();
    /// @brief 批量操作
    void onTableBatchModify();
    /// @brief 转换为真是modelindex
    /// @param indexlist 代理index列表
    /// @return
    QModelIndexList transToSourceIndexes(const QModelIndexList &indexlist);
    /// @brief 获取表格当前选中的原始index
    /// @return
    QModelIndex getTableSourceModelIndex();
    /// @brief 通过uuid和模型类型获取模型
    /// @param uuid 模型uuid
    /// @param modelType 模型类型
    /// @return
    QSharedPointer<Kcc::BlockDefinition::Model> getModelByUUIDTYPE(const QString &uuid, const int &modelType);
    /// @brief 检查是否重名
    /// @param project 项目
    /// @param model 模型
    /// @param name 新名字
    /// @param errorinfo 错误信息
    /// @return 可用返回true，否则返回false
    bool checkNameValid(QSharedPointer<KLProject> project, QSharedPointer<Kcc::BlockDefinition::Model> model,
                        const QString &name, QString &errorinfo);
    /// @brief 跳转到指定位置
    /// @param indexNames 从根节点到当前节点的列表。
    void JumptoPosition(const QStringList &indexNames);
    /// @brief 获取tree选中的index列表
    /// @return
    QModelIndexList getTreeSelectedIndexes();
    /// @brief 确认是否删除
    /// @param typeName 类型名
    /// @param contentName 类型对应的内容名
    /// @return true点击确认，否则返回false
    bool questionDelete(const QString &typeName, const QString &contentName);
    /// @brief 保存模型
    /// @param model
    /// @return
    bool saveModel(QSharedPointer<Kcc::BlockDefinition::Model> model);

protected:
    // ui相关
    Ui::ProjectManagerView ui;
    QLabel *m_rowlabel;
    CustomHorizontalScrollBar *m_horizontalScrollbar;
    CustomTreeModel *m_treeModel;
    CustomSortFilterProxyModel *m_pProxyModel;
    TableModelDataModel *m_pTableModel;
    QSharedPointer<ProjDataManager> m_pProjDataManager;
    PUndoStack m_pUndoStack;
    // 存储表格显示隐藏列数
    TableSetStruct m_tableSetStruct;
    bool m_DrawboardIsRuning;
    SyncModelData *m_syncModelData;
    bool m_isUpdateWidthOrScroll; // 正在修改宽度的时候，或者计算滚动条的时候，表头resize禁用。
};

#endif
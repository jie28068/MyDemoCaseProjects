#ifndef PROJECTMANAGERWIDGET_H
#define PROJECTMANAGERWIDGET_H

#pragma once
#include "CustomSortFilterProxyModel.h"
#include "CustomTreeModel.h"
#include "ManagerCommonWidget.h"
#include "TableModelDataModel.h"
#include "server/Base/IServerInterfaceBase.h"
#include "undocommand.h"
#include "undostackmanager.h"
#include <QMainWindow>
#include <QSharedPointer>
#include <QSpinBox>
#include <QTimer>

class ProjDataManager;
class KLProject;
using Kcc::NotifyStruct;
namespace Kcc {
namespace BlockDefinition {

class Model;
}
}
class ProjectManagerWidget : public ManagerCommonWidget
{
    Q_OBJECT
public:
    ProjectManagerWidget(QSharedPointer<ProjDataManager> pProjDataManager, QWidget *parent = nullptr);
    ~ProjectManagerWidget();

    virtual void initData() override;
    void JumpToReferenceModel(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void editExternalProject(const QString &projectAbsPath);
    const int getSortType();
    void setDemoINI(bool);
    bool getDemoINI();

public slots:
    virtual void onManagerClosed(int btn) override;
    virtual void onTreeClicked(const QModelIndex &index) override;
    virtual void onTreeContextMenu(const QPoint &point) override;
    virtual void onActionTriggered() override;
    virtual void onModelValueChanged(const QList<OldNewDataStruct> &modifylist) override;
    virtual void onPropertyManagerServerMsg(unsigned int code, const NotifyStruct &structinfo) override;
    virtual void onReceiveProjectManagerServerMsg(uint code, const NotifyStruct &param) override;
    virtual void onTableMenuEnd() override;
    virtual void onSelectedRowsChanged(const int &totalselectrow) override;
    void onActionTreeTriggered();
    void onActionTableTriggered();
    void onTimerOut();
    void onCreateWorkSpace();
    void onDeleteWorkSpace();
    void onExportWorkSpace(const QString &workspacename);
    void onCreateProj(const QString &workspacenamestr = "");
    void onDeleteProj();
    void onImportProj(const QString &workspace);
    void onExportProj(bool bActive = false);
    void onExportSimuResult();
    void onCreateModel(const QString &boardType = "");
    void onCreateCodeTemplate();
    void onDeleteModel();
    void onDeleteDeviceModel();
    void onDeleteSimuResult();
    void onCreateDeviceType();
    void onNewComplexInstance();
    void onNewBlockInstance(const QString &prototypeName = "");
    void onOpen();
    void onEdit();
    void onRename();
    void onCopy();
    void onCut();
    void onPaste();
    void onActiveClicked();
    void onRecord();
    void onTreeLevelUpDown(bool bup);
    void onFileCreateWorkSpace();
    void onFileCreateProj();
    void onFileImportWorkSpace();
    void onFileImportProj();
    void onFileImportDemoProj();
    void onFileExportWorkSpace();
    void onFileExportProj();
    void onMenuAddDrawBoard();
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onSettings();
    void onImportDeviceCSV();
    void onElementsConnectDeviceTypeCSV();

signals:
    void updateNetwork();
    /// @brief 项目激活状态改变
    /// @param oldActiveProject 上一个激活项目，如果没有则为null
    /// @param newActiveProj 新激活的项目，如果没有则为null
    void projectActiveStsChanged(QSharedPointer<KLProject> oldActiveProject, QSharedPointer<KLProject> newActiveProj);

protected:
    virtual void initUI() override;
    virtual void connectSignals() override;
    virtual void disconnectSignals() override;
    virtual void updateUI(const QStringList &indexNames) override;

protected:
    virtual void enterEvent(QEvent *event);

private:
    void updateAllActionSts(const QModelIndex &treeIndex);
    void updateTreeMenuSts(const QModelIndex &index);
    /// @brief 获取tree可编辑actionlist及新建和激活的文本
    /// @param index treeindex
    /// @param enableList 可以编辑的actionlist
    /// @param activeTxt 激活文本
    /// @param newText 新建文本
    void getTreeEnableActionList(const QModelIndex &index, QList<ProjActionManager::ActionType> &menuEnableList,
                                 QString &activeTxt, QString &newText);
    void updateTableData(const QModelIndex &index);
    QString getCurrentWorkSpaceName(const QString &msg);
    /// @brief 是否可以粘贴
    /// @param cmineData 粘贴内容
    /// @param modelType 模型类型，为-1则不指定模型类型
    /// @param prototype 指定原型名， 为空则不指定原型
    /// @param isInstance 是原型还是实例，true为实例
    /// @return
    bool canPaste(CustomMineData *cmineData, const int &modelType = -1, const QString &prototype = "",
                  bool isInstance = false);
    /// @brief 拷贝模型
    /// @param curProject
    /// @param srcProject
    /// @param srcmodel
    /// @return
    bool copyModelToProject(QSharedPointer<KLProject> curProject, QSharedPointer<KLProject> srcProject,
                            QSharedPointer<Kcc::BlockDefinition::Model> srcmodel);
    /// @brief 删除项目内的model
    /// @param project
    /// @param model
    /// @return
    bool deleteProjectModel(QSharedPointer<KLProject> project, QSharedPointer<Kcc::BlockDefinition::Model> model);

    /// @brief 激活项目相关处理
    /// @param workspacename 工作空间
    /// @param projname 项目名
    /// @param bactive 激活/取消激活
    void activeProjectHandle(const QString &workspacename, const QString &projname, bool bactive);
    /// @brief 是否存在当前类型的模块原型
    /// @param modelType 类型
    /// @return
    bool haveModelProto(const int &modelType);
    /// @brief 查找模块
    /// @param allBoards 所有画板
    /// @param boardName 画板名
    /// @param prototypeName 模块原型
    /// @param elmentsName 模块名
    /// @return
    QSharedPointer<Kcc::BlockDefinition::Model>
    findElecModel(QList<QSharedPointer<Kcc::BlockDefinition::Model>> &allBoards, const QString &boardName,
                  const QString &prototypeName, const QString &elmentsName);
    /// @brief 查找设备类型
    /// @param deviceList
    /// @param deviceName
    /// @return
    QSharedPointer<Kcc::BlockDefinition::DeviceModel>
    findDeviceModel(QList<QSharedPointer<Kcc::BlockDefinition::Model>> &deviceList, const QString &deviceName);
    /// @brief 设置电气模块的设备类型
    /// @param allBoards
    /// @param deviceList
    /// @param boardName
    /// @param prototypeName
    /// @param elmentsName
    /// @param deviceName
    /// @param needSaveBoards
    void setElecDeviceType(QList<QSharedPointer<Kcc::BlockDefinition::Model>> &allBoards,
                           QList<QSharedPointer<Kcc::BlockDefinition::Model>> &deviceList, const QString &boardName,
                           const QString &prototypeName, const QString &elmentsName, const QString &deviceName,
                           QList<QSharedPointer<Kcc::BlockDefinition::Model>> &needSaveBoards);

private:
    QTimer *m_pTimer;
};

#endif
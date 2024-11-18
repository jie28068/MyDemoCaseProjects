#ifndef NETWORKMODELMANAGERWIDGET_H
#define NETWORKMODELMANAGERWIDGET_H

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

using Kcc::NotifyStruct;

class ProjDataManager;
class KLProject;

class NetworkModelManagerWidget : public ManagerCommonWidget
{
    Q_OBJECT
public:
    NetworkModelManagerWidget(QSharedPointer<ProjDataManager> pProjDataManager, QWidget *parent = nullptr);
    ~NetworkModelManagerWidget();
    virtual void initData() override;
public slots:
    virtual void onManagerClosed(int btn) override;
    virtual void onTreeClicked(const QModelIndex &index) override;
    virtual void onActionTriggered() override;
    virtual void onModelValueChanged(const QList<OldNewDataStruct> &modifylist) override;
    virtual void onPropertyManagerServerMsg(unsigned int code, const NotifyStruct &structinfo) override;
    virtual void onReceiveProjectManagerServerMsg(uint code, const NotifyStruct &param) override;
    virtual void onSelectedRowsChanged(const int &totalselectrow) override;
    void onUpdateNetwork();
    void onProjectActiveStsChanged(QSharedPointer<KLProject> oldActiveProj, QSharedPointer<KLProject> newActiveProj);
    void onDeleteModel();
    void onAddModel(QSharedPointer<Kcc::BlockDefinition::Model> model);

protected:
    virtual void initUI() override;
    virtual void connectSignals() override;
    virtual void disconnectSignals() override;
    virtual void updateUI(const QStringList &indexNames) override;

protected:
    virtual void enterEvent(QEvent *event);
};

#endif
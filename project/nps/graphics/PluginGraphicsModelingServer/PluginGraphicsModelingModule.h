#ifndef PLUGINGRAPHICSMODELINGMODULE_H
#define PLUGINGRAPHICSMODELINGMODULE_H

#include "CoreLib/Module.h"
#include <QAction>

#include "CoreLib/ServerBase.h"
#include "ExportDrawingboardToFMU.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "IPluginGraphicModelingServer.h"
#include "KLWidgets/KMessageBox.h"
#include "Manager/ModelingServerMng.h"
#include "ToolsLib/Json/json.h"
#include <KLWidgets/KCustomDialog.h>

// ServerBase的命名空间
using namespace Kcc;
using namespace Kcc::PluginGraphicModeling;

class PluginGraphicsModelingModule : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginGraphicsModelingModule(QString moduleName);
    ~PluginGraphicsModelingModule();

    // Module
    virtual void init(KeyValueMap &params) override;
    virtual void unInit(KeyValueMap &saveParams) override;

    static void graphicsKernelLogHandler(QString msg, int level); // 默认日志



private:
    void initActions();
    void initTimer();
    void initServerNotify(); // 初始化服务通知

    void mainWigetAddAction(); // 将需要使用快捷键的Action 依附于主窗体

public slots:
    // 接收服务通知
    void onReceiveSimulationMessage(unsigned int, const NotifyStruct &); // 接收画板运行停止消息
    void onRecievePropertyManagerServerMsg(unsigned int code, const NotifyStruct &param);
    void onRecieveNetworkManagerMsg(unsigned int code, const NotifyStruct &param);
    void onRecevieProjectManagerServerMsg(uint code, const NotifyStruct &param); // 接受 项目管理器服务通知

    // Tool Action
    void onModifyDrawingBoard();
    void onOpenDataManagerView();
    void onSaveDrawingBoard();
    void onExportDrawingBoard();
    void onExportDrawingBoardToFMU();

    // 定时刷新action状态
    void refreshActionState();

    // 书签序列化，反序列化
    void onBookMarkSerialize(QVariantMap map);
    void onBookMarkDeserialize();
    /// @brief 通过书签打开画板
    void onBookMarkOpen();
    /// @brief 判断主窗口是否有该action
    /// @param widget
    /// @param action
    /// @return
    bool isActionExist(QWidget *widget, QAction *action);

    // 控制系统导出为FMU
    void onExportToFmuPbtnOKclicked();
    void onExportToFmuPbtnCancelclicked();

private:
    // Tool Action
    QAction *m_pModifyDrawingBoardAction;
    QAction *m_pSaveDrawingBoardAction;

    // 导出为FMU
    KCustomDialog *m_pPopExportToFMUDlg;
    ExportDrawingboardToFMU *m_pExportDrawingboardToFMU;
    QPushButton *m_pExportPbtnOK;
    QPushButton *m_pExportPbtnCancel;

    QTimer *m_actRefreshTimer;
    // 是否在运行中
    bool m_bOnRunning;
};

#endif // PLUGINGRAPHICSMODELINGMODULE_H

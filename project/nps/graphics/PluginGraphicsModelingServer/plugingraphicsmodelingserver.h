#ifndef PLUGINGRAPHICSMODELINGSERVER_H
#define PLUGINGRAPHICSMODELINGSERVER_H

#include "CoreLib/ServerBase.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "IPluginGraphicModelingServer.h"
#include "server/SimulationProcessServer/SimuVarTable.h"

// ServerBase的命名空间
using namespace Kcc;
using namespace Kcc::PluginGraphicModeling;

#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
using namespace Kcc::SimuNPSAdapter;

// 使用UI服务必须的头文件
#include "server/UILayout/IUIMainLayoutServer.h"

// 添加工作区必须的头文件
#include <server/UILayout/ICoreMainUIServer.h>

// UI服务命名空间
using namespace Kcc::CoreManUI_Layout;

class CanvasWidget;
class ModelWidget;
class PluginGraphicsModelingServer : public QObject, public ServerBase, public IPluginGraphicModelingServer
{
    Q_OBJECT
public:
    PluginGraphicsModelingServer();
    ~PluginGraphicsModelingServer();

    virtual void Init();
    virtual void UnInit();

    /// @brief 接口实现
public:
    virtual PDrawingBoardClass getDrawingBoardInfoById(const QString &boardUUID);
    virtual bool hasDrawingBoard(const QString &boardName);
    virtual bool hasDrawingBoardById(const QString &boardUUID);
    virtual QString getBoardNameByUUID(const QString &boardUUID);
    virtual QString getActiveDrawingBoardUUID();

    virtual bool openDrawingBoard(PModel pBoardModel, const QString &blockID = "");
    virtual bool createDrawingBoard(PModel pBoardModel);
    virtual bool deleteDrawingBoard(const QString &drawingBoardName);
    virtual void modifyUserDefineBlock(const QString &blockName);
    virtual int getBlockPrototypeNextIndex(const QString &prototypeName, PModel pBoardModel,
                                           const QString &blockUUID = "", bool useIndexZero = true);
    virtual void setDrawBoardModified(const QString &boardUUID);
    virtual QRectF getMinRect(QString &boardUUID); // 取能容纳当前画板所有模块的最小size
    virtual QStringList getAllOpenBoardUUID();
    virtual bool setDisplayAndHighlightModel(PModel boardModel, PModel blockModel, PModel subSystemModel = PModel());
    virtual void VerifyStatus(QList<PModel> boardModels);

public:
    QStringList getAllDrawingBoardNames() const;                  // 获取所有已打开画板的名称列表
    CanvasWidget *getCanvasByBoardUUID(const QString &boardUUID); // 通过UUID获取画板Widget
    CanvasWidget *getCanvasByBoardName(const QString &name);      // 通过名称获取画板Widget
    ModelWidget *getModelWidgetByuuid(const QString &rootUuid, QString &uuid); // 通过uuid获取modelwidget

    void drawingBoardLoaded(CanvasWidget *widget);             // 画板已经加载
    void drawingBoardClosed(const QString &blockUUID);         // 画板已经关闭
    void drawingBoardActived(const QString &blockUUID);        // 画板已经激活
    void drawingBoardNotActived(const QString &blockUUID);     // 画板已经隐藏
    void clearBoardWidgetModifyFlag(const QString &blockName); // 清除画板修改标志

    virtual bool getRunningStatus(); // 获取画板是否在运行。针对的是整个程序是否在运行画板，不是单独某一个画板

    QString getValidUserDefinedDrawBoardName(QString prefix = "", QString suffix = ""); // 获取可用的构造型画板名称

    void setBoardPowerFlowState(const QString &blockUUID, bool status); // 更新画板潮流计算参数显示状态

    void setRunStepVariablesData(Simu_Var::SimuVarBoard &boardValue); // 处理单步运行的变量显示
    Simu_Var::SimuVarBoard getRunVariable(QString &uuid); // 通过uuid获取画板单步运行返回的端口参数

protected:
signals:
    void activeDrawingBoardChanged(QString name);

private slots:
    void onReceiveHyperLink(unsigned int, const NotifyStruct &);
    void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param); // 接收仿真引擎消息
    void onWorkTabIndexChange(unsigned int code, const NotifyStruct &param); // 工作区页面切换，消息响应用

private:
    void updateActiveDrawingBoardUUID(QString uuid);
    void setRuningStatus(bool bRunning);
    QString getUUIDByBoardName(const QString &boardName); // 通过当前打开的画板名获取画板UUID
    void getSubSystemAllParentModels(PModel subSystem, QList<PModel> &modelList); // 获取子系统的全部上一级model

private:
    PICoreManUILayoutServer m_pUIServer;                   // UI服务
    PIServerInterfaceBase m_m_pUIServerIF;                 // UI服务，消息响应用
    PISimuNPSDataDictionaryServer m_pDataDictionaryServer; // 数据字典服务

    QMap<QString, CanvasWidget *> m_canvasWidgetMap; // 所有打开画板窗体,key为uuid
    QMap<QString, PModel> m_modelMap;                // 所有打开画板信息,key为uuid

    QString m_activeDrawingBoardUUID; // 当前激活画板uuid
    bool m_bRuningStatus;             // 是否运行状态

    QMap<QString, Simu_Var::SimuVarBoard> m_mapRunValue; // 单步仿真数据
};

typedef QSharedPointer<PluginGraphicsModelingServer> PPluginGraphicsModelingServer;

#endif // PLUGINGRAPHICSMODELINGSERVER_H

#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include "Global_GMS.h"
#include "server/SimulationProcessServer/SimuVarTable.h"
#include <QWidget>

class CanvasWidget;
class NavWidget;
class ModelWidget : public QWidget
{
    Q_OBJECT
public:
    ModelWidget(QWidget *parent = 0);
    ~ModelWidget();

    void initModelWidget(PModel pModel, CanvasWidget *canvasWidget);
    void refreshUI();

    PModel addBlockToView(QString prototypeName, QPointF centerPoint, bool needUndo = true,
                          QString defaultName = QString(), QString modelUUID = ""); // 添加模块到画板视图
    void addBlockToView(PModel pModel, bool needUndo = true);
    void deleteBlockFromView(PModel pModel);             // 删除模块
    void addConnectorToView(PBlockConnector pConnector); // 添加连接线
    void deleteConnector(QString connectorUUID);         // 删除连接线

    void scrollToBlock(const QString &strActiveBlockID);

    PModel getModel() { return m_model; }
    QSharedPointer<ICanvasView> getCanvasView() { return m_canvasView; }
    QSharedPointer<CanvasContext> getCanvasContext() { return m_canvasContext; }
    PBaseKernelInterface getKernelInterface() { return m_pKernelInterface; }

    QList<BlockStruct> getCanCreateBlockList();
    void setRunStepVariablesData(Simu_Var::SimuVarBoard &boardValue);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void initServerNotify(); // 初始化服务通知

    void setBlockState(QString prototypeName, Block::ModelState blockState, QString stateInfo);
    void setSingleBlockState(QString uuid, Block::ModelState blockState, QString stateInfo);

    void changeBlockState(QString blockUUID, PCanvasContext canvasCtx, QMap<QString, QVariant>::const_iterator iter);

    void updateModelOrder(QString boardUUID, QMap<QString, QVariant> orderParams, int &startIndex);

    void defaultAddDataDictionary(PModel model);
    // 返回画板类型
    QString getBroadType(int type);

private slots:
    // 接收服务通知
    void onReceiveSimulationMessage(unsigned int code, const NotifyStruct &param);     // 接收仿真引擎消息
    void onReceivePropertyServerMessage(unsigned int code, const NotifyStruct &param); // 接收属性服务消息
    void onRecieveCtrSysMsg(unsigned int code, const NotifyStruct &param); // 接受控制系统服务接口类，消息响应用
    void onRecieveComponentServerMsg(unsigned int code, const NotifyStruct &param); // 接受元件窗口消息
    void onRecieveNetworkManagerMsg(unsigned int code, const NotifyStruct &param); // 网络管理服务接口类，消息响应用
    void onRecieveDrawingBoardHelperServerMsg(unsigned int code, const NotifyStruct &param); // 电气画板校验
    void onReceiveProjectMngServerMsg(unsigned int code, const NotifyStruct &param); // 接收 项目管理服务通知

private:
    CanvasWidget *m_canvasWidget;
    PModel m_model;

    QSharedPointer<ICanvasView> m_canvasView;      // 画板
    QSharedPointer<CanvasContext> m_canvasContext; // 画板内容
    PBaseKernelInterface m_pKernelInterface;       // 画板核心接口类

    QVBoxLayout *m_vLoyout;
    QWidget *m_toolBar;
    NavWidget *m_nullWidget; // 空白导航栏，用于解决刷新布局时闪烁的问题
};

#endif
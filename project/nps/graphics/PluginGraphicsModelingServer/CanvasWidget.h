#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include "Global_GMS.h"
#include "NavWidget.h"
#include "server/SimulationProcessServer/SimuVarTable.h"
#include <QStackedWidget>

class ModelWidget;
class SearchBox;
struct searchListData;
class CanvasWidget : public BasicWorkareaContentWidget
{
    Q_OBJECT
    DEFINE_WORKAREA

public:
    CanvasWidget(QWidget *parent = 0);
    ~CanvasWidget();

    // 框架函数
    virtual void Save() override;
    virtual void UnSave() override;
    virtual bool init(const QMap<QString, QVariant> &paramMap) override;
    virtual QIcon icon() override;

    void openTopBoardModel(PModel pBoardModel); // 打开顶层画板model
    void openCurrentModel(PModel curModel);     // 打开当前层级model

    PModel addBlockToView(QString prototypeName, QPointF centerPoint, bool needUndo = true,
                          QString defaultName = QString(), QString modelUUID = ""); // 添加模块到画板视图
    void addBlockToView(PModel pModel, bool needUndo = true);                       // 添加模块
    void deleteBlockFromView(PModel pModel);                                        // 删除模块
    void addConnectorToView(PBlockConnector pConnector);                            // 添加连接线
    void deleteConnector(QString connectorUUID);                                    // 删除连接线

    void scrollToBlock(const QString &strActiveBlockID);
    void triggerContensChangedSlot(bool isTouchModified); // 画板已修改,是否触发*
    void boardNameChanged();                              // 画板已修改

    PModel getBoardModel() { return m_boardModel; } // 获取当前的画板Model

    void setRunningStatus(bool bRunning); // 设置运行状态，运行状态下画板不能操作。
    QList<BlockStruct> getCanCreateBlockList();

    void verifyStatus(); // 画板校验

    QRectF getMinRect(); // 取能容纳当前画板所有模块的最小size

    ModelWidget *getCurModelWidget();
    ModelWidget *getModelWidgetByUUid(const QString &uuid);
    ModelWidget *getTopModelWidget();

    void setNextConstructiveBoard(bool falg); // 进入下一级为构造型画板，画板不可操作

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void initUI(); // 初始化UI
    void initSignalSlot();
    void updataDataDictionary();

public slots:
    void onFullscreenChanged(bool bfull); // 全屏变化时
    void onModifyLayerProperty();         // 编辑属性页面时

    void onBookMarkArtboardSwitching(QString uuid); // 通过书签切换画板

    void onStackedWidgetChanged(int curIndex);

    void onSelectedBlock(searchListData &data);

public:
    NavWidget *m_navWidget;

private:
    QStackedWidget *m_stackedWidget;
    ModelWidget *m_topModelWidget; // 顶层画板Model窗口
    ModelWidget *m_curModelWidget; // 当前model窗口
    SearchBox *m_serachBox;        // 搜索窗口

    PModel m_boardModel;

    QString m_drawingBoardName;
    QString m_drawingBoardUuid;

    QSharedPointer<Kcc::ProjectManager::IProjectManagerServer> m_pProjectMngServer;

    bool m_isRunning; // 是否在运行
};

#endif // CANVASWIDGET_H

#pragma once

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "SimuNPSBusinessHooksServer.h"

using namespace Kcc::BlockDefinition;

// 连接线信息
struct ConnectInfo {
    PBlockConnector connector;
    bool isConnectDstBlock; // 是否连接的目的模块
    QString portUUID;

    ConnectInfo(PBlockConnector connector, bool isConnectDstBlock, QString portUUID)
        : connector(connector), isConnectDstBlock(isConnectDstBlock), portUUID(portUUID)
    {
    }
};

// 控制画板业务钩子服务类
class ControlCanvasHooksServer : public SimuNPSBusinessHooksServer
{
public:
    ControlCanvasHooksServer(QSharedPointer<CanvasContext> pcanvascontext,
                             QSharedPointer<Kcc::BlockDefinition::Model> model);

    virtual bool enableCreate(int cpsourcetype, QMap<QString, QSharedPointer<SourceProxy>> cpsourcemap);
    virtual bool enableCreate(int cpsourcetype, QSharedPointer<SourceProxy> cpsource);
    virtual bool isBlockCtrlCombined(QSharedPointer<SourceProxy> source);
    virtual void openModelBoard(SourceProxy *source);
    virtual bool checkNameValid(const QString &nameString);
    virtual void showWaringDialg(const QString &tips);
    virtual void showPropertyDialog();
    virtual bool isExistBlock(QSharedPointer<SourceProxy> source);
    virtual bool checkBlockState(QSharedPointer<SourceProxy> source);
    virtual void changeTextNameHint(const QString &oldname, const QString &newname);
    virtual bool isBlockCtrlUserCombined(QSharedPointer<SourceProxy> source);
    virtual bool sourceNameIsShowWhenSelectedChanged(bool isSelected);

    virtual bool isBlockCtrlCodeType(QSharedPointer<SourceProxy> source);

    virtual bool isShowRunningSort();

private:
private:
    /// *用于创建构造模块的撤销和重做
    QList<PModel> m_originalBlocks;             // 操作前最初的模块
    QList<PBlockConnector> m_originaConnectors; // 操作前最初的连接线
    QList<ConnectInfo> m_lstConnectInfo;        // 记录连接线关系，用于替换构造模块后的连接
    QString m_boardName;
    PModel m_blockCtrlCombined;
    QString m_blockUUID;
    ///
};

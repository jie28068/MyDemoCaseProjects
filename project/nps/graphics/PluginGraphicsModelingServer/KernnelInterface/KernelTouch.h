#pragma once

#include "BlockDelegate/BlockDelegateMng.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"

using namespace Kcc::BlockDefinition;

class BaseKernelInterface;
class KernelTouch : public QObject
{
    Q_OBJECT
public:
    KernelTouch(BaseKernelInterface *baseKernelInterface);
    ~KernelTouch();

    void initTouch(PModel pBoardModel, PCanvasContext pCanvasContext);

    PSourceProxy addBlockToView(PModel pModel);
    void deleteBlockFromeView(PModel pModel);
    PConnectorWireContext addConnectorToView(PBlockConnector pConnector);
    void deleteBlockConnectorToView(PBlockConnector pConnector) {};

private:
    void initBlockDelegate(PModel pBoardClass);               // 初始化模块业务代理
    void initKernelSignalSlot(PCanvasContext pCanvasContext); // 初始化画板信号槽

    // 绑定画板信号槽
    void connectCanvasSigSlot(PCanvasContext canvas);
    void connectSourceSigSlot(PSourceProxy source);
    void connectSourcePortSigSlot(PSourceProxy source); // 模块下所有端口
    void connectWireSigSlot(PConnectorWireContext wire);
    void connectPortSigSlot(PPortContext port); // 单一端口

    void handlePasteBlock(QString originalUUID, PModel &model); // 处理粘贴模块
    void removeBusSelectorBlockOutputPorts(PModel model); // 删除busselector输入端口的连接线时，清空该模块所有输出端口
    /// 判断当前画板层是否存在构造型
    bool DetermineWhetherStereotypeExist(PModel model);

public slots:
    /// <summary>
    /// 画板数据变更 同步到DrawBoardClass
    void onCanvasContextChanged(QString key, QVariant val); // 画板基础数据改变（不包括增加删除模块和模块连接线
    void onCanvasAddSourceProxy(QSharedPointer<SourceProxy> pSourceProxy); // 画板增加模块
    void onCanvaDeleteSourceProxy(QSharedPointer<SourceProxy> pSourceProxy);
    void onCanvaAddConnectorWire(QSharedPointer<ConnectorWireContext> pCWC); // 画板增加连接线
    void onCanvaDeleteConnectorWire(QSharedPointer<ConnectorWireContext> pCWC);
    void onCanvasLayerPropertyChanged(); // 画板图层属性变更

    void onCanvasSourceEdit(CanvasContext::SourceEditFlag flag, QString sourceUUID);

    void onSourceProxyChanged(QString key, QVariant val); // 模块基础数据改变（不包括增加/删除端口
    void onAddPortContext(QSharedPointer<PortContext> portCtx); // 增加端口
    void onDelPortContext(QSharedPointer<PortContext> portCtx); // 删除端口

    void onConnectorWireChanged();                   // 模块连接线改变
    void onConnectorSignalNameChanged(QString name); // 连接线信号名称变化（仅对连接了Bus类型模块时处理）

    void onBlockPortsChanged();          // 模块端口改变
    void onSourceGroupPositionChanged(); // 图元组位置变化

    /// </summary>

public:
    PBlockDelegateMng m_pBlockDelegateMng; // 模块代理管理

private:
    BaseKernelInterface *m_pBaseKernelInterface;
    PModel m_boardModel;
    PCanvasContext m_pCanvasContext;

    QMap<QString, PModel> m_backupBlockMap; // 备份删除的模块用于撤销删除还原模块数据,key为模块uuid

    bool m_notSyncBlock; // 不同步Block
};

typedef QSharedPointer<KernelTouch> PKernelTouch;

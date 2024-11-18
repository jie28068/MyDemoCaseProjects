#pragma once

#include "BusinessHooksServer.h"
#include "ConnectorWireContext.h"
#include "Global.h"
#include "GraphicsKernelDefinition.h"
#include "NPSPropertyManager.h"

#include <QDate>
#include <QScopedPointer>
#include <QSharedPointer>

class SourceProxy;
class BusinessHooksServer;
class CanvasContextPrivate;
class CanvasContextSerialize;
class GRAPHICSMODELINGKERNEL_EXPORT CanvasContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString uuid READ uuid WRITE setUUID)
    Q_PROPERTY(QSize size READ size WRITE setSize)
    Q_PROPERTY(QBoxLayout::Direction direction READ direction WRITE setDirection)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(CanvasContext::Type type READ type WRITE setType)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QPointF centerPos READ centerPos WRITE setCenterPos)
    Q_PROPERTY(QString version READ version)
    Q_PROPERTY(QString author READ author WRITE setAuthor)
    Q_PROPERTY(QString lastModifyTime READ getLastModifyTime WRITE setLastModifyTime)
    Q_PROPERTY(int gridSpace READ gridSpace WRITE setGridSpace)
    Q_PROPERTY(CanvasContext::GridFlag gridFlag READ gridFlag WRITE setGridFlag)
    Q_PROPERTY(CanvasContext::LayerFlag layerFlag READ layerFlag WRITE setLayerFlag)
    Q_PROPERTY(QString connectorWireAlgorithmName READ connectorWireAlgorithmName WRITE setConnectorWireAlgorithmName)
    Q_PROPERTY(bool showArrow READ showArrow WRITE setShowArrow)
    Q_PROPERTY(bool showConnectorWireSwitch READ showConnectorWireSwitch WRITE setShowConnectorWireSwitch)
    Q_PROPERTY(bool showArrow READ showArrow WRITE setShowArrow)
    Q_PROPERTY(int lineStyle READ lineStyle WRITE setLineStyle)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
    Q_PROPERTY(QColor lineSelectColor READ lineSelectColor WRITE setLineSelectColor)
    Q_PROPERTY(bool isShowPowerFlowData READ isShowPowerFlowData WRITE setIsShowPowerFlowData)
    Q_PROPERTY(bool isShowAlignLine READ isShowAlignLine WRITE setIsShowAlignLine)

    Q_ENUMS(LayerFlag)
    Q_ENUMS(Type)
    Q_ENUMS(GridFlag)
public:
    enum StatusFlag {
        kInvalid = 0,
        /// 画板窗口状态
        kActiveWindow = 1,
        /// 画板有图元被选中
        kItemSelectedStatus = 1 << 1,
        /// 当前画板仿真运行状态
        kSimulationRunningStatus = 1 << 2,
        /// 当前画板锁定状态
        kLockedStatus = 1 << 3,
        /// 画板激活状态
        kProjectActivate = 1 << 4,
        /// 所选中的图元可以被删除
        kItemCanDeleteStatus = 1 << 5,
        /// 进入下一级画板，且画板为构造型画板(画板只读状态)
        kNextConstructiveBorad = 1 << 6,
    };

    enum LayerFlag {
        kLayerNone = 0x00,
        kLayerPortFlag = 1 << 0,
        kLayerNameFlag = 1 << 1,
        kLayerNameTriphaseFlag = 1 << 2
    };

    enum Type {
        kInvalidType,
        kElectricalType,
        kControlSystemType,
        kUserDefinedType,
        kUserDefinedFrameType,
        kResultDisplayType,
        kElecUserDefinedType
    };

    enum GridFlag {
        kGridNoneFlag = 0,
        kGridShowFlag = 1,
        kGridAutoAlignmentFlag = 1 << 1,
        kGridShowDotGrid = 1 << 2,
    };

    enum SourceEditFlag {
        kSourceDoubleClick = 1,
        kSourceEdit = 1 << 1,
    };

    Q_ENUMS(SourceEditFlag)

    Q_ENUMS(StatusFlag)
    Q_DECLARE_FLAGS(StatusFlags, StatusFlag)
    Q_FLAG(StatusFlags)

    CanvasContext(QString uuid, QObject *parent = 0);
    ~CanvasContext();

    CanvasContext(const CanvasContext &other);
    CanvasContext &operator=(const CanvasContext &other);

    virtual QString name() const;
    virtual void setName(QString value);

    virtual QString uuid() const;
    virtual void setUUID(QString uuid);

    virtual QSize size() const;
    virtual void setSize(QSize size);

    virtual QBoxLayout::Direction direction() const;
    virtual void setDirection(QBoxLayout::Direction value);

    virtual QColor backgroundColor() const;
    virtual void setBackgroundColor(QColor color);

    virtual qreal scale() const;
    virtual void setScale(qreal scale);

    virtual CanvasContext::Type type() const;
    virtual void setType(CanvasContext::Type type);

    virtual QString description() const;
    virtual void setDescription(QString value);

    virtual QString author() const;
    virtual void setAuthor(QString value);

    virtual QPointF centerPos() const;
    virtual void setCenterPos(QPointF pos);

    virtual QString version() const;
    virtual void setVersion(QString version);

    virtual int gridSpace() const;
    virtual void setGridSpace(int value);

    virtual CanvasContext::GridFlag gridFlag() const;
    virtual void setGridFlag(CanvasContext::GridFlag flag);

    virtual CanvasContext::LayerFlag layerFlag() const;
    virtual void setLayerFlag(CanvasContext::LayerFlag flag);

    virtual QString connectorWireAlgorithmName() const;
    virtual void setConnectorWireAlgorithmName(QString algorithmName);

    virtual bool showArrow() const;
    virtual void setShowArrow(bool value);

    virtual bool showConnectorWireSwitch() const;
    virtual void setShowConnectorWireSwitch(bool value);

    virtual void addSourceProxy(QSharedPointer<SourceProxy> source);
    virtual bool deleteSourceProxy(QString uuid);

    virtual QMap<QString, QSharedPointer<SourceProxy>> getAllSource();
    virtual QSharedPointer<SourceProxy> getSource(QString uuid);

    virtual void addConnectWireContext(PConnectorWireContext ctx);
    virtual bool deleteConnectWireContext(QString uuid);

    virtual QMap<QString, QSharedPointer<ConnectorWireContext>> getAllConnectorWireContext();
    virtual PConnectorWireContext getConnectorWireContext(QString uuid);

    virtual void setBusinessHooksServer(BusinessHooksServer *server);
    virtual BusinessHooksServer *getBusinessHooksServer();

    virtual bool showConnectorWireBranchPoint();
    virtual void enableConnectorWireBranchPoint(bool enable);

    virtual bool getAllowOverlap();
    virtual void setAllowOverlap(bool enable);

    virtual bool highlightHomologousConnectorWire();
    virtual void setHighlightHomologousConnectorWire(bool show);

    virtual bool getShowNavigationPreview();
    virtual void setShowNavigationPreview(bool value);

    virtual bool getMagnify();
    virtual void setMagnify(bool value);

    virtual bool getScreenDrag();
    virtual void setScreenDrag(bool value);

    virtual bool getLocked();
    virtual void setLocked(bool value);

    virtual QString getLastModifyTime();
    virtual void setLastModifyTime(QString strTime);

    virtual int lineStyle();
    virtual void setLineStyle(int value);

    virtual QColor lineColor();
    virtual void setLineColor(QColor color);

    virtual QColor lineSelectColor();
    virtual void setLineSelectColor(QColor color);

    virtual bool isShowPowerFlowData();
    virtual void setIsShowPowerFlowData(bool value);

    virtual bool isShowAlignLine();
    virtual void setIsShowAlignLine(bool value);

    virtual void setInteractionMode(InteractionMode mode);
    virtual InteractionMode getInteractionMode();

    /// @brief 图元组位置改变触发"*"
    void canvasGroupPositionChanged();

    void setStatusFlag(StatusFlag flag, bool on = true);

    StatusFlags getStatusFlags();

    /// @brief 画板当前是否可以修改(锁定、未激活、仿真运行时都不能修改)
    /// @return
    bool canModify();

    /// @brief 获取输入模块的map
    QMap<int, QSharedPointer<SourceProxy>> getInputSourceMap();

    /// @brief 获取输出模块的map
    QMap<int, QSharedPointer<SourceProxy>> getOutputSourceMap();

    CanvasProperty &getCanvasProperty();

    void renameSourceUUID(QString oldUUID, QString newUUID);

    /// @brief 是否显示模型的仿真运行顺序
    /// @return
    bool isShowModelRunningSort();

    /// @brief 设置显示/隐藏模型的仿真运行顺序
    /// @param enable
    void enableShowModelRunningSort(bool enable);

    void setSubSystemClickPos(QPointF pos);
    QPointF subSystemClickPos();

    void setSenceRectf(QRectF rect);
    QRectF senceRectf();

    bool isSubsystemInitialized();

    void setSubsystemInitialized(bool init);

    bool isSupportSuspendedConnectorWire();

    void setSupportSuspendedConnectorWire(bool value);

public:
    virtual QSharedPointer<QByteArray> serialize();
    virtual void unSerialize(QSharedPointer<QByteArray> serializeBuffer);

private:
    void addInputAndOutputSource(PSourceProxy source);

    void deleteInputAndOutputSource(PSourceProxy source);

signals:
    void canvasContextChanged(QString, QVariant); // 画板属性变更  @parm1:属性名称 @parm2:属性值
    void canvasAddSourceProxy(QSharedPointer<SourceProxy> pSourceProxy);       // 画板增加模块
    void canvasDeleteSourceProxy(QSharedPointer<SourceProxy> pSourceProxy);    // 画板删除模块
    void canvasAddConnectorWire(QSharedPointer<ConnectorWireContext> pCWC);    // 画板删除连接线
    void canvasDeleteConnectorWire(QSharedPointer<ConnectorWireContext> pCWC); // 画板删除连接线
    void canvasLayerPropertyChanged();
    void canvasExtralInfoChanged(QString key); // 画板额外信息属性变化
    void canvasNavigationPreviewVisible(bool visible);
    void canvasShowPowerFlowFlagChanged(bool isShow);
    void canvasGroupPositionChange();

    /// @brief  资源编辑信号
    /// @param flag 标记鼠标双击或者是鼠标右键编辑触发
    /// @param sourceUUID 资源UUID
    void sourceProxyEdit(CanvasContext::SourceEditFlag flag, QString sourceUUID);

    /// @brief 画布交互模式发生变化
    /// @param mode
    void canvasInteractionModeChanged(QString canvasUUID, int newMode, int oldMode);

    /// @brief 画板状态变化
    /// @param flags
    void statusFlagsChanged(CanvasContext::StatusFlag flag);

    void gridFlagChanged(int oldFlags, int newFlags);

    void refreshCanvas();

    void showModelRunningSortChanged(bool show);

private:
    QScopedPointer<CanvasContextPrivate> dataPtr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CanvasContext::StatusFlags)

typedef QSharedPointer<CanvasContext> PCanvasContext;

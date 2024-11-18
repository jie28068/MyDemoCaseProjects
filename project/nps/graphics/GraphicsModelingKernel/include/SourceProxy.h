#pragma once

#include "NPSPropertyManager.h"

class SourceProxySerialize;
class PortContext;
class SourceProxyPrivate;
class CanvasContext;

class GRAPHICSMODELINGKERNEL_EXPORT SourceProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString canvasName READ canvasName WRITE setCanvasName)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString uuid READ uuid WRITE setUUID)
    Q_PROPERTY(QString prototypeName READ prototypeName WRITE setPrototypeName)
    Q_PROPERTY(QString moduleType READ moduleType WRITE setModuleType)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QSizeF size READ size WRITE setSize)
    Q_PROPERTY(QTransform scaleTransform READ scaleTransform WRITE setScaleTransform)
    Q_PROPERTY(QString state READ state WRITE setState)
    Q_PROPERTY(QString statetips READ statetips WRITE setStatetips)
    Q_PROPERTY(QString prototypeName_CHS READ prototypeName_CHS WRITE setPrototypeName_CHS)

public:
    SourceProxy(QObject *parent = 0);
    ~SourceProxy();

    SourceProxy(const SourceProxy &other);

    enum CreateType {
        kCreateNormal = 0, // 常规创建(比如画板拖拽)
        kCreateCopy,       // 通过复制的形式创建
        kCreateCut,        // 通过剪切的方式创建
        kCreateRevert,     // 通过撤销重做时创建，比如删除之后撤销
        kCreateCtrlCopy,   // 通过按住ctrl鼠标拖动复制
    };

    virtual QString canvasName() const;
    virtual void setCanvasName(QString name);

    virtual QString name() const;
    virtual void setName(QString name);

    virtual QString uuid() const;
    virtual void setUUID(QString id);

    virtual QString originalUUID() const;
    virtual void setOriginalUUID(QString id);

    virtual QString prototypeName() const;
    virtual void setPrototypeName(QString prototypeName);

    virtual QString moduleType() const;
    virtual void setModuleType(QString moduleType);

    virtual QPointF pos() const;
    virtual void setPos(QPointF pos);

    virtual QSizeF size() const;
    virtual void setSize(QSizeF size);

    virtual QTransform scaleTransform() const;
    virtual void setScaleTransform(QTransform transform);

    virtual QTransform rotateTransform() const;
    virtual void setRotateTransform(QTransform transform);

    virtual QTransform translateTransform() const;
    virtual void setTranslateTransform(QTransform transform);

    virtual QTransform xAxisFlipTransform() const;
    virtual void setxAxisFlipTransform(QTransform transform);

    virtual QTransform yAxisFlipTransform() const;
    virtual void setyAxisFlipTransform(QTransform transform);

    virtual QTransform getCombineTransform();

    virtual QString state() const;
    virtual void setState(QString state);

    virtual QString statetips() const;
    virtual void setStatetips(QString tips);

    // virtual QStringList svgImageList() const;
    // virtual void setSvgImageList(QStringList svgImageList);

    QVariantMap svgImageDatas() const;
    void setSvgImageDatas(const QVariantMap &svgImageDatas);

    bool isSupportAddOrDelPort() const;
    void setIsSupportAddOrDelPort(bool isSupport);

    bool isHadUpperLimitation() const;
    void setIsHadUpperLimitation(bool isSupport);

    bool isHadLowerLimitation() const;
    void setIsHadLowerLimitation(bool isSupport);

    QVariantList listNameDefaultAnchor() const;
    void setListNameDefaultAnchor(QVariantList listNameAnchor);

    QMap<int, QPointF> nameAngleOffset() const;
    void setNameAngleOffset(QMap<int, QPointF> angleOffset);

    QColor getStateColor(); // 获取当前状态的图元颜色值

    QSharedPointer<PortContext> getPortContextByID(QString uuid);   // 根据端口ID获取端口
    QSharedPointer<PortContext> getPortContextByName(QString name); // 根据端口名称获取端口
    QList<QSharedPointer<PortContext>> portList();                  // 获取模块下所有端口

    void resetPortList(QList<QSharedPointer<PortContext>> portContextList);
    void addPortContext(QSharedPointer<PortContext> portContext); // 增加端口
    void delPortContext(QSharedPointer<PortContext> portContext); // 删除端口

    void triggerMouseDoubleClick(); // 触发鼠标双击模块

    void emitEditProperty(); // 发送 编辑属性信号

    void emitOtherChange(); // 发送 其他属性变更信号，如电气属性DC、AC

    virtual QMap<QString, QVariant> &getAnnotationMap();
    virtual void resetAnnotationMap(QMap<QString, QVariant> &value);
    virtual void setAnnotationProperty(QString key, QVariant value); // 设置注解模块属性值

    virtual QMap<QString, QVariant> getDataPointMap();
    virtual void setDataPointMap(QString key, QVariant value);
    virtual void reSetDataPointMap(QMap<QString, QVariant> &value);

    void setBadgeMsg(const QString &msg);

    QString getBadgeMsg();

    void setRunVariableDataMap(QMap<QString, QMap<QString, QVariant>> &dataMap);

    QTransform getSRTTransform();

    /// @brief 修改端口中sourceuuid
    void resetPortSourceUUid();

    SourceProperty &getSourceProperty();

    void setSourceCreateType(CreateType type);

    SourceProxy::CreateType getSourceCreateType();

    QString debugMsg();

    virtual QString prototypeName_CHS() const;
    virtual void setPrototypeName_CHS(QString prototypeName);

    void setReadyDeleteFlag(bool isReadyDelete);
    bool getReadyDeleteFlag();

    QString getPrototypeReadaleName();
    void setPrototypeReadableName(const QString &name);

    QString getCommentState();
    void setCommentState(const QString &state);

public:
    void serialize(SourceProxySerialize *serialize);
    void unSerialize(SourceProxySerialize *serialize);

    void attachToCanvasContext(CanvasContext *canvasContext);

signals:
    void sourceChange(QString, QVariant); // 模块属性变更  @parm1:属性名称 @parm2:属性值

    void sigAddPortContext(QSharedPointer<PortContext> portCtx); // 模块增加端口
    void sigDelPortContext(QSharedPointer<PortContext> portCtx); // 模块删除端口
    void sigResetPortList(int sizeOffset);                       // 端口重置

    /// @brief 显示资源属性信号，用于在画板中双击的时候，弹出属性对话框
    void mouseDoubleClick();
    void editProperty(); // 编辑属性
    void runVariableDataChange(QMap<QString, QMap<QString, QVariant>>);

    void badgeMsgChanged(const QString &msg);

private:
    QScopedPointer<SourceProxyPrivate> dataPtr;
};

typedef QSharedPointer<SourceProxy> PSourceProxy;

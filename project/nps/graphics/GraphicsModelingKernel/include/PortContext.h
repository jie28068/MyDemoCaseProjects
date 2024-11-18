#pragma once
#include "Global.h"
#include "GraphicsKernelDefinition.h"
#include <QScopedPointer>

#define PortInputType "input"
#define PortOutputType "output"

class PortContextSerialize;

class PortContextPrivate;
class GRAPHICSMODELINGKERNEL_EXPORT PortContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceUUID READ sourceUUID WRITE setSourceUUID)
    Q_PROPERTY(QString type READ type WRITE setType)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString uuid READ uuid);
    Q_PROPERTY(QSize size READ size WRITE setSize)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(bool isAbsolutePos READ isAbsolutePos WRITE setIsAbsolutePos)
    Q_PROPERTY(bool supportMultipConnectorWire READ supportMultipConnectorWire WRITE setSupportMultipConnectorWire)
    Q_PROPERTY(Qt::AnchorPoint anchorPoint READ anchorPoint WRITE setAnchorPoint)
    Q_PROPERTY(bool isShowName READ isShowName WRITE setIsShowName)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName)

public:
    PortContext(QString sourceUUID, QString uuid, QObject *parent = 0);
    ~PortContext();

    PortContext(const PortContext &other);

    PortContext &operator=(const PortContext &other);

    QString sourceUUID() const;
    void setSourceUUID(QString value);

    QString originalPortUUID();
    void setOriginalPortUUID(QString value);

    QString type() const;
    void setType(QString type);

    QString name() const;
    void setName(QString name);

    QString uuid() const;

    void setUUID(const QString &value);

    QSize size() const;
    void setSize(QSize size);

    QPointF pos() const;
    void setPos(QPointF pos);

    bool isAbsolutePos() const;
    void setIsAbsolutePos(bool value);

    bool supportMultipConnectorWire() const;
    void setSupportMultipConnectorWire(bool value);

    Qt::AnchorPoint anchorPoint() const;
    void setAnchorPoint(Qt::AnchorPoint anchorPoint);

    bool isShowName() const;
    void setIsShowName(bool value);

    QString displayName() const;
    void setDisplayName(QString value);

    void serialize(PortContextSerialize *portSerialize);

    void unSerialize(PortContextSerialize *portSerialize);

    void setAngle(int angle);

    int getAngle() const;

signals:
    void portChange();
    void onPortHideOrShow(bool value);
    void portNameChanged(QString portName);

private:
    QScopedPointer<PortContextPrivate> dataPtr;
};
typedef QSharedPointer<PortContext> PPortContext;

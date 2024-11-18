#pragma once
#include "Global.h"

#include <QScopedPointer>

class ConnectorWireContextSerialize;
class SourceProxy;
class ConnectorWireContextPrivate;

class GRAPHICSMODELINGKERNEL_EXPORT ConnectorWireContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUUID)
    Q_PROPERTY(bool autoCalculation READ autoCalculation WRITE setAutoCalculation)
    Q_PROPERTY(QPolygonF points READ points WRITE setPoints)
    Q_PROPERTY(bool connected READ connected WRITE setConnected)
    Q_PROPERTY(QSharedPointer<SourceProxy> srcSource READ srcSource WRITE setSrcSource)
    Q_PROPERTY(QSharedPointer<SourceProxy> dstSource READ dstSource WRITE setDstSource)
    Q_PROPERTY(QString srcPortID READ srcPortID WRITE setSrcPortID)
    Q_PROPERTY(QString dstPortID READ dstPortID WRITE setDstPortID)

public:
    ConnectorWireContext(QString uuid, QObject *parent = 0);
    ~ConnectorWireContext();

    ConnectorWireContext(const ConnectorWireContext &other);

    ConnectorWireContext &operator=(const ConnectorWireContext &other);

    QString uuid() const;
    void setUUID(QString id);

    QString originalUUID() const;
    void setOriginalUUID(QString id);

    bool autoCalculation() const;
    void setAutoCalculation(bool value);

    QPolygonF points() const;
    void setPoints(QPolygonF points);

    bool connected() const;
    void setConnected(bool value);

    QSharedPointer<SourceProxy> srcSource() const;
    void setSrcSource(QSharedPointer<SourceProxy> source);

    QSharedPointer<SourceProxy> dstSource() const;
    void setDstSource(QSharedPointer<SourceProxy> source);

    QString srcPortID() const;
    void setSrcPortID(QString value);

    QString dstPortID() const;
    void setDstPortID(QString value);

    QPointF pos() const;
    void setPos(QPointF pos);

    void triggerMouseDoubleClick();

    void setSignalName(QString name);
    QString signalName();

    void serialize(ConnectorWireContextSerialize *connectorSerialize);
    void unSerialize(ConnectorWireContextSerialize *connectorSerialize);

signals:
    void ConnectorWireChange(); // 连接线变更

    void mouseDoubleClick();

    void connectedChanged(bool connected);

    void signalNameChanged(QString name);

    void pointsUpdated(QPolygonF points);

private:
    QScopedPointer<ConnectorWireContextPrivate> dataPtr;
};

typedef QSharedPointer<ConnectorWireContext> PConnectorWireContext;
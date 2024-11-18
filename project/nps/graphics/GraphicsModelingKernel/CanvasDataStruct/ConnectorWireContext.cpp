#include "ConnectorWireContext.h"
#include "ConnectorWireContext.pb.h"
#include "ConnectorWireContextPrivate.h"
#include "SourceProxy.h"

#include <QDebug>

ConnectorWireContext::ConnectorWireContext(QString uuid, QObject *parent) : QObject(parent)
{
    dataPtr.reset(new ConnectorWireContextPrivate());
    dataPtr->uuid = uuid;
}

ConnectorWireContext::~ConnectorWireContext() { }

ConnectorWireContext::ConnectorWireContext(const ConnectorWireContext &other)
{
    ConnectorWireContextPrivate *otherDataPtr = other.dataPtr.data();
    dataPtr.reset(new ConnectorWireContextPrivate(*otherDataPtr));
}

ConnectorWireContext &ConnectorWireContext::operator=(const ConnectorWireContext &other)
{
    ConnectorWireContextPrivate *otherDataPtr = other.dataPtr.data();
    dataPtr.reset(new ConnectorWireContextPrivate(*otherDataPtr));
    return *this;
}

QString ConnectorWireContext::uuid() const
{
    return dataPtr->uuid;
}

void ConnectorWireContext::setUUID(QString id)
{
    dataPtr->uuid = id;

    emit ConnectorWireChange();
}

QString ConnectorWireContext::originalUUID() const
{
    return dataPtr->originalUUID;
}

void ConnectorWireContext::setOriginalUUID(QString id)
{
    if (dataPtr->originalUUID == id)
        return;

    dataPtr->originalUUID = id;
}

bool ConnectorWireContext::autoCalculation() const
{
    return dataPtr->autoCalculation;
}

void ConnectorWireContext::setAutoCalculation(bool value)
{
    if (dataPtr->autoCalculation == value) {
        return;
    }
    dataPtr->autoCalculation = value;

    emit ConnectorWireChange();
}

QPolygonF ConnectorWireContext::points() const
{
    return dataPtr->points;
}

void ConnectorWireContext::setPoints(QPolygonF points)
{
    if (points == dataPtr->points) {
        return;
    }
    dataPtr->points = points;
    emit pointsUpdated(points);
}

bool ConnectorWireContext::connected() const
{
    return dataPtr->connected;
}

void ConnectorWireContext::setConnected(bool value)
{
    if (dataPtr->connected == value) {
        return;
    }
    dataPtr->connected = value;

    emit ConnectorWireChange();
    emit connectedChanged(dataPtr->connected);
}

QSharedPointer<SourceProxy> ConnectorWireContext::srcSource() const
{
    return dataPtr->srcSource;
}

void ConnectorWireContext::setSrcSource(QSharedPointer<SourceProxy> source)
{
    dataPtr->srcSource = source;

    emit ConnectorWireChange();
}

QSharedPointer<SourceProxy> ConnectorWireContext::dstSource() const
{
    return dataPtr->dstSource;
}

void ConnectorWireContext::setDstSource(QSharedPointer<SourceProxy> source)
{
    dataPtr->dstSource = source;

    emit ConnectorWireChange();
}

QString ConnectorWireContext::srcPortID() const
{
    return dataPtr->srcPortUUID;
}

void ConnectorWireContext::setSrcPortID(QString value)
{
    dataPtr->srcPortUUID = value;

    emit ConnectorWireChange();
}

QString ConnectorWireContext::dstPortID() const
{
    return dataPtr->dstPortUUID;
}

void ConnectorWireContext::setDstPortID(QString value)
{
    dataPtr->dstPortUUID = value;

    emit ConnectorWireChange();
}

QPointF ConnectorWireContext::pos() const
{
    return dataPtr->pos;
}

void ConnectorWireContext::setPos(QPointF pos)
{
    dataPtr->pos = pos;
}

void ConnectorWireContext::triggerMouseDoubleClick()
{
    emit mouseDoubleClick();
}

void ConnectorWireContext::setSignalName(QString name)
{
    if (dataPtr->signalName == name) {
        return;
    }
    dataPtr->signalName = name;
    emit signalNameChanged(name);
}

QString ConnectorWireContext::signalName()
{
    return dataPtr->signalName;
}

void ConnectorWireContext::serialize(ConnectorWireContextSerialize *connectorSerialize)
{
    if (!connectorSerialize) {
        return;
    }
    connectorSerialize->set_uuid(dataPtr->uuid.toUtf8().data());
    connectorSerialize->set_auto_calculation(dataPtr->autoCalculation);
    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->points;
        connectorSerialize->set_points(buffer.data(), buffer.length());
    }
    connectorSerialize->set_connected(dataPtr->connected);
    auto srcSource = connectorSerialize->mutable_src_source();
    if (dataPtr->srcSource) {
        dataPtr->srcSource->serialize(srcSource);
    }

    auto dstSource = connectorSerialize->mutable_dst_source();
    if (dataPtr->dstSource) {
        dataPtr->dstSource->serialize(dstSource);
    }

    connectorSerialize->set_src_port_uuid(dataPtr->srcPortUUID.toUtf8().data());
    connectorSerialize->set_dst_port_uuid(dataPtr->dstPortUUID.toUtf8().data());

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->pos;
        connectorSerialize->set_pos(buffer.data(), buffer.length());
    }
    connectorSerialize->set_signal_name(dataPtr->signalName.toUtf8().data());
}

void ConnectorWireContext::unSerialize(ConnectorWireContextSerialize *connectorSerialize)
{
    if (!connectorSerialize) {
        return;
    }
    dataPtr->uuid = QString::fromUtf8(connectorSerialize->uuid().c_str());
    dataPtr->autoCalculation = connectorSerialize->auto_calculation();

    {
        QByteArray buffer(connectorSerialize->points().c_str(), connectorSerialize->points().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->points;
        dataPtr->points = dataPtr->points.toPolygon();
    }

    dataPtr->connected = connectorSerialize->connected();
    auto serializeSrcSource = connectorSerialize->src_source();
    dataPtr->srcSource = PSourceProxy(new SourceProxy());
    dataPtr->srcSource->unSerialize(&serializeSrcSource);

    auto serializeDstSource = connectorSerialize->dst_source();
    dataPtr->dstSource = PSourceProxy(new SourceProxy());
    dataPtr->dstSource->unSerialize(&serializeDstSource);

    dataPtr->srcPortUUID = QString::fromUtf8(connectorSerialize->src_port_uuid().c_str());
    dataPtr->dstPortUUID = QString::fromUtf8(connectorSerialize->dst_port_uuid().c_str());
    {
        QByteArray buffer(connectorSerialize->pos().c_str(), connectorSerialize->pos().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->pos;
    }
    dataPtr->signalName = QString::fromUtf8(connectorSerialize->signal_name().c_str());
}

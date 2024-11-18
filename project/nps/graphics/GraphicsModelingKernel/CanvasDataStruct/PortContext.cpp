#include "PortContext.h"
#include "PortContext.pb.h"
#include "PortContextPrivate.h"

PortContext::PortContext(QString sourceUUID, QString uuid, QObject *parent) : QObject(parent)
{
    dataPtr.reset(new PortContextPrivate());
    dataPtr->sourceUUID = sourceUUID;
    dataPtr->uuid = uuid;
}

PortContext::~PortContext() { }

PortContext::PortContext(const PortContext &other)
{
    PortContextPrivate *otherDataPtr = other.dataPtr.data();
    dataPtr.reset(new PortContextPrivate(*otherDataPtr));
}

PortContext &PortContext::operator=(const PortContext &other)
{
    PortContextPrivate *otherDataPtr = other.dataPtr.data();
    dataPtr.reset(new PortContextPrivate(*otherDataPtr));
    return *this;
}

QString PortContext::sourceUUID() const
{
    return dataPtr->sourceUUID;
}

void PortContext::setSourceUUID(QString value)
{
    dataPtr->sourceUUID = value;
}

QString PortContext::originalPortUUID()
{
    return dataPtr->originPortUUID;
}

void PortContext::setOriginalPortUUID(QString value)
{
    dataPtr->originPortUUID = value;
}

QString PortContext::type() const
{
    return dataPtr->type;
}

void PortContext::setType(QString type)
{
    dataPtr->type = type;
}

QString PortContext::name() const
{
    return dataPtr->name;
}

void PortContext::setName(QString name)
{
    if (dataPtr->name == name) {
        return;
    }

    dataPtr->name = name;
    emit portNameChanged(name);
}

QString PortContext::uuid() const
{
    return dataPtr->uuid;
}

void PortContext::setUUID(const QString &value)
{
    dataPtr->uuid = value;
}

QSize PortContext::size() const
{
    return dataPtr->size;
}

void PortContext::setSize(QSize size)
{
    dataPtr->size = size;
}

QPointF PortContext::pos() const
{
    return dataPtr->pos;
}

void PortContext::setPos(QPointF pos)
{
    dataPtr->pos = pos;
    emit portChange();
}

bool PortContext::isAbsolutePos() const
{
    return dataPtr->isAbsolutePos;
}

void PortContext::setIsAbsolutePos(bool value)
{
    dataPtr->isAbsolutePos = value;
}

bool PortContext::supportMultipConnectorWire() const
{
    return dataPtr->supportMultipConnectorWire;
}

void PortContext::setSupportMultipConnectorWire(bool value)
{
    dataPtr->supportMultipConnectorWire = value;
}

Qt::AnchorPoint PortContext::anchorPoint() const
{
    return dataPtr->anchorPoint;
}

void PortContext::setAnchorPoint(Qt::AnchorPoint anchor)
{
    dataPtr->anchorPoint = anchor;
}

bool PortContext::isShowName() const
{
    return dataPtr->isShowName;
}
void PortContext::setIsShowName(bool value)
{
    if (dataPtr->isShowName == value) {
        return;
    }
    dataPtr->isShowName = value;
    emit onPortHideOrShow(value);
    emit portChange();
}

QString PortContext::displayName() const
{
    return dataPtr->displayName;
}
void PortContext::setDisplayName(QString value)
{

    dataPtr->displayName = value;
}

void PortContext::serialize(PortContextSerialize *portSerialize)
{
    if (!portSerialize) {
        return;
    }
    portSerialize->set_source_uuid(dataPtr->sourceUUID.toUtf8().data());
    portSerialize->set_type(dataPtr->type.toUtf8().data());
    portSerialize->set_name(dataPtr->name.toUtf8().data());
    portSerialize->set_uuid(dataPtr->uuid.toUtf8().data());

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->size;
        portSerialize->set_size(buffer.data(), buffer.length());
    }

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->pos;
        portSerialize->set_pos(buffer.data(), buffer.length());
    }

    portSerialize->set_is_absolute_pos(dataPtr->isAbsolutePos);
    portSerialize->set_support_multip_connectorwire(dataPtr->supportMultipConnectorWire);
    portSerialize->set_anchor_point(dataPtr->anchorPoint);
    portSerialize->set_is_show_name(dataPtr->isShowName);
    portSerialize->set_display_name(dataPtr->displayName.toUtf8().data());

    {
        QByteArray buffer;
        QDataStream outStream(&buffer, QIODevice::WriteOnly);
        outStream << dataPtr->extraInfoMap;
        portSerialize->set_extra_info_map(buffer.data(), buffer.length());
    }

    portSerialize->set_angle(dataPtr->angle);

    portSerialize->set_original_port_uuid(dataPtr->originPortUUID.toUtf8().data());
}

void PortContext::unSerialize(PortContextSerialize *portSerialize)
{
    if (!portSerialize) {
        return;
    }
    dataPtr->sourceUUID = QString::fromUtf8(portSerialize->source_uuid().c_str());
    dataPtr->type = QString::fromUtf8(portSerialize->type().c_str());
    dataPtr->name = QString::fromUtf8(portSerialize->name().c_str());
    dataPtr->uuid = QString::fromUtf8(portSerialize->uuid().c_str());

    {
        QByteArray buffer(portSerialize->size().c_str(), portSerialize->size().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->size;
        if(dataPtr->size == QSize(6,6))
        {
            dataPtr->size = QSize(GKD::PORT_SIZE, GKD::PORT_SIZE);
        }
    }

    {
        QByteArray buffer(portSerialize->pos().c_str(), portSerialize->pos().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->pos;
    }

    dataPtr->isAbsolutePos = portSerialize->is_absolute_pos();
    dataPtr->supportMultipConnectorWire = portSerialize->support_multip_connectorwire();
    dataPtr->anchorPoint = (Qt::AnchorPoint)portSerialize->anchor_point();
    dataPtr->isShowName = portSerialize->is_show_name();
    dataPtr->displayName = QString::fromUtf8(portSerialize->display_name().c_str());

    {
        QByteArray buffer(portSerialize->extra_info_map().c_str(), portSerialize->extra_info_map().length());
        QDataStream inStream(&buffer, QIODevice::ReadWrite);
        inStream >> dataPtr->extraInfoMap;
    }

    dataPtr->angle = portSerialize->angle();

    dataPtr->originPortUUID = portSerialize->original_port_uuid().c_str();
}

void PortContext::setAngle(int angle)
{
    dataPtr->angle = angle;
}

int PortContext::getAngle() const
{
    return dataPtr->angle;
}

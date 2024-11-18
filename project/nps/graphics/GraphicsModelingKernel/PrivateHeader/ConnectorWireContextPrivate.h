#pragma once

#include "SourceProxy.h"

#include <QPolygonF>
#include <QSharedPointer>
#include <QString>

class SourceProxy;

class ConnectorWireContextPrivate
{
public:
    ConnectorWireContextPrivate();

    ConnectorWireContextPrivate(const ConnectorWireContextPrivate &other);

    /// @brief 连接线唯一标识符
    QString uuid;
    /// @brief 原始UUID,资源可以被拷贝粘贴。该值保存复制的原资源id
    QString originalUUID;
    /// @brief 连接线是否需要自动计算坐标点
    bool autoCalculation;
    /// @brief 连接线在场景中的坐标点
    QPolygonF points;
    /// @brief 连接线是否连通(连接线上有开关)
    bool connected;
    /// @brief 连接线的源Source
    QSharedPointer<SourceProxy> srcSource;
    /// @brief 连接线的目的Source
    QSharedPointer<SourceProxy> dstSource;
    /// @brief 连接线上源Source的端口uuid
    QString srcPortUUID;
    /// @brief 连接线上目的Souce的端口uuid
    QString dstPortUUID;
    /// @brief 连接线在场景中的坐标
    QPointF pos;
    /// @brief 连接线关联的信号名称
    QString signalName;
};

ConnectorWireContextPrivate::ConnectorWireContextPrivate()
    : autoCalculation(true),
      points(QPolygonF()),
      connected(true),
      srcSource(QSharedPointer<SourceProxy>()),
      dstSource(QSharedPointer<SourceProxy>()),
      srcPortUUID(""),
      dstPortUUID(""),
      pos(QPointF(0, 0)),
      signalName("")
{
}

ConnectorWireContextPrivate::ConnectorWireContextPrivate(const ConnectorWireContextPrivate &other)
{
    uuid = other.uuid;
    autoCalculation = other.autoCalculation;
    points = other.points;
    connected = other.connected;
    if (!other.srcSource.isNull()) {
        srcSource = QSharedPointer<SourceProxy>(new SourceProxy(*other.srcSource.data()));
    } else {
        srcSource = QSharedPointer<SourceProxy>();
    }

    if (!other.dstSource.isNull()) {
        dstSource = QSharedPointer<SourceProxy>(new SourceProxy(*other.dstSource.data()));
    } else {
        dstSource = QSharedPointer<SourceProxy>();
    }

    srcPortUUID = other.srcPortUUID;
    dstPortUUID = other.dstPortUUID;
    signalName = other.signalName;
}

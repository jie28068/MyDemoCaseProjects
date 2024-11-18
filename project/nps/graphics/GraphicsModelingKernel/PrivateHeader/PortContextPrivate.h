#pragma once
#include <QPointF>
#include <QSize>
#include <QString>

#include "Utility.h"

class PortContextPrivate
{
public:
    PortContextPrivate();

    PortContextPrivate(const PortContextPrivate &other);

    /// @brief 端口所属的资源唯一标识符
    QString sourceUUID;
    /// @brief 端口类型(input、output)
    QString type;
    /// @brief 名称
    QString name;
    /// @brief 端口唯一标识符
    QString uuid;
    /// @brief 端口图元的大小
    QSize size;
    /// @brief 端口图元在资源图元中的位置坐标
    QPointF pos;
    /// @brief 端口图元坐标是否是相对值(0-1.0)
    bool isAbsolutePos;
    /// @brief 端口是否支持同时连接多个其他端口
    bool supportMultipConnectorWire;
    /// @brief 端口在图元的锚点方向（ AnchorLeft,AnchorRight,AnchorTop,AnchorBottom）
    Qt::AnchorPoint anchorPoint;
    /// @beief 端口是否显示名称
    bool isShowName;
    /// @brief 端口在图形上显示的名字
    QString displayName;
    /// @brief 端口的附加参数信息，方便后续拓展
    QMap<QString, QVariant> extraInfoMap;
    /// @brief 端口角度，目前用于圆形加法器
    int angle;
    /// @brief 被复制的端口uuid
    QString originPortUUID;
};

PortContextPrivate::PortContextPrivate()
    : type("input"),
      name(""),
      size(QSize(5, 5)),
      pos(QPointF(0, 0)),
      isAbsolutePos(false),
      supportMultipConnectorWire(false),
      anchorPoint(Qt::AnchorTop),
      isShowName(true),
      displayName(""),
      angle(0)
{
}

PortContextPrivate::PortContextPrivate(const PortContextPrivate &other)
{
    sourceUUID = other.sourceUUID;
    type = other.type;
    name = other.name;
    uuid = other.uuid; // Utility::createUUID();
    size = other.size;
    pos = other.pos;
    isAbsolutePos = other.isAbsolutePos;
    supportMultipConnectorWire = other.supportMultipConnectorWire;
    anchorPoint = other.anchorPoint;
    isShowName = other.isShowName;
    displayName = other.displayName;
    extraInfoMap = other.extraInfoMap;
    angle = other.angle;
    originPortUUID = other.uuid;
}

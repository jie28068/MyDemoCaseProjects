#pragma once
#include "BadgeGraphicsObject.h"
#include "ICanvasScene.h"
#include "ImageGraphicsObject.h"
#include "MathMLGraphicsObject.h"
#include "SizeControlGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "SourceNameTextGraphicsObject.h"
#include "TransformProxyOutlineGraphicsObject.h"

#include <QRectF>
#include <QSharedPointer>
#include <QTransform>

class TransformProxyGraphicsObjectPrivate
{
public:
    TransformProxyGraphicsObjectPrivate();

    /// @brief 鼠标操作缩放之前的缩放矩阵
    QTransform lastScaleTransform;
    /// @brief 旋转之前的矩阵
    QTransform lastRotateTransform;
    /// @brief 鼠标操作拖动图元移动之前的平移矩阵
    QTransform lastTranslateTransform;
    /// @brief 代理的资源类图元对象
    SourceGraphicsObject *sourceGraphics;
    /// @brief 资源类实例
    QSharedPointer<SourceProxy> sourceProxy;
    /// @brief 代理图元的边界大小
    QRectF boundingRect;
    /// @brief 旋转角度(目前只会取-90,0,90,180)
    int angle;
    /// @brief 大小调节控制块图元列表
    QList<SizeControlGraphicsObject *> sizeControlGraphics;

    QRectF lastRect;

    QPointF lastPos;

    /// @brief 旋转之后为了对齐网格的偏移量
    QPointF alignmentOffset;

    /// @brief 图元位置发生变化之前所有关联的连接线状态
    QMap<QString, PConnectorWireContext> lastConnectorWireCtx;

    /// @brief 当前图元鼠标是否按下
    bool isMousePressed;

    /// @brief 鼠标移动过程中是否已经触发了删除分支点操作
    bool isMovingCleanBranchPoints;

    /// @brief 鼠标是否悬浮在图元上
    bool isHover;

    bool isCopy;

    QPointF mousePressedPoint;

    QString copySourceUUID;

    // 模块名称
    SourceNameTextGraphicsObject *nameGraphics;

    QVector<QLineF> alignLine;

    QList<QGraphicsLineItem *> alignLineItems;

    /// @brief 外边框图层
    TransformProxyOutlineGraphicsObject *outlineGraphics;

    MathMLGraphicsObject *mathMLGraphics;

    QList<PortGraphicsObject *> readyCheckPorts;

    QList<PortGraphicsObject *> readyMatchPorts;

    /// @brief 代理图元是否在进行缩放
    bool isOnSizeChange;

    ImageGraphicsObject *backgroundImageGraphics;

    ImageGraphicsObject *foregroundImageGraphics;

    BadgeGraphicsObject *badgeGraphics;

    QPointF lastScenePos;
};

TransformProxyGraphicsObjectPrivate::TransformProxyGraphicsObjectPrivate()
{
    sourceGraphics = nullptr;
    sourceProxy = QSharedPointer<SourceProxy>();
    boundingRect = QRectF(0, 0, 0, 0);
    angle = 0;
    alignmentOffset = QPointF(0, 0);
    isMousePressed = false;
    isMovingCleanBranchPoints = false;
    isHover = false;
    isCopy = false;
    nameGraphics = nullptr;
    outlineGraphics = nullptr;
    mathMLGraphics = nullptr;
    isOnSizeChange = false;
    backgroundImageGraphics = nullptr;
    foregroundImageGraphics = nullptr;
    badgeGraphics = nullptr;
}
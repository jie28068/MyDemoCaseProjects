#ifndef TRANSFORMPROXYGRAPHICSOBJECT_H
#define TRANSFORMPROXYGRAPHICSOBJECT_H

#include "GraphicsLayer.h"
#include "IConnectorWireAlgorithm.h"
#include "SourceGraphicsObject.h"

#include <QGraphicsObject>
#include <QRectF>
#include <QScopedPointer>
#include <QSharedPointer>

const QString ProxyIDPrefix = "TransformProxy_";

class ICanvasScene;
class SourceProxy;
class SourceGraphicsObject;
class PortGraphicsObject;
class TransformProxyGraphicsObjectPrivate;
class WireParameters;
class SourceNameTextGraphicsObject;
class TransformProxyOutlineGraphicsObject;
class MathMLGraphicsObject;
class ConnectorWireGraphicsObject;
class QwtMathMLDocument;

class GRAPHICSMODELINGKERNEL_EXPORT TransformProxyGraphicsObject : public GraphicsLayer
{
    Q_OBJECT
public:
    friend class ICanvasGraphicsObjectFactory;
    friend class EditableGraphicsTextItem;
    friend class CanvasSceneDefaultImpl;

public:
    TransformProxyGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> sourceProxy,
                                 QGraphicsItem *parent = 0);
    virtual ~TransformProxyGraphicsObject();

    virtual int type() const override;

    virtual QString id() override;

    virtual void updatePosition();

    /// @brief 获取代理图元内部的资源图元
    /// @return
    SourceGraphicsObject *getSourceGraphicsObject();

    SourceNameTextGraphicsObject *getSourceNameTextGraphicsObject();

    /// @brief 获取当前代理图元内部的资源指针
    /// @return
    QSharedPointer<SourceProxy> getSourceProxy();

    /// @brief 获取数学公式图元
    /// @return
    MathMLGraphicsObject *getMathMLGraphicsObject();

    /// @brief 创建内部图元
    virtual void createInternalGraphics() override;

    /// @brief 中心点旋转
    void rotateOnCenter(int angle, Qt::Axis axis = Qt::ZAxis);

    /// @brief 应用transform
    /// @param scale 缩放
    /// @param rotate 旋转
    /// @param translate 平移
    void applyTransforms(QTransform scale, QTransform rotate, QTransform translate, QTransform xFlipTransform,
                         QTransform yFlipTransform);

    /// @brief 获取当前旋转角度
    /// @return
    int getAngle() const;

    virtual QRectF boundingRect() const override;

    virtual QPainterPath shape() const;

    /// @brief 获取端口图元对象
    /// @param id 端口id
    /// @return
    PortGraphicsObject *getPortGraphicsObject(QString id);

    /// @brief 获取端口图元列表
    /// @return
    QList<PortGraphicsObject *> getPortGraphicsObjectList();

    /// @brief 获取当前图元的连接线列表
    /// @return
    QList<ConnectorWireGraphicsObject *> getLinkedConnectorWires();

    void emitSourceNameHasChanged(QPointF pointDelta);

    /// @brief 获取代理图元的场景Rect
    /// @return  场景Rect
    QRectF getTransformSceneRect();

    /// @brief 更新大小缩放控制块,目前主要是更新坐标
    /// @param isSelected 当前代理图元是否被选中
    void refreshSizeControlGraphics(bool isSelected = false);

    /// @brief 获取模块名称的boundingRect
    /// @return  boundingRectt
    QRectF getSourceNameBounding();

    bool isOnSizeChange();

signals:
    void positionChanged(QPointF newPos);

    void transformChanged(QRectF rect, int angle);

    void sourceNamePositionChanged(QPointF pointDelta);

    void selectedChanged(bool selected);

public slots:
    void setBoundingRect(const QRectF &boundingRect);

    /// @brief 处理大小控制块拖动信号
    /// @param offset 拖动的鼠标偏移量
    void onProcessSizeChanged(QPointF offset);

    void onReadySizeChange();

    void onFinishSizeChange();

    void applyTransformChanged();

    //////////////////////////////////////////

    void onSourceChange(QString key, QVariant val); // @key:属性名称 @val:属性值

    /// @brief 资源状态变化时响应
    /// @param state 状态:normal disable warning error
    void onSourceStateChange(QString state);

    /// @brief 资源注释状态(直通禁用等)变化时响应
    /// @param commentState 状态:normal disable through
    void onSourceCommentStateChange(QString state);

    /// @brief 更新端口信息及保留原有连接线
    /// @param sizeOffset 有连接线的输出端口ID需要偏移的值，根据输出端口数量增减获得
    void onResetPortList(int sizeOffset);

    // 生成对齐线
    void createAlignLine();

    // 清除对齐线
    void clearAlignLine();

    /// @brief 创建端口自动连接提示连接线
    void createConnectionPrompt();

    void emitDeleteSource();

    void onSourceBadgeMsgChanged(const QString &msg);

    /// @brief 调整端口时可能超出边界，需要进行senceRect调整
    void adjustSceneRect();

protected:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    bool isCopying();

    /// @brief 校验名称是否合法
    /// @return
    bool checkNameValid(QString nameString);

    /// @brief 初始化控制块Visible属性
    void initSizeControlVisibleProperty(SourceGraphicsObject::ScaleDirection scaleDir);

    /// @brief 计算新增的资源在不允许重叠的画板模式下的合适坐标位置
    /// 目前实现的规则是一直向下偏移一个网格距离，直到当前的点不会和其他图元重合
    /// @param pos 坐标
    /// @param size 大小
    /// @return
    QPointF calcuteOverlapPos(QPointF pos, QSizeF size);

    void adjustSize();

    void adjustPos();

    QPointF calcChangingRect(QRectF &rc, QPointF offset, int direction);

    /// @brief 触发拷贝操作
    void onTriggerCopy();

    /// @brief 触发模块间自动连线
    void onTriggerAutoConnect();

    /// @brief 水平对齐线
    void alignHorizontalLine();

    /// @brief 垂直对齐线
    void alignVerticalLine();

    /// @brief 中心对齐线
    void alignCenterLine();

    // 判断该图元是可以与本图元对齐 生成对齐线
    bool checkAlignRule(TransformProxyGraphicsObject *transProxy);

    /// @brief 获取可以和当前图元端口检测自动匹配的端口列表
    /// @return
    QList<PortGraphicsObject *> getBeCheckedAutoMatchPorts();

    QList<PortGraphicsObject *> getReadyCheckPorts();

    TransformProxyOutlineGraphicsObject *getOutlineGraphicsObject();

signals:
    void deleteSource(QString uuid);

private:
    QScopedPointer<TransformProxyGraphicsObjectPrivate> dataPtr;

    QwtMathMLDocument *mathDoc;
};

#endif // TRANSFORMPROXYGRAPHICSOBJECT_H

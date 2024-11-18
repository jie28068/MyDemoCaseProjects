#ifndef CONTROLMODULEPORTGRAPHICSOBJECT_H
#define CONTROLMODULEPORTGRAPHICSOBJECT_H

#include "ControlDataTextGraphicsObject.h"
#include "defines.h"

/*
控制类型画板的端口图元实现
*/

class ControlModulePortGraphicsObjectPrivate;
class ControlModulePortGraphicsObject : public PortGraphicsObject
{
    Q_OBJECT

public:
    ControlModulePortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                                    QSharedPointer<PortContext> context, QGraphicsItem *parent = 0);
    ~ControlModulePortGraphicsObject();

    /// @brief 获取端口中心点坐标(场景坐标)
    /// @return
    virtual QPointF getLinkPos() override;

    /// @brief 获取端口的外联点坐标(场景坐标),在使用曼哈顿路径算法的时候需要
    /// 也就是根据锚点方向计算延伸出来的线段长度
    /// @return
    virtual QPointF getExternalPos() override;

    virtual QPointF getCenterPos() override;

    /// @brief 创建内部图元
    virtual void createInternalGraphics() override;

    virtual void linkChanged() override;

    virtual bool canMove() override;

    virtual QRectF boundingRect() const;

    virtual QPainterPath shape() const;

    /// @brief 更新坐标
    virtual void updatePosition() override;

    QRectF portNameRect();

    void setDataTextShow(const QString &text);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private slots:
    void onPortHideOrShowChange(bool value);
    void onPortNameChanged(QString name);

private:
    QVector<QPointF> calcPoints();

    void isShowPortName();

private:
    QScopedPointer<ControlModulePortGraphicsObjectPrivate> dataPtr;
};

#endif // CONTROLMODULEPORTGRAPHICSOBJECT_H

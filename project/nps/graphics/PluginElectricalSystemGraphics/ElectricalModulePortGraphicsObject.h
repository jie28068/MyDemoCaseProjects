#pragma once

#include "ElectrialDataTextGraphicsObject.h"
#include "defines.h"

class ElectPortNameGraphics;
class ElectricalModulePortGraphicsObject : public PortGraphicsObject
{
    Q_OBJECT

public:
    ElectricalModulePortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                                       QSharedPointer<PortContext> context, QGraphicsItem *parent = 0);
    ~ElectricalModulePortGraphicsObject();

    virtual Qt::AnchorPoint getAnchorPoint() override;

    virtual QPointF getLinkPos() override;

    virtual QPointF getExternalPos() override;

    virtual void updatePosition() override;

    /// @brief 创建内部图元
    virtual void createInternalGraphics() override;

    QRectF portNameRect();

    void setTempAnchor(Qt::AnchorPoint anchor);

    /// @brief 重置端口锚点方向
    virtual void resetPortAnchor();

    virtual void linkChanged();

    /// @brief 端口是否可以移动
    /// @return
    virtual bool canMove();

    QList<showDataStruct> dealDataList(QList<showDataStruct> &originalDataList, PortGraphicsObject *otherPort,
                                       SourceGraphicsObject *otherSource);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual void autoLayout() override;

private:
    void isShowPortName();

public slots:
    void onReceiveShowPowerFlowChange(bool flag);

private:
    /// @brief 临时锚点方向，只用于电气传输线自动旋转时，连接线算法时使用
    /// 因为目前不会存在Qt::AnchorHorizontalCenter，所以以Qt::AnchorHorizontalCenter为标记
    /// 判断是否使用当前临时锚点
    Qt::AnchorPoint tempAnchor;

    /// @brief 点状母线端口的锚点方向，需要动态计算最优方向
    Qt::AnchorPoint dotBusPortAnchor;

    ElectrialDataTextGraphicsObject *dataTextGraphicsObject;

    ElectPortNameGraphics *portNameGraphics;

    /// @brief 当前端口所属的电气元件是否是电气构造型
    bool isElectricalCombineType;
};

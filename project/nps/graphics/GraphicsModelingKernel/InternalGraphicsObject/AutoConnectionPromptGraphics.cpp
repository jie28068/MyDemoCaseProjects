#include "AutoConnectionPromptGraphics.h"
#include "CanvasSceneDefaultImpl.h"
#include "ConnectorWirePrompt.h"
#include "PortGraphicsObject.h"

AutoConnectionPromptGraphics::AutoConnectionPromptGraphics(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    setLayerFlag(kAutoConnectionPromptGraphicsLayer);
    setAcceptHoverEvents(true);
}

AutoConnectionPromptGraphics::~AutoConnectionPromptGraphics() { }

void AutoConnectionPromptGraphics::clean()
{
    for each (auto prompt in wirePormpts) {
        if (prompt) {
            delete prompt;
        }
    }
    wirePormpts.clear();
    unitedRect = QRectF();
}

bool AutoConnectionPromptGraphics::addPortPair(PortGraphicsObject *port1, PortGraphicsObject *port2)
{
    if (!port1 || !port2) {
        return false;
    }
    if (!port1->getLinkedConnectorWireList().isEmpty() || !port2->getLinkedConnectorWireList().isEmpty()) {
        return false;
    }

    ConnectorWirePrompt *wirePrompt = new ConnectorWirePrompt(port1, port2, this);
    wirePrompt->setToolTip(tr("Click Auto Connect")); // 点击自动连接
    wirePormpts.push_back(wirePrompt);
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
    connect(wirePrompt, SIGNAL(clicked(PortGraphicsObject *, PortGraphicsObject *)), defaultScene,
            SLOT(onNewConnectWireShouldBeCreate(PortGraphicsObject *, PortGraphicsObject *)));

    QRectF rcWire = wirePrompt->boundingRect();
    unitedRect = unitedRect.unite(rcWire.adjusted(-80, -80, 80, 80));
    return true;
}

void AutoConnectionPromptGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    GraphicsLayer::paint(painter, option, widget);
}

QRectF AutoConnectionPromptGraphics::boundingRect() const
{
    return unitedRect;
}

void AutoConnectionPromptGraphics::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    GraphicsLayer::hoverEnterEvent(event);
}

void AutoConnectionPromptGraphics::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    for each (auto prompt in wirePormpts) {
        if (prompt) {
            prompt->setVisible(false);
        }
    }
    GraphicsLayer::hoverLeaveEvent(event);
}

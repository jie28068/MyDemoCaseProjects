#include "ConstantSourceGraphicsObject.h"

ConstantSourceGraphicsObject::ConstantSourceGraphicsObject(ICanvasScene *canvasScene,
                                                           QSharedPointer<SourceProxy> source, QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent)
{
    // setFlag(ItemIgnoresTransformations, true);
}

ConstantSourceGraphicsObject::~ConstantSourceGraphicsObject() { }

void ConstantSourceGraphicsObject::createInternalGraphics()
{
    ControlModuleSourceGraphicsObject::createInternalGraphics();
    // PSourceProxy sourceProxy = getSourceProxy();
    // if (!sourceProxy) {
    //     return;
    // }

    // SourceProperty &sourceProperty = sourceProxy->getSourceProperty();

    // TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    // displayText = new DisplayParameterTextGraphicsObject(getCanvasScene(), proxyGraphics);
    // QString strConstantValue = sourceProperty.getDisplayData().toString();
    // displayText->setPlainText(strConstantValue);
    // QFont font = displayText->font();
    // font.setPointSizeF(10);
    // font.setFamily("Microsoft YaHei UI");
    // displayText->setfont(font);
    // displayText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
    // displayText->updatePosition();
}

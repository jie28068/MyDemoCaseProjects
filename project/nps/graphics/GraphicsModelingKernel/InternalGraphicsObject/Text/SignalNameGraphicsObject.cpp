#include "SignalNameGraphicsObject.h"
#include "CanvasViewDefaultImpl.h"
#include "ConnectorWireGraphicsObject.h"

SignalNameGraphicsObject::SignalNameGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject<EditableGraphicsTextItem>(canvasScene, parent)
{
    enableEditable(true);
    textItem->showBoundings(true);
    CanvasViewDefaultImpl *defaultView = dynamic_cast<CanvasViewDefaultImpl *>(canvasScene->getCanvasView());
    if (defaultView) {
        connect(textItem, SIGNAL(startTextEdit()), defaultView, SLOT(onEditTextName()));
        connect(textItem, SIGNAL(finishTextEdit()), defaultView, SLOT(onFinishEditTextName()));
    }
    connect(textItem, SIGNAL(finishTextEdit()), this, SLOT(onTextChanged()));
    QFont font;
    font.setPixelSize(8);
    font.setFamily("Microsoft YaHei UI");
    font.setBold(false);
    setfont(font);
    textItem->setWipeOffRegexp(QRegExp("[^a-zA-Z0-9\u4e00-\u9fa5_]"));
    textItem->setMaxStringLength(32);

    auto canvasContext = getCanvasContext();
    if (canvasContext) {
        connect(canvasContext.data(), &CanvasContext::statusFlagsChanged, this,
                &SignalNameGraphicsObject::onCanvasStatusChanged);
    }
}

SignalNameGraphicsObject::~SignalNameGraphicsObject() { }

QRectF SignalNameGraphicsObject::boundingRect() const
{
    QRectF parentBoundingRc = TextGraphicsObject<EditableGraphicsTextItem>::boundingRect();
    qreal margin = 10;
    return parentBoundingRc.adjusted(-margin, -margin, margin, margin);
}

void SignalNameGraphicsObject::updatePosition()
{
    QRectF rc = boundingRect();
}

void SignalNameGraphicsObject::onCanvasStatusChanged(CanvasContext::StatusFlag flag)
{
    auto canvasContext = getCanvasContext();
    if (canvasContext) {
        bool canModify = canvasContext->canModify();
        if (!canModify) {
            enableEditable(false);
        } else {
            enableEditable(true);
        }
    }
}

void SignalNameGraphicsObject::onTextChanged()
{
    ConnectorWireGraphicsObject *wire = dynamic_cast<ConnectorWireGraphicsObject *>(parentItem());
    if (wire) {
        QString plainText = getPlainText().trimmed();
        auto context = wire->getConnectorWireContext();
        context->setSignalName(plainText);
    }
}
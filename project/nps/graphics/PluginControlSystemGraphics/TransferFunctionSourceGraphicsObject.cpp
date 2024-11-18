#include "TransferFunctionSourceGraphicsObject.h"

#include <QApplication>
#include <QClipboard>

TransferFunctionSourceGraphicsObject::TransferFunctionSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                           QSharedPointer<SourceProxy> source,
                                                                           const QChar &let, QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent), letter(let)
{
    fractionalExpression = nullptr;
}

TransferFunctionSourceGraphicsObject::~TransferFunctionSourceGraphicsObject() { }

void TransferFunctionSourceGraphicsObject::createInternalGraphics()
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    fractionalExpression = new FractionalExpressionGraphicsObject(getCanvasScene(), proxyGraphics);
    fractionalExpression->setLetter(letter);
    QSharedPointer<SourceProxy> sourceProxy = proxyGraphics->getSourceProxy();
    SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
    QString displayString = sourceProperty.getDisplayData().toString();
    if (displayString != "") {
        fractionalExpression->processDisplayData(displayString);
    }
}

void TransferFunctionSourceGraphicsObject::onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue)
{
    if (key == SOURCE_DISPLAYDATA_PROPERTY_KEY) {
        if (controlSvgGraphics) {
            controlSvgGraphics->userShow(false);
        }
        if (displayText) {
            displayText->userShow(false);
        }
        QString displayString = newValue.toString();
        if (fractionalExpression) {

            fractionalExpression->processDisplayData(displayString);
        }
    }
}

void TransferFunctionSourceGraphicsObject::onSourceChange(QString key, QVariant value)
{
    if (key == GKD::SOURCE_SIZE) { // 模块大小
        this->setSourceBoundingRect();
    } else if (key == GKD::SOURCE_STATE) { // 模块状态
        // 当控制模块状态为disable时或模块状态为error时,若剪贴板中复制了改模块,则清除剪贴板,限制该模块粘贴
        QSharedPointer<SourceProxy> source = getSourceProxy();
        if (source) {
            if ((source->moduleType() != GKD::SOURCE_MODULETYPE_ELECTRICAL && value.toString() == "disable")
                || value.toString() == "error") {
                ClipBoardMimeData *mimeData =
                        dynamic_cast<ClipBoardMimeData *>((QMimeData *)QApplication::clipboard()->mimeData());
                if (mimeData) {
                    if (mimeData->IsContainSource(source->uuid())) {
                        QApplication::clipboard()->clear();
                    }
                }
            }
        }
    }
}
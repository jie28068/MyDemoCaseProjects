#ifndef GRAPHICSMODELINGKERNEL_H
#define GRAPHICSMODELINGKERNEL_H

#include "ActionManager.h"
#include "BookMark.h"
#include "BusinessHooksServer.h"
#include "CanvasContext.h"
#include "ClipBoardMimeData.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "Global.h"
#include "GraphicsKernelDefinition.h"
#include "GraphicsLayer.h"
#include "GraphicsLayerManager.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "ICanvasScene.h"
#include "ICanvasView.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "SvgGraphicsObject.h"
#include "TransformProxyGraphicsObject.h"

#include "Utility.h"

#include "BorderTextGraphicsObject.h"
#include "EditableGraphicsTextItem.h"
#include "PortNameTextGraphicsObject.h"
#include "SourceNameTextGraphicsObject.h"
#include "TextGraphicsObject.h"

#include "IConnectorWireAlgorithm.h"

#include "MathMLGraphicsObject.h"
#include "MathMLManager.h"
#include "NPSPropertyManager.h"

#include "SourceProtoTypeTextGraphicsObject.h"

typedef void (*logHandler)(QString msg, int level);

GRAPHICSMODELINGKERNEL_EXPORT bool
registerCanvasGraphicsObjectFactory(QString type, QSharedPointer<ICanvasGraphicsObjectFactory> factory);

GRAPHICSMODELINGKERNEL_EXPORT ICanvasView *createCanvasView(QSharedPointer<CanvasContext> canvasContext);

GRAPHICSMODELINGKERNEL_EXPORT void setBaseLogHandler(logHandler handler);

GRAPHICSMODELINGKERNEL_EXPORT void blog(QString msg, int level = 0);

GRAPHICSMODELINGKERNEL_EXPORT QSharedPointer<IConnectorWireAlgorithm>
createConnectorWireAlgorithm(QString algorithmName);

GRAPHICSMODELINGKERNEL_EXPORT void saveBuriedData();

#endif // GRAPHICSMODELINGKERNEL_H

#include "graphicsmodelingkernel.h"
#include "BezierConnectorWireAlgorithm.h"
#include "BuriedData.h"
#include "CanvasViewDefaultImpl.h"
#include "GraphicsFactoryManager.h"
#include "GraphicsLayer.h"
#include "IConnectorWireAlgorithm.h"
#include "ManhattaConnectorWireAlgorithm.h"
#include "StraightLineConnectorWireAlgorithm.h"
#include <QDebug>
#include <QMap>

static void defaultLogHandler(QString msg, int level)
{
    qDebug() << msg;
}

static logHandler sLogHandler = defaultLogHandler;

bool registerCanvasGraphicsObjectFactory(QString type, QSharedPointer<ICanvasGraphicsObjectFactory> factory)
{
    return GraphicsFactoryManager::getInstance().registerFactory(type, factory);
}

ICanvasView *createCanvasView(QSharedPointer<CanvasContext> canvasContext)
{
    return new CanvasViewDefaultImpl(canvasContext);
}

void setBaseLogHandler(logHandler handler)
{
    if (nullptr == handler) {
        return;
    }
    sLogHandler = handler;
}

void blog(QString msg, int level)
{
    if (sLogHandler == nullptr) {
        return;
    }
    sLogHandler(msg, level);
}

QSharedPointer<IConnectorWireAlgorithm> createConnectorWireAlgorithm(QString algorithmName)
{
    QSharedPointer<IConnectorWireAlgorithm> algorithm;
    if (algorithmName == ManhattaAlgorithmName) {
        algorithm = QSharedPointer<IConnectorWireAlgorithm>(new ManhattaConnectorWireAlgorithm(nullptr));
    } else if (algorithmName == StraightLineAlgorithmName) {
        algorithm = QSharedPointer<IConnectorWireAlgorithm>(new StraightLineConnectorWireAlgorithm(nullptr));
    } else if (algorithmName == BezierAlogrithmName) {
        algorithm = QSharedPointer<IConnectorWireAlgorithm>(new BezierConnectorWireAlgorithm(nullptr));
    }
    return algorithm;
}

GRAPHICSMODELINGKERNEL_EXPORT void saveBuriedData()
{
    BuriedData::getInstance().save();
}

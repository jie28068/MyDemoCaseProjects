#include "ICanvasScene.h"
#include "ActionManager.h"
#include "CanvasContext.h"
#include "GraphicsFactoryManager.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"

#include <QPainter>

ICanvasScene::ICanvasScene(QObject *parent) : QGraphicsScene(parent)
{
    // 不禁用索引时，连接线删除时会导致Scene的bspTree索引异常，程序崩溃
    this->setItemIndexMethod(QGraphicsScene::NoIndex);
}

ICanvasScene::~ICanvasScene() { }

unsigned int ICanvasScene::getGridSpace()
{
    return 10;
}

bool ICanvasScene::isAutoAlignmentGrid()
{
    return true;
}

bool ICanvasScene::allowOverlap()
{
    return false;
}

void ICanvasScene::setMouseClickedPort(PortGraphicsObject *port) { }

PortGraphicsObject *ICanvasScene::getMouseClickedPort()
{
    return nullptr;
}

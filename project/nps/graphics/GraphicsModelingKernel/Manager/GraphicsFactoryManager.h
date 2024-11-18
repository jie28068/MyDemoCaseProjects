#pragma once
#include "ICanvasGraphicsObjectFactory.h"

#include <QStringList>

/*
 * 图元工厂管理类
 * 外部调用者可以通过接口registerCanvasGraphicsObjectFactory注册对应模块类型的工厂实例
 */

class GraphicsFactoryManager
{
public:
    static GraphicsFactoryManager &getInstance();

    /// @brief 注册图元工厂实例
    /// @param type 类型
    /// @param factory 工厂实例
    /// @return true:注册成功 false:注册失败
    bool registerFactory(QString type, QSharedPointer<ICanvasGraphicsObjectFactory> factory);

    QSharedPointer<ICanvasGraphicsObjectFactory> getFactory(QString type);

private:
    GraphicsFactoryManager();

private:
    QMap<QString, QSharedPointer<ICanvasGraphicsObjectFactory>> canvasGraphicsObjectFactoryMap;
};

#pragma once

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"

namespace Kcc {
namespace BlockDefinition {
class Model;
using PModel = QSharedPointer<Model>;
}
}

// 画板钩子服务 简单工厂
class CanvasBusinessHooksServerFactory
{
public:
    static BusinessHooksServer *create(QSharedPointer<CanvasContext> pcanvascontext,
                                       QSharedPointer<Kcc::BlockDefinition::Model> model);
};

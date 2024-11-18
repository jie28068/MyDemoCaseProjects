#ifndef GLOBAL_GMS_H
#define GLOBAL_GMS_H

#include "CoreLib/ServerBase.h"
#include "CoreUi/Common/BasicWorkareaContentWidget.h"

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"

#include "KernnelInterface/BaseKernelInterface.h"
#include "KernnelInterface/KernelInterfaceFactory.h"

namespace GMS {
using namespace Kcc::BlockDefinition;

static const QString BoardTypeElectrical = QObject::tr("Electrical System");                    // 电气系统
static const QString BoardTypeControl = QObject::tr("Control System");                          // 控制系统
static const QString BoardTypeCombine = QObject::tr("Structure Model Template");                // 构造型模板
static const QString BoardTypeComplex = QObject::tr("Compound Model Template");                 // 复合型模板
static const QString BoardTypeElecCombine = QObject::tr("Electrical Structure Model Template"); // 电气构造型模板
}

#endif // GLOBAL_GMS_H

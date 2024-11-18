#pragma once

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"

using namespace Kcc::BlockDefinition;

typedef void (*pPatchFuc)(PModel, PCanvasContext);

// 画板版本补丁和升级（修改画板版本标识）
class VersionPatch
{
public:
    VersionPatch();

    void putPatch(PModel pBoardModel, PCanvasContext pCanvasCtx);

private:
    QMap<QDateTime, pPatchFuc> m_versionPatchMap; // 版本日期-版本补丁功能函数
};

// 版本补丁功能函数
static void patchFuc20220915to20221212(PModel pBoardModel, PCanvasContext pCanvasCtx);
static void patchFuc20221212to20221222(PModel pBoardModel, PCanvasContext pCanvasCtx);

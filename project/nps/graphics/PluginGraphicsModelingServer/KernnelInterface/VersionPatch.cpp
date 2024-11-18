#include "VersionPatch.h"

#include "GraphicsModelingKernel/GraphicsKernelDefinition.h"
#include "GraphicsModelingTool.h"
#include <QMap>

/// 画板历史版本
const QDateTime historyVersion[] = { QDateTime::fromString("20221212", CANVAS_VERSION_FORMAT),
                                     QDateTime::fromString("20221222", CANVAS_VERSION_FORMAT) };

VersionPatch::VersionPatch()
{
    // 后续加补丁，注意添加补丁日期和补丁函数（Static）
    m_versionPatchMap.insert(historyVersion[0], patchFuc20220915to20221212);
    m_versionPatchMap.insert(historyVersion[1], patchFuc20221212to20221222);
}

void VersionPatch::putPatch(PModel pBoardModel, PCanvasContext pCanvasCtx)
{
    if (!pBoardModel || !pCanvasCtx)
        return;

    QDateTime curVersion = QDateTime::fromString(pCanvasCtx->version(), CANVAS_VERSION_FORMAT);

    // 根据画板最后保存日期，添加版本兼容处理函数
    QList<QDateTime> keys = m_versionPatchMap.keys();
    for (int i = 0; i < keys.size(); i++) {
        pPatchFuc fuc = m_versionPatchMap.value(keys.at(i));
        if (!fuc)
            return;
        if (curVersion < keys.at(i)) {
            fuc(pBoardModel, pCanvasCtx); // 运行版本兼容函数

            curVersion = QDateTime::fromString(pCanvasCtx->version(), CANVAS_VERSION_FORMAT); // 修改版本标识
        }
    }
}

/// @brief
/// 【画板序列化】
/// 2.SourceProxy增加annotationMap
void patchFuc20220915to20221212(PModel pBoardClass, PCanvasContext pCanvasCtx)
{
    if (pBoardClass == nullptr || pCanvasCtx == nullptr) {
        return;
    }
    // 业务模块
    auto childModel = pBoardClass->getChildModels();
    for (auto model : childModel) {
        PSourceProxy pSource = pCanvasCtx->getSource(model->getUUID());
        if (pSource) {
            // 额外参数map处理
            bool isAllowScale = GraphicsModelingTool::getSourceProxyEnableScale(model);
            bool isDoubleBorder = GraphicsModelingTool::getSourceProxyShowInsideBorder(model);

            SourceProperty &property = pSource->getSourceProperty();
            property.setEnableScale(isAllowScale);
            property.setShowInsideBorder(isDoubleBorder);
        }
    }

    pCanvasCtx->setVersion("20221212");
}

void patchFuc20221212to20221222(PModel pBoardClass, PCanvasContext pCanvasCtx)
{
    if (pBoardClass == nullptr || pCanvasCtx == nullptr) {
        return;
    }
    if (pCanvasCtx->lineColor().alpha() == 0) {
        pCanvasCtx->setLineColor(QColor(GKD::WIRE_NORMAL_COLOR));
    }
    if (pCanvasCtx->lineSelectColor().alpha() == 0) {
        pCanvasCtx->setLineSelectColor(QColor(GKD::WIRE_NORMAL_COLOR));
    }

    pCanvasCtx->setVersion("20221222");
}

#include "LayerProperty.h"
#include <QHBoxLayout>
// 注册及请求服务所需要的头文件
#include "CoreLib/ServerManager.h"
#include "GraphicsModelingKernel/GraphicsLayerManager.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include <QApplication>
#include <QDebug>

USE_LOGOUT_("LayerProperty")

LayerProperty::LayerProperty(QSharedPointer<ICanvasScene> pscene)
    : CommonWrapper(), m_pCanvasScene(pscene), m_pLayerWidget(nullptr), m_pLayerSettingWidget(nullptr)
{
}

LayerProperty::~LayerProperty() { }

void LayerProperty::init()
{
    CommonWrapper::init();
    if (m_pCanvasScene == nullptr)
        return;
    PCanvasContext pcanvascontext = m_pCanvasScene->getCanvasContext();
    if (pcanvascontext == nullptr)
        return;

    m_layerItemsMap[DisplayLayer_SourceBasicGraphicsLayer] = pcanvascontext->getCanvasProperty().isBaseLayerVisible();
    m_layerItemsMap[DisplayLayer_SourceNameGraphicsLayer] = pcanvascontext->getCanvasProperty().isSourceNameVisible();
    m_layerItemsMap[DisplayLayer_CommentGraphicsLayer] = pcanvascontext->getCanvasProperty().isAnnotationVisible();
    m_layerItemsMap[DisplayLayer_ConnectorWireLayer] = pcanvascontext->getCanvasProperty().isConnectWireVisible();

    m_pLayerWidget = new LayerWidget(pcanvascontext, m_layerItemsMap);
    addWidget(m_pLayerWidget, LAYER_TAB_NAME_LAYERS);

    m_pLayerSettingWidget =
            new LayerSettingWidget(pcanvascontext, QStringList() << DisplayLayer_SourceNameGraphicsLayer);
    addWidget(m_pLayerSettingWidget, LAYER_TAB_NAME_LAYERSSETTING);
}

QString LayerProperty::getTitle()
{
    return tr("layer properties "); // 图层属性
}

void LayerProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        m_bPropertyIsChanged = false;

        if (m_pLayerWidget == nullptr || m_pLayerSettingWidget == nullptr || m_pCanvasScene == nullptr)
            return;
        auto canvasCtx = m_pCanvasScene->getCanvasContext();
        // 显示隐藏属性
        if (canvasCtx) {
            QMap<QString, bool> oldvisible = m_pLayerWidget->getOldVisibleMap();
            QMap<QString, bool> newvisible = m_pLayerWidget->getNewVisibleMap();
            foreach (QString displayname, newvisible.keys()) {
                if (oldvisible[displayname] != newvisible[displayname]) {
                    m_bPropertyIsChanged = true;
                    if (displayname == DisplayLayer_SourceBasicGraphicsLayer) {
                        canvasCtx->getCanvasProperty().setBaseLayerVisible(newvisible[displayname]);
                    } else if (displayname == DisplayLayer_SourceNameGraphicsLayer) {
                        canvasCtx->getCanvasProperty().setSourceNameVisble(newvisible[displayname]);
                    } else if (displayname == DisplayLayer_CommentGraphicsLayer) {
                        canvasCtx->getCanvasProperty().setAnnotationVisible(newvisible[displayname]);
                    } else if (displayname == DisplayLayer_ConnectorWireLayer) {
                        canvasCtx->getCanvasProperty().setConnectWireVisible(newvisible[displayname]);
                    }
                }
            }

            // 字体设置
            QMap<QString, QFont> oldfontmap = m_pLayerSettingWidget->getOldFontMap();
            QMap<QString, QFont> newfontmap = m_pLayerSettingWidget->getNewFontMap();
            foreach (QString displayname, newfontmap.keys()) {
                if (oldfontmap[displayname] != newfontmap[displayname]) {
                    m_bPropertyIsChanged = true;
                    if (displayname == DisplayLayer_SourceNameGraphicsLayer) {
                        canvasCtx->getCanvasProperty().setSourceNameFont(newfontmap[displayname]);
                    }
                }
            }
        }
    }
}
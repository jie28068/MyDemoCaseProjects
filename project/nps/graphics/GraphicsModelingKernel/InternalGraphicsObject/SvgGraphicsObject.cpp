#include "SvgGraphicsObject.h"
#include "CanvasContext.h"
#include "ICanvasScene.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "PortContext.h"
#include "QFile"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "SvgGraphicsObjectPrivate.h"
#include "TransformProxyGraphicsObject.h"

#include <QMatrix>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <qmath.h>

#define LineColor "#003756"
#define LineLineColor "#006ba5"
#define TextColor "#0079C2"
#define ST5Color "#35405C"

SvgGraphicsObject::SvgGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    dataPtr.reset(new SvgGraphicsObjectPrivate());
    // 绑定模块状态变更信号槽
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        dataPtr->m_sourceGrapics = proxyGraphics->getSourceGraphicsObject();
        dataPtr->m_pSource = proxyGraphics->getSourceProxy();
        if (dataPtr->m_pSource) {
            connect(dataPtr->m_pSource.data(), SIGNAL(sourceChange(QString, QVariant)), this,
                    SLOT(onSourceChange(QString, QVariant)));
        }
        connect(&proxyGraphics->getLayerProperty(), &NPSPropertyManager::propertyChanged, this,
                &SvgGraphicsObject::onLayerPropertyChanged);
    }
}

SvgGraphicsObject::~SvgGraphicsObject() { }

void SvgGraphicsObject::setSvgImageDatas(const QVariantMap &svgImageDatas)
{
    initSvgMap(svgImageDatas);

    reloadSVGRender();
}

void SvgGraphicsObject::setAngle(int angle)
{
    if (angle == dataPtr->m_angle)
        return;
    dataPtr->m_angle = angle;
    reloadSVGRender();

    update();
}

QSizeF SvgGraphicsObject::getSVGSize()
{
    if (dataPtr->m_svgRender) {
        return dataPtr->m_svgRender->defaultSize();
    }
    return QSizeF(0, 0);
}

QSvgRenderer *SvgGraphicsObject::getSvgRenderer() const
{
    if (dataPtr) {
        return dataPtr->m_svgRender;
    }
    return nullptr;
}

QRectF SvgGraphicsObject::getProxyRect() const
{
    if (dataPtr->m_sourceGrapics) {
        return dataPtr->m_sourceGrapics->getTransformBoundingRect();
    }
    return QRectF();
}

void SvgGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (dataPtr->m_svgRender != nullptr) {
        QRectF rect = boundingRect();
        painter->save();
        dataPtr->m_svgRender->render(painter, rect);
        painter->restore();
    }
}

QRectF SvgGraphicsObject::boundingRect() const
{
    if (dataPtr->m_svgRender) {
        return dataPtr->m_svgBoundingRect;
    } else {
        if (dataPtr->m_sourceGrapics) {
            return dataPtr->m_sourceGrapics->boundingRect();
        } else {
            return QRectF(0, 0, 0, 0);
        }
    }
}

void SvgGraphicsObject::reloadSVGRender()
{
    if (!dataPtr->m_pSource)
        return;

    if (dataPtr->m_svgRender) {
        delete dataPtr->m_svgRender;
        dataPtr->m_svgRender = nullptr;
    }
    if (!dataPtr->m_mapSVGContents.contains(dataPtr->m_angle))
        return;

    QByteArray svgBA = dataPtr->m_mapSVGContents[dataPtr->m_angle];
    QByteArray colorName = dataPtr->m_pSource->getStateColor().name().toLatin1();

    // 搜索时高亮显示
    auto transformProxyGraphics = getTransformProxyGraphicsObject();
    auto canvasContext = getCanvasContext();
    auto sourceProxy = getSourceProxy();
    if (transformProxyGraphics && canvasContext && sourceProxy
        && sourceProxy->moduleType() != GKD::SOURCE_MODULETYPE_CONTROL) {
        bool isHighLight = transformProxyGraphics->getLayerProperty().getHightLight();
        if (isHighLight) {
            colorName = canvasContext->getCanvasProperty().getHighLightColor().name().toLatin1();
        }
    }

    // 替换SVG中的固定颜色，后续所有SVG规范了颜色属性，再优化这里
    svgBA.replace(LineColor, colorName)
            .replace(TextColor, colorName)
            .replace(LineLineColor, colorName)
            .replace(ST5Color, colorName)
            .replace("#252D3F", colorName);

    // 构造新的Render
    dataPtr->m_svgRender = new QSvgRenderer(svgBA);

    // add by liwenyu  2023.08.23 某个角度的图片可能是空的，此时就用0度的图片  (配合模型升级兼容)
    if (!dataPtr->m_svgRender->isValid()) {
        svgBA = dataPtr->m_mapSVGContents.value(0, QByteArray());
        if (!svgBA.isNull()) {
            delete dataPtr->m_svgRender;
            svgBA.replace(LineColor, colorName)
                    .replace(TextColor, colorName)
                    .replace(LineLineColor, colorName)
                    .replace(ST5Color, colorName)
                    .replace("#252D3F", colorName);
            dataPtr->m_svgRender = new QSvgRenderer(svgBA);
        }
    }
    // end

    // 根据SVG图片大小，重设BoundingRect
    reclacBoundingRect();

    update();
}

void SvgGraphicsObject::reclacBoundingRect()
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }

    // 如果不是电气模块，则返回
    if (canvasContext->type() != CanvasContext::kElectricalType
        && canvasContext->type() != CanvasContext::kElecUserDefinedType)
        return;

    quint32 gridSpace = canvasContext->gridSpace();
    qreal dWidthHint = static_cast<qreal>(dataPtr->m_svgRender->defaultSize().width()) / gridSpace;
    qreal dHeightHint = static_cast<qreal>(dataPtr->m_svgRender->defaultSize().height()) / gridSpace;
    dWidthHint = (qCeil(dWidthHint) * gridSpace);
    dHeightHint = (qCeil(dHeightHint) * gridSpace);
    // 无图片时给予一个40*40的默认值
    if (dWidthHint == 0 || dHeightHint == 0) {
        dWidthHint = 40;
        dHeightHint = 40;
    }
    dataPtr->m_svgBoundingRect = QRectF(0, 0, dWidthHint, dHeightHint);

    if (!dataPtr->m_pSource->portList().isEmpty()) {
        PPortContext port = dataPtr->m_pSource->portList().at(0);
        if (port) {
            Qt::AnchorPoint anchor = port->anchorPoint();
            if (anchor == Qt::AnchorLeft || anchor == Qt::AnchorRight) {
                if ((((int)dHeightHint / gridSpace) % 2) == 0) { // 偶数网格尺寸
                    dataPtr->m_sourceGrapics->setYAxisAlignment(SourceGraphicsObject::kAlignmentGrid);
                } else {
                    dataPtr->m_sourceGrapics->setYAxisAlignment(SourceGraphicsObject::kAlignmentHalfGrid);
                }
                dataPtr->m_sourceGrapics->setXAxisAlignment(SourceGraphicsObject::kAlignmentGrid);
            } else {
                if ((((int)dWidthHint / gridSpace) % 2) == 0) { // 偶数网格尺寸
                    dataPtr->m_sourceGrapics->setXAxisAlignment(SourceGraphicsObject::kAlignmentGrid);
                } else {
                    dataPtr->m_sourceGrapics->setXAxisAlignment(SourceGraphicsObject::kAlignmentHalfGrid);
                }
                dataPtr->m_sourceGrapics->setYAxisAlignment(SourceGraphicsObject::kAlignmentGrid);
            }
        }
    }

    // m_pSource->setSize(QSize(dWidthHint, dHeightHint));
    dataPtr->m_sourceGrapics->setSourceBoundingRect(dataPtr->m_svgBoundingRect);
}

void SvgGraphicsObject::onLayerPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue)
{
    if (name == LAYER_HIGHLIGHT_PROPERTY_KEY) {
        // 高亮状态发生变化
        reloadSVGRender();
    }
}

// void SvgGraphicsObject::initSvgMap(QStringList picPathList)
// {
//     int angle = 0; // 旋转角度 0 90 180 270

//     dataPtr->m_mapSVGContents.clear();

//     foreach (QString picPath, picPathList) {
//         QFile svgfile(picPath);
//         if (!svgfile.open(QIODevice::ReadOnly | QIODevice::Text))
//             return;
//         QByteArray svgcontent = svgfile.readAll();
//         svgfile.close();

//         dataPtr->m_mapSVGContents.insert(angle, svgcontent);

//         angle += 90;
//     }
// }

void SvgGraphicsObject::initSvgMap(const QVariantMap &svgImageDatas)
{
    dataPtr->m_mapSVGContents.clear();

    // 只提供了一张SVG图片，四个角度都使用这一张
    if (svgImageDatas.size() == 1) {
        if (svgImageDatas.contains(Kcc::BlockDefinition::SVG_0_PIC)) {
            dataPtr->m_mapSVGContents.insert(0, svgImageDatas[Kcc::BlockDefinition::SVG_0_PIC].toByteArray());
            dataPtr->m_mapSVGContents.insert(90, svgImageDatas[Kcc::BlockDefinition::SVG_0_PIC].toByteArray());
            dataPtr->m_mapSVGContents.insert(180, svgImageDatas[Kcc::BlockDefinition::SVG_0_PIC].toByteArray());
            dataPtr->m_mapSVGContents.insert(270, svgImageDatas[Kcc::BlockDefinition::SVG_0_PIC].toByteArray());
            return;
        }
    }

    // 旋转角度 0 90 180 270
    if (svgImageDatas.contains(Kcc::BlockDefinition::SVG_0_PIC)) {
        dataPtr->m_mapSVGContents.insert(0, svgImageDatas[Kcc::BlockDefinition::SVG_0_PIC].toByteArray());
    }
    if (svgImageDatas.contains(Kcc::BlockDefinition::SVG_90_PIC)) {
        dataPtr->m_mapSVGContents.insert(90, svgImageDatas[Kcc::BlockDefinition::SVG_90_PIC].toByteArray());
    }
    if (svgImageDatas.contains(Kcc::BlockDefinition::SVG_180_PIC)) {
        dataPtr->m_mapSVGContents.insert(180, svgImageDatas[Kcc::BlockDefinition::SVG_180_PIC].toByteArray());
    }
    if (svgImageDatas.contains(Kcc::BlockDefinition::SVG_270_PIC)) {
        dataPtr->m_mapSVGContents.insert(270, svgImageDatas[Kcc::BlockDefinition::SVG_270_PIC].toByteArray());
    }
}

void SvgGraphicsObject::onSourceChange(QString key, QVariant val)
{
    if (GKD::SOURCE_STATE == key) {
        reloadSVGRender();
    } else if (GKD::SOURCE_SVG_IMAGEMAP == key) {
        setSvgImageDatas(val.toMap());
    }
}

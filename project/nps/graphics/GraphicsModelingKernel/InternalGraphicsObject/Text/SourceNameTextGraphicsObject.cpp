#include "SourceNameTextGraphicsObject.h"
#include "CanvasContext.h"
#include "CanvasSceneDefaultImpl.h"
#include "CanvasViewDefaultImpl.h"
#include "GraphicsKernelDefinition.h"
#include "SourceNameTextGraphicsObjectPrivate.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include "graphicsmodelingkernel.h"

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QFocusEvent>
#include <QPainter>

SourceNameTextGraphicsObject::SourceNameTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject<EditableGraphicsTextItem>(canvasScene, parent)
{
    dataPtr.reset(new SourceNameTextGraphicsObjectPrivate());
    setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setType(kSourceNameTextGraphics);
    EditableGraphicsTextItem *t = (EditableGraphicsTextItem *)textItem;
    // t->setWipeOffRegexp(QRegExp("[`·！!@#$￥%……^&()（）——=+{}【】,.，。《》<>：;；'‘'’“”"
    //":<>/|\\\"\\*\\?\\\\]"));
    textFont = getSourceNameFont();
    textItem->setFont(textFont);
    textItem->setDefaultTextColor(QColor("#252d3f"));
    int layerFlag = kSourceNameGraphicsLayer | kSelectedGraphicsLayer | kMoveableGraphicsLayer;
    setLayerFlag((GraphicsLayer::GraphicsLayerFlag)layerFlag);

    // bindsingal
    ICanvasScene *scene = getCanvasScene();
    if (scene != nullptr) {
        QList<QGraphicsView *> views = scene->views();
        if (views.size() > 0) {
            CanvasViewDefaultImpl *canvasView = dynamic_cast<CanvasViewDefaultImpl *>(views[0]);
            if (canvasView) {
                connect(textItem, SIGNAL(startTextEdit()), canvasView, SLOT(onEditTextName()));
                connect(textItem, SIGNAL(finishTextEdit()), canvasView, SLOT(onFinishEditTextName()));
            }
        }
        connect(textItem, SIGNAL(finishTextEdit()), this, SLOT(onTextContentsChanged()));
        connect(textItem, SIGNAL(startTextEdit()), this, SLOT(onOriginalTextContentsChanged()));
        auto context = scene->getCanvasContext();
        if (context && context->type() == CanvasContext::kElectricalType) {
            t->setWipeOffRegexp(QRegExp(QString("[^\\)\\(a-zA-Z0-9\u4E00-\u9FA5_-]{1,128}$")));
        } else {
            t->setWipeOffRegexp(QRegExp("[^\\)\\(a-zA-Z0-9\u4E00-\u9FA5_-\\s]{1,128}$"));
        }
    }

    auto transformProxyGraphics = getTransformProxyGraphicsObject();
    if (transformProxyGraphics != nullptr) {
        GraphicsLayerProperty &layerProperty = transformProxyGraphics->getLayerProperty();
        connect(&layerProperty, &NPSPropertyManager::propertyChanged, this,
                &SourceNameTextGraphicsObject::onSourcePropertyChanged);
    }

    dataPtr->normalColor = textItem->defaultTextColor();
}

SourceNameTextGraphicsObject::~SourceNameTextGraphicsObject() { }

void SourceNameTextGraphicsObject::updatePosition()
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        int angle = proxyGraphics->getAngle();
        if (dataPtr->angleOffset.contains(angle)) {
            QPointF offset = dataPtr->angleOffset[angle];
            QRectF proxyRect = proxyGraphics->boundingRect();
            qreal width = proxyRect.width();
            qreal height = proxyRect.height();
            qreal xPos = 0;
            qreal yPos = 0;

            // 第一次运行赋初值
            if (dataPtr->oldRecf == QRectF()) {
                dataPtr->oldRecf = proxyRect;
            }
            if (dataPtr->oldAngle < 0) {
                dataPtr->oldAngle = angle;
            }
            // 如果不是缩放就不改变相对位置
            if (offset.x() < 0 && dataPtr->oldAngle == angle
                && (dataPtr->oldRecf.width() != width || dataPtr->oldRecf.height() != height)) {
                xPos = offset.x() * dataPtr->oldRecf.width() + dataPtr->oldRecf.left()
                        - (dataPtr->oldRecf.left() - proxyRect.left());
            } else {
                xPos = offset.x() * width + proxyRect.left();
            }

            // 如果不是缩放就不改变相对位置
            if (offset.y() < 0 && dataPtr->oldAngle == angle
                && (dataPtr->oldRecf.width() != width || dataPtr->oldRecf.height() != height)) {
                yPos = offset.y() * dataPtr->oldRecf.height() + dataPtr->oldRecf.top()
                        - (dataPtr->oldRecf.top() - proxyRect.top());
            } else {
                yPos = offset.y() * height + proxyRect.top();
            }

            QPointF newPoint = QPointF(xPos, yPos);
            // 如果坐标偏离太远 说明可能是旧画板导入的数据 重新取新数据
            qreal w = boundingRect().width();
            qreal h = boundingRect().height();
            QRectF curBoardRec = proxyRect;
            curBoardRec = curBoardRec.adjusted(-w - 2, -h - 2, w + 2, h + 2);
            if (!curBoardRec.contains(newPoint)) {
                newPoint = getDefaultPoint(angle);
            }

            // 如果名称在图元左边那么要考虑会覆盖图元的问题
            if (newPoint.x() < proxyRect.left()
                && !(newPoint.y() - 11 + h < proxyRect.top() || newPoint.y() + 5.5 > proxyRect.bottom())) {
                if (newPoint.x() + w >= proxyRect.left()) {
                    newPoint.setX(proxyRect.left() - w);
                }
            }

            offset = calcRelPosition(newPoint, proxyRect);
            dataPtr->angleOffset[angle] = offset;
            setPos(newPoint);

        } else {
            QRectF parentRc = proxyGraphics->boundingRect();
            QPointF parentPos = getDefaultPoint(angle);
            QPointF offset = calcRelPosition(parentPos, parentRc);
            // angleOffset[angle] = offset;
            setPos(parentPos);
        }
        dataPtr->oldRecf = proxyGraphics->boundingRect();
        dataPtr->oldAngle = angle;
    }
}

QRectF SourceNameTextGraphicsObject::boundingRect() const
{
    QRectF parentBoundingRc = TextGraphicsObject<EditableGraphicsTextItem>::boundingRect();
    qreal margin = 3.5;
    return parentBoundingRc.adjusted(-margin, -margin, margin, margin);
}

void SourceNameTextGraphicsObject::userShow(bool visible)
{
    if (!visible) {
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        GraphicsLayer::userShow(visible);
    } else {
        GraphicsLayer::userShow(visible);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    }
}

QRectF SourceNameTextGraphicsObject::mapToParentBoundingRect()
{
    return mapToParent(boundingRect()).boundingRect();
}

bool SourceNameTextGraphicsObject::isMousePressing()
{
    return dataPtr->isMousePressed;
}

void SourceNameTextGraphicsObject::seTextNameChanged(QString name, bool isChanged)
{
    dataPtr->isChanged = isChanged;
    auto sourceProxy = getSourceProxy();
    if (sourceProxy) {
        sourceProxy->setName(name);
    }
    setPlainText(name);
    updatePosition();
}

void SourceNameTextGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    dataPtr->isMousePressed = true;
    auto canvasScene = getCanvasScene();
    if (canvasScene) {
        canvasScene->clearSelection();
    }
    TextGraphicsObject<EditableGraphicsTextItem>::mousePressEvent(event);
}

void SourceNameTextGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    dataPtr->isMousePressed = false;
    TextGraphicsObject<EditableGraphicsTextItem>::mouseReleaseEvent(event);
    if (dataPtr->isPositionChanged) {
        updateProxyAngleSet();
        dataPtr->isPositionChanged = false;
    }
}

void SourceNameTextGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto flags = canvasContext->getStatusFlags();
    bool isSimulationRunning = flags.testFlag(CanvasContext::kSimulationRunningStatus);
    bool isProjectActivaed = flags.testFlag(CanvasContext::kProjectActivate);
    bool isNextConstructiveBorad = flags.testFlag(CanvasContext::kNextConstructiveBorad);
    QAction *lockAction = ActionManager::getInstance().getAction(ActionManager::Lock);
    bool isLocked = lockAction->isChecked();
    if (isSimulationRunning || !isProjectActivaed || isLocked || isNextConstructiveBorad) {
        enableEditable(false);
    } else {
        enableEditable(true);
    }
    setCursor(Qt::SizeAllCursor);
    TextGraphicsObject<EditableGraphicsTextItem>::hoverEnterEvent(event);
}

void SourceNameTextGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    TextGraphicsObject<EditableGraphicsTextItem>::hoverLeaveEvent(event);
}

QVariant SourceNameTextGraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemSelectedHasChanged: {
        qDebug() << "name selected changed" << isSelected();
        bool flag = isSelected();
        BusinessHooksServer *hook = getBusinessHook();
        auto proxyGraphics = getTransformProxyGraphicsObject();
        if (!flag && hook && proxyGraphics) {
            bool proxyIsSelected = proxyGraphics->isSelected();
            bool isShowName = true; // hook->sourceNameIsShowWhenSelectedChanged(proxyIsSelected);
            userShow(isShowName);
        }
    } break;
    case QGraphicsItem::ItemPositionChange: {
        return handlePositionChanged(value.toPointF());
    }
    case QGraphicsItem::ItemPositionHasChanged: {
        TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(parentItem());
        if (proxyGraphics) {
            proxyGraphics->emitSourceNameHasChanged(dataPtr->pointPositionChangedDelta);
        }
        dataPtr->pointPositionChangedDelta = QPointF();
    } break;
    default: {
    }
    }
    return TextGraphicsObject<EditableGraphicsTextItem>::itemChange(change, value);
}

void SourceNameTextGraphicsObject::processPropertyChanged(const QString &name, const QVariant &oldValue,
                                                          const QVariant &newValue)
{
    GraphicsLayer::processPropertyChanged(name, oldValue, newValue);
    if (name == LAYER_FONT_PROPERTY_KEY) {
        textFont = getSourceNameFont();
    } else if (name == LAYER_HIGHLIGHT_PROPERTY_KEY) {
        if (newValue == true) {
            textItem->setDefaultTextColor(QColor("#0079c2"));
        } else {
            textItem->setDefaultTextColor(QColor("#252d3f"));
        }
    }
}

void SourceNameTextGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    if (textItem->hasFocus() || dataPtr->isMousePressed) {
        painter->setPen(QPen(QColor("#00cfff"), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(textItem->boundingRect());
    }
    if (dataPtr->isGotoFromHight) {
        textItem->setDefaultTextColor(QColor("#C06EAB"));
    } else if (dataPtr->isHighLight) {
        textItem->setDefaultTextColor(dataPtr->highLightColor);
    } else {
        textItem->setDefaultTextColor(dataPtr->normalColor);
    }

    painter->restore();
    return TextGraphicsObject::paint(painter, option, widget);
}

QPointF SourceNameTextGraphicsObject::handlePositionChanged(const QPointF &pointPosition)
{
    TransformProxyGraphicsObject *transformProxyGraphics = getTransformProxyGraphicsObject();
    if (!dataPtr->isMousePressed || nullptr == transformProxyGraphics) {
        // 仅处理鼠标拖放元素本身导致的位置变化
        // 不处理因最上级项目的矩形发生变化而导致的位置变化(如旋转、拉伸等)
        return pointPosition;
    }
    QPointF pointNew = pointPosition;

    // 文本内容吸附在模块周围，拖动时不会远离模块也不会遮挡模块
    int w = boundingRect().width();
    int h = boundingRect().height();
    QRectF m_currentRect = transformProxyGraphics->boundingRect();
    QPointF topLeft = m_currentRect.topLeft() - QPointF(w, h);
    QPointF bottomLeft = m_currentRect.bottomLeft() - QPointF(w, -7);
    QPointF topRight = m_currentRect.topRight() - QPointF(-7, h);

    QPointF pos = this->pos();
    // 固定新坐标最大值和最小值
    if (pointNew.x() < topLeft.x()) {
        pointNew.setX(topLeft.x());
    } else if (pointNew.x() > topRight.x()) {
        pointNew.setX(topRight.x());
    }
    if (pointNew.y() < topLeft.y()) {
        pointNew.setY(topLeft.y());
    } else if (pointNew.y() > bottomLeft.y()) {
        pointNew.setY(bottomLeft.y());
    }

    // 计算拖动方向，不遮挡模块

    QPointF delta = pointNew - pos;
    if (delta.x() != 0) {
        // 水平方向，只有在最底端或最顶端时可固定Y轴并移动，其余位置只能固定X轴垂直移动
        if (pos.y() == topLeft.y() || pos.y() == bottomLeft.y()) {
            pointNew.setY(pos.y());
        } else {
            pointNew.setX(pos.x());
        }
    } else if (delta.y() != 0) {
        // 垂直方向，只有在最左端或最右端时可固定X轴并移动，其余位置只能固定Y轴水平移动
        if (pos.x() == topLeft.x() || pos.x() == topRight.x()) {
            pointNew.setX(pos.x());
        } else {
            pointNew.setY(pos.y());
        }
    }
    int angle = transformProxyGraphics->getAngle();
    // 计算相对坐标
    QPointF offset = calcRelPosition(pointNew, m_currentRect);
    QPointF oldPoint;
    if (dataPtr->angleOffset.contains(angle)) {
        oldPoint = dataPtr->angleOffset[angle];
    } else {
        oldPoint = calcRelPosition(getDefaultPoint(angle), m_currentRect);
    }
    dataPtr->pointPositionChangedDelta = (offset - oldPoint);
    dataPtr->angleOffset[angle] = offset;
    dataPtr->isPositionChanged = true;
    return pointNew;
}

QPointF SourceNameTextGraphicsObject::getDefaultPoint(int angle)
{
    QPointF parentPos;
    TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(parentItem());
    if (proxyGraphics) {
        Qt::AnchorPoint anchor = Qt::AnchorBottom;
        QVariantList listDefaultAnchor;
        auto sourceProxy = getSourceProxy();
        if (sourceProxy) {
            listDefaultAnchor = sourceProxy->listNameDefaultAnchor();
        }

        if (!listDefaultAnchor.isEmpty()) {
            anchor = calculateDirection(listDefaultAnchor, angle);
        }
        QRectF parentRc = proxyGraphics->boundingRect();
        QRectF rc = boundingRect();
        qreal w = rc.width();
        qreal h = rc.height();
        auto tempRect = parentRc;
        tempRect.moveTo(0, 0);
        QPointF centerDelta = tempRect.center() - rc.center();
        switch (anchor) {
        case Qt::AnchorTop:
            parentPos = parentRc.topLeft() + QPointF(centerDelta.x(), -h);
            break;
        case Qt::AnchorLeft:
            parentPos = parentRc.topLeft() + QPointF(-w, centerDelta.y());
            break;
        case Qt::AnchorBottom:
            parentPos = parentRc.bottomLeft() + QPointF(centerDelta.x(), 7);
            break;
        case Qt::AnchorRight:
            parentPos = parentRc.topRight() + QPointF(7, centerDelta.y());
            break;
        }
    }
    return parentPos;
}

Qt::AnchorPoint SourceNameTextGraphicsObject::calculateDirection(const QVariantList &listAnchor, int angle)
{
    if (listAnchor.size() >= 4) {
        int index = Utility::getAngleIndex(angle);
        auto anchor = static_cast<Qt::AnchorPoint>(listAnchor.at(index).toInt());
        return anchor;
    }

    auto anchor = static_cast<Qt::AnchorPoint>(listAnchor.at(0).toInt());
    return anchor;
}

void SourceNameTextGraphicsObject::onSourceChange(QString key, QVariant val)
{
    if (GKD::SOURCE_NAME != key || dataPtr->isChanged) {
        dataPtr->isChanged = false;
        return;
    }
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    SourceProxy *source = qobject_cast<SourceProxy *>(sender());
    QPair<QString, QString> name(getPlainText(), source->name());
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
    if (source && defaultScene) {
        if (source->name() != getPlainText()) { // 通过模块编辑修改名称
            setPlainText(source->name());
            defaultScene->nameChanged(proxyGraphics->id(), name);
        } else { // 通过双击模块名称修改
            defaultScene->nameChanged(proxyGraphics->id(), textItem->name);
        }
    }
    updatePosition();
}

void SourceNameTextGraphicsObject::setAngleOffset(QMap<int, QPointF> nameOffset)
{
    dataPtr->angleOffset = nameOffset;
}

void SourceNameTextGraphicsObject::updateProxyAngleSet()
{
    auto sourceProxy = getSourceProxy();
    if (sourceProxy) {
        sourceProxy->setNameAngleOffset(dataPtr->angleOffset);
    }
}

void SourceNameTextGraphicsObject::movePositionBy(int angle, const QPointF &pointDelta)
{
    if (dataPtr->angleOffset.contains(angle)) {
        dataPtr->angleOffset[angle] += pointDelta;
    } else {
        QPointF defaultPoint = getDefaultPoint(angle);
        dataPtr->angleOffset[angle] =
                calcRelPosition(defaultPoint, getTransformProxyGraphicsObject()->boundingRect()) + pointDelta;
    }
    updatePosition();
    updateProxyAngleSet();
}

QPointF SourceNameTextGraphicsObject::calcRelPosition(const QPointF &point, const QRectF &recf)
{
    QPointF offset;
    qreal proxyWidth = recf.width();
    qreal proxyHeight = recf.height();

    offset.setX((point.x() - recf.left()) / proxyWidth);
    offset.setY((point.y() - recf.top()) / proxyHeight);
    return offset;
}

QFont SourceNameTextGraphicsObject::getSourceNameFont()
{
    ICanvasScene *pscene = getCanvasScene();
    if (pscene == nullptr) {
        return GKD::DEFAULT_FONT;
    }
    PCanvasContext pCanvasContext = pscene->getCanvasContext();
    if (pCanvasContext == nullptr) {
        return GKD::DEFAULT_FONT;
    }
    return pCanvasContext->getCanvasProperty().getSourceNameFont();
}

void SourceNameTextGraphicsObject::onTextContentsChanged()
{
    ICanvasScene *canvasScene = getCanvasScene();
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (nullptr == canvasScene) {
        return;
    }
    BusinessHooksServer *hook = getBusinessHook();
    if (proxyGraphics && hook) {
        QString newName = getPlainText();
        if (dataPtr->originalText != newName && !newName.isEmpty()) {
            hook->changeTextNameHint(dataPtr->originalText, newName);
            bool proxyIsSelected = proxyGraphics->isSelected();
            bool isShowName = true; // hook->sourceNameIsShowWhenSelectedChanged(proxyIsSelected);
            userShow(isShowName);
        }
    }
}

void SourceNameTextGraphicsObject::onOriginalTextContentsChanged()
{
    auto canvasScene = getCanvasScene();
    if (canvasScene) {
        canvasScene->clearSelection();
    }
    auto sourceProxy = getSourceProxy();
    if (sourceProxy) {
        dataPtr->originalText = sourceProxy->name();
    }
}

void SourceNameTextGraphicsObject::onSourcePropertyChanged(const QString &name, const QVariant &oldValue,
                                                           const QVariant &newValue)
{
    if (name == LAYER_HIGHLIGHT_PROPERTY_KEY) {
        dataPtr->isHighLight = newValue.toBool();
        if (dataPtr->isHighLight) {
            auto canvasContext = getCanvasContext();
            if (canvasContext) {
                dataPtr->highLightColor = canvasContext->getCanvasProperty().getHighLightColor();
            }
        }
    } else if (name == LAYER_GOTOFROM_HIGHLIGHT_PROPERTY_KEY) {
        dataPtr->isGotoFromHight = newValue.toBool();
    }
}

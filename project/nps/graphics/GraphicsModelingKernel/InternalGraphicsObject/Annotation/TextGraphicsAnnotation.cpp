#include "TextGraphicsAnnotation.h"
#include "ActionManager.h"
#include "SizeControlGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextDocument>

static const int MARGIN = 2;

TextGraphicsAnnotation::TextGraphicsAnnotation(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                               QGraphicsItem *parent)
    : AnnotationSourceGraphicsObject(canvasScene, source, parent)
{
    setScaleDirection(kScaleNone);
    setLayerFlag(kCommentGraphicsLayer);
    initProperty();
    oldFont = getFont();
    m_pTextbox = new TextboxItem(source, canvasScene, this);
    m_pTextbox->setFont(oldFont);
    m_pTextbox->setRegExp(QRegExp());
    m_pTextbox->setTextWidth(source->getAnnotationMap().value(Annotation::textWidth, -1).toReal());
    connect(source.data(), SIGNAL(sourceChange(QString, QVariant)), this,
            SLOT(onCommentSourceChange(QString, QVariant)));
    connect(m_pTextbox->document(), SIGNAL(contentsChanged()), this, SLOT(onTextChanged()));
    this->setFocus();
    // 如果是通过快捷创建 那么需要设置初始大小
    if (source->size() == QSizeF(0, 0)) {
        onTextChanged();
    }
    QRectF boundingRect;
    boundingRect.setHeight(source->size().height());
    boundingRect.setWidth(source->size().width());
    setSourceBoundingRect(boundingRect);
    if (!source->getAnnotationMap().value(Annotation::content, QVariant()).toString().isEmpty()) {
        status = kStatusFinished;
    }
}

TextGraphicsAnnotation::~TextGraphicsAnnotation() { }

void TextGraphicsAnnotation::createInternalGraphics()
{
    sizeControlGraphics = SizeControlGraphicsObject::createAllSizeControlGraphics(getCanvasScene());
    QListIterator<SizeControlGraphicsObject *> sizeControlIter(sizeControlGraphics);
    while (sizeControlIter.hasNext()) {
        SizeControlGraphicsObject *sizeControlGraphics = sizeControlIter.next();
        if (sizeControlGraphics) {
            sizeControlGraphics->setParentItem(this);
            connect(sizeControlGraphics, SIGNAL(sizeChanged(QPointF)), this, SLOT(onProcessSizeChanged(QPointF)));
            sizeControlGraphics->setZValue(sizeControlGraphics->zValue() + 1);
            connect(sizeControlGraphics, SIGNAL(readyChange()), this, SLOT(onReadySizeChange()));
            connect(sizeControlGraphics, SIGNAL(finishChange()), this, SLOT(onFinishSizeChange()));
        }
    }
}

void TextGraphicsAnnotation::userMenu(QSharedPointer<QMenu> menu)
{
    SourceGraphicsObject::userMenu(menu);
    menu->removeAction(ActionManager::getInstance().getAction(ActionManager::RotateAll));
}

QPainterPath TextGraphicsAnnotation::shape() const
{
    QPainterPath path;
    path.addRect(getTransformBoundingRect());
    path.closeSubpath();
    return path;
}

void TextGraphicsAnnotation::onProcessSizeChanged(QPointF pos)
{
    // 如果偏移量都为0，不处理
    if (Utility::isEqualZero(pos.x()) && Utility::isEqualZero(pos.y())) {
        return;
    }
    SizeControlGraphicsObject *sizeControlGraphics = dynamic_cast<SizeControlGraphicsObject *>(sender());
    ICanvasScene *canvasScene = getCanvasScene();
    auto sourceProxy = getSourceProxy();
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!canvasScene || !sourceProxy || !sizeControlGraphics || !proxyGraphics) {
        return;
    }

    int gridSpace = canvasScene->getGridSpace();
    pos = Utility::pointAlignmentToGrid(pos, gridSpace);

    QPointF offset = pos - lastPos;

    QRectF changingRect = boundingRect();
    QRectF tempRect = changingRect;

    SizeControlGraphicsObject::Direction direction = sizeControlGraphics->getDirection();

    QPointF pointHandle;
    switch (direction) {
    case SizeControlGraphicsObject::kDirectionRightCenter:
    case SizeControlGraphicsObject::kDirectionRightBottom:
    case SizeControlGraphicsObject::kDirectionBottomCenter: {
        pointHandle = changingRect.bottomRight();
        changingRect.setBottomRight(changingRect.bottomRight() + offset);
    } break;
    case SizeControlGraphicsObject::kDirectionLeftCenter:
    case SizeControlGraphicsObject::kDirectionLeftTop:
    case SizeControlGraphicsObject::kDirectionTopCenter: {
        pointHandle = changingRect.topLeft();
        changingRect.setTopLeft(changingRect.topLeft() + offset);
    } break;
    case SizeControlGraphicsObject::kDirectionLeftBottom: {
        pointHandle = changingRect.bottomLeft();
        changingRect.setBottomLeft(changingRect.bottomLeft() + offset);
    } break;
    case SizeControlGraphicsObject::kDirectionRightTop: {
        pointHandle = changingRect.topRight();
        changingRect.setTopRight(changingRect.topRight() + offset);
    } break;
    default: {
        return;
    }
    }

    QRectF changingSceneRect = mapRectToScene(changingRect);
    // 不允许缩放到场景外
    if (!canvasScene->sceneRect().contains(changingSceneRect)) {
        return;
    }

    // 拖动的时候，校验资源允许缩放的最小尺寸
    QSizeF sourceMinimumSize = QSizeF(20, 10);
    if (changingSceneRect.width() < sourceMinimumSize.width()
        || changingSceneRect.height() < sourceMinimumSize.height()) {
        return;
    }

    lastPos = pos;

    setSourceBoundingRect(changingRect);
    sourceProxy->setSize(changingRect.size());

    // 若拖动的handle对应的矩形角跟item的原点在同一X或同一Y，则需要进行位移
    QPointF originPoint = sourceProxy->getCombineTransform().map(QPointF(0, 0));
    QPointF pointDelta = offset;
    if (Utility::isEqualZero(originPoint.x() - pointHandle.x())
        || Utility::isEqualZero(originPoint.y() - pointHandle.y())) {
        if (Utility::isEqualZero(originPoint.x() - pointHandle.x())
            && Utility::isEqualZero(originPoint.y() - pointHandle.y())) {
            pointDelta = offset;
        } else if (Utility::isEqualZero(originPoint.x() - pointHandle.x())) {
            pointDelta.setY(0);
        } else if (Utility::isEqualZero(originPoint.y() - pointHandle.y())) {
            pointDelta.setX(0);
        }
        proxyGraphics->moveBy(pointDelta.x(), pointDelta.y());
        sourceProxy->setPos(proxyGraphics->pos());
    }
    m_pTextbox->setTextWidth(changingRect.width() - 2 * MARGIN > 5 ? changingRect.width() - 2 * MARGIN : 5);
    sourceProxy->setAnnotationProperty(Annotation::textWidth, changingRect.width() - 2 * MARGIN);
    refreshSizeControlGraphics(proxyGraphics->isSelected());
}

void TextGraphicsAnnotation::onReadySizeChange()
{
    lastPos = QPointF(0, 0);
}

void TextGraphicsAnnotation::onFinishSizeChange()
{
    if (!m_pTextbox) {
        return;
    }
    auto sourceProxy = getSourceProxy();
    auto textDocument = m_pTextbox->document();
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!sourceProxy || !textDocument || !proxyGraphics) {
        return;
    }
    auto documentLayout = textDocument->documentLayout();
    // 如果文本框的高度更大 那就更新为文本框的高度
    qreal height = documentLayout->documentSize().height() + 2 * MARGIN;
    qreal width = documentLayout->documentSize().width() + 2 * MARGIN;
    QRectF rect = boundingRect();
    if (height != boundingRect().height()) {
        rect.setHeight(height);
    }
    // 文本框缩放到宽度过短 文本框会自己换行变宽
    if (width > boundingRect().width()) {
        rect.setWidth(width);
    }
    setSourceBoundingRect(rect);
    sourceProxy->setSize(rect.size());
    refreshSizeControlGraphics(proxyGraphics->isSelected());
}

void TextGraphicsAnnotation::refreshSizeControlGraphics(bool isSelected)
{
    if (sizeControlGraphics.size() <= 0) {
        return;
    }
    if (status != kStatusFinished) {
        isSelected = false;
    }
    for each (SizeControlGraphicsObject *sizeControl in sizeControlGraphics) {
        if (sizeControl) {
            sizeControl->getLayerProperty().setVisible(isSelected);
            sizeControl->userShow(isSelected);
        }
    }

    QRectF rc = getTransformBoundingRect();
    QListIterator<SizeControlGraphicsObject *> sizeControlIter(sizeControlGraphics);
    while (sizeControlIter.hasNext()) {
        SizeControlGraphicsObject *graphics = sizeControlIter.next();
        if (graphics) {
            graphics->userShow(isSelected);
            switch (graphics->getDirection()) {
            case SizeControlGraphicsObject::kDirectionLeftTop:
                graphics->setPos(rc.topLeft());
                break;
            case SizeControlGraphicsObject::kDirectionTopCenter:
                graphics->setPos(rc.center().x(), rc.top());
                break;
            case SizeControlGraphicsObject::kDirectionRightTop:
                graphics->setPos(rc.topRight());
                break;
            case SizeControlGraphicsObject::kDirectionRightCenter:
                graphics->setPos(rc.right(), rc.center().y());
                break;
            case SizeControlGraphicsObject::kDirectionRightBottom:
                graphics->setPos(rc.bottomRight());
                break;
            case SizeControlGraphicsObject::kDirectionBottomCenter:
                graphics->setPos(rc.center().x(), rc.bottom());
                break;
            case SizeControlGraphicsObject::kDirectionLeftBottom:
                graphics->setPos(rc.bottomLeft());
                break;
            case SizeControlGraphicsObject::kDirectionLeftCenter:
                graphics->setPos(rc.left(), rc.center().y());
                break;

            default:
                break;
            }
        }
    }
}

void TextGraphicsAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    painter->save();
    QPen pen;
    if (sourceProxy->getAnnotationMap().value(Annotation::isShowframe, false).toBool()) {
        pen.setColor(getSegmentColor());
        pen.setWidthF(getAnnotationLineWidth());
        painter->setPen(pen);
    } else {
        painter->setPen(Qt::NoPen);
    }
    if (sourceProxy->getAnnotationMap().value(Annotation::isShowframe, false).toBool()) {
        painter->drawRect(boundingRect());
    }
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics && proxyGraphics->isSelected()) {
        QPen boundPen;
        boundPen.setColor(QColor("lightskyblue"));
        boundPen.setWidthF(getAnnotationLineWidth());
        painter->setPen(boundPen);
        painter->drawRect(boundingRect());
    }
    painter->restore();
}

void TextGraphicsAnnotation::onTextChanged()
{
    if (!m_pTextbox) {
        return;
    }
    auto sourceProxy = getSourceProxy();
    auto textDocument = m_pTextbox->document();
    if (!sourceProxy || !textDocument) {
        return;
    }
    auto documentLayout = textDocument->documentLayout();
    bool isInit = false;
    qreal width = documentLayout->documentSize().width();
    qreal setTextWidth = sourceProxy->getAnnotationMap().value(Annotation::textWidth, -1).toReal();
    if (width > 500 && setTextWidth == -1) {
        m_pTextbox->setTextWidth(500);
        sourceProxy->blockSignals(true);
        sourceProxy->setAnnotationProperty(Annotation::textWidth, 500);
        sourceProxy->blockSignals(false);
    }
    if (sourceProxy->size() == QSizeF(0, 0)) {
        isInit = true;
    }
    updateBourndingRect(isInit);
}

void TextGraphicsAnnotation::onCommentSourceChange(QString key, QVariant val)
{
    auto sourceProxy = getSourceProxy();
    if (key == GKD::SOURCE_ANNOTATION_INFO && m_pTextbox != nullptr && sourceProxy != nullptr) {
        QString textcontent = getContent();
        // 如果为空 就删掉
        if (textcontent == "") {
            auto proxyGraphics = getTransformProxyGraphicsObject();
            if (proxyGraphics != nullptr) {
                proxyGraphics->emitDeleteSource();
            }
        }
        QFont font = getFont();
        int alignent = sourceProxy->getAnnotationMap().value(Annotation::alignment, 1).toInt();
        QColor textColor =
                sourceProxy->getAnnotationMap().value(Annotation::textColor, GKD::WIRE_NORMAL_COLOR).value<QColor>();
        if (font != oldFont) {
            oldFont = font;
        }
        if (textcontent.trimmed().isEmpty()) // 只有空格不显示下划线上划线
        {
            font.setUnderline(false);
            font.setStrikeOut(false);
        }
        m_pTextbox->setFont(font);
        m_pTextbox->setDefaultTextColor(textColor);
        m_pTextbox->setAlignment(alignent);
        m_pTextbox->blockSignals(true);
        m_pTextbox->setText(textcontent);
        m_pTextbox->blockSignals(false);
    }
}

void TextGraphicsAnnotation::setTextboxPlainText(const QString &str)
{
    m_pTextbox->setPlainText(str);
}

void TextGraphicsAnnotation::initProperty()
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    if (sourceProxy->getAnnotationMap().isEmpty()) {
        QVariantMap properties;
        properties[Annotation::content] = QString();
        QFont font(QFont(), QApplication::desktop());
        font.setBold(false);
        font.setFamily("Microsoft YaHei UI");

        properties[Annotation::textFont] = font;
        properties[Annotation::alignment] = 1; // 默认居左
        properties[Annotation::segmentColor] = QColor(GKD::WIRE_NORMAL_COLOR);
        properties[Annotation::textColor] = QColor("black");
        properties[Annotation::isShowframe] = false;
        properties[Annotation::segmentWidth] = 3;
        sourceProxy->resetAnnotationMap(properties);
    }
}

void TextGraphicsAnnotation::updateBourndingRect(bool isInit)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy || !m_pTextbox) {
        return;
    }
    auto textDocument = m_pTextbox->document();
    if (!textDocument) {
        return;
    }
    auto documentLayout = textDocument->documentLayout();
    QSizeF size = documentLayout->documentSize();
    QRectF rect;
    rect.setHeight(size.height());
    rect.setWidth(size.width());
    rect.setTopLeft(rect.topLeft() - QPointF(MARGIN, MARGIN));
    rect.setBottomRight(rect.bottomRight() + QPointF(MARGIN, MARGIN));
    this->setSourceBoundingRect(rect);
    sourceProxy->setSize(rect.size());
    if (!isInit) {
        auto proxyGraphics = getTransformProxyGraphicsObject();
        if (proxyGraphics) {
            refreshSizeControlGraphics(proxyGraphics->isSelected());
        }
    }
}

///////自定义QGraphicsTextItem，自定义鼠标指针和textChanged信号
TextboxItem::TextboxItem(QSharedPointer<SourceProxy> source, ICanvasScene *weakCanvasScene,
                         QGraphicsItem *parent /*= nullptr*/)
    : QGraphicsTextItem("", parent), m_source(source), m_weakCanvasScene(weakCanvasScene)
{
    regExp = QRegExp("[:<>/|\\\"\\*\\?\\\\]");
    m_strText = m_source->getAnnotationMap().value(Annotation::content, QVariant()).toString();
    if (m_strText.isEmpty()) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }

    // setFont(m_source->getAnnotationMap().value(Annotation::textFont, QVariant()).value<QFont>());
    setFocus(Qt::MouseFocusReason);
    setPos(0, 0);
    QColor textColor =
            m_source->getAnnotationMap().value(Annotation::textColor, GKD::WIRE_NORMAL_COLOR).value<QColor>();
    setDefaultTextColor(textColor);
    setEnabled(true);

    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    setTextCursor(cursor);
    setPlainText(m_strText);
    setAlignment(m_source->getAnnotationMap().value(Annotation::alignment, 1).toInt());
}

void TextboxItem::updatePosition(QPointF pointDelta)
{
    setPos(this->pos() + pointDelta);
}

void TextboxItem::setText(QString text)
{
    QString newText = text;
    newText.remove(regExp);
    setPlainText(newText);
    if (m_strText != newText) {
        QString strOldText = m_strText;
        m_strText = newText;
    }
}

void TextboxItem::setRegExp(QRegExp exp)
{
    regExp = exp;
}

void TextboxItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    TextGraphicsAnnotation *pBlockText = dynamic_cast<TextGraphicsAnnotation *>(parentItem());
    if (toPlainText() == "" && pBlockText != nullptr && m_weakCanvasScene != nullptr) {
        pBlockText->setStatus(AnnotationSourceGraphicsObject::kStatusCancled);
        m_weakCanvasScene->deleteTransformProxyGraphicsObject(
                m_weakCanvasScene->getTransformProxyGraphicsBySource(m_source));
        return;
    }
    QString newText = toPlainText();
    setText(newText);
    m_source->setAnnotationProperty(Annotation::content, newText);
    if (pBlockText) {
        pBlockText->setStatus(AnnotationSourceGraphicsObject::kStatusFinished);
    }
    QGraphicsTextItem::focusOutEvent(event);
}

void TextboxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        QGraphicsTextItem::contextMenuEvent(event);
    }
}

void TextboxItem::setAlignment(int alignment)
{
    QTextDocument *document = this->document();
    QTextOption option = document->defaultTextOption();
    option.setAlignment(Utility::transAlignment(alignment));
    document->setDefaultTextOption(option);
}

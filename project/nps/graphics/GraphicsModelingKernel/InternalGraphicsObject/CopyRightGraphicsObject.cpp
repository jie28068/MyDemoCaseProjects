#include "CopyRightGraphicsObject.h"
#include "BusinessHooksServer.h"
#include "CanvasContext.h"
#include "EditableGraphicsTextItem.h"
#include "ICanvasScene.h"
#include "Utility.h"

#include <QDateTime>
#include <QPainter>

const static int MAX_TEXT_LENGTH = 512;
CopyRightGraphicsObject::CopyRightGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent /*= 0*/)
    : GraphicsLayer(canvasScene, parent)
{
    if (!canvasScene) {
        return;
    }
    canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    initTableTitle();
    initItem();
    hookserver = canvasContext->getBusinessHooksServer();
    itemList << softVersionItem << canvasTypeItem << canvasNameItem << descriptionItem << dateItem << authorItem
             << ver_titleItem << type_titleItem << name_titleItem << desc_titleItem << date_titleItem
             << author_titleItem;
    setLayerFlag(GraphicsLayer::kLegendGraphicsLayer);
    connect(canvasContext.data(), SIGNAL(canvasContextChanged(QString, QVariant)), this,
            SLOT(onContextChanged(QString, QVariant)));
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges, true);
    CanvasProperty &canvasProperty = canvasContext->getCanvasProperty();
    connect(&canvasProperty, &NPSPropertyManager::propertyChanged, this,
            &CopyRightGraphicsObject::onContextPropertyChanged);
}

CopyRightGraphicsObject::~CopyRightGraphicsObject() { }

QRectF CopyRightGraphicsObject::boundingRect() const
{
    return QRectF(11, 0, 398, 100);
}

void CopyRightGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    if (isSelected()) {
        QColor penColor("lightskyblue");
        painter->setPen(QPen(penColor, 2, Qt::SolidLine));
    } else {
        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    }
    painter->drawRect(boundingRect());
    painter->restore();
}

int CopyRightGraphicsObject::type() const
{

    return kLegendGraphics;
}

QVariant CopyRightGraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    auto canvasScene = getCanvasScene();
    switch (change) {
    case ItemPositionChange: {
        QPointF pt = value.toPointF();
        if (canvasScene) {
            QRectF rc = boundingRect();
            QRectF changingRect = QRectF(pt + rc.topLeft(), QSizeF(rc.width(), rc.height()));
            auto path = canvasScene->getTransformProxyGraphicsPainterPath();
            if (path.intersects(changingRect)) {
                return pos();
            }
        }
        pt = Utility::pointAlignmentToGrid(pt, 10);

        return pt;
    } break;
    default: {
    }
    }
    return GraphicsLayer::itemChange(change, value);
}

void CopyRightGraphicsObject::onContextChanged(QString key, QVariant value)
{
    if (!canvasContext) {
        return;
    }
    if (GKD::CANVAS_DESCRIPTION == key) {
        setText(value.toString(), descriptionItem);
    } else if (GKD::CANVAS_NAME == key) {
        setText(value.toString(), canvasNameItem);
    } else if (GKD::CANVAS_AUTHOR == key) {
        setText(value.toString(), authorItem);
    } else if (GKD::CANVAS_LastModify_Time == key) {
        setText(canvasContext->getLastModifyTime(), dateItem);
    } else {
        return;
    }
}
void CopyRightGraphicsObject::setText(const QString &text, BorderTextGraphicsObject<QGraphicsTextItem> *Item)
{
    // 变量在if中仅赋值，在elidedText中使用
    int width;
    if (Item == descriptionItem || Item == canvasNameItem) {
        width = 311;
        // 判断描述文本是否超过限制
        if (text.length() > MAX_TEXT_LENGTH && Item == descriptionItem) {
            if (hookserver != nullptr) {
                hookserver->showWaringDialg(tr("The description character length cannot exceed [%1] characters")
                                                    .arg(MAX_TEXT_LENGTH)); // 描述长度不超过[%1]个字符
            }
            return;
        }
    } else if (Item == authorItem || Item == dateItem || Item == softVersionItem || Item == canvasTypeItem) {
        width = 105;
        // 判断作者文本是否超过限制
        if (text.length() > (MAX_TEXT_LENGTH / 4) && Item == authorItem) {
            if (hookserver != nullptr) {
                hookserver->showWaringDialg(tr("The author character length cannot exceed [%1] characters")
                                                    .arg(MAX_TEXT_LENGTH / 4)); // 作者长度不超过[%1]个字符
            }
            return;
        }
    } else {
        width = 88;
    }
    QString strTemp = QFontMetrics(Item->font()).elidedText(text, Qt::ElideRight, width);
    auto textItem = Item->getTextItem();
    if (textItem != nullptr) {
        textItem->setPlainText(strTemp);
    }

    Item->setBoundingRect(Item->boundingRect());
}

QFont CopyRightGraphicsObject::getFont()
{
    CanvasProperty &canvasProperty = canvasContext->getCanvasProperty();
    QFont font = canvasProperty.getCopyRightFont();
    // 若属性中没存字体 返回默认值
    if (font == QFont()) {
        return QFont("长仿宋体", 9);
    } else {
        return font;
    }
}
void CopyRightGraphicsObject::initTableTitle()
{
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    ver_titleItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, tr("Version"), Qt::AlignCenter, this);
    ver_titleItem->setfont(getFont());
    ver_titleItem->setPos(12, 25);
    ver_titleItem->setBoundingRect(QRectF(0, 0, 88, 25));

    type_titleItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, tr("Type"), Qt::AlignCenter, this);
    type_titleItem->setfont(getFont());
    type_titleItem->setPos(12, 50);
    type_titleItem->setBoundingRect(QRectF(0, 0, 88, 25));

    name_titleItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, tr("Name"), Qt::AlignCenter, this);
    name_titleItem->setfont(getFont());
    name_titleItem->setPos(12, 0);
    name_titleItem->setBoundingRect(QRectF(0, 0, 88, 25));

    desc_titleItem =
            new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, tr("Explanation"), Qt::AlignCenter, this);
    desc_titleItem->setfont(getFont());
    desc_titleItem->setPos(12, 75);
    desc_titleItem->setBoundingRect(QRectF(0, 0, 88, 25));

    date_titleItem =
            new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, tr("Last Modify"), Qt::AlignCenter, this);
    date_titleItem->setfont(getFont());
    date_titleItem->setPos(220, 50);
    date_titleItem->setBoundingRect(QRectF(0, 0, 85, 25));

    author_titleItem =
            new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, tr("Author"), Qt::AlignCenter, this);
    author_titleItem->setfont(getFont());
    author_titleItem->setPos(220, 25);
    author_titleItem->setBoundingRect(QRectF(0, 0, 85, 25));
}

void CopyRightGraphicsObject::initItem()
{
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasContext || !canvasScene) {
        return;
    }
    QString versionStr = canvasContext->version();
    softVersionItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, versionStr, Qt::AlignCenter, this);
    softVersionItem->setfont(getFont());
    softVersionItem->setPos(100, 25);
    softVersionItem->setBoundingRect(QRectF(0, 0, 120, 25));

    CanvasContext::Type typeEnum = canvasContext->type();
    QString typeStr;
    if (typeEnum == CanvasContext::Type::kElectricalType || typeEnum == CanvasContext::Type::kElecUserDefinedType) {
        typeStr = tr("Electrical");
    } else if (typeEnum == CanvasContext::Type::kUserDefinedFrameType) {
        typeStr = tr("Composite Model"); // 复合模型;
    } else {
        typeStr = tr("Control");
    }
    canvasTypeItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, typeStr, Qt::AlignCenter, this);
    canvasTypeItem->setfont(getFont());
    canvasTypeItem->setPos(100, 50);
    canvasTypeItem->setBoundingRect(QRectF(0, 0, 120, 25));

    canvasNameItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, "", Qt::AlignCenter, this);
    canvasNameItem->setfont(getFont());
    canvasNameItem->setPos(100, 0);
    canvasNameItem->setBoundingRect(QRectF(0, 0, 310, 25));
    setText(canvasContext->name(), canvasNameItem);

    descriptionItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, " ", Qt::AlignCenter, this);
    descriptionItem->setfont(getFont());
    descriptionItem->setPos(100, 75);
    descriptionItem->setBoundingRect(QRectF(0, 0, 310, 25));
    setText(canvasContext->description(), descriptionItem);

    QString dateStr;
    QDateTime dateTime = QDateTime::fromString(canvasContext->getLastModifyTime(), Qt::ISODate);
    if (dateTime.isValid()) {
        QDate date = dateTime.date();
        dateStr = date.toString("yyyy-MM-dd");
    } else {
        dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    }
    dateItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, dateStr, Qt::AlignCenter, this);
    dateItem->setfont(getFont());
    dateItem->setPos(305, 50);
    dateItem->setBoundingRect(QRectF(0, 0, 105, 25));

    authorItem = new BorderTextGraphicsObject<QGraphicsTextItem>(canvasScene, "", Qt::AlignCenter, this);
    authorItem->setfont(getFont());
    authorItem->setPos(305, 25);
    authorItem->setBoundingRect(QRectF(0, 0, 105, 25));
    setText(canvasContext->author(), authorItem);
}

void CopyRightGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (hookserver != nullptr)
        hookserver->showPropertyDialog();
}

void CopyRightGraphicsObject::onEditClicked()
{
    if (hookserver != nullptr)
        hookserver->showPropertyDialog();
}

void CopyRightGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (rgMenu.isNull()) {
        rgMenu = QSharedPointer<QMenu>(new QMenu());
    }
    if (rgMenu->isEmpty()) {
        rightGraphicsMenu(rgMenu);
    }
    if (rgMenu->isEmpty()) {
        return GraphicsLayer::contextMenuEvent(event);
    }
    ActionManager::getInstance().getAction(ActionManager::ElementProperty)->setEnabled(true);
    connect(ActionManager::getInstance().getAction(ActionManager::ElementProperty), SIGNAL(triggered()), this,
            SLOT(onEditClicked()), Qt::UniqueConnection);
    rgMenu->setObjectName("uniformStyleMenu");
    rgMenu->exec(event->screenPos());
    event->accept();

    disconnect(ActionManager::getInstance().getAction(ActionManager::ElementProperty), 0, this, 0);
}

void CopyRightGraphicsObject::rightGraphicsMenu(QSharedPointer<QMenu> rgMenu)
{
    rgMenu->addAction(ActionManager::getInstance().getAction(ActionManager::ElementProperty));
}

void CopyRightGraphicsObject::onContextPropertyChanged(QString key, QVariant oldValue, QVariant value)
{
    if (FONT_COPYRIGHT_PROPERTY_KEY == key) {
        foreach (BorderTextGraphicsObject<QGraphicsTextItem> *item, itemList) {
            item->setfont(value.value<QFont>());
            item->setBoundingRect(item->boundingRect());
        }
        setText(canvasContext->description(), descriptionItem);
        setText(canvasContext->name(), canvasNameItem);
        setText(canvasContext->author(), authorItem);
        setText(canvasContext->getLastModifyTime(), dateItem);
        setText(tr("Last Modify"), date_titleItem);
        setText(tr("Explanation"), desc_titleItem);
        canvasContext->getCanvasProperty().setCopyRightFont(value.value<QFont>());
    }
}

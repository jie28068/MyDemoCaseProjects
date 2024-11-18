#include "EditableGraphicsTextItem.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"

#include "SourceNameTextGraphicsObject.h"
#include <QCursor>
#include <QGraphicsDropShadowEffect>
#include <QStyleOptionGraphicsItem>
#include <QTextCursor>
#include <QTextDocument>

EditableGraphicsTextItem::EditableGraphicsTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent), wipeOffRegexp(QRegExp())
{
    QTextDocument *doc = document();
    connect(doc, SIGNAL(contentsChanged()), this, SLOT(onDocumentContentsChanged()));
    isShowBounds = false;
    maxStringLength = 0xffffff;
}

EditableGraphicsTextItem::~EditableGraphicsTextItem()
{
    disconnect();
}

void EditableGraphicsTextItem::setWipeOffRegexp(QRegExp exp)
{
    wipeOffRegexp = exp;
}

void EditableGraphicsTextItem::showBoundings(bool show)
{
    isShowBounds = show;
}

void EditableGraphicsTextItem::setMaxStringLength(int length)
{
    if (length <= 0) {
        return;
    }
    maxStringLength = length;
}

void EditableGraphicsTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::IBeamCursor);
    QGraphicsTextItem::hoverEnterEvent(event);
}

void EditableGraphicsTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    QGraphicsTextItem::hoverLeaveEvent(event);
}

void EditableGraphicsTextItem::onDocumentContentsChanged()
{
    QString contents = toPlainText();
    QString originalContents = contents;
    if (contents.length() > maxStringLength) {
        contents = contents.mid(0, maxStringLength);
    }

    if (wipeOffRegexp.isValid()) {
        contents.remove(wipeOffRegexp);
    }
    if (contents != originalContents) {
        setPlainText(contents);
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
    }
}

void EditableGraphicsTextItem::focusInEvent(QFocusEvent *event)
{
    emit startTextEdit();
    QGraphicsTextItem::focusInEvent(event);
}

void EditableGraphicsTextItem::focusOutEvent(QFocusEvent *event)
{
    QRegExp regexp = (QRegExp("[`·！!@#$￥%……^&（）——=+{}【】,.，。《》<>：;；'‘'’“”"
                              ":<>/|\\\"\\*\\?\\\\]{1,128}"));
    QGraphicsItem *pItem = parentItem();
    QGraphicsItem *ppItem = nullptr;
    if (pItem) {
        ppItem = pItem->parentItem();
    }
    if (regexp.isValid() && pItem && ppItem) {
        QString contents = toPlainText();
        QString lastContents = contents;
        TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(ppItem);
        if (proxyGraphics) {
            auto sourceProxy = proxyGraphics->getSourceProxy();
            if (sourceProxy) {
                QString oldName = sourceProxy->name();
                contents = contents.remove(regexp).trimmed();
                contents.replace("\n", "");
                lastContents.replace("\n", "");
                if (contents == lastContents.trimmed() && !contents.isEmpty() && contents != oldName
                    && proxyGraphics->checkNameValid(contents)) {
                    name = QPair<QString, QString>(oldName, contents);
                    sourceProxy->setName(contents);
                } else {
                    setPlainText(oldName);
                    proxyGraphics->updatePosition();
                }
            }
        }
    }
    emit finishTextEdit();
    QGraphicsTextItem::focusOutEvent(event);
}

void EditableGraphicsTextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) { }

void EditableGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!isShowBounds) {
        if (hasFocus()) {
            QStyleOptionGraphicsItem optionx;
            optionx.initFrom(widget);
            optionx.state = QStyle::State_None;
            // optionx.state &= ~QStyle::State_Selected;
            return QGraphicsTextItem::paint(painter, &optionx, widget);
        } else {
            QGraphicsTextItem::paint(painter, option, widget);
        }
    } else {
        QGraphicsTextItem::paint(painter, option, widget);
    }
}

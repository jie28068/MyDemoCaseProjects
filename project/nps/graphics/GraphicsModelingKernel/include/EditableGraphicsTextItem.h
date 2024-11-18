#ifndef EDITABLEGRAPHICSTEXTITEM_H
#define EDITABLEGRAPHICSTEXTITEM_H

#include "Global.h"
#include <QGraphicsTextItem>

class GRAPHICSMODELINGKERNEL_EXPORT EditableGraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    EditableGraphicsTextItem(QGraphicsItem *parent = 0);
    ~EditableGraphicsTextItem();

    /// @brief 设置对内容清洗正则表达式
    /// @param exp
    void setWipeOffRegexp(QRegExp exp);

    void showBoundings(bool show);

    void setMaxStringLength(int length);

public:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void startTextEdit();
    void finishTextEdit();

public:
    QPair<QString, QString> name;

protected slots:
    void onDocumentContentsChanged();

private:
    /// @brief 数据清洗正则表达式
    QRegExp wipeOffRegexp;
    bool isShowBounds;
    int maxStringLength;
};

#endif // EDITABLEGRAPHICSTEXTITEM_H

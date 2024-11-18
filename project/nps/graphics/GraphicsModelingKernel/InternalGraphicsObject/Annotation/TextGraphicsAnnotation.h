#ifndef TEXTGRAPHICSANNOTATION_H
#define TEXTGRAPHICSANNOTATION_H

#include "AnnotationSourceGraphicsObject.h"
#include "EditableGraphicsTextItem.h"
#include "TextGraphicsObject.h"

#include <QRegExp>

class TextboxItem;
class SizeControlGraphicsObject;

class TextGraphicsAnnotation : public AnnotationSourceGraphicsObject
{
    Q_OBJECT

public:
    TextGraphicsAnnotation(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source, QGraphicsItem *parent = 0);
    ~TextGraphicsAnnotation();

    void setTextboxPlainText(const QString &str);

    virtual void createInternalGraphics();

    void refreshSizeControlGraphics(bool isSelected = false);

    // virtual QRectF boundingRect() const;

    virtual QPainterPath shape() const;

    /// @brief �û��Զ���˵�
    virtual void userMenu(QSharedPointer<QMenu> menu);

    void updateBourndingRect(bool isInit = false);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    virtual void initProperty();

private slots:
    void onTextChanged();
    void onCommentSourceChange(QString key, QVariant val);

    void onProcessSizeChanged(QPointF offset);
    void onReadySizeChange();
    void onFinishSizeChange();

private:
    TextboxItem *m_pTextbox;
    QList<SizeControlGraphicsObject *> sizeControlGraphics;
    QPointF lastPos;
    QFont oldFont;
};

// 自定义QGraphicsTextItem，自定义鼠标指针
class TextboxItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    TextboxItem(QSharedPointer<SourceProxy> source, ICanvasScene *weakCanvasScene, QGraphicsItem *parent = nullptr);
    void setText(QString text);
    void setAlignment(int alignment);

    void updatePosition(QPointF pointDelta);

    void setRegExp(QRegExp exp);
signals:
    void textChanged(const QString &strText, const QString &strOldText);

protected:
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    QString m_strText;
    QSharedPointer<SourceProxy> m_source;
    ICanvasScene *m_weakCanvasScene;
    QRegExp regExp;
};

#endif // TEXTGRAPHICSANNOTATION_H

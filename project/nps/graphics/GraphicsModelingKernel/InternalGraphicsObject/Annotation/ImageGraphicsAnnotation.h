#ifndef IMAGEGRAPHICSANNOTATION_H
#define IMAGEGRAPHICSANNOTATION_H

#include "AnnotationSourceGraphicsObject.h"

class ImageGraphicsAnnotation : public AnnotationSourceGraphicsObject
{
    Q_OBJECT

public:
    ImageGraphicsAnnotation(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source, QGraphicsItem *parent = 0);
    ~ImageGraphicsAnnotation();

    virtual QSizeF getMinimumSize() const { return QSizeF(10.0f, 10.0f); };

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private slots:
    void onImagePixmapChanged(QString key, QVariant data);

private:
    QPixmap imagePixmap;
    QSharedPointer<SourceProxy> m_source;
};

#endif // IMAGEGRAPHICSANNOTATION_H

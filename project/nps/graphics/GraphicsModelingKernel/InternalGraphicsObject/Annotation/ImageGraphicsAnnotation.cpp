#include "ImageGraphicsAnnotation.h"
#include "SourceProxy.h"

#include <QFile>
#include <QPainter>

ImageGraphicsAnnotation::ImageGraphicsAnnotation(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                 QGraphicsItem *parent)
    : AnnotationSourceGraphicsObject(canvasScene, source, parent)
{
    m_source = source;
    QString imagePath = source->getAnnotationMap().value(Annotation::iconPath, "").toString();
    QFile file(imagePath);
    if (file.exists()) {
        imagePixmap = QPixmap(imagePath);
    } else {
        auto variant = m_source->getSourceProperty().getAnnotationImage();
        imagePixmap = variant.value<QPixmap>();
    }
    status = kStatusFinished;
    setLayerFlag(kCommentGraphicsLayer);
    connect(source.data(), SIGNAL(sourceChange(QString, QVariant)), this, SLOT(onImagePixmapChanged(QString, QVariant)),
            Qt::UniqueConnection);
}

ImageGraphicsAnnotation::~ImageGraphicsAnnotation() { }
void ImageGraphicsAnnotation::onImagePixmapChanged(QString key, QVariant data)
{
    if (key == GKD::SOURCE_ANNOTATION_INFO) {
        QString imagePath = m_source->getAnnotationMap().value(Annotation::iconPath, "").toString();
        if (!imagePath.isEmpty()) {
            imagePixmap = QPixmap(imagePath);
        }
    }
}

void ImageGraphicsAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawPixmap(boundingRect(), imagePixmap,
                        QRectF(0, 0, imagePixmap.size().width(), imagePixmap.size().height()));
}

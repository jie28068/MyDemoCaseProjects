#include "InternalAnnotationFactory.h"
#include "AnnotationSourceGraphicsObject.h"
#include "GeometricGraphicsAnnotation.h"
#include "GraphicsKernelDefinition.h"
#include "ImageGraphicsAnnotation.h"
#include "SourceProxy.h"
#include "TextGraphicsAnnotation.h"

InternalAnnotationFactory::InternalAnnotationFactory() : ICanvasGraphicsObjectFactory() { }

SourceGraphicsObject *InternalAnnotationFactory::createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                            QSharedPointer<SourceProxy> source)
{
    QString prototypeName = source->prototypeName();
    if (prototypeName.compare(CircleAnnotationStr) == 0 || prototypeName.compare(RectAnnotationStr) == 0
        || prototypeName.compare(LineSegmentAnnotationStr) == 0
        || prototypeName.compare(LineSegmentArrowAnnotationStr) == 0
        || prototypeName.compare(BrokenLineAnnotationStr) == 0
        || prototypeName.compare(BrokenLineArrowAnnotationStr) == 0 || prototypeName.compare(PolygonAnnotationStr) == 0
        || prototypeName.compare(ArcAnnotationStr) == 0) {
        return new GeometricGraphicsAnnotation(canvasScene, source);
    } else if (prototypeName.compare(ImageAnnotationStr) == 0) {
        return new ImageGraphicsAnnotation(canvasScene, source);
    } else if (prototypeName.compare(TextAnnotationStr) == 0) {
        return new TextGraphicsAnnotation(canvasScene, source);
    } else {
        return nullptr;
    }

    return nullptr;
}

PortGraphicsObject *InternalAnnotationFactory::createPortGraphicsObject(ICanvasScene *canvasScene,
                                                                        SourceGraphicsObject *sourceGraphics,
                                                                        QSharedPointer<PortContext> context)
{
    return nullptr;
}

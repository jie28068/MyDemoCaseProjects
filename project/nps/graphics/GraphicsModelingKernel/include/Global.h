#ifndef GLOBAL_H
#define GLOBAL_H

#include <QBoxLayout>
#include <QColor>
#include <QFont>
#include <QGraphicsItem>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPointF>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSize>
#include <QStringList>
#include <QTransform>
#include <QVariant>
#include <QVector>
#include <QtCore/qglobal.h>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifdef GRAPHICSMODELINGKERNEL_LIB
#define GRAPHICSMODELINGKERNEL_EXPORT Q_DECL_EXPORT
#else
#define GRAPHICSMODELINGKERNEL_EXPORT Q_DECL_IMPORT
#endif

#define BeginGraphicsModelNameSpace                                                                                    \
    namespace kcc {                                                                                                    \
    namespace GraphicsModeling {

#define EndGraphicsModelNameSpace                                                                                      \
    }                                                                                                                  \
    }

#endif // GLOBAL_H
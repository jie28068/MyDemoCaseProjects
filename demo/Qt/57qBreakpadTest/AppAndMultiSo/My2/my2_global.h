#ifndef MY2_GLOBAL_H
#define MY2_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MY2_LIBRARY)
#  define MY2SHARED_EXPORT Q_DECL_EXPORT
#else
#  define MY2SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MY2_GLOBAL_H

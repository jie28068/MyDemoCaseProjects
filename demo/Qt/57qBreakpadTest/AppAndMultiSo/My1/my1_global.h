#ifndef MY1_GLOBAL_H
#define MY1_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MY1_LIBRARY)
#  define MY1SHARED_EXPORT Q_DECL_EXPORT
#else
#  define MY1SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MY1_GLOBAL_H

﻿#ifndef PLUGINUIA_GLOBAL_H
#define PLUGINUIA_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef PLUGINUIA_LIB
#define PLUGINUIA_EXPORT Q_DECL_EXPORT
#else
#define PLUGINUIA_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGINUIA_GLOBAL_H

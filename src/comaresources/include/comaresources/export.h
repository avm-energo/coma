#pragma once

#include <QtCore/qglobal.h>

#if defined(COMARES_SHARED_EXPORT)
#define COMARES_EXPORT Q_DECL_EXPORT
#elif defined(COMARES_SHARED_IMPORT)
#define COMARES_EXPORT Q_DECL_IMPORT
#else
#define COMARES_EXPORT
#endif

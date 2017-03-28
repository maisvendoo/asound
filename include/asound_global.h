#ifndef ASOUND_GLOBAL_H
#define ASOUND_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ASOUND_LIBRARY)
#  define ASOUNDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ASOUNDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ASOUND_GLOBAL_H

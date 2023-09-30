#ifndef KLEINERBRAUHELFERCORE_GLOBAL_H
#define KLEINERBRAUHELFERCORE_GLOBAL_H

#ifdef QT_CORE_LIB
#  include <QtCore/qglobal.h>
#else
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
#endif

#if defined(KLEINERBRAUHELFERCORE_LIBRARY_STATIC)
#  define LIB_EXPORT
#else
#  if defined(KLEINERBRAUHELFERCORE_LIBRARY_SHARED)
#    define LIB_EXPORT Q_DECL_EXPORT
#  else
#    define LIB_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // KLEINERBRAUHELFERCORE_GLOBAL_H

﻿#ifndef DAPLUGINGLOBAL_H
#define DAPLUGINGLOBAL_H
#include <QtCore/QtGlobal>
#include "DAGlobals.h"

#if defined(DAPLUGINSUPPORT_BUILDLIB)
#define DAPLUGINSUPPORT_API Q_DECL_EXPORT
#else
#define DAPLUGINSUPPORT_API
#endif

#endif // DAPLUGINGLOBAL_H

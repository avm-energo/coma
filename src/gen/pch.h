#pragma once

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifndef __GNUC__
#define MAXFLOAT 3.40282347E+38F
#else
#include <cmath>
#endif

#include <QObject>

typedef unsigned char byte;

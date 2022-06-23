#include "error.h"

QStringList &operator<<(QStringList &l, const ErrorMsg &obj)
{
    l << obj.DateTime << obj.file << QString::number(obj.line, 10) << obj.msg;
    return l;
}

QDebug operator<<(QDebug debug, const ErrorMsg &error)
{
    debug.nospace() << error.DateTime << ":" << error.file << ":" << error.line << ":" << error.msg;
    return debug.maybeSpace();
}

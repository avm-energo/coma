#include "error.h"

#include <QDateTime>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

QStringList &operator<<(QStringList &l, const ErrorMsg &obj)
{
    l << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << obj.file << QString::number(obj.line, 10)
      << obj.msg;
    return l;
}

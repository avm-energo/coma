#include "comaversion/comaversion.h"

#include <gversion.h>

QString GitVersion::getGitHash() const
{
    return QString { gitHash };
}

unsigned long long GitVersion::getGitCounter() const
{
    return gitCommitCounter;
}

QString GitVersion::getConfigVersion() const
{
    QString str;
    auto counter = gitCommitCounter;
    while (counter != 0)
    {
        str.prepend('.');
        str.prepend(QString::number(counter % 10));
        counter /= 10;
    }
    if (str.back() == '.')
        str.push_back('0');
    return str;
}

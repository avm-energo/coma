#include "comaversion.h"

#include <gversion.h>

QString GitVersion::getGitHash()
{
    const QString buffer(gitHash);
    return buffer;
}

unsigned long long GitVersion::getGitCounter()
{
    return gitCommitCounter;
}

QString GitVersion::getConfigVersion()
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

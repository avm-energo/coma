#pragma once

#include <QString>

class GitVersion
{
public:
    QString getGitHash();
    unsigned long long getGitCounter();
    QString getConfigVersion();

private:
};

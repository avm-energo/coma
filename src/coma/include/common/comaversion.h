#pragma once

#include <QString>

class GitVersion
{
public:
    QString getGitHash() const;
    unsigned long long getGitCounter() const;
    QString getConfigVersion() const;
};

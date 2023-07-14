#pragma once

#include "s2configuration.h"

namespace S2
{

class DataManager : public QObject
{
    Q_OBJECT
private:
    S2ConfigStorage &storage;
    S2Configuration defaultConfig;
    S2Configuration workingConfig;

public:
    explicit DataManager(QObject *parent = nullptr);

    S2ConfigStorage &getStorage() noexcept;
    const S2ConfigStorage &getStorage() const noexcept;
};

}

using S2DataManager = S2::DataManager;

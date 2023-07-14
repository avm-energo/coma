#pragma once

#include "s2configuration.h"

namespace S2
{

enum class BoardConfig : bool
{
    Base = false,
    Mezz = true
};

struct BoardConfiguration
{
    S2Configuration defaultConfig;
    S2Configuration workingConfig;
};

class DataManager : public QObject
{
    Q_OBJECT
private:
    S2ConfigStorage &storage;
    std::map<BoardConfig, BoardConfiguration> data;
    BoardConfig currentParseTarget;

public:
    explicit DataManager(QObject *parent = nullptr);

    S2ConfigStorage &getStorage() noexcept;
    const S2ConfigStorage &getStorage() const noexcept;
    void clear() noexcept;
    bool isOneBoard() const noexcept;

public slots:
    void startNewConfig();
    void configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count);
};

}

using S2BoardConfig = S2::BoardConfiguration;
using S2DataManager = S2::DataManager;

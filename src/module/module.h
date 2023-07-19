#pragma once

#include "board.h"
#include "configstorage.h"

namespace S2
{
class ConfigStorage;
class DataManager;
}

class Module : public QObject
{
    Q_OBJECT
private:
    bool checks;
    bool state;
    quint16 typeB;
    quint16 typeM;
    const Modules::StartupInfoBlock &sInfoBlock;

public:
    explicit Module(const bool criticalCheck = true,
        const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        QObject *parent = nullptr);

    bool loadS2Settings(S2::ConfigStorage &s2Storage);
    bool loadSettings(ConfigStorage &mStorage, S2::DataManager &s2manager);

private:
    void loadModuleSettings(ConfigStorage &mStorage, S2::DataManager &s2manager);

private slots:
    void parseErrorHandle(const QString errMsg);
};

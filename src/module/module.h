#pragma once

#include "board.h"
#include "configstorage.h"

namespace S2
{
class ConfigStorage;
}

class Module : public QObject
{
    Q_OBJECT
private:
    bool checks;
    bool state;
    const Modules::StartupInfoBlock &sInfoBlock;
    ConfigStorage *mStorage;

public:
    explicit Module(const bool criticalCheck = true,
        const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        QObject *parent = nullptr);

    bool loadS2Settings(S2::ConfigStorage *s2Storage);
    bool loadSettings();

private:
    void loadModuleSettings(const quint16 &typeB, const quint16 &typeM);

private slots:
    void parseErrorHandle(const QString errMsg);
};

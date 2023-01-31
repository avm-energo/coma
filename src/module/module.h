#pragma once

#include "board.h"
#include "configstorage.h"

#include <QtXml>

class Module : public QObject
{
    Q_OBJECT
private:
    bool checks;
    const Modules::StartupInfoBlock &sInfoBlock;
    ConfigStorage *mStorage;

public:
    explicit Module(const bool criticalCheck = true,
        const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        QObject *parent = nullptr);
    bool loadSettings();

private:
    bool loadS2Settings();
    bool loadModuleSettings(const QString &filename, const quint16 &typeB, const quint16 &typeM);
};

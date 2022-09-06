#pragma once

#include "board.h"
#include "configstorage.h"

#include <QtXml>

class NewModule : public QObject
{
    Q_OBJECT
private:
    const Modules::StartupInfoBlock &sInfoBlock;
    ConfigStorage *mStorage;

public:
    explicit NewModule(const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        QObject *parent = nullptr);
    bool loadSettings();

private:
    bool isFileExist(const QString &filename);
    QDomDocument getFileContent(const QString &filename);
    bool loadS2Settings();
    bool loadModuleSettings(const QString &filename, const quint16 &typeB, const quint16 &typeM);
};

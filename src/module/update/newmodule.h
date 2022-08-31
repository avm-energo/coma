#pragma once

#include "../board.h"
#include "configstorage.h"

#include <QtXml>

class NewModule : public QObject
{
    Q_OBJECT
public:
    explicit NewModule(const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        QObject *parent = nullptr);
    bool loadSettings();

private:
    bool isFileExist(const QString &filename);
    QDomDocument getFileContent(const QString &filename);
    bool loadS2Settings();
    bool loadModuleSettings(const QString &filename);

    // ModuleSettings mSettings;
    ConfigStorage *mStorage;
    // S2XmlParser *mS2Parser;
};

#pragma once

#include "../board.h"
#include "../modulesettings.h"
#include "newxmlparser.h"

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
    bool loadMainSettings(const QString &filename);

    // GlobalSettings m_gsettings;
    ModuleSettings mSettings;
    NewXmlParser *mParser;
};

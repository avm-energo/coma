#include "xmlconfigparser.h"

#include "gen/stdfunc.h"

#include <QtXml>

QMap<QString, quint16> XmlConfigParser::NameIdMap = QMap<QString, quint16>();

void XmlConfigParser::ParseS2ConfigToMap()
{
    QDir homeDir(StdFunc::GetSystemHomeDir());
    auto flist = homeDir.entryList(QDir::Files).filter("s2files.xml");
    if (flist.count() > 0)
    {
        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(homeDir.filePath(flist[0]));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            if (domDoc->setContent(moduleFile))
            {
                if (NameIdMap.count() > 0)
                    NameIdMap.clear();
                auto recordElement = domDoc->documentElement().firstChild().nextSibling();
                while (!recordElement.isNull())
                {
                    auto idElement = recordElement.firstChild().toElement();
                    auto nameElement = idElement.nextSibling().toElement();
                    auto idValue = static_cast<quint16>(idElement.text().toInt());
                    auto nameValue = nameElement.text();
                    NameIdMap.insert(nameValue, idValue);
                    recordElement = recordElement.nextSibling();
                }
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
    else
    {
        qCritical("Error: \"s2files.xml\" not found");
    }
}

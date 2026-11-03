#include "xml/xmlparser/xmls2parser.h"

#include <common/appconfig.h>
#include <device/current_device.h>
#include <libavm-gen/datatypes.h>
#include <libavm-gen/xml/xmlbase.h>
#include <libavm-gen/xml/xmlparse.h>
#include <s2/s2datatypes.h>
#include <libavm-gen/settings.h>
#include <libavm-gen/stdfunc.h>
#include <xml/xmltags.h>

#include <QDir>
#include <QtDebug>

Xml::XmlS2Parser::XmlS2Parser(QObject *parent)
{
    constexpr auto filename = "s2files.xml";
    m_content = XmlBase::getXMLFirstElementFromFile(filename, tags::s2files);
}

bool Xml::XmlS2Parser::readMinVersion(const QDomElement &root, const QString &tagName, quint32 &version)
{
    const auto text = root.firstChildElement(tags::firmwares).firstChildElement(tags::fw_prefix + tagName).text();
    if (text.isEmpty())
        return false;
    version = StdFunc::StrToVer(text);
    return true;
}

Xml::XmlS2Parser::XmlS2Parser(const Device::CurrentDevice *device, QObject *parent) : XmlS2Parser(parent)
{
    static const QString oldDirName("old");

    if (m_content.isNull() || device == nullptr)
        return;
    // Имя тега совпадает с именем конфигурации модуля, например, a243
    const auto tagName = QString("%1%2").arg(device->getBaseType(), 2, 16, QChar('0')).arg(device->getMezzType(), 2, 16, QChar('0'));
    quint32 minVersion = 0;
    // Тега нет — версия не ограничена
    if (!readMinVersion(m_content, tagName, minVersion) || !device->isOutdatedFirmware(minVersion))
        return;

    const QDir oldDir(QDir(Settings::configDir()).filePath(oldDirName));
    const auto candidates = oldDir.entryList({ "s2files*.xml" }, QDir::Files, QDir::Name);
    for (auto it = candidates.crbegin(); it != candidates.crend(); ++it)
    {
        const auto candidate = oldDirName + "/" + *it;
        const auto content = XmlBase::getXMLFirstElementFromFile(candidate, tags::s2files);
        if (content.isNull())
            continue;
        if (readMinVersion(content, tagName, minVersion) && !device->isOutdatedFirmware(minVersion))
        {
            qInfo() << "Для версии ВПО" << StdFunc::VerToStr(device->bsi().data(Device::BsiIndexes::Fwver))
                    << "выбран файл" << candidate;
            m_content = content;
            return;
        }
    }
}

void Xml::XmlS2Parser::parse()
{
    if (!m_content.isNull())
    {
        XmlParse::parseNode(m_content, tags::conf_tabs, [this](const QDomNode &tabNode) { parseConfigTab(tabNode); });
        XmlParse::parseNode(m_content, tags::records, [this](const QDomNode &recordNode) { parseRecord(recordNode); });
    }
}

#include "xml/xmlparser/xmls2parser.h"

#include <common/appconfig.h>
#include <gen/datatypes.h>
#include <gen/xml/xmlbase.h>
#include <gen/xml/xmlparse.h>
#include <s2/s2datatypes.h>
#include <xml/xmltags.h>

Xml::XmlS2Parser::XmlS2Parser(QObject *parent)
{
    constexpr auto filename = "s2files.xml";
    m_content = XmlBase::getXMLFirstElementFromFile(filename, tags::s2files);
}

void Xml::XmlS2Parser::parse()
{
    if (!m_content.isNull())
    {
        XmlParse::parseNode(m_content, tags::conf_tabs, [this](const QDomNode &tabNode) { parseConfigTab(tabNode); });
        XmlParse::parseNode(m_content, tags::records, [this](const QDomNode &recordNode) { parseRecord(recordNode); });
    }
}

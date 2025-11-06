#pragma once

#include "xml/xmlparser/xmlparser.h"

namespace Xml
{
/// \brief Class for parsing "s2files.xml" file.
class XmlS2Parser final : public XmlParser
{
    Q_OBJECT
public:
    explicit XmlS2Parser(QObject *parent = nullptr);
    /// \brief Парсинг всех нод <record> файла s2files.xml.
    void parse();

signals:

private:
    QDomElement m_content;
};

}

#ifndef MODULEXMLPARSER_H
#define MODULEXMLPARSER_H

#include "basexmlparser.h"

class ModuleXmlParser : public BaseXmlParser
{
    Q_OBJECT
public:
    explicit ModuleXmlParser(QObject *parent = nullptr);
};

#endif // MODULEXMLPARSER_H

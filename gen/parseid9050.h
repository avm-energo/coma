#ifndef PARSEID9050_H
#define PARSEID9050_H

#include "parsemodule.h"

class ParseID9050 : public ParseModule
{
public:
    ParseID9050(QByteArray &BA);

    int Parse(int &count);
};

#endif // PARSEID9050_H

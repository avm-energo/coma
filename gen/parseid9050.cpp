#include <QVector>
#include "../config/config.h"
#include "eoscillogram.h"
#include "parsemodule.h"
#include "parseid9050.h"
#include "../models/trendviewmodel.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../gen/colors.h"
#include "../dialogs/trendviewdialog.h"
#include "modulebsi.h"

ParseID9050::ParseID9050(QByteArray &BA) : ParseModule(BA)
{

}

int ParseID9050::Parse(int &count)
{

    return Error::ER_NOERROR;
}

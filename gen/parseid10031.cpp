#include <QVector>
#include "../config/config.h"
#include "eoscillogram.h"
#include "parsemodule.h"
#include "parseid10031.h"
#include "../models/trendviewmodel.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../gen/colors.h"
#include "../dialogs/trendviewdialog.h"
#include "modulebsi.h"

ParseID10031::ParseID10031(QByteArray &BA) : ParseModule(BA)
{

}

int ParseID10031::Parse(int &count)
{
    ParseID10031::SWJournalRecordStruct SWJ;
    PosPlusPlus(&SWJ, count, sizeof(SWJ));
    return Error::ER_NOERROR;
}

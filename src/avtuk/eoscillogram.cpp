#include "eoscillogram.h"

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"
#include "parseid10031.h"
#include "parseid9000.h"
#include "parseid9050.h"

#include <QDebug>

EOscillogram::EOscillogram(QObject *parent) : QObject(parent)
{
    PointsNum = 0;
}

EOscillogram::~EOscillogram()
{
}

void EOscillogram::SaveToExcel()
{
}

void EOscillogram::SaveToComtrade()
{
}
bool EOscillogram::PosPlusPlus(void *dst, int size)
{
    BSize = size;
    if (Pos > (BASize - BSize))
    {
        qCritical("pos > ba.size");
        return false;
    }
    memcpy(dst, &(BA.data()[Pos]), BSize);
    Pos += BSize;
    return true;
}

bool EOscillogram::ProcessOsc(TrendViewModel *mdl)
{
    Pos = 0;
    // разбираем осциллограмму
    S2DataTypes::FileHeader FH;

    BASize = (BA.size() + sizeof(FH));
    // EOscillogram::SWJournalRecordStruct SWJRecord;

    if (Pos > BASize)
        return false;
    memcpy(&FH, &(BA.data()[0]), sizeof(FH));

    Pos += sizeof(FH);
    S2DataTypes::DataRecHeader DR;
    if (Pos > BASize)
        return false;
    memcpy(&DR, &(BA.data()[Pos]), sizeof(DR));

    while (DR.id != 0xFFFFFFFF)
    {
        if (Pos >= BASize)
            break;

        std::unique_ptr<ParseModule> parseModule;
        Pos += sizeof(DR);
        switch (DR.id)
        {
        case MT_HEAD_ID:
            parseModule = std::unique_ptr<ParseModule>(new ParseID9000(BA));
            break;

            // #TODO find real module
            //        case AVTUK_87::OSC_SPC_ID:
            //            parseModule = std::unique_ptr<ParseModule>(new ParseID9050(BA));
            //            break;

        case AVTUK_85::SWJ_ID:
        {
            memcpy(&SWJRecord, &(BA.data()[Pos]), sizeof(S2DataTypes::SwitchJourRecord));
            if (DR.numByte == sizeof(S2DataTypes::SwitchJourRecord))
            {
                Pos += sizeof(S2DataTypes::SwitchJourRecord);
                parseModule = std::unique_ptr<ParseModule>(new ParseID10031(BA));
            }
            else
                return false;
            break;
        }
        default:
            return false;
        }
        if (!parseModule->Parse(Pos))
            return false;

        if (Pos > BASize)
        {
            qWarning() << Error::SizeError;
            return false;
        }
        memcpy(&DR, &(BA.data()[Pos]), sizeof(DR));
        Pos += sizeof(DR);
        *mdl = *(parseModule->trendViewModel());
    }

    return true;
}

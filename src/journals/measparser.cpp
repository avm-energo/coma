#include "measparser.h"

namespace journals
{

MeasParser::MeasParser(QTimeZone timeZone, QObject *parent) : QObject(parent), timezone(timeZone)
{
}

std::size_t MeasParser::getRecordSize(const MeasSettings &settings)
{
    using namespace ModuleTypes;
    std::size_t recSize = 0;
    for (auto &rec : settings)
    {
        switch (rec.type)
        {
        case BinaryType::float32:
            recSize += sizeof(float);
            break;
        case BinaryType::uint32:
            recSize += sizeof(quint32);
            break;
        case BinaryType::time32:
            recSize += sizeof(quint32);
            break;
        case BinaryType::time64:
            recSize += sizeof(quint64);
            break;
        }
    }
    return recSize;
}

int MeasParser::findTimeIndex(const MeasSettings &settings)
{
    using namespace ModuleTypes;
    int index = 0;
    for (auto i = 0; i < settings.size(); i++)
    {
        auto type = settings[i].type;
        if (type == BinaryType::time32 || type == BinaryType::time64)
        {
            index = i;
            break;
        }
    }
    return index;
}

void MeasParser::update(const QByteArray &ba, const MeasSettings &settings)
{
    timeIndex = findTimeIndex(settings);
    recordSize = getRecordSize(settings);
    record.clear();
    iter = reinterpret_cast<const quint8 *>(ba.constData());
    if (recordSize != 0)
    {
        size = ba.size() / recordSize;
        record.reserve(recordSize);
    }
}

bool MeasParser::parseRecord(const MeasSettings &settings)
{
    using namespace ModuleTypes;
    auto status = true;
    QVariant storage;
    for (auto &rec : settings)
    {
        switch (rec.type)
        {
        case BinaryType::float32:
        {
            // auto value = *reinterpret_cast<const float *>(iter);
            // iter += sizeof(float);
            // storage.setValue(value);
            storage = iterateValue<float>();
            break;
        }
        case BinaryType::uint32:
        {
            // auto value = *reinterpret_cast<const quint32 *>(iter);
            // iter += sizeof(quint32);
            // storage.setValue(value);
            storage = iterateValue<quint32>();
            break;
        }
        case BinaryType::time32:
        {
            // auto value = *reinterpret_cast<const quint32 *>(iter);
            // iter += sizeof(quint32);
            // if (value == UINT32_MAX)
            //    status = false;
            // auto time = TimeFunc::UnixTime32ToInvString(value, timezone);
            // storage.setValue(time);
            storage = iterateTime<quint32>(status);
            break;
        }
        case BinaryType::time64:
        {
            // auto value = *reinterpret_cast<const quint64 *>(iter);
            // iter += sizeof(quint64);
            // if (value == UINT64_MAX)
            //    status = false;
            // auto time = TimeFunc::UnixTime64ToInvStringFractional(value, timezone);
            // storage.setValue(time);
            storage = iterateTime<quint64>(status);
            break;
        }
        }

        if (rec.visibility)
            record.append(storage);
    }
    return status;
}

bool MeasParser::sortByTime(const QVector<QVariant> &lhs, const QVector<QVariant> &rhs)
{
    auto lStrTime = lhs[timeIndex].value<QString>();
    auto rStrTime = rhs[timeIndex].value<QString>();
    // auto lTime = TimeFunc::InvStringToUnixTime32(lStrTime, timezone);
    // auto rTime = TimeFunc::InvStringToUnixTime32(rStrTime, timezone);
    return lStrTime > rStrTime;
}

const Data MeasParser::parse(const MeasSettings &settings)
{
    Data retVal;
    retVal.reserve(size);

    for (auto i = 0; i < size; i++)
    {
        if (parseRecord(settings))
            retVal.push_back(record);
        record.clear();
    }

    if (timeIndex != 0)
        std::sort(retVal.begin(), retVal.end(),
            [this](const QVector<QVariant> &lhs, const QVector<QVariant> &rhs) { return sortByTime(lhs, rhs); });

    return retVal;
}

}

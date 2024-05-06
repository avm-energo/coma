#include "journals/measparser.h"

namespace journals
{

using namespace Device::XmlDataTypes;
constexpr auto notFound = -1;

MeasParser::MeasParser(QTimeZone timeZone, QObject *parent)
    : QObject(parent), m_timezone(timeZone), m_timeIndex(notFound)
{
}

std::size_t MeasParser::getRecordSize(const MeasSettings &settings)
{
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
        default:
            break;
        }
    }
    return recSize;
}

int MeasParser::findTimeIndex(const MeasSettings &settings)
{
    int index = notFound;
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
    m_timeIndex = findTimeIndex(settings);
    m_recordSize = getRecordSize(settings);
    m_record.clear();
    m_iter = reinterpret_cast<const quint8 *>(ba.constData());
    if (m_recordSize != 0)
    {
        m_totalSize = ba.size() / m_recordSize;
        m_record.reserve(m_recordSize);
    }
}

bool MeasParser::parseRecord(const MeasSettings &settings)
{
    auto status = true;
    QVariant storage;
    for (auto &rec : settings)
    {
        switch (rec.type)
        {
        case BinaryType::float32:
            storage = iterateValue<float>();
            break;
        case BinaryType::uint32:
            storage = iterateValue<quint32>();
            break;
        case BinaryType::time32:
            storage = iterateTime<quint32>(status);
            break;
        case BinaryType::time64:
            storage = iterateTime<quint64>(status);
            break;
        default:
            break;
        }
        if (rec.visibility)
            m_record.append(storage);
    }
    return status;
}

bool MeasParser::sortByTime(const QVector<QVariant> &lhs, const QVector<QVariant> &rhs)
{
    auto lStrTime = lhs[m_timeIndex].value<QString>();
    auto rStrTime = rhs[m_timeIndex].value<QString>();
    return lStrTime > rStrTime;
}

const JournalData MeasParser::parse(const MeasSettings &settings)
{
    JournalData retVal;
    retVal.reserve(m_totalSize);

    for (auto i = 0; i < m_totalSize; i++)
    {
        if (parseRecord(settings))
            retVal.push_back(m_record);
        m_record.clear();
    }

    if (m_timeIndex != notFound)
        std::sort(retVal.begin(), retVal.end(),
            [this](const QVector<QVariant> &lhs, const QVector<QVariant> &rhs) { return sortByTime(lhs, rhs); });

    return retVal;
}

}

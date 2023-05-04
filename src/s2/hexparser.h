#pragma once

#include <QByteArray>
#include <QObject>

/// \brief Enumeration to represent the HEX record types.
/// \see https://en.wikipedia.org/wiki/Intel_HEX
enum RecordType : quint8
{
    BinaryData = 0x00,       ///< Binary Data.
    EndOfFile = 0x01,        ///< End Of File.
    SegmentAddr = 0x02,      ///< Extended Segment Address.
    StartSegmentAddr = 0x03, ///< Start Segment Address.
    LinearAddr = 0x04,       ///< Extended Linear Address.
    StartLinearAddr = 0x05   ///< Start Linear Address.
};

/// \brief A structure for representing a HEX record from a HEX file.
struct HexRecord
{
    quint8 dataLength;     ///< Byte count of the data.
    quint16 address;       ///< Address for the HEX record.
    RecordType recordType; ///< HEX record type.
    quint8 checksum;       ///< Checksum of the HEX record.
    QByteArray data;       ///< Data unit of the record (may be null).
};

enum HexParseError : quint8
{
    InvalidHexRecord = 0,
    IncorrectChecksum
};

constexpr quint8 startCode = ':';

class HexParser : public QObject
{
    Q_OBJECT
private:
    bool parseASCII(const QString &strRecord);

public:
    explicit HexParser(QObject *parent = nullptr);

    void parseFile(const QByteArray &hexFile);
};

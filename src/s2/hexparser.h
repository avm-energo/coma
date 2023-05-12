#pragma once

#include "filestruct.h"

#include <QObject>
#include <vector>

namespace S2Dev
{

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

constexpr quint8 startCode = ':';                    ///< HEX record start code.
constexpr auto minRecordSize = 5;                    ///< Min HEX record size.
constexpr auto maxRecordSize = 0xff + minRecordSize; ///< Max HEX record size.
constexpr quint16 idNotFound = 0xffff;               ///< ID for the invalid block address.

/// \brief Enumeration to represent the error type of the HEX parser.
enum HexParseError : quint8
{
    InvalidHexRecord = 0, ///< An invalid HEX record in the binary file.
    InvalidRecordSize,    ///< Invalid size of the HEX record.
    IncorrectChecksum,    ///< Invalid checksum of the HEX record.
    InvalidBlockAddress   ///< Invalid block address of the HEX linear address record.
};

/// \brief The HEX parser for parsing given binary file.
class HexParser final : public QObject
{
    Q_OBJECT
private:
    std::vector<HexRecord> records;

    /// \brief Verifying the checksum of the HEX record.
    /// \details A record's checksum byte is the two's complement of the least significant byte (LSB)
    /// of the sum of all decoded byte values in the record preceding the checksum. It is computed by
    /// summing the decoded byte values and extracting the LSB of the sum (i.e., the data checksum),
    /// and then calculating the two's complement of the LSB (e.g., by inverting its bits and adding one).
    /// \param data[in] - pointer to the binary HEX record's data.
    /// \param size[in] - size of the HEX record in bytes.
    /// \returns True if checksum of the HEX record is correct, otherwise false.
    /// \see https://en.wikipedia.org/wiki/Intel_HEX
    bool verifyChecksum(const quint8 *data, int size);

    /// \brief Parsing the given HEX record from ASCII-string representation to the HexRecord structure.
    /// \param strRecord[in] - the given ASCII-string representation of the HEX record.
    /// \param record[out] - the output HexRecord structure.
    /// \returns True if the HEX record parsing is successful, otherwise false.
    bool parseASCII(const QString &strRecord, HexRecord &record);

    /// \brief Getting ID for the given linear address of the HEX section.
    quint16 getIdByAddress(const QByteArray &binAddr);

public:
    explicit HexParser(QObject *parent = nullptr);

    /// \brief Initializing parser.
    /// \details A given binary file contains HEX record ASCII-string representations. In runtime this function
    /// The given binary file contains ASCII-string representations of HEX records. During
    /// the execution of this function, the correctness of the given file is checked, the
    /// conversion of string representations of HEX records to the HexRecord structures and
    /// their verification. HEX records are stored in the 'records' vector.
    void parseFile(const QByteArray &binaryFile);

    /// \brief Returns S2 format of the stored HEX records.
    std::vector<DataTypes::FileStruct> getS2Format();

signals:
    /// \brief The signal for the parsing error information.
    void error(S2Dev::HexParseError error);
};

}

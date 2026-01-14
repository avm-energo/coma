#pragma once

#include <QDebug>
#include <array>
#include <vector>

namespace S2
{
constexpr quint32 dummyElement = 0xffffffff;

using BYTE = std::uint8_t;
using WORD = std::uint16_t;
using DWORD = std::uint32_t;
using INT32 = std::int32_t;
using BYTE_3t = std::array<BYTE, 3>;
using BYTE_4t = std::array<BYTE, 4>;
using WORD_3t = std::array<WORD, 3>;
using WORD_4t = std::array<WORD, 4>;
using DWORD_4t = std::array<DWORD, 4>;
using BYTE_6t = std::array<BYTE, 6>;
using WORD_6t = std::array<WORD, 6>;
using DWORD_6t = std::array<DWORD, 6>;
using BYTE_8t = std::array<BYTE, 8>;
using WORD_8t = std::array<WORD, 8>;
using DWORD_8t = std::array<DWORD, 8>;
using BYTE_16t = std::array<BYTE, 16>;
using WORD_16t = std::array<WORD, 16>;
using DWORD_16t = std::array<DWORD, 16>;
using BYTE_32t = std::array<BYTE, 32>;
using WORD_32t = std::array<WORD, 32>;
using DWORD_32t = std::array<DWORD, 32>;
using FLOAT = float;
using FLOAT_2t = std::array<FLOAT, 2>;
using FLOAT_3t = std::array<FLOAT, 3>;
using FLOAT_4t = std::array<FLOAT, 4>;
using FLOAT_6t = std::array<FLOAT, 6>;
using FLOAT_8t = std::array<FLOAT, 8>;

static_assert(sizeof(BYTE) != sizeof(WORD), "Broken datatypes");
static_assert(sizeof(BYTE) != sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(INT32) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(FLOAT) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(WORD_3t) == sizeof(BYTE_6t), "Broken datatypes");
static_assert(sizeof(WORD_4t) == sizeof(BYTE_8t), "Broken datatypes");
static_assert(sizeof(DWORD_4t) == sizeof(BYTE_16t), "Broken datatypes");
static_assert(sizeof(FLOAT_2t) == sizeof(BYTE_8t), "Broken datatypes");

using valueType = std::variant<BYTE, WORD, DWORD, INT32, //
    BYTE_3t, WORD_3t,                                    //
    BYTE_4t, WORD_4t, DWORD_4t,                          //
    BYTE_6t, WORD_6t, DWORD_6t,                          //
    BYTE_8t, WORD_8t, DWORD_8t,                          //
    BYTE_16t, WORD_16t, DWORD_16t,                       //
    BYTE_32t, WORD_32t, DWORD_32t,                       //
    FLOAT, FLOAT_2t, FLOAT_3t, FLOAT_4t, FLOAT_6t, FLOAT_8t>;

struct DataRecHeader
{
    quint32 id;      // ID
    quint32 numByte; // количество байт в TypeTheData
};

struct DataRec
{
    DataRecHeader header;
    void *thedata;

    friend inline bool operator==(const DataRec &lhs, const DataRec &rhs);
    friend inline bool operator!=(const DataRec &lhs, const DataRec &rhs);
};

inline bool operator==(const DataRec &lhs, const DataRec &rhs)
{
    bool is_same_value = false;
    if ((lhs.header.id == rhs.header.id) && (lhs.header.numByte == rhs.header.numByte))
        is_same_value = !memcmp(lhs.thedata, rhs.thedata, lhs.header.numByte);
    return is_same_value;
}

inline bool operator!=(const DataRec &lhs, const DataRec &rhs)
{
    return !(lhs == rhs);
}

using S2ConfigType = std::vector<DataRec>;

/// S2: Определение типа заголовка
/// Заголовок файла (Прил. 1)
struct S2FileHeader
{
    quint16 fname;
    quint16 service;
    quint32 size;
    quint32 crc32;
    quint32 thetime;
};

/// \brief S2B File header format.
struct S2BFileHeader
{
    quint16 fname;
    quint16 types2b;
    quint32 size;
    quint16 typeB;
    quint16 typeM;
    quint32 thetime;
};

/// \brief S2B File tail format.
struct S2BFileTail
{
    quint32 crc32;
    quint32 serialnum;
    quint32 reserved;
    quint32 end;
};

/// \brief S2B Binary file format.
struct S2BFile
{
    S2BFileHeader header;
    QByteArray data;
    S2BFileTail tail;
};

// struct DataRecSwitchJour
//{
//    DataRecHeader header;
//    SwitchJourRecord swjRecord;
//};

struct OscHeader
{
    quint64 time; // время первой точки в Unix-формате
    float step;   // шаг по времени в мс
    quint32 len;  // длина осциллограммы в количестве точек по времени
};

}

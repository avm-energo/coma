#include "filestruct.h"

#include <QDebug>

namespace DataTypes
{

FileStruct::FileStruct(const FilesEnum num, const QByteArray &file) : ID(num), data(file)
{
}

FileStruct::FileStruct(const quint16 num, const QByteArray &file) : ID(num), data(file)
{
}

// FileStruct::FileStruct(const quint16 num) : ID(num)
//{
//}

S2DataTypes::DataRec FileStruct::serialize() const
{
    auto constDataPtr = static_cast<const void *>(data.data());
    auto dataPtr = const_cast<void *>(constDataPtr);
    return { { ID, quint32(data.size()) }, dataPtr };
}

QDataStream &operator<<(QDataStream &stream, const FileStruct &str)
{
#if QT_VERSION >= 0x050C00
    stream << str.ID;
#else
    stream << std::underlying_type_t<FilesEnum>(str.ID);
#endif
    stream << str.data;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, FileStruct &str)
{
#if QT_VERSION >= 0x050C00
    stream >> str.ID;
#else
    stream >> *reinterpret_cast<std::underlying_type_t<FilesEnum> *>(&str.ID);
#endif
    stream >> str.data;
    return stream;
}

QDebug operator<<(QDebug debug, const DataTypes::FileStruct &st)
{
    debug.nospace() << QString::number(st.ID) << ":"
                    << "File size: " << QString::number(st.data.size());
    return debug.maybeSpace();
}

}

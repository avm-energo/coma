#include "filestruct.h"

#include "s2datatypes.h"

#include <QDebug>

namespace S2DataTypes
{

FileStruct::FileStruct(const FilesEnum num, const QByteArray &file) : ID(quint16(num)), data(file)
{
}

FileStruct::FileStruct(const quint32 num, const QByteArray &file) : ID(num), data(file)
{
}

DataRec FileStruct::serialize() const
{
    auto constDataPtr = static_cast<const void *>(data.data());
    auto dataPtr = const_cast<void *>(constDataPtr);
    return { { ID, quint32(data.size()) }, dataPtr };
}

QDataStream &operator<<(QDataStream &stream, const FileStruct &str)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    stream << str.ID;
#else
    stream << std::underlying_type_t<FilesEnum>(str.ID);
#endif
    stream << str.data;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, FileStruct &str)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    stream >> str.ID;
#else
    stream >> *reinterpret_cast<std::underlying_type_t<FilesEnum> *>(&str.ID);
#endif
    stream >> str.data;
    return stream;
}

QDebug operator<<(QDebug debug, const FileStruct &st)
{
    debug.nospace() << QString::number(st.ID) << ":"
                    << "File size: " << QString::number(st.data.size());
    return debug.maybeSpace();
}

}

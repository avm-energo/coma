#pragma once

#include <QDataStream>
#include <QVariant>

namespace S2
{

struct DataRec;

/*! Приложение 3. Номера файлов */
enum class FilesEnum : quint16
{
    Config = 1,        ///< Конфигурация
    Firmware = 3,      ///< Встроенное ПО (Firmware)
    JourSys = 4,       ///< Системный журнал
    JourWork = 5,      ///< Рабочий журнал
    JourMeas = 6,      ///< Журнал измерений
    JourSw = 17,       ///< Журнал переключений
    JourEv = 18,       ///< Events journal (12->62)
    FileOscMin = 1000, ///< Oscillogramm ID min
    FileOscMax = 2999  ///< Oscillogram ID max
};

struct FileStruct
{
    quint32 ID;
    QByteArray data;

    FileStruct() = default;
    FileStruct(const FilesEnum num, const QByteArray &file);
    FileStruct(const quint32 num, const QByteArray &file);
    DataRec serialize() const;

    friend QDataStream &operator<<(QDataStream &stream, const FileStruct &str);
    friend QDataStream &operator>>(QDataStream &stream, FileStruct &str);
    friend QDebug operator<<(QDebug debug, const FileStruct &st);
};

QDataStream &operator<<(QDataStream &stream, const FileStruct &str);
QDataStream &operator>>(QDataStream &stream, FileStruct &str);
QDebug operator<<(QDebug debug, const FileStruct &st);

}

Q_DECLARE_METATYPE(S2::FilesEnum)
Q_DECLARE_METATYPE(S2::FileStruct)

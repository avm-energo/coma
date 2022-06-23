#ifndef FILESTRUCT_H
#define FILESTRUCT_H

#include "../gen/datamanager/datatypeimpl.h"
#include "datarec.h"

#include <QDataStream>
#include <QVariant>

namespace DataTypes
{

/*! Приложение 3. Номера файлов */
enum FilesEnum : quint16
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

class FileStruct : public DataTypeImpl<FileStruct>
{
    Q_OBJECT
public:
    std::underlying_type_t<FilesEnum> ID;
    QByteArray data;

    FileStruct();
    FileStruct(const FilesEnum num, const QByteArray &file);
    FileStruct(const quint8 num, const QByteArray &file);
    FileStruct(const quint8 num);
    S2DataTypes::DataRec serialize();

    friend QDataStream &operator<<(QDataStream &stream, const FileStruct &str);
    friend QDataStream &operator>>(QDataStream &stream, FileStruct &str);
    friend QDebug operator<<(QDebug debug, const DataTypes::FileStruct &st);

signals:
    void DataTypeReceived(const FileStruct &);
};

}

Q_DECLARE_METATYPE(DataTypes::FileStruct)
Q_DECLARE_METATYPE(DataTypes::FilesEnum)

#endif // FILESTRUCT_H

#pragma once

#include <gen/datatypes.h>
#include <gen/error.h>
#include <s2/dataitem.h>

namespace S2
{
// Forward declarations
class ConfigStorage;
class Configuration;
class DataFactory;

/// \brief Структура с данными для эмуляции формата S2B.
struct EmulateData
{
    quint32 m_typeB = 0;
    quint32 m_typeM = 0;
    quint32 m_serialnum = 0;

    bool isEmpty() const noexcept
    {
        return ((m_typeB == 0) && (m_typeM == 0) && (m_serialnum == 0));
    }
};

/// \brief Класс, предоставляющий интерфейс для работы с массивами
/// данных в формате S2, конвертированию их из одного формата в другие.
class Util
{
public:
    Util() = default;

    /// \brief Конвертирует элемент конфигурации в формате S2 в массив байт.
    [[nodiscard]] QByteArray convert(const quint32 id, const DataItem &item) const;
    /// \brief Конвертирует конфигурацию в формате S2 в массив байт.
    [[nodiscard]] QByteArray convert(const Configuration &config, quint32 fileType) const;
    /// \brief Конвертирует массив байт в конфигурацию в формате S2.
    [[nodiscard]] Error::Msg convert(const QByteArray &rawFile, const DataFactory &factory, //
        std::map<quint32, DataItem> &result) const;

    /// \brief Возвращает размер файла, полученного в формате S2.
    [[nodiscard]] std::size_t getFileSize(const QByteArray &s2file) const;
    /// \brief Возвращает массив байт в формате S2B.
    [[nodiscard]] S2BFile emulateS2B(const QByteArray &data, quint16 fname, const EmulateData &emulData) const;
    /// \brief Функция для парсинга массива байт в файл формата S2B.
    [[nodiscard]] Error::Msg parseS2B(const QByteArray &file, S2BFile &result) const;

    // deprecated part
    static void StoreDataMem(QByteArray &mem, const std::vector<FileStruct> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const std::vector<DataRec> &dr, int fname);
    static bool RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist);
    //[[deprecated]] static quint16 GetIdByName(const QString &name);

private:
    static quint32 getTime32() noexcept;
};

} // namespace S2

using S2Util = S2::Util;
using S2BEmulateData = S2::EmulateData;
